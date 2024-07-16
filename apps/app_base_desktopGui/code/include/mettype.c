/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
    ��������:   mettype.c
    ��    �ܣ������Ͷ���
    �� �� �ˣ���һ��  
    ����ʱ�䣺2008-01-07
    
    �汾��ʷ:
            2008-01-07 ,v1.0 : New              
    
***************************************************************************
*/
//#include "data_type.h"
#include "sys_cfg.h"
#include "mettype.h"

METTYPE_CONTRAST g_MettypeContrast[] = 
{
	#if STD_698PRO_BETA == 1
	{UNKNOW, "δ֪"},
	{DLT645_97, "DLT645-97"},
	{DLT645_07, "DLT645-07"},
	{DLT698_45, "DLT698.45"},
	{CJ_T188_04, "CJ/T188_2004"},
	
	#endif
    #if BEIJING_FUN
    {DLT645_APP,    "DLT645_97"},
    {DL2007_PRTL,   "DLT645_07"},
    {ABB_APP,       "ABB����"},
    {ABB_APP_S,     "ABBԲ��"},
    {HETONG_OLD,    "��ͨI��"},
    {HETONG_NEW,    "��ͨII��"},
    {LGR_APP_D3,    "������d-33"},
    {LGR_APP,       "������d-34"},
    {LGR_D_MET_B3,  "������B-33"},
    {LGR_D_MET,     "������B-34"},
    {EDMI_APP,      "����MK6"},
    {EDMI_APP_Y,    "����MK3"},
    {WS20_APP,      "��ʤ���"},
    {HND_APP,       "������"},
    /*#else
    {DLT645_APP,    "������"},
    {AC_SAMPINGMET, "���ɵ��"},
    {ABB_APP,       "ABB����"},
    {ABB_APP_S,     "ABBԲ��"},
    {HETONG_OLD, 	"��ͨI��"},
    {HETONG_NEW,   	"��ͨII��"},
    {LGR_APP_D3,    "������d-33"},
    {LGR_APP,       "������d-34"},
    {LGR_D_MET_B3,  "������B-33"},
    {LGR_D_MET,     "������B-34"},
    {EDMI_APP,      "����MK6"},
    {EDMI_APP_Y,    "����MK3"},
    {WS20_APP,      "��ʤ���"},
    {HND_APP,       "������"},
    {WXABB_APP,     "����ABB"},
    {LG9600_APP,    "����LDS_D"},
    {SIMPLE_DLT_APP,"��������"},
    {FQD_APP,       "���Ե��"},
    {DLT645_TLLD,   "���ܷ��Ե��"},
    {DL2007_PRTL,	"����2007"},
    {DLMS_APP,      "DLMS��Լ"},*/

    #if JIANG_SU_FUN == 1
    {DLT645_FFEE_APP, 	"�ķ���97��"},
    {DLT645_TFEE_APP, 	"������97��"},
    {DL2007_PLC_EVT, 	"13����07��"},

    #if OS_SMARTBREAKER_EN == 1
    {SMARTBREAKER_PRTL,     "���ܶ�·��"},      //���ܶ�·����Լ
    #endif
    #endif  

	#if HUBEI_FUN == 1    
    {HB_STD_APP, 	"����๦��"},
    {HB_JY_APP, 	"������׶๦��"},
    {HB_PT_APP, 	"������ͨ"},
    {HB_FFL_APP, 	"�������"},
    {DLT645_PRTL20,	"����ת��ģ��"},//���Ե�ģ��:����ʵ�ַ��Ե繦�ܵ�����ת��ģ�飬ע:����Ҫ���Ӧ DLT645_PRTL20
	{HB_MUL_STD_APP,"�����๦�ܱ�"},
	{HB_JY_WP_APP,"���������з���"},
	{HB_JY_NP_APP,"���������޷���"},
	{HB_JY_STD_APP,"������ױ�"},
    #endif	
    #endif
};
uint32 g_nMettypeCnt = sizeof(g_MettypeContrast)/sizeof(g_MettypeContrast[0]);

