/*
***************************************************************************
  Copyright (c) 2006-2007,南京新联电子仪器有限责任公司
             All rights reserved.
    程序名称:   mettype.c
    功    能：表类型定义
    开 发 人：高一峰  
    开发时间：2008-01-07
    
    版本历史:
            2008-01-07 ,v1.0 : New              
    
***************************************************************************
*/
//#include "data_type.h"
#include "sys_cfg.h"
#include "mettype.h"

METTYPE_CONTRAST g_MettypeContrast[] = 
{
	#if STD_698PRO_BETA == 1
	{UNKNOW, "未知"},
	{DLT645_97, "DLT645-97"},
	{DLT645_07, "DLT645-07"},
	{DLT698_45, "DLT698.45"},
	{CJ_T188_04, "CJ/T188_2004"},
	
	#endif
    #if BEIJING_FUN
    {DLT645_APP,    "DLT645_97"},
    {DL2007_PRTL,   "DLT645_07"},
    {ABB_APP,       "ABB方表"},
    {ABB_APP_S,     "ABB圆表"},
    {HETONG_OLD,    "恒通I型"},
    {HETONG_NEW,    "恒通II型"},
    {LGR_APP_D3,    "兰吉尔d-33"},
    {LGR_APP,       "兰吉尔d-34"},
    {LGR_D_MET_B3,  "兰吉尔B-33"},
    {LGR_D_MET,     "兰吉尔B-34"},
    {EDMI_APP,      "红相MK6"},
    {EDMI_APP_Y,    "红相MK3"},
    {WS20_APP,      "威胜电表"},
    {HND_APP,       "宁光电表"},
    /*#else
    {DLT645_APP,    "国标电表"},
    {AC_SAMPINGMET, "交采电表"},
    {ABB_APP,       "ABB方表"},
    {ABB_APP_S,     "ABB圆表"},
    {HETONG_OLD, 	"恒通I型"},
    {HETONG_NEW,   	"恒通II型"},
    {LGR_APP_D3,    "兰吉尔d-33"},
    {LGR_APP,       "兰吉尔d-34"},
    {LGR_D_MET_B3,  "兰吉尔B-33"},
    {LGR_D_MET,     "兰吉尔B-34"},
    {EDMI_APP,      "红相MK6"},
    {EDMI_APP_Y,    "红相MK3"},
    {WS20_APP,      "威胜电表"},
    {HND_APP,       "宁光电表"},
    {WXABB_APP,     "无锡ABB"},
    {LG9600_APP,    "无锡LDS_D"},
    {SIMPLE_DLT_APP,"特殊国标表"},
    {FQD_APP,       "防窃电表"},
    {DLT645_TLLD,   "三能防窃电表"},
    {DL2007_PRTL,	"国标2007"},
    {DLMS_APP,      "DLMS规约"},*/

    #if JIANG_SU_FUN == 1
    {DLT645_FFEE_APP, 	"四费率97表"},
    {DLT645_TFEE_APP, 	"二费率97表"},
    {DL2007_PLC_EVT, 	"13智能07表"},

    #if OS_SMARTBREAKER_EN == 1
    {SMARTBREAKER_PRTL,     "智能断路器"},      //智能断路器规约
    #endif
    #endif  

	#if HUBEI_FUN == 1    
    {HB_STD_APP, 	"鄂规多功能"},
    {HB_JY_APP, 	"鄂规简易多功能"},
    {HB_PT_APP, 	"鄂规普通"},
    {HB_FFL_APP, 	"鄂规居民"},
    {DLT645_PRTL20,	"数据转换模块"},//防窃电模块:辅助实现防窃电功能的数据转换模块，注:湖北要求对应 DLT645_PRTL20
	{HB_MUL_STD_APP,"湖北多功能表"},
	{HB_JY_WP_APP,"湖北简易有分相"},
	{HB_JY_NP_APP,"湖北简易无分相"},
	{HB_JY_STD_APP,"国标简易表"},
    #endif	
    #endif
};
uint32 g_nMettypeCnt = sizeof(g_MettypeContrast)/sizeof(g_MettypeContrast[0]);

