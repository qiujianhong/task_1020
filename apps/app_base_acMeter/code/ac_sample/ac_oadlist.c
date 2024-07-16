/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#include "ac_sample698.h"
#include "ac_sample.h"
#include "ac_update.h"
#include "ac_update698.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac_mqtt.h"
#include "ac_save.h"
#include "commClass.h"
#include "ac_report.h"
#include "ac_parser.h"
#include "ac_oadlist.h"

//交采实时数据及数量
const OAD_INFO_MAP_T gRealDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_REALTIME, {0x20000200}, 3,  DT_ARRAY,                11, 2, E_OOP_WORD3V_T,          -1, &TermSharedData.PhaAVolt,         NULL,   ac_Volt_parser       }, 
    {SAMPLE_REALTIME, {0x20000400}, 3,  DT_ARRAY,                0,  0, E_OOP_DWORD3V_T,         -1, &TermSharedData.PhaAVoltHD,       NULL,   ac_VoltHD_parser     }, 
    {SAMPLE_REALTIME, {0x20000600}, 3,  DT_LONG_UNSIGNED,        3,  2, E_LONG_UNSIGNED,         -1, &TermSharedData.PhaZeroVolt,      NULL,   ac_uint16_parser     }, 
    {SAMPLE_REALTIME, {0x20000a00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineABVolt,       NULL,   ac_uint32_parser     },
    {SAMPLE_REALTIME, {0x20000b00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineBCVolt,       NULL,   ac_uint32_parser     },
    {SAMPLE_REALTIME, {0x20000c00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineCAVolt,       NULL,   ac_uint32_parser     },
    {SAMPLE_REALTIME, {0x20010200}, 3,  DT_ARRAY,                14, 3, E_OOP_INT3V_T,           -1, &TermSharedData.PhaACurr,         NULL,   ac_Curr_parser       }, 
    {SAMPLE_REALTIME, {0x20010400}, 3,  DT_DOUBLE_LONG,          0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.PhaGCurr,         NULL,   ac_zero_line_Curr_parser }, 
    {SAMPLE_REALTIME, {0x20010500}, 3,  DT_DOUBLE_LONG,          0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.ResidCurr,        NULL,   ac_int32_parser      },
    {SAMPLE_REALTIME, {0x20010600}, 3,  DT_DOUBLE_LONG,          0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.PhaMCurr,         NULL,   ac_zero_Curr_parser  },
    {SAMPLE_REALTIME, {0x20020200}, 3,  DT_ARRAY,                0,  0, E_OOP_WORD3V_T,          -1, &TermSharedData.PhaAVoltAngle,    NULL,   ac_Volt_parser       },
    {SAMPLE_REALTIME, {0x20030200}, 3,  DT_ARRAY,                0,  0, E_OOP_WORD3V_T,          -1, &TermSharedData.PhaAAngle,        NULL,   ac_PhAngle_parser    },
    {SAMPLE_REALTIME, {0x20040200}, 3,  DT_ARRAY,                0,  0, E_OOP_INT4V_T,           -1, &TermSharedData.PosActPower,      NULL,   ac_ActPower_parser   }, 
    {SAMPLE_REALTIME, {0x20050200}, 3,  DT_ARRAY,                0,  0, E_OOP_INT4V_T,           -1, &TermSharedData.PosReaPower,      NULL,   ac_ReaPower_parser   }, 
    {SAMPLE_REALTIME, {0x20050a00}, 3,  DT_ARRAY,                0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.PhaAReaPowerBasic,NULL,   ac_int32_parser      },
    {SAMPLE_REALTIME, {0x20050b00}, 3,  DT_ARRAY,                0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.PhaBReaPowerBasic,NULL,   ac_int32_parser      },
    {SAMPLE_REALTIME, {0x20050c00}, 3,  DT_ARRAY,                0,  0, E_DOUBLE_LONG,           -1, &TermSharedData.PhaCReaPowerBasic,NULL,   ac_int32_parser      },
    {SAMPLE_REALTIME, {0x20060200}, 3,  DT_ARRAY,                0,  0, E_OOP_INT4V_T,           -1, &TermSharedData.PosApparentPower, NULL,   ac_AppPower_parser   },
    {SAMPLE_REALTIME, {0x200A0200}, 3,  DT_ARRAY,                0,  0, E_OOP_LONG4V_T,          -1, &TermSharedData.PowerFactor,      NULL,   ac_PowerFactor_parser},
    {SAMPLE_REALTIME, {0x200F0200}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineFrz,          NULL,   ac_HZ_parser         },
    {SAMPLE_REALTIME, {0x200F0A00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineAFrz,         NULL,   ac_HZ_parser         },
    {SAMPLE_REALTIME, {0x200F0B00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineBFrz,         NULL,   ac_HZ_parser         },
    {SAMPLE_REALTIME, {0x200F0C00}, 3,  DT_DOUBLE_LONG_UNSIGNED, 0,  0, E_DOUBLE_LONG_UNSIGNED,  -1, &TermSharedData.LineCFrz,         NULL,   ac_HZ_parser         },
    {SAMPLE_REALTIME, {0x20100200}, 3,  DT_LONG,                 0,  0, E_LONG,                  -1, &TermSharedData.MtrTemp,          NULL,   ac_int16_parser      },
    {SAMPLE_REALTIME, {0x20260200}, 6,  DT_LONG_UNSIGNED,        0,  0, E_LONG_UNSIGNED,         -1, &TermSharedData.VoltUnbalanceRate,NULL,   ac_uint16_parser     },
    {SAMPLE_REALTIME, {0x20270200}, 6,  DT_LONG_UNSIGNED,        0,  0, E_LONG_UNSIGNED,         -1, &TermSharedData.CurrUnbalanceRate,NULL,   ac_uint16_parser     },
    {SAMPLE_REALTIME, {0x20140200}, 6,  DT_ARRAY,                0,  0, E_NULL,                   0, &TermSharedData.MetStusPara.Rsw1, NULL,   ac_MeterSstate_parser},
};
const uint32 gRealDataMapNum = sizeof(gRealDataMap)/sizeof(OAD_INFO_MAP_T);  

//交采电量数据及数量
const OAD_INFO_MAP_T gEnergyDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_ENERGY,   {0x00000200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_T,    -2, &TermSharedData.CmbActEng[0],       NULL, ac_Energy_parser}, 
    {SAMPLE_ENERGY,   {0x00010200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaACmbActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00020200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaBCmbActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00030200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaCCmbActEng[0],   NULL, ac_Energy_parser},

    {SAMPLE_ENERGY,   {0x00100200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_T,     -2, &TermSharedData.PosActEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00110200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaAPosActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00120200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaBPosActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00130200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaCPosActEng[0],   NULL, ac_Energy_parser},  

    {SAMPLE_ENERGY,   {0x00200200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_T,     -2, &TermSharedData.NegActEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00210200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaANegActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00220200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaBNegActEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00230200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaCNegActEng[0],   NULL, ac_Energy_parser},  

    {SAMPLE_ENERGY,   {0x00300200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_T,    -2, &TermSharedData.PosReaEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00310200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaAPosReaEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00320200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaBPosReaEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00330200}, 1,  DT_ARRAY, 22, 4,  E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaCPosReaEng[0],   NULL, ac_Energy_parser},  

    {SAMPLE_ENERGY,   {0x00400200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGYS_T,    -2, &TermSharedData.NegReaEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00410200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaANegReaEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00420200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaBNegReaEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00430200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGYS_ONE_T,-2, &TermSharedData.PhaCNegReaEng[0],   NULL, ac_Energy_parser},  

    {SAMPLE_ENERGY,   {0x00500200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.Quad1ReaEng[0],     NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00510200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaA1ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00520200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaB1ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00530200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaC1ReaEng[0],     NULL, ac_Energy_parser}, 

    {SAMPLE_ENERGY,   {0x00600200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.Quad2ReaEng[0],     NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00610200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaA2ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00620200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaB2ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00630200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaC2ReaEng[0],     NULL, ac_Energy_parser}, 

    {SAMPLE_ENERGY,   {0x00700200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.Quad3ReaEng[0],     NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00710200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaA3ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00720200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaB3ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00730200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaC3ReaEng[0],     NULL, ac_Energy_parser}, 

    {SAMPLE_ENERGY,   {0x00800200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.Quad4ReaEng[0],     NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00810200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaA4ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00820200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaB4ReaEng[0],     NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00830200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaC4ReaEng[0],     NULL, ac_Energy_parser},

    {SAMPLE_ENERGY,   {0x00900200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.PosAppEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00910200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaAActAppEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00920200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaBActAppEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00930200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaCActAppEng[0],   NULL, ac_Energy_parser},

    {SAMPLE_ENERGY,   {0x00a00200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_T,     -2, &TermSharedData.NegAppEng[0],       NULL, ac_Energy_parser},
    {SAMPLE_ENERGY,   {0x00a10200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaANegAppEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00a20200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaBNegAppEng[0],   NULL, ac_Energy_parser},  
    {SAMPLE_ENERGY,   {0x00a30200}, 1,  DT_ARRAY, 22,  4, E_OOP_ENERGY_ONE_T, -2, &TermSharedData.PhaCNegAppEng[0],   NULL, ac_Energy_parser},
};
const uint32 gEnergyDataMapNum = sizeof(gEnergyDataMap)/sizeof(OAD_INFO_MAP_T); 

//交采高精度电量数据及数量
const OAD_INFO_MAP_T gHEnergyDataMap[] =
{
    { SAMPLE_HENERGY,   {0x00000400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_T,    -4, &TermSharedData.CmbActEngHD[0],       &TermSharedData.CmbActEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00010400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaACmbActEngHD[0],   &TermSharedData.PhaACmbActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00020400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaBCmbActEngHD[0],   &TermSharedData.PhaBCmbActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00030400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaCCmbActEngHD[0],   &TermSharedData.PhaCCmbActEng[0], ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00100400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_T,     -4, &TermSharedData.PosActEngHD[0],       &TermSharedData.PosActEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00110400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaAPosActEngHD[0],   &TermSharedData.PhaAPosActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00120400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaBPosActEngHD[0],   &TermSharedData.PhaBPosActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00130400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaCPosActEngHD[0],   &TermSharedData.PhaCPosActEng[0], ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00200400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_T,     -4, &TermSharedData.NegActEngHD[0],       &TermSharedData.NegActEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00210400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaANegActEngHD[0],   &TermSharedData.PhaANegActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00220400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaBNegActEngHD[0],   &TermSharedData.PhaBNegActEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00230400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaCNegActEngHD[0],   &TermSharedData.PhaCNegActEng[0], ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00300400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_T,    -4, &TermSharedData.PosReaEngHD[0],       &TermSharedData.PosReaEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00310400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaAPosReaEngHD[0],   &TermSharedData.PhaAPosReaEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00320400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaBPosReaEngHD[0],   &TermSharedData.PhaBPosReaEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00330400}, 1,  DT_ARRAY, 42, 8,  E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaCPosReaEngHD[0],   &TermSharedData.PhaCPosReaEng[0], ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00400400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGYS_T,    -4, &TermSharedData.NegReaEngHD[0],       &TermSharedData.NegReaEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00410400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaANegReaEngHD[0],   &TermSharedData.PhaANegReaEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00420400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaBNegReaEngHD[0],   &TermSharedData.PhaBNegReaEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00430400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGYS_ONE_T,-4, &TermSharedData.PhaCNegReaEngHD[0],   &TermSharedData.PhaCNegReaEng[0], ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00500400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.Quad1ReaEngHD[0],     &TermSharedData.Quad1ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00510400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaA1ReaEngHD[0],     &TermSharedData.PhaA1ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00520400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaB1ReaEngHD[0],     &TermSharedData.PhaB1ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00530400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaC1ReaEngHD[0],     &TermSharedData.PhaC1ReaEng[0],   ac_Energy_parser},

    { SAMPLE_HENERGY,   {0x00600400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.Quad2ReaEngHD[0],     &TermSharedData.Quad2ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00610400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaA2ReaEngHD[0],     &TermSharedData.PhaA2ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00620400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaB2ReaEngHD[0],     &TermSharedData.PhaB2ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00630400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaC2ReaEngHD[0],     &TermSharedData.PhaC2ReaEng[0],   ac_Energy_parser},
                                                                                                                                                                             
    { SAMPLE_HENERGY,   {0x00700400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.Quad3ReaEngHD[0],     &TermSharedData.Quad3ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00710400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaA3ReaEngHD[0],     &TermSharedData.PhaA3ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00720400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaB3ReaEngHD[0],     &TermSharedData.PhaB3ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00730400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaC3ReaEngHD[0],     &TermSharedData.PhaC3ReaEng[0],   ac_Energy_parser},
                                                                                                                                                                             
    { SAMPLE_HENERGY,   {0x00800400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.Quad4ReaEngHD[0],     &TermSharedData.Quad4ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00810400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaA4ReaEngHD[0],     &TermSharedData.PhaA4ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00820400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaB4ReaEngHD[0],     &TermSharedData.PhaB4ReaEng[0],   ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00830400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaC4ReaEngHD[0],     &TermSharedData.PhaC4ReaEng[0],   ac_Energy_parser},
                                                                                                                                                                             
    { SAMPLE_HENERGY,   {0x00900400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.PosAppEngHD[0],       &TermSharedData.PosAppEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00910400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaAActAppEngHD[0],   &TermSharedData.PhaAActAppEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00920400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaBActAppEngHD[0],   &TermSharedData.PhaBActAppEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00930400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaCActAppEngHD[0],   &TermSharedData.PhaCActAppEng[0], ac_Energy_parser},
                                                                                                                                                                             
    { SAMPLE_HENERGY,   {0x00a00400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_T,     -4, &TermSharedData.NegAppEngHD[0],       &TermSharedData.NegAppEng[0],     ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00a10400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaANegAppEngHD[0],   &TermSharedData.PhaANegAppEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00a20400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaBNegAppEngHD[0],   &TermSharedData.PhaBNegAppEng[0], ac_Energy_parser},
    { SAMPLE_HENERGY,   {0x00a30400}, 1,  DT_ARRAY, 42,  8, E_OOP_HENERGY_ONE_T, -4, &TermSharedData.PhaCNegAppEngHD[0],   &TermSharedData.PhaCNegAppEng[0], ac_Energy_parser},
};
const uint32 gHEnergyDataMapNum = sizeof(gHEnergyDataMap) / sizeof(OAD_INFO_MAP_T);

//交采需量数据及数量
const OAD_INFO_MAP_T gDemandDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_DEMAND,   {0x10100200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.CurrDemand[0],   &TermSharedData.CurrDemandTime[0],   ac_Demand_parser}, 
    {SAMPLE_DEMAND,   {0x10110200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.PosActDemand[0], &TermSharedData.PosActDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10120200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.PosActDemand[1], &TermSharedData.PosActDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10130200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.PosActDemand[2], &TermSharedData.PosActDemandTime[2], ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10200200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.CurrDemand[1],   &TermSharedData.CurrDemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10210200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegActDemand[0], &TermSharedData.NegActDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10220200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegActDemand[1], &TermSharedData.NegActDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10230200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegActDemand[2], &TermSharedData.NegActDemandTime[2], ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10300200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_T,    0, &TermSharedData.CurrDemand[2],   &TermSharedData.CurrDemandTime[2],   ac_Demand_parser}, 
    {SAMPLE_DEMAND,   {0x10310200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.PosReaDemand[0], &TermSharedData.PosReaDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10320200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.PosReaDemand[1], &TermSharedData.PosReaDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10330200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.PosReaDemand[2], &TermSharedData.PosReaDemandTime[2], ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10400200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_T,    0, &TermSharedData.CurrDemand[3],   &TermSharedData.CurrDemandTime[3],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10410200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.NegReaDemand[0], &TermSharedData.NegReaDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10420200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.NegReaDemand[1], &TermSharedData.NegReaDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10430200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMANDS_ONE_T,0, &TermSharedData.NegReaDemand[2], &TermSharedData.NegReaDemandTime[2], ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10500200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.QuaDemand[0],    &TermSharedData.QuaDemandTime[0],    ac_Demand_parser}, 
    {SAMPLE_DEMAND,   {0x10510200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua1Demand[0],   &TermSharedData.Qua1DemandTime[0],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10520200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua1Demand[1],   &TermSharedData.Qua1DemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10530200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua1Demand[2],   &TermSharedData.Qua1DemandTime[2],   ac_Demand_parser},
};
const uint32 gDemandDataMapNum = sizeof(gDemandDataMap)/sizeof(OAD_INFO_MAP_T); 

//交采需量数据及数量
const OAD_INFO_MAP_T gDemandDataMap1[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_DEMAND,   {0x10600200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.QuaDemand[1],    &TermSharedData.QuaDemandTime[1],    ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10610200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua2Demand[0],   &TermSharedData.Qua2DemandTime[0],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10620200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua2Demand[1],   &TermSharedData.Qua2DemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10630200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua2Demand[2],   &TermSharedData.Qua2DemandTime[2],   ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10700200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.QuaDemand[2],    &TermSharedData.QuaDemandTime[2],    ac_Demand_parser}, 
    {SAMPLE_DEMAND,   {0x10710200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua3Demand[0],   &TermSharedData.Qua3DemandTime[0],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10720200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua3Demand[1],   &TermSharedData.Qua3DemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10730200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua3Demand[2],   &TermSharedData.Qua3DemandTime[2],   ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10800200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.QuaDemand[3],    &TermSharedData.QuaDemandTime[3],    ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10810200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua4Demand[0],   &TermSharedData.Qua4DemandTime[0],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10820200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua4Demand[1],   &TermSharedData.Qua4DemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10830200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.Qua4Demand[2],   &TermSharedData.Qua4DemandTime[2],   ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10900200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.AppaDemand[0],   &TermSharedData.AppaDemandTime[0],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10910200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.ActAppDemand[0], &TermSharedData.ActAppDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10920200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.ActAppDemand[1], &TermSharedData.ActAppDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10930200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.ActAppDemand[2], &TermSharedData.ActAppDemandTime[2], ac_Demand_parser},
                                                                                                                                                                      
    {SAMPLE_DEMAND,   {0x10a00200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_T,     0, &TermSharedData.AppaDemand[1],   &TermSharedData.AppaDemandTime[1],   ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10a10200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegAppDemand[0], &TermSharedData.NegAppDemandTime[0], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10a20200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegAppDemand[1], &TermSharedData.NegAppDemandTime[1], ac_Demand_parser},
    {SAMPLE_DEMAND,   {0x10a30200}, 2,  DT_ARRAY, 0,  0, E_OOP_DEMAND_ONE_T, 0, &TermSharedData.NegAppDemand[2], &TermSharedData.NegAppDemandTime[2], ac_Demand_parser},
};
const uint32 gDemandDataMapNum1 = sizeof(gDemandDataMap1)/sizeof(OAD_INFO_MAP_T); 

//交采谐波数据及数量
const OAD_INFO_MAP_T gHarmonicDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_HARMONIC,   {0x200D0200}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaAVoltHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x200D0300}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaBVoltHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x200D0400}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaCVoltHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x200E0200}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaACurrHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x200E0300}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaBCurrHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x200E0400}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_T,            0, &TermSharedData.PhaCCurrHarmonic[0],      NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x20330200}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_U_VALUE_T,    0, &TermSharedData.PhaAVoltHarmonicValue[0], NULL, ac_Hormonic_parser}, 
    {SAMPLE_HARMONIC,   {0x20330300}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_U_VALUE_T,    0, &TermSharedData.PhaBVoltHarmonicValue[0], NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x20330400}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_U_VALUE_T,    0, &TermSharedData.PhaCVoltHarmonicValue[0], NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x20340200}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_I_VALUE_T,    0, &TermSharedData.PhaACurrHarmonicValue[0], NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x20340300}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_I_VALUE_T,    0, &TermSharedData.PhaBCurrHarmonicValue[0], NULL, ac_Hormonic_parser},
    {SAMPLE_HARMONIC,   {0x20340400}, 5,  DT_ARRAY,          0,   0,   E_OOP_HARMONIC_I_VALUE_T,    0, &TermSharedData.PhaCCurrHarmonicValue[0], NULL, ac_Hormonic_parser}, 
};
const uint32 gHarmonicDataMapNum = sizeof(gHarmonicDataMap)/sizeof(OAD_INFO_MAP_T); 

//其他数据
const OAD_INFO_MAP_T gOtherDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_OTHER,     {0x43000300}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, &gVerBuf[0],                       &gDateBuf[0],                    ac_Version_parser},
    {SAMPLE_OTHER,     {0xf0010400}, 5,  DT_BIT_STRING,     0,   0,    E_NULL,                      0, &g_iUpdate698Cfg.transStatus[0],   &g_iUpdate698Cfg.transStatusNum, ac_TransStatus_parser},
    {SAMPLE_OTHER,     {0x40100200}, 5,  DT_UNSIGNED,       0,   0,    E_NULL,                      0, &TermSharedData.powerType,         NULL,                            ac_PowerType_parser},
};
const uint32 gOtherDataMapNum = sizeof(gOtherDataMap)/sizeof(OAD_INFO_MAP_T); 

//数据总表
const OAD_INFO_LIST_T gOadInfoList[] =
{
    {SAMPLE_REALTIME, (OAD_INFO_MAP_T *)&gRealDataMap,     (uint32 *)&gRealDataMapNum},
    {SAMPLE_ENERGY,   (OAD_INFO_MAP_T *)&gEnergyDataMap,   (uint32 *)&gEnergyDataMapNum},
    {SAMPLE_HENERGY,  (OAD_INFO_MAP_T *)&gHEnergyDataMap,  (uint32 *)&gHEnergyDataMapNum},
    {SAMPLE_DEMAND,   (OAD_INFO_MAP_T *)&gDemandDataMap,   (uint32 *)&gDemandDataMapNum},
    {SAMPLE_DEMAND1,  (OAD_INFO_MAP_T *)&gDemandDataMap1,  (uint32 *)&gDemandDataMapNum1},
    {SAMPLE_HARMONIC, (OAD_INFO_MAP_T *)&gHarmonicDataMap, (uint32 *)&gHarmonicDataMapNum},
    {SAMPLE_OTHER,    (OAD_INFO_MAP_T *)&gOtherDataMap,    (uint32 *)&gOtherDataMapNum},
};
const uint32 gOadInfoListNum = sizeof(gOadInfoList)/sizeof(OAD_INFO_LIST_T);

//上报数据
const OAD_INFO_MAP_T gRptDataMap[] = 
{
    //                  oad   class  type        total加类型   len            type             换算      缓存
    {SAMPLE_OTHER,     {0x3e030200}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, &TermSharedData.TopoIdenResult,                       NULL, ac_rpt_Topo_parser},
    {SAMPLE_OTHER,     {0x30000700}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x30000800}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x30000900}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x30040700}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x30040800}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x30040900}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    {SAMPLE_OTHER,     {0x300D0200}, 5,  DT_STRUCTURE,      0,   0,    E_NULL,                      0, NULL,                       NULL, ac_rpt_meter_event_parser},
    //{SAMPLE_OTHER,     {0xfff20600}, 5,  DT_VISIBLE_STRING, 0,   0,    E_NULL,                      0, NULL,                                                 NULL, ac_rpt_log_parser},
};
const uint32 gRptDataMapNum = sizeof(gRptDataMap)/sizeof(OAD_INFO_MAP_T); 

//预处理报文
#ifdef AREA_SHANDONG
const AC_PRE_PROC_T gPreprocList[] = 
{
    //周休日特征字
    {
        SERVERID_SETREQUEST,
        {0x40120200},
        0x0B,
        {
            0x06, 0x01, 0x01, 0x40, 0x12, 0x02, 0x00, 0x04, 0x08, 0xfe, 0x00,
        },
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },
    
    //年时区数 日时段表数 时段数 费率数
    {
        SERVERID_SETREQUEST,
        {0x400c0200}, 
        0x14,
        {
            0x06, 0x01, 0x0c, 0x40, 0x0c, 0x02, 0x00, 0x02, 0x05, 0x11, 0x0e, 0x11, 0x08, 
            0x11, 0x0e, 0x11, 0x0c, 0x11, 0x00, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //备用套时段表
    {
        SERVERID_SETREQUEST,
        {0x40150200}, 
        0x7A,
        {
            0x06, 0x01, 0x10, 0x40, 0x15, 0x02, 0x00, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x01, 
            0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 
            0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 
            0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 
            0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 
            0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 
            0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 
            0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 
            0x01, 0x02, 0x03, 0x11, 0x01, 0x11, 0x01, 0x11, 0x01, 0x02, 0x03, 0x11, 0x01, 
            0x11, 0x01, 0x11, 0x01, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //时区表切换
    {
        SERVERID_SETREQUEST,
        {0x40080200},
        0x10,
        {
            0x06, 0x01, 0x21, 0x40, 0x08, 0x02, 0x00, 0x1c, 0x07, 0xe4, 0x01, 0x01, 0x01, 
            0x01, 0xff, 0x00,
        },
        5,
        NULL,
        NULL,
        ac_Set_normal_parser,
    },

    //召测验证时区表
    {
        SERVERID_GETREQUEST,
        {0x40140200},
        0x08,
        {
            0x05, 0x01, 0x00, 0x40, 0x14, 0x02, 0x00, 0x00,
        },
        0,
        NULL,
        NULL,
        ac_get_40140200_parser_sd,
    },

    //备用时段表
    {
        SERVERID_SETREQUEST,
        {0x40170200}, 
        0x39A,
        {
            0x06, 0x01, 0x00, 0x40, 0x17, 0x02, 0x00, 0x01, 0x08, 0x01, 0x0e, 0x02, 0x03, 
            0x11, 0x07, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 
            0x02, 0x02, 0x03, 0x11, 0x0a, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x0b, 
            0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 0x02, 
            0x03, 0x11, 0x0d, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 0x1e, 
            0x11, 0x02, 0x02, 0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 
            0x15, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 
            0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 
            0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 
            0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 0x11, 
            0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 
            0x11, 0x0a, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 0x11, 
            0x03, 0x02, 0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x0d, 
            0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 0x02, 
            0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 
            0x11, 0x03, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 
            0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 
            0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 
            0x00, 0x11, 0x04, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 0x11, 0x00, 0x11, 0x03, 
            0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x0a, 0x11, 
            0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 
            0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x0d, 0x11, 0x00, 0x11, 
            0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x13, 
            0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 0x02, 
            0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 
            0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 
            0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 
            0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 
            0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x0a, 0x11, 0x00, 0x11, 0x01, 
            0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0c, 0x11, 
            0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x0d, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 
            0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 
            0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x17, 
            0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 
            0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 
            0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x01, 0x0e, 0x02, 
            0x03, 0x11, 0x07, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 
            0x11, 0x02, 0x02, 0x03, 0x11, 0x0a, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 
            0x0b, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 
            0x02, 0x03, 0x11, 0x0d, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 
            0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 
            0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 
            0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 
            0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 
            0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 
            0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 
            0x03, 0x11, 0x0a, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 
            0x11, 0x03, 0x02, 0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 
            0x0d, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 
            0x02, 0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 
            0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 
            0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 
            0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 
            0x11, 0x00, 0x11, 0x04, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 0x11, 0x00, 0x11, 
            0x03, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x0a, 
            0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 0x11, 0x03, 0x02, 
            0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x0d, 0x11, 0x00, 
            0x11, 0x03, 0x02, 0x03, 0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 
            0x13, 0x11, 0x00, 0x11, 0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 
            0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 
            0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 
            0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 
            0x04, 0x01, 0x0e, 0x02, 0x03, 0x11, 0x07, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 
            0x11, 0x08, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x0a, 0x11, 0x00, 0x11, 
            0x01, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x0c, 
            0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x0d, 0x11, 0x00, 0x11, 0x03, 0x02, 
            0x03, 0x11, 0x0e, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x13, 0x11, 0x00, 
            0x11, 0x01, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 
            0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 
            0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 
            0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x04, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //时段表切换
    {
        SERVERID_SETREQUEST,
        {0x40090200},
        0x10,
        {
            0x06, 0x01, 0x21, 0x40, 0x09, 0x02, 0x00, 0x1c, 0x07, 0xe4, 0x01, 0x01, 0x01, 
            0x01, 0xff, 0x00,
        },
        5,
        NULL,
        NULL,
        ac_Set_normal_parser,
    },

    //验证时段表
    {
        SERVERID_GETREQUEST,
        {0x40160200},
        0x08,
        {
            0x05, 0x01, 0x00, 0x40, 0x16, 0x02, 0x00, 0x00,
        },
        0,
        NULL,
        NULL,
        ac_get_40160200_parser_sd,
    },
};
#elif AREA_FUJIAN
const AC_PRE_PROC_T gPreprocList[] = 
{
    {
        //年时区数 日时段表数 时段数 费率数
        SERVERID_SETREQUEST,
        {0x400c0200}, 
        0x14,
        {
            0x06, 0x01, 0x12, 0x40, 0x0c, 0x02, 0x00, 0x02, 0x05, 0x11, 0x01, 0x11, 0x08, 
            0x11, 0x09, 0x11, 0x04, 0x11, 0x00, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //备用时段表
    {
        SERVERID_SETREQUEST,
        {0x40170200}, 
        0x54,
        {
            0x06, 0x01, 0x02, 0x40, 0x17, 0x02, 0x00, 0x01, 0x01, 0x01, 0x09, 0x02, 0x03, 
            0x11, 0x07, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x08, 0x11, 0x1e, 0x11, 
            0x03, 0x02, 0x03, 0x11, 0x0b, 0x11, 0x1e, 0x11, 0x02, 0x02, 0x03, 0x11, 0x0e, 
            0x11, 0x1e, 0x11, 0x03, 0x02, 0x03, 0x11, 0x11, 0x11, 0x1e, 0x11, 0x02, 0x02, 
            0x03, 0x11, 0x13, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 
            0x11, 0x02, 0x02, 0x03, 0x11, 0x17, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 
            0x18, 0x11, 0x00, 0x11, 0x04, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //时段表切换
    {
        SERVERID_SETREQUEST,
        {0x40090200},
        0x10,
        {
            0x06, 0x01, 0x21, 0x40, 0x09, 0x02, 0x00, 0x1c, 0x07, 0xe4, 0x01, 0x01, 0x01, 
            0x01, 0xff, 0x00,
        },
        5,
        NULL,
        NULL,
        ac_Set_normal_parser,
    },

    //验证时段表
    {
        SERVERID_GETREQUEST,
        {0x40160201},
        0x08,
        {
            0x05, 0x01, 0x00, 0x40, 0x16, 0x02, 0x01, 0x00,
        },
        0,
        NULL,
        NULL,
        ac_get_40160201_parser_fj,
    },
};
#elif AREA_JIANGSU
const AC_PRE_PROC_T gPreprocList[] = 
{
    {
        //年时区数 日时段表数 时段数 费率数
        SERVERID_SETREQUEST,
        {0x400c0200}, 
        0x14,
        {
            0x06, 0x01, 0x12, 0x40, 0x0c, 0x02, 0x00, 0x02, 0x05, 0x11, 0x01, 0x11, 0x01, 
            0x11, 0x05, 0x11, 0x04, 0x11, 0x00, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //备用时段表
    {
        SERVERID_SETREQUEST,
        {0x40170200}, 
        0x34,
        {
            0x06, 0x01, 0x00, 0x40, 0x17, 0x02, 0x00, 0x01, 0x01, 0x01, 0x05, 0x02, 0x03, 
            0x11, 0x00, 0x11, 0x00, 0x11, 0x04, 0x02, 0x03, 0x11, 0x08, 0x11, 0x00, 0x11, 
            0x02, 0x02, 0x03, 0x11, 0x0c, 0x11, 0x00, 0x11, 0x03, 0x02, 0x03, 0x11, 0x11, 
            0x11, 0x00, 0x11, 0x02, 0x02, 0x03, 0x11, 0x15, 0x11, 0x00, 0x11, 0x03, 0x00,
        }, 
        0, 
        NULL, 
        NULL, 
        ac_Set_normal_parser,
    },

    //时段表切换
    {
        SERVERID_SETREQUEST,
        {0x40090200},
        0x10,
        {
            0x06, 0x01, 0x21, 0x40, 0x09, 0x02, 0x00, 0x1c, 0x07, 0xd0, 0x01, 0x01, 0x01, 
            0x01, 0xff, 0x00,
        },
        5,
        NULL,
        NULL,
        ac_Set_normal_parser,
    },

    //验证时段表
    {
        SERVERID_GETREQUEST,
        {0x40160201},
        0x08,
        {
            0x05, 0x01, 0x00, 0x40, 0x16, 0x02, 0x01, 0x00,
        },
        0,
        NULL,
        NULL,
        ac_get_40160201_parser_js,
    },
};
#else
const AC_PRE_PROC_T gPreprocList[] = 
{
};
#endif

const uint32 gPreprocListNum = sizeof(gPreprocList)/sizeof(AC_PRE_PROC_T);

