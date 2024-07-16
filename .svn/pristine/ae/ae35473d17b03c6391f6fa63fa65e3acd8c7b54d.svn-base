/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析表
* @date： 2019.12.03
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"

//
//OAD列表
//

/* 电量量类OAD列表 OIA1 = 0x00 */
const OAD_TAB_T oadEnergyInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                    len                          eData*/
    {{0x00000200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //组合有功电能 - 电能量
    {{0x00000201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //组合有功电能-费率1 - 电能量
    {{0x00000400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //组合有功电能 - 高精度电能量
    {{0x00000401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //组合有功电能-费率1 - 电能量
    {{0x00100200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //正向有功电能 - 电能量
    {{0x00100201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //正向有功电能-费率1 - 电能量
    {{0x00100400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //正向有功电能 - 高精度电能量
    {{0x00100401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //正向有功电能-费率1 - 电能量
    {{0x00110200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相正向有功电能 - 电能量
    {{0x00110201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相正向有功电能-费率1 - 电能量
    {{0x00110400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相正向有功电能 - 高精度电能量
    {{0x00110401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相正向有功电能-费率1 - 电能量
    {{0x00120200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相正向有功电能 - 电能量
    {{0x00120201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相正向有功电能-费率1 - 电能量
    {{0x00120400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相正向有功电能 - 高精度电能量
    {{0x00120401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相正向有功电能-费率1 - 电能量
    {{0x00130200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相正向有功电能 - 电能量
    {{0x00130201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相正向有功电能-费率1 - 电能量
    {{0x00130400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相正向有功电能 - 高精度电能量
    {{0x00130401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相正向有功电能-费率1 - 电能量
    {{0x00200200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //反向有功电能 - 电能量
    {{0x00200201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //反向有功电能-费率1 - 电能量
    {{0x00200400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //反向有功电能 - 高精度电能量
    {{0x00200401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //反向有功电能-费率1 - 电能量
    {{0x00210200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相反向有功电能 - 电能量
    {{0x00210201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相反向有功电能-费率1 - 电能量
    {{0x00210400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相反向有功电能 - 高精度电能量
    {{0x00210401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相反向有功电能-费率1 - 电能量
    {{0x00220200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相反向有功电能 - 电能量
    {{0x00220201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相反向有功电能-费率1 - 电能量
    {{0x00220400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相反向有功电能 - 高精度电能量
    {{0x00220401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相反向有功电能-费率1 - 电能量
    {{0x00230200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相反向有功电能 - 电能量
    {{0x00230201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相反向有功电能-费率1 - 电能量
    {{0x00230400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相反向有功电能 - 高精度电能量
    {{0x00230401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相反向有功电能-费率1 - 电能量
    {{0x00300200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //组合无功1电能 - 电能量
    {{0x00300201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //组合无功1电能-费率1 - 电能量
    {{0x00300400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //组合无功1电能 - 高精度电能量
    {{0x00300401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //组合无功1电能-费率1 - 电能量
    {{0x00310200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //A相组合无功1电能 - 电能量
    {{0x00310201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //A相组合无功1电能-费率1 - 电能量
    {{0x00310400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //A相组合无功1电能 - 高精度电能量
    {{0x00310401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //A相组合无功1电能-费率1 - 电能量
    {{0x00320200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //B相组合无功1电能 - 电能量
    {{0x00320201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //B相组合无功1电能-费率1 - 电能量
    {{0x00320400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //B相组合无功1电能 - 高精度电能量
    {{0x00320401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //B相组合无功1电能-费率1 - 电能量
    {{0x00330200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //C相组合无功1电能 - 电能量
    {{0x00330201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //C相组合无功1电能-费率1 - 电能量
    {{0x00330400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //C相组合无功1电能 - 高精度电能量
    {{0x00330401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //C相组合无功1电能-费率1 - 电能量
    {{0x00400200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //组合无功2电能 - 电能量
    {{0x00400201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //组合无功2电能-费率1 - 电能量
    {{0x00400400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //组合无功2电能 - 高精度电能量
    {{0x00400401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //组合无功2电能-费率1 - 电能量
    {{0x00410200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //A相组合无功2电能 - 电能量
    {{0x00410201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //A相组合无功2电能-费率1 - 电能量
    {{0x00410400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //A相组合无功2电能 - 高精度电能量
    {{0x00410401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //A相组合无功2电能-费率1 - 电能量
    {{0x00420200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //B相组合无功2电能 - 电能量
    {{0x00420201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //B相组合无功2电能-费率1 - 电能量
    {{0x00420400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //B相组合无功2电能 - 高精度电能量
    {{0x00420401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //B相组合无功2电能-费率1 - 电能量
    {{0x00430200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //C相组合无功2电能 - 电能量
    {{0x00430201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //C相组合无功2电能-费率1 - 电能量
    {{0x00430400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //C相组合无功2电能 - 高精度电能量
    {{0x00430401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //C相组合无功2电能-费率1 - 电能量
    {{0x00500200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //第一象限无功电能 - 电能量
    {{0x00500201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //第一象限无功电能-费率1 - 电能量
    {{0x00500400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //第一象限无功电能 - 高精度电能量
    {{0x00500401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //第一象限无功电能-费率1 - 电能量
    {{0x00510200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相第一象限无功电能 - 电能量
    {{0x00510201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相第一象限无功电能-费率1 - 电能量
    {{0x00510400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相第一象限无功电能 - 高精度电能量
    {{0x00510401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相第一象限无功电能-费率1 - 电能量
    {{0x00520200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相第一象限无功电能 - 电能量
    {{0x00520201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相第一象限无功电能-费率1 - 电能量
    {{0x00520400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相第一象限无功电能 - 高精度电能量
    {{0x00520401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相第一象限无功电能-费率1 - 电能量
    {{0x00530200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相第一象限无功电能 - 电能量
    {{0x00530201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相第一象限无功电能-费率1 - 电能量
    {{0x00530400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相第一象限无功电能 - 高精度电能量
    {{0x00530401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相第一象限无功电能-费率1 - 电能量
    {{0x00600200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //第二象限无功电能 - 电能量
    {{0x00600201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //第二象限无功电能-费率1 - 电能量
    {{0x00600400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //第二象限无功电能 - 高精度电能量
    {{0x00600401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //第二象限无功电能-费率1 - 电能量
    {{0x00610200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相第二象限无功电能 - 电能量
    {{0x00610201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相第二象限无功电能-费率1 - 电能量
    {{0x00610400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相第二象限无功电能 - 高精度电能量
    {{0x00610401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相第二象限无功电能-费率1 - 电能量
    {{0x00620200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相第二象限无功电能 - 电能量
    {{0x00620201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相第二象限无功电能-费率1 - 电能量
    {{0x00620400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相第二象限无功电能 - 高精度电能量
    {{0x00620401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相第二象限无功电能-费率1 - 电能量
    {{0x00630200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相第二象限无功电能 - 电能量
    {{0x00630201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相第二象限无功电能-费率1 - 电能量
    {{0x00630400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相第二象限无功电能 - 高精度电能量
    {{0x00630401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相第二象限无功电能-费率1 - 电能量
    {{0x00700200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //第三象限无功电能 - 电能量
    {{0x00700201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //第三象限无功电能-费率1 - 电能量
    {{0x00700400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //第三象限无功电能 - 高精度电能量
    {{0x00700401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //第三象限无功电能-费率1 - 电能量
    {{0x00710200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相第三象限无功电能 - 电能量
    {{0x00710201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相第三象限无功电能-费率1 - 电能量
    {{0x00710400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相第三象限无功电能 - 高精度电能量
    {{0x00710401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相第三象限无功电能-费率1 - 电能量
    {{0x00720200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相第三象限无功电能 - 电能量
    {{0x00720201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相第三象限无功电能-费率1 - 电能量
    {{0x00720400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相第三象限无功电能 - 高精度电能量
    {{0x00720401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相第三象限无功电能-费率1 - 电能量
    {{0x00730200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相第三象限无功电能 - 电能量
    {{0x00730201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相第三象限无功电能-费率1 - 电能量
    {{0x00730400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相第三象限无功电能 - 高精度电能量
    {{0x00730401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相第三象限无功电能-费率1 - 电能量
    {{0x00800200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //第四象限无功电能 - 电能量
    {{0x00800201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_ENERGY_T},          //第四象限无功电能-费率1 - 电能量
    {{0x00800400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //第四象限无功电能 - 高精度电能量
    {{0x00800401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //第四象限无功电能-费率1 - 电能量
    {{0x00810200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相第四象限无功电能 - 电能量
    {{0x00810201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相第四象限无功电能-费率1 - 电能量
    {{0x00810400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相第四象限无功电能 - 高精度电能量
    {{0x00810401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相第四象限无功电能-费率1 - 电能量
    {{0x00820200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相第四象限无功电能 - 电能量
    {{0x00820201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相第四象限无功电能-费率1 - 电能量
    {{0x00820400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相第四象限无功电能 - 高精度电能量
    {{0x00820401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相第四象限无功电能-费率1 - 电能量
    {{0x00830200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相第四象限无功电能 - 电能量
    {{0x00830201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相第四象限无功电能-费率1 - 电能量
    {{0x00830400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相第四象限无功电能 - 高精度电能量
    {{0x00830401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相第四象限无功电能-费率1 - 电能量
    {{0x00900200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //正向视在电能 - 电能量
    {{0x00900201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //正向视在电能-费率1 - 电能量
    {{0x00900400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //正向视在电能 - 高精度电能量
    {{0x00900401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //正向视在电能-费率1 - 电能量
    {{0x00910200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相正向视在电能 - 电能量
    {{0x00910201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相正向视在电能-费率1 - 电能量
    {{0x00910400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相正向视在电能 - 高精度电能量
    {{0x00910401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相正向视在电能-费率1 - 电能量
    {{0x00920200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相正向视在电能 - 电能量
    {{0x00920201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相正向视在电能-费率1 - 电能量
    {{0x00920400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相正向视在电能 - 高精度电能量
    {{0x00920401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相正向视在电能-费率1 - 电能量
    {{0x00930200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相正向视在电能 - 电能量
    {{0x00930201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相正向视在电能-费率1 - 电能量
    {{0x00930400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相正向视在电能 - 高精度电能量
    {{0x00930401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相正向视在电能-费率1 - 电能量
    {{0x00A00200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //反向视在电能 - 电能量
    {{0x00A00201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //反向视在电能-费率1 - 电能量
    {{0x00A00400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //反向视在电能 - 高精度电能量
    {{0x00A00401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //反向视在电能-费率1 - 电能量
    {{0x00A10200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相反向视在电能 - 电能量
    {{0x00A10201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相反向视在电能-费率1 - 电能量
    {{0x00A10400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相反向视在电能 - 高精度电能量
    {{0x00A10401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相反向视在电能-费率1 - 电能量
    {{0x00A20200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相反向视在电能 - 电能量
    {{0x00A20201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相反向视在电能-费率1 - 电能量
    {{0x00A20400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相反向视在电能 - 高精度电能量
    {{0x00A20401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相反向视在电能-费率1 - 电能量
    {{0x00A30200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相反向视在电能 - 电能量
    {{0x00A30201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相反向视在电能-费率1 - 电能量
    {{0x00A30400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相反向视在电能 - 高精度电能量
    {{0x00A30401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相反向视在电能-费率1 - 电能量
    {{0x01100200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //正向有功基波总电能 - 电能量
    {{0x01100201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //正向有功基波总电能-费率1 - 电能量
    {{0x01100400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //正向有功基波总电能 - 高精度电能量
    {{0x01100401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //正向有功基波总电能-费率1 - 电能量
    {{0x01110200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相正向有功基波总电能 - 电能量
    {{0x01110201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相正向有功基波总电能-费率1 - 电能量
    {{0x01110400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相正向有功基波总电能 - 高精度电能量
    {{0x01110401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相正向有功基波总电能-费率1 - 电能量
    {{0x01120200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相正向有功基波总电能 - 电能量
    {{0x01120201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相正向有功基波总电能-费率1 - 电能量
    {{0x01120400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相正向有功基波总电能 - 高精度电能量
    {{0x01120401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相正向有功基波总电能-费率1 - 电能量
    {{0x01130200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相正向有功基波总电能 - 电能量
    {{0x01130201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相正向有功基波总电能-费率1 - 电能量
    {{0x01130400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相正向有功基波总电能
    {{0x01130401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相正向有功基波总电能-费率1 - 电能量
    {{0x01200200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //反向有功基波总电能 - 电能量
    {{0x01200201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //反向有功基波总电能-费率1 - 电能量
    {{0x01200400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //反向有功基波总电能 - 高精度电能量
    {{0x01200401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //反向有功基波总电能-费率1 - 电能量
    {{0x01210200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相反向有功基波总电能 - 电能量
    {{0x01210201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相反向有功基波总电能-费率1 - 电能量
    {{0x01210400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相反向有功基波总电能 - 高精度电能量
    {{0x01210401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相反向有功基波总电能-费率1 - 电能量
    {{0x01220200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相反向有功基波总电能 - 电能量
    {{0x01220201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相反向有功基波总电能-费率1 - 电能量
    {{0x01220400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相反向有功基波总电能 - 高精度电能量
    {{0x01220401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相反向有功基波总电能-费率1 - 电能量
    {{0x01230200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相反向有功基波总电能 - 电能量
    {{0x01230201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相反向有功基波总电能-费率1 - 电能量
    {{0x01230400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相反向有功基波总电能 - 高精度电能量
    {{0x01230401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相反向有功基波总电能-费率1 - 电能量
    {{0x02100200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //正向有功谐波总电能 - 电能量
    {{0x02100201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //正向有功谐波总电能-费率1 - 电能量
    {{0x02100400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //正向有功谐波总电能 - 高精度电能量
    {{0x02100401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //正向有功谐波总电能-费率1 - 电能量
    {{0x02110200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相正向有功谐波总电能 - 电能量
    {{0x02110201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相正向有功谐波总电能-费率1 - 电能量
    {{0x02110400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相正向有功谐波总电能 - 高精度电能量
    {{0x02110401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相正向有功谐波总电能-费率1 - 电能量
    {{0x02120200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相正向有功谐波总电能 - 电能量
    {{0x02120201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相正向有功谐波总电能-费率1 - 电能量
    {{0x02120400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相正向有功谐波总电能 - 高精度电能量
    {{0x02120401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相正向有功谐波总电能-费率1 - 电能量
    {{0x02130200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相正向有功谐波总电能 - 电能量
    {{0x02130201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相正向有功谐波总电能-费率1 - 电能量
    {{0x02130400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相正向有功谐波总电能 - 高精度电能量
    {{0x02130401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相正向有功谐波总电能-费率1 - 电能量
    {{0x02200200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //反向有功谐波总电能 - 电能量
    {{0x02200201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //反向有功谐波总电能-费率1 - 电能量
    {{0x02200400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //反向有功谐波总电能 - 高精度电能量
    {{0x02200401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //反向有功谐波总电能-费率1 - 电能量
    {{0x02210200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相反向有功谐波总电能 - 电能量
    {{0x02210201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相反向有功谐波总电能-费率1 - 电能量
    {{0x02210400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相反向有功谐波总电能 - 高精度电能量
    {{0x02210401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相反向有功谐波总电能-费率1 - 电能量
    {{0x02220200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相反向有功谐波总电能 - 电能量
    {{0x02220201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相反向有功谐波总电能-费率1 - 电能量
    {{0x02220400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相反向有功谐波总电能 - 高精度电能量
    {{0x02220401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相反向有功谐波总电能-费率1 - 电能量
    {{0x02230200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相反向有功谐波总电能 - 电能量
    {{0x02230201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相反向有功谐波总电能-费率1 - 电能量
    {{0x02230400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相反向有功谐波总电能 - 高精度电能量
    {{0x02230401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相反向有功谐波总电能-费率1 - 电能量
    {{0x03000200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //铜损有功总电能补偿量 - 电能量
    {{0x03000201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //铜损有功总电能补偿量-费率1 - 电能量
    {{0x03000400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //铜损有功总电能补偿量 - 高精度电能量
    {{0x03000401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //铜损有功总电能补偿量-费率1 - 电能量
    {{0x03010200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相铜损有功总电能补偿量 - 电能量
    {{0x03010201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相铜损有功总电能补偿量-费率1 - 电能量
    {{0x03010400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相铜损有功总电能补偿量 - 高精度电能量
    {{0x03010401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相铜损有功总电能补偿量-费率1 - 电能量
    {{0x03020200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相铜损有功总电能补偿量 - 电能量
    {{0x03020201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相铜损有功总电能补偿量-费率1 - 电能量
    {{0x03020400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相铜损有功总电能补偿量 - 高精度电能量
    {{0x03020401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相铜损有功总电能补偿量-费率1 - 电能量
    {{0x03030200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相铜损有功总电能补偿量 - 电能量
    {{0x03030201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相铜损有功总电能补偿量-费率1 - 电能量
    {{0x03030400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相铜损有功总电能补偿量 - 高精度电能量
    {{0x03030401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相铜损有功总电能补偿量-费率1 - 电能量
    {{0x04000200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //铁损有功总电能补偿量 - 电能量
    {{0x04000201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //铁损有功总电能补偿量-费率1 - 电能量
    {{0x04000400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //铁损有功总电能补偿量 - 高精度电能量
    {{0x04000401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //铁损有功总电能补偿量-费率1 - 电能量
    {{0x04010200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相铁损有功总电能补偿量 - 电能量
    {{0x04010201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相铁损有功总电能补偿量-费率1 - 电能量
    {{0x04010400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相铁损有功总电能补偿量 - 高精度电能量
    {{0x04010401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相铁损有功总电能补偿量-费率1 - 电能量
    {{0x04020200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相铁损有功总电能补偿量 - 电能量
    {{0x04020201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相铁损有功总电能补偿量-费率1 - 电能量
    {{0x04020400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相铁损有功总电能补偿量 - 高精度电能量
    {{0x04020401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相铁损有功总电能补偿量-费率1 - 电能量
    {{0x04030200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相铁损有功总电能补偿量 - 电能量
    {{0x04030201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相铁损有功总电能补偿量-费率1 - 电能量
    {{0x04030400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相铁损有功总电能补偿量 - 高精度电能量
    {{0x04030401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相铁损有功总电能补偿量-费率1 - 电能量
    {{0x05000200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //关联总电能 - 电能量
    {{0x05000201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //关联总电能-费率1 - 电能量
    {{0x05000400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //关联总电能 - 高精度电能量
    {{0x05000401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //关联总电能-费率1 - 电能量
    {{0x05010200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A相关联总电能 - 电能量
    {{0x05010201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A相关联总电能-费率1 - 电能量
    {{0x05010400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A相关联总电能 - 高精度电能量
    {{0x05010401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A相关联总电能-费率1 - 电能量
    {{0x05020200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B相关联总电能 - 电能量
    {{0x05020201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B相关联总电能-费率1 - 电能量
    {{0x05020400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B相关联总电能 - 高精度电能量
    {{0x05020401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B相关联总电能-费率1 - 电能量
    {{0x05030200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C相关联总电能 - 电能量
    {{0x05030201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C相关联总电能-费率1 - 电能量
    {{0x05030400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C相关联总电能 - 高精度电能量
    {{0x05030401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C相关联总电能-费率1 - 电能量
};               
const uint32 oadEnergyInfoListLen = sizeof(oadEnergyInfoList)/sizeof(oadEnergyInfoList[0]);

/* 最大需量类OAD列表 OIA1 = 0x01 */
const OAD_TAB_T oadDemandInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                         len                    eData*/
    {{0x10100200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //正向有功最大需量
    {{0x10100201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //正向有功最大需量-费率
    {{0x10110200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相正向有功最大需量
    {{0x10110201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相正向有功最大需量-费率
    {{0x10120200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相正向有功最大需量
    {{0x10120201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相正向有功最大需量-费率
    {{0x10130200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相正向有功最大需量
    {{0x10130201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相正向有功最大需量-费率
    {{0x10200200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //反向有功最大需量
    {{0x10200201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //反向有功最大需量-费率
    {{0x10210200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相反向有功最大需量
    {{0x10210201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相反向有功最大需量-费率
    {{0x10220200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相反向有功最大需量
    {{0x10220201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相反向有功最大需量-费率
    {{0x10230200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相反向有功最大需量
    {{0x10230201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相反向有功最大需量-费率
    {{0x10300200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //组合无功1最大需量
    {{0x10300201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //组合无功1最大需量-费率
    {{0x10310200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //A相组合无功1最大需量
    {{0x10310201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //A相组合无功1最大需量-费率
    {{0x10320200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //B相组合无功1最大需量
    {{0x10320201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //B相组合无功1最大需量-费率
    {{0x10330200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //C相组合无功1最大需量
    {{0x10330201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //C相组合无功1最大需量-费率
    {{0x10400200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //组合无功2最大需量
    {{0x10400201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //组合无功2最大需量-费率
    {{0x10410200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //A相组合无功2最大需量
    {{0x10410201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //A相组合无功2最大需量-费率
    {{0x10420200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //B相组合无功2最大需量
    {{0x10420201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //B相组合无功2最大需量-费率
    {{0x10430200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //C相组合无功2最大需量
    {{0x10430201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //C相组合无功2最大需量-费率
    {{0x10500200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //第一象限最大需量
    {{0x10500201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //第一象限最大需量-费率
    {{0x10510200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相第一象限最大需量
    {{0x10510201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相第一象限最大需量-费率
    {{0x10520200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相第一象限最大需量
    {{0x10520201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相第一象限最大需量-费率
    {{0x10530200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相第一象限最大需量
    {{0x10530201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相第一象限最大需量-费率
    {{0x10600200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //第二象限最大需量
    {{0x10600201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //第二象限最大需量-费率
    {{0x10610200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相第二象限最大需量
    {{0x10610201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相第二象限最大需量-费率
    {{0x10620200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相第二象限最大需量
    {{0x10620201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相第二象限最大需量-费率
    {{0x10630200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相第二象限最大需量
    {{0x10630201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相第二象限最大需量-费率
    {{0x10700200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //第三象限最大需量
    {{0x10700201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //第三象限最大需量-费率
    {{0x10710200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相第三象限最大需量
    {{0x10710201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相第三象限最大需量-费率
    {{0x10720200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相第三象限最大需量
    {{0x10720201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相第三象限最大需量-费率
    {{0x10730200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相第三象限最大需量
    {{0x10730201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相第三象限最大需量-费率
    {{0x10800200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //第四象限最大需量
    {{0x10800201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //第四象限最大需量-费率
    {{0x10810200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相第四象限最大需量
    {{0x10810201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相第四象限最大需量-费率
    {{0x10820200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相第四象限最大需量
    {{0x10820201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相第四象限最大需量-费率
    {{0x10830200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相第四象限最大需量
    {{0x10830201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相第四象限最大需量-费率
    {{0x10900200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //正向视在最大需量
    {{0x10900201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //正向视在最大需量-费率
    {{0x10910200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相正向视在最大需量
    {{0x10910201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相正向视在最大需量-费率
    {{0x10920200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相正向视在最大需量
    {{0x10920201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相正向视在最大需量-费率
    {{0x10930200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相正向视在最大需量
    {{0x10930201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相正向视在最大需量-费率
    {{0x10A00200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //反向视在最大需量
    {{0x10A00201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //反向视在最大需量-费率
    {{0x10A10200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A相反向视在最大需量
    {{0x10A10201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A相反向视在最大需量-费率
    {{0x10A20200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B相反向视在最大需量
    {{0x10A20201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B相反向视在最大需量-费率
    {{0x10A30200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C相反向视在最大需量
    {{0x10A30201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C相反向视在最大需量-费率
    {{0x11100200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内正向有功最大需量
    {{0x11100201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内正向有功最大需量-费率
    {{0x11110200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相正向有功最大需量
    {{0x11110201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相正向有功最大需量-费率
    {{0x11120200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相正向有功最大需量
    {{0x11120201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相正向有功最大需量-费率
    {{0x11130200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相正向有功最大需量
    {{0x11130201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相正向有功最大需量-费率
    {{0x11200200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内反向有功最大需量
    {{0x11200201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内反向有功最大需量-费率
    {{0x11210200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相反向有功最大需量
    {{0x11210201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相反向有功最大需量-费率
    {{0x11220200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相反向有功最大需量
    {{0x11220201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相反向有功最大需量-费率
    {{0x11230200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相反向有功最大需量
    {{0x11230201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相反向有功最大需量-费率
    {{0x11300200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内组合无功1最大需量
    {{0x11300201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内组合无功1最大需量-费率
    {{0x11310200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内A相组合无功1最大需量
    {{0x11310201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内A相组合无功1最大需量-费率
    {{0x11320200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内B相组合无功1最大需量
    {{0x11320201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内B相组合无功1最大需量-费率
    {{0x11330200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内C相组合无功1最大需量
    {{0x11330201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内C相组合无功1最大需量-费率
    {{0x11400200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内组合无功2最大需量
    {{0x11400201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内组合无功2最大需量-费率
    {{0x11410200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内A相组合无功2最大需量
    {{0x11410201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内A相组合无功2最大需量-费率
    {{0x11420200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内B相组合无功2最大需量
    {{0x11420201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内B相组合无功2最大需量-费率
    {{0x11430200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //冻结周期内C相组合无功2最大需量
    {{0x11430201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //冻结周期内C相组合无功2最大需量-费率
    {{0x11500200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内第一象限最大需量
    {{0x11500201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内第一象限最大需量-费率
    {{0x11510200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相第一象限最大需量
    {{0x11510201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相第一象限最大需量-费率
    {{0x11520200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相第一象限最大需量
    {{0x11520201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相第一象限最大需量-费率
    {{0x11530200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相第一象限最大需量
    {{0x11530201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相第一象限最大需量-费率
    {{0x11600200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内第二象限最大需量
    {{0x11600201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内第二象限最大需量-费率
    {{0x11610200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相第二象限最大需量
    {{0x11610201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相第二象限最大需量-费率
    {{0x11620200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相第二象限最大需量
    {{0x11620201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相第二象限最大需量-费率
    {{0x11630200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相第二象限最大需量
    {{0x11630201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相第二象限最大需量-费率
    {{0x11700200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内第三象限最大需量
    {{0x11700201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内第三象限最大需量-费率
    {{0x11710200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相第三象限最大需量
    {{0x11710201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相第三象限最大需量-费率
    {{0x11720200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相第三象限最大需量
    {{0x11720201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相第三象限最大需量-费率
    {{0x11730200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相第三象限最大需量
    {{0x11730201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相第三象限最大需量-费率
    {{0x11800200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内第四象限最大需量
    {{0x11800201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内第四象限最大需量-费率
    {{0x11810200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相第四象限最大需量
    {{0x11810201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相第四象限最大需量-费率
    {{0x11820200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相第四象限最大需量
    {{0x11820201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相第四象限最大需量-费率
    {{0x11830200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相第四象限最大需量
    {{0x11830201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相第四象限最大需量-费率
    {{0x11900200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内正向视在最大需量
    {{0x11900201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内正向视在最大需量-费率
    {{0x11910200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相正向视在最大需量
    {{0x11910201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相正向视在最大需量-费率
    {{0x11920200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相正向视在最大需量
    {{0x11920201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相正向视在最大需量-费率
    {{0x11930200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相正向视在最大需量
    {{0x11930201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相正向视在最大需量-费率
    {{0x11A00200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内反向视在最大需量
    {{0x11A00201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内反向视在最大需量-费率
    {{0x11A10200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内A相反向视在最大需量
    {{0x11A10201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内A相反向视在最大需量-费率
    {{0x11A20200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内B相反向视在最大需量
    {{0x11A20201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内B相反向视在最大需量-费率
    {{0x11A30200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //冻结周期内C相反向视在最大需量
    {{0x11A30201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //冻结周期内C相反向视在最大需量-费率
};
const uint32 oadDemandInfoListLen = sizeof(oadDemandInfoList)/sizeof(oadDemandInfoList[0]);

/* 变量类OAD列表 OIA1 = 0x02 */
const OAD_TAB_T oadVariateInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                 len*/
    {{0x20000200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},  
    {{0x20000400}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(uint16),          E_LONG_UNSIGNED},
    {{0x20000600}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(uint16),          E_LONG_UNSIGNED},  //电压  OOP_WORD3V_T分相变量都用这个结构体？？？
//    {{0x20000201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A相电压
//    {{0x20000202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B相电压
//    {{0x20000203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C相电压
    {{0x20010200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_INT3V_T),     E_OOP_INT3V_T},                 //电流
//    {{0x20010201}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //A相电流
//    {{0x20010202}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //B相电流
//    {{0x20010203}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //C相电流
    {{0x20010400}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //N相电流
    {{0x20010500}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //剩余电流
    {{0x20010600}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //剩余电流
    {{0x20010800}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //剩余电流

    {{0x20020200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                 //电压相角  
//    {{0x20020201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A相电压相角
//    {{0x20020202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B相电压相角
//    {{0x20020203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C相电压相角
    {{0x20030200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                 //电压电流相角  
//    {{0x20030201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A相电压电流相角
//    {{0x20030202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B相电压电流相角
//    {{0x20030203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C相电压电流相角
    {{0x20040200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //有功功率
    {{0x20050200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //无功功率
    {{0x20060200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //视在功率
    {{0x20070200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //一分钟平均有功功率
    {{0x20080200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //一分钟平均无功功率
    {{0x20090200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //一分钟平均视在功率
    {{0x200A0200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_INT4V_T),     E_OOP_LONG4V_T},                          //功率因数
    {{0x200B0200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                          //电压波形失真度
//    {{0x200B0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //A相电压波形失真度
//    {{0x200B0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //B相电压波形失真度
//    {{0x200B0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //C相电压波形失真度
    {{0x200C0200}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                          //电流波形失真度
//    {{0x200C0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //A相电流波形失真度
//    {{0x200C0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //B相电流波形失真度
//    {{0x200C0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //C相电流波形失真度
    {{0x200D0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //A相电压谐波含有量（总及2…n次）
    {{0x200D0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //B相电压谐波含有量（总及2…n次）
    {{0x200D0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //C相电压谐波含有量（总及2…n次）
    {{0x200D0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //电压谐波含有量最高谐波次数
    {{0x200E0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //A相电流谐波含有量（总及2…n次）
    {{0x200E0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //B相电流谐波含有量（总及2…n次）
    {{0x200E0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //C相电流谐波含有量（总及2…n次）
    {{0x200E0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //电流谐波含有量最高谐波次数
    {{0x200F0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //电网频率
    {{0x200F0A00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //A相频率
    {{0x200F0B00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //B相频率
    {{0x200F0C00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //C相频率
    {{0x20100200}, 6,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //表内温度
    {{0x20100400}, 6,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_LONG3V_T),   E_OOP_LONG3V_T},                          //表内温度

    {{0x20110200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //时钟电池电压
    {{0x20120200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //停电抄表电池电压
    {{0x20130200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //时钟电池工作时间
    {{0x20140200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_METWORD_T),   E_OOP_METWORD_T},                 //电能表运行状态字
    {{0x20140400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //计量模组系统状态字
    {{0x20140500}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //采样系统状态字
    {{0x20150200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //电能表跟随上报状态字
    {{0x20150400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //电能表跟随上报模式字
    {{0x20150500}, 6,       0x02,   0x00, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                           //上报方式
    {{0x20170200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //当前有功需量
    {{0x20180200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //当前无功需量
    {{0x20190200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //当前视在需量
    {{0x201A0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //当前电价
    {{0x201B0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //当前费率电价
    {{0x201C0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //当前阶梯电价
    {{0x201E0200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //事件发生时间
    {{0x20200200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //事件结束时间
    {{0x20210200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //数据冻结时间
    {{0x20220200}, 8,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //事件记录序号 
    {{0x20230200}, 8,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //冻结记录序号 
    {{0x20240200}, 8,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTSOURCE_T), E_OOP_EVTSOURCE_T},               //事件发生源     
    {{0x20260200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //电压不平衡率
    {{0x20270200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //电流不平衡率
    {{0x20280200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //负载率
    {{0x20290200}, 6,       0x02,   0x00, 1,      DT_ARRAY,                4*sizeof(uint32),        E_DOUBLE_LONG_UNSIGNED},          //安时值????找不到对应结构体
    {{0x202A0200}, 8,       0x02,   0x00, 0,      DT_TSA,                  sizeof(OOP_TSA_T),       E_TSA},  
    {{0x202C0200}, 8,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},  
    {{0x202D0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //（当前）透支金额
    {{0x202E0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //累计购电金额
    {{0x20310200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //月度用电量
    {{0x20320200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //阶梯结算用电量
    {{0x20330200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //A相电压谐波含有量（2…n次）
    {{0x20330300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //B相电压谐波含有量（2…n次）
    {{0x20330400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //C相电压谐波含有量（2…n次）
    {{0x20340200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //A相电流谐波含有量（2…n次）
    {{0x20340300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //B相电流谐波含有量（2…n次）
    {{0x20340400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //C相电流谐波含有量（2…n次）
    {{0x20350200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20360200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20370200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20380200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  


    {{0x20400200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T),  E_OOP_BITSTR16_T},                //控制命令执行状态字
    {{0x20410200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T),  E_OOP_BITSTR16_T},                //控制命令错误状态字
    {{0x20420200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //外置蓝牙负荷开关状态字
    {{0x20420400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                //电能质量模组状态字
    {{0x20500200}, 6,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOOPSTAT_T),  E_OOP_LOOPSTAT_T},                //电流回路状态
    {{0x21000200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //分钟区间统计 - 统计周期单位为分钟 - 统计结果表
    {{0x21000300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //分钟区间统计 - 统计周期单位为分钟 - 关联对象属性表
    {{0x21010200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //小时区间统计 - 统计周期单位为小时 - 统计结果表
    {{0x21010300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //小时区间统计 - 统计周期单位为小时 - 关联对象属性表
    {{0x21020200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //日区间统计 - 统计周期单位为日 - 统计结果表
    {{0x21020300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //日区间统计 - 统计周期单位为日 - 关联对象属性表
    {{0x21030200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //月区间统计 - 统计周期单位为月 - 统计结果表
    {{0x21030300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //月区间统计 - 统计周期单位为月 - 关联对象属性表
    {{0x21040200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //年区间统计 - 统计周期单位为年 - 统计结果表
    {{0x21040300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //年区间统计 - 统计周期单位为年 - 关联对象属性表
    {{0x21100200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //分钟平均  - 统计结果表
    {{0x21100300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //分钟平均  - 关联对象属性表
    {{0x21110200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //小时平均  - 统计结果表
    {{0x21110300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //小时平均  - 关联对象属性表
    {{0x21120200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //日平均  - 统计结果表
    {{0x21120300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //日平均  - 关联对象属性表
    {{0x21130200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //月平均  - 统计结果表
    {{0x21130300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //月平均  - 关联对象属性表
    {{0x21140200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //年平均  - 统计结果表
    {{0x21140300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //年平均  - 关联对象属性表
    {{0x21200200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //分钟极值  - 统计结果表
    {{0x21200300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //分钟极值  - 关联对象属性表
    {{0x21210200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //小时极值  - 统计结果表
    {{0x21210300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //小时极值  - 关联对象属性表
    {{0x21220200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //日极值  - 统计结果表
    {{0x21220300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //日极值  - 关联对象属性表
    {{0x21230200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //月极值  - 统计结果表
    {{0x21230300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //月极值  - 关联对象属性表
    {{0x21240200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //年极值  - 统计结果表
    {{0x21240300}, 16,       0x02,   0x01, 0,      DT_ARRAY,               sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //年极值  - 关联对象属性表
    {{0x21310200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //当月A相电压合格率
    {{0x21320200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //当月B相电压合格率
    {{0x21330200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //当月B相电压合格率
    {{0x21400200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T),    E_OOP_DEMAND_T},                  //日最大有功功率及发生时间
    {{0x21410200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T),    E_OOP_DEMAND_T},                  //月最大有功功率及发生时间
    {{0x22000200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},                  //通信流量
    {{0x22030200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},                   //供电时间
    {{0x22040200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_WORD2_T),     E_OOP_WORD2_T},                   //复位次数
    {{0x23010200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组1 - 总加组配置表
    {{0x23010300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 总加有功功率
    {{0x23010400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 总加无功功率
    {{0x23010500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 总加滑差时间内平均有功功率
    {{0x23010600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 总加滑差时间内平均无功功率
    {{0x23010700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组1 - 总加日有功电量
    {{0x23010800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组1 - 总加日无功电量
    {{0x23010900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组1 - 总加日有功电量
    {{0x23010A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组1 - 总加日无功电量
    {{0x23010B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 总加剩余电量(费)
    {{0x23010C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组1 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23010D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组1 - 当总加组滑差时间周期
    {{0x23010E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组1 - 总加组功控轮次配置
    {{0x23010F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组1 - 总加组电控轮次配置
    {{0x23011000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组1 - 总加组控制设置状态
    {{0x23011100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组1 - 总加组当前控制状态
    {{0x23011200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组1 - 换算及单位
    {{0x23020200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组2 - 总加组配置表
    {{0x23020300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 总加有功功率
    {{0x23020400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 总加无功功率
    {{0x23020500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 总加滑差时间内平均有功功率
    {{0x23020600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 总加滑差时间内平均无功功率
    {{0x23020700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组2 - 总加日有功电量
    {{0x23020800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组2 - 总加日无功电量
    {{0x23020900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组2 - 总加日有功电量
    {{0x23020A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组2 - 总加日无功电量
    {{0x23020B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 总加剩余电量(费)
    {{0x23020C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组2 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23020D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组2 - 当总加组滑差时间周期
    {{0x23020E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组2 - 总加组功控轮次配置
    {{0x23020F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组2 - 总加组电控轮次配置
    {{0x23021000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组2 - 总加组控制设置状态
    {{0x23021100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组2 - 总加组当前控制状态
    {{0x23021200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组2 - 换算及单位
    {{0x23030200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组3 - 总加组配置表
    {{0x23030300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 总加有功功率
    {{0x23030400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 总加无功功率
    {{0x23030500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 总加滑差时间内平均有功功率
    {{0x23030600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 总加滑差时间内平均无功功率
    {{0x23030700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组3 - 总加日有功电量
    {{0x23030800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组3 - 总加日无功电量
    {{0x23030900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组3 - 总加日有功电量
    {{0x23030A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组3 - 总加日无功电量
    {{0x23030B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 总加剩余电量(费)
    {{0x23030C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组3 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23030D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组3 - 当总加组滑差时间周期
    {{0x23030E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组3 - 总加组功控轮次配置
    {{0x23030F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组3 - 总加组电控轮次配置
    {{0x23031000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组3 - 总加组控制设置状态
    {{0x23031100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组3 - 总加组当前控制状态
    {{0x23031200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组3 - 换算及单位
    {{0x23040200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组4 - 总加组配置表
    {{0x23040300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 总加有功功率
    {{0x23040400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 总加无功功率
    {{0x23040500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 总加滑差时间内平均有功功率
    {{0x23040600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 总加滑差时间内平均无功功率
    {{0x23040700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组4 - 总加日有功电量
    {{0x23040800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组4 - 总加日无功电量
    {{0x23040900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组4 - 总加日有功电量
    {{0x23040A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组4 - 总加日无功电量
    {{0x23040B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 总加剩余电量(费)
    {{0x23040C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组4 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23040D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组4 - 当总加组滑差时间周期
    {{0x23040E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组4 - 总加组功控轮次配置
    {{0x23040F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组4 - 总加组电控轮次配置
    {{0x23041000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组4 - 总加组控制设置状态
    {{0x23041100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组4 - 总加组当前控制状态
    {{0x23041200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组4 - 换算及单位
    {{0x23050200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组5 - 总加组配置表
    {{0x23050300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 总加有功功率
    {{0x23050400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 总加无功功率
    {{0x23050500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 总加滑差时间内平均有功功率
    {{0x23050600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 总加滑差时间内平均无功功率
    {{0x23050700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组5 - 总加日有功电量
    {{0x23050800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组5 - 总加日无功电量
    {{0x23050900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组5 - 总加日有功电量
    {{0x23050A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组5 - 总加日无功电量
    {{0x23050B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 总加剩余电量(费)
    {{0x23050C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组5 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23050D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组5 - 当总加组滑差时间周期
    {{0x23050E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组5 - 总加组功控轮次配置
    {{0x23050F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组5 - 总加组电控轮次配置
    {{0x23051000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组5 - 总加组控制设置状态
    {{0x23051100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组5 - 总加组当前控制状态
    {{0x23051200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组5 - 换算及单位
    {{0x23060200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组6 - 总加组配置表
    {{0x23060300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 总加有功功率
    {{0x23060400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 总加无功功率
    {{0x23060500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 总加滑差时间内平均有功功率
    {{0x23060600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 总加滑差时间内平均无功功率
    {{0x23060700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组6 - 总加日有功电量
    {{0x23060800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组6 - 总加日无功电量
    {{0x23060900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组6 - 总加日有功电量
    {{0x23060A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组6 - 总加日无功电量
    {{0x23060B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 总加剩余电量(费)
    {{0x23060C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组6 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23060D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组6 - 当总加组滑差时间周期
    {{0x23060E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组6 - 总加组功控轮次配置
    {{0x23060F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组6 - 总加组电控轮次配置
    {{0x23061000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组6 - 总加组控制设置状态
    {{0x23061100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组6 - 总加组当前控制状态
    {{0x23061200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组6 - 换算及单位
    {{0x23070200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组7 - 总加组配置表
    {{0x23070300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 总加有功功率
    {{0x23070400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 总加无功功率
    {{0x23070500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 总加滑差时间内平均有功功率
    {{0x23070600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 总加滑差时间内平均无功功率
    {{0x23070700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组7 - 总加日有功电量
    {{0x23070800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组7 - 总加日无功电量
    {{0x23070900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组7 - 总加日有功电量
    {{0x23070A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组7 - 总加日无功电量
    {{0x23070B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 总加剩余电量(费)
    {{0x23070C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组7 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23070D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组7 - 当总加组滑差时间周期
    {{0x23070E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组7 - 总加组功控轮次配置
    {{0x23070F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组7 - 总加组电控轮次配置
    {{0x23071000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组7 - 总加组控制设置状态
    {{0x23071100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组7 - 总加组当前控制状态
    {{0x23071200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组7 - 换算及单位
    {{0x23080200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //总加组8 - 总加组配置表
    {{0x23080300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 总加有功功率
    {{0x23080400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 总加无功功率
    {{0x23080500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 总加滑差时间内平均有功功率
    {{0x23080600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 总加滑差时间内平均无功功率
    {{0x23080700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组8 - 总加日有功电量
    {{0x23080800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组8 - 总加日无功电量
    {{0x23080900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组8 - 总加日有功电量
    {{0x23080A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //总加组8 - 总加日无功电量
    {{0x23080B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 总加剩余电量(费)
    {{0x23080C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //总加组8 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23080D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //总加组8 - 当总加组滑差时间周期
    {{0x23080E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组8 - 总加组功控轮次配置
    {{0x23080F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //总加组8 - 总加组电控轮次配置
    {{0x23081000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //总加组8 - 总加组控制设置状态
    {{0x23081100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //总加组8 - 总加组当前控制状态
    {{0x23081200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //总加组8 - 换算及单位
    {{0x24010200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量1 - 通信地址
    {{0x24010300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量1 - 互感器倍率
    {{0x24010400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量1 - 脉冲配置
    {{0x24010500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量1 - 有功功率
    {{0x24010600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量1 - 无功功率
    {{0x24010700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当日正向有功电量
    {{0x24010800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当月正向有功电量
    {{0x24010900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当日反向有功电量
    {{0x24010A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当月反向有功电量
    {{0x24010B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当日正向无功电量
    {{0x24010C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当月正向无功电量
    {{0x24010D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当日反向无功电量
    {{0x24010E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 当月反向无功电量
    {{0x24010F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 正向有功电能示值
    {{0x24011000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 反向有功电能示值
    {{0x24011100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 正向无功电能示值
    {{0x24011200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量1 - 反向无功电能示值
    {{0x24011300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量1 - 单位及换算
    {{0x24011C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量1 - 脉冲配置
    {{0x24020200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量2 - 通信地址
    {{0x24020300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量2 - 互感器倍率
    {{0x24020400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量2 - 脉冲配置
    {{0x24020500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量2 - 有功功率
    {{0x24020600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量2 - 无功功率
    {{0x24020700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当日正向有功电量
    {{0x24020800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当月正向有功电量
    {{0x24020900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当日反向有功电量
    {{0x24020A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当月反向有功电量
    {{0x24020B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当日正向无功电量
    {{0x24020C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当月正向无功电量
    {{0x24020D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当日反向无功电量
    {{0x24020E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 当月反向无功电量
    {{0x24020F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 正向有功电能示值
    {{0x24021000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 反向有功电能示值
    {{0x24021100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 正向无功电能示值
    {{0x24021200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量2 - 反向无功电能示值
    {{0x24021300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量2 - 单位及换算
    {{0x24021C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量2 - 脉冲配置
    {{0x24030200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量3 - 通信地址
    {{0x24030300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量3 - 互感器倍率
    {{0x24030400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量3 - 脉冲配置
    {{0x24030500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量3 - 有功功率
    {{0x24030600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量3 - 无功功率
    {{0x24030700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当日正向有功电量
    {{0x24030800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当月正向有功电量
    {{0x24030900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当日反向有功电量
    {{0x24030A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当月反向有功电量
    {{0x24030B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当日正向无功电量
    {{0x24030C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当月正向无功电量
    {{0x24030D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当日反向无功电量
    {{0x24030E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 当月反向无功电量
    {{0x24030F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 正向有功电能示值
    {{0x24031000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 反向有功电能示值
    {{0x24031100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 正向无功电能示值
    {{0x24031200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量3 - 反向无功电能示值
    {{0x24031300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量3 - 单位及换算
    {{0x24031C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量3 - 脉冲配置
    {{0x24040200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量4 - 通信地址
    {{0x24040300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量4 - 互感器倍率
    {{0x24040400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量4 - 脉冲配置
    {{0x24040500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量4 - 有功功率
    {{0x24040600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量4 - 无功功率
    {{0x24040700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当日正向有功电量
    {{0x24040800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当月正向有功电量
    {{0x24040900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当日反向有功电量
    {{0x24040A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当月反向有功电量
    {{0x24040B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当日正向无功电量
    {{0x24040C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当月正向无功电量
    {{0x24040D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当日反向无功电量
    {{0x24040E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 当月反向无功电量
    {{0x24040F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 正向有功电能示值
    {{0x24041000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 反向有功电能示值
    {{0x24041100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 正向无功电能示值
    {{0x24041200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量4 - 反向无功电能示值
    {{0x24041300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量4 - 单位及换算
    {{0x24041C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量4 - 脉冲配置
    {{0x24050200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量5 - 通信地址
    {{0x24050300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量5 - 互感器倍率
    {{0x24050400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量5 - 脉冲配置
    {{0x24050500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量5 - 有功功率
    {{0x24050600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量5 - 无功功率
    {{0x24050700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当日正向有功电量
    {{0x24050800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当月正向有功电量
    {{0x24050900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当日反向有功电量
    {{0x24050A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当月反向有功电量
    {{0x24050B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当日正向无功电量
    {{0x24050C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当月正向无功电量
    {{0x24050D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当日反向无功电量
    {{0x24050E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 当月反向无功电量
    {{0x24050F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 正向有功电能示值
    {{0x24051000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 反向有功电能示值
    {{0x24051100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 正向无功电能示值
    {{0x24051200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量5 - 反向无功电能示值
    {{0x24051300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量5 - 单位及换算
    {{0x24051C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量5 - 脉冲配置
    {{0x24060200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量6 - 通信地址
    {{0x24060300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量6 - 互感器倍率
    {{0x24060400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量6 - 脉冲配置
    {{0x24060500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量6 - 有功功率
    {{0x24060600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量6 - 无功功率
    {{0x24060700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当日正向有功电量
    {{0x24060800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当月正向有功电量
    {{0x24060900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当日反向有功电量
    {{0x24060A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当月反向有功电量
    {{0x24060B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当日正向无功电量
    {{0x24060C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当月正向无功电量
    {{0x24060D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当日反向无功电量
    {{0x24060E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 当月反向无功电量
    {{0x24060F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 正向有功电能示值
    {{0x24061000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 反向有功电能示值
    {{0x24061100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 正向无功电能示值
    {{0x24061200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量6 - 反向无功电能示值
    {{0x24061300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量6 - 单位及换算
    {{0x24061C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量6 - 脉冲配置
    {{0x24070200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量7 - 通信地址
    {{0x24070300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量7 - 互感器倍率
    {{0x24070400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量7 - 脉冲配置
    {{0x24070500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量7 - 有功功率
    {{0x24070600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量7 - 无功功率
    {{0x24070700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当日正向有功电量
    {{0x24070800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当月正向有功电量
    {{0x24070900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当日反向有功电量
    {{0x24070A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当月反向有功电量
    {{0x24070B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当日正向无功电量
    {{0x24070C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当月正向无功电量
    {{0x24070D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当日反向无功电量
    {{0x24070E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 当月反向无功电量
    {{0x24070F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 正向有功电能示值
    {{0x24071000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 反向有功电能示值
    {{0x24071100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 正向无功电能示值
    {{0x24071200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量7 - 反向无功电能示值
    {{0x24071300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量7 - 单位及换算
    {{0x24071C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量7 - 脉冲配置
    {{0x24080200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //脉冲计量8 - 通信地址
    {{0x24080300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //脉冲计量8 - 互感器倍率
    {{0x24080400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //脉冲计量8 - 脉冲配置
    {{0x24080500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量8 - 有功功率
    {{0x24080600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //脉冲计量8 - 无功功率
    {{0x24080700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当日正向有功电量
    {{0x24080800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当月正向有功电量
    {{0x24080900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当日反向有功电量
    {{0x24080A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当月反向有功电量
    {{0x24080B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当日正向无功电量
    {{0x24080C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当月正向无功电量
    {{0x24080D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当日反向无功电量
    {{0x24080E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 当月反向无功电量
    {{0x24080F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 正向有功电能示值
    {{0x24081000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 反向有功电能示值
    {{0x24081100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 正向无功电能示值
    {{0x24081200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //脉冲计量8 - 反向无功电能示值
    {{0x24081300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //脉冲计量8 - 单位及换算
    {{0x24081C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //脉冲计量1 - 脉冲配置
    {{0x25000200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //累计水（热）流量
    {{0x25010200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //累计气流量
    {{0x25020200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //累计热量
    {{0x25030200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //热功率
    {{0x25040200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //累计工作时间
    {{0x25050200}, 6,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(Water_Temperature_T),     E_Water_Temperature_T},                   //水温
    {{0x25060200}, 6,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ST_T),        E_OOP_ST_T},                      //（仪表）状态ST
    {{0x26000200}, 6,       0x02,   0x06, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //温度
    {{0x26010200}, 6,       0x02,   0x06, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                  //湿度
    {{0x20100400}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //端子温度
    {{0x20430200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //端子温度分钟变化量
    {{0x20440200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //误差自监测
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    {{0x28110200}, 6,       0x02,   0x08, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //烟感状态
    {{0x29000200}, 6,       0x02,   0x09, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //传感器温度
    {{0x29040200}, 6,       0x02,   0x09, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //水浸状态
    {{0x29050200}, 6,       0x02,   0x09, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //水浸状态
#endif
#ifdef AREA_SHANDONG
    {{0x2E200200}, 6,       0x02,   0x0E, 0,      DT_STRUCTURE,            sizeof(OOP_HUMITURE_2E20_T), E_OOP_HUMITURE_2E20_T},       //温湿度
    {{0x2E210200}, 6,       0x02,   0x0E, 0,      DT_STRUCTURE,            sizeof(OOP_LOCK_STATE_2E21_T), E_OOP_LOCK_STATE_2E21_T},   //门锁状态
#endif
};          
const uint32 oadVariateInfoListLen = sizeof(oadVariateInfoList)/sizeof(oadVariateInfoList[0]);

/* 事件类OAD列表 OIA1 = 0x03 */
const OAD_TAB_T oadEvnetInfoList[] =
{
    /*OAD        class     A1      A2   rsv       dataType                   len*/
    {{0x30000200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表失压事件 - 关联对象属性表
    {{0x30000300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表失压事件 - 当前记录数
    {{0x30000400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表失压事件 - 最大记录数
    {{0x30000500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3000_T),    E_OOP_EVTCFG_3000_T},            //电能表失压事件 - 配置参数
    {{0x30000600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失压事件 - 事件记录单元1
    {{0x30000700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失压事件 - 事件记录单元2
    {{0x30000800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失压事件 - 事件记录单元3
    {{0x30000900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失压事件 - 事件记录单元4
    {{0x30000A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表失压事件 - 当前值记录表
    {{0x30000B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表失压事件 - 上报标识
    {{0x30000C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表失压事件 - 有效标识
    {{0x30000D00}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOSTVSTAT_T),      E_OOP_LOSTVSTAT_T},              //电能表失压事件 - 失压统计
    {{0x30000E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表失压事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30010200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表欠压事件 - 关联对象属性表
    {{0x30010300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表欠压事件 - 当前记录数
    {{0x30010400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表欠压事件 - 最大记录数
    {{0x30010500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T),      E_OOP_EVTCFG_WD_T},              //电能表欠压事件 - 配置参数
    {{0x30010600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表欠压事件 - 事件记录单元1
    {{0x30010700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表欠压事件 - 事件记录单元2
    {{0x30010800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表欠压事件 - 事件记录单元3
    {{0x30010900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表欠压事件 - 事件记录单元4
    {{0x30010A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表欠压事件 - 当前值记录表
    {{0x30010B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表欠压事件 - 上报标识
    {{0x30010C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表欠压事件 - 有效标识
    {{0x30010E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表欠压事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30020200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表过压事件 - 关联对象属性表
    {{0x30020300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表过压事件 - 当前记录数
    {{0x30020400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表过压事件 - 最大记录数
    {{0x30020500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T),      E_OOP_EVTCFG_WD_T},              //电能表过压事件 - 配置参数
    {{0x30020600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过压事件 - 事件记录单元1
    {{0x30020700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过压事件 - 事件记录单元2
    {{0x30020800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过压事件 - 事件记录单元3
    {{0x30020900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过压事件 - 事件记录单元4
    {{0x30020A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表过压事件 - 当前值记录表
    {{0x30020B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表过压事件 - 上报标识
    {{0x30020C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表过压事件 - 有效标识
    {{0x30020E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表过压事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30030200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表断相事件 - 关联对象属性表
    {{0x30030300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表断相事件 - 当前记录数
    {{0x30030400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表断相事件 - 最大记录数
    {{0x30030500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3003_T),    E_OOP_EVTCFG_3003_T},            //电能表断相事件 - 配置参数
    {{0x30030600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断相事件 - 事件记录单元1
    {{0x30030700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断相事件 - 事件记录单元2
    {{0x30030800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断相事件 - 事件记录单元3
    {{0x30030900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断相事件 - 事件记录单元4
    {{0x30030A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表断相事件 - 当前值记录表
    {{0x30030B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表断相事件 - 上报标识
    {{0x30030C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表断相事件 - 有效标识
    {{0x30030E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30040200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表失流事件 - 关联对象属性表
    {{0x30040300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表失流事件 - 当前记录数
    {{0x30040400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表失流事件 - 最大记录数
    {{0x30040500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3004_T),    E_OOP_EVTCFG_3004_T},            //电能表失流事件 - 配置参数
    {{0x30040600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失流事件 - 事件记录单元1
    {{0x30040700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失流事件 - 事件记录单元2
    {{0x30040800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失流事件 - 事件记录单元3
    {{0x30040900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表失流事件 - 事件记录单元4
    {{0x30040A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表失流事件 - 当前值记录表
    {{0x30040B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表失流事件 - 上报标识
    {{0x30040C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表失流事件 - 有效标识
    {{0x30040E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表失流事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30050200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表过流事件 - 关联对象属性表
    {{0x30050300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表过流事件 - 当前记录数
    {{0x30050400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表过流事件 - 最大记录数
    {{0x30050500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //电能表过流事件 - 配置参数
    {{0x30050600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过流事件 - 事件记录单元1
    {{0x30050700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过流事件 - 事件记录单元2
    {{0x30050800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过流事件 - 事件记录单元3
    {{0x30050900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过流事件 - 事件记录单元4
    {{0x30050A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表过流事件 - 当前值记录表
    {{0x30050B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                     //电能表过流事件 - 上报标识
    {{0x30050C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表过流事件 - 有效标识
    {{0x30050E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表过流事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30060200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表断流事件 - 关联对象属性表
    {{0x30060300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表断流事件 - 当前记录数
    {{0x30060400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表断流事件 - 最大记录数
    {{0x30060500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3006_T),    E_OOP_EVTCFG_3006_T},            //电能表断流事件 - 配置参数
    {{0x30060600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断流事件 - 事件记录单元1
    {{0x30060700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断流事件 - 事件记录单元2
    {{0x30060800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断流事件 - 事件记录单元3
    {{0x30060900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表断流事件 - 事件记录单元4
    {{0x30060A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表断流事件 - 当前值记录表
    {{0x30060B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表断流事件 - 上报标识
    {{0x30060C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表断流事件 - 有效标识
    {{0x30060E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表断流事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30070200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表功率反向事件 - 关联对象属性表
    {{0x30070300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表功率反向事件 - 当前记录数
    {{0x30070400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表功率反向事件 - 最大记录数
    {{0x30070500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //电能表功率反向事件 - 配置参数
    {{0x30070600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表功率反向事件 - 事件记录单元1
    {{0x30070700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表功率反向事件 - 事件记录单元2
    {{0x30070800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表功率反向事件 - 事件记录单元3
    {{0x30070900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表功率反向事件 - 事件记录单元4
    {{0x30070A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表功率反向事件 - 当前值记录表
    {{0x30070B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表功率反向事件 - 上报标识
    {{0x30070C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表功率反向事件 - 有效标识
    {{0x30070E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表功率反向事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30080200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表过载事件 - 关联对象属性表
    {{0x30080300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表过载事件 - 当前记录数
    {{0x30080400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表过载事件 - 最大记录数
    {{0x30080500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //电能表过载事件 - 配置参数
    {{0x30080600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过载事件 - 事件记录单元1
    {{0x30080700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过载事件 - 事件记录单元2
    {{0x30080800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过载事件 - 事件记录单元3
    {{0x30080900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //电能表过载事件 - 事件记录单元4
    {{0x30080A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //电能表过载事件 - 当前值记录表
    {{0x30080B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表过载事件 - 上报标识
    {{0x30080C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表过载事件 - 有效标识
    {{0x30080E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表过载事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x30090200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表正向有功需量超限事件 - 事件记录表
    {{0x30090300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表正向有功需量超限事件 - 关联对象属性表
    {{0x30090400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表正向有功需量超限事件 - 当前记录数
    {{0x30090500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表正向有功需量超限事件 - 最大记录数
    {{0x30090600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //电能表正向有功需量超限事件 - 配置参数
    {{0x30090700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表正向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30090800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表正向有功需量超限事件 - 上报标识
    {{0x30090900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表正向有功需量超限事件 - 有效标识
    {{0x30090A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表正向有功需量超限事件 - 时间状态记录表
       
    {{0x300A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表反向有功需量超限事件 - 事件记录表
    {{0x300A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表反向有功需量超限事件 - 关联对象属性表
    {{0x300A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表反向有功需量超限事件 - 当前记录数
    {{0x300A0500}, 7,       0x03,   0x00, 1,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表反向有功需量超限事件 - 最大记录数
    {{0x300A0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //电能表反向有功需量超限事件 - 配置参数
    {{0x300A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表反向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表反向有功需量超限事件 - 上报标识
    {{0x300A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表反向有功需量超限事件 - 有效标识
    {{0x300A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表反向有功需量超限事件 - 时间状态记录表
       
    {{0x300B0200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表无功需量超限事件 - 关联对象属性表
    {{0x300B0300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //电能表无功需量超限事件 - 当前记录数
    {{0x300B0400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表无功需量超限事件 - 最大记录数
    {{0x300B0500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //电能表无功需量超限事件 - 配置参数
    {{0x300B0600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表无功需量超限事件 - 事件记录单元1
    {{0x300B0700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表无功需量超限事件 - 事件记录单元2
    {{0x300B0800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表无功需量超限事件 - 事件记录单元3
    {{0x300B0900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //电能表无功需量超限事件 - 事件记录单元4
    {{0x300B0A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32),             E_DOUBLE_LONG_UNSIGNED},         //电能表无功需量超限事件 - 当前值记录表
    {{0x300B0B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表无功需量超限事件 - 上报标识
    {{0x300B0C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表无功需量超限事件 - 有效标识
    {{0x300B0E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //电能表无功需量超限事件 - 时间状态记录表,????是不是用这个结构体，待确认
       
    {{0x300C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表功率因数超下限事件 - 事件记录表
    {{0x300C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表功率因数超下限事件 - 关联对象属性表
    {{0x300C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表功率因数超下限事件 - 当前记录数
    {{0x300C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表功率因数超下限事件 - 最大记录数
    {{0x300C0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //电能表功率因数超下限事件 - 配置参数
    {{0x300C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表功率因数超下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表功率因数超下限事件 - 上报标识
    {{0x300C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表功率因数超下限事件 - 有效标识
    {{0x300C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表功率因数超下限事件 - 时间状态记录表
       
    {{0x300D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表全失压事件 - 事件记录表????
    {{0x300D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表全失压事件 - 关联对象属性表
    {{0x300D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表全失压事件 - 当前记录数
    {{0x300D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表全失压事件 - 最大记录数
    {{0x300D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表全失压事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表全失压事件 - 上报标识
    {{0x300D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表全失压事件 - 有效标识
    {{0x300D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表全失压事件 - 时间状态记录表
       
    {{0x300E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表辅助电源掉电事件 - 事件记录表
    {{0x300E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表辅助电源掉电事件 - 关联对象属性表
    {{0x300E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表辅助电源掉电事件 - 当前记录数
    {{0x300E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表辅助电源掉电事件 - 最大记录数
    {{0x300E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表辅助电源掉电事件 - 配置参数
    {{0x300E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表辅助电源掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表辅助电源掉电事件 - 上报标识
    {{0x300E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表辅助电源掉电事件 - 有效标识
    {{0x300E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表辅助电源掉电事件 - 时间状态记录表
       
    {{0x300F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电压逆相序事件 - 事件记录表
    {{0x300F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电压逆相序事件 - 关联对象属性表
    {{0x300F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电压逆相序事件 - 当前记录数
    {{0x300F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电压逆相序事件 - 最大记录数
    {{0x300F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表电压逆相序事件 - 配置参数
    {{0x300F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电压逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电压逆相序事件 - 上报标识
    {{0x300F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电压逆相序事件 - 有效标识
    {{0x300F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电压逆相序事件 - 时间状态记录表
       
    {{0x30100200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电流逆相序事件 - 事件记录表
    {{0x30100300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电流逆相序事件 - 关联对象属性表
    {{0x30100400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流逆相序事件 - 当前记录数
    {{0x30100500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流逆相序事件 - 最大记录数
    {{0x30100600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表电流逆相序事件 - 配置参数
    {{0x30100700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电流逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30100800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流逆相序事件 - 上报标识
    {{0x30100900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流逆相序事件 - 有效标识
    {{0x30100A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电流逆相序事件 - 时间状态记录表
    {{0x30110B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //掉电事件上报标识   
    {{0x30110200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表掉电事件 - 事件记录表
    {{0x30110300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表掉电事件 - 关联对象属性表
    {{0x30110400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表掉电事件 - 当前记录数
    {{0x30110500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表掉电事件 - 最大记录数
    {{0x30110600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表掉电事件 - 配置参数
    {{0x30110700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30110800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表掉电事件 - 上报标识
    {{0x30110900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表掉电事件 - 有效标识
    {{0x30110A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表掉电事件 - 时间状态记录表
       
    {{0x30120200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T),     E_OOP_PROGRAMLOG_T},             //电能表编程事件 - 事件记录表
    {{0x30120300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表编程事件 - 关联对象属性表
    {{0x30120400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表编程事件 - 当前记录数
    {{0x30120500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表编程事件 - 最大记录数
    {{0x30120700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30120800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表编程事件 - 上报标识
    {{0x30120900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表编程事件 - 有效标识
    {{0x30120A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表编程事件 - 时间状态记录表
       
    {{0x30130200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表清零事件 - 事件记录表
    {{0x30130300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表清零事件 - 关联对象属性表
    {{0x30130400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表清零事件 - 当前记录数
    {{0x30130500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表清零事件 - 最大记录数
    {{0x30130700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30130800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表清零事件 - 上报标识
    {{0x30130900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表清零事件 - 有效标识
    {{0x30130A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表清零事件 - 时间状态记录表
       
    {{0x30140200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表需量清零事件 - 事件记录表
    {{0x30140300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表需量清零事件 - 关联对象属性表
    {{0x30140400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表需量清零事件 - 当前记录数
    {{0x30140500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表需量清零事件 - 最大记录数
    {{0x30140700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表需量清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30140800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表需量清零事件 - 上报标识
    {{0x30140900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表需量清零事件 - 有效标识
    {{0x30140A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表需量清零事件 - 时间状态记录表
       
    {{0x30150200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENTRESETLOG_T),  E_OOP_EVENTRESETLOG_T},          //电能表事件清零事件 - 事件记录表
    {{0x30150300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表事件清零事件 - 关联对象属性表
    {{0x30150400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表事件清零事件 - 当前记录数
    {{0x30150500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表事件清零事件 - 最大记录数
    {{0x30150700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表事件清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30150800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表事件清零事件 - 上报标识
    {{0x30150900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表事件清零事件 - 有效标识
    {{0x30150A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表事件清零事件 - 时间状态记录表
       
    {{0x30160200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表校时事件 - 事件记录表
    {{0x30160300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表校时事件 - 关联对象属性表
    {{0x30160400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表校时事件 - 当前记录数
    {{0x30160500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表校时事件 - 最大记录数
    {{0x30160700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表校时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30160800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表校时事件 - 上报标识
    {{0x30160900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表校时事件 - 有效标识
    {{0x30160A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表校时事件 - 时间状态记录表
       
    {{0x30170200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGPERIODLOG_T),  E_OOP_PROGPERIODLOG_T},          //电能表时段表编程事件 - 事件记录表
    {{0x30170300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表时段表编程事件 - 关联对象属性表
    {{0x30170400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时段表编程事件 - 当前记录数
    {{0x30170500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时段表编程事件 - 最大记录数
    {{0x30170700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表时段表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30170800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时段表编程事件 - 上报标识
    {{0x30170900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时段表编程事件 - 有效标识
    {{0x30170A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表时段表编程事件 - 时间状态记录表
       
    {{0x30180200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表时区表编程事件 - 事件记录表
    {{0x30180300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表时区表编程事件 - 关联对象属性表
    {{0x30180400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时区表编程事件 - 当前记录数
    {{0x30180500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时区表编程事件 - 最大记录数
    {{0x30180700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表时区表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30180800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时区表编程事件 - 上报标识
    {{0x30180900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时区表编程事件 - 有效标识
    {{0x30180A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表时区表编程事件 - 时间状态记录表
       
    {{0x30190200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表周休日编程事件 - 事件记录表
    {{0x30190300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表周休日编程事件 - 关联对象属性表
    {{0x30190400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表周休日编程事件 - 当前记录数
    {{0x30190500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表周休日编程事件 - 最大记录数
    {{0x30190700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表周休日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30190800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表周休日编程事件 - 上报标识
    {{0x30190900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表周休日编程事件 - 有效标识
    {{0x30190A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表周休日编程事件 - 时间状态记录表
       
    {{0x301A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表结算日编程事件 - 事件记录表
    {{0x301A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表结算日编程事件 - 关联对象属性表
    {{0x301A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表结算日编程事件 - 当前记录数
    {{0x301A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表结算日编程事件 - 最大记录数
    {{0x301A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表结算日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表结算日编程事件 - 上报标识
    {{0x301A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表结算日编程事件 - 有效标识
    {{0x301A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表结算日编程事件 - 时间状态记录表
    {{0x301B0B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //开盖事件上报标识   
    {{0x301B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表开盖事件 - 事件记录表
    {{0x301B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表开盖事件 - 关联对象属性表
    {{0x301B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表开盖事件 - 当前记录数
    {{0x301B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表开盖事件 - 最大记录数
    {{0x301B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表开盖事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表开盖事件 - 上报标识
    {{0x301B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表开盖事件 - 有效标识
    {{0x301B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表开盖事件 - 时间状态记录表
       
    {{0x301C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表开端钮盒事件 - 事件记录表
    {{0x301C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表开端钮盒事件 - 关联对象属性表
    {{0x301C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表开端钮盒事件 - 当前记录数
    {{0x301C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表开端钮盒事件 - 最大记录数
    {{0x301C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表开端钮盒事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表开端钮盒事件 - 上报标识
    {{0x301C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表开端钮盒事件 - 有效标识
    {{0x301C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表开端钮盒事件 - 时间状态记录表
       
    {{0x301D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电压不平衡事件 - 事件记录表
    {{0x301D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电压不平衡事件 - 关联对象属性表
    {{0x301D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电压不平衡事件 - 当前记录数
    {{0x301D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电压不平衡事件 - 最大记录数
    {{0x301D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //电能表电压不平衡事件 - 配置参数
    {{0x301D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电压不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电压不平衡事件 - 上报标识
    {{0x301D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电压不平衡事件 - 有效标识
    {{0x301D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电压不平衡事件 - 时间状态记录表
       
    {{0x301E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电流不平衡事件 - 事件记录表
    {{0x301E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电流不平衡事件 - 关联对象属性表
    {{0x301E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流不平衡事件 - 当前记录数
    {{0x301E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流不平衡事件 - 最大记录数
    {{0x301E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //电能表电流不平衡事件 - 配置参数
    {{0x301E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电流不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流不平衡事件 - 上报标识
    {{0x301E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流不平衡事件 - 有效标识
    {{0x301E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电流不平衡事件 - 时间状态记录表
       
    {{0x301F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表跳闸事件 - 事件记录表
    {{0x301F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表跳闸事件 - 关联对象属性表
    {{0x301F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表跳闸事件 - 当前记录数
    {{0x301F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表跳闸事件 - 最大记录数
    {{0x301F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表跳闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表跳闸事件 - 上报标识
    {{0x301F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表跳闸事件 - 有效标识
    {{0x301F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表跳闸事件 - 时间状态记录表
       
    {{0x30200200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表合闸事件 - 事件记录表
    {{0x30200300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表合闸事件 - 关联对象属性表
    {{0x30200400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表合闸事件 - 当前记录数
    {{0x30200500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表合闸事件 - 最大记录数
    {{0x30200700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表合闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30200800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表合闸事件 - 上报标识
    {{0x30200900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表合闸事件 - 有效标识
    {{0x30200A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表合闸事件 - 时间状态记录表
       
    {{0x30210200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGHOLIDAYLOG_T), E_OOP_PROGHOLIDAYLOG_T},         //电能表节假日编程事件 - 事件记录表
    {{0x30210300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表节假日编程事件 - 关联对象属性表
    {{0x30210400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表节假日编程事件 - 当前记录数
    {{0x30210500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表节假日编程事件 - 最大记录数
    {{0x30210700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表节假日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30210800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表节假日编程事件 - 上报标识
    {{0x30210900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表节假日编程事件 - 有效标识
    {{0x30210A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表节假日编程事件 - 时间状态记录表
       
    {{0x30220200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表有功组合方式编程事件 - 事件记录表
    {{0x30220300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表有功组合方式编程事件 - 关联对象属性表
    {{0x30220400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表有功组合方式编程事件 - 当前记录数
    {{0x30220500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表有功组合方式编程事件 - 最大记录数
    {{0x30220700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表有功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30220800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表有功组合方式编程事件 - 上报标识
    {{0x30220900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表有功组合方式编程事件 - 有效标识
    {{0x30220A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表有功组合方式编程事件 - 时间状态记录表
       
    {{0x30230200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表无功组合方式编程事件 - 事件记录表
    {{0x30230300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表无功组合方式编程事件 - 关联对象属性表
    {{0x30230400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表无功组合方式编程事件 - 当前记录数
    {{0x30230500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表无功组合方式编程事件 - 最大记录数
    {{0x30230700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表无功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30230800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表无功组合方式编程事件 - 上报标识
    {{0x30230900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表无功组合方式编程事件 - 有效标识
    {{0x30230A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表无功组合方式编程事件 - 时间状态记录表
       
    {{0x30240200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表费率参数表编程事件 - 事件记录表
    {{0x30240300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表费率参数表编程事件 - 关联对象属性表
    {{0x30240400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表费率参数表编程事件 - 当前记录数
    {{0x30240500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表费率参数表编程事件 - 最大记录数
    {{0x30240700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表费率参数表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30240800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表费率参数表编程事件 - 上报标识
    {{0x30240900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表费率参数表编程事件 - 有效标识
    {{0x30240A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表费率参数表编程事件 - 时间状态记录表
       
    {{0x30250200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表阶梯表编程事件 - 事件记录表
    {{0x30250300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表阶梯表编程事件 - 关联对象属性表
    {{0x30250400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表阶梯表编程事件 - 当前记录数
    {{0x30250500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表阶梯表编程事件 - 最大记录数
    {{0x30250700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表阶梯表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30250800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表阶梯表编程事件 - 上报标识
    {{0x30250900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表阶梯表编程事件 - 有效标识
    {{0x30250A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表阶梯表编程事件 - 时间状态记录表
       
    {{0x30260200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表密钥更新事件 - 事件记录表
    {{0x30260300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表密钥更新事件 - 关联对象属性表
    {{0x30260400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表密钥更新事件 - 当前记录数
    {{0x30260500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表密钥更新事件 - 最大记录数
    {{0x30260700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表密钥更新事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30260800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表密钥更新事件 - 上报标识
    {{0x30260900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表密钥更新事件 - 有效标识
    {{0x30260A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表密钥更新事件 - 时间状态记录表
       
    {{0x30270200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCARD_T),   E_OOP_ABNORMALCARD_T},           //电能表异常插卡事件 - 事件记录表
    {{0x30270300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表异常插卡事件 - 关联对象属性表
    {{0x30270400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表异常插卡事件 - 当前记录数
    {{0x30270500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表异常插卡事件 - 最大记录数
    {{0x30270700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表异常插卡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30270800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表异常插卡事件 - 上报标识
    {{0x30270900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表异常插卡事件 - 有效标识
    {{0x30270A00}, 7,       0x03,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),               E_DOUBLE_LONG_UNSIGNED},         //电能表异常插卡事件 - 非法插卡总次数
       
    {{0x30280200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表购电记录 - 事件记录表
    {{0x30280300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表购电记录 - 关联对象属性表
    {{0x30280400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表购电记录 - 当前记录数
    {{0x30280500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表购电记录 - 最大记录数
    {{0x30280700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表购电记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30280800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表购电记录 - 上报标识
    {{0x30280900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表购电记录 - 有效标识
    {{0x30280A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表购电记录 - 时间状态记录表
       
    {{0x30290200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_REBATELOG_T),      E_OOP_REBATELOG_T},              //电能表退费记录 - 事件记录表
    {{0x30290300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表退费记录 - 关联对象属性表
    {{0x30290400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表退费记录 - 当前记录数
    {{0x30290500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表退费记录 - 最大记录数
    {{0x30290700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表退费记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30290800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表退费记录 - 上报标识
    {{0x30290900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表退费记录 - 有效标识
    {{0x30290A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表退费记录 - 时间状态记录表
       
    {{0x302A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表恒定磁场干扰事件 - 事件记录表
    {{0x302A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表恒定磁场干扰事件 - 关联对象属性表
    {{0x302A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表恒定磁场干扰事件 - 当前记录数
    {{0x302A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表恒定磁场干扰事件 - 最大记录数
    {{0x302A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表恒定磁场干扰事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表恒定磁场干扰事件 - 上报标识
    {{0x302A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表恒定磁场干扰事件 - 有效标识
    {{0x302A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表恒定磁场干扰事件 - 时间状态记录表
       
    {{0x302B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表负荷开关误动作事件 - 事件记录表
    {{0x302B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表负荷开关误动作事件 - 关联对象属性表
    {{0x302B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表负荷开关误动作事件 - 当前记录数
    {{0x302B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表负荷开关误动作事件 - 最大记录数
    {{0x302B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表负荷开关误动作事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表负荷开关误动作事件 - 上报标识
    {{0x302B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表负荷开关误动作事件 - 有效标识
    {{0x302B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表负荷开关误动作事件 - 时间状态记录表
       
    {{0x302C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电源异常事件 - 事件记录表
    {{0x302C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电源异常事件 - 关联对象属性表
    {{0x302C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电源异常事件 - 当前记录数
    {{0x302C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电源异常事件 - 最大记录数
    {{0x302C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电源异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电源异常事件 - 上报标识
    {{0x302C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电源异常事件 - 有效标识
    {{0x302C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电源异常事件 - 时间状态记录表
       
    {{0x302D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表电流严重不平衡事件 - 事件记录表
    {{0x302D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表电流严重不平衡事件 - 关联对象属性表
    {{0x302D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流严重不平衡事件 - 当前记录数
    {{0x302D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表电流严重不平衡事件 - 最大记录数
    {{0x302D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //电能表电流严重不平衡事件 - 配置参数
    {{0x302D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表电流严重不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流严重不平衡事件 - 上报标识
    {{0x302D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表电流严重不平衡事件 - 有效标识
    {{0x302D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表电流严重不平衡事件 - 时间状态记录表
       
    {{0x302E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表时钟故障事件 - 事件记录表
    {{0x302E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表时钟故障事件 - 关联对象属性表
    {{0x302E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时钟故障事件 - 当前记录数
    {{0x302E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时钟故障事件 - 最大记录数
    {{0x302E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表时钟故障事件 - 配置参数
    {{0x302E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表时钟故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时钟故障事件 - 上报标识
    {{0x302E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时钟故障事件 - 有效标识
    {{0x302E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表时钟故障事件 - 时间状态记录表
    {{0x302E0B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //时钟故障事件上报标识   
    {{0x302F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表计量芯片故障事件 - 事件记录表
    {{0x302F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表计量芯片故障事件 - 关联对象属性表
    {{0x302F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表计量芯片故障事件 - 当前记录数
    {{0x302F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表计量芯片故障事件 - 最大记录数
    {{0x302F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //电能表计量芯片故障事件 - 配置参数
    {{0x302F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表计量芯片故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表计量芯片故障事件 - 上报标识
    {{0x302F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表计量芯片故障事件 - 有效标识
    {{0x302F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表计量芯片故障事件 - 时间状态记录表
       
    {{0x30300200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MODULELOG_T),      E_OOP_MODULELOG_T},              //通信模块变更事件 - 事件记录表
    {{0x30300300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //通信模块变更事件 - 关联对象属性表
    {{0x30300400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //通信模块变更事件 - 当前记录数
    {{0x30300500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //通信模块变更事件 - 最大记录数
    {{0x30300600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //通信模块变更事件 - 配置参数
    {{0x30300700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //通信模块变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30300800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //通信模块变更事件 - 上报标识
    {{0x30300900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //通信模块变更事件 - 有效标识
    {{0x30300A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //通信模块变更事件 - 时间状态记录表

    {{0x30310200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //管理模组插拔事件 - 事件记录表
    {{0x30310300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //管理模组插拔事件 - 关联对象属性表
    {{0x30310400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //管理模组插拔事件 - 当前记录数
    {{0x30310500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //管理模组插拔事件 - 最大记录数
    {{0x30310700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //管理模组插拔事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30310800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //管理模组插拔事件 - 上报标识
    {{0x30310900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //管理模组插拔事件 - 有效标识
    {{0x30310A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //管理模组插拔事件 - 时间状态记录表

    {{0x30400600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOAD_I0_EVECFG_T), E_OOP_LOAD_I0_EVECFG_T},
    
    {{0x30320200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //端子座过热报警事件 - 事件记录表
    {{0x30320300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //端子座过热报警事件 - 关联对象属性表
    {{0x30320400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //端子座过热报警事件 - 当前记录数
    {{0x30320500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //端子座过热报警事件 - 最大记录数
    {{0x30320700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //端子座过热报警事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30320800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //端子座过热报警事件 - 上报标识
    {{0x30320900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //端子座过热报警事件 - 有效标识
    {{0x30320A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //端子座过热报警事件 - 时间状态记录表
    {{0x303B0500}, 7,       0x03,   0x01, 0,   DT_STRUCTURE,               sizeof(OOP_CFG_LU_T),            E_OOP_CFG_LU_T},
    {{0x31000200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端初始化事件 - 事件记录表
    {{0x31000300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端初始化事件 - 关联对象属性表
    {{0x31000400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端初始化事件 - 当前记录数
    {{0x31000500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端初始化事件 - 最大记录数
    {{0x31000700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端初始化事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31000800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端初始化事件 - 上报标识
    {{0x31000900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端初始化事件 - 有效标识
    {{0x31000A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端初始化事件 - 时间状态记录表
       
    {{0x31010200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端版本变更事件 - 事件记录表
    {{0x31010300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端版本变更事件 - 关联对象属性表
    {{0x31010400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端版本变更事件 - 当前记录数
    {{0x31010500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端版本变更事件 - 最大记录数
    {{0x31010700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端版本变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31010800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端版本变更事件 - 上报标识
    {{0x31010900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端版本变更事件 - 有效标识
    {{0x31010A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端版本变更事件 - 时间状态记录表
       
    {{0x31040200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端状态量变位事件 - 事件记录表
    {{0x31040300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端状态量变位事件 - 关联对象属性表
    {{0x31040400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端状态量变位事件 - 当前记录数
    {{0x31040500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端状态量变位事件 - 最大记录数
    {{0x31040700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端状态量变位事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31040800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端状态量变位事件 - 上报标识
    {{0x31040900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端状态量变位事件 - 有效标识
    {{0x31040A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端状态量变位事件 - 时间状态记录表
       
    {{0x31050200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_CLOCKFLYLOG_T),    E_OOP_CLOCKFLYLOG_T},            //电能表时钟超差事件 - 事件记录表
    {{0x31050300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表时钟超差事件 - 关联对象属性表
    {{0x31050400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时钟超差事件 - 当前记录数
    {{0x31050500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表时钟超差事件 - 最大记录数
    {{0x31050600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3105_T),    E_OOP_EVTCFG_3105_T},            //电能表时钟超差事件 - 配置参数
    {{0x31050700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表时钟超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31050800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时钟超差事件 - 上报标识
    {{0x31050900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表时钟超差事件 - 有效标识
    {{0x31050A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表时钟超差事件 - 时间状态记录表
       
    {{0x31060200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_POWERLOG_T),       E_OOP_POWERLOG_T},               //终端停/上电事件 - 事件记录表
    {{0x31060300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端停/上电事件 - 关联对象属性表
    {{0x31060400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端停/上电事件 - 当前记录数
    {{0x31060500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端停/上电事件 - 最大记录数
    {{0x31060600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_POWERONCUT_T),     E_OOP_POWERONCUT_T},             //终端停/上电事件 - 配置参数
    {{0x31060700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端停/上电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31060800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端停/上电事件 - 上报标识
    {{0x31060900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端停/上电事件 - 有效标识
    {{0x31060A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端停/上电事件 - 时间状态记录表
    {{0x31060B00}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //终端停/上电事件 - 关联采集任务号
       
    {{0x31070200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端直流模拟量越上限事件 - 事件记录表
    {{0x31070300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端直流模拟量越上限事件 - 关联对象属性表
    {{0x31070400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端直流模拟量越上限事件 - 当前记录数
    {{0x31070500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端直流模拟量越上限事件 - 最大记录数
    {{0x31070600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T),     E_OOP_EVTCFG_INT_T},             //终端直流模拟量越上限事件 - 配置参数
    {{0x31070700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端直流模拟量越上限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31070800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端直流模拟量越上限事件 - 上报标识
    {{0x31070900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端直流模拟量越上限事件 - 有效标识
    {{0x31070A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端直流模拟量越上限事件 - 时间状态记录表
       
    {{0x31080200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端直流模拟量越下限事件 - 事件记录表
    {{0x31080300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端直流模拟量越下限事件 - 关联对象属性表
    {{0x31080400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端直流模拟量越下限事件 - 当前记录数
    {{0x31080500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端直流模拟量越下限事件 - 最大记录数
    {{0x31080600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T),     E_OOP_EVTCFG_INT_T},             //终端直流模拟量越下限事件 - 配置参数
    {{0x31080700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端直流模拟量越下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31080800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端直流模拟量越下限事件 - 上报标识
    {{0x31080900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端直流模拟量越下限事件 - 有效标识
    {{0x31080A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端直流模拟量越下限事件 - 时间状态记录表
       
    {{0x31090200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端消息认证错误事件 - 事件记录表
    {{0x31090300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端消息认证错误事件 - 关联对象属性表
    {{0x31090400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端消息认证错误事件 - 当前记录数
    {{0x31090500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端消息认证错误事件 - 最大记录数
    {{0x31090700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端消息认证错误事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31090800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端消息认证错误事件 - 上报标识
    {{0x31090900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端消息认证错误事件 - 有效标识
    {{0x31090A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端消息认证错误事件 - 时间状态记录表
       
    {{0x310A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //设备故障记录 - 事件记录表
    {{0x310A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //设备故障记录 - 关联对象属性表
    {{0x310A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //设备故障记录 - 当前记录数
    {{0x310A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //设备故障记录 - 最大记录数
    {{0x310A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //设备故障记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //设备故障记录 - 上报标识
    {{0x310A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //设备故障记录 - 有效标识
    {{0x310A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //设备故障记录 - 时间状态记录表
       
    {{0x310B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表示度下降事件 - 事件记录表
    {{0x310B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表示度下降事件 - 关联对象属性表
    {{0x310B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表示度下降事件 - 当前记录数
    {{0x310B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表示度下降事件 - 最大记录数
    {{0x310B0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //电能表示度下降事件 - 配置参数
    {{0x310B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表示度下降事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表示度下降事件 - 上报标识
    {{0x310B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表示度下降事件 - 有效标识
    {{0x310B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表示度下降事件 - 时间状态记录表
       
    {{0x310C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能量超差事件 - 事件记录表
    {{0x310C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能量超差事件 - 关联对象属性表
    {{0x310C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能量超差事件 - 当前记录数
    {{0x310C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能量超差事件 - 最大记录数
    {{0x310C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T),    E_OOP_EVTCFG_310X_T},            //电能量超差事件 - 配置参数
    {{0x310C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能量超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能量超差事件 - 上报标识
    {{0x310C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能量超差事件 - 有效标识
    {{0x310C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能量超差事件 - 时间状态记录表
       
    {{0x310D0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表飞走事件 - 事件记录表
    {{0x310D0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表飞走事件 - 关联对象属性表
    {{0x310D0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表飞走事件 - 当前记录数
    {{0x310D0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表飞走事件 - 最大记录数
    {{0x310D0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T),    E_OOP_EVTCFG_310X_T},            //电能表飞走事件 - 配置参数
    {{0x310D0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表飞走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310D0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表飞走事件 - 上报标识
    {{0x310D0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表飞走事件 - 有效标识
    {{0x310D0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表飞走事件 - 时间状态记录表
       
    {{0x310E0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电能表停走事件 - 事件记录表
    {{0x310E0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表停走事件 - 关联对象属性表
    {{0x310E0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表停走事件 - 当前记录数
    {{0x310E0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表停走事件 - 最大记录数
    {{0x310E0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310E_T),    E_OOP_EVTCFG_310E_T},            //电能表停走事件 - 配置参数
    {{0x310E0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表停走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310E0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表停走事件 - 上报标识
    {{0x310E0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表停走事件 - 有效标识
    {{0x310E0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表停走事件 - 时间状态记录表
       
    {{0x310F0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端抄表失败事件 - 事件记录表
    {{0x310F0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端抄表失败事件 - 关联对象属性表
    {{0x310F0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端抄表失败事件 - 当前记录数
    {{0x310F0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端抄表失败事件 - 最大记录数
    {{0x310F0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310F_T),    E_OOP_EVTCFG_310F_T},            //终端抄表失败事件 - 配置参数
    {{0x310F0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端抄表失败事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310F0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端抄表失败事件 - 上报标识
    {{0x310F0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端抄表失败事件 - 有效标识
    {{0x310F0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端抄表失败事件 - 时间状态记录表
       
    {{0x31100200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //月通信流量超限事件 - 事件记录表
    {{0x31100300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //月通信流量超限事件 - 关联对象属性表
    {{0x31100400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //月通信流量超限事件 - 当前记录数
    {{0x31100500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //月通信流量超限事件 - 最大记录数
    {{0x31100600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3110_T),    E_OOP_EVTCFG_3110_T},            //月通信流量超限事件 - 配置参数
    {{0x31100700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //月通信流量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31100800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //月通信流量超限事件 - 上报标识
    {{0x31100900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //月通信流量超限事件 - 有效标识
    {{0x31100A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //月通信流量超限事件 - 时间状态记录表
       
    {{0x31110200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_SMLOG_T),          E_OOP_SMLOG_T},                  //发现未知电能表事件 - 事件记录表
    {{0x31110300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //发现未知电能表事件 - 关联对象属性表
    {{0x31110400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //发现未知电能表事件 - 当前记录数
    {{0x31110500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //发现未知电能表事件 - 最大记录数
    {{0x31110700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //发现未知电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31110800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //发现未知电能表事件 - 上报标识
    {{0x31110900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //发现未知电能表事件 - 有效标识
    {{0x31110A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //发现未知电能表事件 - 时间状态记录表
       
    {{0x31120200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_UNMETERLOG_T),     E_OOP_UNMETERLOG_T},             //跨台区电能表事件 - 事件记录表
    {{0x31120300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //跨台区电能表事件 - 关联对象属性表
    {{0x31120400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //跨台区电能表事件 - 当前记录数
    {{0x31120500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //跨台区电能表事件 - 最大记录数
    {{0x31120700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //跨台区电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31120800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //跨台区电能表事件 - 上报标识
    {{0x31120900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //跨台区电能表事件 - 有效标识
    {{0x31120A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //跨台区电能表事件 - 时间状态记录表
       
    {{0x31140200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端对时事件 - 事件记录表
    {{0x31140300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端对时事件 - 关联对象属性表
    {{0x31140400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端对时事件 - 当前记录数
    {{0x31140500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端对时事件 - 最大记录数
    {{0x31140700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端对时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31140800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端对时事件 - 上报标识
    {{0x31140900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端对时事件 - 有效标识
    {{0x31140A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端对时事件 - 时间状态记录表
       
    {{0x31150200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_RCTRLLOG_T),       E_OOP_RCTRLLOG_T},               //遥控跳闸记录 - 事件记录表
    {{0x31150300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //遥控跳闸记录 - 关联对象属性表
    {{0x31150400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥控跳闸记录 - 当前记录数
    {{0x31150500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥控跳闸记录 - 最大记录数
    {{0x31150700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //遥控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31150800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //遥控跳闸记录 - 上报标识
    {{0x31150900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //遥控跳闸记录 - 有效标识
    {{0x31150A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //遥控跳闸记录 - 时间状态记录表
       
    {{0x31160200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_DCLOG_T),          E_OOP_DCLOG_T},                  //有功总电能量差动越限事件记录 - 事件记录表
    {{0x31160300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //有功总电能量差动越限事件记录 - 关联对象属性表
    {{0x31160400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //有功总电能量差动越限事件记录 - 当前记录数
    {{0x31160500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //有功总电能量差动越限事件记录 - 最大记录数
    {{0x31160600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3116_T),    E_OOP_EVTCFG_3116_T},            //有功总电能量差动越限事件记录 - 配置参数
    {{0x31160700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //有功总电能量差动越限事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31160800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //有功总电能量差动越限事件记录 - 上报标识
    {{0x31160900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //有功总电能量差动越限事件记录 - 有效标识
    {{0x31160A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //有功总电能量差动越限事件记录 - 时间状态记录表
       
    {{0x31170200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //输出回路接入状态变位事件记录 - 事件记录表
    {{0x31170300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //输出回路接入状态变位事件记录 - 关联对象属性表
    {{0x31170400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //输出回路接入状态变位事件记录 - 当前记录数
    {{0x31170500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //输出回路接入状态变位事件记录 - 最大记录数
    {{0x31170700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //输出回路接入状态变位事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31170800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //输出回路接入状态变位事件记录 - 上报标识
    {{0x31170900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //输出回路接入状态变位事件记录 - 有效标识
    {{0x31170A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //输出回路接入状态变位事件记录 - 时间状态记录表
       
    {{0x31180200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T),     E_OOP_PROGRAMLOG_T},             //终端编程记录 - 事件记录表
    {{0x31180300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端编程记录 - 关联对象属性表
    {{0x31180400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端编程记录 - 当前记录数
    {{0x31180500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端编程记录 - 最大记录数
    {{0x31180700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端编程记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31180800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端编程记录 - 上报标识
    {{0x31180900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端编程记录 - 有效标识
    {{0x31180A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端编程记录 - 时间状态记录表
       
    {{0x31190200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //终端电流回路异常事件 - 事件记录表
    {{0x31190300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端电流回路异常事件 - 关联对象属性表
    {{0x31190400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端电流回路异常事件 - 当前记录数
    {{0x31190500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端电流回路异常事件 - 最大记录数
    {{0x31190700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端电流回路异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31190800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端电流回路异常事件 - 上报标识
    {{0x31190900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端电流回路异常事件 - 有效标识
    {{0x31190A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端电流回路异常事件 - 时间状态记录表
       
    {{0x311A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METEROFFLOG_T),    E_OOP_METEROFFLOG_T},            //电能表在网状态切换事件 - 事件记录表
    {{0x311A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表在网状态切换事件 - 关联对象属性表
    {{0x311A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表在网状态切换事件 - 当前记录数
    {{0x311A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表在网状态切换事件 - 最大记录数
    {{0x311A0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_311A_T),    E_OOP_EVTCFG_311A_T},            //电能表在网状态切换事件 - 配置参数
    {{0x311A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表在网状态切换事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表在网状态切换事件 - 上报标识
    {{0x311A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表在网状态切换事件 - 有效标识
    {{0x311A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表在网状态切换事件 - 时间状态记录表
       
    {{0x311B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_P2PCLOCKLOG_T),    E_OOP_P2PCLOCKLOG_T},            //终端对电能表校时记录 - 事件记录表
    {{0x311B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端对电能表校时记录 - 关联对象属性表
    {{0x311B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端对电能表校时记录 - 当前记录数
    {{0x311B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //终端对电能表校时记录 - 最大记录数
    {{0x311B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //终端对电能表校时记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //终端对电能表校时记录 - 上报标识
    {{0x311B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //终端对电能表校时记录 - 有效标识
    {{0x311B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //终端对电能表校时记录 - 时间状态记录表
       
    {{0x311C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METERDIRTYLOG_T),  E_OOP_METERDIRTYLOG_T},          //电能表数据变更监控记录 - 事件记录表
    {{0x311C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表数据变更监控记录 - 关联对象属性表
    {{0x311C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表数据变更监控记录 - 当前记录数
    {{0x311C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表数据变更监控记录 - 最大记录数
    {{0x311C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //电能表数据变更监控记录 - 配置参数
    {{0x311C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表数据变更监控记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表数据变更监控记录 - 上报标识
    {{0x311C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表数据变更监控记录 - 有效标识
    {{0x311C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表数据变更监控记录 - 时间状态记录表

    {{0x311D0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METOFFLINELOG_T),  E_OOP_METOFFLINELOG_T},          //电能表在线状态变化事件 - 事件记录表
    {{0x311D0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电能表在线状态变化事件 - 关联对象属性表
    {{0x311D0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表在线状态变化事件 - 当前记录数
    {{0x311D0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电能表在线状态变化事件 - 最大记录数
    {{0x311D0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //电能表在线状态变化事件 - 配置参数
    {{0x311D0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电能表在线状态变化事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311D0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电能表在线状态变化事件 - 上报标识
    {{0x311D0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电能表在线状态变化事件 - 有效标识
    {{0x311D0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电能表在线状态变化事件 - 时间状态记录表

    {{0x311E0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METERDIRTYLOG_T),  E_OOP_METERDIRTYLOG_T},          //参数变更事件 - 事件记录表
    {{0x311E0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //参数变更事件 - 关联对象属性表
    {{0x311E0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //参数变更事件 - 当前记录数
    {{0x311E0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //参数变更事件 - 最大记录数
    {{0x311E0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //参数变更事件 - 配置参数
    {{0x311E0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //参数变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311E0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //参数变更事件 - 上报标识
    {{0x311E0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //参数变更事件 - 有效标识
    {{0x311E0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //参数变更事件 - 时间状态记录表

    {{0x311F0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_REFUSENODEACCESSLOG_T),  E_OOP_REFUSENODEACCESSLOG_T},          //拒绝从节点入网 - 事件记录表
    {{0x311F0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //拒绝从节点入网 - 关联对象属性表
    {{0x311F0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //拒绝从节点入网 - 当前记录数
    {{0x311F0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //拒绝从节点入网 - 最大记录数
    {{0x311F0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //拒绝从节点入网 - 配置参数
    {{0x311F0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //拒绝从节点入网 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311F0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //拒绝从节点入网 - 上报标识
    {{0x311F0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //拒绝从节点入网 - 有效标识
    {{0x311F0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //拒绝从节点入网 - 时间状态记录表

    {{0x31200200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //电流互感器异常事件记录 - 事件记录表
    {{0x31200300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电流互感器异常事件记录 - 关联对象属性表
    {{0x31200400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电流互感器异常事件记录 - 当前记录数
    {{0x31200500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电流互感器异常事件记录 - 最大记录数
    {{0x31200700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电流互感器异常事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31200800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电流互感器异常事件记录 - 上报标识
    {{0x31200900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电流互感器异常事件记录 - 有效标识
    {{0x31200A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电流互感器异常事件记录 - 时间状态记录表

    {{0x31210200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //TA专用模块工况事件 - 事件记录表
    {{0x31210300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //TA专用模块工况事件 - 关联对象属性表
    {{0x31210400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //TA专用模块工况事件 - 当前记录数
    {{0x31210500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //TA专用模块工况事件 - 最大记录数
    {{0x31210700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //TA专用模块工况事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31210800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //TA专用模块工况事件 - 上报标识
    {{0x31210900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //TA专用模块工况事件 - 有效标识
    {{0x31210A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //TA专用模块工况事件 - 时间状态记录表
    
    {{0x31400200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_SECURITYLOG_T),    E_OOP_SECURITYLOG_T},            //安全变更记录 - 事件记录表
    {{0x31400300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //安全变更记录 - 关联对象属性表
    {{0x31400400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //安全变更记录 - 当前记录数
    {{0x31400500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //安全变更记录 - 最大记录数
    {{0x31400600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            2*sizeof(uint32),             E_OOP_FLOW_PARAM_T},             //安全变更记录 - 参数下发
    {{0x31400700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //安全变更记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31400800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //安全变更记录 - 上报标识
    {{0x31400900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //安全变更记录 - 有效标识
    {{0x31400A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //安全变更记录 - 时间状态记录表

    {{0x31410200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //STA入网事件 - 事件记录表
    {{0x31410300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //STA入网事件 - 关联对象属性表
    {{0x31410400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //STA入网事件 - 当前记录数
    {{0x31410500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //STA入网事件 - 最大记录数
    {{0x31410700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //STA入网事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31410800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //STA入网事件 - 上报标识
    {{0x31410900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //STA入网事件 - 有效标识
    {{0x31410A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //STA入网事件 - 时间状态记录表

#ifdef AREA_SHAANXI

	{{0x31990200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_ICCID_CHANGE_EVENT_T), E_OOP_ICCIDLOG_T},               //ICCID变更事件 - 事件记录表
    {{0x31990300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ICCID变更事件 - 关联对象属性表
    {{0x31990400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ICCID变更事件 - 当前记录数
    {{0x31990500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ICCID变更事件 - 最大记录数
    {{0x31990700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ICCID变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31990800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ICCID变更事件 - 上报标识
    {{0x31990900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ICCID变更事件 - 有效标识
    {{0x31990A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ICCID变更事件 - 时间状态记录表
#endif

    {{0x32000200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_PCTRLLOG_T),       E_OOP_PCTRLLOG_T},               //功控跳闸记录 - 事件记录表
    {{0x32000300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //功控跳闸记录 - 关联对象属性表
    {{0x32000400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //功控跳闸记录 - 当前记录数
    {{0x32000500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //功控跳闸记录 - 最大记录数
    {{0x32000700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //功控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32000800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //功控跳闸记录 - 上报标识
    {{0x32000900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //功控跳闸记录 - 有效标识
    {{0x32000A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //功控跳闸记录 - 时间状态记录表
       
    {{0x32010200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_ECTRLLOG_T),       E_OOP_ECTRLLOG_T},               //电控跳闸记录 - 事件记录表
    {{0x32010300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电控跳闸记录 - 关联对象属性表
    {{0x32010400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电控跳闸记录 - 当前记录数
    {{0x32010500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电控跳闸记录 - 最大记录数
    {{0x32010700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32010800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电控跳闸记录 - 上报标识
    {{0x32010900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电控跳闸记录 - 有效标识
    {{0x32010A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电控跳闸记录 - 时间状态记录表
       
    {{0x32020200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //购电参数设置记录 - 事件记录表
    {{0x32020300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //购电参数设置记录 - 关联对象属性表
    {{0x32020400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //购电参数设置记录 - 当前记录数
    {{0x32020500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //购电参数设置记录 - 最大记录数
    {{0x32020700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //购电参数设置记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32020800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //购电参数设置记录 - 上报标识
    {{0x32020900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //购电参数设置记录 - 有效标识
    {{0x32020A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //购电参数设置记录 - 时间状态记录表
       
    {{0x32030200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EWARNLOG_T),       E_OOP_EWARNLOG_T},               //电控告警事件记录 - 事件记录表
    {{0x32030300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电控告警事件记录 - 关联对象属性表
    {{0x32030400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电控告警事件记录 - 当前记录数
    {{0x32030500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电控告警事件记录 - 最大记录数
    {{0x32030700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电控告警事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32030800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电控告警事件记录 - 上报标识
    {{0x32030900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电控告警事件记录 - 有效标识
    {{0x32030A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电控告警事件记录 - 时间状态记录表
    {{0x33000200}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_REPORTS_T),        E_OOP_REPORTS_T},                //事件上报状态
    {{0x33030206}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_SEARCHMET_RES_T),  E_OOP_SEARCHMET_RES_T},          //发现未知电能表事件-一个搜表结果
    {{0x33050206}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //功控跳闸单元-发生后2min功率
    {{0x33050209}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //功控跳闸单元-功控定制
    {{0x3305020A}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //功控跳闸单元-跳闸发生前功率
    {{0x33060209}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //电控跳闸单元-跳闸发生时总加组电能量
    {{0x330a0206}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //遥控跳闸单元-发生后2min功率
    {{0x330a0207}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //遥控跳闸单元-发生前功率
    {{0x33090204}, 8,       0x03,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_EVTSOURCE_T),      E_OOP_EVTSOURCE_T},              //终端停上电事件-事件发生源
    {{0x33090206}, 8,       0x03,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),        E_OOP_BITSTR8_T},                //终端停上电事件-属性标志
    {{0x33020206}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //终端编程记录-编程对象列表
    {{0x33120206}, 8,       0x03,   0x03, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),     E_OOP_OCTETVAR16_T},             //模块变更记录-模块宿主的通信地址
    {{0x33120207}, 8,       0x03,   0x03, 0,      DT_VISIBLE_STRING,       sizeof(OOP_VISIBLEVAR256_T),  E_VISIBLE_STRING},               //模块变更记录-变更前的模块描述符
    {{0x33120208}, 8,       0x03,   0x03, 0,      DT_VISIBLE_STRING,       sizeof(OOP_VISIBLEVAR256_T),  E_VISIBLE_STRING},               //模块变更记录-变更后的模块描述符
    {{0x33130206}, 8,       0x03,   0x03, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),     E_DATETIME_S},                   //电能表时钟超差-电能表时钟
    {{0x33130207}, 8,       0x03,   0x03, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),     E_DATETIME_S},                   //电能表时钟超差-终端当前时钟
    {{0x33180204}, 8,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVTSOURCE_T),      E_OOP_EVTSOURCE_T},              //事件发生源

    {{0x3E030200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_IdentifyResult_T), E_OOP_IdentifyResult_T},    
    {{0x3E030800}, 7,       0x03,   0x0E, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //拓扑识别事件 - 上报标识
    {{0x3E030900}, 7,       0x03,   0x0E, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED}, 

    {{0x34010200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电计划生成失败事件 - 事件记录表
    {{0x34010300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电计划生成失败事件 - 关联对象属性表
    {{0x34010400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划生成失败事件 - 当前记录数
    {{0x34010500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划生成失败事件 - 最大记录数
    {{0x34010700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电计划生成失败事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34010800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划生成失败事件 - 上报标识
    {{0x34010900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划生成失败事件 - 有效标识
    {{0x34010A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电计划生成失败事件 - 时间状态记录表

    {{0x34020200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电计划上报事件 - 事件记录表
    {{0x34020300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电计划上报事件 - 关联对象属性表
    {{0x34020400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划上报事件 - 当前记录数
    {{0x34020500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划上报事件 - 最大记录数
    {{0x34020700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电计划上报事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34020800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划上报事件 - 上报标识
    {{0x34020900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划上报事件 - 有效标识
    {{0x34020A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电计划上报事件 - 时间状态记录表

    {{0x34030200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电申请事件 - 事件记录表
    {{0x34030300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电申请事件 - 关联对象属性表
    {{0x34030400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电申请事件 - 当前记录数
    {{0x34030500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电申请事件 - 最大记录数
    {{0x34030700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电申请事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34030800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电申请事件 - 上报标识
    {{0x34030900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电申请事件 - 有效标识
    {{0x34030A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电申请事件 - 时间状态记录表

    {{0x34040200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电执行事件 - 事件记录表
    {{0x34040300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电执行事件 - 关联对象属性表
    {{0x34040400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电执行事件 - 当前记录数
    {{0x34040500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电执行事件 - 最大记录数
    {{0x34040700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电执行事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34040800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电执行事件 - 上报标识
    {{0x34040900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电执行事件 - 有效标识
    {{0x34040A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电执行事件 - 时间状态记录表

    {{0x34050200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电桩异常事件 - 事件记录表
    {{0x34050300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电桩异常事件 - 关联对象属性表
    {{0x34050400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电桩异常事件 - 当前记录数
    {{0x34050500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电桩异常事件 - 最大记录数
    {{0x34050700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电桩异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34050800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电桩异常事件 - 上报标识
    {{0x34050900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电桩异常事件 - 有效标识
    {{0x34050A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电桩异常事件 - 时间状态记录表

    {{0x34060200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电订单结算事件 - 事件记录表
    {{0x34060300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电订单结算事件 - 关联对象属性表
    {{0x34060400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电订单结算事件 - 当前记录数
    {{0x34060500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电订单结算事件 - 最大记录数
    {{0x34060700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电订单结算事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34060800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电订单结算事件 - 上报标识
    {{0x34060900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电订单结算事件 - 有效标识
    {{0x34060A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电订单结算事件 - 时间状态记录表

    {{0x34070200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //充电计划下发失败事件 - 事件记录表
    {{0x34070300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //充电计划下发失败事件 - 关联对象属性表
    {{0x34070400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划下发失败事件 - 当前记录数
    {{0x34070500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //充电计划下发失败事件 - 最大记录数
    {{0x34070700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //充电计划下发失败事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x34070800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划下发失败事件 - 上报标识
    {{0x34070900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //充电计划下发失败事件 - 有效标识
    {{0x34070A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //充电计划下发失败事件 - 时间状态记录表

    {{0x35000200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //遥信变位事件 - 事件记录表
    {{0x35000300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //遥信变位事件 - 关联对象属性表
    {{0x35000400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥信变位事件 - 当前记录数
    {{0x35000500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥信变位事件 - 最大记录数
    {{0x35000700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //遥信变位事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35000800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //遥信变位事件 - 上报标识
    {{0x35000900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //遥信变位事件 - 有效标识
    {{0x35000A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //遥信变位事件 - 时间状态记录表

    {{0x35010200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //遥测越限事件 - 事件记录表
    {{0x35010300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //遥测越限事件 - 关联对象属性表
    {{0x35010400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥测越限事件 - 当前记录数
    {{0x35010500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //遥测越限事件 - 最大记录数
    {{0x35010700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //遥测越限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35010800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //遥测越限事件 - 上报标识
    {{0x35010900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //遥测越限事件 - 有效标识
    {{0x35010A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //遥测越限事件 - 时间状态记录表

    {{0x35020200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //电容衰减率越限事件 - 事件记录表
    {{0x35020300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //电容衰减率越限事件 - 关联对象属性表
    {{0x35020400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电容衰减率越限事件 - 当前记录数
    {{0x35020500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //电容衰减率越限事件 - 最大记录数
    {{0x35020700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //电容衰减率越限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35020800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //电容衰减率越限事件 - 上报标识
    {{0x35020900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //电容衰减率越限事件 - 有效标识
    {{0x35020A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //电容衰减率越限事件 - 时间状态记录表

    {{0x35030200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //断路器保护动作事件 - 事件记录表
    {{0x35030300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //断路器保护动作事件 - 关联对象属性表
    {{0x35030400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器保护动作事件 - 当前记录数
    {{0x35030500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器保护动作事件 - 最大记录数
    {{0x35030700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //断路器保护动作事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35030800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //断路器保护动作事件 - 上报标识
    {{0x35030900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //断路器保护动作事件 - 有效标识
    {{0x35030A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //断路器保护动作事件 - 时间状态记录表 

    {{0x35040200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //断路器闸位变化事件- 事件记录表
    {{0x35040300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //断路器闸位变化事件 - 关联对象属性表
    {{0x35040400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器闸位变化事件 - 当前记录数
    {{0x35040500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器闸位变化事件- 最大记录数
    {{0x35040700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //断路器闸位变化事件- 当前值记录表????是不是用这个结构体，待确认
    {{0x35040800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //断路器闸位变化事件 - 上报标识
    {{0x35040900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //断路器闸位变化事件 - 有效标识
    {{0x35040A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //断路器闸位变化事件 - 时间状态记录表 

    {{0x35050200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_PF_INPUT_EXIT_T),  E_OOP_PF_INPUT_EXIT_T},          //断路器保护功能投退事件 - 事件记录表
    {{0x35050300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //断路器保护功能投退事件 - 关联对象属性表
    {{0x35050400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器保护功能投退事件 - 当前记录数
    {{0x35050500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器保护功能投退事件 - 最大记录数
    {{0x35050700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //断路器保护功能投退事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35050800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //断路器保护功能投退事件 - 上报标识
    {{0x35050900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //断路器保护功能投退事件 - 有效标识
    {{0x35050A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //断路器保护功能投退事件 - 时间状态记录表

    {{0x35060200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //断路器告警事件 - 事件记录表
    {{0x35060300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //断路器告警事件 - 关联对象属性表
    {{0x35060400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器告警事件 - 当前记录数
    {{0x35060500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //断路器告警事件- 最大记录数
    {{0x35060700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //断路器告警事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x35060800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //断路器告警事件- 上报标识
    {{0x35060900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //断路器告警事件 - 有效标识
    {{0x35060A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //断路器告警事件 - 时间状态记录表

	{{0x36000200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		      //设备监测事件 - 事件记录表
	{{0x36000300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //设备监测事件 - 关联对象属性表
	{{0x36000400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //设备监测事件 - 当前记录数
	{{0x36000500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //设备监测事件 - 最大记录数
	{{0x36000600}, 7,       0x03,   0x06, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3600_T),    E_OOP_EVTCFG_3600_T},            //电能表在线状态变化事件 - 配置参数
	{{0x36000700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //设备监测事件 - 当前值记录表????是不是用这个结构体，待确认
	{{0x36000800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //设备监测事件 - 上报标识
	{{0x36000900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //设备监测事件 - 有效标识
	{{0x36000A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //设备监测事件 - 时间状态记录表
	
	{{0x36010200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		  	  //容器监测事件 - 事件记录表
	{{0x36010300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //容器监测事件 - 关联对象属性表
	{{0x36010400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //容器监测事件 - 当前记录数
	{{0x36010500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //容器监测事件 - 最大记录数
	{{0x36010700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //容器监测事件 - 当前值记录表????是不是用这个结构体，待确认
	{{0x36010800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //容器监测事件 - 上报标识
	{{0x36010900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //容器监测事件 - 有效标识
	{{0x36010A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //容器监测事件 - 时间状态记录表
	
	{{0x36020200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		      //应用监测事件 - 事件记录表
	{{0x36020300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //应用监测事件 - 关联对象属性表
	{{0x36020400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //应用监测事件 - 当前记录数
	{{0x36020500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //应用监测事件 - 最大记录数
	{{0x36020700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //应用监测事件 - 当前值记录表????是不是用这个结构体，待确认
	{{0x36020800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //应用监测事件 - 上报标识
	{{0x36020900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //应用监测事件 - 有效标识
	{{0x36020A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //应用监测事件 - 时间状态记录表
	
#ifdef AREA_SHANDONG
    {{0x3E040200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //温度超限事件
    {{0x3E010200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //湿度超限事件
    {{0x3E020200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //门开关事件
    {{0x3E030200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //锁开关事件
    {{0x3E400200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //疑似窃电事件
    {{0x3E410200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //图片视频采集事件
    {{0x3E420200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //强磁场事件
#endif
};
const uint32 oadEvnetInfoListLen = sizeof(oadEvnetInfoList)/sizeof(oadEvnetInfoList[0]);

/* 参变量类OAD列表 OIA1 = 0x04 */
const OAD_TAB_T oadParaInfoList[] =
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x40000200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //短时间格式 日期-时间
    {{0x40000300}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_ENUM},                     //校时模式
    {{0x40000400}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCCLOCK_T),          E_OOP_SYNCCLOCK_T},          //精准校时参数
    {{0x40000500}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_BROADCASTCLOCK_T),          E_OOP_BROADCASTCLOCK_T},          //广播校时参数
    {{0x40000600}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCCYCLE_T),          E_NULL},                     //卫星对时周期
    {{0x40010200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //通信地址
    {{0x40020200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //表号
    {{0x40030200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //客户编号
    {{0x40040200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_GEOGADD_T),            E_OOP_GEOGADD_T},            //设备地理位置
    {{0x40050200}, 8,      0x04,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_GRPADDR_T),            E_OOP_GRPADDR_T},            //组地址
    {{0x40060200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CLOCK_T),              E_OOP_CLOCK_T},              //时钟源参数
    {{0x40070200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LCD_T),                E_OOP_LCD_T},                //LCD参数
    {{0x40080200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //备用套时区表切换时间
    {{0x40090200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //备用套时区表切换时间
    {{0x400A0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //备用套时区表切换时间
    {{0x400B0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //备用套时区表切换时间
    {{0x400C0201}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //时区时段数
    {{0x400C0202}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //时区时段数
    {{0x400C0203}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //时区时段数
    {{0x400C0204}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //时区时段数
    {{0x400C0205}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //时区时段数
    {{0x400C0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PERIOD_T),             E_OOP_PERIOD_T},             //时区时段数
    {{0x400D0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //阶梯数
    {{0x400E0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //谐波分析次数
    {{0x400F0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //密钥总条数
    {{0x40100200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //计量元件数
    {{0x40110200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PUBHOLIDAY_T),         E_OOP_PUBHOLIDAY_T},         //公共假日表
    {{0x40120200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),        E_OOP_BITSTR8_T},        //周休日特征字
    {{0x40130200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //周休日釆用的日时段表号
    {{0x40140200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T),           E_OOP_TIMEZONE_T},           //当前套时区表
    {{0x40150200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T),           E_OOP_TIMEZONE_T},           //备用套时区表
    {{0x40160200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T),          E_OOP_DAYPERIOD_T},          //当前套日时段表
    {{0x40170200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T),          E_OOP_DAYPERIOD_T},          //备用套日时段表
    {{0x40180200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T),         E_OOP_RATETARIFF_T},         //当前套费率电价
    {{0x40190200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T),         E_OOP_RATETARIFF_T},         //备用套费率电价
    {{0x401A0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T),          E_OOP_STEPPRICE_T},          //当前套阶梯电价
    {{0x401B0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T),          E_OOP_STEPPRICE_T},          //备用套阶梯电价
    {{0x401C0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //电流互感器变比
    {{0x401D0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //电压互感器变比
    //{{0x401E0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LIMITCHARGE_T),        E_OOP_LIMITCHARGE_T},        //报警金额限值
    {{0x401E0202}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,             sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},        //报警金额限值
    {{0x401F0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERLIMIT_T),         E_OOP_OTHERLIMIT_T},         //其它金额限值
    {{0x40200200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_POWERLIMIT_T),         E_OOP_POWERLIMIT_T},         //报警电量限值
    {{0x40210200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERPOWERLIMIT_T),    E_OOP_OTHERPOWERLIMIT_T},    //其它电量限值
    {{0x40220200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //插卡状态字
    {{0x40240200}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //剔除
    {{0x40250200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DC_UPDATE_T),          E_OOP_DC_UPDATE_T},          //升级结果列表
    {{0x40250300}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_DCU_CONFIG_T),         E_OOP_DCU_CONFIG_T},         //采集器升级控制参数
    {{0x40270200}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},         

    {{0x40300200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_VOLTPARAM_T),          E_OOP_VOLTPARAM_T},          //电压合格率参数
    {{0x40400200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TASPCUSEMOD_T),        E_OOP_TASPCUSEMOD_T},        //TA模块信息
    {{0x40400300}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_WORKINFO_T),           E_OOP_WORKINFO_T},           //TA模工况信息
    {{0x40400400}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TAUNIT_T),             E_OOP_TAUNIT_T},             //TA模块换算及单位
    {{0x40400500}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_CTINFO_T),             E_OOP_CTINFO_T},             //TA模块版本信息
    {{0x40410200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CURLOOPCKENABLE_T),    E_OOP_CURLOOPCKENABLE_T},    //电流回路监测使能

    {{0x41000200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //最大需量周期
    {{0x41010200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //滑差时间
    {{0x41020200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //校表脉冲宽度
    {{0x41030200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //资产管理编码
    {{0x41040200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //额定电压
    {{0x41050200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //额定电流/基本电流
    {{0x41060200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //最大电流
    {{0x41070200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //有功准确度等级
    {{0x41080200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //无功准确度等级
    {{0x41090200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //电能表有功常数
    {{0x410A0200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //电能表无功常数
    {{0x410B0200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //电能表型号
    {{0x410C0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC各相电导系数
    {{0x410D0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC各相电抗系数
    {{0x410E0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC各相电阻系数
    {{0x410F0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC各相电纳系数
    {{0x41100200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //电能表运行方式特征字
    {{0x41110200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //软件备案号
    {{0x41110300}, 8,      0x04,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_SOFTREC_LIST),       E_OOP_SOFTREC_LIST},       //软件备案号列表
    {{0x41120200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //有功组合方式特征字
    {{0x41130200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //无功组合方式1特征字
    {{0x41140200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //无功组合方式2特征字
    {{0x41160200}, 8,      0x04,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_PAYDAYS_T),            E_OOP_PAYDAYS_T},            //结算日日期
    {{0x41170200}, 8,      0x04,   0x01, 0,      DT_TI,                    sizeof(OOP_TI_T),                 E_OOP_TI_T}, //期间需量冻结周期
    {{0x41210200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_Power_Characteristic_Record),                 E_OOP_Power_Characteristic_Record},
    {{0x41220200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_Energy_Characteristic_Record_Unit),                 E_OOP_Energy_Characteristic_Record},
    {{0x42020200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_CASCADE_T),            E_OOP_CASCADE_T},            //级联通信参数
    {{0x42040200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCTIME_T),           E_OOP_SYNCTIME_T},           //终端广播校时参数
    {{0x42040300}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_P2PSYNCTIME_T),        E_OOP_P2PSYNCTIME_T},        //终端单地址广播校时参数
    {{0x43000200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //电气设备-设备描述符
    {{0x43000300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //电气设备-版本信息
    {{0x43000400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //电气设备-生产日期
    {{0x43000500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //电气设备-子设备列表
    {{0x43000600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //电气设备-支持规约列表
    {{0x43000700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                 //电气设备-允许跟随上报
    {{0x43000800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                     //电气设备-允许主动上报
    {{0x43000900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                 //电气设备-允许与主站通话
    {{0x43000A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //电气设备-上报通道
    {{0x43000B00}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_SERVICES_T),           E_OOP_SERVICES_T},       //电气设备-一组服务授权状态
    {{0x43000C00}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_DEVICERUNSTAT_T),      E_OOP_DEVICERUNSTAT_T},       //电气设备-设备运行时长

    {{0x43070200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //水表-设备描述符
    {{0x43070300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //水表-版本信息
    {{0x43070400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //水表-生产日期
    {{0x43070500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //水表-子设备列表
    {{0x43070600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //水表-支持规约列表
    {{0x43070700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //水表-允许跟随上报
    {{0x43070800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //水表-允许主动上报
    {{0x43070900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //水表-允许与主站通话
    {{0x43070A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //水表-上报通道
    {{0x43080200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //气表-设备描述符
    {{0x43080300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //气表-版本信息
    {{0x43080400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //气表-生产日期
    {{0x43080500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //气表-子设备列表
    {{0x43080600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //气表-支持规约列表
    {{0x43080700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //气表-允许跟随上报
    {{0x43080800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //气表-允许主动上报
    {{0x43080900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //气表-允许与主站通话
    {{0x43080A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //气表-上报通道
    {{0x43090200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //热表-设备描述符
    {{0x43090300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //热表-版本信息
    {{0x43090400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //热表-生产日期
    {{0x43090500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //热表-子设备列表
    {{0x43090600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //热表-支持规约列表
    {{0x43090700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //热表-允许跟随上报
    {{0x43090800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //热表-允许主动上报
    {{0x43090900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //热表-允许与主站通话
    {{0x43090A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //热表-上报通道
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)    
    {{0x43201300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Line_Loss_T),          E_OOP_LINE_LOSS_T},          //线损
    {{0x43201700}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //回路阻抗
    {{0x43201701}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //A相回路阻抗 
    {{0x43201702}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //B相回路阻抗 
    {{0x43201703}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //C相回路阻抗 
#endif    
    {{0x44000200}, 20,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_OI_RIGHTS_T),          E_OOP_OI_RIGHTS_T},     //对象列表???没有找到对应结构体
    {{0x44000300}, 20,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_ASSOCIATION_CONTEXT_T),E_OOP_ASSOCIATION_CONTEXT_T},//应用语境信息
    {{0x44000400}, 20,     0x04,   0x04, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //当前连接的客户机地址
    {{0x44000500}, 20,     0x04,   0x04, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //连接认证机制
    {{0x44010200}, 8,      0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //认证密码
    {{0x45000200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T),         E_OOP_GPRSCONFIG_T},         //公网通信模块参数1 - GPRS通信配置
    {{0x45000300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //公网通信模块参数1 - 主站通信参数表
    {{0x45000400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T),                E_OOP_SMS_T},                //公网通信模块参数1 - 短信通信参数
    {{0x45000500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //公网通信模块参数1 - 版本信息
    {{0x45000600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //公网通信模块参数1 - 支持规约列表
    {{0x45000700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //公网通信模块参数1 - SIM卡的ICCID
    {{0x45000800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T),       E_OOP_VISIBLESTR15_T},       //公网通信模块参数1 - IMSI
    {{0x45000900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG},            		  //公网通信模块参数1 - 信号强度
    {{0x45000A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //公网通信模块参数1 - SIM卡号码
    {{0x45000B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //公网通信模块参数1 - 拨号IP
    {{0x45000C00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR_T),         E_OOP_VISIBLEVAR32_T},       //设备描述符     
    {{0x45000D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T),            E_OOP_VISIBLEVAR32_T},       //运营商及网络制式
    {{0x45000E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),           E_OOP_VISIBLEVAR32_T},       //多网络配置
#if (defined AREA_SHAANXI)    
	{{0x45001600}, 25,	   0x04,   0x05, 0, 	 DT_STRUCTURE,			   sizeof(OOP_4GMONITOR_INFO_T),	 E_4GMONITOR_T},			  //公网通信模块参数1 - 误码率
	{{0x45001700}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG},            		  //公网通信模块参数1 - 误码率
	{{0x45001800}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETSTR4_T),          E_OCTET_STR4_T},             //公网通信模块参数1 - 小区编号
	{{0x45001900}, 25,     0x04,   0x05, 0,      DT_ENUM,                  sizeof(uint8),                    E_ENUM},       			  //公网通信模块参数1 - 网络制式
#endif 

	{{0x45010200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T),         E_OOP_GPRSCONFIG_T},         //公网通信模块参数2 - GPRS通信配置
    {{0x45010300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //公网通信模块参数2 - 主站通信参数表
    {{0x45010400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T),                E_OOP_SMS_T},                //公网通信模块参数2 - 短信通信参数
    {{0x45010500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //公网通信模块参数2 - 版本信息
    {{0x45010600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //公网通信模块参数2 - 支持规约列表
    {{0x45010700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR20_T),       E_OOP_VISIBLESTR20_T},       //公网通信模块参数2 - SIM卡的ICCID
    {{0x45010800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T),       E_OOP_VISIBLESTR15_T},       //公网通信模块参数2 - IMSI
    {{0x45010900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG_UNSIGNED},            //公网通信模块参数2 - 信号强度
    {{0x45010A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //公网通信模块参数2 - SIM卡号码
    {{0x45010B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //公网通信模块参数2 - 拨号IP
    {{0x45010C00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR_T),         E_OOP_VISIBLEVAR32_T},       //设备描述符 
    {{0x45010D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T),            E_OOP_VISIBLEVAR32_T},       //运营商及网络制式
    {{0x45010E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),           E_OOP_VISIBLEVAR32_T},       //多网络配置
    
    {{0x45100200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数1 - 以太网通信配置
    {{0x45100300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数1 - 主站通信参数表
    {{0x45100400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数1 - 以太网本地IP配置
    {{0x45100500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数1 - MAC地址
    {{0x45110200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数2 - 以太网通信配置
    {{0x45110300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数2 - 主站通信参数表
    {{0x45110400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数2 - 以太网本地IP配置
    {{0x45110500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数2 - MAC地址
    {{0x45120200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数3 - 以太网通信配置
    {{0x45120300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数3 - 主站通信参数表
    {{0x45120400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数3 - 以太网本地IP配置
    {{0x45120500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数3 - MAC地址
    {{0x45130200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数4 - 以太网通信配置
    {{0x45130300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数4 - 主站通信参数表
    {{0x45130400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数4 - 以太网本地IP配置
    {{0x45130500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数4 - MAC地址
    {{0x45140200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数5 - 以太网通信配置
    {{0x45140300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数5 - 主站通信参数表
    {{0x45140400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数5 - 以太网本地IP配置
    {{0x45140500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数5 - MAC地址
    {{0x45150200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数6 - 以太网通信配置
    {{0x45150300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数6 - 主站通信参数表
    {{0x45150400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数6 - 以太网本地IP配置
    {{0x45150500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数6 - MAC地址
    {{0x45160200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数7 - 以太网通信配置
    {{0x45160300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数7 - 主站通信参数表
    {{0x45160400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数7 - 以太网本地IP配置
    {{0x45160500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数7 - MAC地址
    {{0x45170200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //以太网通信模块参数8 - 以太网通信配置
    {{0x45170300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //以太网通信模块参数8 - 主站通信参数表
    {{0x45170400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //以太网通信模块参数8 - 以太网本地IP配置
    {{0x45170500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //以太网通信模块参数8 - MAC地址
    {{0x47010200}, 8,      0x04,   0x07, 0,      DT_STRUCTURE,             sizeof(OOP_PILE_WORK_STATUS_T),   E_PILE_WORK_STATUS_T},       //订单状态
    {{0x47020400}, 8,      0x04,   0x07, 0,      DT_STRUCTURE,             sizeof(OOP_CHARGE_SERVICE_T),     E_CHARGE_SERVICE_T},         //充电服务
    {{0x48000400}, 8,      0x04,   0x08, 0,      DT_STRUCTURE,             sizeof(OOP_DEVFREZDATAS_T),       E_OOP_DEVFREZDATAS_T},       //电气设备冻结数据集
    {{0x49080200}, 8,      0x04,   0x09, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //开入量采集防抖时间
#if (defined AREA_FUJIAN)    
	{{0x4e010200}, 8,	   0x04,   0x0e, 0, 	 DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),       	 E_OOP_BITSTR16_T},		  	  //当前电源状态字
	{{0x4e020200}, 8,	   0x04,   0x0e, 0, 	 DT_STRUCTURE,			   sizeof(OOP_BATTERY_STATUS_T),	 E_BATTERY_STATUS_T},		  //停电工作电池状态
	{{0x4e030200}, 8,	   0x04,   0x0e, 0, 	 DT_STRUCTURE,			   sizeof(OOP_SINGAL_INFO_T),	     E_SINGAL_INFO_T},			  //现场信号信息
#endif 


};
const uint32 oadParaInfoListLen = sizeof(oadParaInfoList)/sizeof(oadParaInfoList[0]);

/* 冻结类OAD列表 OIA1 = 0x05 */
const OAD_TAB_T oadFreezeInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x50000200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //瞬时冻结 - 冻结数据表
    {{0x50000300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //瞬时冻结 - 关联对象属性表
    {{0x50010200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //秒冻结 - 冻结数据表
    {{0x50010300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //秒冻结 - 关联对象属性表
    {{0x50020200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //分钟冻结 - 冻结数据表
    {{0x50020300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //分钟冻结 - 关联对象属性表
    {{0x50030200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //小时冻结 - 冻结数据表
    {{0x50030300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //小时冻结 - 关联对象属性表
    {{0x50040200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //日冻结 - 冻结数据表
    {{0x50040300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //日冻结 - 关联对象属性表
    {{0x50050200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //结算日冻结 - 冻结数据表
    {{0x50050300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //结算日冻结 - 关联对象属性表
    {{0x50060200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //月冻结 - 冻结数据表
    {{0x50060300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //月冻结 - 关联对象属性表
    {{0x50070200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //年冻结 - 冻结数据表
    {{0x50070300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //年冻结 - 关联对象属性表
    {{0x50080200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //时区表切换冻结 - 冻结数据表
    {{0x50080300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //时区表切换冻结 - 关联对象属性表
    {{0x50090200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //日时段表切换冻结 - 冻结数据表
    {{0x50090300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //日时段表切换冻结 - 关联对象属性表
    {{0x500A0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //费率电价切换冻结 - 冻结数据表
    {{0x500A0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //费率电价切换冻结 - 关联对象属性表
    {{0x500B0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //阶梯切换冻结 - 冻结数据表
    {{0x500B0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //阶梯切换冻结 - 关联对象属性表
    {{0x50110200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //阶梯结算冻结 - 冻结数据表
    {{0x50110300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //阶梯结算冻结 - 关联对象属性表
};
const uint32 oadFreezeInfoListLen = sizeof(oadFreezeInfoList)/sizeof(oadFreezeInfoList[0]);

/* 采集类OAD列表 OIA1 = 0x06 */
const OAD_TAB_T oadCollectInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x60000200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_METER_LIST},           //采集档案配置表
    {{0x60010200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_OOP_METER_T},              //采集档案配置单元
    {{0x60010201}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //配置序号
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             10,                              E_BASIC_OBJECT},             //基本信息
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //基本信息 - 通信地址
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 波特率
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 规约类型
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //基本信息 - 端口
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_PWD_COMM},                 //基本信息 - 通信密码
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //基本信息 - 费率个数
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //基本信息 - 用户类型
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 接线方式
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //基本信息 - 额度电压
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //基本信息 - 额度电流
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_EXTENDED_OBJECT},          //扩展信息
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //扩展信息 - 采集器地址
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_ASSETNO},                  //扩展信息 - 资产号
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //扩展信息 - 电压互感器变比
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //扩展信息 - 电流互感器变比
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_ANNEX_OBJECT_LIST},        //附属信息   
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_ANNEX_OBJECT},             //附属信息   
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //附属信息 - 对象属性描述
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //附属信息 - 属性值                          
    {{0x60000300}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //采集档案配置表 - 当前元素个数
    {{0x60000400}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //采集档案配置表 - 最大元素个数
    {{0x60020200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_SEARCHMET_RES_LIST},   //所有搜表结果
    {{0x60030200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             7,                               E_OOP_SEARCHMET_RES_T},      //一个搜表结果
    {{0x60030201}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //通信地址
    {{0x60030202}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //所属采集器地址
    {{0x60030203}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //规约类型
    {{0x60030204}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //相位
    {{0x60030205}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //信号品质
    {{0x60030206}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},         //搜到的时间
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_ADDEDINFO_LIST},       //搜到的附加信息
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_OOP_ADDEDINFO_T},          //搜到的附加信息
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //搜到的附加信息 - 对象属性描述
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //搜到的附加信息 - 属性值 
    {{0x60020500}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_ACROSSAREA_RES_LIST},  //跨台区结果
    {{0x60040200}, 11,     0x06,   0x00, 1,      DT_STRUCTURE,             3,                               E_OOP_ACROSSAREA_RES_T},     //一个跨台区结果
    {{0x60040201}, 8,      0x06,   0x00, 1,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //通信地址
    {{0x60040202}, 8,      0x06,   0x00, 1,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //主节点地址
    {{0x60040203}, 8,      0x06,   0x00, 1,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},         //变更时间
    {{0x60020400}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //最大元素个数
    {{0x60020600}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //所有搜表结果记录数
    {{0x60020700}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //跨台区搜表结果记录数
    {{0x60020800}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHPARAM_T),       E_OOP_SEARCHPARAM_T},        //搜表参数
    {{0x60020900}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TIMINGSEARCH_T),      E_OOP_TIMINGSEARCH_T},       //定时搜表参数集合
    {{0x60020A00}, 11,     0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //搜表状态
    {{0x60120200}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_TASK_LIST},            //任务配置表
    {{0x60120300}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //读取任务记录表
    {{0x60128000}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //删除一组任务
    {{0x60128100}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //清空任务配置表
    {{0x60130200}, 10,     0x06,   0x00, 1,      DT_STRUCTURE,             sizeof(OOP_TASK_T),              E_OOP_TASK_T},               //任务配置单元


    {{0x60140200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_NORMAL_T),       E_OOP_PLAN_NORMAL_T},        //普通采集方案集
    {{0x60160200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_EVENT_T),        E_OOP_PLAN_EVENT_T},         //事件采集方案集
    {{0x60180200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_TRANS_T),        E_OOP_PLAN_TRANS_T},         //透明方案集
    {{0x601A0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TRANSRES_T),          E_OOP_TRANSRES_T},           //透明方案结果集
    {{0x601C0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_REPORT_T),       E_OOP_PLAN_REPORT_T},        //上报方案集
    {{0x601E0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_ENCRYPT_T),      E_NULL},                     //转加密方案
    {{0x60200200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ENCRYPTRES_T),        E_NULL},                     //转加密结果
    {{0x60320200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_RLUESTATE_T),     E_OOP_ACQ_RLUESTATE_T},      //采集状态集
    {{0x60340200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T),       E_OOP_ACQ_MONITOR_T},        //采集任务监控集

    {{0x60400200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //事件发生时间
    {{0x60410200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //事件结束时间
    {{0x60420200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},

    {{0x60510200}, 6,      0x06,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_LNLS_STATISVAL_T),    E_OOP_LNLS_STATISVAL_T},     //线损计算数据
    {{0x60510400}, 6,      0x06,   0x00, 0,      DT_TI,                    sizeof(OOP_LNLSPER_T),           E_OOP_LNLSPER_T},            //线损统计周期
//    {{0x6E430200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,                    sizeof(ABNORMAL_METER_INFO_T),           E_ABNORMAL_METER_INFO_T},            //异常电表数据
    {{0x60F10200}, 11,      0x06,   0x00, 0,      DT_ARRAY,                sizeof(OOP_NodeTopology_T), E_OOP_NodeTopology_T},  //节点拓扑信息
    
    {{0x6E000200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(OOP_GATHER_E),            E_NULL},                     //外部设备数据采集模式
    {{0x6E000300}, 8,      0x06,   0x0E, 1,      DT_UNSIGNED,              sizeof(uint8),                   E_NULL},                     //写保护剩余允许时间
    {{0x6E000400}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //非增补代理任务优先级
    {{0x6E010200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //终端本体采集模式
    {{0x6E020200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //加密芯片类型
    {{0x6E030200}, 8,      0x06,   0x0E, 1,      DT_ARRAY,                 sizeof(OOP_PORTS_TI_T),          E_NULL},                     //端口超时响应时间
    {{0x6E030300}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //端口数据共享功能
    {{0x6E050200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //群组档案集
    {{0x6E050300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //群组档案记录数
    {{0x6E060200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_GROUP_METER_T),       E_NULL},                     //一个群组档案
    {{0x6E080200}, 8,      0x06,   0x0E, 1,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //上一次接收到主站通信报文时间
    {{0x6E110200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //全局透明任务方案
    {{0x6E110300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //全局透明任务方案统计
    {{0x6E120200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_T),        E_NULL},                     //一个全局透明任务方案
    {{0x6E130200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //全局透明具体任务
    {{0x6E140200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_NODE_T),   E_NULL},                     //一个全局透明具体任务
    {{0x6E150200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //福建透明方案结果集合
    {{0x6E160200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_RESULT_T), E_NULL},                     //一个福建透明方案结果
    {{0x6E170200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //主动上报方案集【福建】
    {{0x6E180200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_REPORT_T),      E_NULL},                     //一个主动上报方案集【福建】
    {{0x6E200200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //透明任务方案采集状态集
    {{0x6E210200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,         sizeof(OOP_TRANS_TASK_RLUESTATE_T),  E_NULL},                     //一个透明任务方案采集状态
    {{0x6E220200}, 8,      0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //通道采集状态
    {{0x6E230200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //采集状态【福建】
    {{0x6E240200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,            sizeof(OOP_TRANS_ACQ_RLUESTATE_T),E_NULL},                     //一个采集状态【福建】
    {{0x6E250200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_STORE_SIZE_T),        E_NULL},                     //存储空间使用情况
    {{0x6E310200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //福建透明事件结果集合
    {{0x6E310300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //福建透明事件记录状态
    {{0x6E320200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_EVENT_T),       E_NULL},                     //一个福建透明事件结果
    {{0x6E330200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_EVENT_REPORT_T),E_NULL},                     //主动上报事件上报策略
};

const uint32 oadCollectInfoListLen = sizeof(oadCollectInfoList)/sizeof(oadCollectInfoList[0]);

/* 集合类OAD列表 OIA1 = 0x07 */
const OAD_TAB_T oadGatherInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x70000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //文件集合 - 当前元素个数
    {{0x70000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //文件集合 - 最大元素个数
    {{0x70100300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //脚本集合 - 当前元素个数
    {{0x70100400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //脚本集合 - 最大元素个数
    {{0x70120300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //脚本执行结果集 - 当前元素个数
    {{0x70120400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //脚本执行结果集 - 最大元素个数
    {{0x71000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //扩展变量对象集合 - 当前元素个数
    {{0x71000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //扩展变量对象集合 - 最大元素个数
    {{0x71010300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //扩展参变量对象集合 - 当前元素个数
    {{0x71010400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //扩展参变量对象集合 - 最大元素个数
    {{0x73000200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_CONTAINER_LIST},      //容器集合
    {{0x73010200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //应用集合
    {{0x73010500}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //应用集合
    {{0x73000200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_CONTAINER_LIST},      //容器集合
    {{0x73010200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //应用集合
    {{0x73010300}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //许可信息
    {{0x73010500}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //许可信息
};
const uint32 oadGatherInfoListLen = sizeof(oadGatherInfoList)/sizeof(oadGatherInfoList[0]);

/* 控制接口类OAD列表 OIA1 = 0x08 */
const OAD_TAB_T oadControlInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x80000200}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TELECON_PARAM_T),     E_OOP_TELECON_PARAM_T},      //远程控制 - 配置参数

#ifdef PRODUCT_ZCU_1
    {{0x80000400}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),          E_OOP_BITSTR16_T},           //远程控制 - 告警状态
    {{0x80000500}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),          E_OOP_BITSTR16_T},           //远程控制 - 命令状态
#else
    {{0x80000400}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),           E_OOP_BITSTR8_T},            //远程控制 - 告警状态
    {{0x80000500}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),           E_OOP_BITSTR8_T},            //远程控制 - 命令状态
#endif

    {{0x80010200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_UNSIGNED},                 //保电状态
    {{0x80010300}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //允许与主站最大无通信时长（分钟），0表示不自动保电
    {{0x80010400}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //上电自动保电时长（分钟），0表示上电不自动保电。
    {{0x80010500}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_PRTTIME_T),           E_OOP_PRTTIME_T},            //自动保电时段
    {{0x80020200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_UNSIGNED},                 //催费告警状态，只读
    {{0x80020300}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CALLFEE_T),           E_OOP_CALLFEE_T},            //催费告警参数，只读
    {{0x80030200}, 11,     0x08,   0x00, 0,      DT_ARRAY,                 sizeof(CHINESEINFO),             E_CHINESEINFO},              //一般中文信息
    {{0x80040200}, 11,     0x08,   0x00, 0,      DT_ARRAY,                 sizeof(CHINESEINFO),             E_CHINESEINFO},              //重要中文信息
    {{0x81000200}, 8,      0x08,   0x01, 0,      DT_LONG64,                sizeof(int64),                   E_LONG64},                   //终端保安定值 
    {{0x81010200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_TIME_T),       E_OOP_POWCON_TIME_T},        //终端功控时段 
    {{0x81020200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_WARNING_T),    E_OOP_POWCON_WARNING_T},     //功控告警时间
    {{0x81030200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8103_CONFIG_T),       E_OOP_8103_CONFIG_T},        //时段功控  
    {{0x81030300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81030400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81030500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态 
    {{0x81040200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8104_CONFIG_T),       E_OOP_8104_CONFIG_T},        //厂休控  
    {{0x81040300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81040400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81040500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态  
    {{0x81050200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8105_CONFIG_T),       E_OOP_8105_CONFIG_T},        //营业报停控 
    {{0x81050300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81050400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81050500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态  
    {{0x81060300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81060400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81060500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态 
    {{0x81070200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8107_CONFIG_T),       E_OOP_8107_CONFIG_T},        //购电控   
    {{0x81070300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81070400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81070500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态 
    {{0x81080200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8108_CONFIG_T),       E_OOP_8108_CONFIG_T},        //月电控 
    {{0x81080300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制投入状态
    {{0x81080400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //轮次状态
    {{0x81080500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //控制告警状态 
    {{0x810C0200}, 8,      0x08,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_CFGUNIT_810C_T),      E_OOP_GDK_PARAM_T},          //购电控配置单元 
    {{0x810E0200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(uint16),                  E_NULL},           //控制对象 
    {{0x810F0200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_BITSTR8_T),           E_NULL},           //跳闸轮次 
    {{0x81100200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(int64),                   E_NULL},           //电控定值 
};
const uint32 oadControlInfoListLen = sizeof(oadControlInfoList)/sizeof(oadControlInfoList[0]);

/* 文件传输接口类OAD列表 OIA1 = 0x0F OIA2 = 0x00 */
const OAD_TAB_T oadFileInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF0000200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //文件分帧传输管理- 文件信息
    {{0xF0000300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //文件分帧传输管理 - 命令结果
    {{0xF0000400}, 18,     0x0F,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),       E_OOP_OCTETVAR1K_T},        //文件分帧传输管理 - 文件内容
    {{0xF0000500}, 18,     0x0F,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                 E_DOUBLE_LONG_UNSIGNED},    //文件分帧传输管理 - 当前指针
    {{0xF0010200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //文件分块传输管理 - 文件信息
    {{0xF0010300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //文件分块传输管理 - 命令结果
    {{0xF0010400}, 18,     0x0F,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),          E_OOP_BITSTR8_T},           //文件分块传输管理 - 传输块状态字
    {{0xF0020200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //文件扩展传输管理 - 文件信息
    {{0xF0020300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //文件扩展传输管理 - 命令结果
    {{0xF0020400}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FTPSERVER_T),        E_OOP_FTPSERVER_T},         //文件扩展传输管理 - 服务器信息
};
const uint32 oadFileInfoListLen = sizeof(oadFileInfoList)/sizeof(oadFileInfoList[0]);


/* ESAM接口类OAD列表 OIA1 = 0x0F OIA2 = 0x01 */
const OAD_TAB_T oadEsamInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF1000200}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //ESAM序列号
    {{0xF1000300}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //ESAM版本号
    {{0xF1000400}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //对称密钥版本
    {{0xF1000500}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //会话时效门限
    {{0xF1000600}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //会话时效剩余时间
    {{0xF1000700}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_CUR_COUNT_T),          E_OOP_CUR_COUNT_T},          //当前计数器(没找到对应的结构体)
    {{0xF1000800}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_LICENSEVER_T),         E_OOP_LICENSEVER_T},         //证书版本
    {{0xF1000900}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //终端证书序列号
    {{0xF1000A00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR2K_T),         E_OOP_OCTETVAR2K_T},         //终端证书
    {{0xF1000B00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //主站证书序列号
    {{0xF1000C00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR2K_T),         E_OOP_OCTETVAR2K_T},         //主站证书
    {{0xF1000D00}, 21,     0x0F,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_OADS_T),               E_OOP_OADS_T},               //ESAM 安全存储对象列表
    {{0xF1000E00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //红外认证时效门限
    {{0xF1000F00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //红外认证剩余时间
    {{0xF1010200}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SECURITYFLAG_E),       E_ENUM},                     //安全模式开关 
    {{0xF1010300}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SCURITYMODE_T),        E_OOP_SCURITYMODE_T},        //全模式参数 
    {{0xF1001100}, 21,     0x0F,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},
    {{0xF1001500}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_TERMINAL_LIMIT_T),     E_OOP_TERMINAL_LIMIT_T},                   //终端会话门限
    {{0xF1010400}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //SAL安全应用数据链路层参数 
    {{0xF4000200}, 8,      0x0F,   0x04, 0,      DT_ARRAY,                 sizeof(OOP_LOG_INFO_T),           E_OOP_LOG_INFO_T},           //服务组件
    {{0xF4060200}, 8,      0x0F,   0x04, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //安全加固标识信息
};
const uint32 oadEsamInfoListLen = sizeof(oadEsamInfoList)/sizeof(oadEsamInfoList[0]);

/* 输入输出类OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
const OAD_TAB_T oadIoInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF2000200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SERIALPORT_T),         E_OOP_SERIALPORT_T},         //RS232列表参数
    {{0xF2000300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //RS232设备对象数量
    {{0xF2010200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RS485_T),              E_OOP_RS485_T},              //RS485列表参数
    {{0xF2010300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //RS485设备对象数量
    {{0xF2020200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_IR_T),                 E_OOP_IR_T},                 //红外列表参数
    {{0xF2020300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //红外设备对象数量
    {{0xF2030200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_STATE_T),     E_OOP_SWITCHIN_STATE_T},     //信号量列表参数
    {{0xF2030300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //信号量设备对象数量
    {{0xF2030400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T),      E_OOP_SWITCHIN_FLAG_T},      //信号量接入属性
    {{0xF2030600}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_STATUS_SIGNAL_T),      E_OOP_SWITCHIN_FLAG_T},      //信号节点
    {{0xF2031800}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T),      E_OOP_SWITCHIN_FLAG_T},      //信号量接入属性
    {{0xF2040200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_T),             E_OOP_ANALOG_T},             //直流模拟量
    {{0xF2040300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //直流模拟量设备对象数量
    {{0xF2040400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_CONFIG_T),      E_OOP_ANALOG_CONFIG_T},      //直流模拟量配置
    {{0xF2050200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHOUT_T),          E_OOP_SWITCHOUT_T},          //继电器
    {{0xF2050300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //继电器设备对象数量
    {{0xF2060200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGOUT_T),         E_OOP_WARNINGOUT_T},         //告警输出
    {{0xF2060300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //告警设备对象数量
    {{0xF2060400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGTIME_T),        E_OOP_WARNINGTIME_T},        //允许告警时段
    {{0xF2070200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_MUTIFUNCTERMINAL_T),   E_OOP_MUTIFUNCTERMINAL_T},   //多功能端子
    {{0xF2070300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //多功能端子设备对象数量
    {{0xF2080200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_AC_T),                 E_OOP_AC_T},                 //交采接口
    {{0xF2080300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //交采设备对象数量   
    {{0xF2090200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ROUTEMODULE_T),        E_OOP_ROUTEMODULE_T},        //载波/微功率无线接口，本地通信模块单元参数
    {{0xF2090300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //载波/微功率无线接口设备对象数量
    {{0xF2090500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_CHILDNODE_T),          E_OOP_CHILDNODE_T},          //载波/微功率无线接口，从节点对象列表
    {{0xF2090600}, 22,     0x0F,   0x02, 0,      DT_TI,                    sizeof(OOP_TI_T),                 E_OOP_TI_T},                 //载波/微功率无线接口，从节点对象列表更新周期
    {{0xF2090700}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_NETTOPOLOGYINFO_LIST_T), E_OOP_NETTOPOLOGYINFO_T},
    {{0xF2090800}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_NETSINFO_T),           E_OOP_NETSINFO_T},
    {{0xF2090900}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},
    {{0xF2090A00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_NODEPHASEINFO_T),      E_OOP_NODEPHASEINFO_T},
    {{0xF2090B00}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},
    {{0xF2090C00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_NODEVERSIONINFO_T),    E_NULL},
    {{0xF2091400}, 22,     0x0F,   0x02, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                   E_LONG_UNSIGNED},
    {{0xF2091500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_REFUSESLAVE_T),        E_OOP_REFUSESLAVE_T},
    {{0xF2091600}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},
    {{0xF2091E00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_NETBASEINFO_T),        E_OOP_NETBASEINFO_T},
    {{0xF2091F00}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_NETNODEINFO_T),        E_OOP_NETNODEINFO_T},
    {{0xF20A0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_PULSE_T),              E_OOP_PULSE_T},              //脉冲输入端口描述
    {{0xF20A0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //脉冲输入设备对象数量
    {{0xF20B0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BLUETOOTHMODS_T),      E_ARRAY},                  //蓝牙模块,目前没有对应结构体
    {{0xF20B0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //蓝牙模块设备对象数量
    {{0xF20B0400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BLUETOOTHSLAVES_T),    E_ARRAY},                    //从设备列表
    {{0xF20B0500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BTCONNECTINFO_T),      E_ARRAY},                    //连接信息
    {{0xF20B0600}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BTWORKPARAMS_T),       E_ARRAY},                    //工作参数
    {{0xF20B0B00}, 22,     0x0F,   0x02, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //SN
    {{0xF20C0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_T),              E_OOP_RADIO_T},              //230无线专网模块
    {{0xF20C0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //230无线专网模块设备对象数量
    {{0xF20C0400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_CHS_T),          E_OOP_RADIO_CHS_T},          //频道设置
    {{0xF20C0500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_SIGS_T),         E_OOP_RADIO_SIGS_T},         //有效信号强度
    {{0xF20D0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 300,                              E_NULL},                     //模组
    {{0xF20E0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_USB_T),                E_OOP_USB_T},                //USB设备
    {{0xF20E0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //USB设备对象数量
    {{0xF20F0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_REG_APP_INFOS_T),      E_NULL},                     //服务组件
};
const uint32 oadIoInfoListLen = sizeof(oadIoInfoList)/sizeof(oadIoInfoList[0]);

/* 显示类OAD列表 OIA1 = 0x0F OIA2 = 0x03 */
const OAD_TAB_T oadGuiList[] = 
{
    {{0xf3000300},  17,  0x0F,   0x03, 0,DT_LONG_UNSIGNED,        sizeof(uint16),           E_LONG_UNSIGNED },   // 自动轮显每屏显示时间
    {{0xf3000400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(uint16),           E_OOP_OBJ_SHOW_PARAM_T },   // 按键显示屏数
    {{0xf3010200},  17,  0x0F,   0x03,0,DT_ARRAY,                   sizeof(OOP_OBJ_SHOW_T),   E_OOP_OBJ_SHOW_T },   // 显示内容
    {{0xf3010300},  17,  0x0F,   0x03,0,DT_LONG_UNSIGNED,            sizeof(uint16),            E_LONG_UNSIGNED },   // 按键轮显每屏显示时间
    {{0xf3010400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(OOP_OBJ_SHOW_PARAM_T),   E_OOP_OBJ_SHOW_PARAM_T },   // 循环显示屏数
};
const uint32 oadGuiListLen = sizeof(oadGuiList)/sizeof(oadGuiList[0]);


/* 自定义OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
const OAD_TAB_T oadSelfInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xFFF00400}, 8,      0x0F,   0x0F, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //厂家自定义参数
    {{0xFFF00600}, 8,      0x0F,   0x0F, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //厂家自定义参数
    {{0xFFF20400}, 8,      0x0F,   0x0F, 0,      DT_ARRAY,                 sizeof(OOP_INNER_APP_ID_LIST_T),  E_NULL},                     //厂家自定义参数
};
const uint32 oadSelfInfoListLen = sizeof(oadSelfInfoList)/sizeof(oadSelfInfoList[0]);

//
//OMD列表
//

/* 变量类OAD列表 OIA1 = 0x02 */
const OMD_INFO_T omdVariateInfoList[] = 
{
    {{0x24010300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24010400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24011C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)
    
    {{0x24020300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24020400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24021C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24030300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24030400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24031C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24040300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24040400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24041C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24050300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24050400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24051C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24060300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24060400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24061C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24070300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24070400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24071C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

    {{0x24080300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //添加脉冲输入单元
    {{0x24080400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //删除脉冲输入单元
    {{0x24081C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //添加脉冲输入单元(大)

};

const uint32 omdVariateInfoListLen = sizeof(omdVariateInfoList)/sizeof(omdVariateInfoList[0]);


/* 参变量类OMD列表 OIA1 = 0x04 */
const OMD_INFO_T omdParaInfoList[] =
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x40007F00}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //短时间格式 日期-时间
    {{0x40067F00}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},                     //启用 时钟源
    {{0x40068000}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},                     //禁用 时钟源
	{{0x40388000}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},
	{{0x43000300}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //数据初始化
    {{0x43000100}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //终端复位
    {{0x43000400}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //恢复出厂参数
    {{0x43000500}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //事件初始化
    {{0x43000600}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //需量初始化
    {{0x43000700}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //服务授权开启
    {{0x43007F00}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //修改液晶设置密码
    {{0x45000100}, 25,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //重播
    {{0x45010100}, 25,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //重播
    {{0x45100100}, 26,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //重播
    {{0x45110100}, 26,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //重播
};
const uint32 omdParaInfoListLen = sizeof(omdParaInfoList)/sizeof(omdParaInfoList[0]);

/* 采集类OMD列表 OIA1 = 0x06 */
const OMD_INFO_T omdCollectInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x60007F00}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_OOP_METER_T},              //采集档案配置单元
    {{0x60007F01}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //配置序号
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             10,                              E_BASIC_OBJECT},             //基本信息
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_TSA,                   0,                               E_TSA},                      //基本信息 - 通信地址
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 波特率
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 规约类型
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //基本信息 - 端口
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_PWD_COMM},                 //基本信息 - 通信密码
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //基本信息 - 费率个数
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //基本信息 - 用户类型
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //基本信息 - 接线方式
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //基本信息 - 额度电压
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //基本信息 - 额度电流
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_EXTENDED_OBJECT},          //扩展信息
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_TSA,                   0,                               E_TSA},                      //扩展信息 - 采集器地址
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_ASSETNO},                  //扩展信息 - 资产号
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //扩展信息 - 电压互感器变比
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //扩展信息 - 电流互感器变比
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_ANNEX_OBJECT_LIST},        //附属信息   
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_ANNEX_OBJECT},             //附属信息   
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //附属信息 - 对象属性描述
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //附属信息 - 属性值
    {{0x60008000}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_METER_LIST},           //下发多个档案单元  
    {{0x60008100}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_BASIC_OBJECT},             //更新档案单元 
    {{0x60008200}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             3,                               E_EXTENDED_OBJECT},          //更新档案单元      
    {{0x60008300}, 11,     0x06,   0x00, 0,      DT_UNSIGNED,              1,                               E_UNSIGNED},                 //删除档案  
    {{0x60008400}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             11,                              E_EXTENDED_OBJECT},          //删除档案
    {{0x60008500}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_METER_M133_T},             //删除档案
    {{0x60008600}, 11,     0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //清空档案                                
    {{0x60008700}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             0,                               E_DATA},                     //清空档案                                
    {{0x60020200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_SEARCHMET_RES_LIST},   //所有搜表结果
    {{0x60030200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             7,                               E_OOP_SEARCHMET_RES_T},
    {{0x60030201}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030202}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030203}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030204}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030205}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030206}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},

    {{0x60027F00}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //启动搜表
    {{0x60028000}, 11,     0x06,   0x00, 0,      DT_NULL,                  sizeof(uint16),                  E_NULL},                     //清空搜表结果
    {{0x60028100}, 11,     0x06,   0x00, 0,      DT_NULL,                  sizeof(uint16),                  E_NULL},                     //清空跨台区搜表结果

    {{0x60127F00}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_TASK_LIST},            //添加一组任务配置表
    {{0x60128000}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //删除一组配置单元
    {{0x60128100}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //清空任务配置表
    {{0x60128200}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //更新任务配置表
    {{0x60147F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_NORMAL_T},        //添加一组采集档案
    {{0x60148000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //删除一组采集档案
    {{0x60148100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //清空采集档案
    {{0x60148200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //重置方案的记录列选择
    {{0x60160200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_EVENT_T),        E_OOP_PLAN_EVENT_T},         //事件采集方案集
    {{0x60167F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_EVENT_T},         //事件采集方案集
    {{0x60168000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //删除事件采集方案
    {{0x60168100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //删除事件采集方案
    {{0x60168200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //更新事件采集方案
   
    {{0x60180200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_TRANS_T),        E_OOP_PLAN_TRANS_T},         //透明方案集
    {{0x60187F00}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},
    {{0x60188000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL}, 
    {{0x60188300}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL}, 
    {{0x601A0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TRANSRES_T),          E_OOP_TRANSRES_T},           //透明方案结果集
    {{0x601C0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_REPORT_T),       E_OOP_PLAN_REPORT_T},        //上报方案集
    {{0x601C7F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_REPORT_T},        //增加上报方案
    {{0x601C8000}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //删除上报方案
    {{0x601C8100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //清空上报方案
    {{0x601E0200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //采集规则库
    {{0x601E7F00}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //添加或更新一组转加密方案。
    {{0x601E8000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //删除一组转加密方案
    {{0x601E8100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //清空转加密方案集    
    {{0x60320200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_RLUESTATE_T),     E_OOP_ACQ_RLUESTATE_T},      //采集状态集
    {{0x60340200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T),       E_OOP_ACQ_MONITOR_T},        //采集任务监控集

    {{0x6e057f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //添加一组群组档案
    {{0x6e058000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //按群组地址和具体地址删除档案
    {{0x6e058200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //清空群组档案集
    {{0x6e058300}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //删除所有群组档案中存在该地址的清单
    {{0x6e087f00},  8,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //链路测试

    {{0x6e117f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //添加或更新一组透明任务方案单元
    {{0x6e118000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //按任务方案号删除一组透明任务方案单元、具体任务配置及其数据记录
    {{0x6e118100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //清空透明任务方案配置表、具体任务配置及其数据记录。
    {{0x6e118200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //Update

    {{0x6e137f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //添加更新一个或一组透明具体任务
    {{0x6e138000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //按透明任务方案号和地址删除一组透明具体任务
    {{0x6e138100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //按透明任务方案号删除一组透明具体任务
    {{0x6e138200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //清空透明具体任务集
    {{0x6e138300}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //Delete

    {{0x6e177f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //添加或更新一组主动上报方案单元
    {{0x6e178000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //删除一组上报方案单元
    {{0x6e178100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //清空上报方案集
    {{0x6e178400}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //补报一次指定的未成功上报

    {{0x6e317f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //清空事件记录
    {{0x6e318000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //补报一次未成功上报的事件记录
};  
const uint32 omdCollectInfoListLen = sizeof(omdCollectInfoList)/sizeof(omdCollectInfoList[0]);

/* 参变量、集合类OMD列表 OIA1 = 0x07 */
const OMD_INFO_T omdGatherInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x73007F00}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73008000}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73008100}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73008200}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73008300}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73017f00}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018000}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018000}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018100}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018200}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018300}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018400}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73018500}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x73018600}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},
    {{0x73018700}, 11,     0x07,   0x03, 0,      DT_STRUCTURE,             0,                               E_NULL},              //
    {{0x70008300}, 11,     0x07,   0x00, 0,      DT_STRUCTURE,             0,                               E_NULL}, 
    {{0x70008100}, 11,     0x07,   0x00, 0,      DT_STRUCTURE,             0,                               E_NULL}, 
    {{0x73028100}, 11,     0x07,   0x00, 0,      DT_STRUCTURE,             0,                               E_NULL}, 
};
const uint32 omdGatherListLen = sizeof(omdGatherInfoList)/sizeof(omdGatherInfoList[0]);


/* 控制接口类OMD列表 OIA1 = 0x08 */
const OMD_INFO_T omdControlInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x80007F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //触发告警（参数）
    {{0x80008000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //解除报警（参数）
    {{0x80008100}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TELECON_TRIP_T),      E_NULL},                     //遥控跳闸
    {{0x80008200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TELECON_TRIP_T),      E_NULL},                     //遥控合闸
    {{0x80017F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //投入保电
    {{0x80018000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //解除保电
    {{0x80018100}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},    
    {{0x80027F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //催费告警投入
    {{0x80028000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //催费告警取消
    {{0x80037F00}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //添加一般中文信息
    {{0x80038000}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //删除一般中文信息
    {{0x80047F00}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //添加重要中文信息
    {{0x80048000}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //删除重要中文信息
    {{0x81030300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81030400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81030500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81030600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //时段控投入
    {{0x81030700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //时段控取消
    {{0x81037F00}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //解除自动保电
    {{0x81040300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81040400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81040500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81040600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //厂休控投入
    {{0x81040700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //厂休控取消
    {{0x81050300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81050400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81050500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81050600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //营业控投入
    {{0x81050700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //营业控取消
    {{0x81060300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81060400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81060500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81060600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //下浮控投入
    {{0x81060700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //下浮控取消
    {{0x81067f00}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //下浮控投入(参数)
    {{0x81070300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81070400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81070500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81070600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //购电控投入
    {{0x81070700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //购电控取消
    {{0x81080300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案添加
    {{0x81080400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案删除
    {{0x81080500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //方案更新
    {{0x81080600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //月电控投入
    {{0x81080700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //月电控取消
};
const uint32 omdControlInfoListLen = sizeof(omdControlInfoList)/sizeof(omdControlInfoList[0]);

/* 文件传输接口类OMD列表 OIA1 = 0x0F OIA2 = 0x00 */
const OMD_INFO_T omdFileInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF0007F00}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010400}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010500}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010700}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},                                         //启动传输
    {{0xF0010800}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},        //写文件（参数）
    {{0xF0010900}, 18,	   0x0F,   0x00, 0, 	 DT_STRUCTURE,			   0,			  E_NULL},		  //读文件（参数）
    {{0xF0020700}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
};
const uint32 omdFileInfoListLen = sizeof(omdFileInfoList)/sizeof(omdFileInfoList[0]);

/* ESAM接口类OMD列表 OIA1 = 0x0F OIA2 = 0x01 */
const OMD_INFO_T omdEsamInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF1010100}, 8,     0x0F,   0x01, 0,      DT_INTEGER,               sizeof(uint8),                     E_INTEGER},                   //复位时，清空属性3
    {{0xF1017F00}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},              //增加显式安全模式参数（参数）
    {{0xF1018000}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},         //删除显式安全模式参数（对象标识）
    {{0xF1018100}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},         //显式安全模式参数（对象标识）
    
    {{0xF1000700}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_7_T),            E_OOPM_ESAM_7_T},               //密钥更新
    {{0xF1000800}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_8_T),            E_OOPM_ESAM_8_T},               //密钥更新
    {{0xF1000900}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_8_T),            E_OOPM_ESAM_8_T},               //参数更新
    {{0xF4007F00}, 8,     0x0F,   0x04, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR256_T),      E_VISIBLE_STRING},   
};
const uint32 omdEsamInfoListLen = sizeof(omdEsamInfoList)/sizeof(omdEsamInfoList[0]);

/* 输入输出类OMD列表 OIA1 = 0x0F OIA2 = 0x02 */
const OMD_INFO_T omdIoInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF2000300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //RS232端口授权
    {{0xF2007F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SERIALPORT_CONFIG_T), E_OOPM_SERIALPORT_CONFIG_T},         //RS232配置参数
    {{0xF2010300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //RS485端口授权
    {{0xF2017F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SERIALPORT_CONFIG_T), E_OOPM_SERIALPORT_CONFIG_T},         //RS485配置参数
    {{0xF2020300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //IR端口授权
    {{0xF2027F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_IR_CONFIG_T),         E_OOPM_IR_CONFIG_T},                 //红外配置参数
    {{0xF2057F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SWITCHOUT_127_T),     E_OOPM_SWITCHOUT_127_T},             //开关量配置参数
    {{0xF2077F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_MULTIOUT_CONFIG_T),   E_OOPM_MULTIOUT_CONFIG_T},           //多功能端子修改工作模式
    {{0xF2097F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_TRANSCMD_T),          E_OOPM_TRANSCMD_T},                  //透明转发（参数）
    {{0xF2098000}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_PORTPARAM_T),         E_OOPM_PORTPARAM_T},                 //配置端口参数（参数）
    {{0xF20E0300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //USB端口授权
    {{0xF20B7F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHCFG_T),      E_STRUCTURE},                        //蓝牙配置端口
    {{0xF20B8000}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHERRCHK_T),   E_STRUCTURE},                        //蓝牙误差检定
    {{0xF20B8100}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHPASS_T),     E_STRUCTURE},                        //蓝牙更新配对密码
};
const uint32 omdIoInfoListLen = sizeof(omdIoInfoList)/sizeof(omdIoInfoList[0]);

/* 显示类OAD列表 OIA1 = 0x0F OIA2 = 0x03 */
const OMD_INFO_T omdGuiList[] = 
{
    {{0xf3000300},  17,  0x0F,   0x03, 0,DT_LONG_UNSIGNED,        sizeof(uint16),           E_LONG_UNSIGNED },   // 自动轮显每屏显示时间
    {{0xf3000400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(uint16),           E_OOP_OBJ_SHOW_PARAM_T },   // 按键显示屏数
    {{0xf3010200},  17,  0x0F,   0x03,0,DT_ARRAY,                   sizeof(OOP_OBJ_SHOW_T),   E_OOP_OBJ_SHOW_T },   // 显示内容
    {{0xf3010300},  17,  0x0F,   0x03,0,DT_LONG_UNSIGNED,            sizeof(uint16),            E_LONG_UNSIGNED },   // 按键轮显每屏显示时间
    {{0xf3010400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(OOP_OBJ_SHOW_PARAM_T),   E_OOP_OBJ_SHOW_PARAM_T },   // 循环显示屏数
};
const uint32 omdGuiListLen = sizeof(omdGuiList)/sizeof(omdGuiList[0]);

/* 自定义OMD列表 OIA1 = 0x0F OIA2 = 0x0F */
const OMD_INFO_T omdSelfInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xFFF06600},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //解除安全模式（参数）
    {{0xFFF0D300},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //全体参数初始化（参数）
    {{0xFFF0D400},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //全体数据初始化（参数）
};
const uint32 omdSelfInfoListLen = sizeof(omdSelfInfoList)/sizeof(omdSelfInfoList[0]);

/* 总加组类OMD列表 OIA1 = 0x02 OIA2 = 0x03 */
const OMD_INFO_T omdTgInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x23010100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组1-清空总加配置表
    {{0x23010300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组1-添加一个总加组配置单元
    {{0x23010400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组1-批量添加总加组配置单元
    {{0x23010500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组1-删除一个总加组配置单元
    {{0x23020100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组2-清空总加配置表
    {{0x23020300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组2-添加一个总加组配置单元
    {{0x23020400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组2-批量添加总加组配置单元
    {{0x23020500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组2-删除一个总加组配置单元
    {{0x23030100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组3-清空总加配置表
    {{0x23030300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组3-添加一个总加组配置单元
    {{0x23030400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组3-批量添加总加组配置单元
    {{0x23030500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组3-删除一个总加组配置单元
    {{0x23040100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组4-清空总加配置表
    {{0x23040300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组4-添加一个总加组配置单元
    {{0x23040400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组4-批量添加总加组配置单元
    {{0x23040500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组4-删除一个总加组配置单元
    {{0x23050100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组5-清空总加配置表
    {{0x23050300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组5-添加一个总加组配置单元
    {{0x23050400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组5-批量添加总加组配置单元
    {{0x23050500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组5-删除一个总加组配置单元
    {{0x23060100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组6-清空总加配置表
    {{0x23060300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组6-添加一个总加组配置单元
    {{0x23060400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组6-批量添加总加组配置单元
    {{0x23060500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组6-删除一个总加组配置单元
    {{0x23070100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组7-清空总加配置表
    {{0x23070300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组7-添加一个总加组配置单元
    {{0x23070400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组7-批量添加总加组配置单元
    {{0x23070500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组7-删除一个总加组配置单元
    {{0x23080100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组8-清空总加配置表
    {{0x23080300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组8-添加一个总加组配置单元
    {{0x23080400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组8-批量添加总加组配置单元
    {{0x23080500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //总加组8-删除一个总加组配置单元
};
const uint32 omdTgInfoListLen = sizeof(omdTgInfoList)/sizeof(omdTgInfoList[0]);

/* 事件类OMD列表 OIA1 = 0x03 */
const OMD_INFO_T omdEvtInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x300F0100}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电压逆相序事件-复位
    {{0x300F0400}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电压逆相序事件-添加一个事件关联对象属性
    {{0x300F0500}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电压逆相序事件-删除一个事件关联对象属性

    {{0x31000100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端初始化事件-复位
    {{0x31000400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端初始化事件-添加一个事件关联对象属性
    {{0x31000500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端初始化事件-删除一个事件关联对象属性

    {{0x31010100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端版本变更事件-复位
    {{0x31010400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端版本变更事件-添加一个事件关联对象属性
    {{0x31010500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端版本变更事件-删除一个事件关联对象属性

    {{0x31040100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端状态量变位事件-复位
    {{0x31040400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端状态量变位事件-添加一个事件关联对象属性
    {{0x31040500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端状态量变位事件-删除一个事件关联对象属性

    {{0x31050100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表时钟超差事件-复位
    {{0x31050400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表时钟超差事件-添加一个事件关联对象属性
    {{0x31050500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表时钟超差事件-删除一个事件关联对象属性

    {{0x31060100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端停/上电事件-复位
    {{0x31060400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端停/上电事件-添加一个事件关联对象属性
    {{0x31060500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端停/上电事件-删除一个事件关联对象属性

    {{0x310A0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //设备故障记录-复位
    {{0x310A0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //设备故障记录-添加一个事件关联对象属性
    {{0x310A0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //设备故障记录-删除一个事件关联对象属性

    {{0x310B0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表示度下降事件-复位
    {{0x310B0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表示度下降事件-添加一个事件关联对象属性
    {{0x310B0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表示度下降事件-删除一个事件关联对象属性

    {{0x310C0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能量超差事件-复位
    {{0x310C0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能量超差事件-添加一个事件关联对象属性
    {{0x310C0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能量超差事件-删除一个事件关联对象属性

    {{0x310D0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表飞走事件-复位
    {{0x310D0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表飞走事件-添加一个事件关联对象属性
    {{0x310D0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表飞走事件-删除一个事件关联对象属性

    {{0x310E0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表停走事件-复位
    {{0x310E0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表停走事件-添加一个事件关联对象属性
    {{0x310E0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表停走事件-删除一个事件关联对象属性

    {{0x310F0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端抄表失败事件-复位
    {{0x310F0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端抄表失败事件-添加一个事件关联对象属性
    {{0x310F0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端抄表失败事件-删除一个事件关联对象属性

    {{0x31100100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //月通信流量超限事件-复位
    {{0x31100400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //月通信流量超限事件-添加一个事件关联对象属性
    {{0x31100500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //月通信流量超限事件-删除一个事件关联对象属性

    {{0x31110100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //发现未知电能表事件-复位
    {{0x31110400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //发现未知电能表事件-添加一个事件关联对象属性
    {{0x31110500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //发现未知电能表事件-删除一个事件关联对象属性

    {{0x31140100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端对时事件-复位
    {{0x31140400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端对时事件-添加一个事件关联对象属性
    {{0x31140500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端对时事件-删除一个事件关联对象属性

    {{0x31180100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端编程记录-复位
    {{0x31180400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端编程记录-添加一个事件关联对象属性
    {{0x31180500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端编程记录-删除一个事件关联对象属性

    {{0x31190100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端电流回路异常事件-复位
    {{0x31190400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端电流回路异常事件-添加一个事件关联对象属性
    {{0x31190500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //终端电流回路异常事件-删除一个事件关联对象属性

    {{0x311C0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表数据变更监控记录-复位
    {{0x311C0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表数据变更监控记录-添加一个事件关联对象属性
    {{0x311C0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //电能表数据变更监控记录-删除一个事件关联对象属性
};
const uint32 omdEvtInfoListLen = sizeof(omdEvtInfoList)/sizeof(omdEvtInfoList[0]);

/* 冻结类OMD列表 OIA1 = 0x05 */
const OMD_INFO_T omdFrozenInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x50010100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //秒冻结-复位
    {{0x50010400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //秒冻结-添加一个冻结对象属性
    {{0x50010500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //秒冻结-删除一个冻结对象属性
    {{0x50010700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //秒冻结-批了添加冻结对象属性
    {{0x50010800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //秒冻结-清除冻结对象属性表

    {{0x50020100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //分钟冻结-复位
    {{0x50020400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //分钟冻结-添加一个冻结对象属性
    {{0x50020500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //分钟冻结-删除一个冻结对象属性
    {{0x50020700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //分钟冻结-批了添加冻结对象属性
    {{0x50020800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //分钟冻结-清除冻结对象属性表

    {{0x50030100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //小时冻结-复位
    {{0x50030400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //小时冻结-添加一个冻结对象属性
    {{0x50030500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //小时冻结-删除一个冻结对象属性
    {{0x50030700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //小时冻结-批了添加冻结对象属性
    {{0x50030800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //小时冻结-清除冻结对象属性表

    {{0x50040100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //日冻结-复位
    {{0x50040400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //日冻结-添加一个冻结对象属性
    {{0x50040500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //日冻结-删除一个冻结对象属性
    {{0x50040700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //日冻结-批了添加冻结对象属性
    {{0x50040800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //日冻结-清除冻结对象属性表

    {{0x50060100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //月冻结-复位
    {{0x50060400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //月冻结-添加一个冻结对象属性
    {{0x50060500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //月冻结-删除一个冻结对象属性
    {{0x50060700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //月冻结-批了添加冻结对象属性
    {{0x50060800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //月冻结-清除冻结对象属性表

    {{0x50070100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //年冻结-复位
    {{0x50070400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //年冻结-添加一个冻结对象属性
    {{0x50070500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //年冻结-删除一个冻结对象属性
    {{0x50070700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //年冻结-批了添加冻结对象属性
    {{0x50070800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //年冻结-清除冻结对象属性表
};
const uint32 omdFrozenInfoListLen = sizeof(omdFrozenInfoList)/sizeof(omdFrozenInfoList[0]);

/* 统计类OMD列表 OIA1 = 0x02 */
const OMD_INFO_T omdStatInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x21000300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //分区间统计-添加一个统计对象
    {{0x21000400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //分区间统计-删除一个统计对象
    {{0x21010300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //时区间统计-添加一个统计对象
    {{0x21010400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //时区间统计-删除一个统计对象
    {{0x21020300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //日区间统计-添加一个统计对象
    {{0x21020400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //日区间统计-删除一个统计对象
    {{0x21030300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //月区间统计-添加一个统计对象
    {{0x21030400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //月区间统计-删除一个统计对象
    {{0x21040300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //年区间统计-添加一个统计对象
    {{0x21040400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //年区间统计-删除一个统计对象

    {{0x21100300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //分平均统计-添加一个统计对象
    {{0x21100400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //分平均统计-删除一个统计对象
    {{0x21110300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //时平均统计-添加一个统计对象
    {{0x21110400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //时平均统计-删除一个统计对象
    {{0x21120300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //日平均统计-添加一个统计对象
    {{0x21120400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //日平均统计-删除一个统计对象
    {{0x21130300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //月平均统计-添加一个统计对象
    {{0x21130400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //月平均统计-删除一个统计对象
    {{0x21140300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //年平均统计-添加一个统计对象
    {{0x21140400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //年平均统计-删除一个统计对象

    {{0x21200300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //分极值统计-添加一个统计对象
    {{0x21200400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //分极值统计-删除一个统计对象
    {{0x21210300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //时极值统计-添加一个统计对象
    {{0x21210400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //时极值统计-删除一个统计对象
    {{0x21220300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //日极值统计-添加一个统计对象
    {{0x21220400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //日极值统计-删除一个统计对象
    {{0x21230300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //月极值统计-添加一个统计对象
    {{0x21230400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //月极值统计-删除一个统计对象
    {{0x21240300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //年极值统计-添加一个统计对象
    {{0x21240400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //年极值统计-删除一个统计对象
    
};
const uint32 omdStatInfoListLen = sizeof(omdStatInfoList)/sizeof(omdStatInfoList[0]);


//Class1
const aarAccessRight_t Class1AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
};
const marAccessRight_t Class1MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

//Class2
const aarAccessRight_t Class2AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},

    {1,READ},
    {2,READ},
    {3,READ},
};

const marAccessRight_t Class2MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

//Class3
const aarAccessRight_t Class3AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},

    {1,READ},
    {2,READ},
    {3,READ},
};

const aarAccessRight_t Class3AAR_2001[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
};

const marAccessRight_t Class3MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};
//Class4
const aarAccessRight_t Class4AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},

    {1,READ},
    {2,READ},
    {3,READ},
};

const marAccessRight_t Class4MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

//Class5
const aarAccessRight_t Class5AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},
};

const marAccessRight_t Class5MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

//Class6
const aarAccessRight_t Class6AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},

    {1,READ},
    {2,READ},
    {3,READ},
};

const marAccessRight_t Class6MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};


//Class7
const aarAccessRight_t Class7AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READ},
    {5,READ},
    {6,READWRITE},
    {7,READ},
    {8,READWRITE},
    {9,READWRITE},
#if ANHUI_FUN==1
    {11,READWRITE},
#endif
    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READ},
    {5,READ},
    {6,READWRITE},
    {7,READ},
    {8,READWRITE},
    {9,READWRITE},
#if ANHUI_FUN==1
    {11,READWRITE},
#endif

};

const marAccessRight_t Class7MAR[] =
{
    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},


    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
};
//Class8
const aarAccessRight_t Class8AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READWRITE},
    {6,READWRITE},
    {7,READWRITE},
    {8,READWRITE},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READWRITE},
    {6,READWRITE},
    {7,READWRITE},
    {8,READWRITE},
};

const aarAccessRight_t Class8AAR_1[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

const aarAccessRight_t Class8AAR_202F[] =
{
    {1,READ},
    {2,READ},
    {3,READWRITE},

    {1,READ},
    {2,READ},
    {3,READWRITE},
};

const aarAccessRight_t Class8AAR_4000[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
};

const aarAccessRight_t Class8AAR_4204[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
};

const aarAccessRight_t Class8AAR_4401[] =
{
    {1,READ},
    {2,WRITE},

    {1,READ},
    {2,WRITE},
};
const aarAccessRight_t Class8AAR_F101[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
#if SECURITY_DEFEND == 1
    {4,READWRITE},
#endif

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
#if SECURITY_DEFEND == 1
    {4,READWRITE},
#endif
};
const aarAccessRight_t Class8AAR_8000[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},

    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},
};

const aarAccessRight_t Class8AAR_8001[] =
{
    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READWRITE},
    {5,READWRITE},

    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READWRITE},
    {5,READWRITE},
};

const marAccessRight_t Class8MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {102,ACCESS},
    {103,ACCESS},
    {211,ACCESS},
    {212,ACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
    {102,ACCESS},
    {103,ACCESS},
    {211,ACCESS},
    {212,ACCESS},
};
const marAccessRight_t Class8MAR_1[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};


const marAccessRight_t Class8MAR_4000[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//广播校时

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//广播校时
};
const marAccessRight_t Class8MAR_4001[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//ADD
    {128,ACCESS},//DELETE

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//ADD
    {128,ACCESS},//DELETE
};
const marAccessRight_t Class8MAR_4006[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//启用
    {128,ACCESS},//禁用

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//启用
    {128,ACCESS},//禁用
};

const marAccessRight_t Class8MAR_F101[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//增加显式安全模式参数
    {128,ACCESS},//删除显式安全模式参数
    {129,ACCESS},//批量增加显式安全模式参数

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//增加显式安全模式参数
    {128,ACCESS},//删除显式安全模式参数
    {129,ACCESS},//批量增加显式安全模式参数
};

const marAccessRight_t Class8MAR_8000[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,NOACCESS},//
    {128,NOACCESS},//
    {129,ACCESS},//
    {130,ACCESS},//
    {131,ACCESS},//

    {1,NOACCESS},
    {2,NOACCESS},
    {127,NOACCESS},//
    {128,NOACCESS},//
    {129,ACCESS},//
    {130,ACCESS},//
    {131,ACCESS},//
};

const marAccessRight_t Class8MAR_8001[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//
};
const marAccessRight_t Class8MAR_8002[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//
    {128,ACCESS},//

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//
    {128,ACCESS},//
};

//Class9
const aarAccessRight_t Class9AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READWRITE},

    {1,READ},
    {2,READ},
    {3,READWRITE},
    {4,READWRITE},
};

const marAccessRight_t Class9MAR[] =
{
    {1,ACCESS},
    {2,NOACCESS},
    {3,NOACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},

    {1,ACCESS},
    {2,NOACCESS},
    {3,NOACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
};

//Class9
const aarAccessRight_t Class17AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
};

const marAccessRight_t Class17MAR[] =
{
    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},

    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
};


//Class10
const aarAccessRight_t Class10AAR[] =
{
    {1,READ},//逻辑名
    {2,READ},//配置表
    {3,READ},//记录表
    {4,READ},//当前记录数
    {5,READ},//最大记录数

    {1,READ},//逻辑名
    {2,READ},//配置表
    {3,READ},//记录表
    {4,READ},//当前记录数
    {5,READ},//最大记录数
};

const marAccessRight_t Class10MAR[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,NOACCESS},//清空记录表

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,NOACCESS},//清空记录表
};


const marAccessRight_t Class10MAR_6012[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,NOACCESS},//清空记录表
    {127,ACCESS},//添加或更新一组任务配置单元；
    {128,ACCESS},//删除一组配置单元；
    {129,ACCESS},//清空任务配置表；
    {130,ACCESS},//更新任务状态:

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,NOACCESS},//清空记录表
    {127,ACCESS},//添加或更新一组任务配置单元；
    {128,ACCESS},//删除一组配置单元；
    {129,ACCESS},//清空任务配置表；
    {130,ACCESS},//更新任务状态:
};
//Class11
const aarAccessRight_t Class11AAR[] =
{
    {1,READ},//逻辑名
    {2,READWRITE},//集合
    {3,READ},//当前元素个数
    {4,READ},//最大元素个数

    {1,READ},//逻辑名
    {2,READWRITE},//集合
    {3,READ},//当前元素个数
    {4,READ},//最大元素个数
};

const aarAccessRight_t Class11AAR_6002[] ={
    {1,READ},//逻辑名
    {2,READ},//所有搜表结果
    {3,READ},//当前元素个数
    {4,READ},//最大元素个数
    {5,READ},//跨台区搜表结果
    {6,READ},//所有搜表结果记录数
    {7,READ},//跨台区搜表结果记录数
    {8,READWRITE},//
    {9,READWRITE},//每天周期搜表参数配置
    {10,READ},//

    {1,READ},//逻辑名
    {2,READ},//所有搜表结果
    {3,READ},//当前元素个数
    {4,READ},//最大元素个数
    {5,READ},//跨台区搜表结果
    {6,READ},//所有搜表结果记录数
    {7,READ},//跨台区搜表结果记录数
    {8,READWRITE},//
    {9,READWRITE},//每天周期搜表参数配置
    {10,READ},//

};

const marAccessRight_t Class11MAR[] ={
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
};

const marAccessRight_t Class11MAR_4200[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加一个路由信息单元
    {128,ACCESS},//批量添加路由信息单元。
    {129,ACCESS},//更新路由表的路由信息单元对象。
    {134,ACCESS},//清空路由表

    {2,NOACCESS},//执行
    {127,ACCESS},//添加一个路由信息单元
    {128,ACCESS},//批量添加路由信息单元。
    {129,ACCESS},//更新路由表的路由信息单元对象。
    {134,ACCESS},//清空路由表
};
const marAccessRight_t Class11MAR_6000[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加一个采集档案配置单元，的信息中基本必须是完 添加一个采集档案配置单元，的信息中基本必须是完 整的，扩展和附加信息可以为
    {128,ACCESS},//批量添加采集档案配置单元。
    {129,ACCESS},//更新配置单元的基本信息对象
    {130,ACCESS},//更新配置单元的扩展信息以及附属，对象为 更新配置单元的扩展信息以及附属，对象为 NULLNULLNULLNULL表示不更新 表示不更新 。
    {131,ACCESS},//删除配置单元，通过序号
    {132,ACCESS},//删除配置单元，通过基本信息对象
    {133,ACCESS},//删除配置单元，通过信地址及端口
    {134,ACCESS},//清空采集档案配置表

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加一个采集档案配置单元，的信息中基本必须是完 添加一个采集档案配置单元，的信息中基本必须是完 整的，扩展和附加信息可以为
    {128,ACCESS},//批量添加采集档案配置单元。
    {129,ACCESS},//更新配置单元的基本信息对象
    {130,ACCESS},//更新配置单元的扩展信息以及附属，对象为 更新配置单元的扩展信息以及附属，对象为 NULLNULLNULLNULL表示不更新 表示不更新 。
    {131,ACCESS},//删除配置单元，通过序号
    {132,ACCESS},//删除配置单元，通过基本信息对象
    {133,ACCESS},//删除配置单元，通过信地址及端口
    {134,ACCESS},//清空采集档案配置表
};
const marAccessRight_t Class11MAR_6002[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//
};

const marAccessRight_t Class11MAR_6014[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组普通采集方案；
    {128,ACCESS},//删除一组普通采集方案；
    {129,ACCESS},//清空普通采集方案；
    {130,ACCESS},//重置方案的记录列选择
#if (HENAN_FUN==1||GANSU_FUN==1)
    {200,ACCESS},//重置方案的电能表集合。
#endif

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组普通采集方案；
    {128,ACCESS},//删除一组普通采集方案；
    {129,ACCESS},//清空普通采集方案；
    {130,ACCESS},//重置方案的记录列选择
#if (HENAN_FUN==1||GANSU_FUN==1)
    {200,ACCESS},//重置方案的电能表集合。
#endif
};

const marAccessRight_t Class11MAR_6016[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组事件采集方案。
    {128,ACCESS},//删除一组事件采集方案。
    {129,ACCESS},//清空事件采集方案集。
    {130,ACCESS},//UpdateReportFlag

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组事件采集方案。
    {128,ACCESS},//删除一组事件采集方案。
    {129,ACCESS},//清空事件采集方案集。
    {130,ACCESS},//UpdateReportFlag
};

const marAccessRight_t Class11MAR_6018[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加更新一个透明方案或组内容；
    {128,ACCESS},//给一个电能表添加组报文
    {129,ACCESS},//删除一个方案的组内容；
    {130,ACCESS},//删除一组透明方案；
    {131,ACCESS},//清空透明方案集；

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加更新一个透明方案或组内容；
    {128,ACCESS},//给一个电能表添加组报文
    {129,ACCESS},//删除一个方案的组内容；
    {130,ACCESS},//删除一组透明方案；
    {131,ACCESS},//清空透明方案集；
};

const marAccessRight_t Class11MAR_601C[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组上报方案；
    {128,ACCESS},//删除一组上报方案；
    {129,ACCESS},//清空上报方案集；

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组上报方案；
    {128,ACCESS},//删除一组上报方案；
    {129,ACCESS},//清空上报方案集；
};

const marAccessRight_t Class11MAR_601E[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组采集规则；
    {128,ACCESS},//删除一组采集规则。
    {129,ACCESS},//清空采集规则库

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组采集规则；
    {128,ACCESS},//删除一组采集规则。
    {129,ACCESS},//清空采集规则库
};

const marAccessRight_t Class11MAR_6051[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案
    {130,ACCESS},//UpdateReportFlag

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案
    {130,ACCESS},//UpdateReportFlag
};
#if 1
const marAccessRight_t Class11MAR_7000[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案
};
const marAccessRight_t Class11MAR_7010[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案
    {130,ACCESS},//UpdateReportFlag


    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加或更新一组实时监控采集方案。
    {128,ACCESS},//删除一组实时监控采集方案
    {129,ACCESS},//清空实时监控采集方案
    {130,ACCESS},//UpdateReportFlag

};
#endif
const marAccessRight_t Class11MAR_800X[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加一组中文信息
    {128,ACCESS},//删除中文信息

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {127,ACCESS},//添加一组中文信息
    {128,ACCESS},//删除中文信息
};

//Class12
const aarAccessRight_t Class12AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READ},
    {14,READ},
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READ},
    {14,READ},
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},
};

const marAccessRight_t Class12MAR[] =
{
    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},


    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},

};

//Class13
const aarAccessRight_t Class13AAR[] =
{
    {1,READ},//逻辑名
    {2,READWRITE},//控制方案集
    {3,READWRITE},//控制投入状态
    {4,READ},//继电器输出状态
    {5,READ},//越限告警状态

    {1,READ},//逻辑名
    {2,READWRITE},//控制方案集
    {3,READWRITE},//控制投入状态
    {4,READ},//继电器输出状态
    {5,READ},//越限告警状态
};
const aarAccessRight_t Class13AAR_8106[] =
{
    {1,READ},//逻辑名
    {2,NOACCESS},//控制方案集
    {3,READWRITE},//控制投入状态
    {4,READ},//继电器输出状态
    {5,READ},//越限告警状态

    {1,READ},//逻辑名
    {2,NOACCESS},//控制方案集
    {3,READWRITE},//控制投入状态
    {4,READ},//继电器输出状态
    {5,READ},//越限告警状态
};

const marAccessRight_t Class13MAR[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,ACCESS},//添加控制单元
    {4,ACCESS},//删除控制单元
    {5,ACCESS},//更新控制单元
    {6,ACCESS},//控制投入
    {7,ACCESS},//控制解除

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,ACCESS},//添加控制单元
    {4,ACCESS},//删除控制单元
    {5,ACCESS},//更新控制单元
    {6,ACCESS},//控制投入
    {7,ACCESS},//控制解除
};
const marAccessRight_t Class13MAR_8106[] =
{
    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,ACCESS},//添加控制单元
    {4,ACCESS},//删除控制单元
    {5,ACCESS},//更新控制单元
    {6,ACCESS},//控制投入
    {7,ACCESS},//控制解除
    {127,ACCESS},//下浮控投入

    {1,NOACCESS},//复位
    {2,NOACCESS},//执行
    {3,ACCESS},//添加控制单元
    {4,ACCESS},//删除控制单元
    {5,ACCESS},//更新控制单元
    {6,ACCESS},//控制投入
    {7,ACCESS},//控制解除
    {127,ACCESS},//下浮控投入
};


//Class14
//Class15
//Class16
//Class17
//Class18
const aarAccessRight_t Class18AAR_F000[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},

    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},
};

const aarAccessRight_t Class18AAR_F001[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},

    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},
};

const aarAccessRight_t Class18AAR_F002[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READWRITE},

    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READWRITE},
};

const marAccessRight_t Class18MAR_F000[] =
{
    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},

    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
};

const marAccessRight_t Class18MAR_F001[] =
{
    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
    {9,ACCESS},
    {10,ACCESS},

    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
    {9,ACCESS},
    {10,ACCESS},
};

const marAccessRight_t Class18MAR_F002[] =
{
    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},

    {1,ACCESS},
    {2,ACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
};

//Class19
const aarAccessRight_t Class19AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},
    {7,READWRITE},
    {8,READWRITE},
    {9,READWRITE},
    {10,READWRITE},
#if SECURITY_DEFEND == 1
    {11,READWRITE},
#endif

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},
    {7,READWRITE},
    {8,READWRITE},
    {9,READWRITE},
    {10,READWRITE},
#if SECURITY_DEFEND == 1
    {11,READWRITE},
#endif
};

const marAccessRight_t Class19MAR[] =
{
    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {151,ACCESS},
    {171,ACCESS},

    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {151,ACCESS},
    {171,ACCESS},
};

//Class20
const aarAccessRight_t Class20AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READ},
};

const marAccessRight_t Class20MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

//Class21
const aarAccessRight_t Class21AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READWRITE},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READWRITE},

    {1,READ},
    {2,READ},
    {3,READ},
    {4,READ},
    {5,READWRITE},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READWRITE},
};

const marAccessRight_t Class21MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
    {9,ACCESS},
    {10,ACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
    {6,ACCESS},
    {7,ACCESS},
    {8,ACCESS},
    {9,ACCESS},
    {10,ACCESS},
};

//Class22
const aarAccessRight_t Class22AAR[] =
{
    {1,READ},
    {2,READ},
    {3,READ},
    {4,READWRITE},
    {5,READ},
    {6,READWRITE},
    {7,READ},
    {8,READ},
    {9,READWRITE},
    {10,READ},
    {11,READWRITE},
    {12,READ},
    {13,READ},  //先用于属性20，若后续要用属性13需要修改查询网络规模功能
    {14,READ},//14-19 属性保留未用
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},
    {20,READ},//网络规模
    {21,READ},//21-30属性保留
    {22,READ},
    {23,READ},
    {24,READ},
    {25,READ},
    {26,READ},
    {27,READ},
    {28,READ},
    {29,READ},
    {30,READ},
    {31,READ},//陕西本地通信模块类型


    {1,READ},
    {2,READ},
    {3,READ},
    {4,READWRITE},
    {5,READ},
    {6,READWRITE},
    {7,READ},
    {8,READ},
    {9,READWRITE},
    {10,READ},
    {11,READWRITE},
    {12,READ},
    {13,READ},  //先用于属性20，若后续要用属性13需要修改查询网络规模功能
    {14,READ},  //14-19 属性保留未用
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},
    {20,READ},//网络规模
    {21,READ},
    {22,READ},
    {23,READ},
    {24,READ},
    {25,READ},
    {26,READ},
    {27,READ},
    {28,READ},
    {29,READ},
    {30,READ},
    {31,READ},//陕西本地通信模块类型

};

const marAccessRight_t Class22MAR[] =
{
    {1,NOACCESS},
    {2,NOACCESS},

    {1,NOACCESS},
    {2,NOACCESS},
};

const marAccessRight_t Class22MAR_1[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},

    {1,NOACCESS},
    {127,ACCESS},
};

const marAccessRight_t Class22MAR_2[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},
    {128,ACCESS},

    {1,NOACCESS},
    {127,ACCESS},
    {128,ACCESS},
};

//Class23

const aarAccessRight_t Class23AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READWRITE},
    {14,READWRITE},
    {15,READWRITE},
    {16,READWRITE},
    {17,READ},
    {18,READ},

    {1,READ},
    {2,READWRITE},
    {3,READ},
    {4,READ},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READ},
    {11,READ},
    {12,READ},
    {13,READWRITE},
    {14,READWRITE},
    {15,READWRITE},
    {16,READWRITE},
    {17,READ},
    {18,READ},
};

const marAccessRight_t Class23MAR[] =
{
    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},

    {1,ACCESS},
    {2,NOACCESS},
    {3,ACCESS},
    {4,ACCESS},
    {5,ACCESS},
};


//Class24
const aarAccessRight_t Class24AAR[] ={
  {1,READ},
  {2,READ},
  {3,READ},
  {4,READ},
  {5,READWRITE},
  {6,READ},
  {7,READ},
  {8,READ},
  {9,READ},
  {10,READ},
  {11,READWRITE},
  {12,READWRITE},


  {1,READ},
  {2,READ},
  {3,READ},
  {4,READ},
  {5,READWRITE},
  {6,READ},
  {7,READ},
  {8,READ},
  {9,READ},
  {10,READ},
  {11,READWRITE},
  {12,READWRITE},
};

const marAccessRight_t Class24MAR[] =
{
    {1,ACCESS},
    {2,NOACCESS},
    {3,NOACCESS},
    {4,ACCESS},
    {5,ACCESS},


    {1,ACCESS},
    {2,NOACCESS},
    {3,NOACCESS},
    {4,ACCESS},
    {5,ACCESS},

};

//Class25
const aarAccessRight_t Class25AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READWRITE},
    {11,READ},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},
    {6,READ},
    {7,READ},
    {8,READ},
    {9,READ},
    {10,READWRITE},
    {11,READ},
};

const marAccessRight_t Class25MAR[] =
{
    {1,ACCESS},

    {1,ACCESS},

};

//Class26
const aarAccessRight_t Class26AAR[] =
{
    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},

    {1,READ},
    {2,READWRITE},
    {3,READWRITE},
    {4,READWRITE},
    {5,READ},

};

const marAccessRight_t Class26MAR[] =
{
    {1,ACCESS},

    {1,ACCESS},

};

#define CLASS_AAR_MAR(aar,mar) sizeof(aar)/sizeof(aarAccessRight_t)/2,sizeof(mar)/sizeof(marAccessRight_t)/2,aar,mar
const ClassBaseList_t ObjectList_ManageDevice[]=
{
#if(1)
/*001*/{0x0000, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 组合有功电能
/*002*/{0x0001, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相组合有功电能
/*003*/{0x0002, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相组合有功电能
/*004*/{0x0003, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相组合有功电能
/*005*/{0x0010, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 正向有功电能
/*006*/{0x0011, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相正向有功电能
/*007*/{0x0012, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相正向有功电能
/*008*/{0x0013, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相正向有功电能
/*009*/{0x0020, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 反向有功电能
/*010*/{0x0021, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相反向有功电能
/*011*/{0x0022, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相反向有功电能
/*012*/{0x0023, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相反向有功电能
/*013*/{0x0030, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 组合无功1电能
/*014*/{0x0031, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相组合无功1电能
/*015*/{0x0032, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相组合无功1电能
/*016*/{0x0033, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相组合无功1电能
/*017*/{0x0040, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 组合无功2电能
/*018*/{0x0041, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相组合无功2电能
/*019*/{0x0042, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相组合无功2电能
/*020*/{0x0043, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相组合无功2电能
/*021*/{0x0050, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// I象限无功电能
/*022*/{0x0051, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相I象限无功电能
/*023*/{0x0052, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相I象限无功电能
/*024*/{0x0053, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相I象限无功电能
/*025*/{0x0060, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// II象限无功电能
/*026*/{0x0061, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相II象限无功电能
/*027*/{0x0062, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相II象限无功电能
/*028*/{0x0063, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相II象限无功电能
/*029*/{0x0070, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// III象限无功电能
/*030*/{0x0071, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相III象限无功电能
/*031*/{0x0072, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相III象限无功电能
/*032*/{0x0073, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相III象限无功电能
/*033*/{0x0080, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// IV象限无功电能
/*034*/{0x0081, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相IV象限无功电能
/*035*/{0x0082, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相IV象限无功电能
/*036*/{0x0083, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相IV象限无功电能
/*037*/{0x0090, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 正向视在电能
/*038*/{0x0091, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相正向视在电能
/*039*/{0x0092, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相正向视在电能
/*040*/{0x0093, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相正向视在电能
/*041*/{0x00A0, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// 反向视在电能
/*042*/{0x00A1, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相反向视在电能
/*043*/{0x00A2, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相反向视在电能
/*044*/{0x00A3, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相反向视在电能
/*045*/{0x0110, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//正向基波有功总电能
/*046*/{0x0111, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相正向基波有功电能
/*047*/{0x0112, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相正向基波有功电能
/*048*/{0x0113, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相正向基波有功电能
/*049*/{0x0120, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//反向基波有功总电能
/*050*/{0x0121, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相反向基波有功电能
/*051*/{0x0122, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相反向基波有功电能
/*052*/{0x0123, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相反向基波有功电能
/*053*/{0x0210, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//正向谐波有功总电能
/*054*/{0x0211, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相正向谐波有功电能
/*055*/{0x0212, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相正向谐波有功电能
/*056*/{0x0213, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相正向谐波有功电能
/*057*/{0x0220, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//反向谐波有功总电能
/*058*/{0x0221, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A相反向谐波有功电能
/*059*/{0x0222, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B相反向谐波有功电能
/*060*/{0x0223, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C相反向谐波有功电能
/*061*/{0x0300, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//铜损有功总电能补偿量
/*062*/{0x0400, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//铁损有功总电能补偿量
/*063*/{0x0500, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//关联总电能
/*064*/{0x1000, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//组合有功最大需量
/*065*/{0x1001, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相组合有功最大需量
/*066*/{0x1002, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相组合有功最大需量
/*067*/{0x1003, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相组合有功最大需量
/*068*/{0x1010, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//正向有功最大需量
/*069*/{0x1011, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相正向有功最大需量
/*070*/{0x1012, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相正向有功最大需量
/*071*/{0x1013, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相正向有功最大需量
/*072*/{0x1020, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//反向有功最大需量
/*073*/{0x1021, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相反向有功最大需量
/*074*/{0x1022, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相反向有功最大需量
/*075*/{0x1023, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相反向有功最大需量
/*076*/{0x1030, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//组合无功1最大需量
/*077*/{0x1031, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相组合无功1最大需量
/*078*/{0x1032, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相组合无功1最大需量
/*079*/{0x1033, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相组合无功1最大需量
/*080*/{0x1040, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//组合无功2最大需量
/*081*/{0x1041, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相组合无功2最大需量
/*082*/{0x1042, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相组合无功2最大需量
/*083*/{0x1043, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相组合无功2最大需量
/*084*/{0x1050, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//I象限无功最大需量
/*085*/{0x1051, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相I象限无功最大需量
/*086*/{0x1052, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相I象限无功最大需量
/*087*/{0x1053, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相I象限无功最大需量
/*088*/{0x1060, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//II象限无功最大需量
/*089*/{0x1061, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相II象限无功最大需量
/*090*/{0x1062, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相II象限无功最大需量
/*091*/{0x1063, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相II象限无功最大需量
/*092*/{0x1070, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//III象限无功最大需量
/*093*/{0x1071, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相III象限无功最大需量
/*094*/{0x1072, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相III象限无功最大需量
/*095*/{0x1073, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相III象限无功最大需量
/*096*/{0x1080, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//IV象限无功最大需量
/*097*/{0x1081, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相IV象限无功最大需量
/*098*/{0x1082, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相IV象限无功最大需量
/*099*/{0x1083, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相IV象限无功最大需量
/*100*/{0x1090, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//正向视在最大需量
/*101*/{0x1091, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相正向视在最大需量
/*102*/{0x1092, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相正向视在最大需量
/*103*/{0x1093, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相正向视在最大需量
/*104*/{0x10A0, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//反向视在最大需量
/*105*/{0x10A1, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A相反向视在最大需量
/*106*/{0x10A2, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B相反向视在最大需量
/*107*/{0x10A3, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C相反向视在最大需量
/*108*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//计量电压
/*109*/{0x2001, 3, CLASS_AAR_MAR(Class3AAR_2001, Class3MAR      )},//计量电流
/*110*/{0x2002, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//计量电压相角
/*111*/{0x2003, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//计量电压电流相角
/*112*/{0x2004, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//有功功率
/*113*/{0x2005, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//无功功率
/*114*/{0x2006, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//视在功率
/*115*/{0x2007, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//一分钟平均有功功率
/*116*/{0x2008, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//一分钟平均无功功率
/*117*/{0x2009, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//一分钟平均视在功率
/*118*/{0x200A, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//功率因数
/*119*/{0x200B, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//电压波形失真度
/*120*/{0x200C, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//电流波形失真度
/*121*/{0x200D, 5, CLASS_AAR_MAR(Class5AAR,      Class5MAR      )},//电压谐波含量
/*122*/{0x200E, 5, CLASS_AAR_MAR(Class5AAR,      Class5MAR      )},//电流谐波含量
/*123*/{0x200F, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//电网频率
/*124*/{0x2010, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//表内温度
/*125*/{0x2011, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//时钟电池压
/*126*/{0x2012, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//停电抄表池压
/*127*/{0x2013, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//时钟电池工作压
/*128*/{0x2014, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//电能表 运行状态字
/*129*/{0x2017, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*130*/{0x2018, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*131*/{0x2019, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*132*/{0x2130, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*133*/{0x2131, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*134*/{0x2132, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*135*/{0x2133, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//
/*136*/{0x3000,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*137*/{0x3001,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*138*/{0x3002,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*139*/{0x3003,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*140*/{0x3004,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*141*/{0x3005,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*142*/{0x3006,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*143*/{0x3007,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*144*/{0x3008,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*145*/{0x300B,24, CLASS_AAR_MAR(Class24AAR,     Class24MAR     )},//
/*146*/{0x3009,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*147*/{0x300A,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*148*/{0x300C,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*149*/{0x300D,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*150*/{0x300E,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*151*/{0x300F,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*152*/{0x3010,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*153*/{0x3011,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*154*/{0x3012,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*155*/{0x3013,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*156*/{0x3014,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*157*/{0x3015,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*158*/{0x3016,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*159*/{0x3017,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*160*/{0x3018,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*161*/{0x3019,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*162*/{0x301A,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*163*/{0x301B,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*164*/{0x301C,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*165*/{0x301D,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*166*/{0x301E,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*167*/{0x301F,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*168*/{0x3020,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*169*/{0x3021,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*170*/{0x3022,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*171*/{0x3023,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*172*/{0x3024,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*173*/{0x3025,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*174*/{0x3026,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*175*/{0x3027,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*176*/{0x3028,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*177*/{0x3029,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*178*/{0x302A,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*179*/{0x302B,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*180*/{0x302C,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*181*/{0x302D,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*182*/{0x302E,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*183*/{0x302F,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*184*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//电压合格率参数
#if BROADBAND_698_EXT_FUNC == 1
/*185*/{0x3030,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//通信模块变更事件
#endif
#endif
/*001*/{0x201E, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//事件发生时间
/*002*/{0x2020, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//事件结束时间
/*003*/{0x2021, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//数据冻结时间
/*004*/{0x2022, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//事件记录序号
/*005*/{0x2023, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//冻结记录序号
/*006*/{0x2024, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//事件发生源
/*007*/{0x2200, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//通信流量
/*008*/{0x2203, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//供电时间
/*009*/{0x2204, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//复位次数
#if HUNAN_FUN == 1
/*009*/{0x2205, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//备用充电电池
#endif
/*010*/{0x2301,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组1
/*011*/{0x2302,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组2
/*012*/{0x2303,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组3
/*013*/{0x2304,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组4
/*014*/{0x2305,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组5
/*015*/{0x2306,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组6
/*016*/{0x2307,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组7
/*017*/{0x2308,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//总加组8
/*018*/{0x2401,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点1
/*019*/{0x2402,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点2
/*020*/{0x2403,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点3
/*021*/{0x2404,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点4
/*022*/{0x2405,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点5
/*023*/{0x2406,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点6
/*024*/{0x2407,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点7
/*025*/{0x2408,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//脉冲计量点8
/*026*/{0x3100, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端初始化事件
/*027*/{0x3101, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端版本变更事件
/*028*/{0x3104, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端状态量变位事件
/*029*/{0x3105, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能表时钟超差事件
/*030*/{0x3106, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端停/上电事件
/*031*/{0x3109, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//消息认证错误记录
/*032*/{0x310A, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端故障记录
/*033*/{0x310B, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能表示度下降
/*034*/{0x310C, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能量超差
/*035*/{0x310D, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能表飞走
/*036*/{0x310E, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能表停走
/*037*/{0x310F, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//485抄表失败
/*038*/{0x3110, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端与主站通信流量超门限
/*039*/{0x3111, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//发现未知电表事件记录
/*040*/{0x3113, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//补抄失败事件记录
/*041*/{0x3114, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//对时事件记录
/*042*/{0x3115, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//遥控跳闸记录
/*043*/{0x3118, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端编程记录
/*044*/{0x3119, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端电流回路异常事件
/*044*/{0x311B, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//终端对电表校时记录
/*045*/{0x311C, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电能表数据变更监控记录
/*046*/{0x3200, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//功控跳闸记录
/*047*/{0x3201, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电控跳闸记录
/*048*/{0x3202, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//购电参数设置记录
/*049*/{0x3203, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电控告警事件记录
/*050*/{0x3300, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//通道上报状态
/*051*/{0x4000, 8, CLASS_AAR_MAR(Class8AAR_4000, Class8MAR_4000 )},//当前时间
/*052*/{0x4001, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//通信地址
/*053*/{0x4003, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//客户编号
/*054*/{0x4004, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//设备地理位置
/*055*/{0x4005, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//组地址，需要单独实现
/*056*/{0x4006, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR_4006 )},//时钟原
/*057*/{0x4007, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//LCD 参数
/*136*/{0x400A, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//两套分时费率切换时间
/*058*/{0x400C, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//时区时段数
/*146*/{0x4014, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//当前套时区表
/*147*/{0x4015, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//备用套时区表
/*059*/{0x4016, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//当前套日时段表
/*060*/{0x4018, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//当前套费率电价
/*151*/{0x4019, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//备用套费率电价
/*061*/{0x4024, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//剔除
/*062*/{0x4100, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//最大需量周期
/*063*/{0x4101, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//功率滑差时间
/*064*/{0x4102, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//校表脉冲宽度
/*065*/{0x4103, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//资产管理编码
/*066*/{0x4104, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//额定电压
/*067*/{0x4105, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//额定电流 /
/*068*/{0x4106, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//最大电流
/*069*/{0x4107, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//有功准确度等级
/*070*/{0x4108, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//无功准确度等级
/*071*/{0x4109, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//电能表有功常数
/*072*/{0x410A, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//电能表无功常数
/*073*/{0x410B, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//电能表型号
/*074*/{0x4200,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_4200)},//路由信息配置表
/*075*/{0x4202, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//级联参数
/*076*/{0x4204, 8, CLASS_AAR_MAR(Class8AAR_4204, Class8MAR      )},//终端广播校时时间参数
/*077*/{0x4300,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//集中器设备信息
/*078*/{0x4301,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//路由模块设备信息
/*079*/{0x4302,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//GPRS模块设备信息
/*080*/{0x4303,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//专变设备信息
/*081*/{0x4400,20, CLASS_AAR_MAR(Class20AAR,     Class20MAR     )},//应用连接对象
/*082*/{0x4401, 8, CLASS_AAR_MAR(Class8AAR_4401, Class8MAR      )},//密码设置
/*083*/{0x4500,25, CLASS_AAR_MAR(Class25AAR,     Class25MAR     )},//公网通信模块1
/*084*/{0x4510,26, CLASS_AAR_MAR(Class26AAR,     Class26MAR     )},//以太网通信模块1
/*085*/{0x5002, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//分钟冻结
/*085*/{0x5003, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//小时冻结
/*086*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//日冻结
/*086*/{0x5005, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//抄表日冻结
/*087*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//月冻结
/*088*/{0x6012,10, CLASS_AAR_MAR(Class10AAR,     Class10MAR_6012)},//任务配置表
/*089*/{0x6000,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6000)},//采集档案配置表
/*090*/{0x6001, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//采集档案配置单元
/*091*/{0x6002,11, CLASS_AAR_MAR(Class11AAR_6002,Class11MAR_6002)},//搜表
/*092*/{0x6003,8,  CLASS_AAR_MAR(Class11AAR,     Class11MAR_6002)},//一个搜表结果
/*093*/{0x6014,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6014)},//普通采集
/*094*/{0x6015, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//普通采集方案单元
/*095*/{0x6016,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6016)},//事件采集
/*096*/{0x6017, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//事件采集方案单元
/*097*/{0x6018,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6018)},//透明方案集
/*098*/{0x6019, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//透明方案单元
/*099*/{0x601A,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//透明方案结果集
/*100*/{0x601B, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//透明方案结果单元
/*101*/{0x601C,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601C)},//上报方案集
/*102*/{0x601D, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//上报方案单元
/*103*/{0x601E,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601E)},//采集规则库
/*104*/{0x6032,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//采集状态集
/*106*/{0x6033, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//采集任务监控集
/*105*/{0x6034,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//采集任务监控集
/*106*/{0x6035, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//采集任务监控集
/*107*/{0x6051,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6051)},//实时监控采集方案集
/*108*/{0x601E,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601E)},//采集规则库
/*109*/{0x8000, 8, CLASS_AAR_MAR(Class8AAR_8000, Class8MAR_8000 )},//遥控
/*110*/{0x8001, 8, CLASS_AAR_MAR(Class8AAR_8001, Class8MAR_8001 )},//保电
/*111*/{0x8002, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR_8002 )},//催费告警
/*112*/{0x8003,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_800X)},//一般中文信息
/*113*/{0x8004,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_800X)},//重要中文信息
/*114*/{0x8100, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//终端保安定值
/*115*/{0x8101, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//终端功控时段
/*116*/{0x8102, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//功控告警时间
/*117*/{0x8103,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR_8106)},//时段功控
/*118*/{0x8104,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//厂休控
/*119*/{0x8105,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//营业报停控
/*120*/{0x8106,13, CLASS_AAR_MAR(Class13AAR_8106,Class13MAR_8106)},//当前功率下浮控
/*121*/{0x8107,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//购电控
/*122*/{0x8108,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//月电控
/*123*/{0x8109, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//时段控配置单元
/*124*/{0x810A, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//厂休控配置单元
/*125*/{0x810B, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//营业报停控配置单元
/*126*/{0x810C, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//购电控配置单元
/*127*/{0x810D, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//月电控配置单元
/*128*/{0x810E, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//控制对象
/*129*/{0x810F, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//跳闸轮次
/*130*/{0x8110, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//电控定值
/*131*/{0xF000,18, CLASS_AAR_MAR(Class18AAR_F000,Class18MAR_F000)},//文件分帧传输管理
/*132*/{0xF001,18, CLASS_AAR_MAR(Class18AAR_F001,Class18MAR_F001)},//文件分块传输管理
/*133*/{0xF002,18, CLASS_AAR_MAR(Class18AAR_F002,Class18MAR_F002)},//文件扩展传输管理
/*134*/{0xF002,18, CLASS_AAR_MAR(Class18AAR_F002,Class18MAR_F002)},//文件扩展传输管理
/*135*/{0xF100,21, CLASS_AAR_MAR(Class21AAR,     Class21MAR     )},//ESAM
/*136*/{0xF101, 8, CLASS_AAR_MAR(Class8AAR_F101, Class8MAR_F101 )},//安全模式
/*137*/{0xF200,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//RS232
/*138*/{0xF201,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//串口
/*139*/{0xF202,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//红外
/*140*/{0xF203,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//开关量输入
/*141*/{0xF204,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//直流模拟量
/*142*/{0xF205,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//继电器输出
/*143*/{0xF206,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//告警输出
/*144*/{0xF207,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//多功能端子
/*145*/{0xF208,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_2   )},//交采单元
/*145*/{0xF209,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_2   )},//载波/微功率无线模块
/*146*/{0xF20A,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//脉冲输入
/*147*/{0xF20B,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//蓝牙
/*148*/{0xFFF0,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//设备出厂信息
/*149*/{0xFFF1,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//厂家内部通信参数
/*150*/{0xFFF2,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//厂家内部通信参数
/*151*/{0x7000,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_7000)},//文件集合
/*152*/{0x7010,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_7010)},//脚本集合
/*153*/{0xF300,17, CLASS_AAR_MAR(Class17AAR,     Class17MAR     )},//自动轮显
/*154*/{0xF301,17, CLASS_AAR_MAR(Class17AAR,     Class17MAR     )},//按键轮显
/*155*/{0x3108, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*156*/{0x3117, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*157*/{0x311A, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*158*/{0x3107, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*159*/{0x3116, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*160*/{0x3112, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*161*/{0x4520, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//公网远程通信多接入点备用通道
#if HENAN_FUN == 1
/*162*/{0xFF10, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//电表接线错误信息
/*163*/{0x3411, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//电表接线错误事件记录
#endif
#if SECURITY_DEFEND == 1
/*164*/{0x3140, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//安全事件变更记录
#endif
#if HUNAN_FUN == 1
/*165*/{0x2212, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//当前电池状态
#endif
};
#if(HARDWARE_TYPE != 9)
const ClassBaseList_t ObjectList_Logic1Device[]=
{
/*001*/{0x0000, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 组合有功电能
/*002*/{0x0001, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相组合有功电能
/*003*/{0x0002, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相组合有功电能
/*004*/{0x0003, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相组合有功电能
/*005*/{0x0010, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 正向有功电能
/*006*/{0x0011, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相正向有功电能
/*007*/{0x0012, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相正向有功电能
/*008*/{0x0013, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相正向有功电能
/*009*/{0x0020, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 反向有功电能
/*010*/{0x0021, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相反向有功电能
/*011*/{0x0022, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相反向有功电能
/*012*/{0x0023, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相反向有功电能
/*013*/{0x0030, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 组合无功1电能
/*014*/{0x0031, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相组合无功1电能
/*015*/{0x0032, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相组合无功1电能
/*016*/{0x0033, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相组合无功1电能
/*017*/{0x0040, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 组合无功2电能
/*018*/{0x0041, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相组合无功2电能
/*019*/{0x0042, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相组合无功2电能
/*020*/{0x0043, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相组合无功2电能
/*021*/{0x0050, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// I象限无功电能
/*022*/{0x0051, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相I象限无功电能
/*023*/{0x0052, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相I象限无功电能
/*024*/{0x0053, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相I象限无功电能
/*025*/{0x0060, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// II象限无功电能
/*026*/{0x0061, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相II象限无功电能
/*027*/{0x0062, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相II象限无功电能
/*028*/{0x0063, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相II象限无功电能
/*029*/{0x0070, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// III象限无功电能
/*030*/{0x0071, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相III象限无功电能
/*031*/{0x0072, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相III象限无功电能
/*032*/{0x0073, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相III象限无功电能
/*033*/{0x0080, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// IV象限无功电能
/*034*/{0x0081, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相IV象限无功电能
/*035*/{0x0082, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相IV象限无功电能
/*036*/{0x0083, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相IV象限无功电能
/*037*/{0x0090, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 正向视在电能
/*038*/{0x0091, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相正向视在电能
/*039*/{0x0092, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相正向视在电能
/*040*/{0x0093, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相正向视在电能
/*041*/{0x00A0, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// 反向视在电能
/*042*/{0x00A1, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相反向视在电能
/*043*/{0x00A2, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相反向视在电能
/*044*/{0x00A3, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相反向视在电能
/*045*/{0x0110, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//正向基波有功总电能
/*046*/{0x0111, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相正向基波有功电能
/*047*/{0x0112, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相正向基波有功电能
/*048*/{0x0113, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相正向基波有功电能
/*049*/{0x0120, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//反向基波有功总电能
/*050*/{0x0121, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相反向基波有功电能
/*051*/{0x0122, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相反向基波有功电能
/*052*/{0x0123, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相反向基波有功电能
/*053*/{0x0210, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//正向谐波有功总电能
/*054*/{0x0211, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相正向谐波有功电能
/*055*/{0x0212, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相正向谐波有功电能
/*056*/{0x0213, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相正向谐波有功电能
/*057*/{0x0220, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//反向谐波有功总电能
/*058*/{0x0221, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A相反向谐波有功电能
/*059*/{0x0222, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B相反向谐波有功电能
/*060*/{0x0223, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C相反向谐波有功电能
/*061*/{0x0300, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//铜损有功总电能补偿量
/*062*/{0x0400, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//铁损有功总电能补偿量
/*063*/{0x0500, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//关联总电能
/*064*/{0x1000, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//组合有功最大需量
/*065*/{0x1001, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相组合有功最大需量
/*066*/{0x1002, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相组合有功最大需量
/*067*/{0x1003, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相组合有功最大需量
/*068*/{0x1010, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//正向有功最大需量
/*069*/{0x1011, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相正向有功最大需量
/*070*/{0x1012, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相正向有功最大需量
/*071*/{0x1013, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相正向有功最大需量
/*072*/{0x1020, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//反向有功最大需量
/*073*/{0x1021, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相反向有功最大需量
/*074*/{0x1022, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相反向有功最大需量
/*075*/{0x1023, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相反向有功最大需量
/*076*/{0x1030, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//组合无功1最大需量
/*077*/{0x1031, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相组合无功1最大需量
/*078*/{0x1032, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相组合无功1最大需量
/*079*/{0x1033, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相组合无功1最大需量
/*080*/{0x1040, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//组合无功2最大需量
/*081*/{0x1041, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相组合无功2最大需量
/*082*/{0x1042, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相组合无功2最大需量
/*083*/{0x1043, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相组合无功2最大需量
/*084*/{0x1050, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//I象限无功最大需量
/*085*/{0x1051, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相I象限无功最大需量
/*086*/{0x1052, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相I象限无功最大需量
/*087*/{0x1053, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相I象限无功最大需量
/*088*/{0x1060, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//II象限无功最大需量
/*089*/{0x1061, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相II象限无功最大需量
/*090*/{0x1062, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相II象限无功最大需量
/*091*/{0x1063, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相II象限无功最大需量
/*092*/{0x1070, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//III象限无功最大需量
/*093*/{0x1071, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相III象限无功最大需量
/*094*/{0x1072, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相III象限无功最大需量
/*095*/{0x1073, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相III象限无功最大需量
/*096*/{0x1080, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//IV象限无功最大需量
/*097*/{0x1081, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相IV象限无功最大需量
/*098*/{0x1082, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相IV象限无功最大需量
/*099*/{0x1083, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相IV象限无功最大需量
/*100*/{0x1090, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//正向视在最大需量
/*101*/{0x1091, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相正向视在最大需量
/*102*/{0x1092, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相正向视在最大需量
/*103*/{0x1093, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相正向视在最大需量
/*104*/{0x10A0, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//反向视在最大需量
/*105*/{0x10A1, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A相反向视在最大需量
/*106*/{0x10A2, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B相反向视在最大需量
/*107*/{0x10A3, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C相反向视在最大需量
/*108*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//计量电压
/*109*/{0x2001, 3, CLASS_AAR_MAR(Class3AAR_2001,Class3MAR       )},//计量电流
/*110*/{0x2002, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//计量电压相角
/*111*/{0x2003, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//计量电压电流相角
/*112*/{0x2004, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//有功功率
/*113*/{0x2005, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//无功功率
/*114*/{0x2006, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//视在功率
/*115*/{0x2007, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//一分钟平均有功功率
/*116*/{0x2008, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//一分钟平均无功功率
/*117*/{0x2009, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//一分钟平均视在功率
/*118*/{0x200A, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//功率因数
/*119*/{0x200B, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//电压波形失真度
/*120*/{0x200C, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//电流波形失真度
/*121*/{0x200D, 5, CLASS_AAR_MAR(Class5AAR,     Class5MAR       )},//电压谐波含量
/*122*/{0x200E, 5, CLASS_AAR_MAR(Class5AAR,     Class5MAR       )},//电流谐波含量
/*123*/{0x200F, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//电网频率
/*124*/{0x2010, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//表内温度
/*125*/{0x2011, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//时钟电池压
/*126*/{0x2012, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//停电抄表池压
/*127*/{0x2013, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//时钟电池工作压
/*128*/{0x2014, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//电能表 运行状态字
/*129*///{0x2025, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//事件当前值
/*130*///{0x202C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//（当前）钱包文件
/*131*///{0x202F, 8, CLASS_AAR_MAR(Class8AAR_202F,Class8MAR        },//电能表主动上报
/*132*///{0x3300, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//通道上报状态
/*133*/{0x300F, 7, CLASS_AAR_MAR(Class7AAR,     Class7MAR       )},//交采电压逆相序
/*134*/{0x4008, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//两套时区表切换时间
/*135*/{0x4009, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//两套日时段切换时间
/*136*/{0x400A, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//两套分时费率切换时间
/*137*/{0x400B, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//两套阶梯电价切换时间
/*138*/{0x400C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//时区时段数
/*139*/{0x400D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//阶梯数
/*140*/{0x400E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//谐波分析次数
/*141*/{0x400F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//密钥总条数
/*142*/{0x4010, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//计量元件数
/*143*/{0x4011, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//公共假日表
/*144*/{0x4012, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//周休日特征字
/*145*/{0x4013, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//周休日釆用的日时段表号
/*146*/{0x4014, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//当前套时区表
/*147*/{0x4015, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//备用套时区表
/*148*/{0x4016, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//当前套日时段表
/*149*/{0x4017, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//备用套日时段表
/*150*/{0x4018, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//当前套费率电价
/*151*/{0x4019, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//备用套费率电价
/*152*/{0x401A, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//当前套阶梯电价
/*153*/{0x401B, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//备用套阶梯电价
/*154*/{0x401C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//电流互感器变比
/*155*/{0x401D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//电压互感器变比
/*156*/{0x401E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//报警金额限值
/*157*/{0x401F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//其它金额限值
/*158*/{0x4022, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//插卡状态字
/*159*/{0x4023, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//认证有效时长
/*160*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//电压合格率参数
/*161*/{0x410C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC各相电导系数
/*162*/{0x410D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC各相电抗系数
/*163*/{0x410E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC各相电阻系数
/*164*/{0x410F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC各相电纳系数
/*165*/{0x4110, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//电能表运行特征字1
/*166*/{0x4111, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//软件备案号
/*167*/{0x4112, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//有功组合方式特征字
/*168*/{0x4113, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//无功组合方式1特征字
/*169*/{0x4114, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//无功组合方式2特征字
/*170*/{0x4116, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//结算日
/*171*/{0x4117, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//期间需量冻结周期
/*172*/{0x5002, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//分钟冻结
/*172*/{0x5003, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//小时冻结
/*174*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//日冻结
/*175*/{0x5005, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//抄表日冻结
/*176*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//月冻结
/*177*/{0x4300,19, CLASS_AAR_MAR(Class19AAR,   Class19MAR       )},//集中器设备信息
};


#else
const ClassBaseList_t ObjectList_Logic1Device[]=
{
/*001*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,    Class3MAR            )},//计量电压
/*002*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,    Class9MAR            )},//日冻结
/*002*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,    Class9MAR            )},//月冻结
/*003*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,    Class8MAR            )},//电压合格率参数
};

#endif

const ObjectLit_t ObjectList_All[4] = //1个管理逻辑设备+3个逻辑设备
{
    {ObjectList_ManageDevice,sizeof(ObjectList_ManageDevice)/sizeof(ClassBaseList_t)},//管理逻辑设备
    {ObjectList_Logic1Device,sizeof(ObjectList_Logic1Device)/sizeof(ClassBaseList_t)},//逻辑设备-交采
    {NULL,                   0},//未定义
    {NULL,                   0},//未定义
};