/******************************************************************************
* 函数名称:	MtTyp_To_IdMtTyp
* 函数功能:	将规约支持的各种电表类型转换为内部定义的电表类型
* 输 入 值:	MtTyp			规约支持的各种电表类型
* 返 回 值:	>0				内部定义的电表类型
			=0				未接表
* 内部流程
*******************************************************************************/
uint8 MtTyp_To_IdMtTyp(uint8 MtTyp)
{
    uint8 IdMtTyp = NO_MET_APP;					//内部定义的电表类型，0不接表

    switch (MtTyp)
    {
    case DL2007_PRTL:
    case DL2007_PLC:
        IdMtTyp = DLT645_2007;
        break;

    case DLT645_APP:							//1:国标规约表 （国标照明、国标有功，威胜照明）
        IdMtTyp = DLT645;
        break;

    case  AC_SAMPINGMET:						//2:交流采样表
        #if AC_CALC_EN == 1
        IdMtTyp = NZJC;
        #else
        IdMtTyp = DLT645;						//交采表按照国标表来抄表
        #endif
        break;

    case ANHUI_MET:								//3:安徽国标表
        IdMtTyp = DLT645;
        break;

    case  FJ_APP:								//3:福州上工(福建统一规约电表)
        IdMtTyp = FJ;
        break;

    case LGR_APP_D3:							//4:LANDIS_D表(三相三线)   
    case LGR_APP:
    case LGR_D_MET_B3:		  	
    case LGR_D_MET:			
        IdMtTyp = LGR;
        break;

    case ABB_APP:								//ABB(方表)
    case ABB_APP_S:								//ABB(圆表)
        IdMtTyp = ABB;
        break;

    case EDMI_APP:								//EDMI表
    case EDMI_APP_Y:							//EDMI表
        IdMtTyp = EDMI;
        break;

    case WS20_APP:								//湖南威胜电表 
        IdMtTyp = WS20;
        break;

    case DSSD25_APP:							//浙江规约
        IdMtTyp = DSSD25;
        break;

    case HETONG_OLD:							//15:恒通(旧规约) 16, 恒通电表(新规约)
        IdMtTyp = HT;
        break;

    case HETONG_NEW:
        IdMtTyp = HT_NEW;
        break;

    case CQINTELL_APP:		
        IdMtTyp = CQINTELL; 					//重庆智能
        break;

    case HND_APP:								//宁光电表(浩宁达自己规约)
        IdMtTyp = HND;
        break;

    case JIANGBEI:								//江北电表
        break;

    case WXABB_APP:								//德国ABB(ELSTER)
        IdMtTyp = WXABB;
        break;

    case LG9600_APP:							//无锡新版LDS_D(9600波特)
        IdMtTyp = LG9600;
        break;
    case JSSIMULATE_APP:						//江苏模拟量表
        IdMtTyp = JSSIMULATE;
        break;
    case SIMPLE_DLT_APP:						//特殊国标表（单帧抄表，数据项比较少）
        IdMtTyp = DLT645JY;
        break;

    case FQD_APP:
        IdMtTyp = FQD;							//防窃电表
        break;

    case DLT645_TLLD:							//三能防窃电表
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
    case SMARTBREAKER_PRTL:      //智能断路器规约
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
	case HB_STD_APP:			//鄂规（多功能）
		IdMtTyp = HB_STD;
		break;
		
   	case HB_JY_APP:				//鄂规（简易多功能）
		IdMtTyp = HB_JY;
		break;
		
	case HB_PT_APP:				//鄂规（普通电子式电能表）
		IdMtTyp = HB_PT;
		break;	
		
	case HB_FFL_APP:			//鄂规（居民复费率电能表）
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
        return "国标";
    case LGR:
        return "LGR";
    case WS20:
        return "威胜";
    case ABB:
        return "ABB";
    case EDMI:
        return "红相";
    case JSBLACK:
        return "江苏交采";
    case HND:
        return "浩宁达";
    case HT:
        return "恒通";
    case NZJC:
        return "内置交采";
    case WXABB:
        return "无锡ABB";
    case DSSD25:
        return "浙江规约";
    case DLT645TJ:
        return "天津规约";
    case FQD:
        return "防窃电";
    case DLT645JY:
        return "简易国标";
    case JLB:
        return "吉林规约";
    case JSSIMULATE:
        return "江苏模拟";
    case DLT645_2007:
    	return "07国标";
    case ZMC:
        return "兰吉尔C";
    case DLT645_0:
        return "三能防窃电";
    case FJ:
        return "福建电表";
    case CBH:
        return "抄表盒";
    case DLMS:
        return "DLMS";
    case ACTARIS:
        return "ACTARIS";
    case DIRECTSIM:
        return "直流模拟";
    case HR:
        return "谐波";
    case LG9600:
        return "兰吉尔D";
    case ELSTER:
        return "ELSTER";
    case EMAIL:
        return "email";
    case DLT645CQJY:
        return "重庆简易";
    case ZZDB:
        return "郑州规约";
    case CQINTELL:
        return "重庆表";
    case ABB_S:
        return "ABB圆表";
    case FJ_KN:
        return "科能表";
    case LGR_B:
        return "兰吉尔B";

	#if HUBEI_FUN == 1
	case HB_STD:			//鄂规（多功能）
        return "鄂规多功能";
   	case HB_JY:				//鄂规（简易多功能）
        return "鄂规简易多功能";
	case HB_PT:				//鄂规（普通电子式电能表）
        return "鄂规普通";
	case HB_FFL:			//鄂规（居民复费率电能表）
        return "鄂规居民";
	case HB_MUL_STD:
        return "湖北多功能表";
	case HB_JY_WP:
        return "湖北简易有分相";
	case HB_JY_NP:
        return "湖北简易无分相";
    #endif

    #if (JIANG_SU_FUN == 1 && OS_SMARTBREAKER_EN == 1)
    case SMARTBREAKER:
        return "智能断路器";
    #endif

#if STD_698PRO_BETA == 1
	case DLTOOP:
		return "面向对象规约电表";
#endif
    }
    return NULL;
}