/******************************************************************************
* ��������:	MtTyp_To_IdMtTyp
* ��������:	����Լ֧�ֵĸ��ֵ������ת��Ϊ�ڲ�����ĵ������
* �� �� ֵ:	MtTyp			��Լ֧�ֵĸ��ֵ������
* �� �� ֵ:	>0				�ڲ�����ĵ������
			=0				δ�ӱ�
* �ڲ�����
*******************************************************************************/
uint8 MtTyp_To_IdMtTyp(uint8 MtTyp)
{
    uint8 IdMtTyp = NO_MET_APP;					//�ڲ�����ĵ�����ͣ�0���ӱ�

    switch (MtTyp)
    {
    case DL2007_PRTL:
    case DL2007_PLC:
        IdMtTyp = DLT645_2007;
        break;

    case DLT645_APP:							//1:�����Լ�� �����������������й�����ʤ������
        IdMtTyp = DLT645;
        break;

    case  AC_SAMPINGMET:						//2:����������
        #if AC_CALC_EN == 1
        IdMtTyp = NZJC;
        #else
        IdMtTyp = DLT645;						//���ɱ��չ����������
        #endif
        break;

    case ANHUI_MET:								//3:���չ����
        IdMtTyp = DLT645;
        break;

    case  FJ_APP:								//3:�����Ϲ�(����ͳһ��Լ���)
        IdMtTyp = FJ;
        break;

    case LGR_APP_D3:							//4:LANDIS_D��(��������)   
    case LGR_APP:
    case LGR_D_MET_B3:		  	
    case LGR_D_MET:			
        IdMtTyp = LGR;
        break;

    case ABB_APP:								//ABB(����)
    case ABB_APP_S:								//ABB(Բ��)
        IdMtTyp = ABB;
        break;

    case EDMI_APP:								//EDMI��
    case EDMI_APP_Y:							//EDMI��
        IdMtTyp = EDMI;
        break;

    case WS20_APP:								//������ʤ��� 
        IdMtTyp = WS20;
        break;

    case DSSD25_APP:							//�㽭��Լ
        IdMtTyp = DSSD25;
        break;

    case HETONG_OLD:							//15:��ͨ(�ɹ�Լ) 16, ��ͨ���(�¹�Լ)
        IdMtTyp = HT;
        break;

    case HETONG_NEW:
        IdMtTyp = HT_NEW;
        break;

    case CQINTELL_APP:		
        IdMtTyp = CQINTELL; 					//��������
        break;

    case HND_APP:								//������(�������Լ���Լ)
        IdMtTyp = HND;
        break;

    case JIANGBEI:								//�������
        break;

    case WXABB_APP:								//�¹�ABB(ELSTER)
        IdMtTyp = WXABB;
        break;

    case LG9600_APP:							//�����°�LDS_D(9600����)
        IdMtTyp = LG9600;
        break;
    case JSSIMULATE_APP:						//����ģ������
        IdMtTyp = JSSIMULATE;
        break;
    case SIMPLE_DLT_APP:						//����������֡����������Ƚ��٣�
        IdMtTyp = DLT645JY;
        break;

    case FQD_APP:
        IdMtTyp = FQD;							//���Ե��
        break;

    case DLT645_TLLD:							//���ܷ��Ե��
        IdMtTyp = DLT645_0;
        break;

    #if JIANG_SU_FUN == 1
    case DLT645_FFEE_APP:
        IdMtTyp = DLT645_FFEE;
        break;
    case DLT645_TFEE_APP:
        IdMtTyp = DLT645_TFEE;
        break;
    case DL2007_PLC_EVT:
    	IdMtTyp = DLT645_2007_EVT;
    	break;

    #if OS_SMARTBREAKER_EN == 1
    case SMARTBREAKER_PRTL:      //���ܶ�·����Լ
        IdMtTyp = SMARTBREAKER;
        break;
    #endif
    #endif

    case  ZMC_APP:
        IdMtTyp = ZMC;
        break;

    case EMAIL_APP:
        IdMtTyp = EMAIL;
        break;

    case DLMS_APP:
        IdMtTyp = DLMS;
        break;

	#if HUBEI_FUN == 1
	case HB_STD_APP:			//���棨�๦�ܣ�
		IdMtTyp = HB_STD;
		break;
		
   	case HB_JY_APP:				//���棨���׶๦�ܣ�
		IdMtTyp = HB_JY;
		break;
		
	case HB_PT_APP:				//���棨��ͨ����ʽ���ܱ�
		IdMtTyp = HB_PT;
		break;	
		
	case HB_FFL_APP:			//���棨���񸴷��ʵ��ܱ�
		IdMtTyp = HB_FFL;
		break;

	case DLT645_PRTL20:
		IdMtTyp = FQDMK;
		break;

    case HB_MUL_STD_APP:
        IdMtTyp = HB_MUL_STD;
        break;

    case HB_JY_WP_APP:
        IdMtTyp = HB_JY_WP;
        break;

    case HB_JY_NP_APP:
        IdMtTyp = HB_JY_NP;
        break;

    case HB_JY_STD_APP:
        IdMtTyp = DLT645JY;
        break;
	#endif	

#if STD_698PRO_BETA == 1
	case OOP_PRTL:
        IdMtTyp = DLTOOP;
        break;
#endif

    case NO_MET_APP:
    default:
        break;
    }
    return IdMtTyp;
}


