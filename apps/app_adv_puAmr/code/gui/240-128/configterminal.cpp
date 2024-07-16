/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
  ��������: configcontrol.cpp
  ��    �ܣ�����ն���������ҳ���
  �� �� �ˣ�gaoyf
  ����ʱ�䣺2007-1

  �汾��ʷ:

    v1.0:
      2006-1-20, gaoyf : New file

  ����ҳ��⣺
    �ն˵�ַ�����������룩  0x00630000
    �ն˵�ǰ����ͨ����Ϣ    0x00660000


***************************************************************************
*/

#include <sys/stat.h>
#include "miniguidef.h"
#include "commdef.h"
#include "main.h"
#include "commfun.h"
#include "dataType.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "out_msg.h"
#include "pageInterface.h"
#include "framecomm.h"
#include "adc.h"

#include "iniparser.h"
#ifdef __cplusplus
extern "C"
{
#endif

    extern FACTORY_VER_TMP_T g_tVer;

    extern int systime_set_to_smiOS(DateTimeHex_t* pDateTime);
    extern int param_init_sent_to_acmeter(void);
    extern int data_init_sent_to_acmeter(void);
    extern uint8 appctrl_get_devturn(uint8* pTurnmax);
    extern int reboot_sent_to_smiOS(void);
    extern uint8 pulse_get_rmtcfg();
    extern void app_setdefault_cfg_area(int sockdbfd);
    extern void gui_mannual_amr_send(uint8 *apdu, uint16 len); 
    extern uint16 app_crc16_get(const uint8* buf, uint32 len);
    extern int get_meterfile(uint16 infonum, OOP_METER_T* MeterFile);
    extern int statis_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);
    extern int statis_recover_default(DB_CLIENT fd, uint8 logicId);
    extern int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw);

#ifdef __cplusplus
}
#endif


#define MAX_METER_INFONUM   3600   //�����������Ϣ���
#define MAX_TASK_INFONUM 1000       //���������Ϣ���
#define MAX_PLAN_INFONUM 1000       //���������Ϣ���
#define MAX_SHAANXI_SELECT 9        //����Ĭ��9������ͨѶ����

const OOP_OAD_U MeterFileOad = { 0x60000200 };
const OOP_OAD_U NormalPlanOad = { 0x60140200 };
const OOP_OAD_U TaskOad = { 0x60120200 };
const OOP_OAD_U EthIPOad = { 0x45100400 };
const OOP_OAD_U EthConfigOad = { 0x45100200 };
const OOP_OAD_U EthMasterOad = { 0x45100300 };
//const OOP_OAD_U GPRSConfigOad = { 0x45000200 };
//const OOP_OAD_U GPRSMasterOad = { 0x45000300 };
const OOP_OAD_U SecuConfigOad = { 0xF1010200 };
const char* g_pczRadioDelay[] = { "100", "200", "300", "400", "500", "800", "1000", "2000" };
//�������
const char* g_baud[] = { "300bps","600bps","1200bps","2400bps","4800bps","7200bps","9600bps","19200bps","38400bps","57600bps","115200bps","����Ӧ" };

//��Լ����
const char* g_prtl[] = { "δ֪","DL/T 645-1997","DL/T 645��2007","DL/T 698.45","CJ/T 188��2004" };

//TI��ʽʱ����
const char* g_ti[] = { "��","��","ʱ","��","��","��" };

//��������
const char* g_plantype[] = { "��ͨ�ɼ�����","�¼��ɼ�����","͸������","�ϱ�����","�ű�����" };

//��ͨ�ɼ�����
const char* g_normalPlanType[] = {"�ɼ���ǰ����", "�ɼ��ϵ�N��", "������ʱ��ɼ�", "��ʱ�����ɼ�", "����"};

//MS����
const char* g_ms[] = {"�ޱ��", "ȫ���û���ַ", "һ���û�����", "һ���û���ַ", "һ���������", "һ���û���������", "һ���û���ַ����", "һ�������������"};

//�洢ʱ��
const char* g_tmFlag[] = {"δ����", "����ʼʱ��", "��Ե���0:0", "�������23:59", "�������0:0", "��Ե���1��0:0", "���ݶ���ʱ��", "���������ĩ23:59"};

//��������
const char* g_regionType[] = {"ǰ�պ�", "ǰ�����", "ǰ�պ��", "ǰ����"};

//�¼��ɼ�����
const char* g_eventPlanType[] = {"���ڲɼ�", "����֪ͨ�ɼ�����", "����֪ͨ�ɼ�ָ��", "����֪ͨ�洢"};

//�ϱ���ʶ
const char* g_eventReportFlag[] = {"���ϱ�", "�����ϱ�"};

//�ϱ���������
const char* g_reportPlanType[] = {"��������", "��¼�Ͷ�������"};

//�ϱ�����RSD
const char* g_reportPlanRSD[] = {"��ѡ��", "ѡ�񷽷�1",  "ѡ�񷽷�2",  "ѡ�񷽷�3",  "ѡ�񷽷�4",  "ѡ�񷽷�5",  "ѡ�񷽷�6",
                                 "ѡ�񷽷�7",  "ѡ�񷽷�8",  "ѡ�񷽷�9",  "ѡ�񷽷�10"};

//�˿�
const char* g_pipe[] = { "�ز�-1","�ز�-2","485-1","485-2","485-3","485-4","485-5","485-6","485-7","485-8","����" };

//����
const char* g_on_off[] = { "�ر�","����" };

//ip���÷�ʽ
const char* g_ipconfig[] = { "DHCP","��̬","PPPoE" };

//ͨ�����ù���ģʽ
const char* g_work_mode[] = { "���ģʽ","�ͻ���ģʽ","������ģʽ" };

//��Ӫ�̴���
const char* g_operator_seclct[] = { "�й��ƶ�","�й�����","�й���ͨ","δ֪" };

//��������
const char* g_net_mode[] = { "2G","3G","4G","5G","δ֪"};

//��Ȩ��ʽ
const char* g_auth_mode[] = { "null","chap","pap","pap&chap"};

//���ӷ�ʽ
const char* g_conn_type[] = { "TCP","UDP" };

//����Ӧ�÷�ʽ
const char* g_conn_app_type[] = { "����ģʽ","������ģʽ" };

//���߷�ʽ
const char* g_line_type[] = { "��������","��������" };

//�ز�Ƶ��ѡ�����û��߲�ѯ
const char* g_Hplc_type[] = { "�����ز�Ƶ��","��ѯ�ز�Ƶ��" };

const char*  g_sensor_name_value[]={"��ʪ�ȴ�����","ˮ��������","��������","׮ͷ�¶ȴ�����"};

#ifdef AREA_SHAANXI
const char* g_SHANXI_uplink_select[]={"�ƶ�������(����)","�ƶ�������(������)","��ͨ������(����)","��ͨ������(������)","����������(����)","����������(������)","�ƶ���������(����)","��ͨ��������(����)","���ŷ�������(����)"};
#endif
OOP_OAD_U GPRSConfigOad;
OOP_OAD_U GPRSMasterOad;

int RebootTerminal();
OOP_OAD_U get_set_pipe(int index);

void MenuDraw_add_name(HDC hdc, const char* pname);
void hplcAmrMakeFrame(uint8 *apdu, uint8 *len,uint8 flag);

void MenuDraw(HDC hdc)
{
    MenuDraw_add_name(hdc, ADV_SHOW_NAME);
}

BOOL MenuKeyMsg(int nKey);
void InitMenu(DWORD dwPluginId);
PLUGIN_INFO g_menuLayer = {
    0x00A00000/*id*/,
    InitMenu/*InitLayer*/,
    NULL/*UninitLayer*/,
    MenuDraw/*Draw*/,
    NULL/*Timer*/,
    MenuKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

#include "db_api.h"
#include "app_gui.h"
#include "oopVarUpdate.h"

extern APP_MODE_INFO g_APPModeInfoFunc;
extern DB_CLIENT   g_gui_db_client;
const char* g_showTitleVoltag34[] = { "Ua", "Ub", "Uc" };
const char* g_showTitleEelectric[] = { "Ia", "Ib", "Ic" };
const char* g_showTitleJCPower_Y[] = { "�����й�����", "A���й�����", "B���й�����","C���й�����" };
const char* g_showTitleJCPower_W[] = { "�����޹�����", "A���޹�����", "B���޹�����","C���޹�����" };
const char* g_showTitleJCPowerFactor[] = { "�ܹ������� ", "A�๦������", "B�๦������", "C�๦������" };
const char* g_showSwitchState[] = { "��", "��" };
const char* g_showSwitchFlag[] = { "δ����", "����" };
const char* g_showCtrlState[] = { "��", "��" };
const char* g_showtaskStatus[] = { "δִ��", "ִ����", "��ִ��" };
//�ӿ�����
const char* interfaceType[] =
{
    "CDC-ACM",
    "CDC-ECM",
    "HID",
};
//��������
const char* pipeFunc[] =
{
    "����",
    "Զ��",
    "PLC/RF",
    "RS-485",
    "ң��",
    "CAN",
    "M-Bus",
    "����",
    "ģ����",
    "RS232",
};

const char* g_pczPulseAttr[] = { "�����й�", "�����޹�", "�����й�", "�����޹�" };
const char* g_pczDirect[] = { " -", "��", "��" };
const char* g_pczOperator[] = { " -", "��", "��" };

typedef struct
{
    int32            plus_active_power;         //�����й����� 0x24010500
    int32            plus_reactive_power;       //�����޹����� 0x24010600
    int64            gruop_active_power;        //�ܼ����й����� 0x23010300
    int64            gruop_reactive_power;      //�ܼ����޹����� 0x23010400
    OOP_HENERGYS_T   gruop_day_energy;        //�ܼ������й����� 0x23010700
    OOP_HENERGYS_T   gruop_day_ractenergy;    //�ܼ������޹����� 0x23010800
    OOP_HENERGYS_T   gruop_month_energy;      //�ܼ������й����� 0x23010900
    OOP_HENERGYS_T   gruop_month_ractenergy;  //�ܼ������޹����� 0x23010a00
    OOP_BITSTR8_T    ctrlPwTurn;              //0x230x0e00 �ܼ��鹦���ִ�����  ���ƶ�ȡ
    OOP_BITSTR8_T    ctrlEcTurn;              //0x230x0f00 �ܼ������ִ�����  ���ƶ�ȡ
    OOP_TGSETTING_T  ctrl_seting;             //��������״̬         0x23011000
    OOP_TGCURSTATE_T ctrl_status;             //���Ƶ�ǰ״̬         0x23011100
    OOP_PULSESETS_T  plus_param;              //��������       0x24010400
    OOP_PULSESETS_B_T plus_b_param;           //��������       0x24011C00
    OOP_OCTETVAR16_T pulse_addr;              //�����ַ       0x24010200
    OOP_TGSETS_T     gruop_param;             //��������       0x23010200
}CTRL_DATA_INFO;

CTRL_DATA_INFO g_ctrl_data[8];
uint8            eletype;                 //����
OOP_8107_CONFIG_T tBuyenergyconfig;
uint8            g_bGuiBuyLastData = 0;
int64            g_GuiBuyLast = 0;
uint8            g_bGuiBuyEndData = 0;
int64            g_GuiBuyEnd = 0;
int64      g_wallet[8];

DISPLAY_DATA_INFO display_data;
DISPLAY_DATA_INFO display_datas;
DISPLAY_DAY_FROZEN_INFO display_day_frozen;

HIGH_PRECISION_DATA_INFO g_high_precision_data;


MOD_INFO_ACK_T g_module_info[5];
void g_turnstart_init(DWORD dwPluginId);
void g_turnstart_draw(HDC hdc);

void g_safemode_init(DWORD dwPluginId);//��ʼ����ȫģʽ
void g_safemode_draw(HDC hdc);//���ư�ȫģʽ����
BOOL g_safemode_keymsg(int nKey);//��ȫģʽ�����¼�

//״̬�� ��Ϣ��Ŵ���
void Uninit()
{
    advance_gui_infonum(0);
}
    
#if DESC("�������Ķ�д",1)
int write_normal_data(uint8* DataBuf, uint16 DataLen, OOP_OAD_U Oad, uint16 infonum, uint8 logicid, uint8 classtag)
{
    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = logicid;
    NomalOad.oad = Oad;
    NomalOad.classtag = classtag;
    NomalOad.infoNum = infonum;

    ret = db_write_nomal(g_gui_db_client, &NomalOad, DataBuf, DataLen);
    if (ret != 0)
    {
        GUI_FMT_DEBUG("��ͨ����д�����ݿ�ʧ��oad %08x  \n", Oad.value);
    }
    return ret;
}
int read_db_noamal_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int ret = 0;
    uint32 len = 0;
    NOMAL_OAD_T nomalOAD = { 0 };

    nomalOAD.logicId = logic_id;
    nomalOAD.infoNum = info_num;
    nomalOAD.oad.value = oad_value;
    ret = db_read_nomal(fp, &nomalOAD, buf_len, pbuf, &len);
    if (ret != ERR_OK || len != buf_len)
    {
        GUI_FMT_DEBUG("db_read_nomal  err ret=%d, recNum=%d, buf_len=%u!\n", ret, len, buf_len);
        return -1;
    }

    return 0;
}

int read_db_record_data(DB_CLIENT fp, uint32 oad_value, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep = 0;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.cType = COL_NO_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", oad_value, ret);
        return -1;
    }
    memcpy(pbuf, stepRecordOut.record[0].outDataBuf, buf_len);

    return 0;
}
#endif
    
#if DESC("���������Զ�����",1)
extern OOP_SWITCHIN_STATE_T    switchState;    //ң��״̬
extern OOP_SWITCHIN_FLAG_T     switchFlag;     //�����־

//���������Զ�����
typedef struct 
{
    uint32  oadValue;
    uint32  dateLen;
    uint8   *pBuf;
    uint8   *pvaild;
}AcDataInfo;

//���������Զ������б�
AcDataInfo g_ac_data_list[] = 
{/*     ���ݱ�ʶ,   ���ݳ���,     �������� */
    {0x00100200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.penergy},
    {0x00300200,    sizeof(OOP_ENERGYS_T),                 (uint8 *)&display_data.penergys},
    {0x00200200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.nenergy},
    {0x00400200,    sizeof(OOP_ENERGYS_T),                 (uint8 *)&display_data.nenergys},
    {0x00500200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant1},
    {0x00600200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant2},
    {0x00700200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant3},
    {0x00800200,    sizeof(OOP_ENERGY_T),                  (uint8 *)&display_data.quadrant4},
    {0x20000200,    sizeof(OOP_WORD3V_T),                  (uint8 *)&display_data.vol},
    {0x20010200,    sizeof(OOP_INT3V_T),                   (uint8 *)&display_data.cur},
    {0x20010400,    sizeof(int32),                   	   (uint8 *)&display_data.zeroCur},
    {0x20040200,    sizeof(OOP_INT4V_T),                   (uint8 *)&display_data.ppower},
    {0x20050200,    sizeof(OOP_INT4V_T),                   (uint8 *)&display_data.npower},
    {0x200A0200,    sizeof(OOP_LONG4V_T),                  (uint8 *)&display_data.fpower},
    {0x10100200,    sizeof(OOP_DEMAND_T),                  (uint8 *)&display_data.pdemand},
    {0x10200200,    sizeof(OOP_DEMAND_T),                  (uint8 *)&display_data.ndemand},
    {0xf2030200,    sizeof(OOP_SWITCHIN_STATE_T),          (uint8 *)&switchState},
    {0xf2030400,    sizeof(OOP_SWITCHIN_FLAG_T),           (uint8 *)&switchFlag},

    {0x00100400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.penergy},
    {0x00300400,    sizeof(OOP_HENERGYS_T),                 (uint8 *)&g_high_precision_data.penergys},
    {0x00200400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.nenergy},
    {0x00400400,    sizeof(OOP_HENERGYS_T),                 (uint8 *)&g_high_precision_data.nenergys},
    {0x00500400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant1},
    {0x00600400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant2},
    {0x00700400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant3},
    {0x00800400,    sizeof(OOP_HENERGY_T),                  (uint8 *)&g_high_precision_data.quadrant4},
};

uint16 g_ac_data_num = sizeof(g_ac_data_list)/sizeof(AcDataInfo);


/**********************************************************************
* @name      : update_ac_data
* @brief     : ���½�������
* @param[in] ��int clientid    socket���
               uint8 logicId   �߼���ַ
               uint16 infoNum  ��Ϣ���
               uint32 value    oadֵ
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-07-31
* @Update    :
**********************************************************************/
int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int                i = 0;
    int                ret = 0;
    uint32             readlen = 0;
    uint8              pDataBuf[1024] = {0};
    NOMAL_OAD_T        nomalOAD;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicId;
    nomalOAD.infoNum = infoNum;
    nomalOAD.oad.value = value;

    for(i=0; i<g_ac_data_num; i++)
    {
        if(g_ac_data_list[i].oadValue == value)
        {
            ret = db_read_nomal(clientid, &nomalOAD, 1024, pDataBuf, &readlen);
            if (ret != ERR_OK || readlen == 0 || (g_ac_data_list[i].dateLen != readlen))
            {
                GUI_FMT_DEBUG("Updata logicId=%u, infoNum=%u, OAD=%08X  error ret=%d, len=%u, dateLen=%u\n", logicId, infoNum, value, ret, readlen, g_ac_data_list[i].dateLen);
                return ret;
            }
            
            memcpy(g_ac_data_list[i].pBuf, pDataBuf, readlen);
            GUI_FMT_TRACE("Updata Nomal logicId=%u, infoNum=%u, OAD=%08X  OK !\n", logicId, infoNum, value);
            GUI_BUF_TRACE(pDataBuf, readlen, "buf:");
            return 0;
        }
        
    }
    
    return -1;
}
#endif
    
#if DESC("�ִ�״̬",1)
void g_turnstart_init(DWORD dwPluginId)
{
    memset(&display_data.switchout, 0, sizeof(OOP_SWITCHOUT_T));

    read_db_noamal_data(g_gui_db_client, 0xF2050200, 0, 0, (uint8*)&display_data.switchout, sizeof(OOP_SWITCHOUT_T));
}
void g_turnstart_draw(HDC hdc)
{
    ITEM_NOT_SELECT;
    SelectFont(hdc, g_guiComm.fontBig);

    uint32 i = 0;
    char czBuf[100] = { 0 };
    int nLeft = 5;
    int nTop = 22;
    uint8 turnMax = 0;
    char* turnName[] = { (char*)"һ��", (char*)"����", (char*)"����", (char*)"����",
                        (char*)"����", (char*)"����", (char*)"����", (char*)"����" };
    RECT rc;

    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    sprintf(czBuf, "%s", "�ִ�״̬");
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    nTop += 24;
    appctrl_get_devturn(&turnMax);
    SelectFont(hdc, g_guiComm.fontSmall);
    for (i = 0; i < turnMax; i++)
    {
        if (i >= display_data.switchout.nNum)
        {
            sprintf(czBuf, "%s:   --    --", turnName[i]);
        }
        else
        {
            if (0 == display_data.switchout.value[i].input)
            {
                sprintf(czBuf, "%s:   %s  %s", turnName[i],
                    0 == display_data.switchout.value[i].current ? "��բ" : "��բ",
                    0 == display_data.switchout.value[i].attribute ? "����ʽ" : "����ʽ");
            }
            else
            {
                sprintf(czBuf, "%s:   --    --", turnName[i]);
            }
        }
        TextOut(hdc, nLeft, nTop, czBuf);
        nTop += 20;
    }
}

//�ִ�״̬
PLUGIN_INFO g_turnstart = {
    0x11010012/*id*/,
    g_turnstart_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    g_turnstart_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("��ȫģʽ",1)
uint8 SafeConfigIndex;
//��ʼ����ȫģʽ
void g_safemode_init(DWORD dwPluginId)
{
    memset(&display_data.safemode, 0, sizeof(OOP_SECURITYFLAG_E));

    int ret = read_db_noamal_data(g_gui_db_client, 0xF1010200, 0, 0, (uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E));
    printf("ret=%d, mod %u", ret, display_data.safemode);

    SafeConfigIndex = 0;
}
//���ư�ȫģʽ����
void g_safemode_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "��ȫģʽ");

    if (display_data.safemode == SECURITY_OFF)
    {
        sprintf(czBuf, "%s", g_on_off[0]);
    }
    else {
        sprintf(czBuf, "%s", g_on_off[1]);
    }
    if (SafeConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nLeft2 += 60;
    if (SafeConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ȡ��");
    }

    index++;
    nTop += 20;
    if (SafeConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

    PopListDraw(hdc);

}
BOOL g_safemode_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 2;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel = 0;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (SafeConfigIndex)
            {
            case 0://��ȫģʽ
            {
                if (nSel == 0) {
                    display_data.safemode = SECURITY_OFF;
                }
                else {
                    display_data.safemode = SECURITY_ON;
                }

                SafeConfigIndex = 1;
            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (SafeConfigIndex > 0)
            SafeConfigIndex--;
        else
            SafeConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (SafeConfigIndex < maxindex)
            SafeConfigIndex++;
        else
            SafeConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (SafeConfigIndex)
            {
            case 0: //��ȫģʽģʽ
            {
                PopList(g_on_off, 2, g_on_off[0], 70, 20, FALSE);
                return TRUE;
            }
            case 1://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E), SecuConfigOad, 0, 0, 2);
                    if (ret != 0)
                    {
                        MessageBox("��ȫģʽд��ʧ�ܣ�");
                    }

                }
                SafeConfigIndex = 0;
                return TRUE;
            }
            case 2://����
            {
                ret = write_normal_data((uint8*)&display_data.safemode, sizeof(OOP_SECURITYFLAG_E), SecuConfigOad, 0, 0, 2);
                if (ret != 0)
                {
                    MessageBox("��ȫģʽд��ʧ�ܣ�");
                }
                else {
                    MessageBox("��ȫģʽд��ɹ���");
                }
                SafeConfigIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }
    return FALSE;
}

//��ȫģʽ
PLUGIN_INFO g_safemode = {
    0x000c0003/*id*/,
    g_safemode_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    g_safemode_draw/*Draw*/,
    NULL/*Timer*/,
    g_safemode_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�߾��ȵ�ǰ�����й�����ʾ��",1)
void high_forward_active_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�����й�����ʾֵ");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����й�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (high_precision_data.penergy.nNum == 0 ||get_bit_value((uint8 *)&high_precision_data.penergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)high_precision_data.penergy.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

//�����޹�����ʾ��
void high_forward_reactive_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�����޹�����ʾֵ");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����޹�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (high_precision_data.penergys.nNum == 0||get_bit_value((uint8 *)&high_precision_data.penergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)high_precision_data.penergys.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


int g_ac_data_display_drawpage_index;
void ac_high_forward_power_display_init(DWORD dwPluginId)
{
    memset(&g_high_precision_data.penergy, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.penergys, 0, sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_gui_db_client, 0x00100400, 1, 0, (uint8*)&g_high_precision_data.penergy, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_gui_db_client, 0x00300400, 1, 0, (uint8*)&g_high_precision_data.penergys, sizeof(OOP_HENERGYS_T));

    g_ac_data_display_drawpage_index = 0;
}

//��ǰ�������ʾ��
void high_forward_power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        high_forward_active_power_draw(hdc, g_high_precision_data);
        break;
    }
    case 1://��2ҳ
    {
        high_forward_reactive_power_draw(hdc, g_high_precision_data);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL high_power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//��ǰ�������ʾ��
PLUGIN_INFO g_layer_ac_high_forward_power_display = {
    0x11010007/*id*/,
    ac_high_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_forward_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�߾��ȵ�ǰ�������ʾ��",1)
void ac_high_reverse_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&g_high_precision_data.nenergy, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.nenergys, 0, sizeof(OOP_HENERGYS_T));
    read_db_noamal_data(g_gui_db_client, 0x00200400, 1, 0, (uint8*)&g_high_precision_data.nenergy, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_gui_db_client, 0x00400400, 1, 0, (uint8*)&g_high_precision_data.nenergys, sizeof(OOP_HENERGYS_T));

    g_ac_data_display_drawpage_index = 0;
}
//�����й�����ʾ��
void high_reverse_active_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�����й�����ʾֵ");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����й�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (high_precision_data.nenergy.nNum == 0||get_bit_value((uint8 *)&high_precision_data.nenergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kWh", (double)high_precision_data.nenergy.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//�����޹�����ʾ��
void high_reverse_reactive_power_draw(HDC hdc, HIGH_PRECISION_DATA_INFO high_precision_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�����޹�����ʾֵ");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����޹�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (high_precision_data.nenergys.nNum == 0||get_bit_value((uint8 *)&high_precision_data.nenergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)high_precision_data.nenergys.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//��ǰ�������ʾ��
void high_reverse_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        high_reverse_active_power_draw(hdc, g_high_precision_data);
        break;
    }
    case 1://��2ҳ
    {
        high_reverse_reactive_power_draw(hdc, g_high_precision_data);
        break;
    }
    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//��ǰ�������ʾ��
PLUGIN_INFO g_layer_ac_high_reverse_power_display = {
    0x11010008/*id*/,
    ac_high_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�߾����������޹�����",1)
void ac_high_quadrant_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&g_high_precision_data.quadrant1, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant2, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant3, 0, sizeof(OOP_HENERGY_T));
    memset(&g_high_precision_data.quadrant4, 0, sizeof(OOP_HENERGY_T));

    read_db_noamal_data(g_gui_db_client, 0x00500400, 1, 0, (uint8*)&g_high_precision_data.quadrant1, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_gui_db_client, 0x00600400, 1, 0, (uint8*)&g_high_precision_data.quadrant2, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_gui_db_client, 0x00700400, 1, 0, (uint8*)&g_high_precision_data.quadrant3, sizeof(OOP_HENERGY_T));
    read_db_noamal_data(g_gui_db_client, 0x00800400, 1, 0, (uint8*)&g_high_precision_data.quadrant4, sizeof(OOP_HENERGY_T));
    
}

//�����й�����ʾ��
void high_quadrant_power_draw(HDC hdc, int id, OOP_HENERGY_T quadrant)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "��%d�����޹�����", id);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����޹�����
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (quadrant.nNum == 0||get_bit_value((uint8 *)&quadrant.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.4f kvarh", (double)quadrant.nValue[i] / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


//�������޹�����
void high_quadrant_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��һҳ
    {
        // ��һ�����޹�  
        high_quadrant_power_draw(hdc, 1, g_high_precision_data.quadrant1);

        break;
    }
    case 1://��4ҳ
    {
        // �ڶ������޹�
        high_quadrant_power_draw(hdc, 2, g_high_precision_data.quadrant2);
        break;
    }
    case 2://��5ҳ
    {
        // ���������޹�
        high_quadrant_power_draw(hdc, 3, g_high_precision_data.quadrant3);
        break;
    }
    case 3://��6ҳ
    {
        // ���������޹�
        high_quadrant_power_draw(hdc, 4, g_high_precision_data.quadrant4);
        break;
    }

    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL high_quadrant_power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 3)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//�������޹�����
PLUGIN_INFO g_layer_ac_high_quadrant_power_display = {
    0x11010009/*id*/,
    ac_high_quadrant_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    high_quadrant_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    high_quadrant_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�����й�����ʾ��",1)
void forward_active_power_draw(HDC hdc, OOP_ENERGY_T penergy, char *title)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����й�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (penergy.nNum == 0 || get_bit_value((uint8 *)&penergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kWh", (double)penergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

//�����޹�����ʾ��
void forward_reactive_power_draw(HDC hdc, OOP_ENERGYS_T penergys, char *title)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����޹�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (penergys.nNum == 0 || get_bit_value((uint8 *)&penergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kvarh", (double)penergys.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void ac_forward_power_display_init(DWORD dwPluginId)
{
    memset(&display_data.penergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.penergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00100200, 0, taskid, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00300200, 0, taskid, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
    }
    g_ac_data_display_drawpage_index = 0;
}

//��ǰ�������ʾ��
void forward_power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        char title[64] = "�����й�����ʾֵ";
        forward_active_power_draw(hdc, display_data.penergy, title);
        break;
    }
    case 1://��2ҳ
    {
        char title[64] = "�����޹�����ʾֵ";
        forward_reactive_power_draw(hdc, display_data.penergys, title);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//��ǰ�������ʾ��
PLUGIN_INFO g_layer_ac_forward_power_display = {
    0x11010001/*id*/,
    ac_forward_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    forward_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("��ǰ�������ʾ��",1)
void ac_reverse_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.nenergy, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.nenergys, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00200200, 0, taskid, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00400200, 0, taskid, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
    }
    g_ac_data_display_drawpage_index = 0;
}
//�����й�����ʾ��
void reverse_active_power_draw(HDC hdc, OOP_ENERGY_T nenergy, char *title)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����й�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (nenergy.nNum == 0  || get_bit_value((uint8 *)&nenergy.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kWh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kWh", (double)nenergy.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//�����޹�����ʾ��
void reverse_reactive_power_draw(HDC hdc, OOP_ENERGYS_T nenergys, char *title)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����޹�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (nenergys.nNum == 0  || get_bit_value((uint8 *)&nenergys.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kvarh", (double)nenergys.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

//��ǰ�������ʾ��
void reverse_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        char title[64] = "�����й�����ʾֵ";
        reverse_active_power_draw(hdc, display_data.nenergy, title);
        break;
    }
    case 1://��2ҳ
    {
        char title[64] = "�����޹�����ʾֵ";
        reverse_reactive_power_draw(hdc, display_data.nenergys, title);
        break;
    }
    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//��ǰ�������ʾ��
PLUGIN_INFO g_layer_ac_reverse_power_display = {
    0x11010002/*id*/,
    ac_reverse_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reverse_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�������޹�����",1)
void ac_quadrant_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.quadrant1, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant2, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant3, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.quadrant4, 0, sizeof(OOP_ENERGY_T));
    //   if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x00500200, 0, taskid, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00600200, 0, taskid, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00700200, 0, taskid, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        //        read_db_record_data(g_gui_db_client, 0x00800200, 0, taskid, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
    }
}

//�����й�����ʾ��
void quadrant_power_draw(HDC hdc, OOP_ENERGY_T quadrant, char *title)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 30;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    DrawText(hdc, title, strlen(title), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    // �����й�
    for (int i = 0; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft, nTop, "��"); break;
        case 1:TextOut(hdc, nLeft, nTop, "��"); break;
        case 2:TextOut(hdc, nLeft, nTop, "��"); break;
        case 3:TextOut(hdc, nLeft, nTop, "ƽ"); break;
        case 4:TextOut(hdc, nLeft, nTop, "��"); break;
        default:ASSERT(FALSE);
        }

        if (quadrant.nNum == 0 || get_bit_value((uint8 *)&quadrant.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kvarh");
        }
        else
        {
            sprintf(czBuf, "%12.2f kvarh", (double)quadrant.nValue[i] / 100);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


//�������޹�����
void quadrant_power_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��һҳ
    {
        // ��һ�����޹� 
        char title[64] = "��1�����޹�����";
        quadrant_power_draw(hdc, display_data.quadrant1, title);

        break;
    }
    case 1://��4ҳ
    {
        // �ڶ������޹�
        char title[64] = "��2�����޹�����";
        quadrant_power_draw(hdc, display_data.quadrant2, title);
        break;
    }
    case 2://��5ҳ
    {
        // ���������޹�
        char title[64] = "��3�����޹�����";
        quadrant_power_draw(hdc, display_data.quadrant3, title);
        break;
    }
    case 3://��6ҳ
    {
        // ���������޹�
        char title[64] = "��4�����޹�����";
        quadrant_power_draw(hdc, display_data.quadrant4, title);
        break;
    }

    }
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL quadrant_power_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 3)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//�������޹�����
PLUGIN_INFO g_layer_ac_quadrant_power_display = {
    0x11010003/*id*/,
    ac_quadrant_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    quadrant_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    quadrant_power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("��ѹ��������",1)
void ac_vol_cur_display_init(DWORD dwPluginId)
{
    memset(&display_data.vol, 0, sizeof(OOP_ENERGY_T));
    memset(&display_data.cur, 0, sizeof(OOP_ENERGYS_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
#ifdef PRODUCT_CCU
		display_data.retzeroCur = read_db_noamal_data(g_gui_db_client, 0x20010400, 1, 0, (uint8*)&display_data.zeroCur, sizeof(int32));
#endif
    }
    //   else
    {
        //       read_db_record_data(g_gui_db_client, 0x20000200, 0, taskid, (uint8*)&display_data.vol, sizeof(OOP_DWORD3V_T));
        //        read_db_record_data(g_gui_db_client, 0x20010200, 0, taskid, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
    }

}

void vol_cur_display_draw(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 50;
    int nTop = 42;
    int tmpTop = 13;
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "��ѹ����");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "��ѹ");
    SelectFont(hdc, g_guiComm.fontSmall);

    // ��ѹ
    for (i = 0; i < 3; i++)
    {
        if (bdisplay_data.vol.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.vol.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)bdisplay_data.vol.nValue[i] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "����");
    SelectFont(hdc, g_guiComm.fontSmall);
    // ����
    for (i = 0; i < 3; i++)
    {
        if (bdisplay_data.cur.nNum == 0 || get_bit_value((uint8 *)&bdisplay_data.cur.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (bdisplay_data.cur.nValue[i] < 0)
            {
                tmp = ~bdisplay_data.cur.nValue[i] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = bdisplay_data.cur.nValue[i];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

}

void vol_cur_display_draws(HDC hdc)
{
	SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 50;
    int nTop = 42;
#ifdef PRODUCT_CCU
    int tmpTop = 13;
#else
    int tmpTop = 14;
#endif
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "��ѹ����");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "��ѹ");
    SelectFont(hdc, g_guiComm.fontSmall);

    // ��ѹ
    for (i = 0; i < 3; i++)
    {
        if (display_data.vol.nNum == 0 || get_bit_value((uint8 *)&display_data.vol.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.vol.nValue[i] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "����");
    SelectFont(hdc, g_guiComm.fontSmall);
    // ����
    for (i = 0; i < 3; i++)
    {
        if (display_data.cur.nNum == 0 || get_bit_value((uint8 *)&display_data.cur.rsv,32,i) == 0)
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (display_data.cur.nValue[i] < 0)
            {
                tmp = ~display_data.cur.nValue[i] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = display_data.cur.nValue[i];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }
#ifdef PRODUCT_CCU
    if(0 != display_data.retzeroCur)
    {
        sprintf(czBuf, "%s --- A", "In");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        tmp = (float)display_data.zeroCur / 1000;
        sprintf(czBuf, "%s %3.3f A", "In", tmp);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    nTop += tmpTop;
#endif
}


//��ѹ��������
PLUGIN_INFO g_layer_ac_vol_cur_display = {
    0x11010004/*id*/,
    ac_vol_cur_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    vol_cur_display_draws/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("��ǰ����ʾ��",1)
void ac_power_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.ppower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.npower, 0, sizeof(OOP_INT4V_T));
    memset(&display_data.fpower, 0, sizeof(OOP_LONG4V_T));
    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x20040200, 0, taskid, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_gui_db_client, 0x20050200, 0, taskid, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        //        read_db_record_data(g_gui_db_client, 0x200a0200, 0, taskid, (uint8*)&display_data.fpower, sizeof(OOP_INT4V_T));
    }

//    printf("display_data.fpower[0] %d, [1]%d\n ", display_data.fpower.nValue[0], display_data.fpower.nValue[0]);

}

void power_display_draw_param(HDC hdc, const char* pname, const char** item, OOP_INT4V_T power, const char* punit, int multiple)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };
    float tmp;

    int nLeft = 5;
    int nLeft2 = 80;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, item[i]);
        if (power.nNum == 0 || get_bit_value((uint8 *)&power.rsv,32,i) == 0)
        {
            sprintf(czBuf, "-------- %s", punit);
        }
        else
        {
            if (power.nValue[i] < 0)
            {
                tmp = ~power.nValue[i] + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / multiple, punit);
            }
            else
            {
                tmp = power.nValue[i];
                sprintf(czBuf, "%6.4f %s", tmp / multiple, punit);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}


void power_display_draw_params(HDC hdc, const char* pname, const char** item, OOP_LONG4V_T power, const char* punit, int multiple)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };
    float tmp;

    int nLeft = 5;
    int nLeft2 = 80;
    int nTop = 26;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = 0; i < 4; i++)
    {
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, item[i]);
        if (power.nNum == 0 || get_bit_value((uint8 *)&power.rsv,32,i) == 0)
        {
            sprintf(czBuf, "-------- %s", punit);
        }
        else
        {
            if (power.nValue[i] < 0)
            {
                tmp = ~power.nValue[i] + 1;
                sprintf(czBuf, "-%4.3f %s", tmp / multiple, punit);
            }
            else
            {
                tmp = power.nValue[i];
                sprintf(czBuf, "%4.3f %s", tmp / multiple, punit);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

void power_display_draw(HDC hdc)
{

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        // �й�����
        power_display_draw_param(hdc, "�й�����", g_showTitleJCPower_Y, display_data.ppower, "kw", 10000);
        break;
    }
    case 1://��2ҳ
    {
        // �޹�����
        power_display_draw_param(hdc, "�޹�����", g_showTitleJCPower_W, display_data.npower, "kvar", 10000);
        break;
    }
    case 2://��3ҳ
    {
        // ��������
        power_display_draw_params(hdc, "��������", g_showTitleJCPowerFactor, display_data.fpower, "", 1000);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL powers_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 2)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 2;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//��ǰ����ʾ��
PLUGIN_INFO g_layer_ac_power_display = {
    0x11010005/*id*/,
    ac_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    power_display_draw/*Draw*/,
    NULL/*Timer*/,
    powers_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
    
#if DESC("�������й�����",1)
void ac_demand_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(&display_data.pdemand, 0, sizeof(OOP_DEMAND_T));
    memset(&display_data.ndemand, 0, sizeof(OOP_DEMAND_T));

    //    if(taskid == 0)
    {
        read_db_noamal_data(g_gui_db_client, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_gui_db_client, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
    //    else
    {
        //        read_db_record_data(g_gui_db_client, 0x10100200, 0, taskid, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        //        read_db_record_data(g_gui_db_client, 0x10200200, 0, taskid, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
    }
}

void demand_display_draw_param(HDC hdc, const char* pname, OOP_DEMAND_T demand)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 8;
    int nLeft1 = 22;
    int nLeft2 = 45;
    int nTop = 26;
    int tmpTop = 16;
    int index = g_ac_data_display_drawpage_index;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%s", pname);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, g_guiComm.fontSmall);

    for (int i = (index%2)*3; i < 5; i++)
    {
        nTop += tmpTop;
        switch (i) {
        case 0:TextOut(hdc, nLeft1, nTop, "��:"); break;
        case 1:TextOut(hdc, nLeft1, nTop, "��:"); break;
        case 2:TextOut(hdc, nLeft1, nTop, "��:"); break;
        case 3:TextOut(hdc, nLeft1, nTop, "ƽ:"); break;
        case 4:TextOut(hdc, nLeft1, nTop, "��:"); break;
        default:ASSERT(FALSE);
        }

        if (demand.nNum == 0 || get_bit_value((uint8 *)&demand.rsv,32,i) == 0)
        {
            sprintf(czBuf, "------------ kW");
        }
        else
        {
            sprintf(czBuf, "%10.4f kW", (double)demand.demand[i].nValue / 10000);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, "ʱ��:");
        if (demand.nNum == 0)
        {
            sprintf(czBuf, "------------ ");
        }
        else
        {
            sprintf(czBuf, "%hu-%02hu-%02hu %02hu:%02hu:%02hu",
                demand.demand[i].time.year,
                demand.demand[i].time.month,
                demand.demand[i].time.day,
                demand.demand[i].time.hour,
                demand.demand[i].time.minute,
                demand.demand[i].time.second);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        if (nTop >= 122)
        {
            break;
        }
    }

}

void demand_display_draw(HDC hdc)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    case 1://��2ҳ
    {
        // �����й�����
        demand_display_draw_param(hdc, "�����й��������", display_data.pdemand);

        break;
    }
    case 2://��3ҳ
    case 3://��4ҳ
    {
        // �����й�����
        demand_display_draw_param(hdc, "�����й��������", display_data.ndemand);
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

BOOL demand_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 3)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 3;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//�������й�����
PLUGIN_INFO g_layer_ac_demand_display = {
    0x11010006/*id*/,
    ac_demand_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    demand_display_draw/*Draw*/,
    NULL/*Timer*/,
    demand_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��ѹ����г��������",1)
int8 g_harmonic_index;
void harmonic_display_init(DWORD dwPluginId)
{
    memset(display_data.harmonicV, 0, sizeof(OOP_HARMONIC_T)*3);
    memset(display_data.harmonicC, 0, sizeof(OOP_HARMONIC_T)*3);
    g_harmonic_index = 0;

    read_db_noamal_data(g_gui_db_client, 0x200D0200, 1, 0, (uint8*)&display_data.harmonicV[0], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_gui_db_client, 0x200D0300, 1, 0, (uint8*)&display_data.harmonicV[1], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_gui_db_client, 0x200D0400, 1, 0, (uint8*)&display_data.harmonicV[2], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_gui_db_client, 0x200E0200, 1, 0, (uint8*)&display_data.harmonicC[0], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_gui_db_client, 0x200E0300, 1, 0, (uint8*)&display_data.harmonicC[1], sizeof(OOP_HARMONIC_T));
    read_db_noamal_data(g_gui_db_client, 0x200E0400, 1, 0, (uint8*)&display_data.harmonicC[2], sizeof(OOP_HARMONIC_T));

}

void harmonic_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 50;
    int nTop = 42;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    if(g_harmonic_index == 0)
    {
        sprintf(czBuf, "��г��������");
    }
    else
    {
        sprintf(czBuf, "%u��г��������", g_harmonic_index + 1);
    }
    
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "��ѹ");
    SelectFont(hdc, g_guiComm.fontSmall);

    // ��ѹ
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicV[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicV[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- %%", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.harmonicV[i].nValue[g_harmonic_index] / 100;
            sprintf(czBuf, "%s %2.2f %%", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "����");
    SelectFont(hdc, g_guiComm.fontSmall);
    // ����
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicC[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicC[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- %%", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            
            tmp = display_data.harmonicC[i].nValue[g_harmonic_index];
            tmp = tmp / 100;
            sprintf(czBuf, "%s %2.2f %%", g_showTitleEelectric[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

BOOL harmonic_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_harmonic_index++;
        if (g_harmonic_index >= OOP_MAX_HARMONIC)
        {
            g_harmonic_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_harmonic_index--;
        if (g_harmonic_index < 0)
        {
            g_harmonic_index = OOP_MAX_HARMONIC - 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//��ѹ����г��������
PLUGIN_INFO g_layer_harmonic_display = {
    0x1101000a/*id*/,
    harmonic_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    harmonic_display_draw/*Draw*/,
    NULL/*Timer*/,
    harmonic_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��ѹ����г������",1)
int8 g_harmonic_value_index;
void harmonic_value_display_init(DWORD dwPluginId)
{
    memset(display_data.harmonicV_value, 0, sizeof(OOP_HARMONIC_U_VALUE_T)*3);
    memset(display_data.harmonicC_value, 0, sizeof(OOP_HARMONIC_I_VALUE_T)*3);
    g_harmonic_value_index = 1;

    read_db_noamal_data(g_gui_db_client, 0x20330200, 1, 0, (uint8*)&display_data.harmonicV_value[0], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_gui_db_client, 0x20330300, 1, 0, (uint8*)&display_data.harmonicV_value[1], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_gui_db_client, 0x20330400, 1, 0, (uint8*)&display_data.harmonicV_value[2], sizeof(OOP_HARMONIC_U_VALUE_T));
    read_db_noamal_data(g_gui_db_client, 0x20340200, 1, 0, (uint8*)&display_data.harmonicC_value[0], sizeof(OOP_HARMONIC_I_VALUE_T));
    read_db_noamal_data(g_gui_db_client, 0x20340300, 1, 0, (uint8*)&display_data.harmonicC_value[1], sizeof(OOP_HARMONIC_I_VALUE_T));
    read_db_noamal_data(g_gui_db_client, 0x20340400, 1, 0, (uint8*)&display_data.harmonicC_value[2], sizeof(OOP_HARMONIC_I_VALUE_T));

}

void harmonic_value_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft2 = 50;
    int nTop = 42;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 18;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "%u��г����ѹ����", g_harmonic_value_index + 1);
    
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "��ѹ");
    SelectFont(hdc, g_guiComm.fontSmall);

    // ��ѹ
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicV_value[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicV_value[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- V", g_showTitleVoltag34[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            tmp = (float)display_data.harmonicV_value[i].nValue[g_harmonic_value_index] / 10;
            sprintf(czBuf, "%s %2.1f V", g_showTitleVoltag34[i], tmp);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;
    }

    SelectFont(hdc, g_guiComm.fontBig);
    TextOut(hdc, 14, nTop, "����");
    SelectFont(hdc, g_guiComm.fontSmall);
    // ����
    for (i = 0; i < 3; i++)
    {
        if (display_data.harmonicC_value[i].nNum == 0 || get_bit_value((uint8 *)&display_data.harmonicC_value[i].rsv,32,g_harmonic_index) == 0 )
        {
            sprintf(czBuf, "%s --- A", g_showTitleEelectric[i]);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (display_data.harmonicC_value[i].nValue[g_harmonic_value_index] < 0)
            {
                tmp = ~display_data.harmonicC_value[i].nValue[g_harmonic_value_index] + 1;
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s -%3.3f A", g_showTitleEelectric[i], tmp);
            }
            else
            {
                tmp = display_data.harmonicC_value[i].nValue[g_harmonic_value_index];
                tmp = tmp / 1000.0;
                sprintf(czBuf, "%s %3.3f A", g_showTitleEelectric[i], tmp);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        nTop += tmpTop;

    }
}

BOOL harmonic_value_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_harmonic_value_index++;
        if (g_harmonic_value_index > OOP_MAX_HARMONIC - 1)
        {
            g_harmonic_value_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_harmonic_value_index--;
        if (g_harmonic_value_index < 1)
        {
            g_harmonic_value_index = OOP_MAX_HARMONIC - 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//��ѹ����г��������
PLUGIN_INFO g_layer_harmonic_value_display = {
    0x1101000b/*id*/,
    harmonic_value_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    harmonic_value_display_draw/*Draw*/,
    NULL/*Timer*/,
    harmonic_value_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ն�������", 1)
MEASURE_MAIN g_measure_main_data;
OOP_METER_T  MeterFile;
const char*  g_measure_point_state[] = {"��Ч", "��Ч"};
const char*  g_measure_point_type[] = {"-", "485��", "�ز���", "���ɱ�"};
char g_day_frozen_store_time[16] = "";
int g_day_frozen_display_drawpage_index;

//���˿�OADת��Ϊ����������
int get_infonum_type(OOP_OAD_U port)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
            return 2;
    }
    break;
    case 0xF201:
    {
        if (port.attID == 2)
            return 1;
    }
    break;
	case 0xF208:
    {
        if (port.attID == 2)
            return 3;
    }
    break;
    default:
        break;
    }
    return 0;

}

void day_frozen_time2str(char *str, int strlen)
{
    time_t timep = 0;
    time(&timep);       //��ȡ��1970������˶����룬����time_t���͵�timep
    timep -= 24 * 3600; //��ǰʱ���ǰһ��
    strftime(str, strlen, "%Y%m%d", localtime((time_t*)&timep));
}

void day_frozen_time_flush(char* pNewStr, void* pPara)
{
    GUI_FMT_DEBUG("day_frozen_time_flush %s\n", pNewStr);
    int len;
    len = strlen(pNewStr);
    if (len < 0 || len > 8)
    {
        char bzbuf[100];
        char buf[32];
        time_t timep = 0;
        
        time(&timep);       //��ȡ��1970������˶����룬����time_t���͵�timep
        timep -= 24 * 3600; //��ǰʱ���ǰһ��
        strftime(buf, sizeof(buf), "%Y%m%d(%Y��%m��%d��)", localtime((time_t*)&timep));
        sprintf(bzbuf, "����ʾ��Ϊ:\n %s\nȷ���Ƿ���������?", buf);
        if(TRUE == MessageBox(bzbuf, MB_OKCANCEL))
        {
            InputBox(g_day_frozen_store_time, day_frozen_time_flush, NULL, 0x01);
        }
    }
    else
    {
        memcpy(g_day_frozen_store_time, pNewStr, sizeof(g_day_frozen_store_time));
    }
}

void day_frozen_str2time(uint32            *tm)
{
    uint16          year;
    uint8           month;
    uint8           day;
    DateTimeHex_t   startData;

    year    = (g_day_frozen_store_time[0] - '0') * 1000 + (g_day_frozen_store_time[1] - '0') * 100 +
                (g_day_frozen_store_time[2] - '0') * 10 + (g_day_frozen_store_time[3]- '0');
    month   = (g_day_frozen_store_time[4] - '0') * 10 + (g_day_frozen_store_time[5] - '0');
    day     = (g_day_frozen_store_time[6] - '0') * 10 + (g_day_frozen_store_time[7] - '0');

    // �ն���洢ʱ��Ϊĳ���23��59��0��
    startData.year_h    = year >> 8;
    startData.year_l    = year & 0xFF;
    startData.month     = month;
    startData.day       = day;
    startData.hour      = 0x0;
    startData.min       = 0x0;
    startData.sec       = 0x0;

    if (!DT_DateTimeHex2Time((DateTimeHex_t*)&startData, (time_t*)tm))
    {
        GUI_FMT_DEBUG("day_frozen_str2time: store time error!\n");
    }
    
}

int day_frozen_db_read(DB_CLIENT fp, uint8 logic_id, uint16 info_num, uint8* pbuf, uint32 buf_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;
    //DateTimeHex_t      startData;
    uint32             startTm = 0;
    uint32             endTm = 0;

    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    day_frozen_str2time(&startTm);
    // ע��+8��Сʱ(28800)ת�ɸ���ʱ��
    startTm += COMPENSATION_TIME;
    endTm = startTm+24*60*60-1;

    inReadRecord.recordOAD.optional = 1;
    inReadRecord.recordOAD.logicId = logic_id;
    inReadRecord.recordOAD.infoNum = info_num;
    inReadRecord.recordOAD.road.oadMain.value = 0x50040200;
    inReadRecord.recordOAD.road.oadCols.nNum = 8;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x00100200;
    inReadRecord.recordOAD.road.oadCols.oad[1].value = 0x00300200;
    inReadRecord.recordOAD.road.oadCols.oad[2].value = 0x00200200;
    inReadRecord.recordOAD.road.oadCols.oad[3].value = 0x00400200;
    inReadRecord.recordOAD.road.oadCols.oad[4].value = 0x00500200;
    inReadRecord.recordOAD.road.oadCols.oad[5].value = 0x00600200;
    inReadRecord.recordOAD.road.oadCols.oad[6].value = 0x00700200;
    inReadRecord.recordOAD.road.oadCols.oad[7].value = 0x00800200;
    inReadRecord.recordOAD.classtag = 5;
    inReadRecord.cType = COL_TM_STORE;
    inReadRecord.cStart = startTm;
    inReadRecord.cEnd = endTm;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.ti.unit = 0;
    inReadRecord.ti.value = 0;

    inReadRecord.MAC.nNum = 6;

    if(info_num == 0)
    {
        memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
        nomalOAD.logicId = logic_id;
        nomalOAD.oad.value = 0x40010200;
        ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
        if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
        {
            GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
            return -1;
        }
         memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);
    }
    else
    {
        if (app_get_meter_list(info_num, &oopMeter) != ERR_OK)
        {
            GUI_FMT_DEBUG(" app_get_meter_list %d !\n", info_num);
            return -1;
        }
        memcpy(inReadRecord.MAC.value, oopMeter.basic.tsa.add, 6);
    }

    ret = db_read_record(fp, &inReadRecord, buf_len, &stepRecordOut);
    if (ret != ERR_OK)
    {
        GUI_FMT_DEBUG(" db_read_record %08x ret=%d !\n", 0x00100200, ret);
        return -1;
    }
    memcpy(&display_day_frozen.penergy, stepRecordOut.record[0].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.penergys, stepRecordOut.record[1].outDataBuf, sizeof(OOP_ENERGYS_T));
    memcpy(&display_day_frozen.nenergy, stepRecordOut.record[2].outDataBuf, sizeof(OOP_ENERGY_T));

    memcpy(&display_day_frozen.nenergys, stepRecordOut.record[3].outDataBuf, sizeof(OOP_ENERGYS_T));
    memcpy(&display_day_frozen.quadrant1, stepRecordOut.record[4].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.quadrant2, stepRecordOut.record[5].outDataBuf, sizeof(OOP_ENERGY_T));
    memcpy(&display_day_frozen.quadrant3, stepRecordOut.record[6].outDataBuf, sizeof(OOP_ENERGY_T)); 
    memcpy(&display_day_frozen.quadrant4, stepRecordOut.record[7].outDataBuf, sizeof(OOP_ENERGY_T));
    return 0;
}

void day_frozen_display_init(DWORD dwPluginId)
{
    g_day_frozen_display_drawpage_index = 0;
    if (g_guiComm.nCurPoint == 0)
    {
        memset(&display_data, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_noamal_data(g_gui_db_client, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
    }
    else
    {
        memset(&display_day_frozen, 0, sizeof(DISPLAY_DAY_FROZEN_INFO));
        day_frozen_db_read(g_gui_db_client, 0, g_guiComm.nCurPoint, (uint8*)&display_day_frozen, sizeof(DISPLAY_DAY_FROZEN_INFO));
    }
}

void day_frozen_display_draw(HDC hdc)
{
    switch (g_day_frozen_display_drawpage_index)
    {
        case 0://��1ҳ
        {
            // �����й�
            char title[64] = "�ն��������й�����";
            forward_active_power_draw(hdc, display_day_frozen.penergy, title);
            break;
        }
        case 1://��2ҳ
        {
            // �����޹�
            char title[64] = "�ն��������޹�����";
            forward_reactive_power_draw(hdc, display_day_frozen.penergys, title);
            break;
        }
        case 2:
        {
            // �����й�
            char title[64] = "�ն��ᷴ���й�����";
            reverse_active_power_draw(hdc, display_day_frozen.nenergy, title);
            break;
        }
        case 3:
        {
            // �����޹�
            char title[64] = "�ն��ᷴ���޹�����";
            reverse_reactive_power_draw(hdc, display_day_frozen.nenergys, title);
            break;
        }
        case 4:
        {
            // ��һ�����޹�
            char title[64] = "�ն����1�����޹�";
            quadrant_power_draw(hdc, display_day_frozen.quadrant1, title);
            break;
        }
        case 5:
        {
            // �ڶ������޹�
            char title[64] = "�ն����2�����޹�";
            quadrant_power_draw(hdc, display_day_frozen.quadrant2, title);
            break;
        }
        case 6:
        {
            // ���������޹�
            char title[64] = "�ն����3�����޹�";
            quadrant_power_draw(hdc, display_day_frozen.quadrant3, title);
            break;
        }
        case 7:
        {
            // ���������޹�
            char title[64] = "�ն����4�����޹�";
            quadrant_power_draw(hdc, display_day_frozen.quadrant4, title);
            break;
        }
        default:
            break;
    }
}

BOOL day_frozen_display_keymsg(int nKey)
{
    switch (nKey)
    {
        case KEY_DOWN:
        {
            
            g_day_frozen_display_drawpage_index++;
            if (g_day_frozen_display_drawpage_index > 7)
            {
                g_day_frozen_display_drawpage_index = 0;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return  TRUE;

        }
        case KEY_UP:
        {
            g_day_frozen_display_drawpage_index--;
            if (g_day_frozen_display_drawpage_index < 0)
            {
                g_day_frozen_display_drawpage_index = 7;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 0;
            }
            advance_gui_infonum((uint16)g_guiComm.nCurPoint);
            day_frozen_display_init(0);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return  TRUE;

        }
        break;
        
        case KEY_LEFT:
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 0)
            {
                g_guiComm.nCurPoint = MAX_SEL_POINT;
            }
            advance_gui_infonum((uint16)g_guiComm.nCurPoint);
            day_frozen_display_init(0);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
        
        default:
            break;
    }

    return FALSE;
}

//�ն�������
PLUGIN_INFO g_layer_day_frozen_display = {
    0x00/*id*/,
    day_frozen_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    day_frozen_display_draw/*Draw*/,
    NULL/*Timer*/,
    day_frozen_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//�ն�������
PLUGIN_INFO* g_day_frozen_plugin[] =
{
    &g_layer_day_frozen_display,
};

void measure_day_frozen_display_init(DWORD dwPluginId)
{
    memset(&g_measure_main_data, 0, sizeof(g_measure_main_data));
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
        
    // ���õ�ǰѡ����
    g_guiComm.nCurPoint = 1;
    g_measure_main_data.nIndex = 0;
    g_measure_main_data.pCurPlugin = NULL;

    // �ն���ʱ����ʾΪ��ǰʱ���ǰһ��
    day_frozen_time2str(g_day_frozen_store_time, sizeof(g_day_frozen_store_time));
}

void measure_day_frozen_display_draw(HDC hdc)
{
    // ��������ϸ��Ϣҳ������
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_measure_main_data.pCurPlugin->pfnDraw != NULL);
        g_measure_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    int nTop = 20;
    int nLeft1 = 50;
    TextOut(hdc, nLeft1, nTop, "��������");

    // ѡ�������
    nLeft1 = 5;
    int nLeft2 = 85;
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "���붳������");
    if (g_measure_main_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_day_frozen_store_time);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_day_frozen_store_time);
    }

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "��ѡ�������");
    char czBuf[20];
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    advance_gui_infonum((uint16)g_guiComm.nCurPoint);
    if (g_measure_main_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    //GUI_FMT_DEBUG("measure_data_display_draw %d\n", g_guiComm.nCurPoint);
    if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) == ERR_OK)
    {
        g_measure_main_data.nState = 1;
        g_measure_main_data.nType = get_infonum_type(MeterFile.basic.port);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        g_measure_main_data.nState = 0;
        g_measure_main_data.nType = 0;
    }
    
    // ������״̬
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "������״̬");
    TextOut(hdc, nLeft2, nTop, g_measure_point_state[g_measure_main_data.nState]);
    
    // ����������
    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "����������");
    TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
    
    if (g_measure_main_data.nState)
    {
        // ���ַ
        nTop += 18;
        TextOut(hdc, nLeft1, nTop, "���ַ");
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ��ϸ��Ϣ
        nTop += 18;
        if (g_measure_main_data.nIndex == 2)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "��ϸ��Ϣ");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "��ϸ��Ϣ");
        }
    }
}

static BOOL measure_day_frozen_display_keymsg(int nKey)
{
    // ������ҳ��İ�����Ϣ����
    // ��ǰ���ǻ��ж������ҳ�棬���·�ҳ������
    // ���Ǻ���ֻ��һ��ҳ���ˣ����ǽṹ�����棬�����Ժ����չ
    int nPoint = g_guiComm.nCurPoint;
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        if (g_measure_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_measure_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }
        // ʹ��ȱʡ����
        switch (nKey)
        {
        case KEY_CANCEL:
            if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnUninitLayer();
                g_guiComm.nCurPoint = nPoint;
            }
            g_measure_main_data.pCurPlugin = NULL;
            if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) != ERR_OK) //�����ȡ����ʧ��
                g_measure_main_data.nIndex = 0;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// ���Ϸ�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin--;
                if (g_measure_main_data.nCurPlugin < 0)
                {
                    g_measure_main_data.nCurPlugin = g_measure_main_data.nPluginCnt - 1;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        case KEY_DOWN:// ���·�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin++;
                if (g_measure_main_data.nCurPlugin >= g_measure_main_data.nPluginCnt)
                {
                    g_measure_main_data.nCurPlugin = 0;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_UP:
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex--;
            if (g_measure_main_data.nIndex < 0)
            {
                g_measure_main_data.nIndex = 2;
            }
        }
        else
        {
             if (g_measure_main_data.nIndex == 1)
                g_measure_main_data.nIndex = 0;
             else
                g_measure_main_data.nIndex = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    {
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex++;
            if (g_measure_main_data.nIndex > 2)
            {
                g_measure_main_data.nIndex = 0;
            }
        }
        else
        {
             if (g_measure_main_data.nIndex == 1)
                g_measure_main_data.nIndex = 0;
             else
                g_measure_main_data.nIndex = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:
    {
        if (g_measure_main_data.nIndex == 1)
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 1)
            {
                g_guiComm.nCurPoint = MAX_SEL_POINT;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_RIGHT:
    {
        if (g_measure_main_data.nIndex == 1)
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 1;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_OK:
    {// ȷ��ʱ����
        switch (g_measure_main_data.nIndex)
        {
        case 0: // ��������ѡ��List
            InputBox(g_day_frozen_store_time, day_frozen_time_flush, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 1:// ����������ѡ��List
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 2:
            g_measure_main_data.ppDetail = g_day_frozen_plugin;
            g_measure_main_data.nPluginCnt = sizeof(g_day_frozen_plugin) / sizeof(g_day_frozen_plugin[0]);
            g_measure_main_data.nCurPlugin = 0;
            g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
            ASSERT(g_measure_main_data.pCurPlugin != NULL);
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.nArrow = 2;
            }
            if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        }
        return TRUE;
    }
    }
    return FALSE;
}


//�����ն������ݲ鿴
PLUGIN_INFO g_layer_measure_day_frozen_display = {
    0x11020002/*id*/,
    measure_day_frozen_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    measure_day_frozen_display_draw/*Draw*/,
    NULL/*Timer*/,
    measure_day_frozen_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�������ݲ鿴",1)
void data_display_init(DWORD dwPluginId)
{ 
    g_ac_data_display_drawpage_index = 0;
    if (g_guiComm.nCurPoint == 0)
    {
        memset(&display_data, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_noamal_data(g_gui_db_client, 0x00100200, 1, 0, (uint8*)&display_data.penergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00300200, 1, 0, (uint8*)&display_data.penergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_gui_db_client, 0x00200200, 1, 0, (uint8*)&display_data.nenergy, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00400200, 1, 0, (uint8*)&display_data.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_noamal_data(g_gui_db_client, 0x00500200, 1, 0, (uint8*)&display_data.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00600200, 1, 0, (uint8*)&display_data.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00700200, 1, 0, (uint8*)&display_data.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x00800200, 1, 0, (uint8*)&display_data.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_noamal_data(g_gui_db_client, 0x20000200, 1, 0, (uint8*)&display_data.vol, sizeof(OOP_WORD3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20010200, 1, 0, (uint8*)&display_data.cur, sizeof(OOP_INT3V_T));
        read_db_noamal_data(g_gui_db_client, 0x20040200, 1, 0, (uint8*)&display_data.ppower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x20050200, 1, 0, (uint8*)&display_data.npower, sizeof(OOP_INT4V_T));
        read_db_noamal_data(g_gui_db_client, 0x200a0200, 1, 0, (uint8*)&display_data.fpower, sizeof(OOP_LONG4V_T));
        read_db_noamal_data(g_gui_db_client, 0x10100200, 1, 0, (uint8*)&display_data.pdemand, sizeof(OOP_DEMAND_T));
        read_db_noamal_data(g_gui_db_client, 0x10200200, 1, 0, (uint8*)&display_data.ndemand, sizeof(OOP_DEMAND_T));
        
        display_data.retzeroVol = read_db_noamal_data(g_gui_db_client, 0x20000600, 1, 0, (uint8*)&display_data.zeroVol, sizeof(uint32));
        display_data.zeroCur = read_db_noamal_data(g_gui_db_client, 0x20010400, 1, 0, (uint8*)&display_data.zeroCur, sizeof(int));
    }
    else
    {
        memset(&display_datas, 0, sizeof(DISPLAY_DATA_INFO));
        read_db_record_data(g_gui_db_client, 0x00100200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.penergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00300200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.penergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_gui_db_client, 0x00200200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.nenergy, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00400200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.nenergys, sizeof(OOP_ENERGYS_T));
        read_db_record_data(g_gui_db_client, 0x00500200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant1, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00600200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant2, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00700200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant3, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x00800200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.quadrant4, sizeof(OOP_ENERGY_T));
        read_db_record_data(g_gui_db_client, 0x20000200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.vol, sizeof(OOP_WORD3V_T));
        read_db_record_data(g_gui_db_client, 0x20010200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.cur, sizeof(OOP_INT3V_T));
        read_db_record_data(g_gui_db_client, 0x20040200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.ppower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_gui_db_client, 0x20050200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.npower, sizeof(OOP_INT4V_T));
        read_db_record_data(g_gui_db_client, 0x200a0200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.fpower, sizeof(OOP_LONG4V_T));
        read_db_record_data(g_gui_db_client, 0x10100200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.pdemand, sizeof(OOP_DEMAND_T));
        read_db_record_data(g_gui_db_client, 0x10200200, 0, g_guiComm.nCurPoint, (uint8*)&display_datas.ndemand, sizeof(OOP_DEMAND_T));

        display_data.retzeroVol = read_db_noamal_data(g_gui_db_client, 0x20000600, 0, g_guiComm.nCurPoint, (uint8*)&display_data.zeroVol, sizeof(uint32));
        display_data.zeroCur = read_db_noamal_data(g_gui_db_client, 0x20010400, 0, g_guiComm.nCurPoint, (uint8*)&display_data.zeroCur, sizeof(int));
    }

}

void data_display_draws(HDC hdc, DISPLAY_DATA_INFO bdisplay_data)
{
    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        // �����й�
        char title[64] = "�����й�����ʾֵ";
        forward_active_power_draw(hdc, bdisplay_data.penergy, title);
        break;
    }
    case 1://��2ҳ
    {
        // �����޹�
        char title[64] = "�����޹�����ʾֵ";
        forward_reactive_power_draw(hdc, bdisplay_data.penergys, title);
        break;
    }
    case 2://��3ҳ
    {
        // �����й�
        char title[64] = "�����й�����ʾֵ";
        reverse_active_power_draw(hdc, bdisplay_data.nenergy, title);
        break;
    }
    case 3://��4ҳ
    {
        // �����޹�
        char title[64] = "�����޹�����ʾֵ";
        reverse_reactive_power_draw(hdc, bdisplay_data.nenergys, title);
        break;
    }
    case 4://��5ҳ
    {
        // ��һ�����޹�
        char title[64] = "��1�����޹�����";
        quadrant_power_draw(hdc, bdisplay_data.quadrant1, title);
        break;
    }
    case 5://��6ҳ
    {
        // �ڶ������޹�
        char title[64] = "��2�����޹�����";
        quadrant_power_draw(hdc, bdisplay_data.quadrant2, title);
        break;
    }
    case 6://��7ҳ
    {
        // ���������޹�
        char title[64] = "��3�����޹�����";
        quadrant_power_draw(hdc, bdisplay_data.quadrant3, title);
        break;
    }
    case 7://��8ҳ
    {
        // ���������޹�
        char title[64] = "��4�����޹�����";
        quadrant_power_draw(hdc, bdisplay_data.quadrant4, title);
        break;
    }
    case 8://��9ҳ
    {
        vol_cur_display_draw(hdc, bdisplay_data);
        break;
    }
    case 9://��10ҳ
    {
        // �й�����
        power_display_draw_param(hdc, "�й�����", g_showTitleJCPower_Y, bdisplay_data.ppower, "kw", 10000);
        break;
    }
    case 10://��11ҳ
    {
        // �޹�����
        power_display_draw_param(hdc, "�޹�����", g_showTitleJCPower_W, bdisplay_data.npower, "kvar", 10000);

        break;
    }
    case 11://��12ҳ
    {
        // ��������
        power_display_draw_params(hdc, "��������", g_showTitleJCPowerFactor, bdisplay_data.fpower, "", 1000);
        break;
    }
    case 12://��13ҳ
    case 13://��14ҳ
    {
        // �����й�����
        demand_display_draw_param(hdc, "�����й��������", bdisplay_data.pdemand);

        break;
    }
    case 14://��15ҳ
    case 15://��16ҳ
    {
        // �����й�����
        demand_display_draw_param(hdc, "�����й��������", bdisplay_data.ndemand);
    }
    break;

    }

}

void data_display_draw(HDC hdc)
{
    if (g_guiComm.nCurPoint == 0)
    {
        data_display_draws(hdc, display_data);
    }
    else
    {
        data_display_draws(hdc, display_datas);
    }
}


BOOL data_display_keymsg(int nKey)
{
    int tmp_index = 0;
    switch (nKey)
    {
    case KEY_DOWN:
    {
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 15)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 15;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        g_guiComm.nCurPoint++;
        if (g_guiComm.nCurPoint > MAX_SEL_POINT)
        {
            g_guiComm.nCurPoint = 1;
        }
        advance_gui_infonum((uint16)g_guiComm.nCurPoint);
        tmp_index = g_ac_data_display_drawpage_index;
        data_display_init(0);
        g_ac_data_display_drawpage_index = tmp_index;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint < 1)
        {
            g_guiComm.nCurPoint = MAX_SEL_POINT;
        }
        advance_gui_infonum((uint16)g_guiComm.nCurPoint);
        tmp_index = g_ac_data_display_drawpage_index;
        data_display_init(0);
        g_ac_data_display_drawpage_index = tmp_index;
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//��������
PLUGIN_INFO g_layer_data_display = {
    0x00/*id*/,
    data_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_display_draw/*Draw*/,
    NULL/*Timer*/,
    data_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//��������
PLUGIN_INFO* g_measure_plugin[] =
{
    &g_layer_data_display,
};

void measure_data_display_init(DWORD dwPluginId)
{
    memset(&g_measure_main_data, 0, sizeof(g_measure_main_data));
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
        
    // ���õ�ǰѡ����
    g_guiComm.nCurPoint = 1;
    g_measure_main_data.nIndex = 0;
}

void measure_data_display_draw(HDC hdc)
{
    // ��������ϸ��Ϣҳ������
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_measure_main_data.pCurPlugin->pfnDraw != NULL);
        g_measure_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    int nTop = 20;
    int nLeft1 = 50;
    TextOut(hdc, nLeft1, nTop, "ʵʱ����");

    // ѡ�������
    nLeft1 = 5;
    int nLeft2 = 85;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��ѡ�������");
    char czBuf[20];
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    advance_gui_infonum((uint16)g_guiComm.nCurPoint);
    if (g_measure_main_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    //GUI_FMT_DEBUG("measure_data_display_draw %d\n", g_guiComm.nCurPoint);
    if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) == ERR_OK)
    {
        g_measure_main_data.nState = 1;
        g_measure_main_data.nType = get_infonum_type(MeterFile.basic.port);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        g_measure_main_data.nState = 0;
        g_measure_main_data.nType = 0;
    }
    
    // ������״̬
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "������״̬");
    TextOut(hdc, nLeft2, nTop, g_measure_point_state[g_measure_main_data.nState]);
    
    // ����������
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "����������");
    TextOut(hdc, nLeft2, nTop, g_measure_point_type[g_measure_main_data.nType]);
    
    if (g_measure_main_data.nState)
    {
        // ���ַ
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "���ַ");
        TextOut(hdc, nLeft2, nTop, czBuf);

        // ��ϸ��Ϣ
        nTop += 20;
        if (g_measure_main_data.nIndex == 1)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "��ϸ��Ϣ");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "��ϸ��Ϣ");
        }
    }
}

static BOOL measure_data_display_keymsg(int nKey)
{
    // ������ҳ��İ�����Ϣ����
    // ��ǰ���ǻ��ж������ҳ�棬���·�ҳ������
    // ���Ǻ���ֻ��һ��ҳ���ˣ����ǽṹ�����棬�����Ժ����չ
    int nPoint = g_guiComm.nCurPoint;
    if (g_measure_main_data.pCurPlugin != NULL)
    {
        if (g_measure_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_measure_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }
        // ʹ��ȱʡ����
        switch (nKey)
        {
        case KEY_CANCEL:
            if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnUninitLayer();
                g_guiComm.nCurPoint = nPoint;
            }
            g_measure_main_data.pCurPlugin = NULL;
            if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) != ERR_OK) //�����ȡ����ʧ��
                g_measure_main_data.nIndex = 0;
            g_guiComm.nArrow = -1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case KEY_UP:// ���Ϸ�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin--;
                if (g_measure_main_data.nCurPlugin < 0)
                {
                    g_measure_main_data.nCurPlugin = g_measure_main_data.nPluginCnt - 1;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        case KEY_DOWN:// ���·�ҳ�棨���ж��ҳ�������£�
            if (g_measure_main_data.nPluginCnt > 1)
            {
                if (g_measure_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnUninitLayer();
                    g_guiComm.nCurPoint = nPoint;
                }
                g_measure_main_data.pCurPlugin = NULL;

                g_measure_main_data.nCurPlugin++;
                if (g_measure_main_data.nCurPlugin >= g_measure_main_data.nPluginCnt)
                {
                    g_measure_main_data.nCurPlugin = 0;
                }
                g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
                ASSERT(g_measure_main_data.pCurPlugin != NULL);

                if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
                }

                if (g_measure_main_data.nCurPlugin > 0 && g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 0;
                }
                else if (g_measure_main_data.nCurPlugin > 0)
                {
                    g_guiComm.nArrow = 1;
                }
                else if (g_measure_main_data.nCurPlugin < g_measure_main_data.nPluginCnt - 1)
                {
                    g_guiComm.nArrow = 2;
                }
                else
                {
                    g_guiComm.nArrow = -1;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
            }
            break;
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_UP:
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex--;
            if (g_measure_main_data.nIndex < 0)
            {
                g_measure_main_data.nIndex = 1;
                return FALSE;
            }
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_DOWN:
    {
        if (g_measure_main_data.nState)
        {
            g_measure_main_data.nIndex++;
            if (g_measure_main_data.nIndex > 1)
            {
                g_measure_main_data.nIndex = 0;
                return FALSE;
            }
        }   
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:
    {
        if (g_measure_main_data.nIndex == 0)
        {
            g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint < 1)
            {
                g_guiComm.nCurPoint = MAX_SEL_POINT;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_RIGHT:
    {
        if (g_measure_main_data.nIndex == 0)
        {
            g_guiComm.nCurPoint++;
            if (g_guiComm.nCurPoint > MAX_SEL_POINT)
            {
                g_guiComm.nCurPoint = 1;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    break;
    case KEY_OK:
    {// ȷ��ʱ����
        switch (g_measure_main_data.nIndex)
        {
        case 0:// ����������ѡ��List
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        case 1:
            g_measure_main_data.ppDetail = g_measure_plugin;
            g_measure_main_data.nPluginCnt = sizeof(g_measure_plugin) / sizeof(g_measure_plugin[0]);
            g_measure_main_data.nCurPlugin = 0;
            g_measure_main_data.pCurPlugin = g_measure_main_data.ppDetail[g_measure_main_data.nCurPlugin];
            ASSERT(g_measure_main_data.pCurPlugin != NULL);
            if (g_measure_main_data.nPluginCnt > 1)
            {
                g_guiComm.nArrow = 2;
            }
            if (g_measure_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_measure_main_data.pCurPlugin->pfnInitLayer(g_measure_main_data.pCurPlugin->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            break;
        }
        return TRUE;
    }
    }
    return FALSE;
}


//����ʵʱ���ݲ鿴
PLUGIN_INFO g_layer_measure_data_display = {
    0x11020001/*id*/,
    measure_data_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    measure_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    measure_data_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("ң��״̬",1)

void switch_status_display_init(DWORD dwPluginId)
{
    memset(&switchState, 0, sizeof(OOP_SWITCHIN_STATE_T));
    memset(&switchFlag, 0, sizeof(OOP_SWITCHIN_FLAG_T));
    
    read_db_noamal_data(g_gui_db_client, 0xf2030200, 0, 0, (uint8*)&switchState, sizeof(OOP_SWITCHIN_STATE_T));
    read_db_noamal_data(g_gui_db_client, 0xf2030400, 0, 0, (uint8*)&switchFlag, sizeof(OOP_SWITCHIN_FLAG_T));
}

//ң��״̬
void switch_status_display_draw(HDC hdc)
{
    char *switchname[] = {(char*)"һ", (char*)"��", (char*)"��", (char*)"��", (char*)"��", 
                          (char*)"��", (char*)"��", (char*)"��"};
    char czBuf[64] = {0};
    OOP_STATUS_SIGNAL_T  g_StatusSignal;//�źŽڵ�

    int nLeft = 5;
    int nLeft1 = 90;
    int nTop = 35;
    int tmpTop = 22;
    uint8 status = 0;
#ifdef PRODUCT_ECU
    uint8 portMax = 8;
#else
    uint8 portMax = 2;
#endif
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "ң��״̬");
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    status = pulse_get_rmtcfg();

    // ң��״̬
    for (int i = 0; i < portMax; i++)
    {
        if(0 == i % 2)
        {
            nTop += tmpTop;
        }
        if((switchFlag.inputFlag.bitValue >> i) & 0x01 && 1 == ((status >> i) & 0x01))
        {
            sprintf(czBuf, "��%s·  %s", switchname[i], g_showSwitchState[switchState.state[i].st]);
        }
        else
        {
            sprintf(czBuf, "��%s·  --", switchname[i]);
        }
        
        TextOut(hdc, (0 == i % 2) ? nLeft : nLeft1, nTop, czBuf);
    }

    memset(&g_StatusSignal, 0, sizeof(OOP_STATUS_SIGNAL_T));
    read_db_noamal_data(g_gui_db_client, 0xf2030600, 0, 0, (uint8*)&g_StatusSignal, sizeof(OOP_STATUS_SIGNAL_T));

    //�Žڵ�
#ifdef PRODUCT_CCU
    nTop += tmpTop;
    memset(czBuf, 0, 64);
    if(0 == g_StatusSignal.nNum)
    {
        sprintf(czBuf, "�Žڵ�  --");
    }
    else
    {
        sprintf(czBuf, "�Žڵ�  %s",  g_showSwitchState[g_StatusSignal.ST.nValue & 0x01]);
    }
    TextOut(hdc, nLeft, nTop, czBuf);
#endif
    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//ң��״̬
PLUGIN_INFO g_layer_switch_status_display = {
    0x11010011/*id*/,
    switch_status_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    switch_status_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("ESAM��Ϣ",1)
extern uint8 g_TESAMInfoBuff[2048];

//ң��״̬
void esam_info_display_draw(HDC hdc)
{
    char czBuf[64] = {0};

    int nLeft = 5;
    int nLeft2 = 48;
    int nTop = 24;
    int tmpTop = 22;
    
    SelectFont(hdc, g_guiComm.fontBig);

    TextOut(hdc, nLeft2, nTop, "ESAM��Ϣ");
    ITEM_NOT_SELECT;

    SelectFont(hdc, g_guiComm.fontSmall);

    nTop += tmpTop + tmpTop;
    sprintf(czBuf, "���к�: %02X%02X%02X%02X%02X%02X%02X%02X", g_TESAMInfoBuff[0], g_TESAMInfoBuff[1], 
                   g_TESAMInfoBuff[2], g_TESAMInfoBuff[3], g_TESAMInfoBuff[4], g_TESAMInfoBuff[5],
                   g_TESAMInfoBuff[6], g_TESAMInfoBuff[7]);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�汾��: %02X%02X%02X%02X", g_TESAMInfoBuff[8], g_TESAMInfoBuff[9], 
                   g_TESAMInfoBuff[10], g_TESAMInfoBuff[11]);
    TextOut(hdc, nLeft, nTop, czBuf);
}

//ң��״̬
PLUGIN_INFO g_layer_esam_display = {
    0x11010031/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    esam_info_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif


/**********************************************************************
* @name      : gui_check_cell_insert
* @brief     ����ң���л�ȡ����Ƿ����
* @param[in] ��
* @param[out]��
* @return    ��TRUE-����/FALSE-û�в���
* @Create    : ����
* @Date      ��2022-4-13
* @Update    :
**********************************************************************/
uint8 gui_check_cell_insert()
{
    uint8  type = 1;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    
    int fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (fd < 0)
    {
        return 0;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        //����5�ζ�����ͬ��ֵ����ֹ����
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            type = ((data1 >> 8) & 0x0001);
            break;
        }
    }
    close(fd);

    return 1 == type ? FALSE : TRUE;
}

void battery_voltage_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
	if(pg_guicomm == NULL)
		return;
    //int       nTop = 0;
    char      czbuf[32];
    RECT      rc;

    uint8     flag1 = 0;
    uint8     flag2 = 0;
    uint8     flag3 = 0;

    int       datav1 = 0;
    int       datav2 = 0;
    int       datav3 = 0;
    int nTop = 16;
    int nLeft = 10;
    int nLeft2 = 80;


	SelectFont(hdc,g_guiComm.fontBig);

    //������ʾ
    ITEM_NOT_SELECT;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    rc.top = 0;
    rc.bottom = rc.top+16;

	SelectFont(hdc,pg_guicomm->fontSmall);
    nTop = rc.top + 24;

    ADC_DEVICE_T* adcdev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
    if(NULL != adcdev)
    {
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_RTCVOL_CHANNEL, &datav1))
        {
            flag1 = 1;
        }
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_BATVOL_CHANNEL, &datav2))
        {
            flag2 = 1;
        }
        
        if(0 == adcdev->adc_volt_read(adcdev, EMADC_CAPVOL_CHANNEL, &datav3))
        {
            flag3 = 1;
        }
        hal_device_release((HW_DEVICE *)adcdev);
    }

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "ʱ�ӵ�أ�");
    memset(czbuf, 0, 32);
    if(0 == flag1)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav1 / 1000, datav1 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "���õ�أ�");
    memset(czbuf, 0, 32);
    if(0 == flag2 || FALSE == gui_check_cell_insert())
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav2 / 1000, datav2 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);

    nTop += 20;
    TextOut(hdc, nLeft, nTop, "�������ݣ�");
    memset(czbuf, 0, 32);
    if(0 == flag3)
    {
        sprintf(czbuf, "----");
    }
    else
    {
        sprintf(czbuf, "%d.%03d V", datav3 / 1000, datav3 % 1000);
    }
    TextOut(hdc, nLeft2, nTop, czbuf);
}

PLUGIN_INFO g_layer_battery_display = {
    0x11040031/*id*/,
    NULL/*InitLayer*/,
    NULL/*UninitLayer*/,
    battery_voltage_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


#if DESC("ֱ��ģ����",1)
void dc_data_display_init(DWORD dwPluginId)
{
    

}

void dc_data_display_draw(HDC hdc)
{

}

//ֱ��ģ����
PLUGIN_INFO g_layer_dc_data_display = {
    0xffffffff/*id*/,
    dc_data_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    dc_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��������",1)
void plus_data_display_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));

    read_db_noamal_data(g_gui_db_client, 0x24010500, 0, 0, (uint8*)&g_ctrl_data[0].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24010600, 0, 0, (uint8*)&g_ctrl_data[0].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24020500, 0, 0, (uint8*)&g_ctrl_data[1].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24020600, 0, 0, (uint8*)&g_ctrl_data[1].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24030500, 0, 0, (uint8*)&g_ctrl_data[2].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24030600, 0, 0, (uint8*)&g_ctrl_data[2].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24040500, 0, 0, (uint8*)&g_ctrl_data[3].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24040600, 0, 0, (uint8*)&g_ctrl_data[3].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24050500, 0, 0, (uint8*)&g_ctrl_data[4].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24050600, 0, 0, (uint8*)&g_ctrl_data[4].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24060500, 0, 0, (uint8*)&g_ctrl_data[5].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24060600, 0, 0, (uint8*)&g_ctrl_data[5].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24070500, 0, 0, (uint8*)&g_ctrl_data[6].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24070600, 0, 0, (uint8*)&g_ctrl_data[6].plus_reactive_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24080500, 0, 0, (uint8*)&g_ctrl_data[7].plus_active_power, sizeof(int32));
    read_db_noamal_data(g_gui_db_client, 0x24080600, 0, 0, (uint8*)&g_ctrl_data[7].plus_reactive_power, sizeof(int32));

}

void plus_data_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    float tmp;
    int i = 0;

    char czBuf[64] = { 0 };

    int nLeft = 8;
    int nLeft2 = 55;
    int nTop = 20;
    int tmpTop = 14;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = 16;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "���幦��");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    switch (g_ac_data_display_drawpage_index)
    {
    case 0://��1ҳ
    {
        for (i = 0; i < 4; i++)
        {
            nTop += tmpTop;
            sprintf(czBuf, " %d·�й�", i + 1);
            TextOut(hdc, nLeft, nTop, czBuf);
            if (g_ctrl_data[i].plus_active_power < 0)
            {
                tmp = ~g_ctrl_data[i].plus_active_power + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kw");
            }
            else
            {
                tmp = g_ctrl_data[i].plus_active_power;
                sprintf(czBuf, "%6.4f %s", tmp / 10000, "kw");
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += tmpTop;
            sprintf(czBuf, " %d·�޹�", i + 1);
            TextOut(hdc, nLeft, nTop, czBuf);
            if (g_ctrl_data[i].plus_reactive_power < 0)
            {
                tmp = ~g_ctrl_data[i].plus_reactive_power + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kvar");
            }
            else
            {
                tmp = g_ctrl_data[i].plus_reactive_power;
                sprintf(czBuf, "%6.4f %s", tmp / 10000, "kvar");
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        break;
    }
    case 1://��2ҳ
    {
        for (i = 4; i < 8; i++)
        {
            nTop += tmpTop;
            sprintf(czBuf, " %d·�й�", i + 1);
            TextOut(hdc, nLeft, nTop, czBuf);
            if (g_ctrl_data[i].plus_active_power < 0)
            {
                tmp = ~g_ctrl_data[i].plus_active_power + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kw");
            }
            else
            {
                tmp = g_ctrl_data[i].plus_active_power;
                sprintf(czBuf, "%6.4f %s", tmp / 10000, "kw");
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += tmpTop;
            sprintf(czBuf, " %d·�޹�", i + 1);
            TextOut(hdc, nLeft, nTop, czBuf);
            if (g_ctrl_data[i].plus_reactive_power < 0)
            {
                tmp = ~g_ctrl_data[i].plus_reactive_power + 1;
                sprintf(czBuf, "-%6.4f %s", tmp / 10000, "kvar");
            }
            else
            {
                tmp = g_ctrl_data[i].plus_reactive_power;
                sprintf(czBuf, "%6.4f %s", tmp / 10000, "kvar");
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        break;
    }
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//��������
PLUGIN_INFO g_layer_plus_data_display = {
    0x11010013/*id*/,
    plus_data_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    power_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ܼ�������",1)
void gruop_data_display_init(DWORD dwPluginId)
{
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;

    advance_gui_infonum(g_guiComm.nCurPoint + 1);
    oadvalue = 0x23010300 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_active_power, sizeof(int64));

    oadvalue = 0x23010400 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_reactive_power, sizeof(int64));

    oadvalue = 0x23010700 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_day_energy, sizeof(OOP_HENERGYS_T));
    oadvalue = 0x23010800 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_day_ractenergy, sizeof(OOP_HENERGYS_T));

    oadvalue = 0x23010900 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_month_energy, sizeof(OOP_HENERGYS_T));
    oadvalue = 0x23010a00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_month_ractenergy, sizeof(OOP_HENERGYS_T));
}

void gruop_data_display_uninit()
{
    advance_gui_infonum(0);
}

void gruop_data_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ܼ��� %d ����", g_guiComm.nCurPoint + 1);

    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    int nTop = rc.top + 20;
    int nLeft1 = 5;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "���й�����");

    // �����ۼ��õ���
    if (g_ctrl_data[g_guiComm.nCurPoint].gruop_day_energy.nNum == 0)
    {
        sprintf(czBuf, "       - kWh");
    }
    else
    {
        sprintf(czBuf, "%9.0f kWh", (double)(g_ctrl_data[g_guiComm.nCurPoint].gruop_day_energy.nValue[0] / 10000));
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "���޹�����");
    if (g_ctrl_data[g_guiComm.nCurPoint].gruop_day_ractenergy.nNum == 0)
    {
        sprintf(czBuf, "       - kvarh");
    }
    else
    {
        sprintf(czBuf, "%9.0f kvarh", (double)(g_ctrl_data[g_guiComm.nCurPoint].gruop_day_ractenergy.nValue[0] / 10000));
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "���й�����");
    if (g_ctrl_data[g_guiComm.nCurPoint].gruop_month_energy.nNum == 0)
    {
        sprintf(czBuf, "       - kWh");
    }
    else
    {
        sprintf(czBuf, "%9.0f kWh", (double)(g_ctrl_data[g_guiComm.nCurPoint].gruop_month_energy.nValue[0] / 10000));
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "���޹�����");
    if (g_ctrl_data[g_guiComm.nCurPoint].gruop_month_ractenergy.nNum == 0)
    {
        sprintf(czBuf, "       - kvarh");
    }
    else
    {
        sprintf(czBuf, "%9.0f kvarh", (double)(g_ctrl_data[g_guiComm.nCurPoint].gruop_month_ractenergy.nValue[0] / 10000));
    }
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "  �й�����");
    sprintf(czBuf, "%9.2f kW", (double)g_ctrl_data[g_guiComm.nCurPoint].gruop_active_power / 10000);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 18;
    TextOut(hdc, nLeft1, nTop, "  �޹�����");
    sprintf(czBuf, "%9.2f kvar", (double)g_ctrl_data[g_guiComm.nCurPoint].gruop_reactive_power / 10000);

    TextOut(hdc, nLeft2, nTop, czBuf);
}

//�ܼ�����1
PLUGIN_INFO g_layer_gruop_data_display1 = {
    0x11011000/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ�����2
PLUGIN_INFO g_layer_gruop_data_display2 = {
    0x11011001/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ܼ�����3
PLUGIN_INFO g_layer_gruop_data_display3 = {
    0x11011002/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ܼ�����4
PLUGIN_INFO g_layer_gruop_data_display4 = {
    0x11011003/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ܼ�����5
PLUGIN_INFO g_layer_gruop_data_display5 = {
    0x11011004/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ�����6
PLUGIN_INFO g_layer_gruop_data_display6 = {
    0x11011005/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ܼ�����7
PLUGIN_INFO g_layer_gruop_data_display7 = {
    0x11011006/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ܼ�����8
PLUGIN_INFO g_layer_gruop_data_display8 = {
    0x11011007/*id*/,
    gruop_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ܼ������״̬",1)
void control_group_display_init(DWORD dwPluginId)
{
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;
    advance_gui_infonum(g_guiComm.nCurPoint + 1);

    oadvalue = 0x23011000 + (0x00010000) * g_guiComm.nCurPoint;
    memset(&g_ctrl_data[g_guiComm.nCurPoint].ctrl_seting, 0, sizeof(OOP_TGSETTING_T));
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].ctrl_seting, sizeof(OOP_TGSETTING_T));

    oadvalue = 0x23011100 + (0x00010000) * g_guiComm.nCurPoint;
    memset(&g_ctrl_data[g_guiComm.nCurPoint].ctrl_status, 0, sizeof(OOP_TGCURSTATE_T));
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].ctrl_status, sizeof(OOP_TGCURSTATE_T));
}

void control_group_display_draw(HDC hdc)
{
    uint8 status_tmp = 0;
    uint8 status_tmp1 = 0;

    char czBuf[300];
    int nTop = 32;
    int nLeft1 = 1;
    int nLeft2 = 60;
    int tmpTop = 16;
    int i = 0;
    uint8 turnmax = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�ܼ���%d ����״̬", g_guiComm.nCurPoint + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* Ĭ���Ѿ�����һ��ģ�� */
    appctrl_get_devturn(&turnmax);

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += tmpTop;
    nLeft2 = nLeft1 + 50;
    status_tmp = g_ctrl_data[g_guiComm.nCurPoint].ctrl_status.pcTrip.nValue;
    status_tmp1 = g_ctrl_data[g_guiComm.nCurPoint].ctrl_seting.pcTurn.nValue;
    TextOut(hdc, nLeft1, nTop, "  ������բ");
    for (i = 0; i < turnmax; i++)
    {
        nLeft2 += 20;
        if ((status_tmp1 >> i) & 0x01)
        {
            sprintf(czBuf, "%s", g_showCtrlState[(status_tmp >> i & 0x01)]);
        }
        else
        {
            sprintf(czBuf, "%s", "--");

        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += tmpTop;
    nLeft2 = nLeft1 + 50;
    status_tmp = g_ctrl_data[g_guiComm.nCurPoint].ctrl_status.mecTrip.nValue;
    status_tmp1 = g_ctrl_data[g_guiComm.nCurPoint].ctrl_seting.ecTurn.nValue;
    TextOut(hdc, nLeft1, nTop, "�µ����բ");
    for (i = 0; i < turnmax; i++)
    {
        nLeft2 += 20;
        if ((status_tmp1 >> i) & 0x01)
        {
            sprintf(czBuf, "%s", g_showCtrlState[(status_tmp >> i & 0x01)]);
        }
        else
        {
            sprintf(czBuf, "%s", "--");

        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += tmpTop;
    nLeft2 = nLeft1 + 50;
    status_tmp = g_ctrl_data[g_guiComm.nCurPoint].ctrl_status.becTrip.nValue;
    TextOut(hdc, nLeft1, nTop, "�������բ");
    for (i = 0; i < turnmax; i++)
    {
        nLeft2 += 20;
        if ((status_tmp1 >> i) & 0x01)
        {
            sprintf(czBuf, "%s", g_showCtrlState[(status_tmp >> i & 0x01)]);
        }
        else
        {
            sprintf(czBuf, "%s", "--");

        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    /* ����ֻ��һ����Ч */
    nTop += tmpTop + 8;
    TextOut(hdc, nLeft1, nTop, "�澯״̬");
    status_tmp = g_ctrl_data[g_guiComm.nCurPoint].ctrl_status.pcWarn.nValue;
    nLeft2 = nLeft1 + 64;
    if (status_tmp & 0x01)
    {
        TextOut(hdc, nLeft2, nTop, "ʱ�ο�");
    }
    if (status_tmp & 0x02)
    {
        TextOut(hdc, nLeft2, nTop, "���ݿ�");
    }
    if (status_tmp & 0x04)
    {
        TextOut(hdc, nLeft2, nTop, "��ͣ��");
    }
    if (status_tmp & 0x08)
    {
        TextOut(hdc, nLeft2, nTop, "�¸���");
    }

    status_tmp = g_ctrl_data[g_guiComm.nCurPoint].ctrl_status.ecWarn.nValue;
    nTop += tmpTop;
    nLeft2 = nLeft1 + 64;
    if (status_tmp & 0x01)
    {
        TextOut(hdc, nLeft2, nTop, "�µ��");
        nLeft2 += 48;
    }
    if (status_tmp & 0x02)
    {
        TextOut(hdc, nLeft2, nTop, "�����");
    }
}

//�ܼ������״̬1
PLUGIN_INFO g_layer_control_group_display1 = {
    0x11012000/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬2
PLUGIN_INFO g_layer_control_group_display2 = {
    0x11012001/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬3
PLUGIN_INFO g_layer_control_group_display3 = {
    0x11012002/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬4
PLUGIN_INFO g_layer_control_group_display4 = {
    0x11012003/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬5
PLUGIN_INFO g_layer_control_group_display5 = {
    0x11012004/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬6
PLUGIN_INFO g_layer_control_group_display6 = {
    0x11012005/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬7
PLUGIN_INFO g_layer_control_group_display7 = {
    0x11012006/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�ܼ������״̬8
PLUGIN_INFO g_layer_control_group_display8 = {
    0x11012007/*id*/,
    control_group_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_group_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("���ص�ؿ���״̬",1)

uint16 g_CtrlInfoNum = 0;

uint8  g_CtrlPeriodEffect = 0;
uint8  g_CtrlRestEffect = 0;
uint8  g_CtrlStopEffect = 0;
uint8  g_CtrlDownEffect = 0;

uint8  g_CtrlBuyEffect = 0;
uint8  g_CtrlMonthEffect = 0;

uint8  g_CtrlProtectStatus = 0;

void control_pc_display_init(DWORD dwPluginId)
{
    g_CtrlInfoNum = (dwPluginId & 0x0F) + 1;
    advance_gui_infonum(g_CtrlInfoNum);

    g_CtrlPeriodEffect = 0;
    g_CtrlRestEffect = 0;
    g_CtrlStopEffect = 0;
    g_CtrlDownEffect = 0;
    read_db_noamal_data(g_gui_db_client, 0x810306ff, 0, g_CtrlInfoNum, &g_CtrlPeriodEffect, sizeof(uint8));
    read_db_noamal_data(g_gui_db_client, 0x810406ff, 0, g_CtrlInfoNum, &g_CtrlRestEffect, sizeof(uint8));
    read_db_noamal_data(g_gui_db_client, 0x810506ff, 0, g_CtrlInfoNum, &g_CtrlStopEffect, sizeof(uint8));
    read_db_noamal_data(g_gui_db_client, 0x810606ff, 0, g_CtrlInfoNum, &g_CtrlDownEffect, sizeof(uint8));
}

void control_pc_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft1 = 10;
    int tmpTop = 16;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "����״̬ �ܼ���%d", g_CtrlInfoNum);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += tmpTop;
    sprintf(czBuf, "ʱ�ο�    %s", 0 == g_CtrlPeriodEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "���ݿ�    %s", 0 == g_CtrlRestEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��ͣ��    %s", 0 == g_CtrlStopEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�¸���    %s", 0 == g_CtrlDownEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);
}

void control_ec_display_init(DWORD dwPluginId)
{
    g_CtrlBuyEffect = 0;
    g_CtrlMonthEffect = 0;
    g_CtrlInfoNum = (dwPluginId & 0x0F) + 1;
    advance_gui_infonum(g_CtrlInfoNum);

    read_db_noamal_data(g_gui_db_client, 0x810706ff, 0, g_CtrlInfoNum, &g_CtrlBuyEffect, sizeof(uint8));
    read_db_noamal_data(g_gui_db_client, 0x810806ff, 0, g_CtrlInfoNum, &g_CtrlMonthEffect, sizeof(uint8));

    g_CtrlProtectStatus = 0;
    read_db_noamal_data(g_gui_db_client, 0x80010200, 0, 0, &g_CtrlProtectStatus, sizeof(uint8));
}

void control_ec_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft1 = 10;
    int tmpTop = 16;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "���״̬ �ܼ���%d", g_CtrlInfoNum);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += tmpTop;
    sprintf(czBuf, "�µ��    %s", 0 == g_CtrlMonthEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�����    %s", 0 == g_CtrlBuyEffect ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);

    nTop += tmpTop + 8;
    sprintf(czBuf, "����״̬  %s", 0 == g_CtrlProtectStatus ? "���" : "Ͷ��");
    TextOut(hdc, nLeft1, nTop, czBuf);
}


//���ؿ���״̬1
PLUGIN_INFO g_layer_control_pc_display1 = {
    0x11012100/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬2
PLUGIN_INFO g_layer_control_pc_display2 = {
    0x11012101/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬3
PLUGIN_INFO g_layer_control_pc_display3 = {
    0x11012102/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬4
PLUGIN_INFO g_layer_control_pc_display4 = {
    0x11012103/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬5
PLUGIN_INFO g_layer_control_pc_display5 = {
    0x11012104/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬6
PLUGIN_INFO g_layer_control_pc_display6 = {
    0x11012105/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬7
PLUGIN_INFO g_layer_control_pc_display7 = {
    0x11012106/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//���ؿ���״̬8
PLUGIN_INFO g_layer_control_pc_display8 = {
    0x11012107/*id*/,
    control_pc_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_pc_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬1
PLUGIN_INFO g_layer_control_ec_display1 = {
    0x11012200/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬2
PLUGIN_INFO g_layer_control_ec_display2 = {
    0x11012201/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬3
PLUGIN_INFO g_layer_control_ec_display3 = {
    0x11012202/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬4
PLUGIN_INFO g_layer_control_ec_display4 = {
    0x11012203/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬5
PLUGIN_INFO g_layer_control_ec_display5 = {
    0x11012204/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬6
PLUGIN_INFO g_layer_control_ec_display6 = {
    0x11012205/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬7
PLUGIN_INFO g_layer_control_ec_display7 = {
    0x11012206/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ؿ���״̬8
PLUGIN_INFO g_layer_control_ec_display8 = {
    0x11012207/*id*/,
    control_ec_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    control_ec_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
#endif


#if DESC("ͣ�ϵ��¼�",1)
uint32 g_event_num;
int    g_event_index;

BOOL event_display_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        g_event_index++;
        if ((uint32)g_event_index >= g_event_num)
        {
            g_event_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    case KEY_LEFT:
    {
        g_event_index--;
        if (g_event_index < 0)
        {
            g_event_index = (g_event_num==0)?0: g_event_num-1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

void get_event_data(DB_CLIENT fp, uint32 oad_value, uint8** p_event_list, uint32 event_len)
{
    int                ret = ERR_NORMAL;
    uint32             len     = 0;
    time_t             timep   = 0;
    uint32             handle  = 0;
    uint8*             ptmp    =  *p_event_list;

    READ_RECORD_T      inReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_METER_T        oopMeter;
    NOMAL_OAD_T        nomalOAD;
    OOP_OCTETVAR16_T   m_tmnAddr;
    STEP_READ_RECORD_T stepReadRecord;

    g_event_num = 0;
    g_event_index = 0;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&oopMeter, 0, sizeof(OOP_METER_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    time(&timep); //��ȡ��1970������˶����룬����time_t���͵�timep

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0;
    inReadRecord.cType = COL_NO_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = timep;
    inReadRecord.sortType = DOWN_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad_value;

    inReadRecord.MAC.nNum = 6;

    memset(&nomalOAD,0,sizeof(NOMAL_OAD_T));
    nomalOAD.logicId = 0;
    nomalOAD.oad.value = 0x40010200;
    ret = db_read_nomal(fp, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&m_tmnAddr, &len);
    if(0 != ret || len != sizeof(OOP_OCTETVAR16_T))
    {
        GUI_FMT_DEBUG(" db_read_nomal 0x40010200 err = %d!\n", ret);
        return ;
    }
     memcpy(inReadRecord.MAC.value, m_tmnAddr.value, 6);

     ret = db_read_record_start(fp, &inReadRecord, &handle, &g_event_num);
    if (0 != ret || 0 == g_event_num)
    {
        GUI_FMT_DEBUG(" db_read_record_start  ret=%d, num%d!\n", ret, g_event_num);
        return ;
    }

    ptmp = (uint8*)malloc(event_len*g_event_num);
    if (ptmp == NULL)
    {
        GUI_FMT_DEBUG(" malloc  err !\n");
        g_event_num = 0;
        return ;
    }
    memset(ptmp, 0, event_len*g_event_num);
    
    memcpy(&stepReadRecord.MAC, &inReadRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(uint32 i=0; i<g_event_num; i++)
    {
        /* �����һ����¼ */
        ret = db_read_record_step(fp, handle, &stepReadRecord, &stepRecordOut);
        if (0 == ret)
        {
            memcpy(ptmp+(i*event_len), stepRecordOut.record[0].outDataBuf, event_len);
        }
    }
    *p_event_list = ptmp;

    /*��������¼*/
     db_read_record_end(fp, handle);
}

OOP_POWERLOG_T *g_powerEventlist;
OOP_POWERLOG_T g_powerEvent;
void power_off_on_display_init(DWORD dwPluginId)
{
    g_powerEventlist = NULL;
    get_event_data(g_gui_db_client, 0x31060200, (uint8**)&g_powerEventlist, sizeof(OOP_POWERLOG_T));
}
void power_off_on_display_Uninit()
{
    if (g_powerEventlist != NULL)
    {
        free(g_powerEventlist);
        g_powerEventlist = NULL;
    }
    
}

//ͣ�ϵ��¼�
void power_off_on_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 65;
    int nTop = 10;
    int tmpTop = 18;

    memset(&g_powerEvent, 0, sizeof(OOP_POWERLOG_T));
    if (g_powerEventlist != NULL)
    {
        g_powerEvent = g_powerEventlist[g_event_index];
    }

    //ͣ�ϵ��¼�
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��¼���");
    sprintf(czBuf, "%u", g_powerEvent.nIndex);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nLeft2 -= 25;
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����");
    sprintf(czBuf, "%hu-%hu-%hu %hu:%hu:%hu",
        g_powerEvent.tmStart.year,
        g_powerEvent.tmStart.month,
        g_powerEvent.tmStart.day,
        g_powerEvent.tmStart.hour,
        g_powerEvent.tmStart.minute,
        g_powerEvent.tmStart.second);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����");
    sprintf(czBuf, "%hu-%hu-%hu %hu:%hu:%hu",
        g_powerEvent.tmEnd.year,
        g_powerEvent.tmEnd.month,
        g_powerEvent.tmEnd.day,
        g_powerEvent.tmEnd.hour,
        g_powerEvent.tmEnd.minute,
        g_powerEvent.tmEnd.second);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nLeft2 += 25;
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����Դ");
    if (g_powerEvent.source.nValue == 0)
    {
        TextOut(hdc, nLeft2, nTop, "ͣ��");
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "�ϵ�");
    }

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "�¼�״̬");
    if ((g_powerEvent.flag.nValue & 0x01) == 0)
    {
        TextOut(hdc, nLeft2, nTop, "�쳣");
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��Ч��");
    if ((g_powerEvent.flag.nValue & 0x02) == 0)
    {
        TextOut(hdc, nLeft2, nTop, "��Ч");
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��Ч");
    }

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);
}

//ͣ�ϵ��¼�
PLUGIN_INFO g_layer_power_off_on_display = {
    0x11010021/*id*/,
    power_off_on_display_init/*InitLayer*/,
    power_off_on_display_Uninit/*UninitLayer*/,
    power_off_on_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�㶨�ų������¼�",1)

OOP_EVENT_T g_magneticEvent;
OOP_EVENT_T *g_magneticEventlist;
void magneticEvent_display_init(DWORD dwPluginId)
{
    g_magneticEventlist = NULL;
    get_event_data(g_gui_db_client, 0x302a0200, (uint8**)&g_magneticEventlist, sizeof(OOP_EVENT_T));
}
void magneticEvent_display_Uninit()
{
    if (g_magneticEventlist != NULL)
    {
        free(g_magneticEventlist);
        g_magneticEventlist = NULL;
    }
    
}

//�㶨�ų������¼�
void magneticEvent_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);

    char czBuf[64] = { 0 };

    int nLeft = 15;
    int nLeft2 = 85;
    int nTop = 22;
    int tmpTop = 25;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�ų������¼�");
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_magneticEvent, 0, sizeof(OOP_EVENT_T));
    if (g_magneticEventlist != NULL)
    {
        g_magneticEvent = g_magneticEventlist[g_event_index];
    }

    //�ų������¼�
    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "��¼���");
    sprintf(czBuf, "%u", g_magneticEvent.nIndex);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
    if (g_magneticEvent.tmStart.year == 0 && g_magneticEvent.tmStart.month == 0 && g_magneticEvent.nIndex == 0)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%04hu-%02hu-%02hu %02hu:%02hu:%02hu",
            g_magneticEvent.tmStart.year,
            g_magneticEvent.tmStart.month,
            g_magneticEvent.tmStart.day,
            g_magneticEvent.tmStart.hour,
            g_magneticEvent.tmStart.minute,
            g_magneticEvent.tmStart.second);
    }

    nTop += tmpTop;
    TextOut(hdc, 30, nTop, czBuf);

    
}

//�㶨�ų������¼�
PLUGIN_INFO g_layer_magneticEvent_display = {
    0x11010026/*id*/,
    magneticEvent_display_init/*InitLayer*/,
    magneticEvent_display_Uninit/*UninitLayer*/,
    magneticEvent_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("������բ�¼�",1)

//���ؼ�¼
OOP_PCTRLLOG_T *g_pctrllogEventlist;
OOP_PCTRLLOG_T g_pctrllogEvent;

void power_ctr_evt_init(DWORD dwPluginId)
{
    g_pctrllogEventlist = NULL;
    get_event_data(g_gui_db_client, 0x32000200, (uint8**)&g_pctrllogEventlist, sizeof(OOP_PCTRLLOG_T));
}
void power_ctr_evt_Uninit()
{
    if (g_pctrllogEventlist != NULL)
    {
        free(g_pctrllogEventlist);
        g_pctrllogEventlist = NULL;
    }
}

void power_ctr_evt_draw(HDC hdc)
{

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 10;
    int nLeft2 = 50;
    int nTop = 22;
    int tmpTop = 18;
    int i = 0;
    float tmp;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "���ؼ�¼%d", g_event_index+1);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_pctrllogEvent, 0, sizeof(OOP_PCTRLLOG_T));
    if (g_pctrllogEventlist != NULL)
    {
        g_pctrllogEvent = g_pctrllogEventlist[g_event_index];
    }

    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "�ִ�");

    if (g_pctrllogEvent.turnState.nValue != 0)
    {
        for (i = 0; i < 8; i++)
        {
            if ((g_pctrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf, "%d ", i + 1);
                break;
            }
        }
    }
    else
    {
        sprintf(czBuf, "--");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nLeft2 += 30;

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��բǰ����");
    if (g_pctrllogEvent.power == 0)
    {
        sprintf(czBuf, "-------- kw");
    }
    else
    {
        tmp = g_pctrllogEvent.power;
        sprintf(czBuf, "%6.4f kw", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��բ����");
    if (g_pctrllogEvent.power2Min == 0)
    {
        sprintf(czBuf, "-------- kw");
    }
    else
    {
        tmp = g_pctrllogEvent.power2Min;
        sprintf(czBuf, "%6.4f kw", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��ǰ��ֵ");
    if (g_pctrllogEvent.llSetting == 0)
    {
        sprintf(czBuf, "-------- kw");
    }
    else
    {
        tmp = g_pctrllogEvent.llSetting;
        sprintf(czBuf, "%6.4f kw", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;

    if (g_pctrllogEvent.tmStart.year == 0)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%hu��%hu��%hu��%hu:%hu",
            g_pctrllogEvent.tmStart.year,
            g_pctrllogEvent.tmStart.month,
            g_pctrllogEvent.tmStart.day,
            g_pctrllogEvent.tmStart.hour,
            g_pctrllogEvent.tmStart.minute);
    }
    TextOut(hdc, 30, nTop, czBuf);

}

//���ؼ�¼
PLUGIN_INFO g_layer_power_ctr_evt_display = {
    0x11010022/*id*/,
    power_ctr_evt_init/*InitLayer*/,
    power_ctr_evt_Uninit/*UninitLayer*/,
    power_ctr_evt_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�����բ�¼�",1)

//��ؼ�¼

OOP_ECTRLLOG_T *g_ectrllogEventlist;
OOP_ECTRLLOG_T g_ectrllogEvent;
void elect_ctr_evt_init(DWORD dwPluginId)
{
    g_ectrllogEventlist = NULL;
    get_event_data(g_gui_db_client, 0x32010200, (uint8**)&g_ectrllogEventlist, sizeof(OOP_ECTRLLOG_T));
}
void elect_ctr_evt_Uninit()
{
    if (g_ectrllogEventlist != NULL)
    {
        free(g_ectrllogEventlist);
        g_ectrllogEventlist = NULL;
    }
}

void elect_ctr_evt_draw(HDC hdc)
{

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 10;
    int nLeft2 = 50;
    int nTop = 22;
    int tmpTop = 16;
    int i = 0;
    float tmp;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "�����բ��¼%d", g_event_index+1);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_ectrllogEvent, 0, sizeof(OOP_ECTRLLOG_T));
    if (g_ectrllogEventlist != NULL)
    {
        g_ectrllogEvent = g_ectrllogEventlist[g_event_index];
    }

    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "�ִ�");

    if (g_ectrllogEvent.turnState.nValue != 0)
    {
        for (i = 0; i < 8; i++)
        {
            if ((g_ectrllogEvent.turnState.nValue >> i & 0x01) != 0)
            {
                sprintf(czBuf, "%d ", i + 1);
                break;
            }
        }
    }
    else
    {
        sprintf(czBuf, "--");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "��ض�ֵ");
    nTop += tmpTop;
    if (g_ectrllogEvent.llSetting == 0)
    {
        sprintf(czBuf, "-------- kWh");
    }
    else
    {
        tmp = g_ectrllogEvent.llSetting;
        sprintf(czBuf, "%12.4f kWh", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += tmpTop;
    if(g_ectrllogEvent.objCtrl == 0x8107)
    {
        /* ����� */
        TextOut(hdc, nLeft, nTop, "ʣ�������/��");
    }
    else
    {
        /* �µ�� */
        TextOut(hdc, nLeft, nTop, "�µ�����");
    }
    nTop += tmpTop;
    if (g_ectrllogEvent.energy == 0)
    {
        sprintf(czBuf, "--------kWh/Ԫ");
    }
    else
    {
        tmp = g_ectrllogEvent.energy;
        sprintf(czBuf, "%12.4fkWh/Ԫ", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += tmpTop;
    if (g_ectrllogEvent.tmStart.year == 0)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%hu��%hu��%hu��%hu:%hu",
            g_ectrllogEvent.tmStart.year,
            g_ectrllogEvent.tmStart.month,
            g_ectrllogEvent.tmStart.day,
            g_ectrllogEvent.tmStart.hour,
            g_ectrllogEvent.tmStart.minute);
    }
    TextOut(hdc, 30, nTop, czBuf);

}

//��ؼ�¼
PLUGIN_INFO g_layer_elect_ctr_evt_display = {
    0x11010023/*id*/,
    elect_ctr_evt_init/*InitLayer*/,
    elect_ctr_evt_Uninit/*UninitLayer*/,
    elect_ctr_evt_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("������������¼�",1)

OOP_EVENT_T *g_buy_elect_Eventlist;
OOP_EVENT_T g_buy_elect_Event;
void buy_elect_evt_init(DWORD dwPluginId)
{
    g_buy_elect_Eventlist = NULL;
    get_event_data(g_gui_db_client, 0x32020200, (uint8**)&g_buy_elect_Eventlist, sizeof(OOP_EVENT_T));
}
void buy_elect_evt_Uninit()
{
    if (g_buy_elect_Eventlist != NULL)
    {
        free(g_buy_elect_Eventlist);
        g_buy_elect_Eventlist = NULL;
    }
}

void buy_elect_evt_draw(HDC hdc)
{

    BOOL NoData = TRUE;
    OOP_CFGUNIT_810C_T cfgunit;

    memset(&g_buy_elect_Event, 0, sizeof(OOP_EVENT_T));
    if (g_buy_elect_Eventlist != NULL)
    {
        g_buy_elect_Event = g_buy_elect_Eventlist[g_event_index];
    }

    if (g_buy_elect_Event.data.oadNum == 1 && g_buy_elect_Event.data.cols.oad[0].value == 0X810C2200)
    {
        NoData = FALSE;
        memcpy(&cfgunit, g_buy_elect_Event.data.buf, sizeof(OOP_CFGUNIT_810C_T));
    }

    uint16 oiobject;

    int index = -1;
    for (uint32 i = 0; i < 8; i++)
    {
        oiobject = 0x2301 + i;
        if (g_buy_elect_Event.source.oi == oiobject)
        {
            index = i;
            break;
        }
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 10;
    int nLeft2 = 40;
    int nTop = 22;
    int tmpTop = 18;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "����������ü�¼%d", g_event_index+1);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "����ǰ�ܼ������");
    nTop += tmpTop;
    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "--------");
    }
    else
    {
        sprintf(czBuf, "%04x", cfgunit.object);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ǰ���絥��");
    nTop += tmpTop;
    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "--------");
    }
    else
    {
        sprintf(czBuf, "%08d", cfgunit.number);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += tmpTop;

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%hu��%hu��%hu��%hu:%hu",
            g_buy_elect_Event.tmStart.year,
            g_buy_elect_Event.tmStart.month,
            g_buy_elect_Event.tmStart.day,
            g_buy_elect_Event.tmStart.hour,
            g_buy_elect_Event.tmStart.minute);
    }
    TextOut(hdc, 30, nTop, czBuf);

}

//����������ü�¼
PLUGIN_INFO g_layer_buy_elect_evt_display = {
    0x11010024/*id*/,
    buy_elect_evt_init/*InitLayer*/,
    buy_elect_evt_Uninit/*UninitLayer*/,
    buy_elect_evt_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��ظ澯�¼�",1)

//��ظ澯��¼
OOP_EWARNLOG_T *g_ewarnEventlist;
OOP_EWARNLOG_T g_ewarnEvent;
void elect_warn_evt_init(DWORD dwPluginId)
{
    g_ewarnEventlist = NULL;
    get_event_data(g_gui_db_client, 0x32030200, (uint8**)&g_ewarnEventlist, sizeof(OOP_EWARNLOG_T));
}
void elect_warn_evt_Uninit()
{
    if (g_ewarnEventlist != NULL)
    {
        free(g_ewarnEventlist);
        g_ewarnEventlist = NULL;
    }
}

void elect_warn_evt_draw(HDC hdc)
{

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    char czBuf[64] = { 0 };

    int nLeft = 10;
    int nLeft2 = 50;
    int nTop = 22;
    int tmpTop = 18;
    float tmp;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;
    rc.top = nTop;
    rc.bottom = rc.top + 16;
    sprintf(czBuf, "��ظ澯��¼%d", g_event_index+1);
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    memset(&g_ewarnEvent, 0, sizeof(OOP_EVENT_T));
    if (g_ewarnEventlist != NULL)
    {
        g_ewarnEvent = g_ewarnEventlist[g_event_index];
    }
    
    nTop += tmpTop + 10;
    TextOut(hdc, nLeft, nTop, "�澯ʱ��������ֵ");
    nTop += tmpTop + 10;
    if (g_ewarnEvent.llSetting == 0)
    {
        sprintf(czBuf, "-------- kWh");
    }
    else
    {
        tmp = g_ewarnEvent.llSetting;
        sprintf(czBuf, "%12.4f kWh", tmp / 10000);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);


    nTop += tmpTop;

    if (g_ectrllogEvent.tmStart.year == 0)
    {
        sprintf(czBuf, "--��--��--��--:--");
    }
    else
    {
        sprintf(czBuf, "%hu��%hu��%hu��%hu:%hu",
            g_ectrllogEvent.tmStart.year,
            g_ectrllogEvent.tmStart.month,
            g_ectrllogEvent.tmStart.day,
            g_ectrllogEvent.tmStart.hour,
            g_ectrllogEvent.tmStart.minute);
    }
    TextOut(hdc, 30, nTop, czBuf);

}

//��ظ澯��¼
PLUGIN_INFO g_layer_elect_warn_evt_display = {
    0x11010025/*id*/,
    elect_warn_evt_init/*InitLayer*/,
    elect_warn_evt_Uninit/*UninitLayer*/,
    elect_warn_evt_draw/*Draw*/,
    NULL/*Timer*/,
    event_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif


uint32 g_display_info_index = 0;
void chinese_info_display_init(DWORD dwPluginId)
{
    memset(&display_data, 0, sizeof(display_data));
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;
    oadvalue = 0x80030200 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&display_data.chinese_info, sizeof(OOP_CHINESEINFOS_T));

    g_display_info_index = 0;
}

void chinese_info_display_draw(HDC hdc)
{
    char czBuf[300] = {0};
    char temp[30] = {0};
    int  top = 17;
    
    if(g_display_info_index >= OOP_MAX_CHINESEINFO)
    {
        g_display_info_index = 0;
    }
    CHINESEINFO *pInfo = &(display_data.chinese_info.item[g_display_info_index]);

    SelectFont(hdc, g_guiComm.fontBig);

    sprintf(temp, "%s", (g_guiComm.nCurPoint == 0) ? "һ��������Ϣ" : "��Ҫ������Ϣ");
    TextOut(hdc, 32, top, temp);
    
    if(0 == display_data.chinese_info.nNum)
    {
        top = 36;
        TextOut(hdc, 10, top, "����Ϣ");
        return;
    }

    if(pInfo->info.nNum < 100)
    {
        SelectFont(hdc, g_guiComm.fontBig);
    }
    else
    {
        SelectFont(hdc, g_guiComm.fontSmall);
    }
    RECT rc;
    rc.left = 0;
    rc.top = 36;
    rc.right = 159;
    rc.bottom = 130;

    memcpy(czBuf, pInfo->info.value, pInfo->info.nNum);
    DrawText(hdc, czBuf, -1, &rc, DT_LEFT);

    SelectFont(hdc, g_guiComm.fontSmall);
    sprintf(temp, "[%d] %04d-%02d-%02d %02d:%02d:%02d", pInfo->index, pInfo->time.year, pInfo->time.month,
                      pInfo->time.day, pInfo->time.hour, pInfo->time.minute, pInfo->time.second);
    top = 131;
    TextOut(hdc, 5, top, temp);

    if(FALSE == pInfo->bRead)
    {
        OOP_OAD_U oadvalue;
        
        pInfo->bRead = TRUE;
        oadvalue.value = 0x80030200 + (0x00010000) * g_guiComm.nCurPoint;
        write_normal_data((uint8*)&display_data.chinese_info, sizeof(OOP_CHINESEINFOS_T), oadvalue, 0, 0, CLASS_DATA_INIT);
    }
}

BOOL chinese_info_keymsg(int nKey)
{
    switch (nKey)
    {
        case KEY_DOWN:
        {
            if(g_display_info_index + 1 < display_data.chinese_info.nNum)
            {
                g_display_info_index++;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return  TRUE;

        }
        case KEY_UP:
        {
            if (g_display_info_index > 0)
            {
                g_display_info_index--;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        default:
            return FALSE;
    }

}


//һ��������Ϣ
PLUGIN_INFO g_layer_chinese_info_display1 = {
    0x11030060/*id*/,
    chinese_info_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    chinese_info_display_draw/*Draw*/,
    NULL/*Timer*/,
    chinese_info_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//��Ҫ������Ϣ
PLUGIN_INFO g_layer_chinese_info_display2 = {
    0x11030061/*id*/,
    chinese_info_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    chinese_info_display_draw/*Draw*/,
    NULL/*Timer*/,
    chinese_info_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void plus_param_init(DWORD dwPluginId)
{
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;
    oadvalue = 0x24010400 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].plus_param, sizeof(OOP_PULSESETS_T));
    oadvalue = 0x24011C00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].plus_b_param, sizeof(OOP_PULSESETS_B_T));

    oadvalue = 0x24010200 + (0x00010000) * g_guiComm.nCurPoint;
    g_ctrl_data[g_guiComm.nCurPoint].pulse_addr.nNum = 0;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].pulse_addr, sizeof(OOP_OCTETVAR16_T));

    g_ac_data_display_drawpage_index = 0;
}

void plus_param_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    char czBuf[300];
    uint16 offset = 0;
    uint8  i = 0;
    uint32 K = 0;
    offset = sprintf(czBuf, "���� %d ", g_guiComm.nCurPoint + 1);
    if (g_ctrl_data[g_guiComm.nCurPoint].pulse_addr.nNum > 0)
    {
        for (i = 0; i < g_ctrl_data[g_guiComm.nCurPoint].pulse_addr.nNum; i++)
        {
            offset += sprintf(czBuf + offset, "%02X", g_ctrl_data[g_guiComm.nCurPoint].pulse_addr.value[i]);
        }
    }
    else
    {
        offset += sprintf(czBuf + offset, "------------");
    }

    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    int nTop = rc.top;
    int nLeft1 = 5;
    int nLeft2 = 70;
    uint32 index = 0;

    for (index = (uint32)g_ac_data_display_drawpage_index + 1; index < (uint32)g_ac_data_display_drawpage_index + 3; index++)
    {
        nTop += 18;
        sprintf(czBuf, "����˿�%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].plus_param.nNum < index)
        {
            sprintf(czBuf, " ---------- ");
        }
        else
        {
            sprintf(czBuf, "%08X", g_ctrl_data[g_guiComm.nCurPoint].plus_param.pulse[index - 1].port.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 18;
        sprintf(czBuf, "��������%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].plus_param.nNum < index)
        {
            sprintf(czBuf, " ---------- ");
        }
        else
        {
            sprintf(czBuf, "%s", g_pczPulseAttr[g_ctrl_data[g_guiComm.nCurPoint].plus_param.pulse[index - 1].property]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 18;
        sprintf(czBuf, "���峣��k%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].plus_param.nNum < index)
        {
            sprintf(czBuf, " ---------- ");
        }
        else
        {
            K = g_ctrl_data[g_guiComm.nCurPoint].plus_param.pulse[index - 1].K;
            if(0 != g_ctrl_data[g_guiComm.nCurPoint].plus_param.rsv)
            {
                K = g_ctrl_data[g_guiComm.nCurPoint].plus_b_param.pulse[index - 1].K;
            }
            sprintf(czBuf, "%u", K);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

BOOL plus_param_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_RIGHT:
    {
        g_ac_data_display_drawpage_index += 2;
        if (g_ac_data_display_drawpage_index > 2)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        g_ac_data_display_drawpage_index -= 2;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 2;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//��������
PLUGIN_INFO g_layer_plus_param1 = {
    0x11030020/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param2 = {
    0x11030021/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param3 = {
    0x11030022/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param4 = {
    0x11030023/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param5 = {
    0x11030024/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param6 = {
    0x11030025/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param7 = {
    0x11030026/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_plus_param8 = {
    0x11030027/*id*/,
    plus_param_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    plus_param_draw/*Draw*/,
    NULL/*Timer*/,
    plus_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void gruop_param_init(DWORD dwPluginId)
{
    memset(g_ctrl_data, 0, sizeof(g_ctrl_data));
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;

    advance_gui_infonum(g_guiComm.nCurPoint + 1);
    oadvalue = 0x23010200 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].gruop_param, sizeof(OOP_TGSETS_T));
    g_ac_data_display_drawpage_index = 0;
}

void gruop_param_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    char czBuf[300];
    uint16 offset = 0;
    sprintf(czBuf, "�ܼ���%d ����", g_guiComm.nCurPoint + 1);

    rc.top = 21;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, g_guiComm.fontSmall);

    int nTop = rc.top;
    int nLeft1 = 5;
    int nLeft2 = 70;
    uint32 index = 0;
    int i = 0;
    for (index = (uint32)g_ac_data_display_drawpage_index + 1; index < (uint32)g_ac_data_display_drawpage_index + 3; index++)
    {
        nTop += 18;
        offset = sprintf(czBuf, "��·��ַ%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].gruop_param.tg[index - 1].add.len <= 0)
        {
            sprintf(czBuf, " ---------- ");
        }
        else
        {
            offset = 0;
            memset(czBuf, 0, offset);
            for (i = 0; i <= g_ctrl_data[g_guiComm.nCurPoint].gruop_param.tg[index - 1].add.len; i++)
            {
                offset += sprintf(czBuf + offset, "%02X", g_ctrl_data[g_guiComm.nCurPoint].gruop_param.tg[index - 1].add.add[i]);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 18;
        sprintf(czBuf, "����%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].gruop_param.nNum < index)
        {
            sprintf(czBuf, "%s", g_pczDirect[0]);
        }
        else
        {
            sprintf(czBuf, "%s", g_pczDirect[g_ctrl_data[g_guiComm.nCurPoint].gruop_param.tg[index - 1].tgFlag + 1]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 18;
        sprintf(czBuf, "�ۼӱ�־%d", index);
        TextOut(hdc, nLeft1, nTop, czBuf);
        if (g_ctrl_data[g_guiComm.nCurPoint].gruop_param.nNum < index)
        {
            sprintf(czBuf, "%s", g_pczOperator[0]);
        }
        else
        {
            sprintf(czBuf, "%s", g_pczOperator[g_ctrl_data[g_guiComm.nCurPoint].gruop_param.tg[index - 1].opFlag + 1]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

}

BOOL gruop_param_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_RIGHT:
    {
        g_ac_data_display_drawpage_index += 2;
        if (g_ac_data_display_drawpage_index > 14)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        g_ac_data_display_drawpage_index -= 2;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 14;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}

//�ܼ�������
PLUGIN_INFO g_layer_gruop_param1 = {
    0x11030030/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param2 = {
    0x11030031/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param3 = {
    0x11030032/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param4 = {
    0x11030033/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param5 = {
    0x11030034/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param6 = {
    0x11030035/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param7 = {
    0x11030036/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_layer_gruop_param8 = {
    0x11030037/*id*/,
    gruop_param_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    gruop_param_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

/**********************************************************************************
���Ӻ���������
�Ӻ���: MakeFormatedStringOfEnergy()
����������������������ת��Ϊ��kWh��MWhΪ��λ���ַ���
���������sint64 energy ������
          size_t n ����ַ�������������󳤶�(ȱʡ����¼ٶ�������������30�ֽڿռ����)
���������char *str ����ַ���������,
��������ֵ����
�����÷�������
    char data[100];
    char str[50];
    if (ReadMtrXData(GROUP_OFFSET+g_guiComm.nCurPoint, RT_DATA, VIR_BUY_EN_CTRL_INFO, data) > 0)
    {
        MakeFormatedStringOfEnergy(chartoint64(data), str, sizeof(str));
        TextOut(hdc, x, y, str);
    }
    ����ɲμ�BuyDataDraw()�ж�MakeFormatedStringOfEnergy()�ĵ���
**********************************************************************************/
static void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n = 30);

/*******************************************************************
�������ܵ�Ԫ����̬ˢ��ҳ����ƺ���
�������������ƹ�����Ϣ
�����������
�����������
��������ֵ����
�ڲ��������̣�
�����ɳ�ʼ���溯����ÿ������g_guiComm.nCurPoint,����4��
����������
********************************************************************/
void BuyDataDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[100];
    int nLeft1 = 4;
    int nLeft2 = 56;
    int nTop = 38;
    int nTempTop = 13;
    uint16 oiobject = 0x2301 + g_guiComm.nCurPoint;

    BOOL NoData = FALSE;
    if (tBuyenergyconfig.nNum == 0)
    {
        NoData = TRUE;
    }

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "������� �ܼ���%d", g_guiComm.nCurPoint + 1);
    rc.top = 22;
    rc.bottom = rc.top + 14;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    int index = -1;
    for (uint32 i = 0; i < tBuyenergyconfig.nNum; i++)
    {
        if (tBuyenergyconfig.item[i].object == oiobject)
        {
            index = i;
            break;
        }
    }

    int isfee = 0;
    if (NoData == FALSE || index >= 0)
    {
        if (tBuyenergyconfig.item[index].type == 0)
            isfee = 0;
        else
            isfee = 1;
    }
    else
        isfee = 0;

    TextOut(hdc, nLeft1, nTop, "���絥��");

    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "--------");
    }
    else
    {
        sprintf(czBuf, "%08d", tBuyenergyconfig.item[index].number);
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "���緽ʽ");

    if (NoData == TRUE || index == -1)
    {
        TextOut(hdc, nLeft2, nTop, "----");
    }
    else
    {
        if (tBuyenergyconfig.item[index].flag == 0)
        {
            TextOut(hdc, nLeft2, nTop, "׷��");
        }
        else if (tBuyenergyconfig.item[index].flag == 1)
        {
            TextOut(hdc, nLeft2, nTop, "ˢ��");
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "��ǰ����");
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyLastData)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", g_GuiBuyLast * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyLast, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "������");
    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", tBuyenergyconfig.item[index].purElectricity * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].purElectricity, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "�������");
    if (NoData == TRUE || index == -1 || FALSE == g_bGuiBuyEndData)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", g_GuiBuyEnd * 0.0001);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            MakeFormatedStringOfEnergy(g_GuiBuyEnd, czBuf);
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "��������");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", tBuyenergyconfig.item[index].alarmLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].alarmLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += nTempTop;
    TextOut(hdc, nLeft1, nTop, "��բ����");

    if (NoData == TRUE || index == -1)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", tBuyenergyconfig.item[index].tripLimit * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy(tBuyenergyconfig.item[index].tripLimit, czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    nTop += nTempTop;
    if (isfee == 1)
        TextOut(hdc, nLeft1, nTop, "ʣ����");
    else
        TextOut(hdc, nLeft1, nTop, "ʣ�����");
    if (NoData == TRUE || index == -1 || g_wallet[g_guiComm.nCurPoint] == 0xffffffff)
    {
        sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    else
    {
        if (isfee == 1)
        {
            sprintf(czBuf, "%12.4f Ԫ", (double)g_wallet[g_guiComm.nCurPoint] * 0.0001);
        }
        else
        {
            MakeFormatedStringOfEnergy((double)g_wallet[g_guiComm.nCurPoint], czBuf);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

void BuyDataInit(DWORD dwPluginID)
{
    g_guiComm.nCurPoint = dwPluginID & 0x0f;
    advance_gui_infonum(g_guiComm.nCurPoint + 1);

    memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
    read_db_noamal_data(g_gui_db_client, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));
    int oadvalue;

    oadvalue = 0x23010b00 + (0x00010000) * g_guiComm.nCurPoint;
    memset(g_wallet + g_guiComm.nCurPoint, 0xff, sizeof(int64));
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)g_wallet + g_guiComm.nCurPoint, sizeof(int64));

    g_bGuiBuyLastData = FALSE;
    if (0 == read_db_noamal_data(g_gui_db_client, 0x810703ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyLast, sizeof(int64)))
    {
        g_bGuiBuyLastData = TRUE;
    }
    g_bGuiBuyEndData = FALSE;
    if (0 == read_db_noamal_data(g_gui_db_client, 0x810704ff, 0, g_guiComm.nCurPoint + 1, (uint8*)&g_GuiBuyEnd, sizeof(int64)))
    {
        g_bGuiBuyEndData = TRUE;
    }
}
void BuyDataCleanup(void)
{
    advance_gui_infonum(0);
    UninitDefault();
}

PLUGIN_INFO g_BuyDatadisplay1 = {
    0x11030040,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay2 = {
    0x11030041,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay3 = {
    0x11030042,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay4 = {
    0x11030043,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay5 = {
    0x11030044,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay6 = {
    0x11030045,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay7 = {
    0x11030046,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};
PLUGIN_INFO g_BuyDatadisplay8 = {
    0x11030047,
    BuyDataInit,
    BuyDataCleanup,
    BuyDataDraw,
    NULL,
    NULL,
    NULL
};

#include <math.h>
//�Ӻ���: MakeFormatedStringOfEnergy()
static
void MakeFormatedStringOfEnergy(int64 energy, char* str, size_t n)
{
    double val_kWh;
    double val_MWh;

    if (str == NULL || n <= 0)
    {
        return;
    }

    val_kWh = energy * 0.0001;//�����ֵ���Լ���ڲ�������10000���洢������
    if (fabs(val_kWh) < 10000000.0)
    {
        snprintf(str, n - 1, "%12.4f kWh", val_kWh);
    }
    else
    {
        val_MWh = val_kWh / 1000.0;
        snprintf(str, n - 1, "%12.2f MWh", val_MWh);
    }
    str[n - 1] = '\0';
}

/************************************************************************/
/* �ն�ʱ������                                                         */
/************************************************************************/
typedef struct _RESET_TIME
{
    int     nIndex;     // ��ǰ����
    char    czInfo[16]; // ��ǰʱ���ַ���������:

    struct tm   tmNow;

}RESET_TIME;
RESET_TIME g_reset_time;

void reset_time_Init(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_reset_time.nIndex = 0;

    // ��ȡ��ǰʱ��
    long lTime = time(NULL);
    tm* tnow = localtime(&lTime);
    memcpy(&g_reset_time.tmNow, tnow, sizeof(tm));


    sprintf(g_reset_time.czInfo, "%04d%02d%02d%02d:%02d:%02d",
        g_reset_time.tmNow.tm_year + 1900,
        g_reset_time.tmNow.tm_mon + 1,
        g_reset_time.tmNow.tm_mday,
        g_reset_time.tmNow.tm_hour,
        g_reset_time.tmNow.tm_min,
        g_reset_time.tmNow.tm_sec);
}
void reset_time_Draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 30;
    rc.bottom = rc.top + 16;
    char pcz[20];

    strcpy(pcz, "�ն�ʱ������");

    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    // ��������
    int nLeft = 24;
    int nTop = rc.top + 30;
    int i;
    for (i = 0; i < 8; i++)
    {
        switch (i)
        {
        case 4:TextOut(hdc, nLeft, nTop, "��"); nLeft += 16; break;
        case 6:TextOut(hdc, nLeft, nTop, "��"); nLeft += 16; break;
        }

        if (g_reset_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
        }
        nLeft += 8;
    }
    TextOut(hdc, nLeft, nTop, "��");

    // ����ʱ��
    nLeft = 48;
    nTop += 20;
    for (; i < 16; i++)
    {
        if (g_reset_time.nIndex == i)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_reset_time.czInfo[i], 1);
        }
        nLeft += 8;
    }

    // ȷ��
    nTop += 20;
    nLeft = 68;
#if MODULARIZATION_TMN_EN == 1
    if (g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft - 27, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft - 27, nTop, "����");
    }
    i++;
    if (g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft + 20, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft + 20, nTop, "ȷ��");
    }
#else
    if (g_reset_time.nIndex == i)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }
#endif
}

BOOL reset_time_KeyMsg(int nKey)
{
    int iRet = 0;

#if MODULARIZATION_TMN_EN == 1
    int max = 17;
#else
    int max = 16;
#endif

    switch (nKey)
    {
    case KEY_LEFT:// �޸����뽹��
        g_reset_time.nIndex--;
        if ((g_reset_time.czInfo[g_reset_time.nIndex] < '0'
            || g_reset_time.czInfo[g_reset_time.nIndex] > '9')
#if MODULARIZATION_TMN_EN == 1
            && g_reset_time.nIndex != 16
#endif
            )
        {
            g_reset_time.nIndex--;
        }
        if (g_reset_time.nIndex < 0)
        {
            g_reset_time.nIndex = max;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// �޸����뽹��
        g_reset_time.nIndex++;
        if ((g_reset_time.czInfo[g_reset_time.nIndex] < '0'
            || g_reset_time.czInfo[g_reset_time.nIndex] > '9')
#if MODULARIZATION_TMN_EN == 1
            && g_reset_time.nIndex != 17
#endif
            && g_reset_time.nIndex != 16)
        {
            g_reset_time.nIndex++;
        }
        if (g_reset_time.nIndex > max)
        {
            g_reset_time.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_UP:// �޸�ʱ����������ж���Чλ����һЩ�ж�
        if (g_reset_time.nIndex >= 0 && g_reset_time.nIndex <= 15)
        {
            char czMax = '9';
            switch (g_reset_time.nIndex)
            {
            case 0:czMax = '2'; break;  // ��ǧλ
            case 4:czMax = '1'; break;  // ��ʮλ
            case 6:czMax = '3'; break;  // ��ʮλ
            case 8:czMax = '2'; break;  // Сʱʮλ
            case 11:czMax = '5'; break; // ��ʮλ
            case 14:czMax = '5'; break; // ��ʮλ
            }
            g_reset_time.czInfo[g_reset_time.nIndex] ++;
            if (g_reset_time.czInfo[g_reset_time.nIndex] > czMax)
            {
                if (g_reset_time.nIndex == 0)
                    g_reset_time.czInfo[g_reset_time.nIndex] = '1';
                else
                    g_reset_time.czInfo[g_reset_time.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_DOWN:// �޸�ʱ����������ж���Чλ����һЩ�ж�
        if (g_reset_time.nIndex >= 0 && g_reset_time.nIndex <= 15)
        {
            char czMax = '9';
            switch (g_reset_time.nIndex)
            {
            case 0:czMax = '2'; break;  // ��ǧλ
            case 4:czMax = '1'; break;  // ��ʮλ
            case 6:czMax = '3'; break;  // ��ʮλ
            case 8:czMax = '2'; break;  // Сʱʮλ
            case 11:czMax = '5'; break; // ��ʮλ
            case 14:czMax = '5'; break; // ��ʮλ
            }
            g_reset_time.czInfo[g_reset_time.nIndex] --;
            if (g_reset_time.czInfo[g_reset_time.nIndex] < '0')
            {
                g_reset_time.czInfo[g_reset_time.nIndex] = czMax;
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
        break;
    case KEY_OK:
#if MODULARIZATION_TMN_EN == 1
        if (g_reset_time.nIndex == 16)
        {
            g_guiComm.bOKiscancel = 1;
            break;
        }
        if (g_reset_time.nIndex == 17 && ValidatePwd() == TRUE)
#else
        if (g_reset_time.nIndex == 16 && ValidatePwd() == TRUE)
#endif
        {
            char czBuf[20];
            int nVal;
            // ��
            memcpy(czBuf, g_reset_time.czInfo, 4);
            czBuf[4] = '\0';
            nVal = atoi(czBuf);
            if (nVal > 2099)
            {
                g_reset_time.nIndex = 3;
                MessageBox("���ֵ���ܳ���2099��\n\n���������룡", MB_OK);
                return TRUE;
            }
            if (nVal < 2000)
            {
                g_reset_time.nIndex = 3;
                MessageBox("���ֵ���ܵ���2000��\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_year = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo + 4, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal == 0)
            {
                g_reset_time.nIndex = 5;
                MessageBox("�·�ֵ����Ϊ���·�\n\n���������룡", MB_OK);
                return TRUE;
            }
            if (nVal > 12)
            {
                g_reset_time.nIndex = 5;
                MessageBox("�·�ֵ���ܴ���12�·�\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_mon = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo + 6, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal == 0)
            {
                g_reset_time.nIndex = 7;
                MessageBox("����ֵ����Ϊ��\n\n���������룡", MB_OK);
                return TRUE;
            }
            if (nVal > 31)
            {
                g_reset_time.nIndex = 7;
                MessageBox("����ֵ������Χ\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_mday = nVal;

            // Сʱ
            memcpy(czBuf, g_reset_time.czInfo + 8, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            if (nVal > 23)
            {
                g_reset_time.nIndex = 9;
                MessageBox("Сʱֵ������Χ\n\n���������룡", MB_OK);
                return TRUE;
            }
            g_reset_time.tmNow.tm_hour = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo + 11, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_reset_time.tmNow.tm_min = nVal;

            // ��
            memcpy(czBuf, g_reset_time.czInfo + 14, 2);
            czBuf[2] = '\0';
            nVal = atoi(czBuf);
            g_reset_time.tmNow.tm_sec = nVal;



            DateTimeHex_t pDateTime;
            memset(&pDateTime, 0, sizeof(DateTimeHex_t));

            pDateTime.year_h = g_reset_time.tmNow.tm_year >> 8;
            pDateTime.year_l = g_reset_time.tmNow.tm_year & 0xff;
            pDateTime.month = g_reset_time.tmNow.tm_mon;
            pDateTime.day = g_reset_time.tmNow.tm_mday;
            pDateTime.hour = g_reset_time.tmNow.tm_hour;
            pDateTime.min = g_reset_time.tmNow.tm_min;
            pDateTime.sec = g_reset_time.tmNow.tm_sec;
            g_reset_time.nIndex = 0;

            int year = 0;
            year = (uint16)pDateTime.year_l + (uint16)(pDateTime.year_h << 8);

            GUI_FMT_TRACE("set time modify time: %d-%d-%d %d:%d:%d]\n", year, pDateTime.month, pDateTime.day, pDateTime.hour, pDateTime.min, pDateTime.sec);
            iRet = systime_set_to_smiOS(&pDateTime);
            if (iRet < 0)
            {
                MessageBox("�ն�ʱ������ʧ�ܣ�", MB_OK);
                return FALSE;
            }

            MessageBox("�ն�ʱ�����óɹ���", MB_OK);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
PLUGIN_INFO g_layer_reset_time = {
    0x11030014/*id*/,
    reset_time_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reset_time_Draw/*Draw*/,
    NULL/*Timer*/,
    reset_time_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};
/************************************************************************/
/* �ն�ʱ�����ý���                                                     */
/************************************************************************/


typedef struct _CHANGE_PWD
{
    int     nIndex;     // ��ǰ����
    char    czPwd[PASSWORD_LEN * 3 + 5]; // ������
}CHANGE_PWD;
CHANGE_PWD g_change_pwd;
void ChangePwdInit(DWORD dwPluginId)
{
    g_guiComm.bCanTrunDisp = FALSE;
    g_change_pwd.nIndex = PASSWORD_LEN;
    memcpy(g_change_pwd.czPwd, "000000000000000000000", sizeof(g_change_pwd.czPwd));
    g_change_pwd.czPwd[PASSWORD_LEN] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN * 2 + 1] = '\0';
    g_change_pwd.czPwd[PASSWORD_LEN * 3 + 2] = '\0';
}
void ChangePwdDraw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    TextOut(hdc, 50, 20, "Ӧ�ò˵���������");


    int nLeft = 24;
    int nTop = 38;
    TextOut(hdc, 10, nTop, "�����������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    // ������
    nLeft += 2;
    uint32 i = 0;
    for (i = 0; i < PASSWORD_LEN; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��ϸ");
    }
    i++;

    // ������
    nLeft = 24;
    nTop += 17;
    TextOut(hdc, 10, nTop, "������������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    nLeft += 2;
    for (; i < PASSWORD_LEN * 2 + 1; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��ϸ");
    }
    i++;

    // ��֤����
    nLeft = 24;

    nTop += 17;
    TextOut(hdc, 10, nTop, "���ٴ�����������");
    nTop += 15;
    Rectangle(hdc, nLeft, nTop, nLeft + 2 + 8 * PASSWORD_LEN + 2, nTop + 16);
    nTop += 2;

    nLeft += 2;
    for (; i < PASSWORD_LEN * 3 + 2; i++)
    {
        if (i == (uint32)g_change_pwd.nIndex)
        {
            ITEM_SELECT;
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOutLen(hdc, nLeft, nTop, (const char*)&g_change_pwd.czPwd[i], 1);
        }
        nLeft += 8;
    }

    nLeft += 8;
    if (i == (uint32)g_change_pwd.nIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "��ϸ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "��ϸ");
    }
    i++;

    // ȷ��
    nLeft += 26;
    if ((uint32)g_change_pwd.nIndex == PASSWORD_LEN * 3 + 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    nLeft += 26;
    if ((uint32)g_change_pwd.nIndex == PASSWORD_LEN * 3 + 4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "����");
    }

}

// ��������ҳ�淵�ش���
void ChangePwdReturn(char* pNewStr, void* pPara)
{
    if (strlen(pNewStr) != PASSWORD_LEN)
    {
        char msg[64];
        sprintf(msg, "���볤�ȱ���Ϊ%d�ֽ�\n\nȷ���Ƿ���������?", PASSWORD_LEN);
        if (TRUE == MessageBox(msg, MB_OKCANCEL))
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
    }
    else
    {
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            strcpy(g_change_pwd.czPwd, pNewStr); break;
        case PASSWORD_LEN * 2 + 1:
            strcpy(g_change_pwd.czPwd + PASSWORD_LEN + 1, pNewStr); break;
        case PASSWORD_LEN * 3 + 2:
            strcpy(g_change_pwd.czPwd + (PASSWORD_LEN + 1) * 2, pNewStr); break;
        }
    }
}

BOOL ChangePwdKeyMsg(int nKey)
{
    switch (nKey)
    {
    case KEY_UP:// �����ڼ�����ϸ�������ƶ�
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 4;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 2) + 1:
            g_change_pwd.nIndex = PASSWORD_LEN;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 2:
            g_change_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 3:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;

        case (PASSWORD_LEN * 3) + 4:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 3;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] ++;
            if (g_change_pwd.czPwd[g_change_pwd.nIndex] > '9')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '0';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_DOWN:// �����ڼ�����ϸ�������ƶ�
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
            g_change_pwd.nIndex = (PASSWORD_LEN * 2) + 1;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 2) + 1:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 2;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 2:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 3;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 3:
            g_change_pwd.nIndex = (PASSWORD_LEN * 3) + 4;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        case (PASSWORD_LEN * 3) + 4:
            g_change_pwd.nIndex = PASSWORD_LEN;
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        default:
            g_change_pwd.czPwd[g_change_pwd.nIndex] --;
            if (g_change_pwd.czPwd[g_change_pwd.nIndex] < '0')
            {
                g_change_pwd.czPwd[g_change_pwd.nIndex] = '9';
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_LEFT:// �޸�����
        g_change_pwd.nIndex--;
        if (g_change_pwd.nIndex < 0)
        {
#if MODULARIZATION_TMN_EN == 1
            g_change_pwd.nIndex = PASSWORD_LEN * 3 + 4;
#else
            g_change_pwd.nIndex = PASSWORD_LEN * 3 + 3;
#endif
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:// �޸�����
        g_change_pwd.nIndex++;
        if ((uint32)g_change_pwd.nIndex > PASSWORD_LEN * 3 + 4)
        {
            g_change_pwd.nIndex = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        // ������ϸ��������������ҳ��
        switch (g_change_pwd.nIndex)
        {
        case PASSWORD_LEN:
        {
            InputBox(g_change_pwd.czPwd, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 2) + 1:
        {
            char* pBuf = g_change_pwd.czPwd;
            InputBox(pBuf + PASSWORD_LEN + 1, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 3) + 2:
        {
            char* pBuf = g_change_pwd.czPwd;
            InputBox(pBuf + (PASSWORD_LEN * 2) + 2, ChangePwdReturn);
        }
        break;
        case (PASSWORD_LEN * 3) + 4:
        {
            g_guiComm.bOKiscancel = 1;
            break;
        }
        }
        // �޸�����
        if (g_change_pwd.nIndex == PASSWORD_LEN * 3 + 3)
        {
            // �������ֵ��ж�ȡ
            char czPwd[PASSWORD_LEN * 3 + 5];
#ifdef _MSC_VER
            memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN + 1);
#else
            int nRet = read_pdata_xram(GUI_PRIVATE_DATA, czPwd, PRIVATE_PASSWORD, PASSWORD_LEN);
            if (nRet < 0)
            {
                memcpy(czPwd, PASSWORD_STR, PASSWORD_LEN + 1);
            }
#endif

            // �ȽϾ�����
            if (memcmp(czPwd, g_change_pwd.czPwd, PASSWORD_LEN) != 0)
            {
                MessageBox("��ʾ\n�������������");
                return TRUE;
            }
            // �Ƚ�����������
            if (memcmp(g_change_pwd.czPwd + PASSWORD_LEN + 1,
                g_change_pwd.czPwd + PASSWORD_LEN * 2 + 2, PASSWORD_LEN) != 0)
            {
                MessageBox("��ʾ\n�����������벻һ�£�");
                return TRUE;
            }
#ifndef _MSC_VER
            // ��������
            nRet = write_pdata_xram(GUI_PRIVATE_DATA, g_change_pwd.czPwd + PASSWORD_LEN + 1, PRIVATE_PASSWORD, PASSWORD_LEN);
            if (nRet >= 0)
            {
                MessageBox("��ʾ\n�����޸ĳɹ���");
            }
            else
            {
                MessageBox("��ʾ\n�����޸�ʧ�ܣ�");
            }
#endif
            return TRUE;
        }
        break;
    }
    return FALSE;
}
//��������
PLUGIN_INFO g_layerChangePwd = {
    0x11040001/*id*/,
    ChangePwdInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    ChangePwdDraw/*Draw*/,
    NULL/*Timer*/,
    ChangePwdKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void module_info_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.nCurPoint = dwPluginId & 0x0f;
    memset(g_module_info + g_guiComm.nCurPoint - 1, 0, sizeof(MOD_INFO_ACK_T));
    uint8 buf[320] = { 0 };
    uint16 msglen = 0;

    msglen = g_APPModeInfoFunc(g_guiComm.nCurPoint, buf, 320);
    if (0 == msglen)
    {
        GUI_FMT_DEBUG("get map_info failed, mod[%d]\n", g_guiComm.nCurPoint);
        return;
    }
    g_module_info[g_guiComm.nCurPoint - 1].status = 2;
    appmsg_modinfo_split(buf, g_module_info + g_guiComm.nCurPoint - 1);
    return;
}

//ģ���豸�ͺš�ģ���豸ID������汾������������ڡ�Ӳ���汾��
//Ӳ���������ڡ����̱��롢�ӿ����͡���������
void module_info_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;


    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    uint8   validFlag = g_module_info[g_guiComm.nCurPoint - 1].status;
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char  czBuf[300] = { 0 };
    sprintf(czBuf, "ģ��%d��Ϣ", g_guiComm.nCurPoint);
    rc.top = 26;
    rc.bottom = rc.top + 16;
    int nTop = rc.top;
    int nLeft1 = 7;
    int nLeft2 = 60;
    //int nLeft3 = 57;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    switch (g_ac_data_display_drawpage_index)
    {
    case 0:
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "�豸�ͺ�:");
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].type.value);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "�豸ID:");
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].Id.value);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "����汾:");
        sprintf(czBuf, "%08x", g_module_info[g_guiComm.nCurPoint - 1].softVersion);
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "��������:");
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].softDate.value);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        break;

    case 1:
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "Ӳ���汾:");
        sprintf(czBuf, "%08x", g_module_info[g_guiComm.nCurPoint - 1].hwVersion);
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, czBuf);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "��������:");
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].hwDate.value);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        nTop += 20;
        TextOut(hdc, nLeft1, nTop, "���̱���:");
        if (validFlag == 0)
            TextOut(hdc, nLeft2, nTop, g_module_info[g_guiComm.nCurPoint - 1].manuCode.value);
        else
            TextOut(hdc, nLeft2, nTop, "--------");
        break;

    case 2:
    {
        nTop += 20;
        int pipeCnt = g_module_info[g_guiComm.nCurPoint - 1].num;
        int i;
        if (validFlag == 0)
        {
            for (i = 0; i < pipeCnt; i++)
            {
                sprintf(czBuf, "ͨ��%d:", i);
                TextOut(hdc, nLeft1, nTop, czBuf);
                sprintf(czBuf, "%s %s", interfaceType[g_module_info[g_guiComm.nCurPoint - 1].channel[i].type], pipeFunc[g_module_info[g_guiComm.nCurPoint - 1].channel[i].function]);
                TextOut(hdc, nLeft2, nTop, czBuf);
                nTop += 15;
            }
        }
        else
            TextOut(hdc, nLeft1, nTop, "ͨ��: --------");

        break;
    }
    default:
        break;
    }
}
BOOL module_info_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_UP:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
            g_ac_data_display_drawpage_index = 2;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;

    case KEY_DOWN:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 2)
            g_ac_data_display_drawpage_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;

    default:
        break;
    }
    return FALSE;
}

//ģ����Ϣ
PLUGIN_INFO g_module_info_display1 = {
    0x11050011/*id*/,
    module_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    module_info_Draw/*Draw*/,
    NULL/*Timer*/,
    module_info_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_module_info_display2 = {
    0x11050012/*id*/,
    module_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    module_info_Draw/*Draw*/,
    NULL/*Timer*/,
    module_info_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_module_info_display3 = {
    0x11050013/*id*/,
    module_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    module_info_Draw/*Draw*/,
    NULL/*Timer*/,
    module_info_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_module_info_display4 = {
    0x11050014/*id*/,
    module_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    module_info_Draw/*Draw*/,
    NULL/*Timer*/,
    module_info_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

PLUGIN_INFO g_module_info_display5 = {
    0x11050015/*id*/,
    module_info_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    module_info_Draw/*Draw*/,
    NULL/*Timer*/,
    module_info_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void restartAPPInit(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void restartAPPDraw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "����Ӧ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

}

BOOL restartAPPKeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            //            if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("��ʾ\n�Ƿ�����Ӧ��\n����ȷ��һ��", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            PopMsg("��������Ӧ��...");
            //                appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
            exit(0);
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//����Ӧ��
PLUGIN_INFO g_layer_restartAPP =
{
    0x11040002/*id*/,
    restartAPPInit/*InitLayer*/,
    NULL/*UninitLayer*/,
    restartAPPDraw/*Draw*/,
    NULL/*Timer*/,
    restartAPPKeyMsg/*keymsg*/,
    NULL/*ipc*/
};

void reboot_terminal_Init(DWORD dwPluginId)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    g_ac_data_display_drawpage_index = 0;
    pg_guicomm->bCanTrunDisp = FALSE;
}
void reboot_terminal_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "�����ն�";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

}

BOOL reboot_terminal_KeyMsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            //            if(ValidatePwd() == TRUE)
        {
            if (FALSE == MessageBox("��ʾ\n�Ƿ������ն�\n����ȷ��һ��", MB_OKCANCEL))
            {
                return TRUE;
            }
            g_ac_data_display_drawpage_index = 0;
            PopMsg("���������ն�...");
            reboot_sent_to_smiOS();
        }
        break;
        }
        return TRUE;
    }
    return FALSE;
}
//�����ն�
PLUGIN_INFO g_layer_reboot_terminal =
{
    0x1104100b/*id*/,
    reboot_terminal_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    reboot_terminal_Draw/*Draw*/,
    NULL/*Timer*/,
    reboot_terminal_KeyMsg/*keymsg*/,
    NULL/*ipc*/
};


void data_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void data_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "���ݳ�ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

}

int data_initialization()
{
    PopMsg("�������ݳ�ʼ��...");

    CLEAR_DATA_T ClearData;
    ClearData.recordOAD.logicId = 0;
    ClearData.bClassTag = FALSE;
    int ret = 0;
    ret = statis_clear_data(g_gui_db_client, &ClearData);
    if (ret == ERR_OK)
    {
        GUI_FMT_TRACE("db_clear_data ok. ret(%d)\n", ret);
    }
    else
    {
        GUI_FMT_TRACE("db_clear_data failed. ret(%d)\n", ret);
    }
//  ret |= data_init_sent_to_acmeter();
//  if (ret != ERR_OK)
    {
//      GUI_FMT_TRACE("param_init_sent_to_acmeter err. ret(%d)\n", ret);
    }

    ClosePopLayer();
    return ret;

}

BOOL data_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 0:
            AppCancelMenu();
            return TRUE;
        case 1:
            if (data_initialization() != 0)
            {
                MessageBox("���ݳ�ʼ��ʧ��!");
            }
            else
            {
                MessageBox("���ݳ�ʼ���ɹ�!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//���ݳ�ʼ��
PLUGIN_INFO g_layer_data_initialization =
{
    0x11040003/*id*/,
    data_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    data_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    data_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "������ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }
}

BOOL param_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 1:
            AppCancelMenu();
            break;
        case 0:
            if (statis_recover_default(g_gui_db_client, 0) == 0)
            {
                PopMsg("������ʼ���ɹ���\n��������Ӧ��...");
                //��������Ĭ�ϲ����Ľӿ� �������
                app_setdefault_cfg_area(g_gui_db_client);
                reboot_sent_to_smiOS();
            }
            else
            {
                MessageBox("������ʼ��ʧ��!");
                AppCancelMenu();
            }
            break;
        }
        return TRUE;
    }
    return FALSE;
}

//������ʼ��
PLUGIN_INFO g_layer_param_initialization =
{
    0x11040004/*id*/,
    param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ac_param_initialization_init(DWORD dwPluginId)
{
    g_ac_data_display_drawpage_index = 0;
    g_guiComm.bCanTrunDisp = FALSE;
}
void ac_param_initialization_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    rc.top = 40;
    rc.bottom = rc.top + 16;
    const char* pcz = "���ɲ�����ʼ��";
    DrawText(hdc, pcz, strlen(pcz), &rc, DT_CENTER | DT_VCENTER);

    int nTop = 80;
    int nLeft = 26;
    if (g_ac_data_display_drawpage_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȷ��");
    }

    nLeft = 110;
    if (g_ac_data_display_drawpage_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft, nTop, "ȡ��");
    }
}

BOOL ac_param_initialization_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_LEFT:
        g_ac_data_display_drawpage_index--;
        if (g_ac_data_display_drawpage_index < 0)
        {
            g_ac_data_display_drawpage_index = 1;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        g_ac_data_display_drawpage_index++;
        if (g_ac_data_display_drawpage_index > 1)
        {
            g_ac_data_display_drawpage_index = 0;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_OK:
        switch (g_ac_data_display_drawpage_index)
        {
        case 1:
            AppCancelMenu();
            return TRUE;
        case 0:
            if (param_init_sent_to_acmeter() == 0)
            {
                MessageBox("���ɲ�����ʼ���ɹ�!");
                AppCancelMenu();
            }
            else
            {
                MessageBox("���ɲ�����ʼ��ʧ��!");
            }
            return TRUE;

        }
        return TRUE;
    }
    return FALSE;
}

//���ɲ�����ʼ��
PLUGIN_INFO g_layer_ac_param_initialization =
{
    0x11040008/*id*/,
    ac_param_initialization_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ac_param_initialization_draw/*Draw*/,
    NULL/*Timer*/,
    ac_param_initialization_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#if DESC("����״̬�鿴",1)
uint8 tsakID;
OOP_ACQ_MONITOR_T g_acq;

int get_task_status(uint8 id, OOP_ACQ_MONITOR_T* pacq)
{
    OOP_TASK_T oopTask;
    if (app_get_task_list(id, &oopTask) != 0)
        return 0;
    return read_db_noamal_data(g_gui_db_client, 0x60340200, 0, id, (uint8*)pacq, sizeof(OOP_ACQ_MONITOR_T));
}

void taskStatusIdReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (value > 255 && TRUE == MessageBox("����ID����255\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", tsakID);
        InputBox(tmp, taskStatusIdReturn, NULL, 0x01);
    }
    else
    {
        tsakID = value;
        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID, &g_acq);
    }
}

void task_status_init(DWORD dwPluginId)
{
    tsakID = 1;
    memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
    get_task_status(tsakID, &g_acq);
    printf("taskid %d state %d \n", tsakID, g_acq.taskState);
    g_guiComm.bCanTrunDisp = FALSE;
}
void task_status_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    char czBuf[64] = { 0 };
    TextOut(hdc, nLeft1, nTop, "����ID:");
    sprintf(czBuf, "%hu", tsakID);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����״̬:");
    TextOut(hdc, nLeft2, nTop, g_showtaskStatus[g_acq.taskState]);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ�����:");
    sprintf(czBuf, "%hu", g_acq.sum);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ��ɹ���:");
    sprintf(czBuf, "%hu", g_acq.success);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ͱ�����:");
    sprintf(czBuf, "%hu", g_acq.send);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ձ�����:");
    sprintf(czBuf, "%hu", g_acq.receive);
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʼʱ��:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.startTime.month, g_acq.startTime.day,
        g_acq.startTime.hour, g_acq.startTime.minute, g_acq.startTime.second);
    TextOut(hdc, nLeft2 - 5, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ʱ��:");
    sprintf(czBuf, "%02d-%02d %02d:%02d:%02d", g_acq.endTime.month, g_acq.endTime.day,
        g_acq.endTime.hour, g_acq.endTime.minute, g_acq.endTime.second);
    TextOut(hdc, nLeft2 - 5, nTop, czBuf);

}

BOOL task_status_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_DOWN:
    {
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_UP:
    {
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_LEFT:

        if (tsakID == 1)
        {
            tsakID = 255;
        }
        else
        {
            tsakID--;
        }
        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID, &g_acq);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    case KEY_RIGHT:
        if (tsakID == 255)
        {
            tsakID = 1;
        }
        else
        {
            tsakID++;
        }
        memset(&g_acq, 0, sizeof(OOP_ACQ_MONITOR_T));
        get_task_status(tsakID, &g_acq);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
        
    case KEY_OK:
        char bzbuf[50];
        sprintf(bzbuf, "%d", tsakID);
        InputBox(bzbuf, taskStatusIdReturn, NULL, 0x01);
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }

    return FALSE;
}

//����״̬�鿴
PLUGIN_INFO g_layer_task_status =
{
    0x11040005/*id*/,
    task_status_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_status_draw/*Draw*/,
    NULL/*Timer*/,
    task_status_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ֶ��������",1)
struct manual_amr_result_struc
{
    uint8   protocol;   //��Լ����
    uint8   piid;       //piid
    char    state[16];  //���ؽ��״̬
    char    time[16];   //���ؽ��ʱ��
    char    data[16];   //���ؽ������(�����й�)
};
int8    g_manual_amr_index;
uint16  g_manual_amr_timeout;
uint8   g_manual_amr_piid = 1;
struct manual_amr_result_struc g_manual_amr_rst;


void manualAmrTimeOutReturn(char* pNewStr, void* pPara)
{
    int value = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (value > 65535 && TRUE == MessageBox("��ʱʱ�����65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {           
        sprintf(tmp, "%d", g_manual_amr_timeout);
        InputBox(tmp, manualAmrTimeOutReturn, NULL, 0x01);
    }
    else
    {
        g_manual_amr_timeout = value; 
    }
}

void manualAmrMakeFrame(uint8 *apdu, uint8 *len, OOP_METER_T oopMeter)
{
    uint8   index = 0;
    uint16  crc;
    
    apdu[index++] = 0x09;   //����
    apdu[index++] = 0x07;   //͸��ת��
    apdu[index++] = g_manual_amr_piid++;   //PIID
    //�˿�OAD
    apdu[index++] = (oopMeter.basic.port.value >> 24) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 16) & 0xFF;
    apdu[index++] = (oopMeter.basic.port.value >> 8) & 0xFF;
    apdu[index++] = oopMeter.basic.port.value & 0xFF;

    apdu[index++] = oopMeter.basic.baud;    //������
    apdu[index++] = 0x02;   //żУ��
    apdu[index++] = 0x08;   //����λ
    apdu[index++] = 0x01;   //ֹͣλ
    apdu[index++] = 0x00;   //����
    //���ĳ�ʱʱ��
    apdu[index++] = g_manual_amr_timeout >> 8;
    apdu[index++] = g_manual_amr_timeout & 0xFF;
    //�ֽڳ�ʱʱ��(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if (oopMeter.basic.protocol == 3)   //698Э��
    {
        apdu[index++] =  0x19; //͸�����ĳ��� 
        apdu[index++] = 0x68;   //��ʼ��
        //����
        apdu[index++] = 0x17;
        apdu[index++] = 0x00;
        
        apdu[index++] = 0x43;   //������
        apdu[index++] = 0x05;   //SA��־
        //SA��ַ
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];

        apdu[index++] = 0x00;   //CA��ַ

        crc = app_crc16_get(&apdu[index - 11], 11);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x05;   //��ȡ����
        apdu[index++] = 0x01;   //��ȡһ����������
        apdu[index++] = 0x05;   //PIID
        //OAD
        apdu[index++] = 0x00;
        apdu[index++] = 0x10;
        apdu[index++] = 0x02;
        apdu[index++] = 0x00;

        apdu[index++] = 0x00;   //timeTag

        crc = app_crc16_get(&apdu[index - 21], 21);
        apdu[index++] = crc & 0xFF;
        apdu[index++] = crc >> 8;

        apdu[index++] = 0x16;   //�����ַ�

    }   
    else
    {
        uint8   checkSum = 0; //У���
        int     i;
        
        apdu[index++] = 0x13; //͸�����ĳ���
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0xFE;
        apdu[index++] = 0x68; //��ʼ��
        //SA��ַ
        apdu[index++] = oopMeter.basic.tsa.add[5];
        apdu[index++] = oopMeter.basic.tsa.add[4];
        apdu[index++] = oopMeter.basic.tsa.add[3];
        apdu[index++] = oopMeter.basic.tsa.add[2];
        apdu[index++] = oopMeter.basic.tsa.add[1];
        apdu[index++] = oopMeter.basic.tsa.add[0];
        apdu[index++] = 0x68; //��ʼ��
        apdu[index++] = 0x11; //������
        apdu[index++] = 0x04; //����
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x00 + 0x33;
        apdu[index++] = 0x01 + 0x33;
        apdu[index++] = 0x00 + 0x33;

        for (i = 0; i < index; i++)
        {
            checkSum += apdu[index - 14 + i];
        }
        apdu[index++] = checkSum;
        apdu[index++] = 0x16;   //�����ַ�
    }
    apdu[index++] = 0x00;   //timeTag

    *len = index;
}

void manual_amr_init(DWORD dwPluginId)
{
    g_manual_amr_index = 0;
    g_guiComm.nCurPoint = 1;
    g_manual_amr_timeout = 50;
    memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
}

void manual_amr_draw(HDC hdc)
{
    //int flag = 0;  //��ʶ�����Ƿ����
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 85;
    int nLeft3 = 50;
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    //����Ƴ������ȴ�����
    //g_guiMain.nLightonTime = 0;
    //g_guiMain.bLighton = TRUE;

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    TextOut(hdc, nLeft1, nTop, "�������");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_manual_amr_index == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�����ַ");
    if (app_get_meter_list(g_guiComm.nCurPoint, &MeterFile) == ERR_OK)
    {
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", MeterFile.basic.tsa.add[0], MeterFile.basic.tsa.add[1],
                MeterFile.basic.tsa.add[2], MeterFile.basic.tsa.add[3], MeterFile.basic.tsa.add[4], MeterFile.basic.tsa.add[5]);
    }
    else
    {
        sprintf(czBuf, "--");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��(s)");
    sprintf(czBuf, "%d", g_manual_amr_timeout);
    if (g_manual_amr_index == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
  
    
    nTop += 20;
    if (g_manual_amr_index == 2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "ȷ������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "ȷ������");
    }

    nTop += 12;
    TextOut(hdc, 0, nTop, "---------------------------------------------");
    
    nTop += 12;
    TextOut(hdc, nLeft1, nTop, "״̬");
    if (g_manual_amr_index == 3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.state);
    }
    
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ʱ��");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.time);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�������й�");
    TextOut(hdc, nLeft2 - 10, nTop, g_manual_amr_rst.data);
    
}

BOOL manual_amr_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    { 
        case KEY_UP:
        {
            g_manual_amr_index--;
            if (g_manual_amr_index < 0)
                g_manual_amr_index = 2;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_manual_amr_index++;
            if (g_manual_amr_index > 2)
                g_manual_amr_index = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint--;
                if (g_guiComm.nCurPoint < 0)
                {
                    g_guiComm.nCurPoint = MAX_SEL_POINT;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_RIGHT:
        {
            if (g_manual_amr_index == 0)
            {
                g_guiComm.nCurPoint++;
                if (g_guiComm.nCurPoint > MAX_SEL_POINT)
                {
                    g_guiComm.nCurPoint = 0;
                }
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                return TRUE;
            }
        }
        break;
        case KEY_OK:
        {
            if (g_manual_amr_index == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }

            if (g_manual_amr_index == 1)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_manual_amr_timeout);
                InputBox(bzbuf, manualAmrTimeOutReturn, NULL, 0x01);
            }
            
            if (g_manual_amr_index == 2)
            {
                OOP_METER_T        oopMeter;
                uint8   apdu[100] = {0};
                uint8   len = 0;
                
                memset(&oopMeter, 0, sizeof(OOP_METER_T));
                if (app_get_meter_list(g_guiComm.nCurPoint, &oopMeter) != ERR_OK)
                {
                    GUI_FMT_DEBUG(" app_get_meter_list %d !\n", g_guiComm.nCurPoint);
                    MessageBox("��������ȷ�Ĳ������!", MB_OK);
                    return FALSE;
                }

                g_manual_amr_index++;
                memset(&g_manual_amr_rst, 0, sizeof(g_manual_amr_rst));
                sprintf(g_manual_amr_rst.state, "������...");
                g_manual_amr_rst.piid = g_manual_amr_piid;
                g_manual_amr_rst.protocol = oopMeter.basic.protocol;
                manualAmrMakeFrame(apdu, &len, oopMeter);
                gui_mannual_amr_send(apdu, len);
            }

            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}


//�ֶ��������
PLUGIN_INFO g_layer_manual_amr =
{
    0x11040006/*id*/,
    manual_amr_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    manual_amr_draw/*Draw*/,
    NULL/*Timer*/,
    manual_amr_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�ز�������Ϣ�鿴",1)
struct line_net_result_struc
{
    uint8   protype;        //��ѯ״̬���Ƿ�ʱ��0,��ʼ��1������2.�������3.���Գ�ʱ
    uint8   routestate;    //����·��ѧϰ״̬ 0.δ��� 1.����� 2.��ʼ״̬
    uint16  linefilenum;    //��ǰ��������
    uint16  netnum;         //���������ɹ�����
};
struct line_net_result_struc g_line_net_rst;

int8    g_line_net_index;
uint16  g_line_net_timeout;
uint16  len;
uint8   g_line_net_piid = 1;


void line_net_apdu(uint8 *apdu,uint16 *len,uint8 protype)
{
    uint8   index=0;
    memset(apdu,0,100);
    apdu[index++]=0x09;
    apdu[index++]=0x07;
    apdu[index++]=g_line_net_piid++;
    apdu[index++]=0xf2;
    apdu[index++]=0x09;
    apdu[index++]=0x02;    
    apdu[index++]=0x01;
    //������
    apdu[index++]=0x03;
    apdu[index++]=0x02;
    apdu[index++]=0x08;
    apdu[index++]=0x01;
    apdu[index++]=0x00;
    //
    apdu[index++] = g_line_net_timeout >> 8;
    apdu[index++] = g_line_net_timeout & 0xFF;
    //�ֽڳ�ʱʱ��(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if(protype==0)
    {
    //͸��������֡1736.2 ��������
       apdu[index++] = 0x12;
       apdu[index++] = 0x68;
       apdu[index++] = 0x12;
       apdu[index++] = 0x00;
       apdu[index++] = 0x43;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x80;
       apdu[index++] = 0x25;
       apdu[index++] = 0x33;
       apdu[index++] = 0x10;
       apdu[index++] = 0x10;
       apdu[index++] = 0x02;
       apdu[index++] = 0x01;
       apdu[index++] = 0x00;
       apdu[index++] = 0x0a;
       apdu[index++] = 0x48;
       apdu[index++] = 0x16;

    }
    else
    {
       apdu[index++] = 0x0f;
       apdu[index++] = 0x68;
       apdu[index++] = 0x0f;
       apdu[index++] = 0x00;
       apdu[index++] = 0x43;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x00;
       apdu[index++] = 0x0e;
       apdu[index++] = 0x10;
       apdu[index++] = 0x08;
       apdu[index++] = 0x00;
       apdu[index++] = 0x69;
       apdu[index++] = 0x16;

    }
    apdu[index++] = 0x00;   //timeTag
    *len=index;
}

void line_net_init(DWORD dwPluginId)
{

    g_line_net_index = 0;
    g_line_net_timeout = 50;
    memset(&g_line_net_rst, 0, sizeof(line_net_result_struc));
    uint8   apdu[100] = {0};
    g_line_net_rst.linefilenum=oopMeterList.oopMeterNum;
    g_line_net_rst.routestate=2;
    line_net_apdu(apdu,&len,0);
    gui_mannual_amr_send(apdu, len);
    g_line_net_rst.protype=1;

    line_net_apdu(apdu,&len,1);  //��ѯ·��ѧϰ
    gui_mannual_amr_send(apdu, len);

}

void line_net_draw(HDC hdc)
{
    
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
    return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf,"%s","�ز�������Ϣ");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 100;
    TextOut(hdc, nLeft1, nTop, "��ǰ��������");
    if (g_line_net_rst.linefilenum == 0)
    {
        sprintf(czBuf, "0");
    }
    else
    {
        sprintf(czBuf, "%d",g_line_net_rst.linefilenum);
    }

    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�����ɹ�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (g_line_net_rst.netnum == 0)
    {
        sprintf(czBuf, "0");
    }
    else    
    {
        sprintf(czBuf, "%d", g_line_net_rst.netnum);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "·��ѧϰ״̬");
    if(g_line_net_rst.routestate==0)
    {
        sprintf(czBuf, "δ���"); 
    }
    else if(g_line_net_rst.routestate==1)
    {
        sprintf(czBuf,"�����");
    }
    else if(g_line_net_rst.routestate==2)
    {
        sprintf(czBuf,"��ʼ");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += 20;
    TextOut(hdc, 0, nTop, "---------------------------------------------");

    nTop +=12;
    TextOut(hdc, nLeft1, nTop, "״̬:");

    if(g_line_net_rst.protype==0)
    {
        memset(czBuf,0x00,sizeof(czBuf));
        sprintf(czBuf, "��ʼ");
    }
    if(g_line_net_rst.protype==1)
    {
        memset(czBuf,0x00,sizeof(czBuf));
        sprintf(czBuf, "������");   
    }
    if(g_line_net_rst.protype==2)
    {
        memset(czBuf,0x00,sizeof(czBuf));
        sprintf(czBuf, "�������");   
    }
    if(g_line_net_rst.protype==3)
    {
        memset(czBuf,0x00,sizeof(czBuf));
        sprintf(czBuf, "���Գ�ʱ"); 
    }
    if(g_line_net_index==0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+30, nTop, czBuf); 
    }
    memset(czBuf,0x00,sizeof(czBuf));
    sprintf(czBuf, "����");
    if(g_line_net_index==1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+90, nTop, czBuf);
        ITEM_NOT_SELECT;

    }
    else
    {
        TextOut(hdc, nLeft1+90, nTop, czBuf);
    }
}

BOOL line_net_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    switch(nKey)
    {
        case KEY_UP:
          {
            if(g_line_net_index==0)
                g_line_net_index=1;
            else
                g_line_net_index=0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          }  
        case KEY_DOWN:
          {
            if(g_line_net_index==0)
                g_line_net_index=1;
            else
                g_line_net_index=0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          }
        case KEY_OK:
            {
            uint8 apdu[100]={0}; 
    
            memset(&g_line_net_rst,0,sizeof(line_net_result_struc)); //������Ҫ��ʼ��
            g_line_net_rst.linefilenum=oopMeterList.oopMeterNum;           
            g_line_net_rst.routestate=2;            //�趨·��״̬Ϊ��ʼ
            line_net_apdu(apdu,&len,0);             //�����������֡
            gui_mannual_amr_send(apdu, len);        //����
            g_line_net_rst.protype=1;               //�ᶨ״̬Ϊ��ѯ��
            line_net_apdu(apdu,&len,1);             //��ѯ·��ѧϰ
            gui_mannual_amr_send(apdu, len);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            sleep(10);
            if(g_line_net_rst.protype!=2)
               g_line_net_rst.protype=3;
            return TRUE;
            }
    }
    return FALSE;
}


//�ز�������Ϣ�鿴
PLUGIN_INFO g_layer_line_net =
{
    0x1104000a/*id*/,
    line_net_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    line_net_draw/*Draw*/,
    NULL/*Timer*/,
    line_net_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#ifdef AREA_HUNAN
OOP_SENSOR_PARAM_T sensor_parm;
uint8 g_sensor_index;
uint8 g_sensor_get_index;


#if DESC("�����������鿴",1)

void sensorparam_check_init(DWORD dwPluginId)
{
    memset(&sensor_parm,0x00,sizeof(sensor_parm));
    g_sensor_get_index=0;
}
void sensorparam_check_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
    return;
    int index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf,"%s","�����������鿴");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 50;
    TextOut(hdc, nLeft1, nTop, "������:");
    memset(czBuf,0,sizeof(czBuf));
    sprintf(czBuf, "%s",g_sensor_name_value[g_sensor_get_index]);
    if(g_sensor_index==index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+50, nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+50, nTop,czBuf);
    }
    nTop +=20;
    index++;
    if(g_sensor_index==index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "������ѯ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "������ѯ");
    }
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "*******************************************");
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    //memcpy(&czBuf,&sensor_parm.addr.value,sensor_parm.addr.nNum);
    //strcpy(czBuf,(const char*)sensor_parm.addr.value);
    if(g_sensor_get_index == 3)
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", sensor_parm.addr.value[0], sensor_parm.addr.value[1], sensor_parm.addr.value[2], sensor_parm.addr.value[3], sensor_parm.addr.value[4], sensor_parm.addr.value[5]);
    else
        sprintf(czBuf, "%02x", sensor_parm.addr.value[0]);
    //sprintf(czBuf, "%02x",sensor_parm.addr.value[0]);
    TextOut(hdc, nLeft2, nTop, czBuf);
    nTop += 20;
    
    TextOut(hdc, nLeft1, nTop, "�˿�");

    memset(czBuf, 0x00, sizeof(czBuf));
    if(sensor_parm.port.value == 0xF2010201)
    {
        sprintf(czBuf, "485-1");
    }
    else if(sensor_parm.port.value == 0xF2010202)
    {
        sprintf(czBuf, "485-2");
    }
    else if(sensor_parm.port.value == 0xF2010203)
    {
       sprintf(czBuf, "485-3");
    }
    else if(sensor_parm.port.value == 0xF2010204)
    {
        sprintf(czBuf, "485-4");
    }
    else
    {
        sprintf(czBuf,"-----");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    PopListDraw(hdc);
}
int read_sensor_param(void)
{
    OOP_OAD_U sensor_info[4]={0x60E10200,0x60E20200,0x60E30200,0x60E40200};
    NOMAL_OAD_T sensor_info_tmp = {0};
    uint32 len;
    int ret;
    sensor_info_tmp.oad.value = sensor_info[g_sensor_get_index].value;
    sensor_info_tmp.classtag = 3;
    ret = db_read_nomal(g_gui_db_client, &sensor_info_tmp,sizeof(sensor_parm),(uint8 *)&sensor_parm,&len);
    if (ret != 0 || len != sizeof(OOP_SENSOR_PARAM_T))
        return -1;
    return 1;
}
BOOL sensorparam_check_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_sensor_index)
            {
                case 0:
                {
                    g_sensor_get_index = nSel;
                }
                break;
                default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
        case KEY_UP:
        case KEY_RIGHT: 
          {
            if(g_sensor_index==0)
                g_sensor_index=1;
            else
                g_sensor_index=0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          }  
        case KEY_DOWN:
        case KEY_LEFT:    
          {
            if(g_sensor_index==0)
                g_sensor_index=1;
            else
                g_sensor_index=0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          }
        case KEY_OK:
            {
            switch (g_sensor_index)
            {
                case 0:
                {
                    PopList(g_sensor_name_value, 4, g_sensor_name_value[0], 60, 50, FALSE);
                }
                break;
                case 1:
                {
                    read_sensor_param();
                }
                break;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
            }
    }
    return FALSE;
}

//�����������鿴
PLUGIN_INFO g_layer_sensor_param =
{
    0x1104000c/*id*/,
    sensorparam_check_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    sensorparam_check_draw/*Draw*/,
    NULL/*Timer*/,
    sensorparam_check_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("��������������",1)
int g_set_sensor_index;
uint8 g_sensor_port_index;

OOP_SENSOR_PARAM_T sensor_parm_set;
int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt)
{
    int i, odd;
    uint8 c;
    uint8 a;

    odd = len&0x01;
    if ( odd && !fmt)
        *bcd++ = (*asc++) & '\x0F';

    len >>= 1;

    for (i=0; i<len; i++)
    {
        if(*asc >= 'A' && *asc <= 'F')
        {
            c = *asc - 0x37; 
        }
        else if(*asc >= 'a' && *asc <= 'f')
        {
            c  = *asc - 0x57;
        }
        else
        {
            c  = *asc - 0x30;
        }
        //printf("1 = %x, %x\n", *asc, c);
        c = c << 4;
        //printf("2 = %x\n", c);
        asc++;

        if(*asc >= 'A' && *asc <= 'F')
        {
            a = *asc - 0x37;
        }
        else if(*asc >= 'a' && *asc <= 'f')
        {
            a  = *asc - 0x57;
        }
        else
        {
            a  = *asc - 0x30;
        }
        //printf("3 = %x, %x\n", *asc, c);
        c  |= a;
        //printf("4 = %x\n", c);
        asc++;

        *bcd++ = c;
    }

    if ( odd && fmt)
        *bcd = (*asc) << 4;

    return (i+odd);
}

/*******************************************************************************
* ��������: sensor_addr_set
* ��������: ���ô�������ַ
* �������:
        str �������ļ��ж����ĵ�ַ
* �������: addr ��������ַ

* �� �� ֵ: 
*******************************************************************************/
void sensor_addr_set(uint8 *addr,char* str)
{
    char tem[10] = {0};
    
    if(strlen(str)>2)
    {
        memcpy(tem,str,2);
    }else
    {
        strcpy(tem,"00");
        strcpy(tem+2-strlen(str),str);
    }

    commfun_ASC2BCD((char*)addr,(const char*)tem,2,0);

}

/*******************************************************************************
* ��������: sensor_addr_set_6
* ��������: ���ô�������ַ
* �������: 
        str �������ļ��ж����ĵ�ַ
* �������: addr ��������ַ 6�ֽ�

* �� �� ֵ: 
*******************************************************************************/
void sensor_addr_set_6(uint8 *addr,char* str)
{
    char tem[20] = {0};
    
    if(strlen(str)>12)
    {
        memcpy(tem,str,12);
    }else
    {
        strcpy(tem,"000000000000");
        strcpy(tem+12-strlen(str),str);
    }

    commfun_ASC2BCD((char*)addr,(const char*)tem,12,0);

}
void sensorparam_set_init(DWORD dwPluginId)
{
    memset(&sensor_parm_set,0x00,sizeof(sensor_parm_set));
    g_set_sensor_index =0;
    g_sensor_port_index = 3;
}
void sensorparam_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
    return;
    
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;
    char czBuf[300];
    sprintf(czBuf,"%s","��������������");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    TextOut(hdc, nLeft1, nTop, "������:");
    memset(czBuf,0,sizeof(czBuf));
    sprintf(czBuf, "%s",g_sensor_name_value[g_sensor_get_index]);
    if(g_set_sensor_index==0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+50, nTop,czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+50, nTop,czBuf);
    }
    
    nTop +=20;
    TextOut(hdc, nLeft1, nTop, "��ַ");
    
    if(g_sensor_get_index == 3)
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", sensor_parm_set.addr.value[0], sensor_parm_set.addr.value[1], sensor_parm_set.addr.value[2], sensor_parm_set.addr.value[3], sensor_parm_set.addr.value[4], sensor_parm_set.addr.value[5]);
    else
        sprintf(czBuf, "%02x", sensor_parm_set.addr.value[0]);
    //sprintf(czBuf, "%x", sensor_parm_set.addr.value);
    if(g_set_sensor_index==1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+50, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+50, nTop, czBuf);
    } 

    nTop +=20;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    sprintf(czBuf,"%s",g_pipe[g_sensor_port_index]);
    if(g_set_sensor_index==2)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+40, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+40, nTop, czBuf);
    }
    nTop +=40;
    if(g_set_sensor_index==3)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȷ��");
    }
    
    if(g_set_sensor_index==4)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+40, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+40, nTop, "����");
    }
    PopListDraw(hdc);
}
int write_sensor_param(void)
{
    OOP_OAD_U sensor_info[4]={0x60E10200,0x60E20200,0x60E30200,0x60E40200};
    NOMAL_OAD_T sensor_info_tmp = {0};
    int ret;
    sensor_info_tmp.oad.value = sensor_info[g_sensor_get_index].value;
    sensor_info_tmp.classtag = 3;

   if(g_sensor_port_index == 2)
       sensor_parm_set.port.value = 0xF2010201;
   else if(g_sensor_port_index == 3)
       sensor_parm_set.port.value = 0xF2010202;
   else if(g_sensor_port_index == 4)
       sensor_parm_set.port.value = 0xF2010203;
   else if(g_sensor_port_index == 5)
       sensor_parm_set.port.value = 0xF2010204;

    ret = db_write_nomal(g_gui_db_client,&sensor_info_tmp, (uint8 *)&sensor_parm_set, sizeof(sensor_parm_set));
    if(ret != ERR_OK)
    {
        GUI_FMT_DEBUG("%s����д��ʧ�� \n",g_sensor_name_value[g_sensor_get_index]);
        MessageBox("��������������ʧ��");
        return DATA_OTHER_REASON;
    }
    else
    {
        GUI_FMT_DEBUG("%s����д��ɹ� \n",g_sensor_name_value[g_sensor_get_index]);
        MessageBox("�������������óɹ�");
    }
    return DATA_SUCCESS;
}

void AddrsetReturn(char* pNewStr, void* pPara)
{
    char tmp[20] = {0};
    uint8 len = strlen(pNewStr);
    switch(g_set_sensor_index)
    {
        case 1:
        {
          if((g_sensor_get_index ==3 && len!=12) || (g_sensor_get_index !=3 && len!=2))
          {
              if(TRUE == MessageBox("��ַ�Ƿ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
              {
          
                  InputBox(tmp, AddrsetReturn, NULL, 0x01|0x03);
              }
          }
          else
          {
            sensor_parm_set.addr.nNum = strlen(pNewStr);
        if(g_sensor_get_index !=3)  
            sensor_addr_set(sensor_parm_set.addr.value,pNewStr);
        else
            sensor_addr_set_6(sensor_parm_set.addr.value,pNewStr);
          }
          
        }break;
        default: break;
    }
}

BOOL sensorparam_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_set_sensor_index)
            {
                case 0:
                {
                    g_sensor_get_index = nSel;
                }
                break;
                case 2:
                {
                    g_sensor_port_index = nSel;
                }
                break;
                default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch(nKey)
    {
        case KEY_RIGHT:
        case KEY_DOWN:
          {
            g_set_sensor_index++;
            if(g_set_sensor_index == 5)
            g_set_sensor_index=0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          }
        case KEY_UP:
        case KEY_LEFT:    
          {
              g_set_sensor_index--;
              if(g_set_sensor_index<0)
                  g_set_sensor_index=4;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
          } 
        case KEY_OK:
            {
            switch (g_set_sensor_index)
            {
                
                case 0:
                {
                    PopList(g_sensor_name_value, 4, g_sensor_name_value[0], 60, 50, FALSE);
                    return TRUE; 
                }
                break;
                case 1:
                {
                    char bzbuf[20];
                    sprintf(bzbuf, "%s",sensor_parm_set.addr.value);
                    //sprintf(bzbuf, "%02x%02x%02x%02x%02x%02x",sensor_parm_set.addr.value[0],sensor_parm_set.addr.value[1],sensor_parm_set.addr.value[2],sensor_parm_set.addr.value[3],sensor_parm_set.addr.value[4],sensor_parm_set.addr.value[5]);
                    InputBox(bzbuf, AddrsetReturn, NULL, 0x01|0x04);
                }break;
                case 2:
                {
                    PopList(g_pipe, 11, g_pipe[0], 60, 90, FALSE); 
                    return TRUE;   
                }break;
                case 3:
                {
                    write_sensor_param();
                }break;
                break;
                case 4:
                {
                      AppCancelMenu();
                }
                break;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
            }
    }
    return FALSE;
}

//��������������
PLUGIN_INFO g_layer_sensor_set =
{
    0x1104000d/*id*/,
    sensorparam_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    sensorparam_set_draw/*Draw*/,
    NULL/*Timer*/,
    sensorparam_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
#endif
void app_version_Init(DWORD dwPluginId)
{
}
int get_addr(uint8* Addr)
{

    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x40010200;
    NormalOad.infoNum = 0;
    OOP_OCTETVAR16_T LocalAddr;
    memset(&LocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_OCTETVAR16_T), (uint8*)&LocalAddr, &len);
    if (ret != 0 || len != sizeof(OOP_OCTETVAR16_T))
        return -1;
    memcpy(Addr, LocalAddr.value, 6);
    printf("�ն˵�ַ 0x%02x%02x%02x%02x%02x%02x \n", Addr[5], Addr[4], Addr[3], Addr[2], Addr[1], Addr[0]);
    return 0;
}
uint8 LocalAddr[10] = { 0 };

void app_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 20;
    int tmpTop = 20;

    // ����汾��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����汾��");

    nTop += tmpTop;
    sprintf(czBuf, "%s", APP_VERSION);
    TextOut(hdc, nLeft2, nTop, czBuf);


    // ����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����ʱ��");
    nTop += tmpTop;
    snprintf(czBuf, 64, "%s", APP_PUBLISH_TIME);
    TextOut(hdc, nLeft2, nTop, czBuf);

    //    CancelKeyDraw(hdc, 3, 16, 12, 12);

}
//�汾��Ϣ
PLUGIN_INFO g_layer_app_version_display =
{
    0x11040007/*id*/,
    app_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    app_version_Draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


void tmn_version_Init(DWORD dwPluginId)
{
}

void get_area_name(char *areaName)
{
    if (areaName == NULL)
    {
        printf("get_area_name input err!\n");
        return;
    }

#ifdef AREA_ANHUI
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_HUNAN
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_ZHEJIANG
    strcpy(areaName, "�㽭");
    return;
#endif

#ifdef AREA_JIANGXI
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_SHANDONG
    strcpy(areaName, "ɽ��");
    return;
#endif

#ifdef AREA_GANSU
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_HUBEI
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_SHAANXI
    strcpy(areaName, "����");
    return;
#endif

#ifdef AREA_FUJIAN
    strcpy(areaName, "����");
    return;
#endif
    strcpy(areaName, "����");
    return;
}

void tmn_version_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    SelectFont(hdc, pg_guicomm->fontSmall);
    ITEM_NOT_SELECT;


    char czBuf[64] = { 0 };
    char areaName[64] = { 0 };

    int nLeft = 5;
    int nLeft2 = 15;
    int nTop = 10;
    int tmpTop = 16;

    // ����������ID
    get_area_name(areaName);
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����������ID");
    nTop += tmpTop;
    sprintf(czBuf, "%s %s", areaName, g_tVer.factory);
    TextOut(hdc, nLeft2, nTop, czBuf);

    // ����汾�ż�����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "����汾�ż�����ʱ��");

    nTop += tmpTop;
    sprintf(czBuf, "%s %s", g_tVer.softVer, g_tVer.softDate);
    TextOut(hdc, nLeft2, nTop, czBuf);


    // Ӳ���汾�ż�����ʱ��
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "Ӳ���汾�ż�����ʱ��");
    nTop += tmpTop;
    snprintf(czBuf, 64, "%s %s", g_tVer.hardVer, g_tVer.hardDate);
    TextOut(hdc, nLeft2, nTop, czBuf);

	// ��չ��Ϣ
    nTop += tmpTop;
    sprintf(czBuf, "������չ��Ϣ  %s", g_tVer.extend);
    TextOut(hdc, nLeft, nTop, czBuf);
}

//�ն˰汾��Ϣ
PLUGIN_INFO g_layer_tmn_version_display =
{
    0x11040009/*id*/,
    tmn_version_Init/*InitLayer*/,
    NULL/*UninitLayer*/,
    tmn_version_Draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void address_display_init(HDC hdc)
{
    get_addr(LocalAddr);
}

void address_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ն�ͨ�ŵ�ַ");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 10;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

}

//ͨ�ŵ�ַ�鿴
PLUGIN_INFO g_layer_address_display = {
    0x11030001/*id*/,
    address_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

OOP_ETHIP_T EthIP;      //�ն�ip
OOP_ETHIP_T EthIP1;      //�ն�ip		ecuר���Ʒ����������

int EthIPisSet;
int EthIPisSet1;

int g_ethIndex;

void ip_display_init(HDC hdc)
{
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_gui_db_client, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));

	g_ethIndex = 0;


}

void ip_display_draws(HDC hdc, OOP_ETHIP_T ethIP, int flag, uint8 index)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
	if(index != 0)
	{
		sprintf(czBuf, "2����IP����");
	}
	else
	{
		sprintf(czBuf, "IP��ַ����");
	}
    
    rc.top = 16;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "���÷�ʽ");
    if (flag < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (ethIP.ipConfig < 3)
            sprintf(czBuf, "%s", g_ipconfig[ethIP.ipConfig]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.ip[0], ethIP.ip[1], ethIP.ip[2], ethIP.ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.mask[0], ethIP.mask[1], ethIP.mask[2], ethIP.mask[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ص�ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.ipGateway[0], ethIP.ipGateway[1], ethIP.ipGateway[2], ethIP.ipGateway[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�û���");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.pppoeUser.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", ethIP.pppoeUser.value);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.pppoePwd.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", ethIP.pppoePwd.value);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}

void ip_display_draw(HDC hdc)
{
   
    if (g_ethIndex == 0)
    {
        ip_display_draws(hdc, EthIP, EthIPisSet, 0);
    }
    else
    {
        ip_display_draws(hdc, EthIP1, EthIPisSet1, 1);
    }
}

//��̫��ͨ���鿴
PLUGIN_INFO g_layer_ip_display = {
    0x11030002/*id*/,
    ip_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void ip_display_init1(HDC hdc)
{
	memset(&EthIP1, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet1 = read_db_noamal_data(g_gui_db_client, 0X45110400, 0, 0, (uint8*)&EthIP1, sizeof(OOP_ETHIP_T));
	g_ethIndex = 1;
}

//��̫��ͨ���鿴
PLUGIN_INFO g_layer_ip_display1 = {
    0x1103000B/*id*/,
    ip_display_init1/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthIPIndex;
void ip_set_init(HDC hdc)
{
    memset(&EthIP, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet = read_db_noamal_data(g_gui_db_client, EthIPOad.value, 0, 0, (uint8*)&EthIP, sizeof(OOP_ETHIP_T));
    EthIPIndex = 0;
	g_ethIndex = 0;
}

void ip_set_draws(HDC hdc, OOP_ETHIP_T ethIP, int flag, uint8 ethIndex)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
	if(ethIndex != 0)
	{
		sprintf(czBuf, "2����IP����");
	}
	else
	{
		sprintf(czBuf, "IP��ַ����");
	}
	
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "���÷�ʽ");
    if (flag < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (ethIP.ipConfig < 3)
            sprintf(czBuf, "%s", g_ipconfig[ethIP.ipConfig]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.ip[0], ethIP.ip[1], ethIP.ip[2], ethIP.ip[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.mask[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.mask[0], ethIP.mask[1], ethIP.mask[2], ethIP.mask[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ص�ַ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.ipGateway[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", ethIP.ipGateway[0], ethIP.ipGateway[1], ethIP.ipGateway[2], ethIP.ipGateway[3]);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�û���");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.pppoeUser.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", ethIP.pppoeUser.value);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag < 0 || ethIP.pppoePwd.nNum == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", ethIP.pppoePwd.value);
    }
    if (EthIPIndex == index)
    {
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;

    if (EthIPIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 80;
    if (EthIPIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��������");
    }

    PopListDraw(hdc);
}

void ip_set_draw(HDC hdc)
{
   
    if (g_ethIndex == 0)
    {
        ip_set_draws(hdc, EthIP, EthIPisSet, 0);
    }
    else
    {
        ip_set_draws(hdc, EthIP1, EthIPisSet1, 1);
    }
}


//���ַ�����ʽ��ipת��Ϊ���� ʧ�ܷ���false
BOOL commfun_IP(char* ipin, uint8* ipout)
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

void IpConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

	OOP_ETHIP_T *pethIP;
	int *pEthIPisSet;
	if (g_ethIndex == 0)
	{
		pethIP = &EthIP;
		pEthIPisSet = &EthIPisSet;
	}
	else
	{
		pethIP = &EthIP1;
		pEthIPisSet = &EthIPisSet1;
	}

    switch (EthIPIndex)
    {
    case 1: //ip��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (pethIP->ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->ip[0], pethIP->ip[1], pethIP->ip[2], pethIP->ip[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(pethIP->ip, tmp, 4);
            *pEthIPisSet = 1;
        }
    }
    break;
    case 2://��������
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("���������ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (pethIP->mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->mask[0], pethIP->mask[1], pethIP->mask[2], pethIP->mask[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(pethIP->mask, tmp, 4);
            *pEthIPisSet = 1;
        }
    }
    break;
    case 3://���ص�ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("���ص�ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (pethIP->ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->ipGateway[0], pethIP->ipGateway[1], pethIP->ipGateway[2], pethIP->ipGateway[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(pethIP->ipGateway, tmp, 4);
            *pEthIPisSet = 1;
        }
    }
    break;
    case 4:
    {
        pethIP->pppoeUser.nNum = strlen(pNewStr);
        strcpy(pethIP->pppoeUser.value, pNewStr);
        *pEthIPisSet = 1;
    }
    break;
    case 5:
    {
        pethIP->pppoePwd.nNum = strlen(pNewStr);
        strcpy(pethIP->pppoePwd.value, pNewStr);
        *pEthIPisSet = 1;
    }
    break;
    }

}

BOOL ip_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
	
	OOP_ETHIP_T *pethIP;
	int *pEthIPisSet;
	OOP_OAD_U ethOad;
	OOP_OAD_U ethOadNum;
	if (g_ethIndex == 0)
	{
		pethIP = &EthIP;
		pEthIPisSet = &EthIPisSet;
		ethOad.value = 0x45100400;
		ethOadNum.value = 0x451004FF;
	}
	else
	{
		pethIP = &EthIP1;
		pEthIPisSet = &EthIPisSet1;
		ethOad.value = 0x45110400;
		ethOadNum.value = 0x451104FF;
	}

	char ethName[36] = {0};
#ifdef ARM32R
	sprintf(ethName, "eth%d", g_ethIndex);
#else
	sprintf(ethName, "FE%d", g_ethIndex);
#endif

    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (EthIPIndex)
            {
            case 0://IP���÷�ʽ
            {
                pethIP->ipConfig = nSel;
                *pEthIPisSet = 1;
                if(0 == pethIP->ipConfig)
                {
                    memset(pethIP->ip, 0, 4);
                    memset(pethIP->mask, 0, 4);
                    memset(pethIP->ipGateway, 0, 4);
                }
            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
	case KEY_LEFT:
    case KEY_UP:
    {
        if (EthIPIndex > 0)
            EthIPIndex--;
        else
            EthIPIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
	case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (EthIPIndex < maxindex)
            EthIPIndex++;
        else
            EthIPIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (EthIPIndex == 6)
        {
            AppCancelMenu();
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (EthIPIndex)
            {
            case 0: //ip���÷�ʽ
            {
                PopList(g_ipconfig, 3, g_ipconfig[0], 70, 40, FALSE);
                return TRUE;
            }
            case 1://IP��ַ
            {
                char czBuf[20];
                if (pethIP->ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->ip[0], pethIP->ip[1], pethIP->ip[2], pethIP->ip[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 2://��������
            {
                char czBuf[20];
                if (pethIP->mask[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->mask[0], pethIP->mask[1], pethIP->mask[2], pethIP->mask[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://���ص�ַ
            {
                char czBuf[20];
                if (pethIP->ipGateway[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", pethIP->ipGateway[0], pethIP->ipGateway[1], pethIP->ipGateway[2], pethIP->ipGateway[3]);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 4://�û���
            {
                char czBuf[20];
                if (pethIP->pppoeUser.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", pethIP->pppoeUser.value);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 5://����
            {
                char czBuf[20];
                if (pethIP->pppoePwd.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", pethIP->pppoePwd.value);
                }
                InputBox(czBuf, IpConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 6://ȡ��
            {
                AppCancelMenu();
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)pethIP, sizeof(OOP_ETHIP_T), ethOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("IP��ַ����д��ʧ�ܣ�");
                    }
                    else
                    {
                    	if(0 != ipconfig_send_to_smiOS(ethName, pethIP->ipConfig, pethIP->ip, pethIP->mask, pethIP->ipGateway))
                        {
                            MessageBox("����IP��ַʧ�ܣ�");
                        }
                        uint8 flag = 0;
                        write_normal_data(&flag, sizeof(uint8), ethOadNum, 0, 0, 1);
                    }
                }
                EthIPIndex = 0;
                return TRUE;
            }
            case 7://����
            {
                ret = write_normal_data((uint8*)pethIP, sizeof(OOP_ETHIP_T), ethOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("IP��ַ����д��ʧ�ܣ�");
                }
                else
                {
                    MessageBox("IP��ַ����д��ɹ���");
					if(0 != ipconfig_send_to_smiOS(ethName, pethIP->ipConfig, pethIP->ip, pethIP->mask, pethIP->ipGateway))
                    {
                        MessageBox("����IP��ַʧ�ܣ�");
                    }
					else
					{
						reboot_sent_to_smiOS();
                        MessageBox("���óɹ�\n\n�ն�����\n\n��ȴ���");
                    }
                    uint8 flag = 0;
                    write_normal_data(&flag, sizeof(uint8), ethOadNum, 0, 0, 1);
                }
                AppCancelMenu();
                EthIPIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}

//��̫��ͨ���鿴
PLUGIN_INFO g_layer_ip_set = {
    0x11030012/*id*/,
    ip_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_set_draw/*Draw*/,
    NULL/*Timer*/,
    ip_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void ip_set_init1(HDC hdc)
{
	memset(&EthIP1, 0x00, sizeof(OOP_ETHIP_T));
    EthIPisSet1 = read_db_noamal_data(g_gui_db_client, 0X45110400, 0, 0, (uint8*)&EthIP1, sizeof(OOP_ETHIP_T));
	EthIPIndex = 0;
	g_ethIndex = 1;
}

//��̫��ͨ���鿴
PLUGIN_INFO g_layer_ip_set1 = {
    0x1103001B/*id*/,
    ip_set_init1/*InitLayer*/,
    NULL/*UninitLayer*/,
    ip_set_draw/*Draw*/,
    NULL/*Timer*/,
    ip_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};


OOP_ETHCONFIG_T EthConfig;
int EthConfigisSet;
void eth_display_init(HDC hdc)
{
    memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
    EthConfigisSet = read_db_noamal_data(g_gui_db_client, EthConfigOad.value, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
}

void eth_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    if (EthConfigisSet < 0 || EthConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.contype>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.conmode>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����IP");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.proxyport);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d ��", EthConfig.dail.mask.dailtime);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ط�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d ��", EthConfig.heart);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

}


//��̫��ͨ�����ò鿴
PLUGIN_INFO g_layer_eth_display = {
    0x11030006/*id*/,
    eth_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    eth_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthConfigIndex;
void eth_set_init(HDC hdc)
{
    memset(&EthConfig, 0x00, sizeof(OOP_ETHCONFIG_T));
    EthConfigisSet = read_db_noamal_data(g_gui_db_client, EthConfigOad.value, 0, 0, (uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T));
    EthConfigIndex = 0;
}
void eth_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    if (EthConfigisSet < 0 || EthConfig.workmode>2)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_work_mode[EthConfig.workmode]);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.contype>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%s", g_conn_type[EthConfig.contype]);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ģʽ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.conmode>1)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", g_conn_app_type[EthConfig.conmode]);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����IP");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {

        sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.proxyport);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d ��", EthConfig.dail.mask.dailtime);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ط�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��������");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthConfigisSet < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d ��", EthConfig.heart);
    }
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop -= 30;
    nLeft2 = 130;
    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ȡ��");
    }

    index++;
    nTop += 25;

    if (EthConfigIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

    PopListDraw(hdc);

}

void EthConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthConfigIndex)
    {
    case 3: //�����������ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthConfig.proxyip, tmp, 4);
            EthConfigisSet = 1;
        }
    }
    break;
    case 4:     //����˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthConfig.proxyport = port;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.proxyport);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 5:     //��ʱʱ��
    {
        int time = atoi(pNewStr);
        if (time <= 0x3f)      //��ʱʱ��ռ6λ ���0x3f
        {
            EthConfig.dail.mask.dailtime = time;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("��ʱʱ�����63 \n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 6:     //�ط�����
    {
        int dailcnt = atoi(pNewStr);
        if (dailcnt <= 3)      //�ط�����ռ2λ ���0x3
        {
            EthConfig.dail.mask.dailcnt = dailcnt;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("�ط��������3 \n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 7:     //��������
    {
        int heart = atoi(pNewStr);
        if (heart <= 65535)      //
        {
            EthConfig.heart = heart;
            EthConfigisSet = 1;
        }
        else
        {
            if (TRUE == MessageBox("������������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.heart);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}

BOOL eth_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 9;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (EthConfigIndex)
            {
            case 0://����ģʽ
            {
                EthConfig.workmode = nSel;
                EthConfigisSet = 1;
            }
            break;
            case 1://���ӷ�ʽ
            {
                EthConfig.contype = nSel;
                EthConfigisSet = 1;
            }
            break;
            case 2://����Ӧ�÷�ʽ
            {
                EthConfig.conmode = nSel;
                EthConfigisSet = 1;
            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (EthConfigIndex > 0)
            EthConfigIndex--;
        else
            EthConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (EthConfigIndex < maxindex)
            EthConfigIndex++;
        else
            EthConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (EthConfigIndex)
            {
            case 0: //����ģʽ
            {
                PopList(g_work_mode, 3, g_work_mode[0], 70, 20, FALSE);
                return TRUE;
            }
            case 1://���ӷ�ʽ
            {
                PopList(g_conn_type, 2, g_conn_type[0], 70, 40, FALSE);
                return TRUE;
            }
            case 2://����Ӧ�÷�ʽ
            {
                PopList(g_conn_app_type, 2, g_conn_app_type[0], 70, 60, FALSE);
                return TRUE;
            }
            case 3://�����������ַ
            {
                char czBuf[20];
                if (EthConfigisSet < 0 || EthConfig.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d.%d.%d.%d", EthConfig.proxyip[0], EthConfig.proxyip[1], EthConfig.proxyip[2], EthConfig.proxyip[3]);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 4://����˿�
            {
                char czBuf[20];
                if (EthConfigisSet < 0 || EthConfig.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.proxyport);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 5://��ʱʱ��
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 6://�ط�����
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 7://��������
            {
                char czBuf[20];
                if (EthConfigisSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthConfig.heart);
                }
                InputBox(czBuf, EthConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 8://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), EthConfigOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("��̫��ͨ������д��ʧ�ܣ�");
                    }

                }
                EthConfigIndex = 0;
                return TRUE;
            }
            case 9://����
            {
                ret = write_normal_data((uint8*)&EthConfig, sizeof(OOP_ETHCONFIG_T), EthConfigOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("��̫��ͨ������д��ʧ�ܣ�");
                }
                else {
                    MessageBox("��̫��ͨ������д��ɹ���");
                }
                EthConfigIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }
    return FALSE;
}

//��̫��ͨ����������
PLUGIN_INFO g_layer_eth_set = {
    0x11030016/*id*/,
    eth_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    eth_set_draw/*Draw*/,
    NULL/*Timer*/,
    eth_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};


OOP_MASTERPARAMS_T EthMaster;
void ethmaster_display_init(HDC hdc)
{
    read_db_noamal_data(g_gui_db_client, EthMasterOad.value, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
}

void ethmaster_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "��̫����վͨ�Ų���");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���õ�ַ");
    if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���ö˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[1].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

}

//��̫����վ�����鿴
PLUGIN_INFO g_layer_ethmaster_display = {
    0x11030007/*id*/,
    ethmaster_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ethmaster_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 EthMasterIndex;
void ethmaster_set_init(HDC hdc)
{
    read_db_noamal_data(g_gui_db_client, EthMasterOad.value, 0, 0, (uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T));
    EthMasterIndex = 0;
}

void ethmaster_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "��̫����վ��������");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[0].port);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���õ�ַ");
    if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���ö˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", EthMaster.master[1].port);
    }
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;

    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 80;
    if (EthMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

}
void EthMasterReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (EthMasterIndex)
    {
    case 0: //IP��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthMaster.master[0].ip, tmp, 4);
            if (EthMaster.nNum < 1)
                EthMaster.nNum = 1;
        }
    }
    break;
    case 1:     //�˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthMaster.master[0].port = port;
            if (EthMaster.nNum < 1)
                EthMaster.nNum = 1;
        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", EthMaster.master[0].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 2: //����IP��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(EthMaster.master[1].ip, tmp, 4);
            if (EthMaster.nNum < 2)
                EthMaster.nNum = 2;
        }
    }
    break;
    case 3:     //�˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            EthMaster.master[1].port = port;
            if (EthMaster.nNum < 2)
                EthMaster.nNum = 2;
        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (EthMaster.nNum < 1 || EthMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", EthMaster.master[1].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}
BOOL ethmaster_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 5;

    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (EthMasterIndex > 0)
            EthMasterIndex--;
        else
            EthMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (EthMasterIndex < maxindex)
            EthMasterIndex++;
        else
            EthMasterIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (EthMasterIndex)
            {
            case 0://IP��ַ
            {
                char czBuf[20];
                if (EthMaster.nNum < 1 || EthMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[0].ip[0], EthMaster.master[0].ip[1], EthMaster.master[0].ip[2], EthMaster.master[0].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://�˿�
            {
                char czBuf[20];
                if (EthMaster.nNum < 1 || EthMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[0].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://����IP��ַ
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", EthMaster.master[1].ip[0], EthMaster.master[1].ip[1], EthMaster.master[1].ip[2], EthMaster.master[1].ip[3]);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://���ö˿�
            {
                char czBuf[20];
                if (EthMaster.nNum < 2 || EthMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", EthMaster.master[1].port);
                }
                InputBox(czBuf, EthMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), EthMasterOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("��̫����վ����д��ʧ�ܣ�");
                    }
                }
                EthMasterIndex = 0;
                return TRUE;
            }
            case 5://����
            {
                ret = write_normal_data((uint8*)&EthMaster, sizeof(OOP_MASTERPARAMS_T), EthMasterOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("��̫����վ����д��ʧ�ܣ�");
                }
                else
                {
                    MessageBox("��̫����վ����д��ɹ���");
                }
                EthMasterIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}
//��̫����վ��������
PLUGIN_INFO g_layer_ethmaster_set = {
    0x11030017/*id*/,
    ethmaster_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    ethmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    ethmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

OOP_MASTERPARAMS_T GPRSMaster;
OOP_GPRSCONFIG_T GprsConfig;

void gprsmaster_display_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45000300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
}

void gprsmaster_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "���߹�����վ����");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���õ�ַ");
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���ö˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

}

//���߹���1��վ�����鿴
PLUGIN_INFO g_layer_gprsmaster_display = {
    0x11030009/*id*/,
    gprsmaster_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
void gprs2master_display_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45010300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
}
//���߹���2��վ�����鿴
PLUGIN_INFO g_layer_gprs2master_display = {
    0x11030121/*id*/,
    gprs2master_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


uint8 GprsMasterIndex;
void gprsmaster_set_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45000300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    GprsMasterIndex = 0;
}

void gprsmaster_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "���߹�����վ����");
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);

    int nTop = rc.top + 30;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "IP��ַ");
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
    }
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[0].port);
    }
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���õ�ַ");
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
    {
        sprintf(czBuf, "-.-.-.-");
    }
    else
    {
        sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
    }
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "���ö˿�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
    {
        sprintf(czBuf, "-");
    }
    else
    {

        sprintf(czBuf, "%d", GPRSMaster.master[1].port);
    }
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;

    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 80;
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

}
void GprsMasterReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };

    switch (GprsMasterIndex)
    {
    case 0: //IP��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(GPRSMaster.master[0].ip, tmp, 4);
            if (GPRSMaster.nNum < 1)
                GPRSMaster.nNum = 1;
        }
    }
    break;
    case 1:     //�˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            GPRSMaster.master[0].port = port;
            if (GPRSMaster.nNum < 1)
                GPRSMaster.nNum = 1;
        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    case 2: //����IP��ַ
    {
        if (FALSE == commfun_IP(pNewStr, tmp))
        {
            if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
            }
        }
        else
        {
            memcpy(GPRSMaster.master[1].ip, tmp, 4);
            if (GPRSMaster.nNum < 2)
                GPRSMaster.nNum = 2;
        }
    }
    break;
    case 3:     //�˿�
    {
        int port = atoi(pNewStr);
        if (port <= 65535)
        {
            GPRSMaster.master[1].port = port;
            if (GPRSMaster.nNum < 2)
                GPRSMaster.nNum = 2;
        }
        else
        {
            if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
            }
        }
    }
    break;
    }
}
BOOL gprsmaster_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 5;

    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (GprsMasterIndex > 0)
            GprsMasterIndex--;
        else
            GprsMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsMasterIndex < maxindex)
            GprsMasterIndex++;
        else
            GprsMasterIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (GprsMasterIndex)
            {
            case 0://IP��ַ
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[0].ip[0], GPRSMaster.master[0].ip[1], GPRSMaster.master[0].ip[2], GPRSMaster.master[0].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://�˿�
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 1 || GPRSMaster.master[0].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GPRSMaster.master[0].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://����IP��ַ
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].ip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GPRSMaster.master[1].ip[0], GPRSMaster.master[1].ip[1], GPRSMaster.master[1].ip[2], GPRSMaster.master[1].ip[3]);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x02);
                return TRUE;
            }
            case 3://���ö˿�
            {
                char czBuf[20];
                if (GPRSMaster.nNum < 2 || GPRSMaster.master[1].port == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GPRSMaster.master[1].port);
                }
                InputBox(czBuf, GprsMasterReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), GPRSMasterOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("���߹�����վ����д��ʧ�ܣ�");
                    }
                }
                GprsMasterIndex = 0;
                return TRUE;
            }
            case 5://����
            {
                ret = write_normal_data((uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T), GPRSMasterOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("���߹�����վ����д��ʧ�ܣ�");
                }
                else
                {
                    MessageBox("���߹�����վ����д��ɹ���");
                }
                GprsMasterIndex = 0;
                return TRUE;
            }
            }
        }
    }
    }

    return FALSE;
}
//��̫����վ��������
PLUGIN_INFO g_layer_gprsmaster_set = {
    0x11030019/*id*/,
    gprsmaster_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprsmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void gprs2master_set_init(HDC hdc)
{
    GPRSMasterOad.value = 0x45010300;
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    GprsMasterIndex = 0;
}
PLUGIN_INFO g_layer_gprs2master_set = {
    0x1103002b/*id*/,
    gprs2master_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprsmaster_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprsmaster_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#ifdef AREA_SHAANXI
uint8 shaan_index;
uint8 select_falg;
typedef struct 
{
  char ip_t[4];
  uint32 port_t;
  uint8 apnum;
  char apnvalue[64];
  uint8 namenum;
  char namevalue[64];
  uint8 passwdnum;
  char passwdvalue[64];
}shaanxi_channel_para;
const shaanxi_channel_para shaanxi_channel_param[MAX_SHAANXI_SELECT] = 
{
    {{192,168,100,11},6137,13,"cmiotsxgd.sn",      5,"card",                    5,"card"},
    {{192,168,100,11},6139,13,"cmiotsxgd.sn",      5,"card",                    5,"card"},
    {{192,168,100,11},6141,16,"sngw01.njm2mapn",   5,"card",                    5,"card"},
    {{192,168,100,11},6143,16,"sngw01.njm2mapn",   5,"card",                    5,"card"},
    {{192,168,100,11},6145,16,"m2msndl.vpdn.sn",  20,"m2m@m2msndl.vpdn.sn",    10,"vnet.mobi"},
    {{192,168,100,11},6147,16,"m2msndl.vpdn.sn",  20,"m2m@m2msndl.vpdn.sn",    10,"vnet.mobi"},
    {{192,168,100,11},6130, 8,"sxgd.sn",           5,"card",                    5,"card"},
    {{192,168,100,11},6131,17,"xasdl.yccb.snapn",  5,"card",                    5,"card"},
    {{192,168,100,11},6132,16,"m2msndl.vpdn.sn",   5,"card",                    5,"card"},
};

OOP_MASTERPARAMS_T shaanxi_GPRSMaster[MAX_SHAANXI_SELECT];
OOP_GPRSCONFIG_T   shaanxi_GprsConfig[MAX_SHAANXI_SELECT];

void shaanxi_uplink_para_init(OOP_MASTERPARAMS_T *master_para,OOP_GPRSCONFIG_T *link_para)
{
    uint8 i,tmpsize;
    //char master1ip[4] = {192,168,100,11};
    for(i=0;i<MAX_SHAANXI_SELECT;i++)
    {
      master_para[i].nNum = 2;
      memcpy(master_para[i].master[1].ip,GPRSMaster.master[1].ip,4);
      master_para[i].master[1].port = GPRSMaster.master[1].port;
      link_para[i].workmode = GprsConfig.workmode;
      link_para[i].linetype = GprsConfig.linetype;
      link_para[i].contype = GprsConfig.contype;
      link_para[i].conmode = GprsConfig.conmode;
      link_para[i].dail.mask.dailcnt = GprsConfig.dail.mask.dailcnt;
      link_para[i].dail.mask.dailtime = GprsConfig.dail.mask.dailtime;
      link_para[i].heart = GprsConfig.heart;

      memcpy(master_para[i].master[0].ip,shaanxi_channel_param[i].ip_t,4);
      master_para[i].master[0].port = shaanxi_channel_param[i].port_t;
      
      link_para[i].apn.apn.nNum = shaanxi_channel_param[i].apnum;
      tmpsize = shaanxi_channel_param[i].apnum;
      memcpy(link_para[i].apn.apn.value,shaanxi_channel_param[i].apnvalue,tmpsize);
      
      link_para[i].apn.username.nNum = shaanxi_channel_param[i].namenum;
      tmpsize = shaanxi_channel_param[i].namenum;
      memcpy(link_para[i].apn.username.value,shaanxi_channel_param[i].namevalue,tmpsize);
  
      link_para[i].apn.pwd.nNum = shaanxi_channel_param[i].passwdnum;
      tmpsize = shaanxi_channel_param[i].passwdnum;
      memcpy(link_para[i].apn.pwd.value,shaanxi_channel_param[i].passwdvalue,tmpsize);
  
    }
}
void get_uplink_paramodule(void)
{
    int i = 0;
    shaanxi_channel_para tmpbuf;
    memcpy(tmpbuf.ip_t,GPRSMaster.master[0].ip,4);
    tmpbuf.port_t = GPRSMaster.master[0].port;
    tmpbuf.apnum = GprsConfig.apn.apn.nNum;
    memcpy(tmpbuf.apnvalue,GprsConfig.apn.apn.value,tmpbuf.apnum);
    tmpbuf.namenum = GprsConfig.apn.username.nNum;
    memcpy(tmpbuf.namevalue,GprsConfig.apn.username.value,tmpbuf.namenum);
    tmpbuf.passwdnum = GprsConfig.apn.pwd.nNum;
    memcpy(tmpbuf.passwdvalue,GprsConfig.apn.pwd.value,tmpbuf.passwdnum);
    for(i = 0;i<MAX_SHAANXI_SELECT;i++)
    {
        if(tmpbuf.port_t == shaanxi_channel_param[i].port_t && (0==strcmp(tmpbuf.ip_t,shaanxi_channel_param[i].ip_t)) 
            && (0==strcmp(tmpbuf.apnvalue,shaanxi_channel_param[i].apnvalue)) && (0==strcmp(tmpbuf.namevalue,shaanxi_channel_param[i].namevalue))
            && (0==strcmp(tmpbuf.passwdvalue,shaanxi_channel_param[i].passwdvalue)))
            select_falg = i;
    }
}
void gprs1chanenl_param_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45000200;
    GPRSMasterOad.value = 0x45000300;
    select_falg = 0xff;
    memset(&shaanxi_GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T)*MAX_SHAANXI_SELECT);
    memset(&shaanxi_GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T)*MAX_SHAANXI_SELECT);
    //Ĭ�ϲ�������
    shaan_index = 0;
    GprsMasterIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    shaanxi_uplink_para_init(shaanxi_GPRSMaster,shaanxi_GprsConfig);
    get_uplink_paramodule();
}

void gprs1chanenl_param_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    char czBuf[300] = {0};
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    TextOut(hdc, nLeft1-10, nTop, "ѡ��ģʽ:");
    //nTop+=20;
    sprintf(czBuf,"%s",g_SHANXI_uplink_select[shaan_index]);
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1+45, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1+45, nTop, czBuf);
    }
    nTop+=15;
    TextOut(hdc, nLeft1-10, nTop, "��ǰģʽ:");
    if(select_falg == 0xff)
    TextOut(hdc, nLeft1+45, nTop, "�Զ���ģʽ");
    else
    {
        sprintf(czBuf,"%s",g_SHANXI_uplink_select[select_falg]);
        TextOut(hdc, nLeft1+45, nTop, czBuf);
    }
    nTop+=15;
    TextOut(hdc, nLeft1, nTop, "**********************");
    nTop+=10;
    sprintf(czBuf,"IP: %d.%d.%d.%d",shaanxi_GPRSMaster[shaan_index].master[0].ip[0],shaanxi_GPRSMaster[shaan_index].master[0].ip[1],shaanxi_GPRSMaster[shaan_index].master[0].ip[2],shaanxi_GPRSMaster[shaan_index].master[0].ip[3]);
    TextOut(hdc, nLeft1, nTop, czBuf);
    nTop+=10;
    sprintf(czBuf,"PORT: %d",shaanxi_GPRSMaster[shaan_index].master[0].port);
    TextOut(hdc, nLeft1, nTop, czBuf);
    nTop+=10;
    sprintf(czBuf,"APN: %s",shaanxi_GprsConfig[shaan_index].apn.apn.value);
    TextOut(hdc, nLeft1, nTop, czBuf);
    nTop+=15;
    sprintf(czBuf,"NAME: %s",shaanxi_GprsConfig[shaan_index].apn.username.value);
    TextOut(hdc, nLeft1, nTop, czBuf);
    nTop+=10;
    sprintf(czBuf,"PassWD: %s",shaanxi_GprsConfig[shaan_index].apn.pwd.value);
    TextOut(hdc, nLeft1, nTop, czBuf);
    nTop+=10;
    TextOut(hdc, nLeft1, nTop, "**********************");
    index++;
    nTop+=15;
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
    }
    index++;
    if (GprsMasterIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ȷ������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ȷ������");
    }
    PopListDraw(hdc);
}

BOOL gprs1chanenl_param_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    uint8 maxindex = 2;
    uint8 ret=0,ret1=0;
    OOP_MASTERPARAMS_T tmpGPRSMaster;
    OOP_GPRSCONFIG_T   tmpGprsConfig;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (GprsMasterIndex)
            {
            case 0://ѡ��Ĭ�ϵ�����ͨѶ����
            {
                shaan_index = nSel;
            }
            break;
            default:
            return FALSE;            
            }
            
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (GprsMasterIndex > 0)
            GprsMasterIndex--;
        else
            GprsMasterIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (GprsMasterIndex < maxindex)
            GprsMasterIndex++;
        else
            GprsMasterIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //Ĭ������ͨѶ����ѡ��
        if (GprsMasterIndex == 0)
        {
            PopList(g_SHANXI_uplink_select, 9, g_SHANXI_uplink_select[shaan_index], 20, 50, FALSE);
            return TRUE;
        }
        //ȡ��
        if (GprsMasterIndex == 1)
        {
            AppCancelMenu();
            GprsMasterIndex = 0;
            return FALSE;
        }
        //����
        if (GprsMasterIndex == 2)
        {
            if (TRUE == MessageBox("\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
               tmpGprsConfig = shaanxi_GprsConfig[shaan_index];
               tmpGPRSMaster = shaanxi_GPRSMaster[shaan_index];
               ret  = write_normal_data((uint8*)&tmpGPRSMaster, sizeof(OOP_MASTERPARAMS_T), GPRSMasterOad, 0, 0, 1);
               ret1 = write_normal_data((uint8*)&tmpGprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
               if(ret!=0&&ret1==0)
               {
                    MessageBox("���߹�����վ����д��ʧ�ܣ�");
               }
               else if(ret!=0&&ret1!=0)
               {
                   MessageBox("���߹���Ĭ�ϲ���д��ʧ�ܣ�");
               }
               else if(ret==0&&ret1!=0)
               {
                   MessageBox("���߹���ͨ�Ų���д��ʧ�ܣ�");
               }
               else if(ret==0&&ret1==0)
               {
                   MessageBox("���߹���Ĭ�ϲ���д��ɹ���");
                   select_falg = shaan_index;
               }
            }            
            GprsMasterIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;

        }
       }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_gprs1chanenl_param_set = {
    0x1103002c/*id*/,
    gprs1chanenl_param_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs1chanenl_param_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs1chanenl_param_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void gprs2chanenl_param_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45010200;
    GPRSMasterOad.value = 0x45010300;
    select_falg = 0xff;
    memset(&shaanxi_GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T)*MAX_SHAANXI_SELECT);
    memset(&shaanxi_GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T)*MAX_SHAANXI_SELECT);
    //Ĭ�ϲ�������
    shaan_index = 0;
    GprsMasterIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    memset(&GPRSMaster, 0x00, sizeof(OOP_MASTERPARAMS_T));
    read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
    read_db_noamal_data(g_gui_db_client, GPRSMasterOad.value, 0, 0, (uint8*)&GPRSMaster, sizeof(OOP_MASTERPARAMS_T));
    shaanxi_uplink_para_init(shaanxi_GPRSMaster,shaanxi_GprsConfig);
    get_uplink_paramodule();
    shaanxi_uplink_para_init(shaanxi_GPRSMaster,shaanxi_GprsConfig);
}

PLUGIN_INFO g_layer_gprs2chanenl_param_set = {
    0x1103002d/*id*/,
    gprs2chanenl_param_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs1chanenl_param_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs1chanenl_param_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

uint8 GprsPageIndex;    //ͨ��������Ҫ��ҳ��ʾ
int GprsConfigIsSet;

void gprs_display_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45000200;
    GprsPageIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}

void gprs_display_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    if (GprsPageIndex == 0)//��һҳ
    {
        TextOut(hdc, nLeft1, nTop, "����ģʽ");
        if (GprsConfigIsSet < 0 || GprsConfig.workmode>2)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "���߷�ʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.linetype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_line_type[GprsConfig.linetype]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.contype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_type[GprsConfig.contype]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����ģʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.conmode>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_app_type[GprsConfig.conmode]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "APN");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "�û���");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        nLeft2 += 20;
        ITEM_SELECT
            TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
    else    //�ڶ�ҳ
    {
        TextOut(hdc, nLeft1, nTop, "�����ַ");
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����˿�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d ��", GprsConfig.dail.mask.dailtime);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "�ط�����");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "��������");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d ��", GprsConfig.heart);
        }
        TextOut(hdc, nLeft2, nTop, czBuf);

        nTop += 15;
        nLeft2 += 20;
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��һҳ");
        ITEM_NOT_SELECT;
    }
}

BOOL gprs_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    if (nKey == KEY_OK)
    {
        if (GprsPageIndex == 0)
            GprsPageIndex = 1;
        else
            GprsPageIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    return FALSE;
}

//���߹���ͨ�����ò鿴
PLUGIN_INFO g_layer_gprs_display = {
    0x11030008/*id*/,
    gprs_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_display_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

uint8 GprsExpandPageIndex;    //��������Ϣ��Ҫ��ҳ��ʾ���·�ҳ
uint8 GprsnumnetPageIndex;   //���ҷ�ҳ
OOP_APNGROUP_T GprsExpand;   //���������ýṹ��
OOP_NETCONFIG_T Gprsnumnet;  //���������ýṹ��
int GprsConfigHandle;        //��db���ݾ����0�ɹ�������ʧ��
OOP_OAD_U GPRSExpandOad;     //��OAD

void gprs_numnet_init(HDC hdc)
{
    GPRSExpandOad.value = 0x45000e00;
    GprsExpandPageIndex = 0;
    GprsnumnetPageIndex = 0;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    GprsConfigHandle = read_db_noamal_data(g_gui_db_client, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}

void gprs_numnet_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "��%d���������",GprsnumnetPageIndex+1);
    rc.top = 20;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = rc.top+20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    memset(&Gprsnumnet, 0, sizeof(OOP_NETCONFIG_T));
    Gprsnumnet=GprsExpand.net[GprsnumnetPageIndex];
    if(GprsExpand.nNum!=0)
    {
        if (GprsExpandPageIndex == 0)  //��һҳ
        {   
            TextOut(hdc, nLeft1, nTop, "��Ӫ��");
            if (GprsConfigHandle < 0 || Gprsnumnet.factory==255)
            {
                sprintf(czBuf, "-");
            }
            else 
            {
                sprintf(czBuf, "%s", g_operator_seclct[Gprsnumnet.factory]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "��������");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.network==255)
            {
                sprintf(czBuf, "-");
            }
            else 
            {

                sprintf(czBuf, "%s", g_net_mode[Gprsnumnet.network]);
            }

            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "APN");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.apn.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.apn.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "�û���");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.username.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.username.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "����");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.pwd.nNum == 0)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%s", Gprsnumnet.apn.pwd.value);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "�����ַ");
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.proxyip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.apn.proxyip[0], Gprsnumnet.apn.proxyip[1],Gprsnumnet.apn.proxyip[2], Gprsnumnet.apn.proxyip[3]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);   
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "����˿�");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.apn.proxyport == 65535)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%d", Gprsnumnet.apn.proxyport);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
       }
        else                                            //�ڶ�ҳ
       {
          
            TextOut(hdc, nLeft1, nTop, "��Ȩ��ʽ");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.auth>4)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%s ", g_auth_mode[Gprsnumnet.auth]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "IP��ַ");
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[0].ip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[0].ip[0], Gprsnumnet.master.master[0].ip[1], Gprsnumnet.master.master[0].ip[2], Gprsnumnet.master.master[0].ip[3]);
            }
            
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "�˿�");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[0].port==0)
            {
                sprintf(czBuf, "-");
            }
            else
            {
            
                sprintf(czBuf, "%d ", Gprsnumnet.master.master[0].port);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "���õ�ַ");
            if (GprsConfigHandle < 0 || Gprsnumnet.master.master[1].ip[0] == 0)
            {
                sprintf(czBuf, "-.-.-.-");
            }
            else
            {
                sprintf(czBuf, "%d.%d.%d.%d", Gprsnumnet.master.master[1].ip[0], Gprsnumnet.master.master[1].ip[1], Gprsnumnet.master.master[1].ip[2], Gprsnumnet.master.master[1].ip[3]);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);

            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "���ö˿�");
            memset(czBuf, 0x00, sizeof(czBuf));
            if (GprsConfigHandle < 0|| Gprsnumnet.master.master[1].port == 65535)
            {
                sprintf(czBuf, "-");
            }
            else
            {

                sprintf(czBuf, "%d", Gprsnumnet.master.master[1].port);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
            
      }
   }
   else
   {
            nTop += 15;
            TextOut(hdc, nLeft1, nTop, "δ��鵽���������");
   }
}

BOOL gprs_numnet_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    switch (nKey)
        {
    case KEY_LEFT:
        if (GprsExpand.nNum!=0)
        {   
            if (GprsnumnetPageIndex == 0)
                {      
                GprsnumnetPageIndex =GprsExpand.nNum-1;
                GprsExpandPageIndex=0;
                }
            else 
                {
                GprsnumnetPageIndex--;
                GprsExpandPageIndex=0;
                }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        else
        {
            GprsnumnetPageIndex=0;
            GprsExpandPageIndex=0;
        }
        return TRUE;
        
    case  KEY_RIGHT:
        if (GprsExpand.nNum!=0)
        {
            if(GprsnumnetPageIndex == GprsExpand.nNum-1)
                {      
                GprsnumnetPageIndex = 0;
                GprsExpandPageIndex=0;
                }
            else 
                {
                GprsnumnetPageIndex++;
                GprsExpandPageIndex=0;
                }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        else
        {
            GprsnumnetPageIndex=0;
            GprsExpandPageIndex=0;
        }
        return TRUE;
        
    case  KEY_UP:
        if(GprsExpandPageIndex == 0)
            GprsExpandPageIndex = 1;
        else 
            GprsExpandPageIndex=0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    case  KEY_DOWN:
        if(GprsExpandPageIndex == 0)
            GprsExpandPageIndex = 1;
        else 
            GprsExpandPageIndex=0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    
        }
        return FALSE;

}


//���߹���1����������
PLUGIN_INFO g_layer_gprs_numnet = {
    0x1103000A/*id*/,
    gprs_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};

void gprs2_numnet_init(HDC hdc)
{
    GPRSExpandOad.value = 0x45010e00;
    GprsExpandPageIndex = 0;
    GprsnumnetPageIndex = 0;
    memset(&GprsExpand, 0x00, sizeof(OOP_APNGROUP_T));
    GprsConfigHandle = read_db_noamal_data(g_gui_db_client, GPRSExpandOad.value, 0, 0, (uint8*)&GprsExpand, sizeof(OOP_APNGROUP_T));
}


//���߹���2����������
PLUGIN_INFO g_layer_gprs2_numnet = {
    0x11030122/*id*/,
    gprs2_numnet_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_numnet_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_numnet_keymsg/*keymsg*/,
    NULL/*ipc*/
};


void gprs2_display_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45010200;
    GprsPageIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}

//���߹���ͨ�����ò鿴
PLUGIN_INFO g_layer_gprs2_display = {
    0x11030120/*id*/,
    gprs2_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_display_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};


uint8 GprsConfigIndex;
void gprs_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45000200;
    GprsPageIndex = 0;
    GprsConfigIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}

void gprs_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    uint8 index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    if (GprsPageIndex == 0)//��һҳ
    {

        TextOut(hdc, nLeft1, nTop, "����ģʽ");
        if (GprsConfigIsSet < 0 || GprsConfig.workmode>2)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            sprintf(czBuf, "%s", g_work_mode[GprsConfig.workmode]);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "���߷�ʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.linetype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_line_type[GprsConfig.linetype]);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "���ӷ�ʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.contype>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_type[GprsConfig.contype]);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����ģʽ");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.conmode>1)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", g_conn_app_type[GprsConfig.conmode]);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "APN");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "�û���");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.username.value);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        nLeft2 += 20;
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "��һҳ");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "��һҳ");
        }
        PopListDraw(hdc);
    }
    else    //�ڶ�ҳ
    {
        TextOut(hdc, nLeft1, nTop, "�����ַ");
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
        {
            sprintf(czBuf, "-.-.-.-");
        }
        else
        {
            sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "����˿�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "��ʱʱ��");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d ��", GprsConfig.dail.mask.dailtime);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "�ط�����");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "��������");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (GprsConfigIsSet < 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {

            sprintf(czBuf, "%d ��", GprsConfig.heart);
        }
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, czBuf);
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, czBuf);
        }

        index++;

        nTop += 15;
        nLeft2 += 20;
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "��һҳ");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "��һҳ");
        }

        index++;
        nTop += 25;

        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft1, nTop, "ȡ��");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft1, nTop, "ȡ��");
        }

        index++;
        nLeft2 = 80;
        if (GprsConfigIndex == index)
        {
            ITEM_SELECT;
            TextOut(hdc, nLeft2, nTop, "����");
            ITEM_NOT_SELECT;
        }
        else
        {
            TextOut(hdc, nLeft2, nTop, "����");
        }
    }
}

void GprsConfigReturn(char* pNewStr, void* pPara)
{
    char czBuf[20] = { 0 };
    uint8 tmp[4] = { 0 };
    if (GprsPageIndex == 0)
    {
        switch (GprsConfigIndex)
        {
        case 4: //APN
        {
            GprsConfig.apn.apn.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.apn.value, 0, sizeof(GprsConfig.apn.apn.value));
            memcpy(GprsConfig.apn.apn.value, pNewStr, GprsConfig.apn.apn.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 5: //�û���
        {
            GprsConfig.apn.username.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.username.value, 0, sizeof(GprsConfig.apn.username.value));
            memcpy(GprsConfig.apn.username.value, pNewStr, GprsConfig.apn.username.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        case 6: //����
        {
            GprsConfig.apn.pwd.nNum = strlen(pNewStr);
            memset(GprsConfig.apn.pwd.value, 0, sizeof(GprsConfig.apn.pwd.value));
            memcpy(GprsConfig.apn.pwd.value, pNewStr, GprsConfig.apn.pwd.nNum);
            GprsConfigIsSet = 1;
        }
        break;
        }
    }
    else
    {
        switch (GprsConfigIndex)
        {
        case 0: //�����������ַ
        {
            if (FALSE == commfun_IP(pNewStr, tmp))
            {
                if (TRUE == MessageBox("IP��ַ��ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {
                        sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                }
            }
            else
            {
                memcpy(GprsConfig.apn.proxyip, tmp, 4);
                GprsConfigIsSet = 1;
            }
        }
        break;
        case 1: //�˿�
        {
            int port = atoi(pNewStr);
            if (port <= 65535)
            {
                GprsConfig.apn.proxyport = port;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�˿ڸ�ʽ����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 2: //��ʱʱ��
        {
            int time = atoi(pNewStr);
            if (time < 0x3f)
            {
                GprsConfig.dail.mask.dailtime = time;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("��ʱʱ������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0 || GprsConfig.dail.mask.dailtime == 0x3f)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 3: //�ط�����
        {
            int cnt = atoi(pNewStr);
            if (cnt < 4)
            {
                GprsConfig.dail.mask.dailcnt = cnt;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("�ط���������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;
        case 4: //��������
        {
            int heart = atoi(pNewStr);
            if (heart < 65535)
            {
                GprsConfig.heart = heart;
                GprsConfigIsSet = 1;
            }
            else
            {
                if (TRUE == MessageBox("������������\n\nȷ���Ƿ���������?", MB_OKCANCEL))
                {
                    if (GprsConfigIsSet < 0)
                    {
                        strcpy(czBuf, "");
                    }
                    else
                    {

                        sprintf(czBuf, "%d", GprsConfig.heart);
                    }
                    InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                }
            }
        }
        break;

        }
    }
}

BOOL gprs_set_keymsg_page1(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (GprsConfigIndex)
            {
            case 0://����ģʽ
            {
                GprsConfig.workmode = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 1://���߷�ʽ
            {
                GprsConfig.linetype = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 2://���ӷ�ʽ
            {
                GprsConfig.contype = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            case 3://����ģʽ
            {
                GprsConfig.conmode = nSel;
                GprsConfigIsSet = 1;
            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (GprsConfigIndex > 0)
            GprsConfigIndex--;
        else
            GprsConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsConfigIndex < maxindex)
            GprsConfigIndex++;
        else
            GprsConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (GprsConfigIndex == 7)
        {
            GprsPageIndex = 1;
            GprsConfigIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (GprsConfigIndex)
            {
            case 0: //����ģʽ
            {
                PopList(g_work_mode, 3, g_work_mode[0], 70, 20, FALSE);
                return TRUE;
            }
            case 1: //����ģʽ
            {
                PopList(g_line_type, 2, g_line_type[0], 70, 35, FALSE);
                return TRUE;
            }
            case 2://���ӷ�ʽ
            {
                PopList(g_conn_type, 2, g_conn_type[0], 70, 50, FALSE);
                return TRUE;
            }
            case 3://����Ӧ�÷�ʽ
            {
                PopList(g_conn_app_type, 2, g_conn_app_type[0], 70, 65, FALSE);
                return TRUE;
            }
            case 4://APN
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.apn.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%s", GprsConfig.apn.apn.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 5://�û���
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.username.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {

                    sprintf(czBuf, "%s", GprsConfig.apn.username.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }
            case 6://����
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.pwd.nNum == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%s", GprsConfig.apn.pwd.value);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01 | 0x02 | 0x04 | 0x08 | 0x10, 0x04);
                return TRUE;
            }

            }
        }
    }
    }
    return FALSE;
}
BOOL gprs_set_keymsg_page2(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (GprsConfigIndex > 0)
            GprsConfigIndex--;
        else
            GprsConfigIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (GprsConfigIndex < maxindex)
            GprsConfigIndex++;
        else
            GprsConfigIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (GprsConfigIndex == 5)
        {
            GprsPageIndex = 0;
            GprsConfigIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        if (ValidatePwd() == TRUE)
        {
            switch (GprsConfigIndex)
            {
            case 0://�����ַ
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyip[0] == 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d.%d.%d.%d", GprsConfig.apn.proxyip[0], GprsConfig.apn.proxyip[1], GprsConfig.apn.proxyip[2], GprsConfig.apn.proxyip[3]);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x02);
                return TRUE;
            }
            case 1://����˿�
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0 || GprsConfig.apn.proxyport == 65535)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.apn.proxyport);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 2://��ʱʱ��
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailtime);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 3://���Դ���
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.dail.mask.dailcnt);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }
            case 4://����
            {
                char czBuf[20];
                if (GprsConfigIsSet < 0)
                {
                    strcpy(czBuf, "");
                }
                else
                {
                    sprintf(czBuf, "%d", GprsConfig.heart);
                }
                InputBox(czBuf, GprsConfigReturn, NULL, 0x01);
                return TRUE;
            }

            case 6://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
                    if (ret != 0)
                    {
                        MessageBox("���߹���ͨ������д��ʧ�ܣ�");
                    }
                }
                GprsPageIndex = 0;
                GprsConfigIndex = 0;
                return TRUE;
            }
            case 7://����
            {
                ret = write_normal_data((uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T), GPRSConfigOad, 0, 0, 1);
                if (ret != 0)
                {
                    MessageBox("���߹���ͨ������д��ʧ�ܣ�");
                }
                else
                {
                    MessageBox("���߹�����վ����д��ɹ���");
                }
                GprsPageIndex = 0;
                GprsConfigIndex = 0;
                return TRUE;
            }
            }
        }
    }

    }
    return FALSE;
}

BOOL gprs_set_keymsg(int nKey)
{
    if (GprsPageIndex == 0)
        return gprs_set_keymsg_page1(nKey);
    else
        return gprs_set_keymsg_page2(nKey);
}
//���߹���ͨ����������
PLUGIN_INFO g_layer_gprs_set = {
    0x11030018/*id*/,
    gprs_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
void gprs2_set_init(HDC hdc)
{
    GPRSConfigOad.value = 0x45010200;
    GprsPageIndex = 0;
    GprsConfigIndex = 0;
    memset(&GprsConfig, 0x00, sizeof(OOP_GPRSCONFIG_T));
    GprsConfigIsSet = read_db_noamal_data(g_gui_db_client, GPRSConfigOad.value, 0, 0, (uint8*)&GprsConfig, sizeof(OOP_GPRSCONFIG_T));
}
//���߹���2ͨ����������
PLUGIN_INFO g_layer_gprs2_set = {
    0x1103002a/*id*/,
    gprs2_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gprs_set_draw/*Draw*/,
    NULL/*Timer*/,
    gprs_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//������Ϣ��Ż�ȡ������Ϣ
int get_meterfile(uint16 infonum, OOP_METER_T* MeterFile)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad = MeterFileOad;
    NormalOad.infoNum = infonum;
    OOP_METER_T Meter;
    memset(&Meter, 0, sizeof(OOP_METER_T));
    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_METER_T), (uint8*)&Meter, &len);
    if (ret != 0 || len != sizeof(OOP_METER_T))
        return -1;
    memcpy(MeterFile, &Meter, sizeof(OOP_METER_T));
    return 0;
}

//���˿�OADת��Ϊ����
void get_portname(OOP_OAD_U port, char* name)
{
    switch (port.nObjID)
    {
    case 0xF209:
    {
        if (port.attID == 2)
        {
            sprintf(name, "�ز�-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF201:
    {
        if (port.attID == 2)
        {
            sprintf(name, "485-%d", port.nIndex);
            return;
        }
    }
    break;
    case 0xF208:
    {
        sprintf(name, "���ɶ˿�");
        return;
    }
    case 0xF20A:
    {
        if (port.attID == 2)
        {
            sprintf(name, "����-%d", port.nIndex);
            return;
        }
    }
    default:
        break;
    }
    sprintf(name, "-");

}

void meterfile_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
    get_meterfile(g_guiComm.nCurPoint, &MeterFile);
}
void meterfile_display_draw(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    advance_gui_infonum(g_guiComm.nCurPoint);

    //���������ű䣬��Ӧ����û�и���
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
    get_meterfile(g_guiComm.nCurPoint, &MeterFile);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    if (MeterFile.nIndex != 0)
        flag = 1;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "ͨ�ŵ�ַ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 Addr[6] = { 0 };
        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�����ʣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��Լ���ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 prtl = MeterFile.basic.protocol;
        if (prtl > 4)
            prtl = 0;
        sprintf(czBuf, "%s", g_prtl[prtl]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿ڣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        get_portname(MeterFile.basic.port, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}
BOOL meterfile_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_METER_INFONUM;

            memset(&MeterFile, 0, sizeof(OOP_METER_T));
            get_meterfile(g_guiComm.nCurPoint, &MeterFile);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_guiComm.nCurPoint < MAX_METER_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&MeterFile, 0, sizeof(OOP_METER_T));
            get_meterfile(g_guiComm.nCurPoint, &MeterFile);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);

            return TRUE;
        }
    }
    return FALSE;
}
//������鿴
PLUGIN_INFO g_layer_meterfile_display = {
    0x11030003/*id*/,
    meterfile_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    meterfile_display_draw/*Draw*/,
    NULL/*Timer*/,
    meterfile_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#if DESC("��ͨ�ɼ������鿴",1)
MEASURE_MAIN    g_normal_plan_csd_data;
OOP_PLAN_NORMAL_T   g_guiNormalPlan;

//������ͨ������Ϣ��Ż�ȡ��ͨ�ɼ�����
int normal_plan_read(uint16 id,OOP_PLAN_NORMAL_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60140200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_PLAN_NORMAL_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_NORMAL_T))
        return -1 ;
    return 0;
}

void csd2str(char *str, OOP_RECORD_COL_T recdCol)
{
    int i, j;
    int roadColNum;

    for (i = 0; i < recdCol.num; i++) //csd����
    {
        if (recdCol.csd[i].choice == 0) //oad
        {
            sprintf(str + strlen(str), "*00%08X", recdCol.csd[i].oad.value);
        }
        else //road
        {
            roadColNum = recdCol.csd[i].road.oadCols.nNum;
            sprintf(str + strlen(str), "*%08X:", recdCol.csd[i].oad.value);
            sprintf(str + strlen(str), "%02X", roadColNum);
            for (j = 0; j < roadColNum; j++)
            {
                sprintf(str + strlen(str), "%08X,", recdCol.csd[i].road.oadCols.oad[j].value);
            }
            if (roadColNum != 0)
            {
                str[strlen(str) - 1] = '\0'; //ȥ�����һ������
            }
        }
    }
}

void normal_plan_csd_display_init(HDC hdc)
{
}

void normal_plan_csd_display_draw(HDC hdc)
{
    int index = 0;
    int len;
    int nTop = 20;
    int nLeft = 3;
    char czBuf[300] = { 0 };
    char tBuf[27] = {0};    //һ����ʾ26���ַ�

    csd2str(czBuf, g_guiNormalPlan.recdCol);
    len = strlen(czBuf);
        
    //GUI_FMT_DEBUG("czBuf: %s\n", czBuf);
    while (index < len - 26)
    {
        memcpy(tBuf, &czBuf[index], 26);
        tBuf[26] = '\0';
        index += 26;
        TextOut(hdc, nLeft, nTop, tBuf);
        nLeft = 3;
        nTop += 10;
    }
    if (index < len)
    {
        memcpy(tBuf, &czBuf[index], len - index);
        tBuf[len - index] = '\0';
        TextOut(hdc, nLeft, nTop, tBuf);
    }
}

BOOL normal_plan_csd_display_keymsg(int nKey)
{
    return FALSE;
}

//csd��������鿴��ť
PLUGIN_INFO g_layer_normal_plan_csd_display = {
    0x00/*id*/,
    normal_plan_csd_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_csd_display_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_csd_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//csd��������鿴��ť
PLUGIN_INFO* g_normal_plan_csd_display_plugin[] =
{
    &g_layer_normal_plan_csd_display,
};

void normal_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
    normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void normal_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[1300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //���������ű䣬��Ӧ����û�и���
    memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
    normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);

    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_csd_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiNormalPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢��ȣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiNormalPlan.memDepth);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ����ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.recdRow.choice;
        if (index > 4)
            index = 0;
        sprintf(czBuf, "%s", g_normalPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || (index != 0 && index != 2))
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "ֵ�뵥λ��");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {  
            if (index == 1) //�ɼ��ϵ�N��
            {
                sprintf(czBuf, "%d��", g_guiNormalPlan.recdRow.num);
            }
            else if (index == 3) //��ʱ�����ɼ�
            {
                sprintf(czBuf, "%d%s", g_guiNormalPlan.recdRow.ti.value, g_ti[g_guiNormalPlan.recdRow.ti.unit]);
            }
            else //����
            {
                sprintf(czBuf, "%d%s", g_guiNormalPlan.recdRow.rd.ti.value, g_ti[g_guiNormalPlan.recdRow.rd.ti.unit]);
            }
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "������ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.ms.choice;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_ms[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || index == 2 || index == 4)
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "������ã�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            int i;
            if (index == 2) //һ���û�����
            {
                for (i = 0; i <  g_guiNormalPlan.ms.mst.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiNormalPlan.ms.mst.type[i]);
            }
            else if (index == 4) //һ���������
            {
                for (i = 0; i <  g_guiNormalPlan.ms.msi.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiNormalPlan.ms.msi.idx[i]);
            }     
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢ʱ�꣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiNormalPlan.tmFlag;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_tmFlag[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ѡ�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "�鿴����");
    }
    if (g_normal_plan_csd_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

void normal_plan_display_draw(HDC hdc)
{
    // ���Ʋ鿴CSDҳ��
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    normal_plan_display_draw_1st(hdc);
    
}

BOOL normal_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_csd_data.nIndex--;
            if (g_normal_plan_csd_data.nIndex < 0)
                g_normal_plan_csd_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_csd_data.nIndex++;
            if (g_normal_plan_csd_data.nIndex > 1)
                g_normal_plan_csd_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
            normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
            normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_csd_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                g_normal_plan_csd_data.nCurPlugin = 0;
                g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                }
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

//��ͨ�����鿴
PLUGIN_INFO g_layer_normal_plan_display = {
    0x11030070/*id*/,
    normal_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    normal_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�¼��ɼ������鿴",1)
MEASURE_MAIN        g_normal_plan_road_data;
OOP_PLAN_EVENT_T    g_guiEventPlan;

//������Ϣ��Ż�ȡ�¼��ɼ�����
int event_plan_read(uint16 id, OOP_PLAN_EVENT_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60160200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_PLAN_EVENT_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_EVENT_T))
        return -1 ;
    return 0;
}

void road2str(char *str, OOP_EVENTSELECT_T recdCol)
{
    uint32 i, j;
    uint32 roadColNum;

    for (i = 0; i < recdCol.nNum; i++) //road����
    {

        roadColNum = recdCol.cols[i].oadCols.nNum;
        sprintf(str + strlen(str), "*%08X:", recdCol.cols[i].oadMain.value);
        sprintf(str + strlen(str), "%02X", roadColNum);
        for (j = 0; j < roadColNum; j++)
        {
            sprintf(str + strlen(str), "%08X,", recdCol.cols[i].oadCols.oad[j].value);
        }
        if (roadColNum != 0)
        {
            str[strlen(str) - 1] = '\0'; //ȥ�����һ������
        }
    }
}

void event_plan_road_display_init(HDC hdc)
{
}

void event_plan_road_display_draw(HDC hdc)
{
    int index = 0;
    int len;
    int nTop = 20;
    int nLeft = 3;
    char czBuf[300] = { 0 };
    char tBuf[27] = {0};    //һ����ʾ26���ַ�

    road2str(czBuf, g_guiEventPlan.events);
    len = strlen(czBuf);
        
    //GUI_FMT_DEBUG("czBuf: %s\n", czBuf);
    while (index < len - 26)
    {
        memcpy(tBuf, &czBuf[index], 26);
        tBuf[26] = '\0';
        index += 26;
        TextOut(hdc, nLeft, nTop, tBuf);
        nLeft = 3;
        nTop += 10;
    }
    if (index < len)
    {
        memcpy(tBuf, &czBuf[index], len - index);
        tBuf[len - index] = '\0';
        TextOut(hdc, nLeft, nTop, tBuf);
    }
}

BOOL event_plan_road_display_keymsg(int nKey)
{
    return FALSE;
}

//csd��������鿴��ť
PLUGIN_INFO g_layer_event_plan_road_display = {
    0x00/*id*/,
    event_plan_road_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    event_plan_road_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_plan_road_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//csd��������鿴��ť
PLUGIN_INFO* g_event_plan_road_display_plugin[] =
{
    &g_layer_event_plan_road_display,
};

void event_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
    event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);

    g_normal_plan_road_data.ppDetail = NULL;
    g_normal_plan_road_data.pCurPlugin = NULL;
    g_normal_plan_road_data.nIndex = 0;
}

void event_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //���������ű䣬��Ӧ����û�и���
    memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
    event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);

    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_road_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiEventPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢��ȣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiEventPlan.nDepth);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ����ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.nType;
        if (index > 3)
            index = 0;
        sprintf(czBuf, "%s", g_eventPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "������ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.ms.choice;
        if (index > 7)
            index = 0;
        sprintf(czBuf, "%s", g_ms[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    if (flag == 0 || index == 2 || index == 4)
    {
        nTop += 15;
        TextOut(hdc, nLeft1, nTop, "������ã�");
        memset(czBuf, 0x00, sizeof(czBuf));
        if (flag == 0)
        {
            sprintf(czBuf, "-");
        }
        else
        {
            int i;
            if (index == 2) //һ���û�����
            {
                for (i = 0; i <  g_guiEventPlan.ms.mst.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiEventPlan.ms.mst.type[i]);
            }
            else if (index == 4) //һ���������
            {
                for (i = 0; i <  g_guiEventPlan.ms.msi.size; i++)
                    sprintf(czBuf + strlen(czBuf), "%d ", g_guiEventPlan.ms.msi.idx[i]);
            }     
        }
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ϱ���ʶ");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiEventPlan.bReport;
        if (index > 1)
            index = 0;
        sprintf(czBuf, "%s", g_eventReportFlag[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "�鿴����");
    }
    if (g_normal_plan_road_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

void event_plan_display_draw(HDC hdc)
{
    // ���Ʋ鿴ROADҳ��
    if (g_normal_plan_road_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_road_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_road_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    event_plan_display_draw_1st(hdc);
    
}

BOOL event_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_road_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_road_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_road_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_road_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_road_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_road_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_road_data.nIndex--;
            if (g_normal_plan_road_data.nIndex < 0)
                g_normal_plan_road_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_road_data.nIndex++;
            if (g_normal_plan_road_data.nIndex > 1)
                g_normal_plan_road_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
            event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiEventPlan, 0, sizeof(OOP_PLAN_EVENT_T));
            event_plan_read(g_guiComm.nCurPoint, &g_guiEventPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_road_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_road_data.ppDetail = g_event_plan_road_display_plugin;
                g_normal_plan_road_data.nPluginCnt = sizeof(g_event_plan_road_display_plugin) / sizeof(g_event_plan_road_display_plugin[0]);
                g_normal_plan_road_data.nCurPlugin = 0;
                g_normal_plan_road_data.pCurPlugin = g_normal_plan_road_data.ppDetail[g_normal_plan_road_data.nCurPlugin];
                ASSERT(g_normal_plan_road_data.pCurPlugin != NULL);

                if (g_normal_plan_road_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_road_data.pCurPlugin->pfnInitLayer(g_normal_plan_road_data.pCurPlugin->dwId);
                }
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}


//�¼������鿴
PLUGIN_INFO g_layer_event_plan_display = {
    0x11030071/*id*/,
    event_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    event_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    event_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ϱ������鿴",1)
OOP_PLAN_REPORT_T   g_guiReportPlan;

//������Ϣ��Ż�ȡ�¼��ɼ�����
int report_plan_read(uint16 id, OOP_PLAN_REPORT_T *PlanInfo)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x601C0200;
    NormalOad.infoNum = id;

    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_PLAN_REPORT_T), (uint8*)PlanInfo, &len);

    if(ret !=0 ||len!=sizeof(OOP_PLAN_REPORT_T))
        return -1 ;
    return 0;
}

void report_plan_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
    report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void report_plan_display_draw_1st(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };
    uint8 index = 0;

    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;

    //���������ű䣬��Ӧ����û�и���
    memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
    report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);

    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);

    if (g_normal_plan_csd_data.nIndex == 0)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    if (g_guiReportPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ϱ�ͨ����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {  
        sprintf(czBuf, "%08X", g_guiReportPlan.channel.oad[0].value);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱʱ�䣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d %s", g_guiReportPlan.timeout.value, g_ti[g_guiReportPlan.timeout.unit]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "������ԣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiReportPlan.retry);
    }
    TextOut(hdc, nLeft2, nTop, czBuf); 


    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ϱ����ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiReportPlan.data.choice;
        if (index > 1)
            index = 0;
        sprintf(czBuf, "%s", g_reportPlanType[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "RSD��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        index = g_guiReportPlan.data.record.tRSD.choice;
        if (index > 10)
            index = 0;
        sprintf(czBuf, "%s", g_reportPlanRSD[index]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "RCSD��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "�鿴����");
    }
    if (g_normal_plan_csd_data.nIndex == 1)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
}

void report_plan_display_draw(HDC hdc)
{
    // ���Ʋ鿴CSDҳ��
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    report_plan_display_draw_1st(hdc);
    
}

BOOL report_plan_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }

    switch (nKey)
    {  
        case KEY_UP:
        {
            g_normal_plan_csd_data.nIndex--;
            if (g_normal_plan_csd_data.nIndex < 0)
                g_normal_plan_csd_data.nIndex = 1;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_DOWN:
        {
            g_normal_plan_csd_data.nIndex++;
            if (g_normal_plan_csd_data.nIndex > 1)
                g_normal_plan_csd_data.nIndex = 0;

            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_PLAN_INFONUM;

            memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
            report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&g_guiReportPlan, 0, sizeof(OOP_PLAN_REPORT_T));
            report_plan_read(g_guiComm.nCurPoint, &g_guiReportPlan);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if (g_normal_plan_csd_data.nIndex == 0)
            {
                char bzbuf[50];
                sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
                InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            }
            else
            {
                g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                g_normal_plan_csd_data.nCurPlugin = 0;
                g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                }
            }
            if (g_guiReportPlan.data.choice == 1) //��¼��
            {
                g_guiNormalPlan.recdCol.num = g_guiReportPlan.data.record.tRCSD.nNum;
                memcpy(g_guiNormalPlan.recdCol.csd, g_guiReportPlan.data.record.tRCSD.cols, sizeof(OOP_CSD_T) * OOP_MAX_CSDS);
            }            
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}

//�ϱ������鿴
PLUGIN_INFO g_layer_report_plan_display = {
    0x11030072/*id*/,
    report_plan_display_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    report_plan_display_draw/*Draw*/,
    NULL/*Timer*/,
    report_plan_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif

#if DESC("�ɼ�����鿴",1)
//��������id��ȡ������Ϣ
int get_taskinfo(uint16 Taskid, OOP_TASK_T* TaskInfo)
{
    int   ret;
    uint32 len = 0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60120200;
    NormalOad.infoNum = Taskid;
    OOP_TASK_T task;
    memset(&task, 0, sizeof(OOP_TASK_T));
    ret = db_read_nomal(g_gui_db_client, &NormalOad, sizeof(OOP_TASK_T), (uint8*)&task, &len);
    if (ret != 0 || len != sizeof(OOP_TASK_T))
        return -1;
    memcpy(TaskInfo, &task, sizeof(OOP_TASK_T));
    return 0;
}

uint16 TaskIndex;
OOP_TASK_T TaskInfo;
void task_display_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
    get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
}

//��OOP_DATETIME_S_T��ʽ��ʱ��ת��Ϊ�ַ���
void ooptime_to_str(OOP_DATETIME_S_T time, char* buf)
{
    sprintf(buf, "%d/%d/%d %d:%d:%d", time.year, time.month, time.day, time.hour, time.minute, time.second);
}

void task_display_draw(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "����ID��");
    
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    ITEM_SELECT;
    TextOut(hdc, nLeft2, nTop, czBuf);
    ITEM_NOT_SELECT;
    
    if (TaskInfo.id != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "ִ�м����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d %s", TaskInfo.acqFreq.value, g_ti[TaskInfo.acqFreq.unit]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�������ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (TaskInfo.planType > 0 && TaskInfo.planType < 6)
            sprintf(czBuf, "%s", g_plantype[TaskInfo.planType - 1]);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", TaskInfo.planID);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʼʱ�䣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        ooptime_to_str(TaskInfo.startTime, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����ʱ�䣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        ooptime_to_str(TaskInfo.endTime, czBuf);
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "����״̬��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%s", TaskInfo.state == 1 ? "����" : "ͣ��");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}

BOOL task_display_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_LEFT:
        {
            if (g_guiComm.nCurPoint > 0)
                g_guiComm.nCurPoint--;
            if (g_guiComm.nCurPoint == 0)
                g_guiComm.nCurPoint = MAX_TASK_INFONUM;
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        {
            if (g_guiComm.nCurPoint < MAX_TASK_INFONUM)
            {
                g_guiComm.nCurPoint++;
            }
            else
            {
                g_guiComm.nCurPoint = 1;
            }
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
    case KEY_OK:
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            UpdateWindow(g_guiComm.hMainWnd, TRUE);

            return TRUE;
        }
    }
    return FALSE;
}
//�ɼ�����鿴
PLUGIN_INFO g_layer_task_display = {
    0x11030004/*id*/,
    task_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    task_display_draw/*Draw*/,
    NULL/*Timer*/,
    task_display_keymsg/*keymsg*/,
    NULL/*ipc*/
};
#endif
int HPLC_set_index;
uint8 HPLCChannel;      //HPLCƵ��F2090900
int ChannelIsSet;     //�Ƿ�����
BOOL recvbit;
BOOL Hchannel_bit;
/*����11��̨��ʶ����ͣ��ǣ���=unsigned
������־��
bit0��̨��ʶ��ʹ�ܱ�־��0����ֹ��1������
bit1��̨������ʹ�ܱ�־��0����ֹ��1������
bit2��������֪ʹ�ܱ�־��0����ֹ��1������
bit3~bit7�����á�
*/
uint8 HPLCAcrossAreaFlag;  //F2090B00 ̨��ʶ��
int AcrossAreaFlagIsSet;

//����22���ܾ��ڵ��ϱ�ʹ�ܱ�־����= enum{��ֹ��0����ʹ�ܣ�1��}
uint8 HPLCNodeRejectFlag;       //F2091600
int NodeRejectFlagIsSet;

//�ѱ�״̬ ����8 60020a00
uint8 SearchMeterStatus;
int SearchMeterFlagIsSet;


void HPLC_display_init(HDC hdc)
{
    AcrossAreaFlagIsSet = read_db_noamal_data(g_gui_db_client, 0xF2090B00, 0, 0, (uint8*)&HPLCAcrossAreaFlag, 1);
    NodeRejectFlagIsSet = read_db_noamal_data(g_gui_db_client, 0xF2091600, 0, 0, (uint8*)&HPLCNodeRejectFlag, 1);
    SearchMeterFlagIsSet = read_db_noamal_data(g_gui_db_client, 0x60020a00, 0, 0, (uint8*)&SearchMeterStatus, 1);
    printf("AcrossAreaFlagIsSet %d NodeRejectFlagIsSet %d", AcrossAreaFlagIsSet, NodeRejectFlagIsSet);
}
void HPLC_display_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 120;

    TextOut(hdc, nLeft1, nTop, "̨��ʶ�𿪹أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        uint8 tmp = HPLCAcrossAreaFlag & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "�ر�" : "����");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "������֪���أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        uint8 tmp = (HPLCAcrossAreaFlag >> 2) & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "�ر�" : "����");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);

    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�ܾ��ڵ��ϱ����أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (NodeRejectFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        sprintf(czBuf, "%s", HPLCNodeRejectFlag == 0 ? "�ر�" : "����");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
    
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�ѱ�״̬��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (SearchMeterFlagIsSet < 0)
    {
        sprintf(czBuf, "����");
    }
    else
    {
        sprintf(czBuf, "%s", SearchMeterStatus == 0 ? "����" : "�ѱ���");
    }
    TextOut(hdc, nLeft2, nTop, czBuf);
}
//HPLC�����鿴
PLUGIN_INFO g_layer_HPLC_display = {
    0x11030005/*id*/,
    HPLC_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HPLC_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

void address_set_init(HDC hdc)
{
    get_addr(LocalAddr);
}

void address_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
#ifdef AREA_HUNAN
    MessageBox("�����ն�ͨ�ŵ�ַ\n��֧������!");
    AppCancelMenu();
    return;
#endif


    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ն�ͨ�ŵ�ַ");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
}

void AddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    int ret;

#ifdef AREA_ZHEJIANG
    if (nLen > 8 && TRUE == MessageBox("OOP�ն˵�ַ���ȴ���4�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "00000000");
        strcpy(tmp + 8 - nLen, pNewStr);
        printf("����ĵ�ַΪ %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 8, 0);
        printf("ת��Ϊbcd %02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 4;
        memcpy(TmpLocalAddr.value, LocalAddr, 4);
        OOP_OAD_U oad = { 0x40010200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("�ն˵�ַ���óɹ�");
        }
        else
        {
            MessageBox("�ն˵�ַ����ʧ��");
        }
    }
#else
    if (nLen > 12 && TRUE == MessageBox("OOP�ն˵�ַ���ȴ���6�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        char tmp[20] = { 0 };
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        InputBox(tmp, AddrReturn, NULL, 0x01);
    }
    else
    {
        char tmp[20] = { 0 };
        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("����ĵ�ַΪ %s \n", tmp);
        commfun_ASC2BCD((char*)LocalAddr, tmp, 12, 0);
        printf("ת��Ϊbcd %02x%02x%02x%02x%02x%02x \n", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
        OOP_OCTETVAR16_T TmpLocalAddr;
        memset(&TmpLocalAddr, 0, sizeof(OOP_OCTETVAR16_T));
        TmpLocalAddr.nNum = 6;
        memcpy(TmpLocalAddr.value, LocalAddr, 6);
        OOP_OAD_U oad = { 0x40010200 };
        ret = write_normal_data((uint8*)&TmpLocalAddr, sizeof(OOP_OCTETVAR16_T), oad, 0, 0, 1);
        if (ret == 0)
        {
            MessageBox("�ն˵�ַ���óɹ�");
        }
        else
        {
            MessageBox("�ն˵�ַ����ʧ��");
        }
    }
#endif
}

BOOL address_set_keymsg(int nKey)
{

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    switch (nKey)
    {
        case KEY_OK:
        {
            if (ValidatePwd() == TRUE)
            {
                char tmp[20] = {0};
#ifdef AREA_ZHEJIANG
                sprintf(tmp, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
                sprintf(tmp, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
                InputBox(tmp, AddrReturn, NULL, 0x01);
                return TRUE;
            }
            break;
        }
        default: break;
    }
    return FALSE;
}

//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_address_set = {
    0x11030011/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    address_set_draw/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};



uint8 meterfile_set_index;
void meterfile_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&MeterFile, 0, sizeof(OOP_METER_T));
    get_meterfile(g_guiComm.nCurPoint, &MeterFile);
    meterfile_set_index = 0;
}

void meterfile_set_draw(HDC hdc)
{
    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    advance_gui_infonum(g_guiComm.nCurPoint);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    if (MeterFile.nIndex != 0)
        flag = 1;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "ͨ�ŵ�ַ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 Addr[6] = { 0 };
        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }


    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�����ʣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 index = MeterFile.basic.baud;
        if (index > 11)
            index = 11;
        sprintf(czBuf, "%s", g_baud[index]);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��Լ���ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 prtl = MeterFile.basic.protocol;
        if (prtl > 4)
            prtl = 0;
        sprintf(czBuf, "%s", g_prtl[prtl]);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�˿ڣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        get_portname(MeterFile.basic.port, czBuf);
    }
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;

    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 50;
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ɾ��");
    }

    index++;
    int nLeft3 = 90;
    if (meterfile_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "��������");
    }

    PopListDraw(hdc);
}

//���õ������ַ
void MeterAddrReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    uint8 Addr[20] = { 0 };
    char tmp[20] = { 0 };
    if (nLen > 12 && TRUE == MessageBox("���õ���ַ���ȴ���6�ֽ�\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {

        memcpy(Addr, MeterFile.basic.tsa.add, 6);
        sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
        InputBox(tmp, MeterAddrReturn, NULL, 0x01);
    }
    else
    {

        strcpy(tmp, "000000000000");
        strcpy(tmp + 12 - nLen, pNewStr);
        printf("����ĵ�ַΪ %s \n", tmp);
        commfun_ASC2BCD((char*)Addr, tmp, 12, 0);
        printf("ת��Ϊbcd %02x%02x%02x%02x%02x%02x \n", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
        MeterFile.basic.tsa.len = 5;
        memcpy(MeterFile.basic.tsa.add, Addr, 6);
//      memcpy_rev(MeterFile.basic.tsa.add, Addr, 6);
        MeterFile.nIndex = g_guiComm.nCurPoint;
    }

}

OOP_OAD_U get_set_pipe(int index)
{
    OOP_OAD_U oad;
    memset(&oad, 0x00, sizeof(OOP_OAD_U));
    switch (index)
    {
    case 0:
        oad.value = 0xF2090201;
        break;
    case 1:
        oad.value = 0xF2090202;
        break;
    case 2:
        oad.value = 0xF2010201;
        break;
    case 3:
        oad.value = 0xF2010202;
        break;
    case 4:
        oad.value = 0xF2010203;
        break;
    case 5:
        oad.value = 0xF2010204;
        break;
    case 6:
        oad.value = 0xF2010205;
        break;
    case 7:
        oad.value = 0xF2010206;
        break;
    case 8:
        oad.value = 0xF2010207;
        break;
    case 9:
        oad.value = 0xF2010208;
        break;
    case 10:
        oad.value = 0xF2080208;
        break;
    default:
        break;
    }
    return oad;
}

//ɾ�������
void delete_meterfile(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = MeterFileOad;
	RecordDel.classtag = CLASS_DATA_INIT;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_gui_db_client, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("����ɾ���ɹ� infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("����ɾ��ʧ�� infonum %d \n", infonum);
    }

}

BOOL meterfile_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 7;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (meterfile_set_index)
            {
            case 2://������
            {
                MeterFile.basic.baud = nSel;
            }
            break;
            case 3://��Լ����
            {
                MeterFile.basic.protocol = nSel;
            }
            break;
            case 4://�˿�
            {
                MeterFile.basic.port = get_set_pipe(nSel);
            }
            break;
            default:
                return FALSE;
            }
            MeterFile.nIndex = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }


    switch (nKey)
    {
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            g_guiComm.nCurPoint = MAX_METER_INFONUM;
        memset(&MeterFile, 0, sizeof(OOP_METER_T));
        get_meterfile(g_guiComm.nCurPoint, &MeterFile);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        if (g_guiComm.nCurPoint < MAX_METER_INFONUM)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        memset(&MeterFile, 0, sizeof(OOP_METER_T));
        get_meterfile(g_guiComm.nCurPoint, &MeterFile);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (meterfile_set_index > 0)
            meterfile_set_index--;
        else
            meterfile_set_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (meterfile_set_index < maxindex)
            meterfile_set_index++;
        else
            meterfile_set_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (meterfile_set_index == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
//          UpdateWindow(g_guiComm.hMainWnd, TRUE);

            return TRUE;
        }
        
        //���õ���ַ
        if (meterfile_set_index == 1)
        {
            uint8 Addr[6] = { 0 };
            char tmp[20] = { 0 };
            memcpy(Addr, MeterFile.basic.tsa.add, 6);
            sprintf(tmp, "%02x%02x%02x%02x%02x%02x", Addr[0], Addr[1], Addr[2], Addr[3], Addr[4], Addr[5]);
            if (strcmp(tmp, "000000000000") == 0)
                strcpy(tmp, "");
            InputBox(tmp, MeterAddrReturn, NULL, 0x01);
            return TRUE;
        }
        //���ò�����
        if (meterfile_set_index == 2)
        {
            printf("���ò�����\n");
            PopList(g_baud, 12, g_baud[0], 70, 40, FALSE);
            return TRUE;
        }
        //��Լ����
        if (meterfile_set_index == 3)
        {
            printf("���ù�Լ����\n");
            PopList(g_prtl, 5, g_prtl[0], 70, 40, FALSE);
            return TRUE;
        }
        //�˿�
        if (meterfile_set_index == 4)
        {
            printf("���ö˿�\n");
            PopList(g_pipe, 11, g_pipe[0], 70, 40, FALSE);
            return TRUE;
        }
        //ȡ��
        if (meterfile_set_index == 5)
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("��ǰ�������ñ���ɹ���");
                }
            }
            meterfile_set_index = 0;
            return FALSE;
        }
        //ɾ��
        if (meterfile_set_index == 6)
        {
            if (TRUE == MessageBox("ȷ��ɾ����ǰ�������", MB_OKCANCEL))
            {
                delete_meterfile(g_guiComm.nCurPoint);
                g_guiComm.nCurPoint = 1;
                get_meterfile(g_guiComm.nCurPoint, &MeterFile);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            meterfile_set_index = 0;
            return FALSE;
        }
        //��������
        if (meterfile_set_index == 7)
        {
            ret = write_normal_data((uint8*)&MeterFile, sizeof(OOP_METER_T), MeterFileOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("��ǰ�������ñ���ɹ���");
                meterfile_set_index = 0;
                return TRUE;
            }
            else
            {
                MessageBox("��ǰ�������ñ���ʧ�ܣ�");
                return FALSE;
            }

        }
    }
    }
    return FALSE;
}

//���������
PLUGIN_INFO g_layer_meterfile_set = {
    0x11030013/*id*/,
    meterfile_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    meterfile_set_draw/*Draw*/,
    NULL/*Timer*/,
    meterfile_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#if DESC("��ͨ�ɼ�����MS����",1)
uint8       g_normalPlanMsType;
OOP_MSP_T   g_normalPlanMsSet;
int8        g_normalPlanMsSetIndex;
#define     NORMAL_PLAN_MS_REGION_START 0
#define     NORMAL_PLAN_MS_REGION_END               1

void normal_plan_ms_sprintf(char *str, uint8 type, uint8 state)
{
    switch (type)
    {
        case 2: //һ���û�����
            sprintf(str, "%d", g_normalPlanMsSet.mst.type[0]);
            break;
        case 3: //һ���û���ַ
            sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msa.add[0].add[0], g_normalPlanMsSet.msa.add[0].add[1],
                        g_normalPlanMsSet.msa.add[0].add[2], g_normalPlanMsSet.msa.add[0].add[3],
                        g_normalPlanMsSet.msa.add[0].add[4], g_normalPlanMsSet.msa.add[0].add[5]);
            break;
        case 4: //һ���������
            sprintf(str, "%d", g_normalPlanMsSet.msi.idx[0]);
            break;
        case 5:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msrt.region[0].start);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msrt.region[0].end);
            }
            break;
        case 6:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msra.region[0].start.add[0], g_normalPlanMsSet.msra.region[0].start.add[1],
                        g_normalPlanMsSet.msra.region[0].start.add[2], g_normalPlanMsSet.msra.region[0].start.add[3],
                        g_normalPlanMsSet.msra.region[0].start.add[4], g_normalPlanMsSet.msra.region[0].start.add[5]);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%02x%02x%02x%02x%02x%02x", g_normalPlanMsSet.msra.region[0].end.add[0], g_normalPlanMsSet.msra.region[0].end.add[1],
                        g_normalPlanMsSet.msra.region[0].end.add[2], g_normalPlanMsSet.msra.region[0].end.add[3],
                        g_normalPlanMsSet.msra.region[0].end.add[4], g_normalPlanMsSet.msra.region[0].end.add[5]);
            }
            break;
        case 7:
            if (state == NORMAL_PLAN_MS_REGION_START)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msri.region[0].start);
            }
            else if (state == NORMAL_PLAN_MS_REGION_END)
            {
                sprintf(str, "%d", g_normalPlanMsSet.msri.region[0].end);
            }
            break;
    }
}

void normalPlanMsReturn(char* pNewStr, void* pPara)
{
    int value;
    char tmp[20] = { 0 };

    switch (g_normalPlanMsType)
    {
        case 2: case 5: //�û�����
        {
            value = atoi(pNewStr);
            if (value > 255 && MessageBox("�û����ʹ���255\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                if (g_normalPlanMsSetIndex == 0)
                    g_normalPlanMsSet.mst.type[0] = value;
                else if (g_normalPlanMsSetIndex == 1)
                    g_normalPlanMsSet.msrt.region[0].start = value;
                else if (g_normalPlanMsSetIndex == 2)
                    g_normalPlanMsSet.msrt.region[0].end = value;
            }
        }
        break;
        case 4: case 7: //�������
        {
            value = atoi(pNewStr);
            if (value > 65535 && MessageBox("������Ŵ���65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                if (g_normalPlanMsSetIndex == 0)
                    g_normalPlanMsSet.msi.idx[0] = value;
                else if (g_normalPlanMsSetIndex == 1)
                    g_normalPlanMsSet.msri.region[0].start = value;
                else if (g_normalPlanMsSetIndex == 2)
                    g_normalPlanMsSet.msri.region[0].end = value;
            }
        }
        break;
        case 3: case 6: //�û���ַ
        {
            value = strlen(pNewStr);
            if (value != 12 && MessageBox("��ַ����Ϊ12\n\nȷ���Ƿ���������?", MB_OKCANCEL))
            {
                if (g_normalPlanMsSetIndex == 0 || g_normalPlanMsSetIndex == 1)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
                else if (g_normalPlanMsSetIndex == 2)
                    normal_plan_ms_sprintf(tmp, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
                InputBox(tmp, normalPlanMsReturn, NULL, 0x01);
            }
            else
            {
                int i;
                if (g_normalPlanMsSetIndex == 0)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msa.add[0].add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
                else if (g_normalPlanMsSetIndex == 1)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msra.region[0].start.add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
                else if (g_normalPlanMsSetIndex == 2)
                {
                    for (i = 0; i < 6; i++)
                        g_normalPlanMsSet.msra.region[0].end.add[i] = (pNewStr[2 * i] - '0') * 16 + (pNewStr[2 * i + 1] - '0');
                }
            }
        }
        break;
    }
}


void normal_plan_ms_set_init(HDC hdc)
{
    memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
    memset(&g_normalPlanMsSet, 0, sizeof(OOP_MSP_T));
    g_normalPlanMsSetIndex = 0;
}

void normal_plan_ms_set_group_draw(HDC hdc)
{
    int nTop    = 60;
    int nLeft1  = 20;
    int nLeft2  = 85;
    char bzbuf[50] = {0};
    int8 index = 0;

    switch (g_normalPlanMsType)
    {
        case 2: //�û�����
            TextOut(hdc, nLeft1, nTop, "�û����ͣ�");
            break;
        case 3: //�û���ַ
            TextOut(hdc, nLeft1, nTop, "�û���ַ��");
            break;
        case 4: //�������
            TextOut(hdc, nLeft1, nTop, "������ţ�");
            break;
    }
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }

    index++;    
    nTop += 40;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 40, nTop, "���");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 40, nTop, "���");
    }

    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 85, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 85, nTop, "ɾ��");
    } 
}

void normal_plan_ms_set_region_draw(HDC hdc)
{
    int nTop    = 40;
    int nLeft1  = 15;
    int nLeft2  = 85;
    char bzbuf[50] = {0};
    int8 index = 0;

    TextOut(hdc, nLeft1, nTop, "�������ͣ�");
    int i;
    if (g_normalPlanMsType == 5)
        i = g_normalPlanMsSet.msrt.region[0].nType;
    else if (g_normalPlanMsType == 6)
        i = g_normalPlanMsSet.msra.region[0].nType;
    else if (g_normalPlanMsType == 7)
        i = g_normalPlanMsSet.msri.region[0].nType;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_regionType[i]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_regionType[i]);
    }
    
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "������ʼֵ��");
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }
    
    nTop += 25;
    TextOut(hdc, nLeft1, nTop, "�������ֵ��");
    normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, bzbuf);
    }

    index++;    
    nTop += 25;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 35, nTop, "���");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 35, nTop, "���");
    }

    index++;
    if (g_normalPlanMsSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, 90, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 90, nTop, "ɾ��");
    } 
}


void normal_plan_ms_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);

    switch (g_normalPlanMsType)
    {
        case 2: case 3: case 4: //��
            normal_plan_ms_set_group_draw(hdc);
            break;
        case 5: case 6: case 7: //����
            normal_plan_ms_set_region_draw(hdc);
            break;
    }
    PopListDraw(hdc);
}

void normal_plan_ms_set_group_key_ok()
{
    char bzbuf[50];

    switch (g_normalPlanMsSetIndex)
    {
        case 0:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
            break;
        case 1: //���
            if (TRUE == MessageBox("ȷ��Ҫ��ӣ�", MB_OKCANCEL))
            {
                int index;
                g_guiNormalPlan.ms.choice = g_normalPlanMsType;
                switch (g_normalPlanMsType)
                {
                    case 2:
                        index = g_guiNormalPlan.ms.mst.size;
                        g_guiNormalPlan.ms.mst.type[index] = g_normalPlanMsSet.mst.type[0];
                        g_guiNormalPlan.ms.mst.size++;
                        break;
                        
                    case 3:
                        index = g_guiNormalPlan.ms.msa.size;
                        g_guiNormalPlan.ms.msa.add[index].len = 5;
                        memcpy(&g_guiNormalPlan.ms.msa.add[index].add, g_normalPlanMsSet.msa.add[0].add, 6);
                        g_guiNormalPlan.ms.msa.size++;
                        break;

                    case 4:
                        index = g_guiNormalPlan.ms.msi.size;
                        g_guiNormalPlan.ms.msi.idx[index] = g_normalPlanMsSet.msi.idx[0];
                        g_guiNormalPlan.ms.msi.size++;
                        break;
                }
            }
            break;
        case 2: //ɾ��
            if (TRUE == MessageBox("ȷ��Ҫɾ����", MB_OKCANCEL))
            {
                
                memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
            }
            break;
    }
     

}

void normal_plan_ms_set_region_key_ok()
{
    char bzbuf[50];

    switch (g_normalPlanMsSetIndex)
    {
        case 0: //��������
            PopList(g_regionType, 4, g_regionType[0], 70, 40, FALSE);
            break;
        case 1:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_START);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
            break;
        case 2:
            normal_plan_ms_sprintf(bzbuf, g_normalPlanMsType, NORMAL_PLAN_MS_REGION_END);
            InputBox(bzbuf, normalPlanMsReturn, NULL, 0x01); 
        case 3: //���
            if (TRUE == MessageBox("ȷ��Ҫ��ӣ�", MB_OKCANCEL))
            {
                int index;
                g_guiNormalPlan.ms.choice = g_normalPlanMsType;
                switch (g_normalPlanMsType)
                {
                    case 5:
                        index = g_guiNormalPlan.ms.msrt.size;
                        g_guiNormalPlan.ms.msrt.region[index].nType = g_normalPlanMsSet.msrt.region[0].nType;
                        g_guiNormalPlan.ms.msrt.region[index].start = g_normalPlanMsSet.msrt.region[0].start;
                        g_guiNormalPlan.ms.msrt.region[index].end = g_normalPlanMsSet.msrt.region[0].end;
                        g_guiNormalPlan.ms.msrt.size++;
                        break;
                        
                    case 6:
                        index = g_guiNormalPlan.ms.msra.size;
                        g_guiNormalPlan.ms.msra.region[index].nType = g_normalPlanMsSet.msra.region[0].nType;
                        g_guiNormalPlan.ms.msra.region[index].start.len = 5;
                        memcpy(&g_guiNormalPlan.ms.msra.region[index].start.add, g_normalPlanMsSet.msra.region[0].start.add, 6);
                        g_guiNormalPlan.ms.msra.region[index].end.len = 5;
                        memcpy(&g_guiNormalPlan.ms.msra.region[index].end.add, g_normalPlanMsSet.msra.region[0].end.add, 6);
                        g_guiNormalPlan.ms.msra.size++;
                        break;

                    case 7:
                        index = g_guiNormalPlan.ms.msri.size;
                        g_guiNormalPlan.ms.msri.region[index].nType = g_normalPlanMsSet.msri.region[0].nType;
                        g_guiNormalPlan.ms.msri.region[index].start = g_normalPlanMsSet.msri.region[0].start;
                        g_guiNormalPlan.ms.msri.region[index].end = g_normalPlanMsSet.msri.region[0].end;
                        g_guiNormalPlan.ms.msri.size++;
                        break;
                }
            }
            break;
        case 4: //ɾ��
            if (TRUE == MessageBox("ȷ��Ҫɾ����", MB_OKCANCEL))
            {
                
                memset(&g_guiNormalPlan.ms, 0, sizeof(OOP_MSP_T));
            }
            break;
    }
}

BOOL normal_plan_ms_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex;

    switch (g_normalPlanMsType)
    {
        case 2: case 3: case 4:
            maxindex = 2;
            break;
        case 5: case 6: case 7:
            maxindex = 4;
            break;
    }

    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_normalPlanMsSetIndex)
            {
                case 0: //��������
                {
                    if (g_normalPlanMsType == 5)
                        g_normalPlanMsSet.msrt.region[0].nType = nSel;
                    else if (g_normalPlanMsType == 6)
                        g_normalPlanMsSet.msra.region[0].nType = nSel;
                    else if (g_normalPlanMsType == 7)
                        g_normalPlanMsSet.msri.region[0].nType = nSel;
                }
                break;
                default:
                    return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_normalPlanMsSetIndex > 0)
                g_normalPlanMsSetIndex--;
            else
                g_normalPlanMsSetIndex = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_normalPlanMsSetIndex < maxindex)
                g_normalPlanMsSetIndex++;
            else
                g_normalPlanMsSetIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_normalPlanMsType)
            {
                case 2: case 3: case 4:
                    normal_plan_ms_set_group_key_ok();
                    break;
                case 5: case 6: case 7:
                    normal_plan_ms_set_region_key_ok();
                    break;
            }
            return TRUE;
        }
    }
    return FALSE;
}

//MS��������
PLUGIN_INFO g_layer_normal_plan_ms_set = {
    0x00/*id*/,
    normal_plan_ms_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_ms_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_ms_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//MS��������
PLUGIN_INFO* g_normal_plan_ms_set_plugin[] =
{
    &g_layer_normal_plan_ms_set,
};

#endif

#if DESC("��ͨ�ɼ�����CSD����",1)
OOP_CSD_T       g_normalPlanCSD;
uint8           g_csd_oads_set_num;

//OAD��ʮ�������ַ���ת��Ϊ����
int OAD2i(char *str)
{
    int radix = 16;
    int value = 0;

    while(*str)
    {
        if(*str >= '0' && *str <= '9')
            value = value * radix + *str - '0';    //0��9���ַ���
        else if(*str >= 'A' && *str <= 'F')
            value = value * radix + *str - 'A' + 10;   //��дʮ�����Ƶ�ABCDEF���ַ��������
        else if(*str >= 'a' && *str <= 'f')
            value = value * radix + *str - 'a' + 10;  // Сдʮ�����Ƶ�abcdef���ַ��������

        str++;
    }
    return value;
}

void normalPlanCsdReturn(char* pNewStr, void* pPara)
{
    int nLen = strlen(pNewStr);
    char tmp[20] = { 0 };
    if (nLen != 8 && TRUE == MessageBox("OAD�ĳ���Ϊ8\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        switch (g_normal_plan_csd_data.nIndex)
        {
            case 0:
                sprintf(tmp, "%08X", g_normalPlanCSD.oad.value);
                break;
            case 2:
                sprintf(tmp, "%08X", g_normalPlanCSD.road.oadMain.value);
                break;
            case 3:
                sprintf(tmp, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
                break;
        }               
        
        InputBox(tmp, normalPlanCsdReturn, NULL, 0x01);
    }
    else
    {
        int value = OAD2i(pNewStr);
        //GUI_FMT_DEBUG("normalPlanCsdReturn %d\n", value);
        switch (g_normal_plan_csd_data.nIndex)
        {
            case 0:
                g_normalPlanCSD.oad.value = value;
                break;
            case 2:
                g_normalPlanCSD.road.oadMain.value = value;
                break;
            case 3:
                g_normalPlanCSD.road.oadCols.oad[0].value = value;
                break;
        }  
    }
}

void normal_plan_csd_set_init(HDC hdc)
{
    memset(&g_guiNormalPlan.recdCol, 0, sizeof(OOP_RECORD_COL_T));
    memset(&g_normalPlanCSD, 0, sizeof(OOP_CSD_T));
    g_csd_oads_set_num = 0;

    g_normal_plan_csd_data.ppDetail = NULL;
    g_normal_plan_csd_data.pCurPlugin = NULL;
    g_normal_plan_csd_data.nIndex = 0;
}

void normal_plan_csd_set_draw(HDC hdc)
{
    // ���Ʋ鿴CSDҳ��
    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_csd_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_csd_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop    = 20;
    int nLeft1  = 10;
    int nLeft2  = 25;
    int nLeft3  = 65;
    int index = 0;
    char bzbuf[50] = {0};

    TextOut(hdc, nLeft1, nTop, "��ͨ�ͣ�");
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.oad.value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3 + 60, nTop, "���");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3 + 60, nTop, "���");
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "��¼�ͣ�");
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "��OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadMain.value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft2, nTop, "��OAD");
    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, bzbuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, bzbuf);
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3 + 60, nTop - 10, "���");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3 + 60, nTop - 10, "���");
    }

    index++;
    nTop += 20;
    nLeft1 = 40;
    nLeft2 = 100;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "�鿴");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "�鿴");
    }

    index++;
    if (g_normal_plan_csd_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ɾ��");
    }
}

BOOL normal_plan_csd_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int maxindex = 6;

    if (g_normal_plan_csd_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_csd_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_csd_data.pCurPlugin->pfnUninitLayer();
                }
                g_normal_plan_csd_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }
    
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_normal_plan_csd_data.nIndex > 0)
                g_normal_plan_csd_data.nIndex--;
            else
                g_normal_plan_csd_data.nIndex = maxindex;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_normal_plan_csd_data.nIndex < maxindex)
                g_normal_plan_csd_data.nIndex++;
            else
                g_normal_plan_csd_data.nIndex = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            switch (g_normal_plan_csd_data.nIndex)
            {
                char bzbuf[50];
                case 0: //OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.oad.value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);  
                }
                break;
                case 1: //���OAD
                {
                    sprintf(bzbuf, "��ӵ�%d��csd������ΪOAD", g_guiNormalPlan.recdCol.num + 1);
                    if (TRUE == MessageBox(bzbuf, MB_OKCANCEL))
                    {
                        uint8 num = g_guiNormalPlan.recdCol.num;
                        g_guiNormalPlan.recdCol.csd[num].choice = 0;
                        g_guiNormalPlan.recdCol.csd[num].oad.value = g_normalPlanCSD.oad.value;
                        g_guiNormalPlan.recdCol.num++;
                    }
                }
                break;
                case 2: //��OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadMain.value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);
                }
                break;

                case 3: //��OAD
                {
                    sprintf(bzbuf, "%08X", g_normalPlanCSD.road.oadCols.oad[0].value);
                    InputBox(bzbuf, normalPlanCsdReturn, NULL, 0x01);
                }
                break;
                case 4: //�����OAD����OAD
                {
                    sprintf(bzbuf, "��OAD:%08X \n\n��OAD:%08X", 
                        g_normalPlanCSD.road.oadMain.value,
                        g_normalPlanCSD.road.oadCols.oad[0].value);
                    uint8 num = g_guiNormalPlan.recdCol.num; 

                    if (num > OOP_MAX_CSDS)
                    {
                        MessageBox("CSD�����ѵ�����", MB_OK);
                    }
                    else if (g_csd_oads_set_num > OOP_MAX_OADS)
                    {
                        MessageBox("����OAD�����ѵ�����", MB_OK);
                    }
                    else if (TRUE == MessageBox(bzbuf, MB_OKCANCEL))
                    {

                        if (num == 0)
                        {
                            g_guiNormalPlan.recdCol.csd[num].choice = 1;
                            g_guiNormalPlan.recdCol.csd[num].road.oadMain = g_normalPlanCSD.road.oadMain;
                            g_guiNormalPlan.recdCol.csd[num].road.oadCols.oad[g_csd_oads_set_num].value = g_normalPlanCSD.road.oadCols.oad[0].value;
                            g_csd_oads_set_num++;
                            g_guiNormalPlan.recdCol.csd[num].road.oadCols.nNum = g_csd_oads_set_num;
                            num++;
                        }
                        else
                        {
                            if (g_normalPlanCSD.road.oadMain.value !=  g_guiNormalPlan.recdCol.csd[num - 1].road.oadMain.value)
                            {
                                num++;
                                g_csd_oads_set_num = 0;
                            }
                            g_guiNormalPlan.recdCol.csd[num - 1].choice = 1;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadMain = g_normalPlanCSD.road.oadMain;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadCols.oad[g_csd_oads_set_num].value = g_normalPlanCSD.road.oadCols.oad[0].value;
                            g_csd_oads_set_num++;
                            g_guiNormalPlan.recdCol.csd[num - 1].road.oadCols.nNum = g_csd_oads_set_num;
                        }
                        g_guiNormalPlan.recdCol.num = num;
                    }
                }
                break;
                case 5: //�鿴
                {
                    g_normal_plan_csd_data.ppDetail = g_normal_plan_csd_display_plugin;
                    g_normal_plan_csd_data.nPluginCnt = sizeof(g_normal_plan_csd_display_plugin) / sizeof(g_normal_plan_csd_display_plugin[0]);
                    g_normal_plan_csd_data.nCurPlugin = 0;
                    g_normal_plan_csd_data.pCurPlugin = g_normal_plan_csd_data.ppDetail[g_normal_plan_csd_data.nCurPlugin];
                    ASSERT(g_normal_plan_csd_data.pCurPlugin != NULL);

                    if (g_normal_plan_csd_data.pCurPlugin->pfnInitLayer != NULL)
                    {
                        g_normal_plan_csd_data.pCurPlugin->pfnInitLayer(g_normal_plan_csd_data.pCurPlugin->dwId);
                    }
                    UpdateWindow(g_guiComm.hMainWnd, TRUE);
                }
                break;
                case 6: //��������
                {
                    if (TRUE == MessageBox("ȷ��Ҫɾ����", MB_OKCANCEL))
                    {
                        memset(&g_guiNormalPlan.recdCol, 0, sizeof(OOP_RECORD_COL_T));
                    }
                }
                break;
                return TRUE;
            }   
        }
    }
    return FALSE;
}


//csd����
PLUGIN_INFO g_layer_normal_plan_csd_set = {
    0x00/*id*/,
    normal_plan_csd_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    normal_plan_csd_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_csd_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//csd����
PLUGIN_INFO* g_normal_plan_csd_set_plugin[] =
{
    &g_layer_normal_plan_csd_set,
};
#endif

#if DESC("��ͨ�ɼ���������",1)
MEASURE_MAIN g_normal_plan_main_data;

//ɾ����ͨ�ɼ�����
void delete_normal_plan(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = NormalPlanOad;
	RecordDel.classtag = CLASS_DATA_INIT;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_gui_db_client, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("��ͨ�ɼ�����ɾ���ɹ� infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("��ͨ�ɼ�����ɾ��ʧ�� infonum %d \n", infonum);
    }

}


//���ô洢���
void MemDepthReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("���ô洢��ȴ���65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        sprintf(tmp, "%d", g_guiNormalPlan.memDepth);
        InputBox(tmp, MemDepthReturn, NULL, 0x01);
    }
    else
    {
        g_guiNormalPlan.memDepth = nLen;
    }
}

//����ֵ
void normalPlanValueReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("����ֵ����65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.num);
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.ti.value);
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            sprintf(tmp, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
        }
        InputBox(tmp, normalPlanValueReturn, NULL, 0x01);
    }
    else
    {
        if (g_guiNormalPlan.recdRow.choice == 1)
        {
            g_guiNormalPlan.recdRow.num = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 3)
        {
            g_guiNormalPlan.recdRow.ti.value = nLen;
        }
        else if (g_guiNormalPlan.recdRow.choice == 4)
        {
            g_guiNormalPlan.recdRow.rd.ti.value = nLen;
        }
    }
}

void normal_plan_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
    normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
    g_normalPlanMsType = g_guiNormalPlan.ms.choice;

    g_normal_plan_main_data.ppDetail = NULL;
    g_normal_plan_main_data.pCurPlugin = NULL;
    g_normal_plan_main_data.nIndex = 0;
}

void normal_plan_set_draw(HDC hdc)
{
    // ������������
    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        ASSERT(g_normal_plan_main_data.pCurPlugin->pfnDraw != NULL);
        g_normal_plan_main_data.pCurPlugin->pfnDraw(hdc);
        return;
    }

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    int flag = 0;  //��ʶ�����Ƿ����
    char czBuf[300] = { 0 };
    int index = 0;
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    if (g_guiNormalPlan.planID != 0)
        flag = 1;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢��ȣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", g_guiNormalPlan.memDepth);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�ɼ����ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4)
            choice = 0;
        sprintf(czBuf, "%s", g_normalPlanType[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "ֵ�뵥λ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //�ɼ��ϵ�N��
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.num);
            }
            else if (choice == 3) //��ʱ�����ɼ�
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.ti.value);
            }
            else //����
            {
                sprintf(czBuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.recdRow.choice;
        if (choice > 4 || choice == 0 || choice == 2)
            sprintf(czBuf, "%s", "-");
        else
        {
            if (choice == 1) //�ɼ��ϵ�N��
            {
                sprintf(czBuf, "��");
            }
            else if (choice == 3) //��ʱ�����ɼ�
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.ti.unit]);
            }
            else //����
            {
                sprintf(czBuf, "%s", g_ti[g_guiNormalPlan.recdRow.rd.ti.unit]);
            }
        }
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }
    

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��Ƽ��ϣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.ms.choice;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_ms[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�洢ʱ�꣺");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        uint8 choice;
        choice = g_guiNormalPlan.tmFlag;
        if (choice > 7)
            choice = 0;
        sprintf(czBuf, "%s", g_tmFlag[choice]);
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ѡ�����");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (flag == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "��������");
    }
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 50;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ɾ��");
    }

    index++;
    int nLeft3 = 90;
    if (g_normal_plan_main_data.nIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "��������");
    }

    PopListDraw(hdc);
}

BOOL normal_plan_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 10;

    if (g_normal_plan_main_data.pCurPlugin != NULL)
    {
        if (g_normal_plan_main_data.pCurPlugin->pfnKeyMsg != NULL
            && TRUE == g_normal_plan_main_data.pCurPlugin->pfnKeyMsg(nKey))
        {
            return TRUE;
        }

        switch (nKey)
        {
            case KEY_CANCEL:
                if (g_normal_plan_main_data.pCurPlugin->pfnUninitLayer != NULL)
                {
                    g_normal_plan_main_data.pCurPlugin->pfnUninitLayer();
                }
                g_guiComm.nCurPoint = 1;
                g_normal_plan_main_data.pCurPlugin = NULL;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
        }
        return TRUE;
    }
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_normal_plan_main_data.nIndex)
            {
            case 2: //�ɼ�����
            {
                g_guiNormalPlan.recdRow.choice = nSel;
            }
            break;
            case 4: //��λ
            {
                g_guiNormalPlan.recdRow.ti.unit = nSel;
            }
            break;
            case 5: //�������
            {
                g_guiNormalPlan.ms.choice = nSel;
                if (nSel != 0 && nSel != 1) //����"�ޱ��"�Ҳ���"ȫ���û���ַ"
                {
                    g_normalPlanMsType = nSel;
                    g_normal_plan_main_data.ppDetail = g_normal_plan_ms_set_plugin;
                    g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_ms_set_plugin) / sizeof(g_normal_plan_ms_set_plugin[0]);
                    g_normal_plan_main_data.nCurPlugin = 0;
                    g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
                    ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

                    if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
                    {
                        g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
                    }
                }

            }
            break;
            case 6: //�洢ʱ��
            {
                g_guiNormalPlan.tmFlag = nSel;
            }
            break;
            default:
                return FALSE;
            }
            g_guiNormalPlan.planID = g_guiComm.nCurPoint;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            g_guiComm.nCurPoint = MAX_PLAN_INFONUM;
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        memset(&g_guiNormalPlan, 0, sizeof(OOP_PLAN_NORMAL_T));
        normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (g_normal_plan_main_data.nIndex > 0)
            g_normal_plan_main_data.nIndex--;
        else
            g_normal_plan_main_data.nIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (g_normal_plan_main_data.nIndex < maxindex)
            g_normal_plan_main_data.nIndex++;
        else
            g_normal_plan_main_data.nIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (g_normal_plan_main_data.nIndex == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            return TRUE;
        }
        
        //���ô洢���
        if (g_normal_plan_main_data.nIndex == 1)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiNormalPlan.memDepth);
            InputBox(bzbuf, MemDepthReturn, NULL, 0x01);
            return TRUE;
        }
        //���òɼ�����
        if (g_normal_plan_main_data.nIndex == 2)
        {
            PopList(g_normalPlanType, 5, g_normalPlanType[0], 70, 40, FALSE);
            return TRUE;
        }
        //����ֵ
        if (g_normal_plan_main_data.nIndex == 3)
        {
            char bzbuf[50];
            if (g_guiNormalPlan.recdRow.choice == 1)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.num);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 3)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            else if (g_guiNormalPlan.recdRow.choice == 4)
            {
                sprintf(bzbuf, "%d", g_guiNormalPlan.recdRow.rd.ti.value);
                InputBox(bzbuf, normalPlanValueReturn, NULL, 0x01);
            }
            return TRUE;
        }
        //���õ�λ
        if (g_normal_plan_main_data.nIndex == 4 && (g_guiNormalPlan.recdRow.choice == 3 || g_guiNormalPlan.recdRow.choice == 4))
        {
            PopList(g_ti, 6, g_ti[0], 70, 40, FALSE);
            return TRUE;
        }
        //���ñ�Ƽ���
        if (g_normal_plan_main_data.nIndex == 5)
        {
            PopList(g_ms, 8, g_ms[0], 50, 40, FALSE);
            return TRUE;
        }
        //���ô洢ʱ��
        if (g_normal_plan_main_data.nIndex == 6)
        {
            PopList(g_tmFlag, 8, g_tmFlag[0], 50, 40, FALSE);
            return TRUE;
        }

        //����csd
        if (g_normal_plan_main_data.nIndex == 7)
        {
            g_normal_plan_main_data.ppDetail = g_normal_plan_csd_set_plugin;
            g_normal_plan_main_data.nPluginCnt = sizeof(g_normal_plan_csd_set_plugin) / sizeof(g_normal_plan_csd_set_plugin[0]);
            g_normal_plan_main_data.nCurPlugin = 0;
            g_normal_plan_main_data.pCurPlugin = g_normal_plan_main_data.ppDetail[g_normal_plan_main_data.nCurPlugin];
            ASSERT(g_normal_plan_main_data.pCurPlugin != NULL);

            if (g_normal_plan_main_data.pCurPlugin->pfnInitLayer != NULL)
            {
                g_normal_plan_main_data.pCurPlugin->pfnInitLayer(g_normal_plan_main_data.pCurPlugin->dwId);
            }
            UpdateWindow(g_guiComm.hMainWnd, TRUE);
            return TRUE;
        }

        //ȡ��
        if (g_normal_plan_main_data.nIndex == 8)
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("��ǰ�������ñ���ɹ���");
                }
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //ɾ��
        if (g_normal_plan_main_data.nIndex == 9)
        {
            if (TRUE == MessageBox("ȷ��ɾ����ǰ�ɼ�������", MB_OKCANCEL))
            {
                delete_normal_plan(g_guiComm.nCurPoint);
                g_guiComm.nCurPoint = 1;
                normal_plan_read(g_guiComm.nCurPoint, &g_guiNormalPlan);
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            g_normal_plan_main_data.nIndex = 0;
            return FALSE;
        }
        //��������
        if (g_normal_plan_main_data.nIndex == 10)
        {
            GUI_FMT_DEBUG("write_normal_data %d\n", g_guiNormalPlan.ms.choice);
            ret = write_normal_data((uint8*)&g_guiNormalPlan, sizeof(OOP_PLAN_NORMAL_T), NormalPlanOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("��ǰ�ɼ��������ñ���ɹ���");
                g_normal_plan_main_data.nIndex = 0;
                return TRUE;
            }
            else
            {
                MessageBox("��ǰ�ɼ��������ñ���ʧ�ܣ�");
                return FALSE;
            }
        }
    }
    }

    return FALSE;
}

//��ͨ��������
PLUGIN_INFO g_layer_normal_plan_set = {
    0x11030080/*id*/,
    normal_plan_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    normal_plan_set_draw/*Draw*/,
    NULL/*Timer*/,
    normal_plan_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("�ɼ���������", 1)
int     g_tastSetIndex;

void delete_task(uint16 infonum)
{
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
	RecordDel.classtag = CLASS_DATA_INIT;
    RecordDel.infoNum = infonum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum = 1;
    RecordDel.road.oadCols.oad[0] = TaskOad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(g_gui_db_client, &clear);
    if (ret == 0)
    {
        GUI_FMT_DEBUG("�ɼ�����ɾ���ɹ� infonum %d \n", infonum);
    }
    else
    {
        GUI_FMT_DEBUG("�ɼ�����ɾ��ʧ�� infonum %d \n", infonum);
    }

}

void taskSetDefaultPara(void)
{
    TaskInfo.id = g_guiComm.nCurPoint;

    TaskInfo.startTime.year     = 2017;
    TaskInfo.startTime.month    = 1;
    TaskInfo.startTime.day      = 1;
    TaskInfo.startTime.hour     = 0;
    TaskInfo.startTime.minute   = 0;
    TaskInfo.startTime.second   = 0;
    TaskInfo.startTime.week     = 0;

    TaskInfo.endTime.year       = 2099;
    TaskInfo.endTime.month      = 1;
    TaskInfo.endTime.day        = 1;
    TaskInfo.endTime.hour       = 0;
    TaskInfo.endTime.minute     = 0;
    TaskInfo.endTime.second     = 0;
    TaskInfo.endTime.week       = 0;

    TaskInfo.startScriptID      = 0;
    TaskInfo.endScriptID        = 0;

    TaskInfo.workPeriod.nNum    = 1;
    TaskInfo.workPeriod.type    = 0;

    TaskInfo.workPeriod.period[0].startHour = 0;
    TaskInfo.workPeriod.period[0].startMin  = 0;
    TaskInfo.workPeriod.period[0].endHour   = 23;
    TaskInfo.workPeriod.period[0].endMin    = 59;
}

void taskSetTimeReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 65535 && TRUE == MessageBox("����ʱ�����65535\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        if (g_tastSetIndex == 1)
            sprintf(tmp, "%d", TaskInfo.acqFreq.value);
        else if (g_tastSetIndex == 5)
            sprintf(tmp, "%d", TaskInfo.acqDelay.value);
        InputBox(tmp, taskSetTimeReturn, NULL, 0x01);
    }
    else
    {
        if (g_tastSetIndex == 1)
            TaskInfo.acqFreq.value = nLen;
        else if (g_tastSetIndex == 5)
            TaskInfo.acqDelay.value = nLen;
    }
}

void taskSetPlanIdReturn(char* pNewStr, void* pPara)
{
    int nLen = atoi(pNewStr);
    char tmp[20] = { 0 };
    if (nLen > 255 && TRUE == MessageBox("����ֵ����255\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        if (g_tastSetIndex == 4)
            sprintf(tmp, "%d", TaskInfo.planID);
        else if (g_tastSetIndex == 7)
            sprintf(tmp, "%d", TaskInfo.priority);
        InputBox(tmp, taskSetPlanIdReturn, NULL, 0x01);
    }
    else
    {
        if (g_tastSetIndex == 4)
            TaskInfo.planID = nLen;
        else if (g_tastSetIndex == 7)
            TaskInfo.priority = nLen;
    }
}


void task_set_init(HDC hdc)
{
    g_guiComm.nCurPoint = 1;
    memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
    get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);

    g_tastSetIndex = 0;
}

void task_set_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    advance_gui_infonum((uint16)g_guiComm.nCurPoint);

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int index = 0;
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 70;
    TextOut(hdc, nLeft1, nTop, "����ID��");
    
    sprintf(czBuf, "%d", g_guiComm.nCurPoint);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    
    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "ִ�м����");
    memset(czBuf, 0x00, sizeof(czBuf));
    index++;
    sprintf(czBuf, "%d", TaskInfo.acqFreq.value);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    sprintf(czBuf, "%s", g_ti[TaskInfo.acqFreq.unit]);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "�������ͣ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (TaskInfo.planType == 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        if (TaskInfo.planType > 0 && TaskInfo.planType < 6)
            sprintf(czBuf, "%s", g_plantype[TaskInfo.planType - 1]);
    }
    index++;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "������ţ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", TaskInfo.planID);
    index++;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "��ʱ��");
    memset(czBuf, 0x00, sizeof(czBuf));
    index++;
    sprintf(czBuf, "%d", TaskInfo.acqDelay.value);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    sprintf(czBuf, "%s", g_ti[TaskInfo.acqDelay.unit]);
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2 + 30, nTop, czBuf);
    }

    nTop += 15;
    TextOut(hdc, nLeft1, nTop, "���ȼ���");
    memset(czBuf, 0x00, sizeof(czBuf));
    sprintf(czBuf, "%d", TaskInfo.priority);
    index++;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 50;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "ɾ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "ɾ��");
    }

    index++;
    int nLeft3 = 90;
    if (g_tastSetIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "��������");
    }

    PopListDraw(hdc);
}

BOOL task_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    int maxindex = 10;
    
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (g_tastSetIndex)
            {
            case 2: //ִ�м����λ
            {
                TaskInfo.acqFreq.unit = nSel;
            }
            break;
            case 3: //��������
            {         
                TaskInfo.planType = nSel + 1;
            }
            break;
            case 6: //��ʱ��λ
            {
                TaskInfo.acqDelay.unit = nSel;
            }
            break;
            
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    {
        if (g_guiComm.nCurPoint > 0)
            g_guiComm.nCurPoint--;
        if (g_guiComm.nCurPoint == 0)
            g_guiComm.nCurPoint = MAX_PLAN_INFONUM;
        memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
        get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    {
        if (g_guiComm.nCurPoint < MAX_PLAN_INFONUM)
        {
            g_guiComm.nCurPoint++;
        }
        else
        {
            g_guiComm.nCurPoint = 1;
        }
        memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
        get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
        advance_gui_infonum(g_guiComm.nCurPoint);
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_UP:
    {
        if (g_tastSetIndex > 0)
            g_tastSetIndex--;
        else
            g_tastSetIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    {
        if (g_tastSetIndex < maxindex)
            g_tastSetIndex++;
        else
            g_tastSetIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (g_tastSetIndex == 0)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", g_guiComm.nCurPoint);
            InputBox(bzbuf, CurPointReturn, NULL, 0x01);
            return TRUE;
        }
        
        //����ִ�л���ʱ���
        if (g_tastSetIndex == 1 || g_tastSetIndex == 5)
        {
            char bzbuf[50];
            if (g_tastSetIndex == 1)
                sprintf(bzbuf, "%d", TaskInfo.acqFreq.value);
            else
                sprintf(bzbuf, "%d", TaskInfo.acqDelay.value);
            InputBox(bzbuf, taskSetTimeReturn, NULL, 0x01);
            return TRUE;
        }
        
        //����ִ�л���ʱ��λ
        if (g_tastSetIndex == 2 || g_tastSetIndex == 6)
        {
            PopList(g_ti, 6, g_ti[0], 70, 40, FALSE);
            return TRUE;
        }
        
        //���÷������
        if (g_tastSetIndex == 3)
        {
            PopList(g_plantype, 5, g_plantype[0], 70, 40, FALSE);
            return TRUE;
        }

        //���÷������
        if (g_tastSetIndex == 4)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", TaskInfo.planID);
            InputBox(bzbuf, taskSetPlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //�������ȼ�
        if (g_tastSetIndex == 7)
        {
            char bzbuf[50];
            sprintf(bzbuf, "%d", TaskInfo.priority);
            InputBox(bzbuf, taskSetPlanIdReturn, NULL, 0x01);
            return TRUE;
        }
        
        //ȡ��
        if (g_tastSetIndex == 8)
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                taskSetDefaultPara();
                ret = write_normal_data((uint8*)&TaskInfo, sizeof(OOP_TASK_T), TaskOad, g_guiComm.nCurPoint, 0, 2);
                if (ret == 0)
                {
                    MessageBox("��ǰ�������ñ���ɹ���");
                }
            }
            memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
            get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
            g_tastSetIndex = 0;
            return FALSE;
        }
        //ɾ��
        if (g_tastSetIndex == 9)
        {
            if (TRUE == MessageBox("ȷ��ɾ����ǰ�ɼ�����", MB_OKCANCEL))
            {
                delete_task(g_guiComm.nCurPoint);
                memset(&TaskInfo, 0x00, sizeof(OOP_TASK_T));
                get_taskinfo(g_guiComm.nCurPoint, &TaskInfo);
                g_tastSetIndex = 0;
                UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                return TRUE;
            }
            g_tastSetIndex = 0;
            return FALSE;
        }
        //��������
        if (g_tastSetIndex == 10)
        {
            taskSetDefaultPara();
            ret = write_normal_data((uint8*)&TaskInfo, sizeof(OOP_TASK_T), TaskOad, g_guiComm.nCurPoint, 0, 2);
            if (ret == 0)
            {
                MessageBox("��ǰ�ɼ����񱣴�ɹ���");
                g_tastSetIndex = 0;
                return TRUE;
            }
            else
            {
                MessageBox("��ǰ�ɼ����񱣴�ʧ�ܣ�");
                return FALSE;
            }
        }
    }
    }
    return FALSE;
}


//�ɼ���������
PLUGIN_INFO g_layer_task_set = {
    0x11030090/*id*/,
    task_set_init/*InitLayer*/,
    Uninit/*UninitLayer*/,
    task_set_draw/*Draw*/,
    NULL/*Timer*/,
    task_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


void HPLC_set_init(HDC hdc)
{
    AcrossAreaFlagIsSet = read_db_noamal_data(g_gui_db_client, 0xF2090B00, 0, 0, &HPLCAcrossAreaFlag, 1);
    NodeRejectFlagIsSet = read_db_noamal_data(g_gui_db_client, 0xF2091600, 0, 0, &HPLCNodeRejectFlag, 1);
    printf("AcrossAreaFlagIsSet %d NodeRejectFlagIsSet %d", AcrossAreaFlagIsSet, NodeRejectFlagIsSet);
    HPLC_set_index = 0;
}

void HPLC_set_draw(HDC hdc)
{
    char czBuf[300] = { 0 };

    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 120;
    TextOut(hdc, nLeft1, nTop, "̨��ʶ�𿪹أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        uint8 tmp = HPLCAcrossAreaFlag & 0x01;
        sprintf(czBuf, "%s", tmp == 0 ? "�ر�" : "����");
    }
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "������֪���أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (AcrossAreaFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        uint8 tmp = ((HPLCAcrossAreaFlag>>2) & 0x01);
        sprintf(czBuf, "%s", tmp == 0 ? "�ر�" : "����");
    }
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�ܾ��ڵ��ϱ����أ�");
    memset(czBuf, 0x00, sizeof(czBuf));
    if (NodeRejectFlagIsSet < 0)
    {
        sprintf(czBuf, "�ر�");
    }
    else
    {
        sprintf(czBuf, "%s", HPLCNodeRejectFlag == 0 ? "�ر�" : "����");
    }
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

    index++;
    nTop += 20;

    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 80;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��������");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��������");
    }

    PopListDraw(hdc);
}

BOOL HPLC_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    int ret;
    uint8 areaident = HPLCAcrossAreaFlag & 0x01; //̨��ʶ���־
    uint8 awaynet =  (HPLCAcrossAreaFlag>>2) & 0x01;   //������־
    int maxindex = 4;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (HPLC_set_index)
            {
            case 0://̨��ʶ�𿪹�
            {
                AcrossAreaFlagIsSet = 1;
                areaident = nSel;
            }
            break;
            case 1://������֪����
            {
                AcrossAreaFlagIsSet = 1;
                awaynet = nSel;
                //HPLCAcrossAreaFlag = HPLCAcrossAreaFlag | (nSel << 2);
            }
            break;
            case 2://�ܾ��ڵ��ϱ�����
            {
                NodeRejectFlagIsSet = 1;
                HPLCNodeRejectFlag = nSel;
            }
            break;
            default:
                return FALSE;
            }
            HPLCAcrossAreaFlag = (awaynet<<2)|areaident;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }

    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (HPLC_set_index > 0)
            HPLC_set_index--;
        else
            HPLC_set_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (HPLC_set_index < maxindex)
            HPLC_set_index++;
        else
            HPLC_set_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //̨��ʶ�𿪹�
        if (HPLC_set_index == 0)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 20, FALSE);
            return TRUE;
        }
        //������֪����
        if (HPLC_set_index == 1)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 40, FALSE);
            return TRUE;
        }
        //�ܾ��ڵ��ϱ�����
        if (HPLC_set_index == 2)
        {
            PopList(g_on_off, 2, g_on_off[0], 120, 60, FALSE);
            return TRUE;
        }
        //ȡ��
        if (HPLC_set_index == 3)
        {
            if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
            {
                OOP_OAD_U oad;
                oad.value = 0xF2090B00;
                ret = write_normal_data(&HPLCAcrossAreaFlag, 1, oad, 0, 0, 2);
                if (ret != 0)
                {
                    MessageBox("HPLC̨��ʶ�𿪹ر���ʧ��");
                }
                oad.value = 0xF2091600;
                ret = write_normal_data(&HPLCNodeRejectFlag, 1, oad, 0, 0, 2);
                if (ret != 0)
                {
                    MessageBox("HPLC�ܾ��ڵ��ϱ����ر���ʧ��");
                }
                MessageBox("���ñ���ɹ���");
            }
            HPLC_set_index = 0;
            return FALSE;
        }
        //����
        if (HPLC_set_index == 4)
        {
            OOP_OAD_U oad;
            oad.value = 0xF2090B00;
            ret = write_normal_data(&HPLCAcrossAreaFlag, 1, oad, 0, 0, 2);
            if (ret != 0)
            {
                MessageBox("HPLC̨��ʶ�𿪹ر���ʧ��");
            }
            oad.value = 0xF2091600;
            ret = write_normal_data(&HPLCNodeRejectFlag, 1, oad, 0, 0, 2);
            if (ret != 0)
            {
                MessageBox("HPLC�ܾ��ڵ��ϱ����ر���ʧ��");
            }
            MessageBox("���ñ���ɹ���");
            HPLC_set_index = 0;
            return FALSE;
        }
    }
    }
    return FALSE;
}
//HPLC��������
PLUGIN_INFO g_layer_HPLC_set = {
    0x11030015/*id*/,
    HPLC_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HPLC_set_draw/*Draw*/,
    NULL/*Timer*/,
    HPLC_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};
    
#if DESC("�ز��ѱ�����", 1)
extern int seek_meter_to_taskManager(uint16 time);

uint8 gSearchIndex;
uint8 gSearchStatus;
int gSearchTime;
void HPLCSearch_init(HDC hdc)
{
    gSearchIndex = 0;
	gSearchStatus = 0;
	gSearchTime = 3;
}

void HPLCSearch_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    char czBuf[300] = {0};
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 50;
    int nLeft1 = 35;
    int nLeft2 = 105;
    TextOut(hdc, nLeft1, nTop, "�ѱ�״̬");
    if(gSearchStatus == 0)
    {
       sprintf(czBuf, "%s", "�ر�"); 
    }
    else
    {   
		sprintf(czBuf, "%s", "����"); 
    }
	
    if (gSearchIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }

	nTop+=20;
	
    if(gSearchStatus == 1)
    {
	    index++;
	    TextOut(hdc, nLeft1, nTop, "�ѱ�ʱ�� ");
	    if (gSearchTime < 0)
	    {
	        sprintf(czBuf, "-");
	    }
	    else
	    {
	        sprintf(czBuf, "%d ����", gSearchTime);
	    }

	    if (gSearchIndex == index)
	    {
	        ITEM_SELECT;
	        TextOut(hdc, nLeft2, nTop, czBuf);
	        ITEM_NOT_SELECT;
	    }
	    else
	    {
	        TextOut(hdc, nLeft2, nTop, czBuf);
	    }
    }
	
    index++;
    nTop += 40;
    if (gSearchIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
    }

    index++;
    if (gSearchIndex == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }
   
    PopListDraw(hdc);
}

void HPLCSearchReturn(char* pNewStr, void* pPara)
{
    int inputdata = atoi(pNewStr);
    char tmp[20] = { 0 };
    printf("�����ʱ��Ϊ %s %d����\n", pNewStr, inputdata);
    if (inputdata > 600 && TRUE == MessageBox("����Ƶ�����600����\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        strcpy(tmp, "");
        InputBox(tmp, HPLCSearchReturn, NULL, 0x01);
    }
    else
    {
        if (inputdata <= 600)
        {
            gSearchTime = inputdata;
        }
    }
}

BOOL HPLCSearch_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    uint8 maxindex = 3;
    uint8 buf[100]={0};
    uint16 buflen=0;
	int ret = 0;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (gSearchIndex)
            {
	            case 0://����ѡ��
	            {
	                gSearchStatus = nSel;
	            }
            	break;          
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }

	if(gSearchStatus == 0)
    {
       maxindex = 2;
    }
    else
    {   
		maxindex = 3;
    }
	
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (gSearchIndex > 0)
            gSearchIndex--;
        else
            gSearchIndex = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (gSearchIndex < maxindex)
            gSearchIndex++;
        else
            gSearchIndex = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //Ƶ������
        if (gSearchIndex == 0)
        {
            PopList(g_on_off, 2, g_on_off[0], 80, 42, FALSE);
            return TRUE;
        }

		if(gSearchStatus==1)
		{
		    if (gSearchIndex == 1)
		    {
		        char tmp[20] = { 0 };
		        strcpy(tmp, "");
		        InputBox(tmp, HPLCSearchReturn, NULL, 0x01);
		        return TRUE;
		    }
		    //ȡ��
		    if (gSearchIndex == 2)
		    {
		        AppCancelMenu();
		        return FALSE;
		    }
		    //����
		    if (gSearchIndex == 3)
		    {
		    	buflen = 0;
		    	buf[buflen++] = 0xf2;
				buf[buflen++] = 0x09;
				buf[buflen++] = 0x02;
				buf[buflen++] = 0x01;

				buf[buflen++] = (uint8)((gSearchTime >> 8) &0xff);
    			buf[buflen++] = (uint8)(gSearchTime & 0xff);
		        ret = advance_send_taskManager(0, MESSAGE_IID_CTS, CTS_IOP_SEEK_METER, buf, buflen);
	            if (ret != 0)
	            {
	                MessageBox("�ѱ�״̬����ʧ��");
	            }
	            else
	            {
					MessageBox("�ѱ�״̬�����ɹ���");
	            }
	            
		        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
		        return FALSE;
		    }
		}
		else
		{
			if (gSearchIndex == 2)
			{
				buflen = 0;
				buf[buflen++] = 0xf2;
				buf[buflen++] = 0x09;
				buf[buflen++] = 0x02;
				buf[buflen++] = 0x01;
			    ret = advance_send_ccomsg(0, CCOROUTER_IOP_PAUSE_SEEKMETER,  buf, buflen);
				if (ret != 0)
	            {
	                MessageBox("�ѱ�״̬�ر�ʧ��");
	            }
	            else
	            {
					MessageBox("�ѱ�״̬�رճɹ���");
	            }
			    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
			    return FALSE;
			}
			if (gSearchIndex == 1)
			{
			    AppCancelMenu();
			    return FALSE;
			}
		}
    }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_HPLCSearch = {
    0x11030084/*id*/,
    HPLCSearch_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HPLCSearch_draw/*Draw*/,
    NULL/*Timer*/,
    HPLCSearch_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


#if DESC("�ز�Ƶ�����úͲ�ѯ", 1)
uint8 page_select;

void HCHanenl_set_init(HDC hdc)
{
    HPLC_set_index = 0;
    HPLCChannel = 0;
    recvbit=2;
    page_select=0;
}

void HCHanenl_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    char czBuf[300] = {0};
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 20;
    int nLeft1 = 10;
    int nLeft2 = 80;
    TextOut(hdc, nLeft1, nTop, "����ѡ��");
    if(page_select == 0)
    {
       sprintf(czBuf, "%s", "�����ز�Ƶ��"); 
    }
    else
    {   
	sprintf(czBuf, "%s", "��ѯ�ز�Ƶ��"); 
    }
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    if(page_select == 0)
    {
    nTop+=20;
    index++;
    TextOut(hdc, nLeft1, nTop, "�ز�Ƶ�㣺");
    if (HPLCChannel < 0)
    {
        sprintf(czBuf, "-");
    }
    else
    {
        sprintf(czBuf, "%d", HPLCChannel);
    }

    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;
    nTop += 40;

    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
    }

    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "***********************************");
    nTop += 20;
    TextOut(hdc, nLeft1, nTop, "�·����");
    if(recvbit==3)
    TextOut(hdc, nLeft2, nTop, "�·���");
    else if(recvbit == 2)
    TextOut(hdc, nLeft2, nTop, "�ȴ��·�");
    else if(recvbit == 1)
    TextOut(hdc, nLeft2, nTop, "�·��ɹ�");
    else if(recvbit == 0)
    TextOut(hdc, nLeft2, nTop, "�·�ʧ��");
    }
    else
    {       
    nTop+=20;
    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "����");
    }
    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "��ѯ");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "��ѯ");
    }
    nTop+=20;
    TextOut(hdc, nLeft1, nTop, "***********************************");
    nTop += 40;
    TextOut(hdc, nLeft1, nTop, "��ѯ���");
    if(recvbit==5)
    sprintf(czBuf, "%s", "�ȴ��ٲ�");
    else if(recvbit == 4)
    sprintf(czBuf, "%s", "�ٲ���");
    else if(recvbit<4)
    sprintf(czBuf, "%s%d", "Ƶ��",recvbit);
    index++;
    if (HPLC_set_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    }
    PopListDraw(hdc);
}

void HPLCChennelReturn(char* pNewStr, void* pPara)
{
    int inputdata = atoi(pNewStr);
    char tmp[20] = { 0 };
    printf("�����HPLCƵ��Ϊ %s %d\n", pNewStr, inputdata);
    if (inputdata > 3 && TRUE == MessageBox("����Ƶ�����3\n\nȷ���Ƿ���������?", MB_OKCANCEL))
    {
        strcpy(tmp, "");
        InputBox(tmp, HPLCChennelReturn, NULL, 0x01);
    }
    else
    {
        if (inputdata <= 3)
        {
            HPLCChannel = inputdata;
            ChannelIsSet = 0;
        }
    }
}
void hplcAmrMakeFrame(uint8 *apdu, uint8 *len,uint8 flag)
{
    uint8   index=0;
    uint8   tmpoffset;
    uint8 cs=0,i;
    memset(apdu,0,100);
    apdu[index++]=0x09;
    apdu[index++]=0x07;
    apdu[index++]=0x01;
    apdu[index++]=0xf2;
    apdu[index++]=0x09;
    apdu[index++]=0x02;    
    apdu[index++]=0x01;
    //������
    apdu[index++]=0x03;
    apdu[index++]=0x02;
    apdu[index++]=0x08;
    apdu[index++]=0x01;
    apdu[index++]=0x00;
    //
    apdu[index++] = 0x00;
    apdu[index++] = 0x32;
    //�ֽڳ�ʱʱ��(10ms)
    apdu[index++] = 0x00;
    apdu[index++] = 0x0A;
    if(flag == 1)
    {
        apdu[index++] = 0x0F;
        apdu[index++] = 0x68;
        apdu[index++] = 0x0F;
        apdu[index++] = 0x00;
        apdu[index++] = 0x43;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x00;
        apdu[index++] = 0x0E;
        apdu[index++] = 0x03;
        apdu[index++] = 0x80;
        apdu[index++] = 0x01;
        apdu[index++] = 0xD5;
        apdu[index++] = 0x16;
    }
    else
   {
    apdu[index++] = 0x10;
    apdu[index++] = 0x68;
    apdu[index++] = 0x10;
    apdu[index++] = 0x00;
    tmpoffset=index;
    apdu[index++] = 0x43;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x00;
    apdu[index++] = 0x0d;
    apdu[index++] = 0x05;
    apdu[index++] = 0x80;
    apdu[index++] = 0x01;
    apdu[index++] = HPLCChannel;
    for(i=tmpoffset;i<index;i++)
    {
    cs+=apdu[i];
    }
    apdu[index++] = cs;
    apdu[index++] = 0x16;
   }
    apdu[index++] = 0x00;
    *len = index;
}

BOOL HCHanenl_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;
    uint8 maxindex = 3;
    uint8 buf[100]={0};
    uint8 buflen=0;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (HPLC_set_index)
            {
            case 0://����ѡ��
            {
                AcrossAreaFlagIsSet = 1;
                page_select = nSel;
            }
            break;
            default:
            return FALSE;            
            }
            
            if(page_select == 0)
            {
            recvbit = 2;
            }
            else
            {
            recvbit = 5;
            }
            HPLC_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_LEFT:
    case KEY_UP:
    {
        if (HPLC_set_index > 0)
            HPLC_set_index--;
        else
            HPLC_set_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_RIGHT:
    case KEY_DOWN:
    {
        if (HPLC_set_index < maxindex)
            HPLC_set_index++;
        else
            HPLC_set_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        //Ƶ������
        if (HPLC_set_index == 0)
        {
            PopList(g_Hplc_type, 2, g_on_off[0], 80, 20, FALSE);
            return TRUE;
        }

        if(page_select==0)
        {
        if (HPLC_set_index == 1)
        {
            char tmp[20] = { 0 };
            strcpy(tmp, "");
            InputBox(tmp, HPLCChennelReturn, NULL, 0x01);
            return TRUE;
        }
        //ȡ��
        if (HPLC_set_index == 2)
        {
            AppCancelMenu();
            HPLC_set_index = 0;
            return FALSE;
        }
        //����
        if (HPLC_set_index == 3)
        {
            hplcAmrMakeFrame(buf,&buflen,0);
            gui_mannual_amr_send(buf,buflen);
            recvbit=3;
            HPLC_set_index = 0;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return FALSE;
        }
       }
        else
        {
        if (HPLC_set_index == 2)
        {
            hplcAmrMakeFrame(buf,&buflen,1);
            gui_mannual_amr_send(buf,buflen);
            recvbit=4;
            HPLC_set_index = 3;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return FALSE;
        }
        if (HPLC_set_index == 1)
        {
            AppCancelMenu();
            HPLC_set_index = 0;
            return FALSE;
        }
        }
    }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_Hchanenl_set = {
    0x11030083/*id*/,
    HCHanenl_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    HCHanenl_set_draw/*Draw*/,
    NULL/*Timer*/,
    HCHanenl_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

uint8 g_GuiCtrlGroup = 0;
uint8 g_GuiPeriodPlan = 0;
OOP_8103_CONFIG_T   g_GuiPeriodParam;           //0x81030200 ʱ�ο�
OOP_8104_CONFIG_T   g_GuiRestParam;             //0x81040200 ���ݿ�
OOP_8105_CONFIG_T   g_GuiStopParam;             //0x81050200 ��ͣ��
OOP_POWCON_LOW_T    g_GuiDownParam[OOP_MAX_TG]; //0x81067fff �¸���
OOP_8108_CONFIG_T   g_GuiMonthParam;              //0x81080200 �µ�� 

void ctrl_period_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    advance_gui_infonum(g_GuiCtrlGroup + 1);
    g_GuiPeriodPlan = 0;

    memset(&g_GuiPeriodParam, 0, sizeof(OOP_8103_CONFIG_T));
    read_db_noamal_data(g_gui_db_client, 0x81030200, 0, 0, (uint8*)&g_GuiPeriodParam, sizeof(OOP_8103_CONFIG_T));
}

void ctrl_rest_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    advance_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiRestParam, 0, sizeof(OOP_8104_CONFIG_T));
    read_db_noamal_data(g_gui_db_client, 0x81040200, 0, 0, (uint8*)&g_GuiRestParam, sizeof(OOP_8104_CONFIG_T));
}

void ctrl_stop_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    advance_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiStopParam, 0, sizeof(OOP_8105_CONFIG_T));
    read_db_noamal_data(g_gui_db_client, 0x81050200, 0, 0, (uint8*)&g_GuiStopParam, sizeof(OOP_8105_CONFIG_T));
}

void ctrl_down_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    advance_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiDownParam[g_GuiCtrlGroup], 0, sizeof(OOP_POWCON_LOW_T));
    read_db_noamal_data(g_gui_db_client, 0x81067fff, 0, g_GuiCtrlGroup + 1, (uint8*)&g_GuiDownParam[g_GuiCtrlGroup], sizeof(OOP_POWCON_LOW_T));
}

void ctrl_month_param_display_init(DWORD dwPluginId)
{
    g_GuiCtrlGroup = (dwPluginId) & 0x0F;
    advance_gui_infonum(g_GuiCtrlGroup + 1);

    memset(&g_GuiMonthParam, 0, sizeof(OOP_8108_CONFIG_T));
    read_db_noamal_data(g_gui_db_client, 0x81080200, 0, 0, (uint8*)&g_GuiMonthParam, sizeof(OOP_8108_CONFIG_T));
}

void ctrl_period_param_display_draw(HDC hdc)
{
    char czBuf[100];
    uint16 offset = 0;
    int nTop = 32;
    int nLeft = 1;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_8109_T* ptCfg = NULL;
    char* PlanName[] = { (char*)"һ", (char*)"��", (char*)"��" };
    int64 tempnum = 0;
    int64 tempnum1 = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "ʱ�οز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiPeriodParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiPeriodParam.item[i].object)
        {
            ptCfg = &(g_GuiPeriodParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    sprintf(czBuf, "����ϵ��   %d", ptCfg->floatCoe);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    if (0 == ((ptCfg->plan >> g_GuiPeriodPlan) & 0x01))
    {
        sprintf(czBuf, "��%s������Ч", PlanName[g_GuiPeriodPlan]);
        TextOut(hdc, nLeft, nTop, czBuf);
        return;
    }
    sprintf(czBuf, "��%s����(��λkW):", PlanName[g_GuiPeriodPlan]);
    TextOut(hdc, nLeft, nTop, czBuf);

    /* �������ʱ�ζ�ֵ */
    offset = 0;
    for (i = 0; i < 8; i++)
    {
        if ((ptCfg->value[g_GuiPeriodPlan].valid >> i) & 0x01)
        {
            tempnum = ptCfg->value[g_GuiPeriodPlan].defValue[i] / (10000L);
            tempnum1 = ptCfg->value[g_GuiPeriodPlan].defValue[i] - tempnum * (10000L);
            if (ptCfg->value[g_GuiPeriodPlan].defValue[i] > 90000000
                || ptCfg->value[g_GuiPeriodPlan].defValue[i] < -90000000)
            {
                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                {
                    tempnum1 = 0 - tempnum1;
                }
                offset += sprintf(czBuf + offset, "%u:%8lld.%1lld", i + 1, tempnum, tempnum1 / 1000);
            }
            else
            {
                if (ptCfg->value[g_GuiPeriodPlan].defValue[i] < 0)
                {
                    tempnum1 = 0 - tempnum1;
                }
                offset += sprintf(czBuf + offset, "%u:%5lld.%04lld", i + 1, tempnum, tempnum1);
            }
        }
        else
        {
            offset += sprintf(czBuf + offset, "%u:      ----", i + 1);
        }
        if (1 == (i % 2))
        {
            nTop += tmpTop;
            TextOut(hdc, nLeft, nTop, czBuf);
            offset = 0;
        }
        else
        {
            offset += sprintf(czBuf + offset, "  ");
        }
    }
}

void ctrl_rest_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810A_T* ptCfg = NULL;
    int64  tempnum = 0;
    char* PlanName[] = { (char*)"һ", (char*)"��", (char*)"��", (char*)"��", (char*)"��", (char*)"��", (char*)"��" };

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "���ݿز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiRestParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiRestParam.item[i].object)
        {
            ptCfg = &(g_GuiRestParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ   %13lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��ʼ�޵�ʱ��    %02d:%02d:%02d", ptCfg->startTime.hour, ptCfg->startTime.minute, ptCfg->startTime.second);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�޵�����ʱ��    %d min", ptCfg->duration);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "ÿ���޵���");
    nTop += tmpTop;
    for (i = 1; i < 8; i++)
    {
        if ((ptCfg->powLimit.nValue >> i) & 0x01)
        {
            TextOut(hdc, 48 + 16 * (i - 1), nTop, PlanName[i - 1]);
        }
    }
}

void ctrl_stop_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810B_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "��ͣ�ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiStopParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiStopParam.item[i].object)
        {
            ptCfg = &(g_GuiStopParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10L) * 10L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ   %13lld.%lld W", ptCfg->defValue / 10, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "��ʼʱ��    %04d/%02d/%02d", ptCfg->startTime.year, ptCfg->startTime.month, ptCfg->startTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ʱ��    %04d/%02d/%02d", ptCfg->endTime.year, ptCfg->endTime.month, ptCfg->endTime.day);
    TextOut(hdc, nLeft, nTop, czBuf);
}

void ctrl_down_param_display_draw(HDC hdc)
{
    char czBuf[100];
    uint8 offset = 0;
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 14;
    uint8 i = 0;
    uint8 turnMax = 0;
    LCONSCHEME* ptCfg = NULL;
    char* turnName[] = { (char*)"һ��", (char*)"����", (char*)"����", (char*)"����",
                        (char*)"����", (char*)"����", (char*)"����", (char*)"����" };

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�¸��ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    if (0x2301 + g_GuiCtrlGroup != g_GuiDownParam[g_GuiCtrlGroup].object)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    ptCfg = &(g_GuiDownParam[g_GuiCtrlGroup].scheme);

    nTop += tmpTop;
    sprintf(czBuf, "����ʱ��   %d min", ptCfg->slipTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ϵ��   %d%%", ptCfg->floatCoefficient);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "������ʱ   %d min", ptCfg->delayTime);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ʱ��   %d min", ptCfg->conTime * ((uint16)30));
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "�澯ʱ��(��λmin):");
    TextOut(hdc, nLeft, nTop, czBuf);
    appctrl_get_devturn(&turnMax);

    for (i = 0; i < turnMax; i++)
    {
        offset += sprintf(czBuf + offset, "  %s %3d", turnName[i], ptCfg->waringTime[i]);
        if (0 == i % 2)
        {
            offset += sprintf(czBuf + offset, "   ");
        }
        else
        {
            nTop += tmpTop;
            TextOut(hdc, nLeft, nTop, czBuf);
            offset = 0;
        }
    }
}

void ctrl_month_param_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 5;
    int tmpTop = 16;
    uint32 i = 0;
    OOP_CFGUNIT_810D_T* ptCfg = NULL;
    int64  tempnum = 0;

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�µ�ز��� �ܼ���%d", g_GuiCtrlGroup + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    /* �����Ƿ�������� */
    for (i = 0; i < g_GuiMonthParam.nNum; i++)
    {
        if (0x2301 + g_GuiCtrlGroup == g_GuiMonthParam.item[i].object)
        {
            ptCfg = &(g_GuiMonthParam.item[i]);
            break;
        }
    }

    if (NULL == ptCfg)
    {
        nTop += 16;
        TextOut(hdc, nLeft + 20, nTop + 10, "δ���÷���");
        return;
    }

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    tempnum = ptCfg->defValue - ((ptCfg->defValue / 10000L) * 10000L);
    if (ptCfg->defValue < 0)
    {
        tempnum = 0 - tempnum;
    }

    sprintf(czBuf, "��ֵ     %lld.%04lld kWh", ptCfg->defValue / 10000L, tempnum);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ϵ��  %d%%", ptCfg->limitCoe);
    TextOut(hdc, nLeft, nTop, czBuf);

    nTop += tmpTop;
    sprintf(czBuf, "����ϵ��  %d%%", ptCfg->floatCoe);
    TextOut(hdc, nLeft, nTop, czBuf);
}

BOOL ctrl_period_param_keymsg(int nKey)
{
    switch (nKey)
    {
    case KEY_RIGHT:
    {
        if (g_GuiPeriodPlan >= 2)
        {
            g_GuiPeriodPlan = 0;
        }
        else
        {
            g_GuiPeriodPlan++;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return  TRUE;

    }
    case KEY_LEFT:
    {
        if (0 == g_GuiPeriodPlan)
        {
            g_GuiPeriodPlan = 2;
        }
        else
        {
            g_GuiPeriodPlan--;
        }
        UpdateWindow(g_guiComm.hMainWnd, TRUE);
        return TRUE;
    }
    default:
        return FALSE;
    }

}


//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display1 = {
    0x11031010/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display2 = {
    0x11031011/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display3 = {
    0x11031012/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display4 = {
    0x11031013/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display5 = {
    0x11031014/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display6 = {
    0x11031015/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display7 = {
    0x11031016/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};
//ʱ�οز����鿴
PLUGIN_INFO g_layer_ctrl_period_display8 = {
    0x11031017/*id*/,
    ctrl_period_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_period_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    ctrl_period_param_keymsg/*keymsg*/,
    NULL/*ipc*/
};

//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display1 = {
    0x11031020/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display2 = {
    0x11031021/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display3 = {
    0x11031022/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display4 = {
    0x11031023/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display5 = {
    0x11031024/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display6 = {
    0x11031025/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display7 = {
    0x11031026/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//���ݿز����鿴
PLUGIN_INFO g_layer_ctrl_rest_display8 = {
    0x11031027/*id*/,
    ctrl_rest_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_rest_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display1 = {
    0x11031030/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display2 = {
    0x11031031/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display3 = {
    0x11031032/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display4 = {
    0x11031033/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display5 = {
    0x11031034/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display6 = {
    0x11031035/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display7 = {
    0x11031036/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//��ͣ�ز����鿴
PLUGIN_INFO g_layer_ctrl_stop_display8 = {
    0x11031037/*id*/,
    ctrl_stop_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_stop_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display1 = {
    0x11031040/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display2 = {
    0x11031041/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display3 = {
    0x11031042/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display4 = {
    0x11031043/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display5 = {
    0x11031044/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display6 = {
    0x11031045/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display7 = {
    0x11031046/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�¸��ز����鿴
PLUGIN_INFO g_layer_ctrl_down_display8 = {
    0x11031047/*id*/,
    ctrl_down_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_down_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display1 = {
    0x11031050/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display2 = {
    0x11031051/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display3 = {
    0x11031052/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display4 = {
    0x11031053/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display5 = {
    0x11031054/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display6 = {
    0x11031055/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display7 = {
    0x11031056/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};
//�µ�ز����鿴
PLUGIN_INFO g_layer_ctrl_month_display8 = {
    0x11031057/*id*/,
    ctrl_month_param_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_month_param_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


void ctrl_turn_data_display_init(DWORD dwPluginId)
{
    int oadvalue;
    g_guiComm.nCurPoint = (dwPluginId) & 0x0000000F;
    advance_gui_infonum(g_guiComm.nCurPoint + 1);

    oadvalue = 0x23010e00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].ctrlPwTurn, sizeof(OOP_BITSTR8_T));

    oadvalue = 0x23010f00 + (0x00010000) * g_guiComm.nCurPoint;
    read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&g_ctrl_data[g_guiComm.nCurPoint].ctrlEcTurn, sizeof(OOP_BITSTR8_T));
}

void ctrl_turn_data_display_draw(HDC hdc)
{
    char czBuf[100];
    int nTop = 32;
    int nLeft = 16;
    int tmpTop = 14;
    int i = 0;
    uint8 turnmax = 0;
    uint8 pccfg = 0;
    uint8 eccfg = 0;
    char* turnname[] = { (char*)"һ��", (char*)"����", (char*)"����", (char*)"����",
                        (char*)"����", (char*)"����", (char*)"����", (char*)"����" };

    SelectFont(hdc, g_guiComm.fontBig);
    ITEM_NOT_SELECT;

    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "�ִβ��� �ܼ���%d", g_guiComm.nCurPoint + 1);
    rc.top = 22;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    appctrl_get_devturn(&turnmax);

    SelectFont(hdc, g_guiComm.fontSmall);
    nTop += 16;
    TextOut(hdc, nLeft, nTop, "�ִ�     ����    ���");
    nTop += tmpTop;
    TextOut(hdc, nLeft, nTop, "---------------------");

    pccfg = g_ctrl_data[g_guiComm.nCurPoint].ctrlPwTurn.nValue;
    eccfg = g_ctrl_data[g_guiComm.nCurPoint].ctrlEcTurn.nValue;
    for (i = 0; i < turnmax; i++)
    {
        sprintf(czBuf, "%s     %s    %s", turnname[i],
            0 == ((pccfg >> i) & 0x01) ? "����" : "�ܿ�", 0 == ((eccfg >> i) & 0x01) ? "����" : "�ܿ�");
        nTop += tmpTop;
        TextOut(hdc, nLeft, nTop, czBuf);
    }
}

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display1 = {
    0x11031060/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display2 = {
    0x11031061/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display3 = {
    0x11031062/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display4 = {
    0x11031063/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display5 = {
    0x11031064/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display6 = {
    0x11031065/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display7 = {
    0x11031066/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

//�ִβ����鿴
PLUGIN_INFO g_layer_ctrl_turn_display8 = {
    0x11031067/*id*/,
    ctrl_turn_data_display_init/*InitLayer*/,
    gruop_data_display_uninit/*UninitLayer*/,
    ctrl_turn_data_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};

uint8 comm_proxy_enable;
uint8 comm_proxy_index;
//�����õ�ʹ��д�������ļ�
void comm_proxy_enable_set()
{
    char filename[256] = {0};
    dictionary *ini = NULL;
    char curpath[256]={0};
    int ret;
    int i;
    FILE *fp = NULL;
    char buf[100]={0};
     //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf(" get err path!\n");
        
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, ADV_INI_NAME);
    ini = iniparser_load(filename);
    if(ini!=NULL)
    {   
        sprintf(buf,"%d",comm_proxy_enable);
        ret = iniparser_set(ini,"RUN_CONF:COMM_PROXY",(const char*)buf);
        if(ret == 0)
        {
            sprintf(buf,"ͨ�Ŵ���ʹ�� %s �ɹ�\n ����Ӧ��\n",g_on_off[comm_proxy_enable]);
            MessageBox(buf);
            fp = fopen(filename,"w");
            if(fp!=NULL)
            {
                iniparser_dump_ini(ini, fp);
                fclose(fp);
            }

            exit(0);
        }else
        {
            MessageBox("ͨ�Ŵ���ʹ��д��ʧ��\n");
        }
    }

    iniparser_freedict(ini);
}
void comm_proxy_enable_get()
{
    char filename[256] = {0};
    dictionary *ini = NULL;
    char curpath[256]={0};
    int i;
     //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf(" get err path!\n");
        
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, ADV_INI_NAME);
    ini = iniparser_load(filename);
    if(ini!=NULL)
    {
        comm_proxy_enable = iniparser_getint(ini, "RUN_CONF:COMM_PROXY", FALSE);
    }
    iniparser_freedict(ini);
}
void comm_proxy_enable_Init(DWORD dwPluginId)
{
    comm_proxy_enable_get();
    comm_proxy_index = 0;
}
void comm_proxy_enable_Draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;
    int index = 0;
    ITEM_NOT_SELECT;
    char czBuf[300];
    SelectFont(hdc, pg_guicomm->fontSmall);
    int nTop = 40;
    int nLeft1 = 10;
    int nLeft2 = 100;
    TextOut(hdc, nLeft1, nTop, "ͨ�Ŵ���ʹ��:");

    if (comm_proxy_enable == 0)
    {
        sprintf(czBuf, "%s", g_on_off[0]);
    }
    else {
        sprintf(czBuf, "%s", g_on_off[1]);
    }
    if (comm_proxy_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, czBuf);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, czBuf);
    }
    index++;

    nTop += 60;
    if (comm_proxy_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft1, nTop, "ȡ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft1, nTop, "ȡ��");
    }

    index++;
    nLeft2 = 80;
    if (comm_proxy_index == index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, "����");
    }

    PopListDraw(hdc);

}


 BOOL comm_proxy_enable_Msg(int nKey)
 {
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return FALSE;

    int maxindex = 2;
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel = 0;

        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            switch (comm_proxy_index)
            {
            case 0://��ȫģʽ
            {
                if (nSel == 0) {
                    comm_proxy_enable = 0;
                }
                else {
                    comm_proxy_enable = 1;
                }

            }
            break;
            default:
                return FALSE;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        }
        return TRUE;
    }
    switch (nKey)
    {
    case KEY_UP:
    case KEY_LEFT:
    {
        if (comm_proxy_index > 0)
            comm_proxy_index--;
        else
            comm_proxy_index = maxindex;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_DOWN:
    case KEY_RIGHT:
    {
        if (comm_proxy_index < maxindex)
            comm_proxy_index++;
        else
            comm_proxy_index = 0;
        UpdateWindow(pg_guicomm->hMainWnd, TRUE);
        return TRUE;
    }
    case KEY_OK:
    {
        if (ValidatePwd() == TRUE)
        {
            switch (comm_proxy_index)
            {
            case 0: //��ȫģʽģʽ
            {
                PopList(g_on_off, 2, g_on_off[0], 100, 40, FALSE);
                return TRUE;
            }
            case 1://ȡ��
            {
                if (TRUE == MessageBox("��ǰ����û�б���\n\n�Ƿ�ȷ���������ã�", MB_OKCANCEL))
                {
                    comm_proxy_enable_set();
                    AppCancelMenu();
                }else
                    AppCancelMenu();
                comm_proxy_index = 0;
                return TRUE;
            }
            case 2://����
            {
                comm_proxy_enable_set();
                AppCancelMenu();
                comm_proxy_index = 0;
                return TRUE;
            }
            }
        }
    }
    }
 
    return FALSE;
 }
//ͨ�Ŵ���ʹ��
PLUGIN_INFO g_layer_comm_proxy_set=
{
    0x1104000b, // 
    comm_proxy_enable_Init,
    NULL,
    comm_proxy_enable_Draw,
    NULL,
    comm_proxy_enable_Msg,
    NULL
};


void hunan_address_set_draw(HDC hdc)
{
    GUI_COMM* pg_guicomm = AppChkGuiComm();
    if (pg_guicomm == NULL)
        return;

    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    RECT rc;
    rc.left = 0;
    rc.right = pg_guicomm->nWidth;

    char czBuf[300];
    sprintf(czBuf, "�ն�ͨ�ŵ�ַ");
    rc.top = 26;
    rc.bottom = rc.top + 16;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    SelectFont(hdc, pg_guicomm->fontSmall);

    memset(czBuf, 0x00, sizeof(czBuf));
#ifdef AREA_ZHEJIANG
    sprintf(czBuf, "%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3]);
#else
    sprintf(czBuf, "%02x%02x%02x%02x%02x%02x", LocalAddr[0], LocalAddr[1], LocalAddr[2], LocalAddr[3], LocalAddr[4], LocalAddr[5]);
#endif
    rc.top += 30;
    rc.bottom = rc.top + 15;
    ITEM_SELECT;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);
    ITEM_NOT_SELECT;
}



//ͨ�ŵ�ַ����
PLUGIN_INFO g_layer_address_set_hunan = {
    0x000c0004/*id*/,
    address_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    hunan_address_set_draw/*Draw*/,
    NULL/*Timer*/,
    address_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};


#if DESC("ʣ�����",1)
int64      g_LeftPower[8] = {0};
uint8      g_ZjNo[8] = {0};

void zj_left_power_display_init(DWORD dwPluginId)
{
    uint32 oadvalue;
    uint32 i = 0;
    int    ret = 0;
    memset(&tBuyenergyconfig, 0, sizeof(tBuyenergyconfig));
    read_db_noamal_data(g_gui_db_client, 0x81070200, 0, 0, (uint8*)&tBuyenergyconfig, sizeof(OOP_8107_CONFIG_T));

    for(i = 0; i < 8; i++)
    {
        oadvalue = 0x23010b00 + (0x00010000) * i;
        ret = read_db_noamal_data(g_gui_db_client, oadvalue, 0, 0, (uint8*)&(g_LeftPower[i]), sizeof(int64));
        if(0 == ret)
        {
            g_ZjNo[i] = i + 1;
        }
        else
        {
            g_ZjNo[i] = 0;
        }
    }
}

void zj_left_power_display_draw(HDC hdc)
{
    SelectFont(hdc, g_guiComm.fontSmall);
    ITEM_NOT_SELECT;

    char czBuf[100];
    int nLeft1 = 4;
    int nLeft2 = 56;
    int nTop = 22;
    int nTempTop = 13;
    uint16 oiobject = 0x2301;
    uint8  i = 0;
    uint8  j = 0;
    char* ZJName[] = { (char*)"�ܼ�һ", (char*)"�ܼӶ�", (char*)"�ܼ���", (char*)"�ܼ���", 
                       (char*)"�ܼ���", (char*)"�ܼ���", (char*)"�ܼ���", (char*)"�ܼӰ�"};
    int index = -1;
    int isfee = 0;
    
    RECT rc;
    rc.left = 0;
    rc.right = g_guiComm.nWidth;

    sprintf(czBuf, "ʣ�����(��)");
    rc.top = 22;
    rc.bottom = rc.top + 14;
    DrawText(hdc, czBuf, strlen(czBuf), &rc, DT_CENTER | DT_VCENTER);

    for(i = 0; i < 8; i++)
    {
        oiobject = 0x2301 + i;
        index = -1;
        for (j = 0; j < tBuyenergyconfig.nNum; j++)
        {
            if (tBuyenergyconfig.item[j].object == oiobject)
            {
                index = j;
                isfee = tBuyenergyconfig.item[j].type == 0 ? 0 : 1;
                break;
            }
        }
        nTop += nTempTop;
        TextOut(hdc, nLeft1, nTop, ZJName[i]);

        if(0 == g_ZjNo[i] || index == -1)
        {
            sprintf(czBuf, "------------ %s", (isfee == 1) ? "Ԫ" : "kWh");
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
        else
        {
            if (isfee == 1)
            {
                sprintf(czBuf, "%12.4f Ԫ", (double)g_LeftPower[i] * 0.0001);
            }
            else
            {
                MakeFormatedStringOfEnergy((double)g_LeftPower[i], czBuf);
            }
            TextOut(hdc, nLeft2, nTop, czBuf);
        }
    }
}

//ʣ�����
PLUGIN_INFO g_layer_ZJ_left_display = {
    0x12010001/*id*/,
    zj_left_power_display_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    zj_left_power_display_draw/*Draw*/,
    NULL/*Timer*/,
    NULL/*keymsg*/,
    NULL/*ipc*/
};


#endif

#if DESC("���ݲɼ�ģʽ", 1)
uint8 g_GatherType = OOP_GATHER_FJ;
uint8 g_GatherTypeOld = OOP_GATHER_FJ;
uint8 g_GatherIndex = 0;

//��Ӫ��
const char* g_GatherName[] =
{
    "����Э��",
    "��������"
};

void gatherFJ_set_init(HDC hdc)
{
    int ret = 0;
    ret = read_db_noamal_data(g_gui_db_client, 0x6E000200, 0, 0, &g_GatherType, sizeof(uint8));
    if(0 != ret || g_GatherType > OOP_GATHER_FJ)
    {
        g_GatherType = OOP_GATHER_FJ;
		g_GatherTypeOld = 0xff;						//Ĭ��״̬�£����ø���ģʽʱд����������
    }
	else
	{
		g_GatherTypeOld = g_GatherType;				//��Ĭ��״̬�£��ɼ�ģʽû�仯����д��������
	}
    g_GatherIndex = 0;
}

void gatherFJ_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 32, nTop, "�ⲿ�豸����");
    nTop += 20;
    TextOut(hdc, 48, nTop, "�ɼ�ģʽ");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "ģʽ: ");
    if(0 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_GatherName[g_GatherType]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_GatherName[g_GatherType]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "����");
    }

    if(2 == g_GatherIndex)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "ȷ��");
    }
    
    PopListDraw(hdc);
}

BOOL gatherFJ_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_GatherType = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_GatherIndex > 0)
            {
                g_GatherIndex--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_GatherIndex < 2)
            {
                g_GatherIndex++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_GatherIndex)
            {
                AppCancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_GatherIndex)
                {
                    PopList(g_GatherName, 2, g_GatherName[g_GatherType], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_GatherIndex)
                {
                    if(g_GatherType != g_GatherTypeOld)
                    {
                        if (TRUE == MessageBox("��Ҫ�����ն���Ч\n\n�Ƿ�ȷ����Ч���ã�", MB_OKCANCEL))
                        {
                            int ret = 0;
                            OOP_OAD_U gatherOad;
                            gatherOad.value = 0x6E000200;
                            ret = write_normal_data(&g_GatherType, 1, gatherOad, 0, 0, CLASS_DATA_UNINIT);
                            if (ret != 0)
                            {
                                MessageBox("���ݲɼ�ģʽ����д��ʧ�ܣ�");
                            }
                            else
                            {
                                reboot_sent_to_smiOS();
                                MessageBox("���óɹ�\n\n�ն�������");
                            }
                        }
                    }
					else
                	{	
                		MessageBox("�ɼ�ģʽû�б仯\n\n�˳�����ҳ�棡");
                	
					}
                    AppCancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_GatherFJ_set = {
    0x11030084/*id*/,
    gatherFJ_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    gatherFJ_set_draw/*Draw*/,
    NULL/*Timer*/,
    gatherFJ_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif

#if DESC("RS485�˿����ݹ���", 1)
uint8 g_Rs485_status = 0;
uint8 g_Rs485_statusOld = 0;
uint8 g_Rs485Index = 0;

//��Ӫ��
const char* g_Rs485StatusName[] =
{
    "ͣ��",
    "����"
};

void rs485Status_set_init(HDC hdc)
{
    g_Rs485_status = 0;
    read_db_noamal_data(g_gui_db_client, 0x6E030300, 0, 0, &g_Rs485_status, sizeof(uint8));
    if(g_Rs485_status > 1)
    {
        g_Rs485_status = 0;
    }
    g_Rs485Index = 0;
    g_Rs485_statusOld = g_Rs485_status;
}

void rs485Status_set_draw(HDC hdc)
{
    int nTop = 24;
    int nLeft1 = 16;
    int nLeft2 = 60;
    int nLeft3 = 88;

    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return;
    ITEM_NOT_SELECT;
    SelectFont(hdc, pg_guicomm->fontBig);
    TextOut(hdc, 4, nTop, "RS485-2�˿����ݹ���");
    nTop += 20;
    TextOut(hdc, 48, nTop, "��������");

    SelectFont(hdc, pg_guicomm->fontSmall);
    nTop += 32;
    TextOut(hdc, nLeft1, nTop, "����: ");
    if(0 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft2, nTop, g_Rs485StatusName[g_Rs485_status]);
    }

    SelectFont(hdc, pg_guicomm->fontBig);
    nTop += 32;
    if(1 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, 30, nTop, "����");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, 30, nTop, "����");
    }

    if(2 == g_Rs485Index)
    {
        ITEM_SELECT;
        TextOut(hdc, nLeft3, nTop, "ȷ��");
        ITEM_NOT_SELECT;
    }
    else
    {
        TextOut(hdc, nLeft3, nTop, "ȷ��");
    }
    
    PopListDraw(hdc);
}

BOOL rs485Status_set_keymsg(int nKey)
{
    GUI_COMM* pg_guicomm = &g_guiComm;
    if (pg_guicomm == NULL)
        return FALSE;
        
    if (pg_guicomm->bPopList == TRUE)
    {
        int nSel;
        PopListKeyMsg(nKey, &nSel);
        if (nKey == KEY_OK)
        {
            g_Rs485_status = nSel;
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        return TRUE;
    }
    switch (nKey)
    {
        case KEY_LEFT:
        case KEY_UP:
        {
            if (g_Rs485Index > 0)
            {
                g_Rs485Index--;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_RIGHT:
        case KEY_DOWN:
        {
            if (g_Rs485Index < 2)
            {
                g_Rs485Index++;
            }
            UpdateWindow(pg_guicomm->hMainWnd, TRUE);
            return TRUE;
        }
        case KEY_OK:
        {
            if(1 == g_Rs485Index)
            {
                AppCancelMenu();
                return TRUE;
            }
            else if(TRUE == ValidatePwd())
            {
                if(0 == g_Rs485Index)
                {
                    PopList(g_Rs485StatusName, 2, g_Rs485StatusName[g_Rs485_status], 80, 80, FALSE);
                    UpdateWindow(pg_guicomm->hMainWnd, TRUE);
                    return TRUE;
                }
                else if(2 == g_Rs485Index)
                {
                    if(g_Rs485_status != g_Rs485_statusOld)
                    {
                        int ret = 0;
                        OOP_OAD_U statusOad;
                        statusOad.value = 0x6E030300;
                        ret = write_normal_data(&g_Rs485_status, 1, statusOad, 0, 0, CLASS_DATA_INIT);
                        if (ret != 0)
                        {
                            MessageBox("485���ݹ���д��ʧ�ܣ�");
                        }
                        else
                        {
                            MessageBox("���óɹ���");
                        }
                    }
                    AppCancelMenu();
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

PLUGIN_INFO g_layer_rs485Status_set = {
    0x11030085/*id*/,
    rs485Status_set_init/*InitLayer*/,
    NULL/*UninitLayer*/,
    rs485Status_set_draw/*Draw*/,
    NULL/*Timer*/,
    rs485Status_set_keymsg/*keymsg*/,
    NULL/*ipc*/
};

#endif