char* get_met_type_str(uint32 type)
{
    switch(type)
    {
    case DLT645:
        return "����";
    case LGR:
        return "LGR";
    case WS20:
        return "��ʤ";
    case ABB:
        return "ABB";
    case EDMI:
        return "����";
    case JSBLACK:
        return "���ս���";
    case HND:
        return "������";
    case HT:
        return "��ͨ";
    case NZJC:
        return "���ý���";
    case WXABB:
        return "����ABB";
    case DSSD25:
        return "�㽭��Լ";
    case DLT645TJ:
        return "����Լ";
    case FQD:
        return "���Ե�";
    case DLT645JY:
        return "���׹���";
    case JLB:
        return "���ֹ�Լ";
    case JSSIMULATE:
        return "����ģ��";
    case DLT645_2007:
    	return "07����";
    case ZMC:
        return "������C";
    case DLT645_0:
        return "���ܷ��Ե�";
    case FJ:
        return "�������";
    case CBH:
        return "�����";
    case DLMS:
        return "DLMS";
    case ACTARIS:
        return "ACTARIS";
    case DIRECTSIM:
        return "ֱ��ģ��";
    case HR:
        return "г��";
    case LG9600:
        return "������D";
    case ELSTER:
        return "ELSTER";
    case EMAIL:
        return "email";
    case DLT645CQJY:
        return "�������";
    case ZZDB:
        return "֣�ݹ�Լ";
    case CQINTELL:
        return "�����";
    case ABB_S:
        return "ABBԲ��";
    case FJ_KN:
        return "���ܱ�";
    case LGR_B:
        return "������B";

	#if HUBEI_FUN == 1
	case HB_STD:			//���棨�๦�ܣ�
        return "����๦��";
   	case HB_JY:				//���棨���׶๦�ܣ�
        return "������׶๦��";
	case HB_PT:				//���棨��ͨ����ʽ���ܱ�
        return "������ͨ";
	case HB_FFL:			//���棨���񸴷��ʵ��ܱ�
        return "�������";
	case HB_MUL_STD:
        return "�����๦�ܱ�";
	case HB_JY_WP:
        return "���������з���";
	case HB_JY_NP:
        return "���������޷���";
    #endif

    #if (JIANG_SU_FUN == 1 && OS_SMARTBREAKER_EN == 1)
    case SMARTBREAKER:
        return "���ܶ�·��";
    #endif

#if STD_698PRO_BETA == 1
	case DLTOOP:
		return "��������Լ���";
#endif
    }
    return NULL;
}
