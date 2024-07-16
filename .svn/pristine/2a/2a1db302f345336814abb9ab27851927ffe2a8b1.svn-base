/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：MetSTD.c
 * 版本号：1.0
 * 功能：改程序用于数据字典与OAD对应，以及提供查找接口
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/
#include "../../include/CtsAll.h"
#include "area.h"
#ifdef NULL
#undef NULL
#define NULL    0
#endif
#include "min_gather.h"
extern uint16 get_len_offset(uint8 *indata,uint8 *bytes);


/* 电量量类OAD列表 OIA1 = 0x00 */
const OadInfoMap EnergyList[] = 
{
    /*OAD          class     */
    {{0x00000200}, 1, DT_ARRAY, 27, 5,      E_OOP_ENERGYS_T,},           //组合有功电能 - 电能量
    {{0x00000400}, 1, DT_ARRAY, 47, 9,      E_OOP_HENERGYS_T,},         //组合有功电能 - 高精度电能量
    {{0x00100200}, 1, DT_ARRAY, 27, 5,       E_OOP_ENERGY_T,},           //正向有功电能 - 电能量
    {{0x00100400}, 1, DT_ARRAY, 47, 9,      E_OOP_HENERGY_T,},          //正向有功电能 - 高精度电能量
    {{0x00110200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相正向有功电能 - 电能量
    {{0x00110400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相正向有功电能 - 高精度电能量
    {{0x00120200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相正向有功电能 - 电能量
    {{0x00120400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相正向有功电能 - 高精度电能量
    {{0x00130200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相正向有功电能 - 电能量
    {{0x00130400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相正向有功电能 - 高精度电能量
    {{0x00200200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //反向有功电能 - 电能量
    {{0x00200400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //反向有功电能 - 高精度电能量
    {{0x00210200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相反向有功电能 - 电能量
    {{0x00210400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相反向有功电能 - 高精度电能量
    {{0x00220200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相反向有功电能 - 电能量
    {{0x00220400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相反向有功电能 - 高精度电能量
    {{0x00230200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相反向有功电能 - 电能量
    {{0x00230400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相反向有功电能 - 高精度电能量
    {{0x00300200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //组合无功1电能 - 电能量
    {{0x00300400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //组合无功1电能 - 高精度电能量
    {{0x00310200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //A相组合无功1电能 - 电能量
    {{0x00310400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //A相组合无功1电能 - 高精度电能量
    {{0x00320200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //B相组合无功1电能 - 电能量
    {{0x00320400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //B相组合无功1电能 - 高精度电能量
    {{0x00330200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //C相组合无功1电能 - 电能量
    {{0x00330400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //C相组合无功1电能 - 高精度电能量
    {{0x00400200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //组合无功2电能 - 电能量
    {{0x00400400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //组合无功2电能 - 高精度电能量
    {{0x00410200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //A相组合无功2电能 - 电能量
    {{0x00410400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //A相组合无功2电能 - 高精度电能量
    {{0x00420200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //B相组合无功2电能 - 电能量
    {{0x00420400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //B相组合无功2电能 - 高精度电能量
    {{0x00430200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGYS_T,},          //C相组合无功2电能 - 电能量
    {{0x00430400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGYS_T,},         //C相组合无功2电能 - 高精度电能量
    {{0x00500200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //第一象限无功电能 - 电能量
    {{0x00500400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //第一象限无功电能 - 高精度电能量
    {{0x00510200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相第一象限无功电能 - 电能量
    {{0x00510400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相第一象限无功电能 - 高精度电能量
    {{0x00520200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相第一象限无功电能 - 电能量
    {{0x00520400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相第一象限无功电能 - 高精度电能量
    {{0x00530200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相第一象限无功电能 - 电能量
    {{0x00530400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相第一象限无功电能 - 高精度电能量
    {{0x00600200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //第二象限无功电能 - 电能量
    {{0x00600400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //第二象限无功电能 - 高精度电能量
    {{0x00610200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相第二象限无功电能 - 电能量
    {{0x00610400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相第二象限无功电能 - 高精度电能量
    {{0x00620200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相第二象限无功电能 - 电能量
    {{0x00620400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相第二象限无功电能 - 高精度电能量
    {{0x00630200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相第二象限无功电能 - 电能量
    {{0x00630400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相第二象限无功电能 - 高精度电能量
    {{0x00700200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //第三象限无功电能 - 电能量
    {{0x00700400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //第三象限无功电能 - 高精度电能量
    {{0x00710200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相第三象限无功电能 - 电能量
    {{0x00710400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相第三象限无功电能 - 高精度电能量
    {{0x00720200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相第三象限无功电能 - 电能量
    {{0x00720400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相第三象限无功电能 - 高精度电能量
    {{0x00730200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相第三象限无功电能 - 电能量
    {{0x00730400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相第三象限无功电能 - 高精度电能量
    {{0x00800200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //第四象限无功电能 - 电能量
    {{0x00800400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //第四象限无功电能 - 高精度电能量
    {{0x00810200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相第四象限无功电能 - 电能量
    {{0x00810400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相第四象限无功电能 - 高精度电能量
    {{0x00820200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相第四象限无功电能 - 电能量
    {{0x00820400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相第四象限无功电能 - 高精度电能量
    {{0x00830200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相第四象限无功电能 - 电能量
    {{0x00830400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相第四象限无功电能 - 高精度电能量
    {{0x00900200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //正向视在电能 - 电能量
    {{0x00900400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //正向视在电能 - 高精度电能量
    {{0x00910200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相正向视在电能 - 电能量
    {{0x00910400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相正向视在电能 - 高精度电能量
    {{0x00920200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相正向视在电能 - 电能量
    {{0x00920400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相正向视在电能 - 高精度电能量
    {{0x00930200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相正向视在电能 - 电能量
    {{0x00930400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相正向视在电能 - 高精度电能量
    {{0x00A00200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //反向视在电能 - 电能量
    {{0x00A00400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //反向视在电能 - 高精度电能量
    {{0x00A10200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相反向视在电能 - 电能量
    {{0x00A10400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相反向视在电能 - 高精度电能量
    {{0x00A20200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相反向视在电能 - 电能量
    {{0x00A20400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相反向视在电能 - 高精度电能量
    {{0x00A30200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相反向视在电能 - 电能量
    {{0x00A30400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相反向视在电能 - 高精度电能量
    {{0x01100200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //正向有功基波总电能 - 电能量
    {{0x01100400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //正向有功基波总电能 - 高精度电能量
    {{0x01110200}, 1, DT_ARRAY, 27,5,      E_OOP_ENERGY_T,},           //A相正向有功基波总电能 - 电能量
    {{0x01110400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相正向有功基波总电能 - 高精度电能量
    {{0x01120200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相正向有功基波总电能 - 电能量
    {{0x01120400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相正向有功基波总电能 - 高精度电能量
    {{0x01130200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相正向有功基波总电能 - 电能量
    {{0x01130400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相正向有功基波总电能 - 
    {{0x01200200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //反向有功基波总电能 - 电能量
    {{0x01200400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //反向有功基波总电能 - 高精度电能量
    {{0x01210200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相反向有功基波总电能 - 电能量
    {{0x01210400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相反向有功基波总电能 - 高精度电能量
    {{0x01220200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相反向有功基波总电能 - 电能量
    {{0x01220400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相反向有功基波总电能 - 高精度电能量
    {{0x01230200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相反向有功基波总电能 - 电能量
    {{0x01230400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相反向有功基波总电能 - 高精度电能量
    {{0x02100200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //正向有功谐波总电能 - 电能量
    {{0x02100400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //正向有功谐波总电能 - 高精度电能量
    {{0x02110200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相正向有功谐波总电能 - 电能量
    {{0x02110400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相正向有功谐波总电能 - 高精度电能量
    {{0x02120200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相正向有功谐波总电能 - 电能量
    {{0x02120400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相正向有功谐波总电能 - 高精度电能量
    {{0x02130200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相正向有功谐波总电能 - 电能量
    {{0x02130400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相正向有功谐波总电能 - 高精度电能量
    {{0x02200200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //反向有功谐波总电能 - 电能量
    {{0x02200400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //反向有功谐波总电能 - 高精度电能量
    {{0x02210200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相反向有功谐波总电能 - 电能量
    {{0x02210400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相反向有功谐波总电能 - 高精度电能量
    {{0x02220200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相反向有功谐波总电能 - 电能量
    {{0x02220400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相反向有功谐波总电能 - 高精度电能量
    {{0x02230200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相反向有功谐波总电能 - 电能量
    {{0x02230400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相反向有功谐波总电能 - 高精度电能量
    {{0x03000200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //铜损有功总电能补偿量 - 电能量
    {{0x03000400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //铜损有功总电能补偿量 - 高精度电能量
    {{0x03010200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相铜损有功总电能补偿量 - 电能量
    {{0x03010400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相铜损有功总电能补偿量 - 高精度电能量
    {{0x03020200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相铜损有功总电能补偿量 - 电能量
    {{0x03020400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相铜损有功总电能补偿量 - 高精度电能量
    {{0x03030200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相铜损有功总电能补偿量 - 电能量
    {{0x03030400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相铜损有功总电能补偿量 - 高精度电能量
    {{0x04000200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //铁损有功总电能补偿量 - 电能量
    {{0x04000400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //铁损有功总电能补偿量 - 高精度电能量
    {{0x04010200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相铁损有功总电能补偿量 - 电能量
    {{0x04010400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相铁损有功总电能补偿量 - 高精度电能量
    {{0x04020200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相铁损有功总电能补偿量 - 电能量
    {{0x04020400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相铁损有功总电能补偿量 - 高精度电能量
    {{0x04030200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相铁损有功总电能补偿量 - 电能量
    {{0x04030400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相铁损有功总电能补偿量 - 高精度电能量
    {{0x05000200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //关联总电能 - 电能量
    {{0x05000400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //关联总电能 - 高精度电能量
    {{0x05010200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //A相关联总电能 - 电能量
    {{0x05010400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //A相关联总电能 - 高精度电能量
    {{0x05020200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //B相关联总电能 - 电能量
    {{0x05020400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //B相关联总电能 - 高精度电能量
    {{0x05030200}, 1, DT_ARRAY, 27,5,       E_OOP_ENERGY_T,},           //C相关联总电能 - 电能量
    {{0x05030400}, 1, DT_ARRAY, 47,9,       E_OOP_HENERGY_T,},          //C相关联总电能 - 高精度电能量
};



/* 最大需量类OAD列表 OIA1 = 0x01 */
const OadInfoMap DemandList[] = 
{
    /*OAD          class        len*/
    {{0x10100200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //正向有功最大需量
    {{0x10110200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相正向有功最大需量
    {{0x10120200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相正向有功最大需量
    {{0x10130200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相正向有功最大需量
    {{0x10200200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //反向有功最大需量
    {{0x10210200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相反向有功最大需量
    {{0x10220200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相反向有功最大需量
    {{0x10230200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相反向有功最大需量
    {{0x10300200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //组合无功1最大需量
    {{0x10310200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //A相组合无功1最大需量
    {{0x10320200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //B相组合无功1最大需量
    {{0x10330200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //C相组合无功1最大需量
    {{0x10400200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //组合无功2最大需量
    {{0x10410200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //A相组合无功2最大需量
    {{0x10420200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //B相组合无功2最大需量
    {{0x10430200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //C相组合无功2最大需量
    {{0x10500200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //第一象限最大需量
    {{0x10510200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相第一象限最大需量
    {{0x10520200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相第一象限最大需量
    {{0x10530200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相第一象限最大需量
    {{0x10600200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //第二象限最大需量
    {{0x10610200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相第二象限最大需量
    {{0x10620200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相第二象限最大需量
    {{0x10630200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相第二象限最大需量
    {{0x10700200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //第三象限最大需量
    {{0x10710200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相第三象限最大需量
    {{0x10720200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相第三象限最大需量
    {{0x10730200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相第三象限最大需量
    {{0x10800200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //第四象限最大需量
    {{0x10810200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相第四象限最大需量
    {{0x10820200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相第四象限最大需量
    {{0x10830200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相第四象限最大需量
    {{0x10900200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //正向视在最大需量
    {{0x10910200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相正向视在最大需量
    {{0x10920200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相正向视在最大需量
    {{0x10930200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相正向视在最大需量
    {{0x10A00200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //反向视在最大需量
    {{0x10A10200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //A相反向视在最大需量
    {{0x10A20200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //B相反向视在最大需量
    {{0x10A30200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //C相反向视在最大需量
    {{0x11100200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内正向有功最大需量
    {{0x11110200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相正向有功最大需量
    {{0x11120200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相正向有功最大需量
    {{0x11130200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相正向有功最大需量
    {{0x11200200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内反向有功最大需量
    {{0x11210200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相反向有功最大需量
    {{0x11220200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相反向有功最大需量
    {{0x11230200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相反向有功最大需量
    {{0x11300200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内组合无功1最大需量
    {{0x11310200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内A相组合无功1最大需量
    {{0x11320200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内B相组合无功1最大需量
    {{0x11330200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内C相组合无功1最大需量
    {{0x11400200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内组合无功2最大需量
    {{0x11410200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内A相组合无功2最大需量
    {{0x11420200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内B相组合无功2最大需量
    {{0x11430200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMANDS_T,},         //冻结周期内C相组合无功2最大需量
    {{0x11500200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内第一象限最大需量
    {{0x11510200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相第一象限最大需量
    {{0x11520200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相第一象限最大需量
    {{0x11530200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相第一象限最大需量
    {{0x11600200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内第二象限最大需量
    {{0x11610200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相第二象限最大需量
    {{0x11620200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相第二象限最大需量
    {{0x11630200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相第二象限最大需量
    {{0x11700200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内第三象限最大需量
    {{0x11710200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相第三象限最大需量
    {{0x11720200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相第三象限最大需量
    {{0x11730200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相第三象限最大需量
    {{0x11800200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内第四象限最大需量
    {{0x11810200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相第四象限最大需量
    {{0x11820200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相第四象限最大需量
    {{0x11830200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相第四象限最大需量
    {{0x11900200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内正向视在最大需量
    {{0x11910200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相正向视在最大需量
    {{0x11920200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相正向视在最大需量
    {{0x11930200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相正向视在最大需量
    {{0x11A00200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内反向视在最大需量
    {{0x11A10200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内A相反向视在最大需量
    {{0x11A20200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内B相反向视在最大需量
    {{0x11A30200}, 2, DT_ARRAY, 77 ,15,      E_OOP_DEMAND_T,},          //冻结周期内C相反向视在最大需量
};

/* 变量类OAD列表 OIA1 = 0x02 */
const OadInfoMap VariateList[] = 
{
        /*OAD          class   nType dataType             len*/
    {{0x20000200}, 3,            DT_ARRAY,            11,3,   E_OOP_WORD3V_T},          //电压  OOP_DWORD3V_T分相变量都用这个结构体？？？
    {{0x20000201}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //A相电压
    {{0x20000202}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //B相电压
    {{0x20000203}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //C相电压
    {{0x20000400}, 3,            DT_LONG_UNSIGNED,     3,3,   E_LONG_UNSIGNED},                 //C相电压
    {{0x20000600}, 3,            DT_LONG_UNSIGNED,     3,3,   E_LONG_UNSIGNED},                 //C相电压
    {{0x20000700}, 3,            DT_ARRAY,             11,3,  E_OOP_LONG3V_T},                 //分相偏差数值数组

    {{0x20010200}, 3,            DT_ARRAY,            17,5,   E_OOP_INT3V_T},          //电流
    {{0x20010201}, 3,            DT_DOUBLE_LONG,       5,5,   E_OOP_INT3V_T},                  //A相电流
    {{0x20010202}, 3,            DT_DOUBLE_LONG,       5,5,   E_OOP_INT3V_T},                  //B相电流
    {{0x20010203}, 3,            DT_DOUBLE_LONG,       5,5,   E_OOP_INT3V_T},                  //C相电流
    {{0x20010400}, 3,            DT_DOUBLE_LONG,       5,5,   E_DOUBLE_LONG},                  //N相电流
    {{0x20010500}, 3,            DT_DOUBLE_LONG,       5,5,   E_DOUBLE_LONG},                  //剩余电流
    {{0x20010600}, 3,            DT_DOUBLE_LONG,       5,5,   E_DOUBLE_LONG},                  //剩余电流
    {{0x20010800}, 3,            DT_DOUBLE_LONG,       5,5,   E_DOUBLE_LONG},                  //剩余电流

    {{0x20020200}, 3,            DT_ARRAY,            11,3,   E_OOP_WORD3V_T},          //电压相角  
    {{0x20020201}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //A相电压相角
    {{0x20020202}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //B相电压相角
    {{0x20020203}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //C相电压相角
    {{0x20030200}, 3,            DT_ARRAY,            11,3,   E_OOP_WORD3V_T},          //电压电流相角  
    {{0x20030201}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //A相电压电流相角
    {{0x20030202}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //B相电压电流相角
    {{0x20030203}, 3,            DT_LONG_UNSIGNED,     3,3,   E_OOP_WORD3V_T},                 //C相电压电流相角
    {{0x20040200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //有功功率
    {{0x20050200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //无功功率
    {{0x20060200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //视在功率
    {{0x20070200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //一分钟平均有功功率
    {{0x20080200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //一分钟平均无功功率
    {{0x20090200}, 4,            DT_ARRAY,      22,5,   E_OOP_INT4V_T  },                  //一分钟平均视在功率
    {{0x200A0200}, 4,            DT_ARRAY,             14,3,   E_OOP_LONG4V_T },                  //功率因数
    {{0x200B0200}, 3,            DT_ARRAY,             11,3,   E_OOP_LONG3V_T},                  //电压波形失真度
    {{0x200B0201}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //A相电压波形失真度
    {{0x200B0202}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //B相电压波形失真度
    {{0x200B0203}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //C相电压波形失真度
    {{0x200C0200}, 3,            DT_ARRAY,             11,3,   E_OOP_LONG3V_T},                  //电流波形失真度
    {{0x200C0201}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //A相电流波形失真度
    {{0x200C0202}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //B相电流波形失真度
    {{0x200C0203}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},                  //C相电流波形失真度
    {{0x200D0200}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //A相电压谐波含有量（总及2…n次）
    {{0x200D0300}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //B相电压谐波含有量（总及2…n次）
    {{0x200D0400}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //C相电压谐波含有量（总及2…n次）
    {{0x200E0200}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //A相电流谐波含有量（总及2…n次）
    {{0x200E0300}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //B相电流谐波含有量（总及2…n次）
    {{0x200E0400}, 5,            DT_ARRAY,            65,65,   E_OOP_HARMONIC_T},         //C相电流谐波含有量（总及2…n次）
    {{0x200F0200}, 6,            DT_LONG_UNSIGNED,    3,3,    E_LONG_UNSIGNED},                 //电网频率
    {{0x20100200}, 6,            DT_LONG,             3,3,    E_LONG},                  //表内温度
    {{0x20100400}, 6,            DT_ARRAY,             11,3,    E_OOP_LONG3V_T},           //端子温度

    {{0x20110200}, 6,            DT_LONG_UNSIGNED,    3,3,    E_LONG_UNSIGNED},                 //时钟电池电压
    {{0x20120200}, 6,            DT_LONG_UNSIGNED,    3,3,    E_LONG_UNSIGNED},                 //停电抄表电池电压
    {{0x20130200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //时钟电池工作时间
    {{0x20140200}, 6,            DT_ARRAY,            30,30,   E_OOP_METWORD_T},          //电能表运行状态字
    {{0x20140400}, 6,            DT_BIT_STRING,            6,6,   E_OOP_BITSTR32_T},          //计量模组系统状态字
    {{0x20140500}, 6,            DT_BIT_STRING,            6,6,   E_OOP_BITSTR32_T},          //采样系统状态字
    {{0x20150200}, 6,            DT_BIT_STRING,            6,6,   E_OOP_BITSTR32_T},          //电能表跟随上报状态字
    {{0x20150400}, 6,            DT_BIT_STRING,            6,6,   E_OOP_BITSTR32_T},          //电能表跟随上报模式字_上报方式
    {{0x20150500}, 6,            DT_ENUM,            6,6,   E_ENUM},          //电能表跟随上报模式字
    {{0x20170200}, 6,            DT_DOUBLE_LONG,      4,4,    E_DOUBLE_LONG},                  //当前有功需量
    {{0x20180200}, 6,            DT_DOUBLE_LONG,      4,4,    E_DOUBLE_LONG},                  //当前无功需量
    {{0x20190200}, 6,            DT_DOUBLE_LONG,      4,4,    E_DOUBLE_LONG},                  //当前视在需量
    {{0x201A0200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //当前电价
    {{0x201B0200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //当前费率电价
    {{0x201C0200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4, 4,E_DOUBLE_LONG_UNSIGNED},                 //当前阶梯电价
    {{0x201E0200}, 8,            DT_DATETIME_S,          7,7, E_DATETIME_S},                 //事件发生时间
    {{0x201F0201}, 6,            DT_OCTET_STRING,        1,1, E_OOP_OCTETVAR64_T},                 //剩余电流最大相
    {{0x201F0202}, 6,            DT_OCTET_STRING,        1,1, E_OOP_OCTETVAR64_T},                 //故障相别
    {{0x201F0203}, 6,            DT_OCTET_STRING,        1,1, E_OOP_OCTETVAR64_T},                 //运行状态字1
    {{0x201F0204}, 6,            DT_OCTET_STRING,        5,5, E_OOP_OCTETVAR64_T},                 //控制字
    {{0x201F0205}, 6,            DT_OCTET_STRING,        20, 20, E_OOP_OCTETVAR64_T},                   //额定剩余电流动作值参数组
    {{0x201F0206}, 6,            DT_DOUBLE_LONG_UNSIGNED,         5, 5, E_DOUBLE_LONG_UNSIGNED},                   //累计运行时间
    {{0x201F0207}, 6,            DT_LONG,          3,3, E_LONG},                   //额定剩余电流动作值参数组
    //{{0x201F0205}, 6,            DT_OCTET_STRING,          20, E_OOP_OCTETVAR64_T},                   //额定剩余电流动作值参数组
    {{0x201F0800}, 6,            DT_STRUCTURE,          20,20, E_OOP_PROTECTOR_31BB_T},                   //额定剩余电流动作值参数组
    {{0x20200200}, 8,            DT_DATETIME_S,          7,7, E_DATETIME_S},                 //事件结束时间
    {{0x20210200}, 8,            DT_DATETIME_S,          7,7, E_DATETIME_S},                 //数据冻结时间
    {{0x20220200}, 8,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //事件记录序号
    {{0x20230200}, 8,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //冻结记录序号
    {{0x20240200}, 6,            DT_LONG_UNSIGNED,     4,4,   E_OOP_EVTSOURCE_T},                 //
    {{0x20260200}, 6,            DT_LONG_UNSIGNED,     4,4,   E_LONG_UNSIGNED},                 //电压不平衡率
    {{0x20270200}, 6,            DT_LONG_UNSIGNED,     4,4,   E_LONG_UNSIGNED},                 //电流不平衡率
    {{0x20280200}, 6,            DT_LONG_UNSIGNED,     4,4,   E_LONG_UNSIGNED},                 //负载率
    {{0x202C0200}, 6,            DT_STRUCTURE,     10,10,      E_OOP_DWORD2_T},                 //钱包文件
    {{0x202D0200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //（当前）透支金额
    {{0x202E0200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //累计购电金额
    {{0x20310200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //月度用电量
    {{0x20300200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //阶梯结算用电量
    {{0x20350200}, 3,            DT_ARRAY,            17,5,   E_OOP_DWORD3V_T},          //电压波动量
    {{0x20360200}, 3,            DT_ARRAY,            17,5,   E_OOP_DWORD3V_T},
    {{0x20370200}, 3,            DT_ARRAY,            17,5,   E_OOP_DWORD3V_T},
    {{0x20380200}, 3,            DT_ARRAY,            17,5,   E_OOP_DWORD3V_T},


    {{0x21300200}, 6,            DT_STRUCTURE,48,48, E_OOP_VOLTSTAT_T},                 //总电压合格率
    {{0x21300201}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //总日电压合格率
    {{0x21300202}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //总日电压合格率
    {{0x21310200}, 6,            DT_STRUCTURE,48,48, E_OOP_VOLTSTAT_T},                 //A相电压合格率
    {{0x21310201}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //A相日电压合格率
    {{0x21310202}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //A相月电压合格率
    {{0x21320200}, 6,            DT_STRUCTURE,48,48, E_OOP_VOLTSTAT_T},                 //B相电压合格率
    {{0x21320201}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //B相日电压合格率
    {{0x21320202}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //B相月电压合格
    {{0x21330200}, 6,            DT_STRUCTURE,48,48, E_OOP_VOLTSTAT_T},                 //C相电压合格率
    {{0x21330201}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //C相日电压合格率
    {{0x21330202}, 6,            DT_STRUCTURE,23,23, E_OOP_VOLTQR_T},                 //C相月电压合格
    {{0x2E210200}, 6,            DT_ARRAY,23,23, E_OOP_BREAK_RUN_STATUS_T},                 //断路器运行状态字
    {{0x22030200}, 6,            DT_STRUCTURE,10,10, E_OOP_DWORD2_T},                   //供电时间

        
    {{0x25000200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //累计水热流量
    {{0x25010200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //累计气流量
    {{0x25020200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //累计热流量
    {{0x25030200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //热功率
    {{0x25040200}, 6,            DT_DOUBLE_LONG_UNSIGNED,4,4, E_DOUBLE_LONG_UNSIGNED},                 //累计工作时间
    {{0x25050200}, 6,            DT_STRUCTURE,10,10, E_Water_Temperature_T},                   //水温
    {{0x25060200}, 6,            DT_STRUCTURE,10,10, E_OOP_ST_T},                   //水温
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
    {{0x28110200}, 6,            DT_ENUM,     2,2,  E_ENUM},                  //烟感状态
    {{0x29000200}, 6,            DT_LONG,     3,3,  E_LONG},                  //传感器温度
    {{0x29010200}, 6,            DT_DOUBLE_LONG_UNSIGNED,     5,5,  E_DOUBLE_LONG_UNSIGNED},                  //传感器温度
    {{0x29040200}, 6,            DT_ENUM,     2,2,  E_ENUM},                 //水浸状态
    {{0x29050200}, 6,            DT_ENUM,     2,2,  E_ENUM},                 //水浸状态
#endif
#ifdef AREA_SHANDONG
    {{0x2E200200}, 6,            DT_STRUCTURE, 2,2, E_OOP_HUMITURE_2E20_T}, //温湿度
    {{0x2E210200}, 6,            DT_STRUCTURE, 2,2, E_OOP_LOCK_STATE_2E21_T}, //门锁状态
    #endif
    {{0x20460200}, 5,            DT_ARRAY, 252,5, E_OOP_HARMONIC_VOLTAGE_T},   //A相谐波电压
    {{0x20460300}, 5,            DT_ARRAY, 252,5, E_OOP_HARMONIC_VOLTAGE_T},   //B相谐波电压
    {{0x20460400}, 5,            DT_ARRAY, 252,5, E_OOP_HARMONIC_VOLTAGE_T},   //B相谐波电压
    {{0x20470200}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //A相谐波电流
    {{0x20470300}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //B相谐波电流
    {{0x20470400}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //C相谐波电流
    
    {{0x20480200}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //A相谐波电流
    {{0x20480300}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //B相谐波电流
    {{0x20480400}, 5,            DT_ARRAY, 252,2, E_OOP_HARMONIC_CUR_POWER_T},    //C相谐波电流

    {{0x20480700}, 5,            DT_STRUCTURE, 10,5, E_OOP_HARMONIC_ACTIVE_POWER_T},      //C相谐波电流


#ifdef AREA_HUNAN
    /* 湖南精品台区 */
    {{0x20010700}, 3,            DT_DOUBLE_LONG,        5,5,   E_DOUBLE_LONG,   },  // 漏电流
    {{0x26000200}, 6,            DT_ARRAY,              11,11,  E_OOP_LONG3V_T,  },  // 温度
    {{0x26010200}, 6,            DT_ARRAY,              11,11,  E_OOP_WORD3V_T,  },  // 湿度
    {{0x28000200}, 6,            DT_ENUM,               2,2,   E_ENUM           },  // 开关分合状态
    {{0x2A000200}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,          },  // 变压器油温
    {{0x2A010200}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,          },  // 变压器外壳温度
    {{0x2A030200}, 3,            DT_ARRAY,              11,3,  E_OOP_LONG3V_T,  },   // 变压器抽头温度块
    {{0x2A030201}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 变压器抽头A相温度
    {{0x2A030202}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 变压器抽头B相温度
    {{0x2A030203}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 变压器抽头C相温度
    {{0x2A040200}, 3,            DT_ARRAY,              11,3,  E_OOP_LONG3V_T,  },   // 漏保开关温度块
    {{0x2A040201}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 漏保开关A相温度
    {{0x2A040202}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 漏保开关B相温度
    {{0x2A040203}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 漏保开关C相温度
    {{0x2A050200}, 3,            DT_ARRAY,              11,3,  E_OOP_LONG3V_T,  },   // 分支接头温度块
    {{0x2A050201}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 分支接头A相温度
    {{0x2A050202}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 分支接头B相温度
    {{0x2A050203}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 分支接头C相温度
    {{0x2A060200}, 3,            DT_ARRAY,              11,3,  E_OOP_LONG3V_T,  },   // 表箱接头温度块
    {{0x2A060201}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 表箱接头A相温度
    {{0x2A060202}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 表箱接头B相温度
    {{0x2A060203}, 3,            DT_LONG,               3,3,   E_OOP_LONG3V_T,  },   // 表箱接头C相温度
    {{0x21A00200}, 3,            DT_ARRAY,             11,3,   E_OOP_LONG3V_T},      //电压偏差
    {{0x21A00201}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},         //A相电压偏差
    {{0x21A00202}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},         //B相电压偏差
    {{0x21A00203}, 3,            DT_LONG,              3,3,   E_OOP_LONG3V_T},         //C相电压偏差
    {{0x20100400}, 6,            DT_ARRAY,              11,3,   E_OOP_LONG3V_T},         //端子温度
    {{0x20430200}, 6,            DT_ARRAY,              11,3,   E_OOP_LONG3V_T},         //端子温度分钟变化量
    {{0x20440200}, 3,            DT_ARRAY,              11,3,   E_OOP_LONG3V_T},         //自监测误差
#endif
};

/* 事件类OAD列表 OIA1 = 0x03 */
const OadInfoMap EventList[] =
{
    /*OAD        class         dataType                                            STRUCT结构体*/
    {{0x30000200}, 24,         DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表失压事件 - 关联对象属性表
    {{0x30000300}, 24,         DT_STRUCTURE,                0, 0,                   E_NULL},                 //电能表失压事件 - 当前记录数
    {{0x30000400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表失压事件 - 最大记录数
    {{0x30000500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_3000_T},        //电能表失压事件 - 配置参数
    {{0x30000600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失压事件 - 事件记录单元1
    {{0x30000700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失压事件 - 事件记录单元2
    {{0x30000800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失压事件 - 事件记录单元3
    {{0x30000900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失压事件 - 事件记录单元4
    {{0x30000A00}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},                 //电能表失压事件 - 当前值记录表
    {{0x30000B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表失压事件 - 上报标识
    {{0x30000C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表失压事件 - 有效标识
    {{0x30000D00}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},          //电能表失压事件 - 失压统计
    {{0x30000E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表失压事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30010200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表欠压事件 - 关联对象属性表
    {{0x30010300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表欠压事件 - 当前记录数
    {{0x30010400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表欠压事件 - 最大记录数
    {{0x30010500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_WD_T},          //电能表欠压事件 - 配置参数
    {{0x30010600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表欠压事件 - 事件记录单元1
    {{0x30010700}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},             //电能表欠压事件 - 事件记录单元2
    {{0x30010800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表欠压事件 - 事件记录单元3
    {{0x30010900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表欠压事件 - 事件记录单元4
    {{0x30010A00}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表欠压事件 - 当前值记录表
    {{0x30010A02}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30010A03}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30010A04}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表

    {{0x30010B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表欠压事件 - 上报标识
    {{0x30010C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表欠压事件 - 有效标识
    {{0x30010E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表欠压事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30020200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表过压事件 - 关联对象属性表
    {{0x30020300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表过压事件 - 当前记录数
    {{0x30020400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表过压事件 - 最大记录数
    {{0x30020500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_WD_T},          //电能表过压事件 - 配置参数
    {{0x30020600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过压事件 - 事件记录单元1
    {{0x30020700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过压事件 - 事件记录单元2
    {{0x30020800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过压事件 - 事件记录单元3
    {{0x30020900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过压事件 - 事件记录单元4
    {{0x30020A00}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表过压事件 - 当前值记录表
    {{0x30020A02}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30020A03}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30020A04}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表

    {{0x30020B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表过压事件 - 上报标识
    {{0x30020C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表过压事件 - 有效标识
    {{0x30020E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表过压事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30030200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表断相事件 - 关联对象属性表
    {{0x30030300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表断相事件 - 当前记录数
    {{0x30030400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表断相事件 - 最大记录数
    {{0x30030500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_3003_T},        //电能表断相事件 - 配置参数
    {{0x30030600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断相事件 - 事件记录单元1
    {{0x30030700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断相事件 - 事件记录单元2
    {{0x30030800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断相事件 - 事件记录单元3
    {{0x30030900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断相事件 - 事件记录单元4
    {{0x30030A00}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30030A02}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30030A03}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表
    {{0x30030A04}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTSTAT1_T},                 //电能表断相事件 - 当前值记录表

    {{0x30030B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表断相事件 - 上报标识
    {{0x30030C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表断相事件 - 有效标识
    {{0x30030E00}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_EVENTTIME1_T},           //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认
    {{0x30030E02}, 24,         DT_STRUCTURE,                    0, 0,              E_OOP_EVENTTIME1_T},           //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认
    {{0x30030E03}, 24,         DT_STRUCTURE,                    0, 0,              E_OOP_EVENTTIME1_T},           //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认
    {{0x30030E04}, 24,         DT_STRUCTURE,                    0, 0,              E_OOP_EVENTTIME1_T},           //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30040200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表失流事件 - 关联对象属性表
    {{0x30040300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表失流事件 - 当前记录数
    {{0x30040400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表失流事件 - 最大记录数
    {{0x30040500}, 24,         DT_STRUCTURE,            0, 0,         E_OOP_EVTCFG_3004_T},        //电能表失流事件 - 配置参数
    {{0x30040600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失流事件 - 事件记录单元1
    {{0x30040700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失流事件 - 事件记录单元2
    {{0x30040800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表失流事件 - 事件记录单元3
    {{0x30040900}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},             //电能表失流事件 - 事件记录单元4
    {{0x30040A00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},                 //电能表失流事件 - 当前值记录表
    {{0x30040B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表失流事件 - 上报标识
    {{0x30040C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表失流事件 - 有效标识
    {{0x30040E00}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},           //电能表失流事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30050200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表过流事件 - 关联对象属性表
    {{0x30050300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表过流事件 - 当前记录数
    {{0x30050400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表过流事件 - 最大记录数
    {{0x30050500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_ID_T},          //电能表过流事件 - 配置参数
    {{0x30050600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过流事件 - 事件记录单元1
    {{0x30050700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过流事件 - 事件记录单元2
    {{0x30050800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过流事件 - 事件记录单元3
    {{0x30050900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过流事件 - 事件记录单元4
    {{0x30050A00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},                 //电能表过流事件 - 当前值记录表
    {{0x30050B00}, 24,         DT_ENUM,                     0, 0,                  E_ENUM},                    //电能表过流事件 - 上报标识
    {{0x30050C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表过流事件 - 有效标识
    {{0x30050E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表过流事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30060200}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},               //电能表断流事件 - 关联对象属性表
    {{0x30060300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表断流事件 - 当前记录数
    {{0x30060400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表断流事件 - 最大记录数
    {{0x30060500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_3006_T},        //电能表断流事件 - 配置参数
    {{0x30060600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断流事件 - 事件记录单元1
    {{0x30060700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断流事件 - 事件记录单元2
    {{0x30060800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断流事件 - 事件记录单元3
    {{0x30060900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表断流事件 - 事件记录单元4
    {{0x30060A00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},                 //电能表断流事件 - 当前值记录表
    {{0x30060B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表断流事件 - 上报标识
    {{0x30060C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表断流事件 - 有效标识
    {{0x30060E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表断流事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30070200}, 24,         DT_ARRAY,                    0, 0,                  E_OOP_OADS_T},               //电能表功率反向事件 - 关联对象属性表
    {{0x30070300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表功率反向事件 - 当前记录数
    {{0x30070400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表功率反向事件 - 最大记录数
    {{0x30070500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_ID_T},          //电能表功率反向事件 - 配置参数
    {{0x30070600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表功率反向事件 - 事件记录单元1
    {{0x30070700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表功率反向事件 - 事件记录单元2
    {{0x30070800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表功率反向事件 - 事件记录单元3
    {{0x30070900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表功率反向事件 - 事件记录单元4
    {{0x30070A00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},                 //电能表功率反向事件 - 当前值记录表
    {{0x30070B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表功率反向事件 - 上报标识
    {{0x30070C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表功率反向事件 - 有效标识
    {{0x30070E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表功率反向事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30080200}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},               //电能表过载事件 - 关联对象属性表
    {{0x30080300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表过载事件 - 当前记录数
    {{0x30080400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表过载事件 - 最大记录数
    {{0x30080500}, 24,         DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_ID_T},          //电能表过载事件 - 配置参数
    {{0x30080600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过载事件 - 事件记录单元1
    {{0x30080700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过载事件 - 事件记录单元2
    {{0x30080800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过载事件 - 事件记录单元3
    {{0x30080900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},             //电能表过载事件 - 事件记录单元4
    {{0x30080A00}, 24,         DT_ARRAY,                    0, 0,                   E_NULL},                 //电能表过载事件 - 当前值记录表
    {{0x30080B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表过载事件 - 上报标识
    {{0x30080C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表过载事件 - 有效标识
    {{0x30080E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表过载事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x30090200}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},          //电能表正向有功需量超限事件 - 事件记录表
    {{0x30090300}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},               //电能表正向有功需量超限事件 - 关联对象属性表
    {{0x30090400}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表正向有功需量超限事件 - 当前记录数
    {{0x30090500}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表正向有功需量超限事件 - 最大记录数
    {{0x30090600}, 7,          DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_DD_T},          //电能表正向有功需量超限事件 - 配置参数
    {{0x30090700}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表正向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30090800}, 7,          DT_ENUM,                     0, 0,                  E_NULL},                    //电能表正向有功需量超限事件 - 上报标识
    {{0x30090900}, 7,          DT_BOOL,                     0, 0,                  E_NULL},                    //电能表正向有功需量超限事件 - 有效标识
    {{0x30090A00}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表正向有功需量超限事件 - 时间状态记录表
                                                                             
    {{0x300A0200}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},          //电能表反向有功需量超限事件 - 事件记录表
    {{0x300A0300}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},               //电能表反向有功需量超限事件 - 关联对象属性表
    {{0x300A0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表反向有功需量超限事件 - 当前记录数
    {{0x300A0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表反向有功需量超限事件 - 最大记录数
    {{0x300A0600}, 7,          DT_STRUCTURE,                0, 0,                  E_OOP_EVTCFG_DD_T},          //电能表反向有功需量超限事件 - 配置参数
    {{0x300A0700}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表反向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300A0800}, 7,          DT_ENUM,                     0, 0,                  E_NULL},                    //电能表反向有功需量超限事件 - 上报标识
    {{0x300A0900}, 7,          DT_BOOL,                     0, 0,                  E_NULL},                    //电能表反向有功需量超限事件 - 有效标识
    {{0x300A0A00}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表反向有功需量超限事件 - 时间状态记录表
                                                                             
    {{0x300B0200}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},               //电能表无功需量超限事件 - 关联对象属性表
    {{0x300B0300}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},                 //电能表无功需量超限事件 - 当前记录数
    {{0x300B0400}, 24,         DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表无功需量超限事件 - 最大记录数
    {{0x300B0500}, 24,         DT_STRUCTURE,                0, 0,                  E_NULL},          //电能表无功需量超限事件 - 配置参数
    {{0x300B0600}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},          //电能表无功需量超限事件 - 事件记录单元1
    {{0x300B0700}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},          //电能表无功需量超限事件 - 事件记录单元2
    {{0x300B0800}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},          //电能表无功需量超限事件 - 事件记录单元3
    {{0x300B0900}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},          //电能表无功需量超限事件 - 事件记录单元4
    {{0x300B0A00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},                 //电能表无功需量超限事件 - 当前值记录表
    {{0x300B0B00}, 24,         DT_ENUM,                     0, 0,                  E_NULL},                    //电能表无功需量超限事件 - 上报标识
    {{0x300B0C00}, 24,         DT_BOOL,                     0, 0,                  E_NULL},                    //电能表无功需量超限事件 - 有效标识
    {{0x300B0E00}, 24,         DT_ARRAY,                    0, 0,                  E_NULL},           //电能表无功需量超限事件 - 时间状态记录表,????是不是用这个结构体，待确认
                                                                             
    {{0x300C0200}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},              //电能表功率因数超下限事件 - 事件记录表
    {{0x300C0300}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},               //电能表功率因数超下限事件 - 关联对象属性表
    {{0x300C0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表功率因数超下限事件 - 当前记录数
    {{0x300C0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                  E_NULL},                   //电能表功率因数超下限事件 - 最大记录数
    {{0x300C0600}, 7,          DT_STRUCTURE,                0, 0,                  E_NULL},          //电能表功率因数超下限事件 - 配置参数
    {{0x300C0700}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表功率因数超下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300C0800}, 7,          DT_ENUM,                     0, 0,                  E_NULL},                    //电能表功率因数超下限事件 - 上报标识
    {{0x300C0900}, 7,          DT_BOOL,                     0, 0,                  E_NULL},                    //电能表功率因数超下限事件 - 有效标识
    {{0x300C0A00}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},            //电能表功率因数超下限事件 - 时间状态记录表
                                                                             
    {{0x300D0200}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},              //电能表全失压事件 - 事件记录表????
    {{0x300D0300}, 7,          DT_ARRAY,                    0, 0,                  E_NULL},               //电能表全失压事件 - 关联对象属性表
    {{0x300D0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表全失压事件 - 当前记录数
    {{0x300D0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表全失压事件 - 最大记录数
    {{0x300D0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表全失压事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300D0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表全失压事件 - 上报标识
    {{0x300D0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表全失压事件 - 有效标识
    {{0x300D0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表全失压事件 - 时间状态记录表
    {{0x300E0200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表辅助电源掉电事件 - 事件记录表
    {{0x300E0300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表辅助电源掉电事件 - 关联对象属性表
    {{0x300E0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表辅助电源掉电事件 - 当前记录数
    {{0x300E0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表辅助电源掉电事件 - 最大记录数
    {{0x300E0600}, 7,          DT_STRUCTURE,                0, 0,                   E_NULL},       //电能表辅助电源掉电事件 - 配置参数
    {{0x300E0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表辅助电源掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300E0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表辅助电源掉电事件 - 上报标识
    {{0x300E0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表辅助电源掉电事件 - 有效标识
    {{0x300E0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表辅助电源掉电事件 - 时间状态记录表
    {{0x300F0200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表电压逆相序事件 - 事件记录表
    {{0x300F0300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表电压逆相序事件 - 关联对象属性表
    {{0x300F0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电压逆相序事件 - 当前记录数
    {{0x300F0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电压逆相序事件 - 最大记录数
    {{0x300F0600}, 7,          DT_STRUCTURE,                0, 0,                   E_UNSIGNED},       //电能表电压逆相序事件 - 配置参数
    {{0x300F0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电压逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300F0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表电压逆相序事件 - 上报标识
    {{0x300F0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表电压逆相序事件 - 有效标识
    {{0x300F0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电压逆相序事件 - 时间状态记录表
    {{0x30100200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表电流逆相序事件 - 事件记录表
    {{0x30100300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表电流逆相序事件 - 关联对象属性表
    {{0x30100400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电流逆相序事件 - 当前记录数
    {{0x30100500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电流逆相序事件 - 最大记录数
    {{0x30100600}, 7,          DT_STRUCTURE,                0, 0,                   E_UNSIGNED},       //电能表电流逆相序事件 - 配置参数
    {{0x30100700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电流逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30100800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表电流逆相序事件 - 上报标识
    {{0x30100900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表电流逆相序事件 - 有效标识
    {{0x30100A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电流逆相序事件 - 时间状态记录表
    {{0x30110200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表掉电事件 - 事件记录表
    {{0x30110300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表掉电事件 - 关联对象属性表
    {{0x30110400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表掉电事件 - 当前记录数
    {{0x30110500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表掉电事件 - 最大记录数
    {{0x30110600}, 7,          DT_STRUCTURE,                0, 0,                   E_NULL},       //电能表掉电事件 - 配置参数
    {{0x30110700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30110800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表掉电事件 - 上报标识
    {{0x30110900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表掉电事件 - 有效标识
    {{0x30110A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表掉电事件 - 时间状态记录表
    {{0x30110B00}, 7,          DT_ENUM,                     0, 0,                  E_ENUM},                    //掉电事件上报标识
    {{0x30120200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},         //电能表编程事件 - 事件记录表
    {{0x30120300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表编程事件 - 关联对象属性表
    {{0x30120400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表编程事件 - 当前记录数
    {{0x30120500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表编程事件 - 最大记录数
    {{0x30120700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30120701}, 7,          DT_STRUCTURE,                0, 0,              E_OOP_EVTSTAT_T},

    {{0x30120800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表编程事件 - 上报标识
    {{0x30120900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表编程事件 - 有效标识
    {{0x30120A00}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENTTIME_T},            //电能表编程事件 - 时间状态记录表
    {{0x30120A01}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVENTTIME_T},

    {{0x30130200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表清零事件 - 事件记录表
    {{0x30130300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表清零事件 - 关联对象属性表
    {{0x30130400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表清零事件 - 当前记录数
    {{0x30130500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表清零事件 - 最大记录数
    {{0x30130700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表清零事件 - 当前值记录表????是不是用这个结构体，待确认

    {{0x30130701}, 7,          DT_STRUCTURE,                0, 0,               E_OOP_EVTSTAT_T},            //电能表清零事件 事件当前值

    {{0x30130800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表清零事件 - 上报标识
    {{0x30130900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表清零事件 - 有效标识
    {{0x30130A00}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENTTIME_T},            //电能表清零事件 - 时间状态记录表
    {{0x30130A01}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVENTTIME_T},            //电能表清零事件 - 时间状态记录表

    {{0x30140200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表需量清零事件 - 事件记录表
    {{0x30140300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表需量清零事件 - 关联对象属性表
    {{0x30140400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表需量清零事件 - 当前记录数
    {{0x30140500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表需量清零事件 - 最大记录数
    {{0x30140700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表需量清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30140701}, 7,          DT_STRUCTURE,                    0, 0,              E_OOP_EVTSTAT_T},            //电能表需量清零事件 - 当前值记录表????是不是用这个结构体，待确认

    {{0x30140800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表需量清零事件 - 上报标识
    {{0x30140900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表需量清零事件 - 有效标识
    {{0x30140A00}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENTTIME_T},            //电能表需量清零事件 - 时间状态记录表
    {{0x30140A01}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVENTTIME_T},

    {{0x30150200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},      //电能表事件清零事件 - 事件记录表
    {{0x30150300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表事件清零事件 - 关联对象属性表
    {{0x30150400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表事件清零事件 - 当前记录数
    {{0x30150500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表事件清零事件 - 最大记录数
    {{0x30150700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表事件清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30150701}, 7,          DT_STRUCTURE,                    0, 0,              E_OOP_EVTSTAT_T},

    {{0x30150800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表事件清零事件 - 上报标识
    {{0x30150900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表事件清零事件 - 有效标识
    {{0x30150A00}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENTTIME_T},            //电能表事件清零事件 - 时间状态记录表
    {{0x30150A01}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVENTTIME_T},

    {{0x30160200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表校时事件 - 事件记录表
    {{0x30160300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表校时事件 - 关联对象属性表
    {{0x30160400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表校时事件 - 当前记录数
    {{0x30160500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表校时事件 - 最大记录数
    {{0x30160700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表校时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30160701}, 7,          DT_STRUCTURE,                    0, 0,              E_OOP_EVTSTAT_T},

    {{0x30160800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表校时事件 - 上报标识
    {{0x30160900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表校时事件 - 有效标识
    {{0x30160A00}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENTTIME_T},            //电能表校时事件 - 时间状态记录表
    {{0x30160A01}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVENTTIME_T},

    {{0x30170200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},      //电能表时段表编程事件 - 事件记录表
    {{0x30170300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表时段表编程事件 - 关联对象属性表
    {{0x30170400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表时段表编程事件 - 当前记录数
    {{0x30170500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表时段表编程事件 - 最大记录数
    {{0x30170700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表时段表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30170800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表时段表编程事件 - 上报标识
    {{0x30170900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表时段表编程事件 - 有效标识
    {{0x30170A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表时段表编程事件 - 时间状态记录表
    {{0x30180200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表时区表编程事件 - 事件记录表
    {{0x30180300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表时区表编程事件 - 关联对象属性表
    {{0x30180400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表时区表编程事件 - 当前记录数
    {{0x30180500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表时区表编程事件 - 最大记录数
    {{0x30180700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表时区表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30180800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表时区表编程事件 - 上报标识
    {{0x30180900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表时区表编程事件 - 有效标识
    {{0x30180A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表时区表编程事件 - 时间状态记录表
    {{0x30190200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表周休日编程事件 - 事件记录表
    {{0x30190300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表周休日编程事件 - 关联对象属性表
    {{0x30190400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表周休日编程事件 - 当前记录数
    {{0x30190500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表周休日编程事件 - 最大记录数
    {{0x30190700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表周休日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30190800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表周休日编程事件 - 上报标识
    {{0x30190900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表周休日编程事件 - 有效标识
    {{0x30190A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表周休日编程事件 - 时间状态记录表
    {{0x301A0200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表结算日编程事件 - 事件记录表
    {{0x301A0300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表结算日编程事件 - 关联对象属性表
    {{0x301A0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表结算日编程事件 - 当前记录数
    {{0x301A0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表结算日编程事件 - 最大记录数
    {{0x301A0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表结算日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301A0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表结算日编程事件 - 上报标识
    {{0x301A0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表结算日编程事件 - 有效标识
    {{0x301A0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表结算日编程事件 - 时间状态记录表
    {{0x301B0200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表开盖事件 - 事件记录表
    {{0x301B0300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表开盖事件 - 关联对象属性表
    {{0x301B0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                    E_NULL},                  //电能表开盖事件 - 当前记录数
    {{0x301B0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表开盖事件 - 最大记录数
    {{0x301B0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表开盖事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301B0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表开盖事件 - 上报标识
    {{0x301B0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表开盖事件 - 有效标识
    {{0x301B0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表开盖事件 - 时间状态记录表
    {{0x301B0B00}, 7,          DT_ENUM,                     0, 0,                  E_ENUM},                    //开盖事件上报标识
    {{0x301C0200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表开端钮盒事件 - 事件记录表
    {{0x301C0300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表开端钮盒事件 - 关联对象属性表
    {{0x301C0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表开端钮盒事件 - 当前记录数
    {{0x301C0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表开端钮盒事件 - 最大记录数
    {{0x301C0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表开端钮盒事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301C0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表开端钮盒事件 - 上报标识
    {{0x301C0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表开端钮盒事件 - 有效标识
    {{0x301C0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表开端钮盒事件 - 时间状态记录表
    {{0x301D0200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表电压不平衡事件 - 事件记录表
    {{0x301D0300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表电压不平衡事件 - 关联对象属性表
    {{0x301D0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电压不平衡事件 - 当前记录数
    {{0x301D0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电压不平衡事件 - 最大记录数
    {{0x301D0600}, 7,          DT_STRUCTURE,                0, 0,                   E_OOP_CFG_LU_T},          //电能表电压不平衡事件 - 配置参数
    {{0x301D0700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表电压不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301D0701}, 7,          DT_STRUCTURE,               0, 0,              E_OOP_EVTSTAT_T},

    {{0x301D0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表电压不平衡事件 - 上报标识
    {{0x301D0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表电压不平衡事件 - 有效标识
    {{0x301D0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电压不平衡事件 - 时间状态记录表
    {{0x301E0200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表电流不平衡事件 - 事件记录表
    {{0x301E0300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表电流不平衡事件 - 关联对象属性表
    {{0x301E0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电流不平衡事件 - 当前记录数
    {{0x301E0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表电流不平衡事件 - 最大记录数
    {{0x301E0600}, 7,          DT_STRUCTURE,                0, 0,                   E_OOP_CFG_LU_T},          //电能表电流不平衡事件 - 配置参数
    {{0x301E0700}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVTSTAT_T},            //电能表电流不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301E0701}, 7,          DT_STRUCTURE,               0, 0,              E_OOP_EVTSTAT_T},

    {{0x301E0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表电流不平衡事件 - 上报标识
    {{0x301E0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表电流不平衡事件 - 有效标识
    {{0x301E0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表电流不平衡事件 - 时间状态记录表
    {{0x301F0200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表跳闸事件 - 事件记录表
    {{0x301F0300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表跳闸事件 - 关联对象属性表
    {{0x301F0400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表跳闸事件 - 当前记录数
    {{0x301F0500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表跳闸事件 - 最大记录数
    {{0x301F0700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表跳闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301F0800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表跳闸事件 - 上报标识
    {{0x301F0900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表跳闸事件 - 有效标识
    {{0x301F0A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表跳闸事件 - 时间状态记录表
    {{0x30200200}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_EVENT_T},              //电能表合闸事件 - 事件记录表
    {{0x30200300}, 7,          DT_ARRAY,                    0, 0,                   E_OOP_OADS_T},               //电能表合闸事件 - 关联对象属性表
    {{0x30200400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表合闸事件 - 当前记录数
    {{0x30200500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表合闸事件 - 最大记录数
    {{0x30200700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表合闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30200800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表合闸事件 - 上报标识
    {{0x30200900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表合闸事件 - 有效标识
    {{0x30200A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表合闸事件 - 时间状态记录表
    {{0x30210200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},     //电能表节假日编程事件 - 事件记录表
    {{0x30210300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表节假日编程事件 - 关联对象属性表
    {{0x30210400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表节假日编程事件 - 当前记录数
    {{0x30210500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表节假日编程事件 - 最大记录数
    {{0x30210700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表节假日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30210800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表节假日编程事件 - 上报标识
    {{0x30210900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表节假日编程事件 - 有效标识
    {{0x30210A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表节假日编程事件 - 时间状态记录表
    {{0x30220200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表有功组合方式编程事件 - 事件记录表
    {{0x30220300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表有功组合方式编程事件 - 关联对象属性表
    {{0x30220400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表有功组合方式编程事件 - 当前记录数
    {{0x30220500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表有功组合方式编程事件 - 最大记录数
    {{0x30220700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表有功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30220800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表有功组合方式编程事件 - 上报标识
    {{0x30220900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表有功组合方式编程事件 - 有效标识
    {{0x30220A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表有功组合方式编程事件 - 时间状态记录表
    {{0x30230200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表无功组合方式编程事件 - 事件记录表
    {{0x30230300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表无功组合方式编程事件 - 关联对象属性表
    {{0x30230400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表无功组合方式编程事件 - 当前记录数
    {{0x30230500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表无功组合方式编程事件 - 最大记录数
    {{0x30230700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表无功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30230800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表无功组合方式编程事件 - 上报标识
    {{0x30230900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表无功组合方式编程事件 - 有效标识
    {{0x30230A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表无功组合方式编程事件 - 时间状态记录表
    {{0x30240200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表费率参数表编程事件 - 事件记录表
    {{0x30240300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表费率参数表编程事件 - 关联对象属性表
    {{0x30240400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表费率参数表编程事件 - 当前记录数
    {{0x30240500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表费率参数表编程事件 - 最大记录数
    {{0x30240700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表费率参数表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30240800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表费率参数表编程事件 - 上报标识
    {{0x30240900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表费率参数表编程事件 - 有效标识
    {{0x30240A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表费率参数表编程事件 - 时间状态记录表
    {{0x30250200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表阶梯表编程事件 - 事件记录表
    {{0x30250300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表阶梯表编程事件 - 关联对象属性表
    {{0x30250400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表阶梯表编程事件 - 当前记录数
    {{0x30250500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表阶梯表编程事件 - 最大记录数
    {{0x30250700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表阶梯表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30250800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表阶梯表编程事件 - 上报标识
    {{0x30250900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表阶梯表编程事件 - 有效标识
    {{0x30250A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表阶梯表编程事件 - 时间状态记录表
    {{0x30260200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},              //电能表密钥更新事件 - 事件记录表
    {{0x30260300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表密钥更新事件 - 关联对象属性表
    {{0x30260400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表密钥更新事件 - 当前记录数
    {{0x30260500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表密钥更新事件 - 最大记录数
    {{0x30260700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表密钥更新事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30260800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表密钥更新事件 - 上报标识
    {{0x30260900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表密钥更新事件 - 有效标识
    {{0x30260A00}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表密钥更新事件 - 时间状态记录表
    {{0x30270200}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},       //电能表异常插卡事件 - 事件记录表
    {{0x30270300}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},               //电能表异常插卡事件 - 关联对象属性表
    {{0x30270400}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表异常插卡事件 - 当前记录数
    {{0x30270500}, 7,          DT_LONG_UNSIGNED,            0, 0,                   E_NULL},                   //电能表异常插卡事件 - 最大记录数
    {{0x30270700}, 7,          DT_ARRAY,                    0, 0,                   E_NULL},            //电能表异常插卡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30270800}, 7,          DT_ENUM,                     0, 0,                   E_NULL},                    //电能表异常插卡事件 - 上报标识
    {{0x30270900}, 7,          DT_BOOL,                     0, 0,                   E_NULL},                    //电能表异常插卡事件 - 有效标识
    {{0x30270A00}, 7,          DT_DOUBLE_LONG_UNSIGNED,     0, 0,                   E_NULL},                   //电能表异常插卡事件 - 非法插卡总次数
    {{0x30280200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //电能表购电记录 - 事件记录表
    {{0x30280300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表购电记录 - 关联对象属性表
    {{0x30280400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表购电记录 - 当前记录数
    {{0x30280500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表购电记录 - 最大记录数
    {{0x30280700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表购电记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30280800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表购电记录 - 上报标识
    {{0x30280900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表购电记录 - 有效标识
    {{0x30280A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表购电记录 - 时间状态记录表
    {{0x30290200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},          //电能表退费记录 - 事件记录表
    {{0x30290300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表退费记录 - 关联对象属性表
    {{0x30290400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表退费记录 - 当前记录数
    {{0x30290500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表退费记录 - 最大记录数
    {{0x30290700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表退费记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30290800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表退费记录 - 上报标识
    {{0x30290900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表退费记录 - 有效标识
    {{0x30290A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表退费记录 - 时间状态记录表
    {{0x302A0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表恒定磁场干扰事件 - 事件记录表
    {{0x302A0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表恒定磁场干扰事件 - 关联对象属性表
    {{0x302A0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表恒定磁场干扰事件 - 当前记录数
    {{0x302A0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表恒定磁场干扰事件 - 最大记录数
    {{0x302A0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表恒定磁场干扰事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302A0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表恒定磁场干扰事件 - 上报标识
    {{0x302A0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表恒定磁场干扰事件 - 有效标识
    {{0x302A0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表恒定磁场干扰事件 - 时间状态记录表
    {{0x302B0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表负荷开关误动作事件 - 事件记录表
    {{0x302B0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表负荷开关误动作事件 - 关联对象属性表
    {{0x302B0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表负荷开关误动作事件 - 当前记录数
    {{0x302B0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表负荷开关误动作事件 - 最大记录数
    {{0x302B0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表负荷开关误动作事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302B0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表负荷开关误动作事件 - 上报标识
    {{0x302B0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表负荷开关误动作事件 - 有效标识
    {{0x302B0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表负荷开关误动作事件 - 时间状态记录表
    {{0x302C0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表电源异常事件 - 事件记录表
    {{0x302C0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表电源异常事件 - 关联对象属性表
    {{0x302C0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表电源异常事件 - 当前记录数
    {{0x302C0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表电源异常事件 - 最大记录数
    {{0x302C0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表电源异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302C0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表电源异常事件 - 上报标识
    {{0x302C0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表电源异常事件 - 有效标识
    {{0x302C0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表电源异常事件 - 时间状态记录表
    {{0x302D0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表电流严重不平衡事件 - 事件记录表
    {{0x302D0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表电流严重不平衡事件 - 关联对象属性表
    {{0x302D0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表电流严重不平衡事件 - 当前记录数
    {{0x302D0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表电流严重不平衡事件 - 最大记录数
    {{0x302D0600}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_CFG_LU_T},          //电能表电流严重不平衡事件 - 配置参数
    {{0x302D0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表电流严重不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302D0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表电流严重不平衡事件 - 上报标识
    {{0x302D0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表电流严重不平衡事件 - 有效标识
    {{0x302D0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表电流严重不平衡事件 - 时间状态记录表
    {{0x302E0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表时钟故障事件 - 事件记录表
    {{0x302E0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表时钟故障事件 - 关联对象属性表
    {{0x302E0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表时钟故障事件 - 当前记录数
    {{0x302E0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表时钟故障事件 - 最大记录数
    {{0x302E0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},       //电能表时钟故障事件 - 配置参数
    {{0x302E0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表时钟故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302E0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表时钟故障事件 - 上报标识
    {{0x302E0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表时钟故障事件 - 有效标识
    {{0x302E0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表时钟故障事件 - 时间状态记录表
    {{0x302E0B00}, 7,          DT_ENUM,                     0, 0,                  E_ENUM},                    //时钟故障事件上报标识
    {{0x302F0200}, 7,          DT_ARRAY,                    0, 0,            E_OOP_EVENT_T},              //电能表计量芯片故障事件 - 事件记录表
    {{0x302F0300}, 7,          DT_ARRAY,                    0, 0,            E_OOP_OADS_T},               //电能表计量芯片故障事件 - 关联对象属性表
    {{0x302F0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表计量芯片故障事件 - 当前记录数
    {{0x302F0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表计量芯片故障事件 - 最大记录数
    {{0x302F0600}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_EVTCFG_DELAY_T},       //电能表计量芯片故障事件 - 配置参数
    {{0x302F0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表计量芯片故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302F0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表计量芯片故障事件 - 上报标识
    {{0x302F0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表计量芯片故障事件 - 有效标识
    {{0x302F0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表计量芯片故障事件 - 时间状态记录表
    {{0x30300200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},          //通信模块变更事件 - 事件记录表
    {{0x30300300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //通信模块变更事件 - 关联对象属性表
    {{0x30300400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //通信模块变更事件 - 当前记录数
    {{0x30300500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //通信模块变更事件 - 最大记录数
    {{0x30300600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},       //通信模块变更事件 - 配置参数
    {{0x30300700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //通信模块变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30300800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //通信模块变更事件 - 上报标识
    {{0x30300900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //通信模块变更事件 - 有效标识
    {{0x30300A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //通信模块变更事件 - 时间状态记录表
    {{0x30310200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30320200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},          //端子座过热报警事件
    {{0x30330200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30340200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30360200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30320200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30390200}, 24,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x303A0200}, 24,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x303B0200}, 24,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x303B0500}, 7,          DT_STRUCTURE,                0, 0,            E_OOP_CFG_LU_T},          //功率因数超下限参数
    {{0x303C0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30400200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30400600}, 7,          DT_STRUCTURE,                    0, 0,            E_OOP_LOAD_I0_EVECFG_T},
    {{0x30500200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x30510200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},
    {{0x37010200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},


    {{0x30600200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            
    {{0x30610200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            
    {{0x31000200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端初始化事件 - 事件记录表
    {{0x31000300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端初始化事件 - 关联对象属性表
    {{0x31000400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端初始化事件 - 当前记录数
    {{0x31000500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端初始化事件 - 最大记录数
    {{0x31000700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端初始化事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31000800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端初始化事件 - 上报标识
    {{0x31000900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端初始化事件 - 有效标识
    {{0x31000A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端初始化事件 - 时间状态记录表
    {{0x31010200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端版本变更事件 - 事件记录表
    {{0x31010300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端版本变更事件 - 关联对象属性表
    {{0x31010400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端版本变更事件 - 当前记录数
    {{0x31010500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端版本变更事件 - 最大记录数
    {{0x31010700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端版本变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31010800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端版本变更事件 - 上报标识
    {{0x31010900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端版本变更事件 - 有效标识
    {{0x31010A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端版本变更事件 - 时间状态记录表
    {{0x31040200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端状态量变位事件 - 事件记录表
    {{0x31040300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端状态量变位事件 - 关联对象属性表
    {{0x31040400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端状态量变位事件 - 当前记录数
    {{0x31040500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端状态量变位事件 - 最大记录数
    {{0x31040700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端状态量变位事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31040800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端状态量变位事件 - 上报标识
    {{0x31040900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端状态量变位事件 - 有效标识
    {{0x31040A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端状态量变位事件 - 时间状态记录表
    {{0x31050200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},        //电能表时钟超差事件 - 事件记录表
    {{0x31050300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表时钟超差事件 - 关联对象属性表
    {{0x31050400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表时钟超差事件 - 当前记录数
    {{0x31050500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表时钟超差事件 - 最大记录数
    {{0x31050600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表时钟超差事件 - 配置参数
    {{0x31050700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表时钟超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31050800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表时钟超差事件 - 上报标识
    {{0x31050900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表时钟超差事件 - 有效标识
    {{0x31050A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表时钟超差事件 - 时间状态记录表
    {{0x31060200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},           //终端停/上电事件 - 事件记录表
    {{0x31060300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端停/上电事件 - 关联对象属性表
    {{0x31060400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端停/上电事件 - 当前记录数
    {{0x31060500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端停/上电事件 - 最大记录数
    {{0x31060600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},         //终端停/上电事件 - 配置参数
    {{0x31060700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端停/上电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31060800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端停/上电事件 - 上报标识
    {{0x31060900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端停/上电事件 - 有效标识
    {{0x31060A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端停/上电事件 - 时间状态记录表
    {{0x31070200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端直流模拟量越上限事件 - 事件记录表
    {{0x31070300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端直流模拟量越上限事件 - 关联对象属性表
    {{0x31070400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端直流模拟量越上限事件 - 当前记录数
    {{0x31070500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端直流模拟量越上限事件 - 最大记录数
    {{0x31070600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},         //终端直流模拟量越上限事件 - 配置参数
    {{0x31070700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端直流模拟量越上限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31070800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端直流模拟量越上限事件 - 上报标识
    {{0x31070900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端直流模拟量越上限事件 - 有效标识
    {{0x31070A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端直流模拟量越上限事件 - 时间状态记录表
    {{0x31080200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端直流模拟量越下限事件 - 事件记录表
    {{0x31080300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端直流模拟量越下限事件 - 关联对象属性表
    {{0x31080400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端直流模拟量越下限事件 - 当前记录数
    {{0x31080500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端直流模拟量越下限事件 - 最大记录数
    {{0x31080600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},         //终端直流模拟量越下限事件 - 配置参数
    {{0x31080700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端直流模拟量越下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31080800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端直流模拟量越下限事件 - 上报标识
    {{0x31080900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端直流模拟量越下限事件 - 有效标识
    {{0x31080A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端直流模拟量越下限事件 - 时间状态记录表
    {{0x31090200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端消息认证错误事件 - 事件记录表
    {{0x31090300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端消息认证错误事件 - 关联对象属性表
    {{0x31090400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端消息认证错误事件 - 当前记录数
    {{0x31090500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端消息认证错误事件 - 最大记录数
    {{0x31090700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端消息认证错误事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31090800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端消息认证错误事件 - 上报标识
    {{0x31090900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端消息认证错误事件 - 有效标识
    {{0x31090A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端消息认证错误事件 - 时间状态记录表
    {{0x310A0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //设备故障记录 - 事件记录表
    {{0x310A0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //设备故障记录 - 关联对象属性表
    {{0x310A0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //设备故障记录 - 当前记录数
    {{0x310A0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //设备故障记录 - 最大记录数
    {{0x310A0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //设备故障记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310A0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //设备故障记录 - 上报标识
    {{0x310A0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //设备故障记录 - 有效标识
    {{0x310A0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //设备故障记录 - 时间状态记录表
    {{0x310B0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //电能表示度下降事件 - 事件记录表
    {{0x310B0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表示度下降事件 - 关联对象属性表
    {{0x310B0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表示度下降事件 - 当前记录数
    {{0x310B0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表示度下降事件 - 最大记录数
    {{0x310B0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表示度下降事件 - 配置参数
    {{0x310B0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表示度下降事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310B0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表示度下降事件 - 上报标识
    {{0x310B0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表示度下降事件 - 有效标识
    {{0x310B0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表示度下降事件 - 时间状态记录表
    {{0x310C0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //电能量超差事件 - 事件记录表
    {{0x310C0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能量超差事件 - 关联对象属性表
    {{0x310C0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能量超差事件 - 当前记录数
    {{0x310C0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能量超差事件 - 最大记录数
    {{0x310C0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能量超差事件 - 配置参数
    {{0x310C0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能量超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310C0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能量超差事件 - 上报标识
    {{0x310C0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能量超差事件 - 有效标识
    {{0x310C0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能量超差事件 - 时间状态记录表
    {{0x310D0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //电能表飞走事件 - 事件记录表
    {{0x310D0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表飞走事件 - 关联对象属性表
    {{0x310D0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表飞走事件 - 当前记录数
    {{0x310D0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表飞走事件 - 最大记录数
    {{0x310D0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表飞走事件 - 配置参数
    {{0x310D0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表飞走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310D0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表飞走事件 - 上报标识
    {{0x310D0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表飞走事件 - 有效标识
    {{0x310D0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表飞走事件 - 时间状态记录表
    {{0x310E0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //电能表停走事件 - 事件记录表
    {{0x310E0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表停走事件 - 关联对象属性表
    {{0x310E0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表停走事件 - 当前记录数
    {{0x310E0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表停走事件 - 最大记录数
    {{0x310E0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表停走事件 - 配置参数
    {{0x310E0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表停走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310E0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表停走事件 - 上报标识
    {{0x310E0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表停走事件 - 有效标识
    {{0x310E0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表停走事件 - 时间状态记录表
    {{0x310F0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端抄表失败事件 - 事件记录表
    {{0x310F0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端抄表失败事件 - 关联对象属性表
    {{0x310F0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端抄表失败事件 - 当前记录数
    {{0x310F0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端抄表失败事件 - 最大记录数
    {{0x310F0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //终端抄表失败事件 - 配置参数
    {{0x310F0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端抄表失败事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310F0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端抄表失败事件 - 上报标识
    {{0x310F0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端抄表失败事件 - 有效标识
    {{0x310F0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端抄表失败事件 - 时间状态记录表
    {{0x31100200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //月通信流量超限事件 - 事件记录表
    {{0x31100300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //月通信流量超限事件 - 关联对象属性表
    {{0x31100400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //月通信流量超限事件 - 当前记录数
    {{0x31100500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //月通信流量超限事件 - 最大记录数
    {{0x31100600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //月通信流量超限事件 - 配置参数
    {{0x31100700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //月通信流量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31100800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //月通信流量超限事件 - 上报标识
    {{0x31100900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //月通信流量超限事件 - 有效标识
    {{0x31100A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //月通信流量超限事件 - 时间状态记录表
    {{0x31110200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //发现未知电能表事件 - 事件记录表
    {{0x31110300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //发现未知电能表事件 - 关联对象属性表
    {{0x31110400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //发现未知电能表事件 - 当前记录数
    {{0x31110500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //发现未知电能表事件 - 最大记录数
    {{0x31110700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //发现未知电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31110800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //发现未知电能表事件 - 上报标识
    {{0x31110900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //发现未知电能表事件 - 有效标识
    {{0x31110A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //发现未知电能表事件 - 时间状态记录表
    {{0x31120200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},         //跨台区电能表事件 - 事件记录表
    {{0x31120300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //跨台区电能表事件 - 关联对象属性表
    {{0x31120400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //跨台区电能表事件 - 当前记录数
    {{0x31120500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //跨台区电能表事件 - 最大记录数
    {{0x31120700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //跨台区电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31120800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //跨台区电能表事件 - 上报标识
    {{0x31120900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //跨台区电能表事件 - 有效标识
    {{0x31120A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //跨台区电能表事件 - 时间状态记录表
    {{0x31140200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端对时事件 - 事件记录表
    {{0x31140300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端对时事件 - 关联对象属性表
    {{0x31140400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端对时事件 - 当前记录数
    {{0x31140500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端对时事件 - 最大记录数
    {{0x31140700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端对时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31140800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端对时事件 - 上报标识
    {{0x31140900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端对时事件 - 有效标识
    {{0x31140A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端对时事件 - 时间状态记录表
    {{0x31150200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},           //遥控跳闸记录 - 事件记录表
    {{0x31150300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //遥控跳闸记录 - 关联对象属性表
    {{0x31150400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //遥控跳闸记录 - 当前记录数
    {{0x31150500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //遥控跳闸记录 - 最大记录数
    {{0x31150700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //遥控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31150800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //遥控跳闸记录 - 上报标识
    {{0x31150900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //遥控跳闸记录 - 有效标识
    {{0x31150A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //遥控跳闸记录 - 时间状态记录表
    {{0x31160200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //有功总电能量差动越限事件记录 - 事件记录表
    {{0x31160300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //有功总电能量差动越限事件记录 - 关联对象属性表
    {{0x31160400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //有功总电能量差动越限事件记录 - 当前记录数
    {{0x31160500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //有功总电能量差动越限事件记录 - 最大记录数
    {{0x31160600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //有功总电能量差动越限事件记录 - 配置参数
    {{0x31160700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //有功总电能量差动越限事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31160800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //有功总电能量差动越限事件记录 - 上报标识
    {{0x31160900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //有功总电能量差动越限事件记录 - 有效标识
    {{0x31160A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //有功总电能量差动越限事件记录 - 时间状态记录表
    {{0x31170200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //输出回路接入状态变位事件记录 - 事件记录表
    {{0x31170300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //输出回路接入状态变位事件记录 - 关联对象属性表
    {{0x31170400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //输出回路接入状态变位事件记录 - 当前记录数
    {{0x31170500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //输出回路接入状态变位事件记录 - 最大记录数
    {{0x31170700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //输出回路接入状态变位事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31170800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //输出回路接入状态变位事件记录 - 上报标识
    {{0x31170900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //输出回路接入状态变位事件记录 - 有效标识
    {{0x31170A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //输出回路接入状态变位事件记录 - 时间状态记录表
    {{0x31180200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},         //终端编程记录 - 事件记录表
    {{0x31180300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端编程记录 - 关联对象属性表
    {{0x31180400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端编程记录 - 当前记录数
    {{0x31180500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端编程记录 - 最大记录数
    {{0x31180700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端编程记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31180800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端编程记录 - 上报标识
    {{0x31180900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端编程记录 - 有效标识
    {{0x31180A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端编程记录 - 时间状态记录表
    {{0x31190200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //终端电流回路异常事件 - 事件记录表
    {{0x31190300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端电流回路异常事件 - 关联对象属性表
    {{0x31190400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端电流回路异常事件 - 当前记录数
    {{0x31190500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端电流回路异常事件 - 最大记录数
    {{0x31190700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端电流回路异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31190800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端电流回路异常事件 - 上报标识
    {{0x31190900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端电流回路异常事件 - 有效标识
    {{0x31190A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端电流回路异常事件 - 时间状态记录表
    {{0x311A0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},        //电能表在网状态切换事件 - 事件记录表
    {{0x311A0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表在网状态切换事件 - 关联对象属性表
    {{0x311A0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表在网状态切换事件 - 当前记录数
    {{0x311A0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表在网状态切换事件 - 最大记录数
    {{0x311A0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表在网状态切换事件 - 配置参数
    {{0x311A0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表在网状态切换事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311A0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表在网状态切换事件 - 上报标识
    {{0x311A0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表在网状态切换事件 - 有效标识
    {{0x311A0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表在网状态切换事件 - 时间状态记录表
    {{0x311B0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},        //终端对电能表校时记录 - 事件记录表
    {{0x311B0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //终端对电能表校时记录 - 关联对象属性表
    {{0x311B0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端对电能表校时记录 - 当前记录数
    {{0x311B0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //终端对电能表校时记录 - 最大记录数
    {{0x311B0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端对电能表校时记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311B0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //终端对电能表校时记录 - 上报标识
    {{0x311B0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //终端对电能表校时记录 - 有效标识
    {{0x311B0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //终端对电能表校时记录 - 时间状态记录表
    {{0x311C0200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},      //电能表数据变更监控记录 - 事件记录表
    {{0x311C0300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电能表数据变更监控记录 - 关联对象属性表
    {{0x311C0400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表数据变更监控记录 - 当前记录数
    {{0x311C0500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电能表数据变更监控记录 - 最大记录数
    {{0x311C0600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},        //电能表数据变更监控记录 - 配置参数
    {{0x311C0700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表数据变更监控记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311C0800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电能表数据变更监控记录 - 上报标识
    {{0x311C0900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电能表数据变更监控记录 - 有效标识
    {{0x311C0A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电能表数据变更监控记录 - 时间状态记录表
    {{0x32000200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},           //功控跳闸记录 - 事件记录表
    {{0x32000300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //功控跳闸记录 - 关联对象属性表
    {{0x32000400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //功控跳闸记录 - 当前记录数
    {{0x32000500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //功控跳闸记录 - 最大记录数
    {{0x32000700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //功控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32000800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //功控跳闸记录 - 上报标识
    {{0x32000900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //功控跳闸记录 - 有效标识
    {{0x32000A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //功控跳闸记录 - 时间状态记录表
    {{0x32010200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},           //电控跳闸记录 - 事件记录表
    {{0x32010300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电控跳闸记录 - 关联对象属性表
    {{0x32010400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电控跳闸记录 - 当前记录数
    {{0x32010500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电控跳闸记录 - 最大记录数
    {{0x32010700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32010800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电控跳闸记录 - 上报标识
    {{0x32010900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电控跳闸记录 - 有效标识
    {{0x32010A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电控跳闸记录 - 时间状态记录表
    {{0x32020200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},              //购电参数设置记录 - 事件记录表
    {{0x32020300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //购电参数设置记录 - 关联对象属性表
    {{0x32020400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //购电参数设置记录 - 当前记录数
    {{0x32020500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //购电参数设置记录 - 最大记录数
    {{0x32020700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //购电参数设置记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32020800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //购电参数设置记录 - 上报标识
    {{0x32020900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //购电参数设置记录 - 有效标识
    {{0x32020A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //购电参数设置记录 - 时间状态记录表
    {{0x32030200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},           //电控告警事件记录 - 事件记录表
    {{0x32030300}, 7,          DT_ARRAY,                    0, 0,            E_NULL},               //电控告警事件记录 - 关联对象属性表
    {{0x32030400}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电控告警事件记录 - 当前记录数
    {{0x32030500}, 7,          DT_LONG_UNSIGNED,            0, 0,            E_NULL},                   //电控告警事件记录 - 最大记录数
    {{0x32030700}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电控告警事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32030800}, 7,          DT_ENUM,                     0, 0,            E_NULL},                    //电控告警事件记录 - 上报标识
    {{0x32030900}, 7,          DT_BOOL,                     0, 0,            E_NULL},                    //电控告警事件记录 - 有效标识
    {{0x32030A00}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //电控告警事件记录 - 时间状态记录表
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)  
    {{0x31300200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //温度超限事件
    {{0x33000200}, 7,          DT_ARRAY,                    8,8,            E_OOP_REPORTS_T},            //温度超限事件
    {{0x33090206}, 7,          DT_BIT_STRING,                    3,3,            E_OOP_BITSTR8_T},            //温度超限事件
    {{0x33500206}, 7,          DT_DOUBLE_LONG,                    5,5,            E_DOUBLE_LONG},           
    {{0x33500207}, 7,          DT_DOUBLE_LONG,                    5,5,            E_DOUBLE_LONG},           
    {{0x33500208}, 7,          DT_DOUBLE_LONG,                    5,5,            E_DOUBLE_LONG},           
    {{0x33510206}, 7,          DT_ENUM,                    2,2,            E_ENUM},           
    {{0x33510207}, 7,          DT_DOUBLE_LONG,                    5,5,            E_DOUBLE_LONG}, 
    {{0x33520206}, 7,          DT_STRUCTURE,                10,10,            E_TSA},            //温度超限事件
    {{0x33520207}, 7,          DT_ARRAY,                    10,10,            E_OOP_APPURTENANT_METER_LIST_T},            //温度超限事件
    {{0x30710200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //温度超限事件
    {{0x30710600}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},            //温度超限事件 - 温度阈值
    {{0x30910200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //烟雾报警事件
    {{0x30920200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //水浸报警事件
    {{0x39010200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //断路器合闸事件单元
    {{0x39020200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //断路器跳闸事件单元
    {{0x39030200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //开关窃电发生事件
    {{0x39040200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //开关窃电恢复事件
    {{0x39050200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //开关窃电恢复事件
#endif
    {{0x33270200}, 7,          DT_STRUCTURE,                0,0,            E_NULL},            //应用监测事件
    {{0x33270600}, 7,          DT_ENUM,                     2,1,            E_ENUM},            //应用监测事件
    {{0x33270700}, 7,          DT_LONG_UNSIGNED,            3,2,            E_LONG_UNSIGNED},           //应用监测事件
    {{0x33270800}, 7,          DT_LONG_UNSIGNED,            3,2,            E_LONG_UNSIGNED},           //应用监测事件
    {{0x33270900}, 7,          DT_VISIBLE_STRING,           34,32,          E_OOP_VISIBLEVAR32_T},          //应用监测事件    
    {{0x33270A00}, 7,          DT_VISIBLE_STRING,           34,32,          E_OOP_VISIBLEVAR32_T},          //应用监测事件
    {{0x33270B00}, 7,          DT_VISIBLE_STRING,           34,32,          E_OOP_VISIBLEVAR32_T},          //应用监测事件
#ifdef AREA_SHANDONG
    {{0x3E040200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //温度超限事件
    {{0x3E010200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //湿度超限事件
    {{0x3E020200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //门开关事件
    {{0x3E030200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //锁开关事件
    {{0x3E400200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //疑似窃电事件
    {{0x3E410200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //图片视频采集事件
    {{0x3E420200}, 7,          DT_ARRAY,                    0, 0,            E_NULL},            //强磁场事件
#endif
    {{0x31BB0200}, 7,          DT_STRUCTURE,                0, 0,            E_NULL},            //漏保跳闸};
};

/* 参变量类OAD列表 OIA1 = 0x04 */
const OadInfoMap ParaList[] =
{
    /*OAD          class   dataType                  len*/
      {{0x40000200}, 8,      DT_DATETIME_S,            8,8,E_DATETIME_S},         //短时间格式 日期-时间
      {{0x40000300}, 8,      DT_ENUM,                  2,2,E_ENUM},                    //校时模式
      {{0x40000400}, 8,      DT_STRUCTURE,             12,12,E_OOP_SYNCCLOCK_T},          //精准校时参数
      {{0x40000500}, 8,      DT_STRUCTURE,             8,8,E_OOP_BROADCASTCLOCK_T},          //广播校时参数
      {{0x40010200}, 8,      DT_OCTET_STRING,          10,10,E_OOP_OCTETVAR16_T},         //通信地址
      {{0x40020200}, 8,      DT_OCTET_STRING,          10,10,E_OOP_OCTETVAR16_T},         //表号
      {{0x40030200}, 8,      DT_OCTET_STRING,          10,10,E_OOP_OCTETVAR16_T},         //客户编号
      {{0x40040200}, 8,      DT_STRUCTURE,             27,27,E_OOP_GEOGADD_T},            //设备地理位置
      {{0x40050200}, 8,      DT_ARRAY,                 10,10,E_OOP_GRPADDR_T},            //组地址
      {{0x40070200}, 8,      DT_STRUCTURE,             27,27,E_OOP_LCD_T},            //LCD 参数
      {{0x40080200}, 8,      DT_DATETIME_S,            8,8,E_DATETIME_S},         //备用套时区表切换时间
      {{0x40090200}, 8,      DT_DATETIME_S,            8,8,E_DATETIME_S},         //备用套时区表切换时间
      {{0x400A0200}, 8,      DT_DATETIME_S,            8,8,E_DATETIME_S},       //备用套时区表切换时间
      {{0x400B0200}, 8,      DT_DATETIME_S,            8,8,E_DATETIME_S},       //备用套时区表切换时间
      {{0x400C0201}, 8,      DT_UNSIGNED,             2,2,E_UNSIGNED},             //时区时段数
      {{0x400C0202}, 8,      DT_UNSIGNED,             2,2,E_UNSIGNED},             //时区时段数
      {{0x400C0203}, 8,      DT_UNSIGNED,             2,2,E_UNSIGNED},             //时区时段数
      {{0x400C0204}, 8,      DT_UNSIGNED,             2,2,E_UNSIGNED},             //时区时段数
      {{0x400C0205}, 8,      DT_UNSIGNED,             2,2,E_UNSIGNED},             //时区时段数
      //{{0x400C0200}, 8,      DT_STRUCTURE,             12,12,E_OOP_PERIOD_T},             //时区时段数
      {{0x400D0200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //阶梯数
      {{0x400E0200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //谐波分析次数
      {{0x400F0200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //密钥总条数
      {{0x40100200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //计量元件数
      {{0x40110200}, 8,      DT_STRUCTURE,             13,12,E_OOP_PUBHOLIDAY_T},         //公共假日表
      {{0x40120200}, 8,      DT_BIT_STRING,            0,0,E_OOP_BITSTR8_T},        //周休日特征字
      {{0x40130200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                     //周休日釆用的日时段表号
      {{0x40140200}, 8,      DT_ARRAY,                 114,114,E_OOP_TIMEZONE_T},           //当前套时区表
      {{0x40150200}, 8,      DT_ARRAY,                 114,114,E_OOP_TIMEZONE_T},           //备用套时区表
      {{0x40160200}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160201}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160202}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160203}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160204}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160205}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160206}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160207}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40160208}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //当前套日时段表
      {{0x40170200}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170201}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170202}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170203}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170204}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170205}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170206}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170207}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40170208}, 8,      DT_ARRAY,                 114,114,E_OOP_DAYPERIOD_T},          //备用套日时段表
      {{0x40180200}, 8,      DT_ARRAY,                 0,0,E_OOP_RATETARIFF_T},         //当前套费率电价
      {{0x40190200}, 8,      DT_ARRAY,                 0,0,E_OOP_RATETARIFF_T},         //备用套费率电价
      {{0x401A0200}, 8,      DT_STRUCTURE,             0,0,E_OOP_STEPPRICE_T},          //当前套阶梯电价
      {{0x401B0200}, 8,      DT_STRUCTURE,             0,0,E_OOP_STEPPRICE_T},          //备用套阶梯电价
      {{0x401C0200}, 8,      DT_DOUBLE_LONG_UNSIGNED,  5,5,E_DOUBLE_LONG_UNSIGNED},                   //电流互感器变比
      {{0x401D0200}, 8,      DT_DOUBLE_LONG_UNSIGNED,  5,5,E_DOUBLE_LONG_UNSIGNED},                   //电压互感器变比
      {{0x401E0202}, 8,      DT_DOUBLE_LONG_UNSIGNED,             12,12,E_DOUBLE_LONG_UNSIGNED},        //报警金额限值
      //{{0x401E0200}, 8,      DT_STRUCTURE,             12,12,E_OOP_LIMITCHARGE_T},        //报警金额限值
      {{0x401F0200}, 8,      DT_STRUCTURE,             17,17,E_OOP_OTHERLIMIT_T},         //其它金额限值
      {{0x40200200}, 8,      DT_STRUCTURE,             12,12,E_OOP_POWERLIMIT_T},         //报警电量限值
      {{0x40210200}, 8,      DT_STRUCTURE,             17,17,E_OOP_OTHERPOWERLIMIT_T},    //其它电量限值
      {{0x40220200}, 8,      DT_BIT_STRING,            0,0,E_OOP_VISIBLESTR16_T},       //插卡状态字
      {{0x40270200}, 8,      DT_ENUM,                  2,2,E_ENUM},                       //升级结果
      {{0x40270300}, 8,      DT_ENUM,                  2,2,E_ENUM},                       //操作类型
      {{0x40300200}, 8,      DT_STRUCTURE,             14,14,E_OOP_VOLTPARAM_T},          //电压合格率参数
      {{0x40310200}, 8,      DT_ARRAY,                 149,149,E_OOP_METERMOD_LIST},      //模组列表
      {{0x40310300}, 8,      DT_ARRAY,                 149,149,E_OOP_MODVERSION_LIST},    //模组版本信息
      {{0x41030200}, 8,      DT_VISIBLE_STRING,        34,32,E_OOP_VISIBLEVAR32_T},       //资产管理码
      {{0x41100200}, 8,      DT_BIT_STRING,            3,3,   E_OOP_BITSTR8_T},       //有功组合方式特征字
      {{0x41110200}, 8,      DT_BIT_STRING,            0,0,   E_OOP_VISIBLEVAR32_T},       //软件备案号
      {{0x41110300}, 8,      DT_ARRAY,                 149,149,E_OOP_SOFTREC_LIST},       //软件备案号列表
      {{0x41120200}, 8,      DT_BIT_STRING,            3,3,   E_OOP_BITSTR8_T},       //有功组合方式特征字
      {{0x41130200}, 8,      DT_BIT_STRING,            3,3,   E_OOP_BITSTR8_T},       //无功组合方式1特征字
      {{0x41140200}, 8,      DT_BIT_STRING,            3,3,   E_OOP_BITSTR8_T},       //无功组合方式2特征字
      {{0x41160200}, 8,      DT_ARRAY,                 2,2,  E_OOP_PAYDAYS_T},            //结算日日期
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)   
      {{0x41210200}, 19,     DT_STRUCTURE,            10,10,   E_OOP_Power_Characteristic_Record},       //开关有功线损值
      {{0x41220200}, 19,     DT_STRUCTURE,            10,10,   E_OOP_Energy_Characteristic_Record},       //回路阻抗
      {{0x43201300}, 19,     DT_STRUCTURE,            10,10,   E_OOP_LINE_LOSS_T},       //开关有功线损值
      {{0x43201700}, 19,     DT_STRUCTURE,            10,10,   E_OOP_SWITCH_IMPEDANCE_T},       //回路阻抗
      {{0x43201701}, 19,     DT_STRUCTURE,            10,10,   E_OOP_SWITCH_IMPEDANCE_T},       //A相回路阻抗
      {{0x43201702}, 19,     DT_STRUCTURE,            10,10,   E_OOP_SWITCH_IMPEDANCE_T},       //B相回路阻抗
      {{0x43201703}, 19,     DT_STRUCTURE,            10,10,   E_OOP_SWITCH_IMPEDANCE_T},       //C相回路阻抗  
#endif 
      {{0x43000600}, 19,     DT_ARRAY,                 0,0,    E_OOP_PROTOCOL_T},           //电气设备-支持规约列表
      {{0x44000300}, 20,     DT_STRUCTURE,             23,23,  E_OOP_ASSOCIATION_CONTEXT_T},//协议版本号
      {{0x48000300}, 8,      DT_ARRAY,                 149,149,   E_OOP_DEVINSTANTDATAS_T},       //非介入式负荷感知模块，电器设备当前数据集,实际长度257
      {{0x48000400}, 8,      DT_ARRAY,                 92,92,   E_OOP_DEVFREZDATAS_T},       //非介入式负荷感知模块，电器设备冻结数据集
      {{0x48000500}, 8,      DT_ARRAY,                 149,149,   E_OOP_DEVSAMPDATAS_T},       //非介入式负荷感知模块，电器设备冻结数据集,实际长度212
      {{0x41000200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //最大需量周期
      {{0x41010200}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //滑差时间
      {{0x400C0201}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //年时区数 p≤14
      {{0x400C0202}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //日时段表数 q≤8
      {{0x400C0203}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //日时段数（每日切换数）m≤14
      {{0x400C0204}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //费率数 k≤12
      {{0x400C0205}, 8,      DT_UNSIGNED,              2,2,E_UNSIGNED},                    //公共假日数 n≤20
      {{0x43000700}, 19,     DT_BOOL,                  0,0,E_UNSIGNED},                    //电气设备-允许跟随上报
      {{0x43000800}, 19,     DT_BOOL,                  0,0,E_UNSIGNED},                    //电气设备-允许主动上报
      {{0x43000900}, 19,     DT_BOOL,                  0,0,E_NULL},                    //电气设备-允许与主站通话
      {{0x43000A00}, 19,     DT_ARRAY,                 0,0,E_OOP_CHANNEL_T},            //电气设备-上报通道
};

/* 冻结类OAD列表 OIA1 = 0x05 */
const OadInfoMap FreezeList[] = 
{
        /*OAD          class   dataType                  len*/
    {{0x50000200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //瞬时冻结 - 冻结数据表
    {{0x50000300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //瞬时冻结 - 关联对象属性表
    {{0x50010200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //秒冻结 - 冻结数据表
    {{0x50010300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //秒冻结 - 关联对象属性表
    {{0x50020200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //分钟冻结 - 冻结数据表
    {{0x50020300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //分钟冻结 - 关联对象属性表
    {{0x50030200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //小时冻结 - 冻结数据表
    {{0x50030300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //小时冻结 - 关联对象属性表
    {{0x50040200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //日冻结 - 冻结数据表
    {{0x50040300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //日冻结 - 关联对象属性表
    {{0x50050200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //结算日冻结 - 冻结数据表
    {{0x50050300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //结算日冻结 - 关联对象属性表
    {{0x50060200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //月冻结 - 冻结数据表
    {{0x50060300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //月冻结 - 关联对象属性表
    {{0x50070200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //年冻结 - 冻结数据表
    {{0x50070300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //年冻结 - 关联对象属性表
    {{0x50080200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //时区表切换冻结 - 冻结数据表
    {{0x50080300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //时区表切换冻结 - 关联对象属性表
    {{0x50090200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //日时段表切换冻结 - 冻结数据表
    {{0x50090300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //日时段表切换冻结 - 关联对象属性表
    {{0x500A0200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //费率电价切换冻结 - 冻结数据表
    {{0x500A0300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //费率电价切换冻结 - 关联对象属性表
    {{0x500B0200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //阶梯切换冻结 - 冻结数据表
    {{0x500B0300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //阶梯切换冻结 - 关联对象属性表
    {{0x50110200}, 9,      DT_ARRAY,                 1,1,E_NULL},           //阶梯结算冻结 - 冻结数据表
    {{0x50110300}, 9,      DT_ARRAY,                 1,1,E_OOP_FROZENSETS_T},           //阶梯结算冻结 - 关联对象属性表
};

/* 采集类OAD列表 OIA1 = 0x06 */
const OadInfoMap CollectList[] = 
{

    {{0x60020200}, 11,      DT_ARRAY,                 20,20,E_OOP_VISIBLEVAR32_T},           //瞬时冻结 - 冻结数据表
    {{0x60400200}, 8,      DT_DATETIME_S,                 8,8,E_DATETIME_S},           //采集启动时标
    {{0x60410200}, 8,      DT_DATETIME_S,                 8,8,E_DATETIME_S},           //采集成功时标
    {{0x60420200}, 8,      DT_DATETIME_S,                 8,8,E_DATETIME_S},           //采集存储时标
    {{0x6E430200}, 8,      DT_STRUCTURE,                 44,44,E_ABNORMAL_METER_INFO_T},           //采集存储时标
   
    


};

/* 集合类OAD列表 OIA1 = 0x07 */
const OadInfoMap GatherList[] = 
{};

/* 控制接口类OAD列表 OIA1 = 0x08 */
const OadInfoMap ControlList[] = 
{
    {{0x80000200}, 8,      DT_STRUCTURE,             10,10,E_OOP_TELECON_PARAM_T},      //远程控制 - 配置参数
};

/* 有序充电接口类OAD列表 OIA1 = 0x09 */
const OadInfoMap OrderedchargeList[] = 
{
    /*OAD          class   dataType                  len*/
      {{0x90030200}, 8,      DT_ARRAY,            0,0,E_OOP_PILE_WORK_STATUS_T},         //能源路由器工作状态数据项
      {{0x90040400}, 8,      DT_ARRAY,            0,0,E_OOP_CHARGE_SERVICE_T},           //充电服务数据项
};

/* 文件传输接口类OAD列表 OIA1 = 0x0F OIA2 = 0x00 */
const OadInfoMap FileList[] = 
{};

/* ESAM接口类OAD列表 OIA1 = 0x0F OIA2 = 0x01 */
const OadInfoMap EsamList[] = 
{
    {{0xf1000200},  21,  DT_OCTET_STRING,17,17,E_OOP_OCTETVAR64_T },   // ESAM序列号
    {{0xf1000400},  21,  DT_OCTET_STRING,17,17,E_OOP_OCTETVAR64_T },   // ESAM对称密钥版本
    {{0xf1000500},  21,  DT_LONG64_UNSIGNED,8,8,E_DOUBLE_LONG_UNSIGNED },   // 会话时效门限
    {{0xf1000700},  21,  DT_STRUCTURE,17,17,E_ESAM_COUNT },   // ESAM计数器
    {{0xF1010200},  8,   DT_ENUM,2,2,E_SECURITY_FLAG},       //安全模式开关
    {{0xF1001100}, 21,   DT_UNSIGNED,4,4,E_UNSIGNED},                   //身份认证启用
    {{0xF1001500}, 21,   DT_STRUCTURE,16,16,E_OOP_TERMINAL_LIMIT_T},                   //终端会话门限
    {{0xF1000E00}, 21,   DT_DOUBLE_LONG_UNSIGNED,8,8,E_DOUBLE_LONG_UNSIGNED},                   //红外认证时效门限
};

/* 输入输出类OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
const OadInfoMap IoList[] = 
{
    {{0xF2010200}, 22,   DT_ARRAY,0,0,E_OOP_RS485_T},              //RS485列表参数
    {{0xF2010201}, 22,   DT_STRUCTURE,0,0,E_OOP_RS485_T},              //RS485列表参数
    {{0xF2010202}, 22,   DT_STRUCTURE,0,0,E_OOP_RS485_T},              //RS485列表参数
    {{0xF2020200}, 22,   DT_ARRAY,0,0,E_OOP_IR_T},                 //红外输出参数
    {{0xF2050200}, 22,   DT_ARRAY,0,0,E_OOP_SWITCH_OUT_T},        //继电器
    {{0xF2090200}, 22,   DT_ARRAY,0,0,E_OOP_ROUTEMODULE_T},        //载波/微功率无线接口，本地通信模块单元参数
    {{0xF2090201}, 22,   DT_STRUCTURE,0,0,E_OOP_ROUTEMODULE_T},        //载波/微功率无线接口，本地通信模块单元参数
};

/* 显示类OAD列表 OIA1 = 0x0F OIA2 = 0x03 */
const OadInfoMap GuiList[] = 
{
    {{0xf3000300},  17,  DT_LONG_UNSIGNED,            0, 0,            E_LONG_UNSIGNED },   // 自动轮显每屏显示时间
    {{0xf3000400},  17,  DT_STRUCTURE,                0, 0,            E_OOP_OBJ_SHOW_PARAM_T },   // 按键显示屏数
    {{0xf3010200},  17,  DT_ARRAY,                    1, 1,            E_OOP_OBJ_SHOW_T },   // 显示内容
    {{0xf3010300},  17,  DT_LONG_UNSIGNED,            0, 0,            E_LONG_UNSIGNED },   // 按键轮显每屏显示时间
    {{0xf3010400},  17,  DT_STRUCTURE,                0, 0,            E_OOP_OBJ_SHOW_PARAM_T },   // 循环显示屏数
};

const MapOopTo645_NEW MapOopTo645_RT[]= //实时数据映射表
{
    {{0x00000200},  0,  0x0000FF00,4,20,0xFFFF,E_OOP_ENERGYS_T,   0, },   // (当前)组合有功电能块
    {{0x00000201}, -2,  0x00000000,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },   // 4字节, XXXXXX.XX(当前)组合有功总电能
    {{0x00000202}, -2,  0x00000100,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率1组合有功电能
    {{0x00000203}, -2,  0x00000200,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率2组合有功电能
    {{0x00000204}, -2,  0x00000300,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率3组合有功电能
    {{0x00000205}, -2,  0x00000400,4,4, 0xFFFF,E_OOP_ENERGYS_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率4组合有功电能

    // (当前)正向有功总电能
    {{0x00100200}, -2,  0x0001FF00,4,20,0x901F,E_OOP_ENERGY_T, -2, },    // (当前)正向有功电能块                                                                                                                                                                                                              }
    {{0x00100201}, -2,  0x00010000,4,4, 0x9010,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00100202}, -2,  0x00010100,4,4, 0x9011,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率1正向有功电能
    {{0x00100203}, -2,  0x00010200,4,4, 0x9012,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率2正向有功电能
    {{0x00100204}, -2,  0x00010300,4,4, 0x9013,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率3正向有功电能
    {{0x00100205}, -2,  0x00010400,4,4, 0x9014,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率4正向有功电能

    // (当前)A相正向有功总电能        
    {{0x00110200}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    {{0x00110201}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    // (当前)B相正向有功总电能    
    {{0x00120200}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    {{0x00120201}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    // (当前)C相正向有功总电能            
    {{0x00130200}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相正向有功总电能
    {{0x00130201}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相正向有功总电能

    // (当前)A相反向有功总电能            
    {{0x00210200}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    {{0x00210201}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    // (当前)B相反向有功总电能            
    {{0x00220200}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    {{0x00220201}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    // (当前)C相反向有功总电能            
    {{0x00230200}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    {{0x00230201}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    
    {{0x00310200}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能
    {{0x00310201}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能

    {{0x00320200}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能
    {{0x00320201}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能

    {{0x00330200}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能
    {{0x00330201}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能

    {{0x00410200}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能
    {{0x00410201}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能

    {{0x00420200}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能
    {{0x00420201}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能

    {{0x00430200}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能
    {{0x00430201}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能

    //(当前)反向有功总电能
    {{0x00200200}, -2,  0x0002FF00,4,20,0x902F,E_OOP_ENERGY_T, -2, },    // (当前)反向有功电能块
    {{0x00200201}, -2,  0x00020000,4,4, 0x9020,E_OOP_ENERGY_T, -2, },    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00200202}, -2,  0x00020100,4,4, 0x9021,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率1反向有功电能
    {{0x00200203}, -2,  0x00020200,4,4, 0x9022,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率2反向有功电能
    {{0x00200204}, -2,  0x00020300,4,4, 0x9023,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率3反向有功电能
    {{0x00200205}, -2,  0x00020400,4,4, 0x9024,E_OOP_ENERGY_T,  -2, },   // 4字节, XXXXXX.XX(当前)费率4反向有功电能

    //(当前)正向无功总电能(+RL,+RC)
    {{0x00300200}, -2,  0x0003FF00,4,20,0x911F,E_OOP_ENERGY_T, -2, },    // (当前)正向无功电能块(+RL,+RC)
    {{0x00300201}, -2,  0x00030000,4,4, 0x9110,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00300202}, -2,  0x00030100,4,4, 0x9111,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率1正向无功电能
    {{0x00300203}, -2,  0x00030200,4,4, 0x9112,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率2正向无功电能
    {{0x00300204}, -2,  0x00030300,4,4, 0x9113,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率3正向无功电能
    {{0x00300205}, -2,  0x00030400,4,4, 0x9114,E_OOP_ENERGY_T,  -2, },    // 4字节,XXXXXX.XX (当前)费率4正向无功电能

    //(当前)反向无功总电能
    {{0x00400200}, -2,  0x0004FF00,4,20,0x912F,E_OOP_ENERGY_T, -2, },    // (当前)反向无功电能块(-RL,-RC)
    {{0x00400201}, -2,  0x00040000,4,4, 0x9120,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00400202}, -2,  0x00040100,4,4, 0x9121,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1反向无功电能
    {{0x00400203}, -2,  0x00040200,4,4, 0x9122,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2反向无功电能
    {{0x00400204}, -2,  0x00040300,4,4, 0x9123,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3反向无功电能
    {{0x00400205}, -2,  0x00040400,4,4, 0x9124,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4反向无功电能

    //一象限无功总电能
    {{0x00500200}, -2,  0x0005FF00,4,20,0x913F,E_OOP_ENERGY_T, -2, },    // (当前)一象限无功电能块(+RL)
    {{0x00500201}, -2,  0x00050000,4,4, 0x9130,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00500202}, -2,  0x00050100,4,4, 0x9131,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1一象限无功电能
    {{0x00500203}, -2,  0x00050200,4,4, 0x9132,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2一象限无功电能
    {{0x00500204}, -2,  0x00050300,4,4, 0x9133,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3一象限无功电能
    {{0x00500205}, -2,  0x00050400,4,4, 0x9134,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4一象限无功电能

    //二象限无功总电能
    {{0x00600200}, -2,  0x0006FF00,4,20,0x915F,E_OOP_ENERGY_T, -2, },    // (当前)二象限无功电能块(+Rc)
    {{0x00600201}, -2,  0x00060000,4,4, 0x9150,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00600202}, -2,  0x00060100,4,4, 0x9151,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1二象限无功电能
    {{0x00600203}, -2,  0x00060200,4,4, 0x9152,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2二象限无功电能
    {{0x00600204}, -2,  0x00060300,4,4, 0x9153,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3二象限无功电能
    {{0x00600205}, -2,  0x00060400,4,4, 0x9154,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4二象限无功电能

    //三象限无功总电能
    {{0x00700200}, -2,  0x0007FF00,4,20,0x916F,E_OOP_ENERGY_T, -2, },    // (当前)三象限无功电能块(-RL)
    {{0x00700201}, -2,  0x00070000,4,4, 0x9160,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00700202}, -2,  0x00070100,4,4, 0x9161,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1三象限无功电能
    {{0x00700203}, -2,  0x00070200,4,4, 0x9162,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2三象限无功电能
    {{0x00700204}, -2,  0x00070300,4,4, 0x9163,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3三象限无功电能
    {{0x00700205}, -2,  0x00070400,4,4, 0x9164,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4三象限无功电能

    //四象限无功总电能
    {{0x00800200}, -2,  0x0008FF00,4,20,0x914F,E_OOP_ENERGY_T, -2, },    // (当前)四象限无功电能块(-Rc)
    {{0x00800201}, -2,  0x00080000,4,4, 0x9140,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x00800202}, -2,  0x00080100,4,4, 0x9141,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率1四象限无功电能
    {{0x00800203}, -2,  0x00080200,4,4, 0x9142,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率2四象限无功电能
    {{0x00800204}, -2,  0x00080300,4,4, 0x9143,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率3四象限无功电能
    {{0x00800205}, -2,  0x00080400,4,4, 0x9144,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX (当前)费率4四象限无功电能

    //(当前)正向有功总最大需量
    {{0x10100200},  0,   0x0101FF00,    8,40,   0xA01F,E_OOP_DEMAND_T,  0,       },  // (当前)正向有功最大需量块
    {{0x10100200},  0,   0x0101FF00,    8,40,   0xB01F,E_OOP_DEMAND_T,  0,       },  // (当前)正向有功最大需量块
    {{0x10100201},  0,   0x01010000,    8,8,    0xA010,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x10100201},  0,   0x01010000,    8,8,    0xB010,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x10100202},  0,   0x01010100,    8,8,    0xA011,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1正向有功最大需量
    {{0x10100202},  0,   0x01010100,    8,8,    0xB011,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1正向有功最大需量
    {{0x10100203},  0,   0x01010200,    8,8,    0xA012,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2正向有功最大需量
    {{0x10100203},  0,   0x01010200,    8,8,    0xB012,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2正向有功最大需量
    {{0x10100204},  0,   0x01010300,    8,8,    0xA013,E_OOP_DEMAND_T,  0       },  // 3字节, XX.XXXX,  (当前)费率3正向有功最大需量
    {{0x10100204},  0,   0x01010300,    8,8,    0xB013,E_OOP_DEMAND_T,  0       },  // 3字节, XX.XXXX,  (当前)费率3正向有功最大需量
    {{0x10100205},  0,   0x01010400,    8,8,    0xA014,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4正向有功最大需量
    {{0x10100205},  0,   0x01010400,    8,8,    0xB014,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4正向有功最大需量

    //(当前)反向有功总最大需量
    {{0x10200200},  0,   0x0102FF00,    8,40,   0xA02F,E_OOP_DEMAND_T,  -0,       },  // (当前)反向有功最大需量块
    {{0x10200200},  0,   0x0102FF00,    8,40,   0xB02F,E_OOP_DEMAND_T,  -0,       },  // (当前)反向有功最大需量块
    {{0x10200201},  0,   0x01020000,    8,8,    0xA020,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)反向有功总最大需量
    {{0x10200201},  0,   0x01020000,    8,8,    0xB020,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)反向有功总最大需量
    {{0x10200202},  0,   0x01020100,    8,8,    0xA021,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1反向有功最大需量
    {{0x10200202},  0,   0x01020100,    8,8,    0xB021,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1反向有功最大需量
    {{0x10200203},  0,   0x01020200,    8,8,    0xA022,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功向有功最大需量
    {{0x10200203},  0,   0x01020200,    8,8,    0xB022,E_OOP_DEMAND_T,  -0,      },  // 3字节, XX.XXXX,  (当前)费率2反向无功向有功最大需量
    {{0x10200204},  0,   0x01020300,    8,8,    0xA023,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功向有功最大需量
    {{0x10200204},  0,   0x01020300,    8,8,    0xB023,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功向有功最大需量
    {{0x10200205},  0,   0x01020400,    8,8,    0xA024,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功向有功最大需量
    {{0x10200205},  0,   0x01020400,    8,8,    0xB024,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功向有功最大需量

    //(当前)正向无功总最大需量
    {{0x10300200}, 0,    0x0103FF00,    8,40,   0xA11F,E_OOP_DEMAND_T,  -0,       },  //  (当前)正向无功最大需量块
    {{0x10300200}, 0,    0x0103FF00,    8,40,   0xB11F,E_OOP_DEMAND_T,  -0,       },  //  (当前)正向无功最大需量块
    {{0x10300201}, 0,    0x01030000,    8,8,    0xA110,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)正向无功总最大需量
    {{0x10300201}, 0,    0x01030000,    8,8,    0xB110,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)正向无功总最大需量
    {{0x10300202}, 0,    0x01030100,    8,8,    0xA111,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1正向无功最大需量
    {{0x10300202}, 0,    0x01030100,    8,8,    0xB111,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率1正向无功最大需量  
    {{0x10300203}, 0,    0x01030200,    8,8,    0xA112,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2正向无功最大需量
    {{0x10300203}, 0,    0x01030200,    8,8,    0xB112,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率2正向无功最大需量
    {{0x10300204}, 0,    0x01030300,    8,8,    0xA113,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3正向无功最大需量
    {{0x10300204}, 0,    0x01030300,    8,8,    0xB113,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率3正向无功最大需量
    {{0x10300205}, 0,    0x01030400,    8,8,    0xA114,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4正向无功最大需量
    {{0x10300205}, 0,    0x01030400,    8,8,    0xB114,E_OOP_DEMAND_T,  -0,       },  // 3字节, XX.XXXX,  (当前)费率4正向无功最大需量

    //反向无功总最大需量
    {{0x10400200}, 0,    0x0104FF00,    8,40,0xA12F,E_OOP_DEMAND_T,  0,       },  //(当前)反向无功最大需量块
    {{0x10400200}, 0,    0x0104FF00,    8,40,0xB12F,E_OOP_DEMAND_T,  0,       },  //(当前)反向无功最大需量块
    {{0x10400201}, 0,    0x01040000,    8,8,0xA120,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前反向无功总最大需量
    {{0x10400201}, 0,    0x01040000,    8,8,0xB120,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前反向无功总最大需量
    {{0x10400202}, 0,    0x01040100,    8,8,0xA121,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1反向无功最大需量
    {{0x10400202}, 0,    0x01040100,    8,8,0xB121,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1反向无功最大需量
    {{0x10400203}, 0,    0x01040200,    8,8,0xA122,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功最大需量
    {{0x10400203}, 0,    0x01040200,    8,8,0xB122,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2反向无功最大需量
    {{0x10400204}, 0,    0x01040300,    8,8,0xA123,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功最大需量
    {{0x10400204}, 0,    0x01040300,    8,8,0xB123,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3反向无功最大需量
    {{0x10400205}, 0,    0x01040400,    8,8,0xA124,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功最大需量
    {{0x10400205}, 0,    0x01040400,    8,8,0xB124,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4反向无功最大需量
    //分相正向有功总最大需量
    {{0x10110201},  0,0x01150000,   8,8,    0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 3字节, XX.XXXX,  (当前)A相正向有功总最大需量

    {{0x10120201},  0,0x01290000,   8,8,    0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 35字节,XX.XXXX,  (当前)B相正向有功总最大需量

    {{0x10130201},  0,0x013D0000,   8,8,    0xFFFF,E_OOP_DEMAND_T,  -0,       },    // 3字节, XX.XXXX,  (当前)C相正向有功总最大需量

    //电压
    {{0x20000200},             -1,  0x0201FF00,2,6,0xB61F,E_OOP_WORD3V_T,  -1, },   // 电压数据块
    {{0x20000201},             -1,  0x02010100,2,2,0xB610,E_OOP_WORD3V_T,  -1, },   // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20000202},             -1,  0x02010200,2,2,0xB611,E_OOP_WORD3V_T,  -1, },   // 2字节, X.XXX B相电压/无/无
    {{0x20000203},             -1,  0x02010300,2,2,0xB612,E_OOP_WORD3V_T,  -1, },   // 2字节, X.XXX C相电压/CB线电压/无
    //电流
    {{0x20010200},             -3,  0x0202FF00,3,9,0xB62F,E_OOP_INT3V_T,  -3, },  // 电流数据块
    {{0x20010201},             -3,  0x02020100,3,3,0xB620,E_OOP_INT3V_T,  -3, },  // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20010202},             -3,  0x02020200,3,3,0xB621,E_OOP_INT3V_T,  -3, },  // 3字节, XXX.XXX, B相电流/无/无
    {{0x20010203},             -3,  0x02020300,3,3,0xB622,E_OOP_INT3V_T,  -3, },  // 3字节, XXX.XXX, C相电流/C相电流/无
    {{0x20010400},             -3,  0x02800001,3,3,0xFFFF,E_DOUBLE_LONG,  -3, },  // 3字节, XXX.XXX, C相电流/C相电流/无
#ifndef AREA_HUNAN
    {{0x20010500},             -3,  0x02900100,2,2,0xFFFF,E_LONG,  -3, },  // 2字节, XXXX 浙江漏保当前剩余电流
#else
    {{0x20010500},             -3,  0x02900100,2,2,0xFFFF,E_DOUBLE_LONG,  -3, },  // 2字节, XXXX 湖南剩余电流
#endif
    //电压电流相角
    {{0x20030200},         -1,  0x0207FF00,2,6,0xFFFF,E_OOP_WORD3V_T,  -1, },  // 电压电流相角数据块
    {{0x20030201},         -1,  0x02070100,2,2,0xFFFF,E_OOP_WORD3V_T,  -1, },  // 2字节, XXX.X, A相电压电流相角
    {{0x20030202},         -1,  0x02070200,2,2,0xFFFF,E_OOP_WORD3V_T,  -1, },  // 2字节, XXX.X, B相电压电流相角
    {{0x20030203},         -1,  0x02070300,2,2,0xFFFF,E_OOP_WORD3V_T,  -1, },  // 2字节, XXX.X, C相电压电流相角

    //瞬时正向有功功率
    {{0x20040200},           -1,    0x0203FF00,3,12,0xB63F,E_OOP_INT4V_T,   -4, },   // 瞬时正向有功功率块
    {{0x20040201},             -1,  0x02030000,3,3, 0xB630,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
    {{0x20040202},             -1,  0x02030100,3,3, 0xB631,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20040203},             -1,  0x02030200,3,3, 0xB632,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, B相有功功率,有正负,格式同上
    {{0x20040204},             -1,  0x02030300,3,3, 0xB633,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, C相有功功率,有正负,格式同上

    //瞬时正向无功功率
    {{0x20050200},           -1,    0x0204FF00,3,12,0xB64F,E_OOP_INT4V_T,   -4, },   //
    {{0x20050201},             -1,  0x02040000,3,3, 0xB640,E_OOP_INT4V_T,   -4, },   // 瞬时正向无功功率块
    {{0x20050202},             -1,  0x02040100,3,3, 0xB641,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20050203},             -1,  0x02040200,3,3, 0xB642,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, B相无功功率,格式同上
    {{0x20050204},             -1,  0x02040300,3,3, 0xB643,E_OOP_INT4V_T,   -4, },   // 3字节, XX.XXXX, C相无功功率,格式同上

    //瞬时总功率因数
    {{0x200A0200},           -3,    0x0206FF00,2,8,0xB65F,E_OOP_LONG4V_T,   -3, },   // 瞬时功率因数块
    {{0x200A0201},             -3,  0x02060000,2,2,0xB650,E_OOP_LONG4V_T,   -3, },   // 2字节, XX.XXXX, 瞬时总功率因数
    {{0x200A0202},             -3,  0x02060100,2,2,0xB651,E_OOP_LONG4V_T,   -3, },   // 2字节, XX.XXXX, A相功率因数
    {{0x200A0203},             -3,  0x02060200,2,2,0xB652,E_OOP_LONG4V_T,   -3, },   // 2字节, XX.XXXX, B相功率因数
    {{0x200A0204},             -3,  0x02060300,2,2,0xB653,E_OOP_LONG4V_T,   -3, },   // 2字节, XX.XXXX, C相功率因数
    {{0x200F0200},             -2,  0x02800002,2,2,0xFFFF,E_LONG_UNSIGNED,  -2, },   // 2字节, XX.XXXX, 电网频率
    {{0x20100200},             -1,  0x02800007,2,2,0xFFFF,E_LONG,           -1, },   // 2字节, XX.XXXX, 表内温度
    {{0x20110200},             -2,  0x02800008,2,2,0xFFFF,E_LONG_UNSIGNED,  -2, },   // 2字节, XX.XXXX, 时钟电池电压
    {{0x20120200},             -2,  0x02800009,2,2,0xFFFF,E_LONG_UNSIGNED,  -2, },   // 2字节, XX.XXXX, 停电抄表电池电压
    {{0x20130200},              0,  0x0280000A,4,4,0xFFFF,E_DOUBLE_LONG_UNSIGNED,    0, },   // 4字节, XX.XXXX, 电网频率
    //瞬时视在功率
    {{0x20060200},             -1, 0x0205FF00,3,12,0xB66F,E_OOP_INT4V_T,  -4, },    // 瞬时视在功率块
    {{0x20060201},             -1, 0x02050000,3,3, 0xB660,E_OOP_INT4V_T,  -4, },    // 3字节, XX.XXXX, 瞬时视在功率
    {{0x20060202},             -1, 0x02050100,3,3, 0xB661,E_OOP_INT4V_T,  -4, },    // 3字节, XX.XXXX, A相视在功率
    {{0x20060203},             -1, 0x02050200,3,3, 0xB662,E_OOP_INT4V_T,  -4, },    // 3字节, XX.XXXX, B相视在功率
    {{0x20060204},             -1, 0x02050300,3,3, 0xB663,E_OOP_INT4V_T,  -4, },    // 3字节, XX.XXXX, C相视在功率

    {{0x20140200},         0, 0x040005FF,2,14,0xFFFF,E_OOP_METWORD_T,   0,},    // 7*2字节,电表运行状态字块
    {{0x20140201},         0, 0x04000501,2,2,0xC020,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字1
    {{0x20140202},         0, 0x04000502,2,2,0xC021,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字2
    {{0x20140203},         0, 0x04000503,2,2,0xFFFF,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字3
    {{0x20140204},         0, 0x04000504,2,2,0xFFFF,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字4
    {{0x20140205},         0, 0x04000505,2,2,0xFFFF,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字5
    {{0x20140206},         0, 0x04000506,2,2,0xFFFF,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字6
    {{0x20140207},         0, 0x04000507,2,2,0xFFFF,E_OOP_METWORD_T,     0, },  // 2字节,电表运行状态字7
#ifdef AREA_HUNAN
    /* 湖南精品台区 */
    //没有漏电流
    //{{0x20010700},         0,  0x02800501,2,2,0xFFFF,E_DOUBLE_LONG,  0,   },  // 2字节, XXXX, 漏电流
    {{0x26000200},        -1,  0x02800201,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },  // 2字节, XXX.X, 温度
    {{0x26010200},        -1,  0x02800301,2,2,0xFFFF,E_OOP_WORD3V_T,  -1, },  // 2字节, XXX.X, 湿度
    {{0x28000200},         0,  0x04000501,1,1,0xFFFF,E_SWITCH_STATUS,  0, },  // 1字节，开关分合状态
    {{0x2A000200},        -1,  0x02740100,2,2,0xFFFF,E_OOP_LONG3V_T,          -1, },  // 2字节, XXX.X, 变压器油温
    {{0x2A010200},        -1,  0x02750100,2,2,0xFFFF,E_OOP_LONG3V_T,          -1, },  // 2字节, XXX.X, 变压器外壳温度
    {{0x2A030200},        -1,  0x0270FF00,2,6,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 变压器抽头温度块
    {{0x2A030201},        -1,  0x02700100,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 变压器抽头A相温度
    {{0x2A030202},        -1,  0x02700200,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 变压器抽头B相温度
    {{0x2A030203},        -1,  0x02700300,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 变压器抽头C相温度
    {{0x2A040200},        -1,  0x0271FF00,2,6,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 漏保开关温度块
    {{0x2A040201},        -1,  0x02710100,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 漏保开关A相温度
    {{0x2A040202},        -1,  0x02710200,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 漏保开关B相温度
    {{0x2A040203},        -1,  0x02710300,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 漏保开关C相温度
    {{0x2A050200},        -1,  0x0272FF00,2,6,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 分支接头温度块
    {{0x2A050201},        -1,  0x02720100,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 分支接头A相温度
    {{0x2A050202},        -1,  0x02720200,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 分支接头B相温度
    {{0x2A050203},        -1,  0x02720300,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 分支接头C相温度
    {{0x2A060200},        -1,  0x0273FF00,2,6,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 表箱接头温度块
    {{0x2A060201},        -1,  0x02730100,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 表箱接头A相温度
    {{0x2A060202},        -1,  0x02730200,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 表箱接头B相温度
    {{0x2A060203},        -1,  0x02730300,2,2,0xFFFF,E_OOP_LONG3V_T,  -1, },   // 2字节, XXX.X 表箱接头C相温度
#endif
    {{0x40000200},         0, 0x04000101,4,4,0xC010,E_REAL_DATETIME_S,   0, },// 4字节,
    {{0x40000200},         0, 0x04000102,3,3,0xC011,E_REAL_DATETIME_S,   0, },          // 3字节,

    {{0x40010200},         0, 0x04000401,6,6,0xFFFF,E_OOP_OCTETVAR16_T,   0, },// 6字节,
    {{0x40020200},         0, 0x04000402,6,6,0xFFFF,E_OOP_OCTETVAR16_T,   0, },          // 6字节,
    
    {{0x202C0200},        -2,    0x00900200, 4,4,0xFFFF,    E_OOP_WALLET_T, -2, },// 4字节,钱包文件
    {{0x202C0200},        0,     0x03330201, 2,2,0xFFFF,   E_OOP_WALLET_T,  0, },// 3字节,钱包文件
    {{0x202C0201},      -2,      0x00900200, 4,4,0xFFFF,   E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,剩余金额
    {{0x202C0202},        0,     0x03330201, 2,2,0xFFFF,    E_DOUBLE_LONG_UNSIGNED,  0, },// 3字节,购电次数
    {{0x202D0200},        -2,    0x00900201, 4,4,0xFFFF,    E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,透支金额
    {{0x202E0200},       -2,     0x03330601, 4,4,0xFFFF,   E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,累计购电金额

    //第一象限无功总最大需量及发生时间
    {{0x10500200}, 0,    0x0105FF00,    8,40,   0xA13F,E_OOP_DEMAND_T,  0,       },  //  (当前)第一象限无功总最大需量块
    {{0x10500200}, 0,    0x0105FF00,    8,40,   0xB13F,E_OOP_DEMAND_T,  0,       },  //  (当前)第一象限无功总最大需量块
    {{0x10500201}, 0,    0x01050000,    8,8,    0xA130,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第一象限无功总总最大需量
    {{0x10500201}, 0,    0x01050000,    8,8,    0xB130,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第一象限无功总总最大需量
    {{0x10500202}, 0,    0x01050100,    8,8,    0xA131,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第一象限无功最大需量
    {{0x10500202}, 0,    0x01050100,    8,8,    0xB131,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第一象限无功最大需量
    {{0x10500203}, 0,    0x01050200,    8,8,    0xA132,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第一象限无功最大需量
    {{0x10500203}, 0,    0x01050200,    8,8,    0xB132,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第一象限无功最大需量
    {{0x10500204}, 0,    0x01050300,    8,8,    0xA133,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第一象限无功最大需量
    {{0x10500204}, 0,    0x01050300,    8,8,    0xB133,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第一象限无功最大需量
    {{0x10500205}, 0,    0x01050400,    8,8,    0xA134,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第一象限无功最大需量
    {{0x10500205}, 0,    0x01050400,    8,8,    0xB134,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第一象限无功最大需量

    //第二象限无功总最大需量及发生时间
    {{0x10600200}, 0,    0x0106FF00,    8,40,   0xA15F,E_OOP_DEMAND_T,  0,       },  //  (当前)第二象限无功总最大需量块
    {{0x10600200}, 0,    0x0106FF00,    8,40,   0xB15F,E_OOP_DEMAND_T,  0,       },  //  (当前)第二象限无功总最大需量块
    {{0x10600201}, 0,    0x01060000,    8,8,    0xA150,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第二象限无功总总最大需量
    {{0x10600201}, 0,    0x01060000,    8,8,    0xB150,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第二象限无功总总最大需量
    {{0x10600202}, 0,    0x01060100,    8,8,    0xA151,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第二象限无功最大需量
    {{0x10600202}, 0,    0x01060100,    8,8,    0xB151,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第二象限无功最大需量
    {{0x10600203}, 0,    0x01060200,    8,8,    0xA152,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第二象限无功最大需量
    {{0x10600203}, 0,    0x01060200,    8,8,    0xB152,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第二象限无功最大需量
    {{0x10600204}, 0,    0x01060300,    8,8,    0xA153,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第二象限无功最大需量
    {{0x10600204}, 0,    0x01060300,    8,8,    0xB153,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第二象限无功最大需量
    {{0x10600205}, 0,    0x01060400,    8,8,    0xA154,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第二象限无功最大需量    
    {{0x10600205}, 0,    0x01060400,    8,8,    0xB154,E_OOP_DEMAND_T,  0,      },  // 3字节, XX.XXXX,  (当前)费率4第二象限无功最大需量
    //第三象限无功总最大需量及发生时间
    {{0x10700200}, 0,    0x0107FF00,    8,40,   0xA16F,E_OOP_DEMAND_T,  0,       },  //  (当前)第三象限无功总最大需量块
    {{0x10700200}, 0,    0x0107FF00,    8,40,   0xB16F,E_OOP_DEMAND_T,  0,       },  //  (当前)第三象限无功总最大需量块
    {{0x10700201}, 0,    0x01070000,    8,8,    0xA160,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第三象限无功总总最大需量
    {{0x10700201}, 0,    0x01070000,    8,8,    0xB160,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第三象限无功总总最大需量
    {{0x10700202}, 0,    0x01070100,    8,8,    0xA161,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第三象限无功最大需量
    {{0x10700202}, 0,    0x01070100,    8,8,    0xB161,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第三象限无功最大需量
    {{0x10700203}, 0,    0x01070200,    8,8,    0xA162,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第三象限无功最大需量
    {{0x10700203}, 0,    0x01070200,    8,8,    0xB162,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第三象限无功最大需量
    {{0x10700204}, 0,    0x01070300,    8,8,    0xA163,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第三象限无功最大需量
    {{0x10700204}, 0,    0x01070300,    8,8,    0xB163,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第三象限无功最大需量
    {{0x10700205}, 0,    0x01070400,    8,8,    0xA164,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第三象限无功最大需量
    {{0x10700205}, 0,    0x01070400,    8,8,    0xB164,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第三象限无功最大需量
    //第四象限无功总最大需量及发生时间
    {{0x10800200}, 0,    0x0108FF00,    8,40,   0xA14F,E_OOP_DEMAND_T,  0,       },  //  (当前)第四象限无功总最大需量块
    {{0x10800200}, 0,    0x0108FF00,    8,40,   0xB14F,E_OOP_DEMAND_T,  0,       },  //  (当前)第四象限无功总最大需量块
    {{0x10800201}, 0,    0x01080000,    8,8,    0xA140,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第四象限无功总总最大需量
    {{0x10800201}, 0,    0x01080000,    8,8,    0xB140,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)第四象限无功总总最大需量
    {{0x10800202}, 0,    0x01080100,    8,8,    0xA141,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第四象限无功最大需量
    {{0x10800202}, 0,    0x01080100,    8,8,    0xB141,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率1第四象限无功最大需量
    {{0x10800203}, 0,    0x01080200,    8,8,    0xA142,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第四象限无功最大需量
    {{0x10800203}, 0,    0x01080200,    8,8,    0xB142,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率2第四象限无功最大需量
    {{0x10800204}, 0,    0x01080300,    8,8,    0xA143,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第四象限无功最大需量
    {{0x10800204}, 0,    0x01080300,    8,8,    0xB143,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率3第四象限无功最大需量
    {{0x10800205}, 0,    0x01080400,    8,8,    0xA144,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第四象限无功最大需量
    {{0x10800205}, 0,    0x01080400,    8,8,    0xB144,E_OOP_DEMAND_T,  0,       },  // 3字节, XX.XXXX,  (当前)费率4第四象限无功最大需量
           //总电压合格率
    {{0x21300200},   0,  0x03100000,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21300201},   0,  0x03100000,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21300202},   0,  0x03100000,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
        //A相电压合格率
    {{0x21310200},   0,  0x03100100,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21310201},   0,  0x03100100,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21310202},   0,  0x03100100,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    //B相电压合格率
    {{0x21320200},   0,  0x03100200,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21320201},   0,  0x03100200,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21320202},   0,  0x03100200,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    //C相电压合格率
    {{0x21330200},   0,  0x03100300,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21330201},   0,  0x03100300,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21330202},   0,  0x03100300,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    #ifdef AREA_ZHEJIANG
    {{0x201F0201},   0,  0x02900000,1,1,0xFFFF,E_OOP_OCTETVAR64_T,     0, },   //剩余电流最大相
    {{0x201F0202},   0,  0x02900000,1,1,0xFFFF,E_OOP_OCTETVAR64_T,     0, },   //故障相别
    {{0x201F0203},   0,  0x04000501,1,1,0xFFFF,E_OOP_OCTETVAR64_T,     0, },   //运行状态字1
    {{0x201F0204},   0,  0x040010FF,5,5,0xFFFF,E_OOP_OCTETVAR64_T,     0, },   //控制字
    {{0x201F0205},   0,  0x04000411,20,20,0xFFFF,E_OOP_OCTETVAR64_T,     0, },   //额定剩余电流动作值参数组
    {{0x201F0206},   0,  0x03810201,4,4,0xFFFF,E_DOUBLE_LONG_UNSIGNED,     0, },   //浙江漏保累计运行时间
    {{0x201F0207},   0,  0x02910100,2,2,0xFFFF,E_LONG,     0, },   //当前额定剩余电流动作值
    #endif
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,   0xFF,0xFF,0xFFFF,0xFF,0xFF      },
};

const MapOopToCap MapOopToCap_RT_01[]= //查询命令1实时数据映射表

{
    {{0xBBB10200},             0,  2,2,E_LONG_UNSIGNED,  },   
    {{0xEEEEEEEE},             0,  1,1,E_NULL,  }, 
    {{0xBBB30200},             0,  1,1,E_UNSIGNED,  }, 
    {{0x202F0200},             0,  2,5,E_OOP_OCTETVAR64_T,}, 
    {{0xBBB50200},             0,  1,1,E_UNSIGNED,  }, 
    {{0xBBB60200},             0,  1,1,E_UNSIGNED,  }, 
    {{0xBBB70200},             0,  1,1,E_UNSIGNED,  }, 

    //电容器投切次数
    {{0x2A000200},             0,  4,12,E_OOP_DWORD3V_T,  },   //电容器投切次数
    {{0x2A000201},             0,  4,4, E_OOP_DWORD3V_T,  },   //A相电容器投切次数
    {{0x2A000202},             0,  4,4, E_OOP_DWORD3V_T,  },   //B相电容器投切次数
    {{0x2A000203},             0,  4,4, E_OOP_DWORD3V_T,  },   //C相电容器投切次数
    
    //电容器电流
    {{0x260A0200},             0,  2,6, E_OOP_INT3V_T,   },  // 分补电流数据块
    {{0x260A0201},             0,  2,2, E_OOP_INT3V_T,   },  // 
    {{0x260A0202},             0,  2,2, E_OOP_INT3V_T,   },  // 
    {{0x260A0203},             0,  2,2, E_OOP_INT3V_T,   },  // 

    //电容器容量
    {{0x260B0200},             0,  2,6, E_OOP_WORD3V_T,   },  // 电容器容量
    {{0x260B0201},             0,  2,2, E_OOP_WORD3V_T,   },  // 
    {{0x260B0202},             0,  2,2, E_OOP_WORD3V_T,   },  // 
    {{0x260B0203},             0,  2,2, E_OOP_WORD3V_T,   },  // 
    //温度
    {{0x20100200},             0,  2,2,E_LONG,  },
    //掉电复位次数
    {{0xBBB80200},             0,  1,1,E_NULL,  },
    //电容器电量虚拟的，命令中不存在
    //{{0x06000200},             0,  6,6,E_OOP_ENERGY_T,   },  // 
    {{0xFFFFFFFF},             0,  1,1,E_NULL,  },
};

const MapOopToCap MapOopToCap_RT_02[]= //实时数据映射表

{
    {{0xBBC10200},             0,  2,2,E_LONG_UNSIGNED,  },  // 目标功率因数（上限值)
    {{0xBBC20200},             0,  2,2,E_LONG_UNSIGNED,  },  // 目标功率因数（下限值)
    {{0xBBC30200},             0,  2,2,E_LONG_UNSIGNED,  },  // CT变比
    {{0xBBC40200},             0,  1,1,E_UNSIGNED,  },       // 温度保护动作限值
    {{0xBBC50200},             0,  1,1,E_UNSIGNED,  },          // 温度保护回差
    {{0xBBC60200},             0,  2,2,E_LONG_UNSIGNED,  },  // 电流谐波限值 
    {{0xBBC70200},             0,  2,2,E_LONG_UNSIGNED,  },  // 一级过电压限值 

    {{0xBBC80200},             0,  2,2, E_LONG_UNSIGNED,  }, //二级过电压限值
    {{0xBBC90200},             0,  1,1, E_UNSIGNED,  },   //电压限值回差
    {{0xBBCA0200},             0,  1,1, E_UNSIGNED,  },   //低负荷电流闭锁限值
    {{0xBBCB0200},             0,  2,2, E_LONG_UNSIGNED,  },//告警屏蔽  
    {{0xFFFFFFFF},             0,  0,0,E_NULL,  },
};

const MapOopToCap MapOopToCap_RT_03[]= //查询命令3实时数据映射表

{
    {{0x20000200},             0,  2,6,E_OOP_WORD3V_T,  },  // 电压
    {{0x20000201},             0,  2,2,E_OOP_WORD3V_T,  },  // A相电压
    {{0x20000202},             0,  2,2,E_OOP_WORD3V_T,  },  // B相电压
    {{0x20000203},             0,  2,2,E_OOP_WORD3V_T,  },       // C相电压
     
    {{0x20010200},             0,  2,6,E_OOP_INT3V_T,  },          // 电流
    {{0x20010201},             0,  2,2,E_OOP_INT3V_T,  },  // A相电流 
    {{0x20010202},             0,  2,2,E_OOP_INT3V_T,  },  // B相电流 
    {{0x20010203},             0,  2,2,E_OOP_INT3V_T,  }, //C相电流
    
    {{0x20040200},             0,  2,6, E_OOP_INT4V_T,  },   //功率
    {{0x20040201},             0,  2,2, E_OOP_INT4V_T,  },   //A 相有功功率 
    {{0x20040202},             0,  2,2, E_OOP_INT4V_T,  },//B 相有功功率
    {{0x20040203},             0,  2,2, E_OOP_INT4V_T,  }, //C相有功功率
    
    {{0x20050200},             0,  2,6, E_OOP_INT4V_T,  },   //无功功率
    {{0x20050201},             0,  2,2, E_OOP_INT4V_T,  },   //A相无功功率
    {{0x20050202},             0,  2,2, E_OOP_INT4V_T,  },//B相无功功率 
    {{0x20050203},             0,  2,2, E_OOP_INT4V_T,  },//C相无功功率 

    {{0x200E0200},             0,  2,6, E_OOP_HARMONIC_I_VALUE_T,  },   //谐波
    {{0x200E0201},             0,  2,2, E_OOP_HARMONIC_I_VALUE_T,  },   //A谐波
    {{0x200E0202},             0,  2,2, E_OOP_HARMONIC_I_VALUE_T,  },//B谐波 
    {{0x200E0203},             0,  2,2, E_OOP_HARMONIC_I_VALUE_T,  },//C谐波 
    {{0xFFFFFFFF},             0,  0,0,E_NULL,  },
};

const uint16 Map_RT_len = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);

const MapOopTo645_NEW MapOopTo645_Instant[]= //瞬时冻结表
{
        //冻结时间
     {{0x20210200},   0,  0x05010001,5,5,0xFFFF,    E_FRZ_DATETIME_S,0},     // YYMMDD hhmmss冻结时间
    // (当前)正向有功总电能         
     {{0x00100200},  -2,  0x05010101,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)正向有功电能块                                                                                                                                                                                                                }
     {{0x00100201},  -2,  0x05010101,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
     {{0x00100202},  -2,  0x05010101,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率1正向有功电能
     {{0x00100203},  -2,  0x05010101,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率2正向有功电能
     {{0x00100204},  -2,  0x05010101,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率3正向有功电能
     {{0x00100205},  -2,  0x05010101,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率4正向有功电能
    //(当前)反向有功总电能          
     {{0x00200200},  -2,  0x05010201,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)反向有功电能块
     {{0x00200201},  -2,  0x05010201,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
     {{0x00200202},  -2,  0x05010201,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率1反向有功电能
     {{0x00200203},  -2,  0x05010201,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率2反向有功电能
     {{0x00200204},  -2,  0x05010201,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率3反向有功电能
     {{0x00200205},  -2,  0x05010201,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率4反向有功电能
    //(当前)正向无功总电能(+RL,+RC  )
     {{0x00300200},  -2,  0x05010301,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)正向无功电能块(+RL,+RC)
     {{0x00300201},  -2,  0x05010301,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
     {{0x00300202},  -2,  0x05010301,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节,XXXXXX.XX (当前)费率1正向无功电能
     {{0x00300203},  -2,  0x05010301,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节,XXXXXX.XX (当前)费率2正向无功电能
     {{0x00300204},  -2,  0x05010301,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节,XXXXXX.XX (当前)费率3正向无功电能
     {{0x00300205},  -2,  0x05010301,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节,XXXXXX.XX (当前)费率4正向无功电能
    //(当前)反向无功总电能          
     {{0x00400200},  -2,  0x05010401,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)反向无功电能块(-RL,-RC)
     {{0x00400201},  -2,  0x05010401,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
     {{0x00400202},  -2,  0x05010401,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率1反向无功电能
     {{0x00400203},  -2,  0x05010401,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率2反向无功电能
     {{0x00400204},  -2,  0x05010401,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率3反向无功电能
     {{0x00400205},  -2,  0x05010401,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率4反向无功电能
    //一象限无功总电能              
     {{0x00500200},  -2,  0x05010501,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)一象限无功电能块(+RL)
     {{0x00500201},  -2,  0x05010501,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
     {{0x00500202},  -2,  0x05010501,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率1一象限无功电能
     {{0x00500203},  -2,  0x05010501,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率2一象限无功电能
     {{0x00500204},  -2,  0x05010501,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率3一象限无功电能
     {{0x00500205},  -2,  0x05010501,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率4一象限无功电能
    //二象限无功总电能              
     {{0x00600200},  -2,  0x05010601,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)二象限无功电能块(+Rc)
     {{0x00600201},  -2,  0x05010601,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
     {{0x00600202},  -2,  0x05010601,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率1二象限无功电能
     {{0x00600203},  -2,  0x05010601,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率2二象限无功电能
     {{0x00600204},  -2,  0x05010601,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率3二象限无功电能
     {{0x00600205},  -2,  0x05010601,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率4二象限无功电能
    //三象限无功总电能              
     {{0x00700200},  -2,  0x05010701,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)三象限无功电能块(-RL)
     {{0x00700201},  -2,  0x05010701,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
     {{0x00700202},  -2,  0x05010701,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率1三象限无功电能
     {{0x00700203},  -2,  0x05010701,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率2三象限无功电能
     {{0x00700204},  -2,  0x05010701,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率3三象限无功电能
     {{0x00700205},  -2,  0x05010701,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率4三象限无功电能
    //四象限无功总电能              
     {{0x00800200},  -2,  0x05010801,4,20,0xFFFF,  E_OOP_ENERGY_T, -2},  // (当前)四象限无功电能块(-Rc)
     {{0x00800201},  -2,  0x05010801,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
     {{0x00800202},  -2,  0x05010801,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率1四象限无功电能
     {{0x00800203},  -2,  0x05010801,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率2四象限无功电能
     {{0x00800204},  -2,  0x05010801,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率3四象限无功电能
     {{0x00800205},  -2,  0x05010801,4,4, 0xFFFF,   E_OOP_ENERGY_T, -2},     // 4字节, XXXXXX.XX (当前)费率4四象限无功电能

    //(当前)正向有功总最大需量
     {{0x10100200},   0,  0x05010901,     8,40,0xFFFF,  E_OOP_DEMAND_T,  0},  // (当前)正向有功最大需量块
                                        
                                        
     {{0x10100201},   0,  0x05010901,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
                                        
     {{0x10100202},   0,  0x05010901,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率1正向有功最大需量发生时间,格式同上
                                        
     {{0x10100203},   0,  0x05010901,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率2正向有功最大需量发生时间,格式同上
                                        
     {{0x10100204},   0,  0x05010901,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率3正向有功最大需量发生时间,格式同上
                                        
     {{0x10100205},   0,  0x05010901,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率4正向有功最大需量发生时间,格式同上
    //(当前)反向有功总最大需量          
                                        
     {{0x10200200},   0,  0x05010A01,     8,40,0xFFFF,  E_OOP_DEMAND_T,  0    },  // (当前)反向有功最大需量块
                                        
     {{0x10200201},   0,  0x05010A01,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)反向有功总最大需量发生时间
                                        
     {{0x10200202},   0,  0x05010A01,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率1反向有功最大需量发生时间,格式同上
                                        
     {{0x10200203},   0,  0x05010A01,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率2反向无功向有功最大需量发生时间,格式同上
                                        
     {{0x10200204},   0,  0x05010A01,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率3反向无功向有功最大需量发生时间,格式同上
                                        
     {{0x10200205},   0,  0x05010A01,     8,8,0xFFFF,  E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(当前)费率4反向无功向有功最大需量发生时间,格式同上
    //瞬时冻结变量数据                  
     {{0x20040200},  -1, 0x05011001,     3,12,0xFFFF,   E_OOP_INT4V_T,  -4},   // 瞬时正向有功功率块
     {{0x20040201},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
     {{0x20040202},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
     {{0x20040203},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, B相有功功率,有正负,格式同上
     {{0x20040204},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, C相有功功率,有正负,格式同上
     {{0x20050200},  -1, 0x05011001,     3,12,0xFFFF,   E_OOP_INT4V_T,  -4},   //
     {{0x20050201},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 瞬时正向无功功率块
     {{0x20050202},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
     {{0x20050203},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, B相无功功率,格式同上
     {{0x20050204},  -1, 0x05011001,     3,3, 0xFFFF,   E_OOP_INT4V_T,  -4},   // 3字节, XX.XXXX, C相无功功率,格式同上

     {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF  },
    
};

const MapOopTo645_NEW MapOopTo645_Min[]=
{
    {{0x20210200},  0, 0x06000001,5,5,0xFFFF,   E_FRZ_DATETIME_S,0},   // YYMMDD hhmm冻结时间
    {{0x20000200}, -1, 0x06000001,2,6,0xFFFF,   E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20000201}, -1, 0x06000001,2,2,0xFFFF,   E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20000202}, -1, 0x06000001,2,2,0xFFFF,   E_OOP_WORD3V_T, -1}, // 2字节, X.XXX B相电压/无/无
    {{0x20000203}, -1, 0x06000001,2,2,0xFFFF,   E_OOP_WORD3V_T, -1}, // 2字节, X.XXX C相电压/CB线电压/无
    {{0x20010200}, -3, 0x06000001,3,9,0xFFFF,   E_OOP_INT3V_T, -3},  // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20010201}, -3, 0x06000001,3,3,0xFFFF,   E_OOP_INT3V_T, -3},  // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20010202}, -3, 0x06000001,3,3,0xFFFF,   E_OOP_INT3V_T, -3},  // 3字节, XXX.XXX, B相电流/无/无
    {{0x20010203}, -3, 0x06000001,3,3,0xFFFF,   E_OOP_INT3V_T, -3},  // 3字节, XXX.XXX, C相电流/C相电流/无
    {{0x200F0200}, -2, 0x06000001,2,2,0xFFFF,   E_LONG_UNSIGNED,-2},//2字节,XX.XX电网频率
    {{0x20040200}, -1, 0x06000001,3,12,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
    {{0x20040201}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
    {{0x20040202}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20040203}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, B相有功功率,有正负,格式同上
    {{0x20040204}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, C相有功功率,有正负,格式同上   //
    {{0x20050200}, -1, 0x06000001,3,12,0xFFFF,   E_OOP_INT4V_T,    -4},   // 瞬时正向无功功率块
    {{0x20050201}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 瞬时正向无功功率块
    {{0x20050202}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20050203}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, B相无功功率,格式同上
    {{0x20050204}, -1, 0x06000001,3,3,0xFFFF,   E_OOP_INT4V_T,    -4},   // 3字节, XX.XXXX, C相无功功率,格式同上
    {{0x200A0200}, -3, 0x06000001,2,8,0xFFFF,   E_OOP_LONG4V_T, -3},   // 2字节, XX.XXXX, 瞬时总功率因数
    {{0x200A0201}, -3, 0x06000001,2,2,0xFFFF,   E_OOP_LONG4V_T, -3},   // 2字节, XX.XXXX, 瞬时总功率因数
    {{0x200A0202}, -3, 0x06000001,2,2,0xFFFF,   E_OOP_LONG4V_T, -3},   // 2字节, XX.XXXX, A相功率因数
    {{0x200A0203}, -3, 0x06000001,2,2,0xFFFF,   E_OOP_LONG4V_T, -3},   // 2字节, XX.XXXX, B相功率因数
    {{0x200A0204}, -3, 0x06000001,2,2,0xFFFF,   E_OOP_LONG4V_T, -3},   // 2字节, XX.XXXX, C相功率因数
    {{0x00100200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00100201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00200200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00200201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00300200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00300201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00400200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00400201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00500200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00500201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00600200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00600201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00700200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00700201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00800200}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x00800201}, -2, 0x06000001,4,4,0xFFFF,   E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x20170200}, -4, 0x06000001,3,3,0xFFFF,   E_DOUBLE_LONG,-4},  // 3字节, XX.XXXX(当前)有功需量
    {{0x20180200}, -4, 0x06000001,3,3,0xFFFF,   E_DOUBLE_LONG,-4},  // 3字节, XX.XXXX(当前)无功需量
    
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF, 0xFF,  0xFF   },
};

//单相多费率表、单相智能表只支持 A 相电压、 A 相电流、总有功功率、总功率因数、正向有功总电能、反向有功总电能，不支持数据块命令
const MapOopTo645_NEW MapOopTo645_MinOne[]=  //分钟冻结(负荷曲线)
{
   // {{0x20000200},    -1,   0x061001FF,2, 6, 0xFFFF,E_OOP_WORD3V_T, -1,   }, // 电压数据块
    {{0x20000201},    -1,   0x06100101,2, 2, 0xFFFF,E_OOP_WORD3V_T, -1,   }, // 2字节, X.XXX A相电压/AB线电压/单向表电压
//    {{0x20000202},    -1,   0x06100102,2, 2, 0xFFFF,E_OOP_WORD3V_T, -1,   }, // 2字节, X.XXX B相电压/无/无
//    {{0x20000203},    -1,   0x06100103,2, 2, 0xFFFF,E_OOP_WORD3V_T, -1,   }, // 2字节, X.XXX C相电压/CB线电压/无
 //   {{0x20010200},    -3,   0x061002FF,3, 9, 0xFFFF,E_OOP_INT3V_T, -3,   },  // 电流数据块
    {{0x20010201},    -3,   0x06100201,3, 3, 0xFFFF,E_OOP_INT3V_T, -3,   },  // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
 //   {{0x20010202},    -3,   0x06100202,3, 3, 0xFFFF,E_OOP_INT3V_T, -3,   },  // 3字节, XXX.XXX, B相电流/无/无
 //   {{0x20010203},    -3,   0x06100203,3, 3, 0xFFFF,E_OOP_INT3V_T, -3,   },  // 3字节, XXX.XXX, C相电流/C相电流/无
 //   {{0x20040200},    -1,   0x061003FF,3,12,0xFFFF,E_OOP_INT4V_T, -4,   },   // 瞬时正向有功功率块
    {{0x20040201},    -1,   0x06100300,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥
  //  {{0x20040202},    -1,   0x06100301,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
 //   {{0x20040203},    -1,   0x06100302,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, B相有功功率,有正负,格式同上
 //   {{0x20040204},    -1,   0x06100303,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, C相有功功率,有正负,格式同上
 //   {{0x20050200},    -1,   0x061004FF,3,12,0xFFFF,E_OOP_INT4V_T, -4,   },   //瞬时正向无功功率块
    {{0x20050201},    -1,   0x06100400,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, 总无功功率
//    {{0x20050202},    -1,   0x06100401,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
//    {{0x20050203},    -1,   0x06100402,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, B相无功功率,格式同上
//    {{0x20050204},    -1,   0x06100403,3, 3, 0xFFFF,E_OOP_INT4V_T, -4,   },   // 3字节, XX.XXXX, C相无功功率,格式同上
 //   {{0x200A0200},    -3,   0x061005FF,2, 8, 0xFFFF,E_OOP_LONG4V_T, -3,   },   // 瞬时功率因数块
    {{0x200A0201},    -3,   0x06100500,2, 2, 0xFFFF,E_OOP_LONG4V_T, -3,   },   // 2字节, XX.XXXX, 瞬时总功率因数
 //   {{0x200A0202},    -3,   0x06100501,2, 2, 0xFFFF,E_OOP_LONG4V_T, -3,   },   // 2字节, XX.XXXX, A相功率因数
  //  {{0x200A0203},    -3,   0x06100502,2, 2, 0xFFFF,E_OOP_LONG4V_T, -3,   },   // 2字节, XX.XXXX, B相功率因数
  //  {{0x200A0204},    -3,   0x06100503,2, 2, 0xFFFF,E_OOP_LONG4V_T, -3,   },   // 2字节, XX.XXXX, C相功率因数
    {{0x00100201},    -2,   0x06100601,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00200201},    -2,   0x06100602,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX(当前)反向有功总电能
  //  {{0x00100200},    -2,   0x06100601,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX(当前)正向有功总电能
  //  {{0x00200200},    -2,   0x06100602,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX(当前)反向有功总电能
 //   {{0x00300200},    -2,   0x06100603,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
 //   {{0x00400200},    -2,   0x06100604,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00300201},    -2,   0x06100603,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00400201},    -2,   0x06100604,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)

    {{0x00500200},    -2,   0x06100701,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00600200},    -2,   0x06100702,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00700200},    -2,   0x06100703,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00800200},    -2,   0x06100704,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x00500201},    -2,   0x06100701,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)一象限无功总电能(+RL)
    {{0x00600201},    -2,   0x06100702,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)二象限无功总电能(+Rc)
    {{0x00700201},    -2,   0x06100703,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)三象限无功总电能(-RL)
    {{0x00800201},    -2,   0x06100704,4, 4, 0xFFFF,E_OOP_ENERGY_T, -2,   },  // 4字节, XXXXXX.XX (当前)四象限无功总电能(-Rc)
    {{0x20170200},    -4,   0x06100801,3, 3, 0xFFFF,E_DOUBLE_LONG, -4,   },  // 3字节, XX.XXXX(当前)有功需量
    {{0x20180200},    -4,   0x06100802,3, 3, 0xFFFF,E_DOUBLE_LONG, -4,   },  // 3字节, XX.XXXX(当前)无功需量
    //{{0x20010400},    -3,   0x02800001,3, 3, 0xFFFF,E_DOUBLE_LONG, -3},  // 3字节, XXX.XXX, A相电流/A相电流/单向表电流

    {{0xFFFFFFFF},   0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF      },
};

const MapOopTo645_NEW MapOopTo645_Hour[]= //小时冻结表
{
    {{0x20210200},   0, 0x05040001,5,5,0xFFFF,    E_FRZ_DATETIME_S,0},   // YYMMDD hhmm冻结时间
    {{0x00100200},   0, 0x05040101,4,4,0x901F,    E_OOP_ENERGY_T, -2},  // (当前)正向有功电能块
    {{0x00100201},   0, 0x05040101,4,4,0x9010,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00100202},   0, 0x05040101,4,4,0x9011,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率1正向有功总电能
    {{0x00100203},   0, 0x05040101,4,4,0x9012,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率2正向有功总电能
    {{0x00100204},   0, 0x05040101,4,4,0x9013,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率3正向有功总电能
    {{0x00100205},   0, 0x05040101,4,4,0x9014,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率4正向有功总电能
                                   
    {{0x00200200},   0, 0x05040201,4,4,0x902F,    E_OOP_ENERGY_T, -2},  // (当前)反向有功电能块
    {{0x00200201},   0, 0x05040201,4,4,0x9020,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00200202},   0, 0x05040201,4,4,0x9021,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率1反向有功总电能
    {{0x00200203},   0, 0x05040201,4,4,0x9022,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率2反向有功总电能
    {{0x00200204},   0, 0x05040201,4,4,0x9023,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率3反向有功总电能
    {{0x00200205},   0, 0x05040201,4,4,0x9024,  E_OOP_ENERGY_T, -2},  // 4字节, XXXXXX.XX(当前)费率4反向有功总电能
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF     },
    
};
const MapOopTo645_NEW MapOopTo645_DAY[]= //日曲线数据映射表
{
    //冻结时间
    {{0x20210200},   0,  0x05060001,5,5,0xFFFF, E_FRZ_DATETIME_S ,  0 ,},   // YYMMDD hhmm
    // (当前)正向有功总电能
    {{0x00100200},  -2,  0x05060101,4,20,0xFFFF, E_OOP_ENERGY_T, -2 ,},  // (当前)正向有功电能块                                                                                                                                                                                                              }
    //(当前)反向有功总电能
    {{0x00200200},  -2,  0x05060201,4,20,0xFFFF, E_OOP_ENERGY_T, -2 ,},  // (当前)反向有功电能块
    // (当前)A相正向有功总电能        
    {{0x00110200}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    {{0x00110201}, -2,  0x00150000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相正向有功总电能
    // (当前)B相正向有功总电能    
    {{0x00120200}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    {{0x00120201}, -2,  0x00290000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相正向有功总电能
    // (当前)C相正向有功总电能            
    {{0x00130200}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相正向有功总电能
    {{0x00130201}, -2,  0x003D0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相正向有功总电能

    // (当前)A相反向有功总电能            
    {{0x00210200}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    {{0x00210201}, -2,  0x00160000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A相反向有功总电能
    // (当前)B相反向有功总电能            
    {{0x00220200}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    {{0x00220201}, -2,  0x002A0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B相反向有功总电能
    // (当前)C相反向有功总电能            
    {{0x00230200}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    {{0x00230201}, -2,  0x003E0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C相反向有功总电能
    
    {{0x00310200}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能
    {{0x00310201}, -2,  0x00170000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 1 电能

    {{0x00320200}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能
    {{0x00320201}, -2,  0x002B0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 1 电能

    {{0x00330200}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能
    {{0x00330201}, -2,  0x003F0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 1 电能

    {{0x00410200}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能
    {{0x00410201}, -2,  0x00180000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)A 相组合无功 2 电能

    {{0x00420200}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能
    {{0x00420201}, -2,  0x002C0000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)B 相组合无功 2 电能

    {{0x00430200}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能
    {{0x00430201}, -2,  0x00400000,4,4,0xFFFF,E_OOP_ENERGY_T,  -2, },    // 4字节, XXXXXX.XX(当前)C 相组合无功 2 电能

    //(当前)正向无功总电能(+RL,+RC)
    {{0x00300200},  -2,  0x05060301,4,20,0xFFFF, E_OOP_ENERGYS_T, -2, },    // (当前)正向无功电能块(+RL,+RC)
   //(当前)反向无功总电能
    {{0x00400200},  -2,  0x05060401,4,20,0xFFFF, E_OOP_ENERGYS_T, -2, },    // (当前)反向无功电能块(-RL,-RC)
    //一象限无功总电能
    {{0x00500200},  -2,  0x05060501,4,20,0xFFFF, E_OOP_ENERGY_T, -2, },    // (当前)一象限无功电能块(+RL)
    //二象限无功总电能
    {{0x00600200},  -2,  0x05060601,4,20,0xFFFF, E_OOP_ENERGY_T, -2, },    // (当前)二象限无功电能块(+Rc)
    //三象限无功总电能
    {{0x00700200},  -2,  0x05060701,4,20,0xFFFF, E_OOP_ENERGY_T, -2, },    // (当前)三象限无功电能块(-RL)
    //四象限无功总电能
    {{0x00800200},  -2,  0x05060801,4,20,0xFFFF, E_OOP_ENERGY_T, -2, },    // (当前)四象限无功电能块(-Rc)
    //(当前)正向有功总最大需量
    {{0x10100200},  0,  0x05060901,8,40,0xFFFF, E_OOP_DEMAND_T,  0, },  // (当前)正向有功最大需量块
    //(当前)反向有功总最大需量
    {{0x10200200},  0,  0x05060A01,8,40,0xFFFF, E_OOP_DEMAND_T,  0, },  // (当前)反向有功最大需量块
    //(当前)正向无功最大需量块
    {{0x10300200}, 0,    0x0103FF00,    8,40,   0xFFFF,E_OOP_DEMAND_T,  -0,       },  //  (当前)正向无功最大需量块
    //(当前)反向无功最大需量块
    {{0x10400200}, 0,    0x0104FF00,    8,40,0xFFFF,E_OOP_DEMAND_T,  0,       },  //(当前)反向无功最大需量块
//瞬时正向有功功率
    {{0x20040200},  -1,  0x05061001,3,12,0xFFFF, E_OOP_INT4V_T,  -4, },     // 瞬时正向有功功率块
//瞬时正向无功功率
    {{0x20050200},  -1,  0x05061001,3,12,0xFFFF, E_OOP_INT4V_T,  -4, },     //
    {{0x202C0200},  -2,  0x00900200,4,4,0xFFFF, E_OOP_WALLET_T, -2, },// 4字节,钱包文件
    {{0x202C0200},   0,  0x03330201,2,2,0xFFFF, E_OOP_WALLET_T,  0, },// 3字节,钱包文件
    {{0x202C0201},  -2,  0x00900200,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,剩余金额
    {{0x202C0202},   0,  0x03330201,2,2,0xFFFF, E_DOUBLE_LONG_UNSIGNED,  0, },// 3字节,购电次数
    {{0x202D0200},  -2,  0x00900201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,透支金额
    {{0x202E0200},  -2,  0x03330601,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED, -2, },// 4字节,累计购电金额
    //总电压合格率
    {{0x21300200},   0,  0x03100000,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21300201},   0,  0x03100000,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21300202},   0,  0x03100000,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
        //A相电压合格率
    {{0x21310200},   0,  0x03100100,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21310201},   0,  0x03100100,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21310202},   0,  0x03100100,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    //B相电压合格率
    {{0x21320200},   0,  0x03100200,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21320201},   0,  0x03100200,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21320202},   0,  0x03100200,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    //C相电压合格率
    {{0x21330200},   0,  0x03100300,15,15,0xFFFF,E_OOP_VOLTSTAT_T,     0, },   //电压合格率
    {{0x21330201},   0,  0x03100300,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0x21330202},   0,  0x03100300,13,13,0xFFFF,E_OOP_VOLTQR_T,     0, },   //电压合格率
    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};

const MapOopTo645_NEW MapOopTo645_MON[]= //月冻结表
{
    {{0x00000200},     -2,  0x0000FF01,4,20,0xFFFF,  E_OOP_ENERGYS_T,    -2},   // (结算日)组合有功电能块
    {{0x00000201},     -2,  0x00000001,4,4, 0xFFFF,  E_OOP_ENERGYS_T,    -2},   // 4字节, XXXXXX.XX(结算日)组合有功总电能
    {{0x00000202},     -2,  0x00000101,4,4, 0xFFFF,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX(结算日)费率1组合有功电能
    {{0x00000203},     -2,  0x00000201,4,4, 0xFFFF,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX(结算日)费率2组合有功电能
    {{0x00000204},     -2,  0x00000301,4,4, 0xFFFF,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX(结算日)费率3组合有功电能
    {{0x00000205},     -2,  0x00000401,4,4, 0xFFFF,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX(结算日)费率4组合有功电能
                                     
    // (结算日)正向有功总电能        
    {{0x00100200},     -2,  0x0001FF01,4,20,0x941F,  E_OOP_ENERGY_T, -2},    // (结算日)正向有功电能块                                                                                                                                                                                                               }
    {{0x00100201},     -2,  0x00010001,4,4, 0x9410,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)正向有功总电能
    {{0x00100202},     -2,  0x00010101,4,4, 0x9411,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率1正向有功电能
    {{0x00100203},     -2,  0x00010201,4,4, 0x9412,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率2正向有功电能
    {{0x00100204},     -2,  0x00010301,4,4, 0x9413,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率3正向有功电能
    {{0x00100205},     -2,  0x00010401,4,4, 0x9414,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率4正向有功电能
                                     
    //(结算日)反向有功总电能         
    {{0x00200200},     -2,  0x0002FF01,4,20,0x942F,  E_OOP_ENERGY_T, -2},    // (结算日)反向有功电能块
    {{0x00200201},     -2,  0x00020001,4,4, 0x9420,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)反向有功总电能
    {{0x00200202},     -2,  0x00020101,4,4, 0x9421,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率1反向有功电能
    {{0x00200203},     -2,  0x00020201,4,4, 0x9422,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率2反向有功电能
    {{0x00200204},     -2,  0x00020301,4,4, 0x9423,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率3反向有功电能
    {{0x00200205},     -2,  0x00020401,4,4, 0x9424,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX(结算日)费率4反向有功电能
                                     
    //(结算日)正向无功总电能(+RL,+R  C)
    {{0x00300200},   -2,    0x0003FF01,4,20,0x951F,  E_OOP_ENERGYS_T,    -2},    // (结算日)正向无功电能块(+RL,+RC)
    {{0x00300201},   -2,    0x00030001,4,4, 0x9510,  E_OOP_ENERGYS_T,    -2},    // 4字节,XXXXXX.XX (结算日))正向无功总电能(+RL,+RC)
    {{0x00300202},   -2,    0x00030101,4,4, 0x9511,  E_OOP_ENERGYS_T,    -2},    // 4字节,XXXXXX.XX (结算日)费率1正向无功电能
    {{0x00300203},   -2,    0x00030201,4,4, 0x9512,  E_OOP_ENERGYS_T,    -2},    // 4字节,XXXXXX.XX (结算日)费率2正向无功电能
    {{0x00300204},   -2,    0x00030301,4,4, 0x9513,  E_OOP_ENERGYS_T,    -2},    // 4字节,XXXXXX.XX (结算日)费率3正向无功电能
    {{0x00300205},   -2,    0x00030401,4,4, 0x9514,  E_OOP_ENERGYS_T,    -2},    // 4字节,XXXXXX.XX (结算日)费率4正向无功电能
                                     
    //(结算日)反向无功总电能         
    {{0x00400200},    -2,   0x0004FF01,4,20,0x952F,  E_OOP_ENERGYS_T,    -2},    // (结算日)反向无功电能块(-RL,-RC)
    {{0x00400201},    -2,   0x00040001,4,4, 0x9520,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX (结算日)反向无功总电能(-RL,-RC)
    {{0x00400202},    -2,   0x00040101,4,4, 0x9521,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX (结算日)费率1反向无功电能
    {{0x00400203},    -2,   0x00040201,4,4, 0x9522,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX (结算日)费率2反向无功电能
    {{0x00400204},    -2,   0x00040301,4,4, 0x9523,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX (结算日)费率3反向无功电能
    {{0x00400205},    -2,   0x00040401,4,4, 0x9524,  E_OOP_ENERGYS_T,    -2},    // 4字节, XXXXXX.XX (结算日)费率4反向无功电能
                                     
    //(结算日)一象限无功总电能       
    {{0x00500200},    -2,   0x0005FF01,4,20,0x953F,  E_OOP_ENERGY_T, -2},    // (结算日)一象限无功电能块(+RL)
    {{0x00500201},    -2,   0x00050001,4,4, 0x9530,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)一象限无功总电能(+RL)
    {{0x00500202},    -2,   0x00050101,4,4, 0x9531,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率1一象限无功电能
    {{0x00500203},    -2,   0x00050201,4,4, 0x9532,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率2一象限无功电能
    {{0x00500204},    -2,   0x00050301,4,4, 0x9533,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率3一象限无功电能
    {{0x00500205},    -2,   0x00050401,4,4, 0x9534,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率4一象限无功电能
                                     
    //(结算日)二象限无功总电能       
    {{0x00600200},    -2,   0x0006FF01,4,20,0x955F,  E_OOP_ENERGY_T, -2},    // (结算日)二象限无功电能块(+Rc)
    {{0x00600201},    -2,   0x00060001,4,4, 0x9550,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)二象限无功总电能(+Rc)
    {{0x00600202},    -2,   0x00060101,4,4, 0x9551,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率1二象限无功电能
    {{0x00600203},    -2,   0x00060201,4,4, 0x9552,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率2二象限无功电能
    {{0x00600204},    -2,   0x00060301,4,4, 0x9553,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率3二象限无功电能
    {{0x00600205},    -2,   0x00060401,4,4, 0x9554,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率4二象限无功电能
                                     
    //(结算日)三象限无功总电能       
    {{0x00700200},    -2,   0x0007FF01,4,20,0x956F,  E_OOP_ENERGY_T, -2},    // (结算日)三象限无功电能块(-RL)
    {{0x00700201},    -2,   0x00070001,4,4, 0x9560,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)三象限无功总电能(-RL)
    {{0x00700202},    -2,   0x00070101,4,4, 0x9561,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率1三象限无功电能
    {{0x00700203},    -2,   0x00070201,4,4, 0x9562,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率2三象限无功电能
    {{0x00700204},    -2,   0x00070301,4,4, 0x9563,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率3三象限无功电能
    {{0x00700205},    -2,   0x00070401,4,4, 0x9564,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率4三象限无功电能
                                     
    //(结算日)四象限无功总电能       
    {{0x00800200},    -2,   0x0008FF01,4,20,0x954F,  E_OOP_ENERGY_T, -2},    // (结算日)四象限无功电能块(-Rc)
    {{0x00800201},    -2,   0x00080001,4,4, 0x9540,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)四象限无功总电能(-Rc)
    {{0x00800202},    -2,   0x00080101,4,4, 0x9541,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率1四象限无功电能
    {{0x00800203},    -2,   0x00080201,4,4, 0x9542,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率2四象限无功电能
    {{0x00800204},    -2,   0x00080301,4,4, 0x9543,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率3四象限无功电能
    {{0x00800205},    -2,   0x00080401,4,4, 0x9544,  E_OOP_ENERGY_T, -2},    // 4字节, XXXXXX.XX (结算日)费率4四象限无功电能

    //(结算日)正向有功总最大需量
    {{0x10100200},   0,  0x0101FF01,    8,40,0xA41F, E_OOP_DEMAND_T,  0      },  // (结算日)正向有功最大需量块
    {{0x10100200},   0,  0x0101FF01,    8,40,0xB41F, E_OOP_DEMAND_T,  0      },  // (结算日)正向有功最大需量块
    {{0x10100201},   0,  0x01010001,    8, 8,0xA410, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)正向有功总最大需量
    {{0x10100201},   0,  0x01010001,    8, 8,0xB410, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)正向有功总最大需量发生时间
    {{0x10100202},   0,  0x01010101,    8, 8,0xA411, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率1正向有功最大需量
    {{0x10100202},   0,  0x01010101,    8, 8,0xB411, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率1正向有功最大需量发生时间,格式同上
    {{0x10100203},   0,  0x01010201,    8, 8,0xA412, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率2正向有功最大需量
    {{0x10100203},   0,  0x01010201,    8, 8,0xB412, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率2正向有功最大需量发生时间,格式同上
    {{0x10100204},   0,  0x01010301,    8, 8,0xA413, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率3正向有功最大需量
    {{0x10100204},   0,  0x01010301,    8, 8,0xB413, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率3正向有功最大需量发生时间,格式同上
    {{0x10100205},   0,  0x01010401,    8, 8,0xA414, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率4正向有功最大需量
    {{0x10100205},   0,  0x01010401,    8, 8,0xB414, E_OOP_DEMAND_T,  0             },  // 5字节,YYMMDDHHMM,(结算日)费率4正向有功最大需量发生时间,格式同上
                                           
    //(结算日)反向有功总最大需量           
    {{0x10200200},   0,  0x0102FF01,    8,40, 0xA42F, E_OOP_DEMAND_T,  0      },  // (结算日)反向有功最大需量块
    {{0x10200200},   0,  0x0102FF01,    8,40, 0xB42F, E_OOP_DEMAND_T,  0      },  // (结算日)反向有功最大需量块
    {{0x10200201},   0,  0x01020001,    8, 8, 0xA420, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)反向有功总最大需量
    {{0x10200201},   0,  0x01020001,    8, 8, 0xB420, E_OOP_DEMAND_T,  0                },  // 5字节,YYMMDDHHMM,(结算日)反向有功总最大需量发生时间
    {{0x10200202},   0,  0x01020101,    8, 8, 0xA421, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率1反向有功最大需量
    {{0x10200202},   0,  0x01020101,    8, 8, 0xB421, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率1反向有功最大需量发生时间,格式同上
    {{0x10200203},   0,  0x01020201,    8, 8, 0xA422, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率2反向无功向有功最大需量
    {{0x10200203},   0,  0x01020201,    8, 8, 0xB422, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率2反向无功向有功最大需量发生时间,格式同上
    {{0x10200204},   0,  0x01020301,    8, 8, 0xA423, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率3反向无功向有功最大需量
    {{0x10200204},   0,  0x01020301,    8, 8, 0xB423, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率3反向无功向有功最大需量发生时间,格式同上
    {{0x10200205},   0,  0x01020401,    8, 8, 0xA424, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率4反向无功向有功最大需量
    {{0x10200205},   0,  0x01020401,    8, 8, 0xB424, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率4反向无功向有功最大需量发生时间,格式同上
                                           
    //(结算日)正向无功总最大需量           
    {{0x10300200},   0,  0x0103FF01,    8,40, 0xA51F, E_OOP_DEMAND_T,  0      },  //  (结算日)正向无功最大需量块
    {{0x10300200},   0,  0x0103FF01,    8,40, 0xB51F, E_OOP_DEMAND_T,  0      },  //  (结算日)正向无功最大需量块
    {{0x10300201},   0,  0x01030001,    8, 8, 0xA510, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)正向无功总最大需量
    {{0x10300201},   0,  0x01030001,    8, 8, 0xB510, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)正向无功总最大需量发生时间
    {{0x10300202},   0,  0x01030101,    8, 8, 0xA511, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率1正向无功最大需量
    {{0x10300202},   0,  0x01030101,    8, 8, 0xB511, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率1正向无功最大需量发生时间,格式同上
    {{0x10300203},   0,  0x01030201,    8, 8, 0xA512, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率2正向无功最大需量
    {{0x10300203},   0,  0x01030201,    8, 8, 0xB512, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率2正向无功最大需量发生时间,格式同上
    {{0x10300204},   0,  0x01030301,    8, 8, 0xA513, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率3正向无功最大需量
    {{0x10300204},   0,  0x01030301,    8, 8, 0xB513, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率3正向无功最大需量发生时间,格式同上
    {{0x10300205},   0,  0x01030401,    8, 8, 0xA514, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率4正向无功最大需量
    {{0x10300205},   0,  0x01030401,    8, 8, 0xB514, E_OOP_DEMAND_T,  0              },  // 5字节,YYMMDDHHMM,(结算日)费率4正向无功最大需量发生时间,格式同上
                                           
    //(结算日)反向无功总最大需量           
    {{0x10400200},   0,  0x0104FF01,    8,40, 0xA52F, E_OOP_DEMAND_T,  0      },  //(结算日)反向无功最大需量块
    {{0x10400200},   0,  0x0104FF01,    8,40,0xB52F, E_OOP_DEMAND_T,  0      },  //(结算日)反向无功最大需量块
    {{0x10400201},   0,  0x01040001,    8, 8,0xA520, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)反向无功总最大需量
    {{0x10400201},   0,  0x01040001,    8, 8,0xB520, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)反向无功总最大需量发生时间
    {{0x10400202},   0,  0x01040101,    8, 8,0xA521, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率1反向无功最大需量
    {{0x10400202},   0,  0x01040101,    8, 8,0xB521, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率1反向无功最大需量发生时间,格式同上
    {{0x10400203},   0,  0x01040201,    8, 8,0xA522, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率2反向无功最大需量
    {{0x10400203},   0,  0x01040201,    8, 8,0xB522, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率2反向无功最大需量发生时间,格式同上
    {{0x10400204},   0,  0x01040301,    8, 8,0xA523, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率3反向无功最大需量
    {{0x10400204},   0,  0x01040301,    8, 8,0xB523, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率3反向无功最大需量发生时间,格式同上
    {{0x10400205},   0,  0x01040401,    8, 8,0xA524, E_OOP_DEMAND_T,  0      },  // 3字节, XX.XXXX,  (结算日)费率4反向无功最大需量
    {{0x10400205},   0,  0x01040401,    8, 8,0xB524, E_OOP_DEMAND_T,  0           },  // 5字节,YYMMDDHHMM,(结算日)费率4反向无功最大需量发生时间,格式同上
    //A相电压合格率
    {{0x21310201},   0,  0x03100100,13,13,0xFFFF,E_OOP_VOLTQR_T,     0 },   //电压合格率
    //B相电压合格率
    {{0x21320201},   0,  0x03100200,13,13,0xFFFF,E_OOP_VOLTQR_T ,    0 },   //电压合格率
    //C相电压合格率
    {{0x21330201},   0,  0x03100300,13,13,0xFFFF, E_OOP_VOLTQR_T ,   0 },   //电压合格率

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF     },
    
};

const MapOopTo645_NEW MapOopTo645_Unit_1_1[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x10010101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x10010201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x10010301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x10010401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x10010501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)

    {{0x00112201},-2, 0x10010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x10010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x10010801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x10010901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x10010A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x10010B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x10010C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x10010D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x10010E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    
    {{0x00122201},-2, 0x10010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x10011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x10011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x10011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x10011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x10011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x10011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x10011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x10011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数

    {{0x00132201},-2, 0x10011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x10011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x10011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x10011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x10011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x10011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x10011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x10011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x10012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数

    {{0x20292201},-2, 0x10012101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x10012201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x10012301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x10012401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    
    {{0x20200200}, 0, 0x10012501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x10012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x10012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x10012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x10012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)

    {{0x00118201},-2, 0x10012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x10012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x10012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x10012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x10012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x10012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x10013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x10013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x10013201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x10013301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x10013401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x10013501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20250201}, 0, 0x10010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值(发生次数、累计时间)
    {{0x20250202}, 0, 0x10010002,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值-发生次数
    {{0x20220200}, 0, 0x10010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};

const MapOopTo645_NEW MapOopTo645_Unit_5_1[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x18010101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x18010201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x18010301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x18010401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x18010501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x18010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x18010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x18010801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x18010901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x18010A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x18010B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x18010C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x18010D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x18010E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x18010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x18011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x18011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x18011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x18011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x18011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x18011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x18011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x18011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x18011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x18011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x18011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x18011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x18011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x18011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x18011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x18011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x18012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x18012101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x18012201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x18012301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x18012401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x18012501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x18012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x18012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x18012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x18012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x18012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x18012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x18012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x18012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x18012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x18012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x18013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x18013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x18010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};

const MapOopTo645_NEW MapOopTo645_Unit_1_0[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x20220200}, 0, 0x10020002,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
};
const MapOopTo645_NEW MapOopTo645_Unit_1_2[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x10020101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x10020201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x10020301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x10020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x10020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)

    {{0x00112201},-2, 0x10020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x10020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x10020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x10020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x10020A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x10020B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x10020C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x10020D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x10020E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    
    {{0x00122201},-2, 0x10020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x10021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x10021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x10021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x10021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x10021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x10021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x10021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x10021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数

    {{0x00132201},-2, 0x10021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x10021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x10021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x10021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x10021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x10021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x10021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x10021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x10022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数

    {{0x20292201},-2, 0x10022101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x10022201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x10022301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x10022401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x10022501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    
    {{0x00108201},-2, 0x10022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x10022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x10022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x10022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)

    {{0x00118201},-2, 0x10022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x10022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x10022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x10022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x10022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x10022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x10023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x10023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x10023201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x10023301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x10023401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x10023501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20250201}, 0, 0x10020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值(发生次数、累计时间)
    {{0x20250202}, 0, 0x10020002,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值-发生次数
    {{0x20220200}, 0, 0x10020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_1_3[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x10030101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x10030201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x10030301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x10030401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x10030501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x10030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x10030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x10030801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x10030901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x10030A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x10030B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x10030C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x10030D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x10030E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x10030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x10031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x10031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x10031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x10031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x10031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x10031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x10031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x10031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x10031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x10031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x10031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x10031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x10031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x10031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x10031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x10031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x10032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x10032101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x10032201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x10032301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x10032401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    
    {{0x20200200}, 0, 0x10032501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x10032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x10032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x10032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x10032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x10032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x10032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x10032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x10032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x10032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x10032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x10033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x10033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x10033201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x10033301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x10033401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x10033501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x10030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_1_4[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x11010101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x11010201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x11010301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x11010401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x11010501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x11010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x11010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x11010801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x11010901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x11010A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x11010B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x11010C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x11010D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x11010E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x11010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x11011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x11011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x11011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x11011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x11011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x11011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x11011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x11011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x11011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x11011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x11011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x11011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x11011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x11011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x11011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x11011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x11012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x11012101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x11012201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x11012301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x11012401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x11012501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x11012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x11012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x11012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x11012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x11012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x11012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x11012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x11012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x11012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x11012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x11013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x11013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x11013201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x11013301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x11013401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x11013501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x11010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_1_5[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x11020101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x11020201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x11020301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x11020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x11020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x11020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x11020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x11020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x11020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x11020A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x11020B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x11020C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x11020D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x11020E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x11020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x11021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x11021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x11021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x11021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x11021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x11021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x11021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x11021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x11021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x11021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x11021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x11021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x11021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x11021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x11021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x11021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x11022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x11022101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x11022201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x11022301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x11022401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x11022501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x11022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x11022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x11022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x11022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x11022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x11022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x11022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x11022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x11022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x11022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x11023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x11023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x11023201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x11023301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x11023401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x11023501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x11020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_1_6[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x11030101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x11030201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x11030301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x11030401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x11030501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x11030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x11030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x11030801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x11030901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x11030A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x11030B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x11030C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x11030D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x11030E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x11030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x11031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x11031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x11031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x11031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x11031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x11031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x11031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x11031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x11031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x11031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x11031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x11031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x11031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x11031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x11031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x11031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x11032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x11032101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x11032201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x11032301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x11032401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x11032501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x11032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x11032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x11032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x11032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x11032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x11032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x11032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x11032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x11032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x11032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x11033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x11033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x11033201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x11033301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x11033401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x11033501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x11030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_1_7[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x12010101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x12010201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x12010301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x12010401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x12010501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x12010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x12010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x12010801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x12010901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x12010A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x12010B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x12010C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x12010D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x12010E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x12010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x12011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x12011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x12011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x12011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x12011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x12011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x12011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x12011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x12011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x12011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x12011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x12011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x12011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x12011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x12011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x12011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x12012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x12012101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x12012201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x12012301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x12012401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x12012501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x12012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x12012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x12012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x12012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x12012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x12012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x12012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x12012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x12012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x12012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x12013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x12013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x12013201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x12013301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x12013401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x12013501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x12010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_1_8[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x12020101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x12020201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x12020301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x12020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x12020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x12020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x12020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x12020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x12020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x12020A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x12020B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x12020C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x12020D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x12020E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x12020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x12021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x12021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x12021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x12021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x12021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x12021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x12021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x12021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x12021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x12021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x12021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x12021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x12021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x12021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x12021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x12021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x12022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x12022101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x12022201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x12022301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x12022401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x12022501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x12022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x12022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x12022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x12022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x12022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x12022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x12022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x12022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x12022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x12022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x12023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x12023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x12023201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x12023301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x12023401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x12023501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x12020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_1_9[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x12030101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x12030201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x12030301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x12030401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x12030501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x12030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x12030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x12030801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x12030901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x12030A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x12030B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x12030C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x12030D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x12030E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x12030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x12031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x12031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x12031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x12031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x12031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x12031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x12031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x12031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x12031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x12031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x12031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x12031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x12031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x12031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x12031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x12031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x12032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x12032101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x12032201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x12032301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x12032401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x12032501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x12032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x12032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x12032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x12032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x12032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x12032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x12032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x12032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x12032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x12032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x12033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x12033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x12033201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x12033301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x12033401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x12033501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x12030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_1_10[]=//失压，欠压，过压，断相电能质量事件单元
{
    
    {{0x201E0200}, 0, 0x13010101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x13010201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x13010301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x13010401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x13010501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x13010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x13010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x13010801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x13010901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x13010A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x13010B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x13010C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x13010D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x13010E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x13010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x13011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x13011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x13011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x13011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x13011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x13011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x13011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x13011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x13011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x13011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x13011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x13011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x13011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x13011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x13011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x13011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x13012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x13012101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x13012201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x13012301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x13012401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x13012501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x13012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x13012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x13012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x13012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x13012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x13012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x13012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x13012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x13012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x13012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x13013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x13013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x13013201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x13013301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x13013401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x13013501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20250201}, 0, 0x13010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值(发生次数、累计时间)
    {{0x20250202}, 0, 0x13010002,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件当前值-发生次数
    {{0x20220200}, 0, 0x13010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_1_11[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x13020101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x13020201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x13020301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x13020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x13020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x13020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x13020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x13020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x13020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x13020A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x13020B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x13020C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x13020D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x13020E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x13020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x13021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x13021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x13021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x13021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x13021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x13021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x13021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x13021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x13021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x13021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x13021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x13021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x13021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x13021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x13021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x13021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x13022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x13022101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x13022201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x13022301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x13022401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x13022501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x13022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x13022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x13022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x13022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x13022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x13022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x13022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x13022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x13022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x13022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x13023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x13023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x13023201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x13023301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x13023401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x13023501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x13020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_1_12[]=//失压，欠压，过压，断相电能质量事件单元
{
    {{0x201E0200}, 0, 0x13030101,6,6,0xFFFF ,E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x13030201,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x13030301,4,4,0xFFFF ,E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x13030401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x13030501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x13030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x13030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x13030801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x13030901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x13030A01,2,2,0xFFFF, E_OOP_WORD3V_T,  -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x13030B01,3,3,0xFFFF, E_OOP_INT3V_T,  -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x13030C01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x13030D01,3,3,0xFFFF, E_OOP_INT4V_T,    -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x13030E01,2,2,0xFFFF, E_OOP_LONG4V_T,   -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x13030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x13031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x13031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x13031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x13031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x13031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x13031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x13031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x13031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x13031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x13031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x13031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x13031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x13031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x13031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x13031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x13031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x13032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20292201},-2, 0x13032101,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, 总安时数
    {{0x20292202},-2, 0x13032201,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, A相安时数
    {{0x20292203},-2, 0x13032301,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, B相安时数
    {{0x20292204},-2, 0x13032401,4,4,0xFFFF, E_DOUBLE_LONG_UNSIGNED,-2},  // 4字节, XXXXXX.XX, C相安时数
    {{0x20200200}, 0, 0x13032501,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x13032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x13032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x13032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x13032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x13032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x13032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x13032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x13032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x13032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x13032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x13033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x13033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x13033201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x13033301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x13033401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x13033501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x13030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_1[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1B010101,6,6, 0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1B010201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1B010301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1B010401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1B010501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1B010601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1B010701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1B010801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1B010901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1B010A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1B010B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1B010C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1B010D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1B010E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1B010F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1B011001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1B011101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1B011201,6,6, 0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1B011301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1B011401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1B011501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1B011601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1B011701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1B011801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1B011901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1B011A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1B011B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1B011C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1B011D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1B011E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1B011F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1B012001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1B012101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1B012201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1B010001,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_2[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1B020101,6,6,0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1B020201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1B020301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1B020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1B020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1B020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1B020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1B020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1B020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1B020A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1B020B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1B020C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1B020D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1B020E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1B020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1B021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1B021101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1B021201,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1B021301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1B021401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1B021501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1B021601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1B021701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1B021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1B021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1B021A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1B021B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1B021C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1B021D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1B021E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1B021F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1B022001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1B022101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1B022201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1B020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_3[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1B030101,6,6, 0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1B030201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1B030301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1B030401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1B030501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1B030601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1B030701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1B030801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1B030901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1B030A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1B030B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1B030C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1B030D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1B030E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1B030F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1B031001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1B031101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1B031201,6,6, 0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1B031301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1B031401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1B031501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1B031601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1B031701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1B031801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1B031901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1B031A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1B031B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1B031C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1B031D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1B031E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1B031F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1B032001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1B032101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1B032201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1B030001,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_4[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1C010101,6,6, 0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1C010201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1C010301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1C010401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1C010501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1C010601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1C010701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1C010801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1C010901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1C010A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1C010B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1C010C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1C010D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1C010E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1C010F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1C011001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1C011101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1C011201,6,6, 0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1C011301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1C011401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1C011501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1C011601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1C011701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1C011801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1C011901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1C011A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1C011B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1C011C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1C011D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1C011E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1C011F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1C012001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1C012101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1C012201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1C010001,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_5[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1C020101,6,6,0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1C020201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1C020301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1C020401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1C020501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1C020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1C020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1C020801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1C020901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1C020A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1C020B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1C020C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1C020D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1C020E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1C020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1C021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1C021101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1C021201,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1C021301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1C021401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1C021501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1C021601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1C021701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1C021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1C021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1C021A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1C021B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1C021C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1C021D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1C021E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1C021F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1C022001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1C022101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1C022201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1C020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_6[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},    0, 0x1C030101,6,6,0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},   -2, 0x1C030201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},   -2, 0x1C030301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},   -2, 0x1C030401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},   -2, 0x1C030501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},   -2, 0x1C030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},   -2, 0x1C030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},   -2, 0x1C030801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},   -2, 0x1C030901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},   -2, 0x1C030A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},   -2, 0x1C030B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},   -2, 0x1C030C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},   -2, 0x1C030D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},   -2, 0x1C030E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},   -2, 0x1C030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},   -2, 0x1C031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},   -2, 0x1C031101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},    0, 0x1C031201,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},   -2, 0x1C031301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},   -2, 0x1C031401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},   -2, 0x1C031501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},   -2, 0x1C031601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},   -2, 0x1C031701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},   -2, 0x1C031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},   -2, 0x1C031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},   -2, 0x1C031A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},   -2, 0x1C031B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},   -2, 0x1C031C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},   -2, 0x1C031D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},   -2, 0x1C031E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},   -2, 0x1C031F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},   -2, 0x1C032001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},   -2, 0x1C032101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},   -2, 0x1C032201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},    0, 0x1C030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_7[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},      0, 0x14000101,6,6,0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},      1, 0x14000201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},      1, 0x14000301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},      1, 0x14000401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},      1, 0x14000501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},      1, 0x14000601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},      1, 0x14000701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},      1, 0x14000801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},      1, 0x14000901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},      1, 0x14000A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},      1, 0x14000B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},      1, 0x14000C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},      1, 0x14000D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},      1, 0x14000E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},      1, 0x14000F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},      1, 0x14001001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},      1, 0x14001101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},      0, 0x14001201,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},      4, 0x14001301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},      4, 0x14001401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},      4, 0x14001501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},      4, 0x14001601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},      4, 0x14001701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},      4, 0x14001801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},      4, 0x14001901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},      4, 0x14001A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},      4, 0x14001B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},      4, 0x14001C01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},      4, 0x14001D01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},      4, 0x14001E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},      4, 0x14001F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},      4, 0x14002001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},      4, 0x14002101,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},      4, 0x14002201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},      0, 0x14000001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_2_8[]=//逆相序，潮流反向，过载电能质量事件单元
{
    {{0x201E0200},      0, 0x15000101,6,6, 0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},      1, 0x15000201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},      1, 0x15000301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},      1, 0x15000401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},      1, 0x15000501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},      1, 0x15000601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},      1, 0x15000701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},      1, 0x15000801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},      1, 0x15000901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},      1, 0x15000A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},      1, 0x15000B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},      1, 0x15000C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},      1, 0x15000D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},      1, 0x15000E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},      1, 0x15000F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},      1, 0x15001001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},      1, 0x15001101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},      0, 0x15001201,6,6, 0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},      4, 0x15001301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},      4, 0x15001401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},      4, 0x15001501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},      4, 0x15001601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},      4, 0x15001701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},      4, 0x15001801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},      4, 0x15001901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},      4, 0x15001A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},      4, 0x15001B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},      4, 0x15001C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},      4, 0x15001D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},      4, 0x15001E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},      4, 0x15001F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},      4, 0x15002001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},      4, 0x15002101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},      4, 0x15002201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},      0, 0x15000001,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_3[]=//不平衡电能质量事件单元
{
    {{0x201E0200},     0, 0x16000101,6,6,0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},    -2, 0x16000201,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},    -2, 0x16000301,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},    -2, 0x16000401,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},    -2, 0x16000501,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},    -2, 0x16000601,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},    -2, 0x16000701,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},    -2, 0x16000801,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},    -2, 0x16000901,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},    -2, 0x16000A01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},    -2, 0x16000B01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},    -2, 0x16000C01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},    -2, 0x16000D01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},    -2, 0x16000E01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},    -2, 0x16000F01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},    -2, 0x16001001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},    -2, 0x16001101,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20262200},     0, 0x16001201,3,3,0xFFFF,  E_LONG_UNSIGNED,0},   // 3字节, XXXX.XX 电流不平衡最大不平衡率
    {{0x20200200},    -2, 0x16001301,6,6,0xFFFF,  E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},    -2, 0x16001401,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},    -2, 0x16001501,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},    -2, 0x16001601,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},    -2, 0x16001701,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},    -2, 0x16001801,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},    -2, 0x16001901,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},    -2, 0x16001A01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},    -2, 0x16001B01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},    -2, 0x16001C01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},    -2, 0x16001D01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},    -2, 0x16001E01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},    -2, 0x16001F01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},    -2, 0x16002001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},    -2, 0x16002101,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},    -2, 0x16002201,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},    -2, 0x16002301,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},     0, 0x16000001,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_4[]=//不平衡电能质量事件单元
{
    {{0x201E0200},     0, 0x17000101,6,6,0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},    -2, 0x17000201,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},    -2, 0x17000301,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},    -2, 0x17000401,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},    -2, 0x17000501,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},    -2, 0x17000601,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},    -2, 0x17000701,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},    -2, 0x17000801,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},    -2, 0x17000901,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00122201},    -2, 0x17000A01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},    -2, 0x17000B01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},    -2, 0x17000C01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},    -2, 0x17000D01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00132201},    -2, 0x17000E01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},    -2, 0x17000F01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},    -2, 0x17001001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},    -2, 0x17001101,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20272200},     0, 0x17001201,4,4,0xFFFF,  E_LONG_UNSIGNED,0},   // 3字节, XXXX.XX 电流不平衡最大不平衡率
    {{0x20200200},    -2, 0x17001301,6,6,0xFFFF,  E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},    -2, 0x17001401,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},    -2, 0x17001501,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},    -2, 0x17001601,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},    -2, 0x17001701,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},    -2, 0x17001801,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},    -2, 0x17001901,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},    -2, 0x17001A01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},    -2, 0x17001B01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},    -2, 0x17001C01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},    -2, 0x17001D01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},    -2, 0x17001E01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},    -2, 0x17001F01,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},    -2, 0x17002001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},    -2, 0x17002101,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},    -2, 0x17002201,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},    -2, 0x17002301,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},     0, 0x17000001,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_2[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x18020101,6,6,0xFFFF, E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x18020201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x18020301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x18020401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x18020501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x18020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x18020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x18020801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x18020901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x18020A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x18020B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x18020C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x18020D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x18020E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x18020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x18021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x18021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x18021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x18021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x18021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x18021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x18021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x18021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x18021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x18021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x18021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x18021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x18021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x18021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x18021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x18021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x18022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x18022101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x18022201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x18022301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x18022401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x18022501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x18022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x18022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x18022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x18022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x18022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x18022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x18022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x18022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x18022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x18022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x18023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x18023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x18020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_3[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x18030101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x18030201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x18030301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x18030401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x18030501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x18030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x18030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x18030801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x18030901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x18030A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x18030B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x18030C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x18030D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x18030E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x18030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x18031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x18031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x18031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x18031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x18031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x18031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x18031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x18031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x18031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x18031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x18031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x18031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x18031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x18031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x18031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x18031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x18032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x18032101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x18032201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x18032301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x18032401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x18032501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x18032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x18032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x18032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x18032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x18032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x18032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x18032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x18032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x18032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x18032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x18033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x18033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x18030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_4[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x19010101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x19010201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x19010301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x19010401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x19010501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x19010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x19010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x19010801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x19010901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x19010A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x19010B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x19010C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x19010D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x19010E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x19010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x19011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x19011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x19011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x19011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x19011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x19011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x19011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x19011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x19011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x19011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x19011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x19011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x19011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x19011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x19011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x19011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x19012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x19012101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x19012201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x19012301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x19012401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x19012501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x19012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x19012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x19012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x19012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x19012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x19012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x19012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x19012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x19012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x19012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x19013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x19013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x19010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_5[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x19020101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x19020201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x19020301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x19020401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x19020501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x19020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x19020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x19020801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x19020901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x19020A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x19020B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x19020C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x19020D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x19020E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x19020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x19021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x19021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x19021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x19021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x19021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x19021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x19021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x19021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x19021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x19021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x19021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x19021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x19021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x19021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x19021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x19021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x19022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x19022101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x19022201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x19022301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x19022401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x19022501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x19022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x19022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x19022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x19022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x19022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x19022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x19022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x19022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x19022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x19022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x19023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x19023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x19020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_6[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x19030101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x19030201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x19030301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x19030401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x19030501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x19030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x19030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x19030801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x19030901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x19030A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x19030B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x19030C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x19030D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x19030E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x19030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x19031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x19031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x19031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x19031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x19031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x19031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x19031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x19031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x19031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x19031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x19031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x19031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x19031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x19031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x19031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x19031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x19032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x19032101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x19032201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x19032301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x19032401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x19032501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x19032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x19032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x19032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x19032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x19032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x19032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x19032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x19032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x19032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x19032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x19033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x19033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x19030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_7[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x1A010101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x1A010201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x1A010301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x1A010401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x1A010501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x1A010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x1A010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x1A010801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x1A010901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x1A010A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x1A010B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x1A010C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x1A010D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x1A010E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x1A010F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x1A011001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x1A011101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x1A011201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x1A011301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x1A011401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x1A011501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x1A011601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x1A011701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x1A011801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x1A011901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x1A011A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x1A011B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x1A011C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x1A011D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x1A011E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x1A011F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x1A012001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x1A012101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x1A012201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x1A012301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x1A012401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x1A012501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x1A012601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x1A012701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x1A012801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x1A012901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x1A012A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x1A012B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x1A012C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x1A012D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x1A012E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x1A012F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x1A013001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x1A013101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x1A010001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_8[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x1A020101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x1A020201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x1A020301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x1A020401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x1A020501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x1A020601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x1A020701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x1A020801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x1A020901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x1A020A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x1A020B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x1A020C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x1A020D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x1A020E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x1A020F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x1A021001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x1A021101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x1A021201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x1A021301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x1A021401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x1A021501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x1A021601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x1A021701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x1A021801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x1A021901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x1A021A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x1A021B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x1A021C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x1A021D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x1A021E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x1A021F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x1A022001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x1A022101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x1A022201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x1A022301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x1A022401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x1A022501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x1A022601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x1A022701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x1A022801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x1A022901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x1A022A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x1A022B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x1A022C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x1A022D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x1A022E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x1A022F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x1A023001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x1A023101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x1A020001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};
const MapOopTo645_NEW MapOopTo645_Unit_5_9[]=//失流，过流，断流，电能质量事件单元
{
    {{0x201E0200}, 0, 0x1A030101,6,6,0xFFFF ,E_DATETIME_S,0},   //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},-2, 0x1A030201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},-2, 0x1A030301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},-2, 0x1A030401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},-2, 0x1A030501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00112201},-2, 0x1A030601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00212201},-2, 0x1A030701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00312201},-2, 0x1A030801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00412201},-2, 0x1A030901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002201},-1, 0x1A030A01,2,2,0xFFFF, E_OOP_WORD3V_T, -1}, // 2字节, X.XXX A相电压/AB线电压/单向表电压
    {{0x20012201},-3, 0x1A030B01,3,3,0xFFFF, E_OOP_INT3V_T, -3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20042202},-1, 0x1A030C01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052202},-1, 0x1A030D01,3,3,0xFFFF, E_OOP_INT4V_T, -4},  // 3字节, XX.XXXX, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2202},-3, 0x1A030E01,2,2,0xFFFF, E_OOP_LONG4V_T,    -3},  // 2字节, XX.XXXX, A相功率因数
    {{0x00122201},-2, 0x1A030F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00222201},-2, 0x1A031001,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00322201},-2, 0x1A031101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00422201},-2, 0x1A031201,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002202},-1, 0x1A031301,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX B相电压/AB线电压/单向表电压
    {{0x20012202},-3, 0x1A031401,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, B相电流/A相电流/单向表电流
    {{0x20042203},-1, 0x1A031501,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052203},-1, 0x1A031601,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, B相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2203},-3, 0x1A031701,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, B相功率因数
    {{0x00132201},-2, 0x1A031801,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00232201},-2, 0x1A031901,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00332201},-2, 0x1A031A01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00432201},-2, 0x1A031B01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20002203},-1, 0x1A031C01,2,2,0xFFFF, E_OOP_WORD3V_T,-1}, // 2字节, X.XXX C相电压/AB线电压/单向表电压
    {{0x20012203},-3, 0x1A031D01,3,3,0xFFFF, E_OOP_INT3V_T,-3}, // 3字节, XXX.XXX, C相电流/A相电流/单向表电流
    {{0x20042204},-1, 0x1A031E01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x20052204},-1, 0x1A031F01,3,3,0xFFFF, E_OOP_INT4V_T,  -4},  // 3字节, XX.XXXX, C相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    {{0x200A2204},-3, 0x1A032001,2,2,0xFFFF, E_OOP_LONG4V_T, -3},  // 2字节, XX.XXXX, C相功率因数
    {{0x20200200}, 0, 0x1A032101,6,6,0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},-2, 0x1A032201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},-2, 0x1A032301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},-2, 0x1A032401,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},-2, 0x1A032501,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00118201},-2, 0x1A032601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00218201},-2, 0x1A032701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00318201},-2, 0x1A032801,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00418201},-2, 0x1A032901,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00128201},-2, 0x1A032A01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00228201},-2, 0x1A032B01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00328201},-2, 0x1A032C01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00428201},-2, 0x1A032D01,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x00138201},-2, 0x1A032E01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00238201},-2, 0x1A032F01,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00338201},-2, 0x1A033001,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00438201},-2, 0x1A033101,4,4,0xFFFF, E_OOP_ENERGYS_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200}, 0, 0x1A030001,3,3,0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF   },

};

const MapOopTo645_NEW MapOopTo645_Unit_6_1[]=//跳闸，合闸事件记录单元
{
    {{0x201E0200},      0, 0x1D010101,6,6,0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},      0, 0x1D010201,4,4,0xFFFF,  E_NULL,0},  // 4字节, 操作者代码
    {{0x00102201},     -2, 0x1D010201,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},     -2, 0x1D010301,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00502201},     -2, 0x1D010401,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)1象限无功总电能
    {{0x00602201},     -2, 0x1D010501,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)2象限无功总电能
    {{0x00702201},     -2, 0x1D010601,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)3象限有功总电能
    {{0x00802201},     -2, 0x1D010701,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)4象限有功总电能
    {{0x20220200},     0 , 0x1D000001,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_6_2[]=//跳闸，合闸事件记录单元
{
    {{0x201E0200},      0, 0x1E010101,6,6,0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},      0, 0x1E010201,4,4,0xFFFF, E_NULL,0},  // 4字节, 操作者代码
    {{0x00102201},     -2, 0x1E010201,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},     -2, 0x1E010301,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00502201},     -2, 0x1E010401,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)1象限无功总电能
    {{0x00602201},     -2, 0x1E010501,4,4,0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)2象限无功总电能
    {{0x00702201},     -2, 0x1E010601,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)3象限有功总电能
    {{0x00802201},     -2, 0x1E010701,4,4,0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)4象限有功总电能
    {{0x20220200},      0, 0x1E000001,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号

    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },


};
const MapOopTo645_NEW MapOopTo645_Unit_7[]=//总功率因数超下限电能质量事件单元
{
    {{0x201E0200},      0, 0x1F000101,6,6, 0xFFFF, E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},     -2, 0x1F000201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},     -2, 0x1F000301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00302201},     -2, 0x1F000401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00402201},     -2, 0x1F000501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20200200},      0, 0x1F000601,6,6, 0xFFFF, E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x00108201},     -2, 0x1F000701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},     -2, 0x1F000801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00308201},     -2, 0x1F000901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX (当前)正向无功总电能(+RL,+RC)
    {{0x00408201},     -2, 0x1F000A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节, XXXXXX.XX (当前)反向无功总电能(-RL,-RC)
    {{0x20220200},     -2, 0x1F000001,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},   0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },

};
const MapOopTo645_NEW MapOopTo645_Unit_8[]=//全失压电能质量事件单元
{
    {{0x201E0200},  0, 0x03050001,6,6,0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20012201}, -3, 0x03050001,3,3,0xFFFF,  E_OOP_INT4V_T,-3}, // 3字节, XXX.XXX, A相电流/A相电流/单向表电流
    {{0x20200200},  0, 0x03050001,6,6,0xFFFF,  E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x20220200},  0, 0x03050000,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_9[]=//辅助电源掉电，电表掉电
{
    {{0x201E0200},  0, 0x03060001,6,6, 0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},  0, 0x03060001,6,6, 0xFFFF,  E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x20220200},  0, 0x03060000,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_38[]=//辅助电源掉电，电表掉电
{
    {{0x201E0200},  0, 0x03110001,6,6, 0xFFFF,  E_DATETIME_S,0},     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},  0, 0x03110001,6,6, 0xFFFF,  E_DATETIME_S,0},  //6字节, YYMMDDhhmmss 结束时刻
    {{0x20220200},  0, 0x03110000,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF    },
};
const MapOopTo645_NEW MapOopTo645_Unit_10[]=//正向有功需量超限
{
    {{0x201E0200}, 0,  0x03120101,6,6,0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120101,6,6,0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120101,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120101,5,5,0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0x20220200}, 0,  0x03120000,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_11[]=//反向有功需量超限
{
    {{0x201E0200}, 0,  0x03120201,6,6,0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120201,6,6,0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120201,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120201,5,5,0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0xFFFFFFFE}, 0,  0x03120000,3,3,0xFFFF,   E_NULL,                0   },    //跳过偏移
    {{0x20220200}, 0,  0x03120000,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_12[]=//1象限无功需量超限
{
    {{0x201E0200}, 0,  0x03120301,6,6,0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120301,6,6,0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120301,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120301,5,5,0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0xFFFFFFFE}, 0,  0x03120000,6,6,0xFFFF,   E_NULL,                0   },    //跳过偏移
    {{0x20220200}, 0,  0x03120000,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_13[]=//2象限无功需量超限
{
    {{0x201E0200}, 0,  0x03120401,6,6,0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120401,6,6,0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120401,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120401,5,5,0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0xFFFFFFFE}, 0,  0x03120000,9,9,0xFFFF,   E_NULL,                0   },    //跳过偏移
    {{0x20220200}, 0,  0x03120000,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_14[]=//3象限无功需量超限
{
    {{0x201E0200}, 0,  0x03120501,6, 6, 0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120501,6, 6, 0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120501,3, 3, 0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120501,5, 5, 0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0xFFFFFFFE}, 0,  0x03120000,12,12,0xFFFF,  E_NULL,                0   },    //跳过偏移
    {{0x20220200}, 0,  0x03120000,3, 3, 0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_15[]=//4象限无功需量超限
{
    {{0x201E0200}, 0,  0x03120601,6, 6, 0xFFFF,   E_DATETIME_S,          0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200}, 0,  0x03120601,6, 6, 0xFFFF,   E_DATETIME_S,          0   },  //6字节, YYMMDDhhmmss 结束时刻
    {{0x33080206}, 0,  0x03120601,3, 3, 0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },  // 3字节, XX.XXXX,  (当前)正向有功总最大需量
    {{0x33080207}, 0,  0x03120601,5, 5, 0xFFFF,   E_DATETIME_S,          0   },  // 5字节,YYMMDDHHMM,(当前)正向有功总最大需量发生时间
    {{0xFFFFFFFE}, 0,  0x03120000,15,15,0xFFFF,   E_NULL,                0   },    //跳过偏移
    {{0x20220200}, 0,  0x03120000,3, 3, 0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0   },    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF ,               0xFF  },
};
const MapOopTo645_NEW MapOopTo645_Unit_16[]=//编程记录
{ 
    {{0x201E0200}, 0,  0x03300001,6,6,0xFFFF,   E_DATETIME_S, 0         },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20220200}, 0,  0x03300000,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF               },
};
const MapOopTo645_NEW MapOopTo645_Unit_17[]=//电表清零
{
    {{0x201E0200},  0, 0x03300101,6,6, 0xFFFF, E_DATETIME_S,        0   },     //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},  0, 0x03300101,4,4, 0xFFFF, E_NULL,              0   },
    {{0x00102201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00502201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00602201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00702201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00802201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x00112201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  A相正向有功总电能
    {{0x00212201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  A相反向有功总电能
    {{0x00512201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX A相1象限无功总电能
    {{0x00612201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX A相2象限无功总电能
    {{0x00712201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX A相3象限无功总电能
    {{0x00812201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX A相4象限无功总电能
    {{0x00122201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  正向有功总电能
    {{0x00222201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  反向有功总电能
    {{0x00522201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00622201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00722201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00822201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x00132201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  正向有功总电能
    {{0x00232201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX  反向有功总电能
    {{0x00532201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00632201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00732201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00832201}, -2, 0x03300101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x20220200}, -2, 0x03300100,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED, 0},    //事件序号
    {{0xFFFFFFFF}, 0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF              },

};
const MapOopTo645_NEW MapOopTo645_Unit_18[]=//开表盖，端子盖事件
{
    {{0x201E0200},   0, 0x03300D01,6,6, 0xFFFF, E_DATETIME_S  ,   0      },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},   0, 0x03300D01,6,6, 0xFFFF, E_DATETIME_S  ,   0      },     //6字节, YYMMDDhhmmss 结束
    {{0x00102201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00502201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00602201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00702201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00802201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x00108201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00508201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00608201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00708201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00808201},  -2, 0x03300D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x20220200},   0, 0x03300D00,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0 },    //事件序号
    {{0xFFFFFFFF},      0xFF,  0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF        },

};
const MapOopTo645_NEW MapOopTo645_Unit_39[]=//开表盖，端子盖事件
{
    {{0x201E0200},   0, 0x03300E01,6,6, 0xFFFF,  E_DATETIME_S  ,   0       },     //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},   0, 0x03300E01,6,6, 0xFFFF,  E_DATETIME_S  ,   0       },     //6字节, YYMMDDhhmmss 结束
    {{0x00102201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00202201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00502201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00602201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00702201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00802201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x00108201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },  // 4字节, XXXXXX.XX(当前)正向有功总电能
    {{0x00208201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },  // 4字节, XXXXXX.XX(当前)反向有功总电能
    {{0x00508201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 1象限无功总电能
    {{0x00608201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 2象限无功总电能
    {{0x00708201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 3象限无功总电能
    {{0x00808201},  -2, 0x03300E01,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2 },    // 4字节,XXXXXX.XX 4象限无功总电能
    {{0x20220200},   0, 0x03300E00,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0  },    //事件序号
    {{0xFFFFFFFF}, 0xFF,  0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF               },

};
const MapOopTo645_NEW MapOopTo645_Unit_19[]=//需量清零
{
    {{0x201E0200}, 0, 0x03300201,6,6,0xFFFF,    E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE}, 0, 0x03300201,4,4,0xFFFF,    E_NULL, 0                  },
    {{0x10102201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  正向有功总最大需量                              
    {{0x10202201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  反向有功总最大需量                               
    {{0x10502201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  I象限无功总最大需量                               
    {{0x10602201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  II象限无功总最大需量                             
    {{0x10702201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  III象限无功总最大需量                              
    {{0x10802201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  IV象限无功总最大需量                              
    {{0x10112201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  A_正向有功总最大需量                              
    {{0x10212201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  A_反向有功总最大需量                             
    {{0x10512201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  A_I象限无功总最大需量                              
    {{0x10612201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  A_II象限无功总最大需量                              
    {{0x10712201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX, A_ III象限无功总最大需量                             
    {{0x10812201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_IV象限无功总最大需量                             
    {{0x10122201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_正向有功总最大需量                             
    {{0x10222201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_反向有功总最大需量                             
    {{0x10522201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_I象限无功总最大需量                              
    {{0x10622201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_II象限无功总最大需量                             
    {{0x10722201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_ III象限无功总最大需量                             
    {{0x10822201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  B_IV象限无功总最大需量                             
    {{0x10122201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_正向有功总最大需量                             
    {{0x10222201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_反向有功总最大需量                            
    {{0x10522201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_I象限无功总最大需量                             
    {{0x10622201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_II象限无功总最大需量                             
    {{0x10722201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_ III象限无功总最大需量                             
    {{0x10822201}, 0, 0x03300201,8,8,0xFFFF,    E_OOP_DEMAND_T,  0},  // 3字节, XX.XXXX,  C_IV象限无功总最大需量                             
    {{0x20220200}, 0, 0x03300200,8,8,0xFFFF,    E_DOUBLE_LONG_UNSIGNED,  0},    //事件序号
    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF                },

};
const MapOopTo645_NEW MapOopTo645_Unit_20[]=//电能表事件清零事件
{
    {{0x201E0200},    0, 0x03300301,6, 6,  0xFFFF  , E_DATETIME_S   ,0    },  //6字节, YYMMDDhhmmss 发生时刻                                    
    {{0x20220200},    0, 0x03300300,3, 3,  0xFFFF  , E_DOUBLE_LONG_UNSIGNED,0},    //事件序号
    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_21[]=//电能表校时事件
{
    {{0xFFFFFFFE},      0, 0x03300401,4,4, 0xFFFF, E_NULL,0              },
    {{0x201E0200},      0, 0x03300401,6,6, 0xFFFF, E_DATETIME_S  ,   0   },  //6字节, 校时前时间
    {{0x20200200},      0, 0x03300401,6,6, 0xFFFF, E_DATETIME_S  ,   0   },  //6字节, 校时后时间
    {{0x20220200},      0, 0x03300400,3,3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},   0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF              },    
};
const MapOopTo645_NEW MapOopTo645_Unit_22[]=//电能表时段表编程事件
{
    {{0x201E0200},    0, 0x03300501,6,   6,   0xFFFF, E_DATETIME_S, 0      },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},    0, 0x03300501,4,   4,   0xFFFF, E_NULL, 0                 },  //操作者代码
    {{0x40162201},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第1日时段表数据
    {{0x40162202},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第2日时段表数据
    {{0x40162203},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第3日时段表数据
    {{0x40162204},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第4日时段表数据
    {{0x40162205},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第5日时段表数据
    {{0x40162206},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第6日时段表数据
    {{0x40162207},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第7日时段表数据
    {{0x40162208},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第一套第8日时段表数据
    {{0x40172201},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第1日时段表数据
    {{0x40172202},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第2日时段表数据
    {{0x40172203},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第3日时段表数据
    {{0x40172204},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第4日时段表数据
    {{0x40172205},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第5日时段表数据
    {{0x40172206},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第6日时段表数据
    {{0x40172207},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第7日时段表数据
    {{0x40172208},    0, 0x03300501,3*14,3*14,0xFFFF, E_DAYPERIOD,0  },  //3*14字节,时段表编程前的第二套第8日时段表数据
    {{0x20220200},    0, 0x03300500,3,   3,   0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},    0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF       },

};
const MapOopTo645_NEW MapOopTo645_Unit_23[]=//电能表时区表编程事件
{
    {{0x201E0200},    0, 0x03300601,6,6,0xFFFF,   E_DATETIME_S, 0      },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFEFFFE},    0, 0x03300601,4,4,0xFFFF,   E_NULL, 0            },  //操作者代码
    {{0x40142201},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第1时区表数据
    {{0x40142202},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第2时区表数据
    {{0x40142203},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第3时区表数据
    {{0x40142204},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第4时区表数据
    {{0x40142205},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第5时区表数据
    {{0x40142206},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第6时区表数据
    {{0x40142207},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第7时区表数据
    {{0x40142208},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第8时区表数据
    {{0x40142209},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第9时区表数据
    {{0x4014220A},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第10时区表数据
    {{0x4014220B},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第11时区表数据
    {{0x4014220C},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第12时区表数据
    {{0x4014220D},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第13时区表数据
    {{0x4014220E},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第一套第14时区表数据
    {{0x40152201},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第1时区表数据
    {{0x40152202},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第2时区表数据
    {{0x40152203},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第3时区表数据
    {{0x40152204},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第4时区表数据
    {{0x40152205},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第5时区表数据
    {{0x40152206},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第6时区表数据
    {{0x40152207},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第7时区表数据
    {{0x40152208},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第8时区表数据
    {{0x40152209},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第9时区表数据
    {{0x4015220A},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第10时区表数据
    {{0x4015220B},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第11时区表数据
    {{0x4015220C},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第12时区表数据
    {{0x4015220D},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第13时区表数据
    {{0x4015220E},    0, 0x03300601,3,3,0xFFFF,   E_TIMEZONE,0},  //3字节,时区表编程前的第二套第14时区表数据
    {{0x20220200},    0, 0x03300600,3,3,0xFFFF,   E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF}, 0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF             },

};
const MapOopTo645_NEW MapOopTo645_Unit_24[]=//电能表周休日编程事件
{
    {{0x201E0200},     0, 0x03300701,6,6, 0xFFFF,  E_DATETIME_S, 0        },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},     0, 0x03300701,4,4, 0xFFFF,  E_NULL, 0                  },  //操作者代码
    {{0X40132200},     0, 0x03300701,1,1, 0xFFFF,  E_UNSIGNED,0},  //1字节, 周休日编程前采用的日时段表号
    {{0x20220200},     0, 0x03300700,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_25[]=//电能表结算日编程事件
{
    {{0x201E0200},   0, 0x03300C01,6, 6,  0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},   0, 0x03300C01,4, 4,  0xFFFF,  E_NULL, 0            },  //操作者代码
    {{0x41162201},   0, 0x03300C01,2,2, 0xFFFF,  E_OOP_PAYDAY_T,0     },  //2字节,结算日编程前每月第1结算日数据
    {{0x41162202},   0, 0x03300C01,2,2, 0xFFFF,  E_OOP_PAYDAY_T,0     },  //2字节,结算日编程前每月第1结算日数据
    {{0x41162203},   0, 0x03300C01,2,2, 0xFFFF,  E_OOP_PAYDAY_T,0     },  //2字节,结算日编程前每月第1结算日数据
    {{0x20220200},   0, 0x03300C00,3, 3,  0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0     },//事件序号
    {{0xFFFFFFFF}, 0xFF,  0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF          },

};
const MapOopTo645_NEW MapOopTo645_Unit_26[]=//电能表节假日编程事件
{
};
const MapOopTo645_NEW MapOopTo645_Unit_27[]=//电能表有功组合方式编程事件
{
    {{0x201E0200},   0, 0x03300901,6,6, 0xFFFF,    E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},   0, 0x03300901,4,4, 0xFFFF,    E_NULL, 0                  },  //操作者代码
    {{0X41122200},   0, 0x03300901,1,1, 0xFFFF,    E_BIT_STRING,0},  //1字节, 有功组合方式编程前的有功组合方式特征字
    {{0x20220200},   0, 0x03300900,3,3, 0xFFFF,    E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_28[]=//电能表费率参数表编程事件
{
};
const MapOopTo645_NEW MapOopTo645_Unit_29[]=//电能表阶梯表编程事件
{
    {{0x201E0200},         0, 0x03301001,6, 6, 0xFFFF, 0, e645DATETIME       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},         0, 0x03301001,4, 4, 0xFFFF, E_NULL, 0                  },  //操作者代码
    {{0x401A2200},         0, 0x03301001,64,64,0xFFFF, E_OOP_STEPPRICE_T,0},  //4字节, 阶梯表编程前当前套第1阶梯值
    {{0x401B2200},         0, 0x03301001,64,64,0xFFFF, E_OOP_STEPPRICE_T,0 },  //4字节, 阶梯表编程前备用套第1阶梯值
    {{0x20222200},         0, 0x03301000,3, 3, 0xFFFF, E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},      0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_30[]=//电能表密钥更新事件
{
     {{0x201E0200},     0, 0x03301201,6,6,0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
     {{0xFFFFFFFE},     0, 0x03301201,4,4,0xFFFF,  E_NULL, 0                  },  //操作者代码
     {{0x400F0200},     0, 0x03301201,1,1,0xFFFF,  E_UNSIGNED,0},  //1字节, 密钥更新的总条数
     {{0x20152200},     0, 0x03301201,4,4,0xFFFF,  E_BIT_STRING,0},  //4字节, 更新前的密钥状态字
     {{0x20220200},     0, 0x03301200,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
     {{0xFFFFFFFF},  0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            }, 
};
const MapOopTo645_NEW MapOopTo645_Unit_31[]=//电能表异常插卡事件
{
    {{0x201E0200},      0, 0x03301301,6, 6, 0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},      0, 0x03301301,16,16,0xFFFF,  E_NULL, 0                  },  //8字节, 卡序列号
    {{0x202C2202},      0, 0x03301301,2, 2, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},  //2字节, 插卡时总购电次数
    {{0x202C2201},     -2, 0x03301301,4, 4, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,-2},  //4字节, 插卡时剩余金额
    {{0x00102201},     -2, 0x03301301,4, 4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 插卡时正向有功总电能
    {{0x00202201},     -2, 0x03301301,4, 4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 插卡时反向有功总电能
    {{0x20220200},      0, 0x03301300,3, 3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},  0xFF,  0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_32[]=//电能表购电记录
{
};
const MapOopTo645_NEW MapOopTo645_Unit_33[]=//电能表退费记录
{
};
const MapOopTo645_NEW MapOopTo645_Unit_34[]=//电能表恒定磁场干扰事件
{
    {{0x201E0200},   0, 0x03350001,6,6, 0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},   0, 0x03350001,6,6, 0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 结束
    {{0x00102201},  -2, 0x03350001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 发生时刻正向有功总电能
    {{0x00202201},  -2, 0x03350001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 发生时刻反向有功总电能
    {{0x00108201},  -2, 0x03350001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 结束时刻正向有功总电能
    {{0x00208201},  -2, 0x03350001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 结束时刻反向有功总电能
    {{0x20220200},  -2, 0x03350000,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_35[]=//电能表负荷开关误动作事件
{
    {{0x201E0200},   0, 0x03360001,6,6, 0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},   0, 0x03360001,6,6, 0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 结束
    {{0xFFFFFFFE},   0, 0x03360001,1,1, 0xFFFF,  E_NULL, 0},  //1字节, 负荷开关误动作后状态
    {{0x00102201},  -2, 0x03360001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 负荷开关误动作发生时刻正向有功总电能
    {{0x00202201},  -2, 0x03360001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 负荷开关误动作发生时刻反向有功总电能
    {{0x00108201},  -2, 0x03360001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 负荷开关误动作结束时刻正向有功总电能
    {{0x00208201},  -2, 0x03360001,4,4, 0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 负荷开关误动作结束时刻反向有功总电能
    {{0x20220200},   0, 0x03360000,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_36[]=//电能表电源异常事件
{
    {{0x201E0200},   0, 0x03370001,6,6,0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0x20200200},   0, 0x03370001,6,6,0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 结束
    {{0x00102201},   1, 0x03370001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 电源异常发生时刻正向有功总电能
    {{0x00202201},   1, 0x03370001,4,4,0xFFFF,  E_OOP_ENERGY_T,-2},  //4字节, 电源异常发生时刻反向有功总电能
    {{0x20220200},   0, 0x03370000,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_37[]=//电能表电流严重不平衡事件
{
    {{0x201E0200},   0, 0x20000101,6,6, 0xFFFF, E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0x00102201},  -2, 0x20000201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻正向有功总电能
    {{0x00202201},  -2, 0x20000301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻反向有功总电能
    {{0x00302201},  -2, 0x20000401,4,4, 0xFFFF, E_OOP_ENERGYS_T,-2}, //4字节, 电流严重不平衡发生时刻组合无功1总电能
    {{0x00402201},  -2, 0x20000501,4,4, 0xFFFF, E_OOP_ENERGYS_T,-2},  //4字节, 电流严重不平衡发生时刻组合无功2总电能
    {{0x00112201},  -2, 0x20000601,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻A相正向有功电能
    {{0x00212201},  -2, 0x20000701,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻A相反向有功电能
    {{0x00312201},  -2, 0x20000801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻A相组合无功1电能
    {{0x00412201},  -2, 0x20000901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻A相组合无功2电能
    {{0x00122201},  -2, 0x20000A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻B相正向有功电能
    {{0x00222201},  -2, 0x20000B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻B相反向有功电能
    {{0x00322201},  -2, 0x20000C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻B相组合无功1电能
    {{0x00422201},  -2, 0x20000D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻B相组合无功2电能
    {{0x00132201},  -2, 0x20000E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻C相正向有功电能
    {{0x00232201},  -2, 0x20000F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻C相反向有功电能
    {{0x00332201},  -2, 0x20001001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻C相组合无功1电能
    {{0x00432201},  -2, 0x20001101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡发生时刻C相组合无功2电能
    {{0x20272200},  -2, 0x20001201,3,3, 0xFFFF, E_LONG_UNSIGNED,-2},  //3字节, 电流严重不平衡最大不平衡率
    {{0x20200200},   0, 0x20001301,6,6, 0xFFFF, E_DATETIME_S,0       },  //4字节, 电流严重不平衡结束时刻
    {{0x00108201},  -2, 0x20001401,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻正向有功总电能
    {{0x00208201},  -2, 0x20001501,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻反向有功总电能
    {{0x00308201},  -2, 0x20001601,4,4, 0xFFFF, E_OOP_ENERGYS_T,-2},  //4字节, 电流严重不平衡结束时刻组合无功1总电能
    {{0x00408201},  -2, 0x20001701,4,4, 0xFFFF, E_OOP_ENERGYS_T,-2},  //4字节, 电流严重不平衡结束时刻组合无功2总电能
    {{0x00118201},  -2, 0x20001801,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻A相正向有功电能
    {{0x00218201},  -2, 0x20001901,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻A相反向有功电能
    {{0x00318201},  -2, 0x20001A01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻A相组合无功1电能
    {{0x00418201},  -2, 0x20001B01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻A相组合无功2电能
    {{0x00128201},  -2, 0x20001C01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻B相正向有功电能
    {{0x00228201},  -2, 0x20001D01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻B相反向有功电能
    {{0x00328201},  -2, 0x20001E01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻B相组合无功1电能
    {{0x00428201},  -2, 0x20001F01,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻B相组合无功2电能
    {{0x00138201},  -2, 0x20002001,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻C相正向有功电能
    {{0x00238201},  -2, 0x20002101,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻C相反向有功电能
    {{0x00338201},  -2, 0x20002201,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻C相组合无功1电能
    {{0x00438201},  -2, 0x20002301,4,4, 0xFFFF, E_OOP_ENERGY_T,-2},  //4字节, 电流严重不平衡结束时刻C相组合无功2电
    {{0x20220200},  -2, 0x20000001,3,3, 0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},//事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },

};

const MapOopTo645_NEW MapOopTo645_Unit_45[]=//漏保跳闸事件
{
     {{0x201F0800},   0, 0x20000101,25,25, 0xFFFF, E_OOP_PROTECTOR_31BB_T,0       },  //6字节, YYMMDDhhmmss 发生时刻
     {{0x20220200},   0, 0x03810000,2,2, 0xFFFF,  E_LONG_UNSIGNED,0},//事件序号

     {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};

const MapOopTo645_NEW MapOopTo645_Unit_40[]=//电能表无功组合方式编程事件
{
    {{0x201E0200},  0, 0x03300A01,6,6,0xFFFF,  E_DATETIME_S,0       },  //6字节, YYMMDDhhmmss 发生时刻
    {{0xFFFFFFFE},  0, 0x03300A01,4,4,0xFFFF,  E_NULL, 0                  },  //操作者代码
    {{0x41132200},  0, 0x03300A01,1,1,0xFFFF,  E_UNSIGNED},  //1字节, 有功组合方式编程前的有功组合方式特征字
    {{0x20220200},  0, 0x03300A00,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0},  //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
#ifdef AREA_HUNAN
const MapOopTo645_NEW MapOopTo645_Unit_41[]=//断路器保护动作事件
{
    {{0x26060200},  0, 0x038E0001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 故障原因
    {{0x26090200},  0, 0x038E0001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 故障相别
    {{0x201E0200},  0, 0x038E0001,6,6,0xFFFF,  E_DATETIME_S,0           },  //6字节, YYMMDDhhmmss 跳闸发生时刻
    {{0x20002201}, -1, 0x038E0001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 跳闸前A相电压
    {{0x20002202}, -1, 0x038E0001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 跳闸前B相电压
    {{0x20002203}, -1, 0x038E0001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 跳闸前C相电压
    {{0x20012201}, -3, 0x038E0001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 跳闸前A相电流
    {{0x20012202}, -3, 0x038E0001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 跳闸前B相电流
    {{0x20012203}, -3, 0x038E0001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 跳闸前C相电流
    {{0xFFFFFFFE},  0, 0x038E0001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 跳闸前三相电流方向
    {{0x20220200},  0, 0x03810000,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0 },  //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_42[]=//断路器闸位变化事件
{
    {{0x28000200},  0, 0x03910001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 闸位变化状态
    {{0x26080200},  0, 0x03910001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 变位原因
    {{0x26090200},  0, 0x03910001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 故障相别
    {{0x201E0200},  0, 0x03910001,6,6,0xFFFF,  E_DATETIME_S,0           },  //6字节, YYMMDDhhmmss 跳闸发生时刻
    {{0x20002201}, -1, 0x03910001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 变化前A相电压
    {{0x20002202}, -1, 0x03910001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 变化前B相电压
    {{0x20002203}, -1, 0x03910001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 变化前C相电压
    {{0x20012201}, -3, 0x03910001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 变化前A相电流
    {{0x20012202}, -3, 0x03910001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 变化前B相电流
    {{0x20012203}, -3, 0x03910001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 变化前C相电流
    {{0xFFFFFFFE},  0, 0x03910001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 变化前三相电流方向
    {{0x20220200},  0, 0x03810204,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0 },  //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
//bit7-bit4 动作 Bit3-bit2 上一次投入使用的定值 bit1-bit0 当前要投入使用的定值
const MapOopTo645_NEW MapOopTo645_Unit_43[]=//断路器保护功能投退事件
{
    {{0x201E0200},  0, 0x038F0001,6,6,0xFFFF,  E_DATETIME_S,0           },  //6字节, YYMMDDhhmmss 发生时刻
    {{0x33420206},  0, 0x038F0001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 动作原因
    {{0x33420209},  0, 0x038F0001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 投退方式
    //需要代理特殊处理 两个都是取的动作原因里面的几位
    {{0x33420207},  0, 0x038F0001,1,1,0xFFFF,  E_DATA, 0                 }, //上一次投入使用的定值
    {{0x33420208},  0, 0x038F0001,1,1,0xFFFF,  E_DATA, 0                 },//当前要投入使用的定值
    {{0x20220200},  0, 0x03810202,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0 },  //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
const MapOopTo645_NEW MapOopTo645_Unit_44[]=//断路器告警事件
{
    {{0x26040200},  0, 0x03920001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 告警标志
    {{0x26070200},  0, 0x03920001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 告警原因
    {{0x26090200},  0, 0x03920001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 告警相别
    {{0x201E0200},  0, 0x03920001,6,6,0xFFFF,  E_DATETIME_S,0           },  //6字节, YYMMDDhhmmss 发生/恢复时刻
    {{0x20002201}, -1, 0x03920001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 告警前A相电压
    {{0x20002202}, -1, 0x03920001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 告警前B相电压
    {{0x20002203}, -1, 0x03920001,2,2,0xFFFF,  E_OOP_WORD3V_T,  -1      }, // 2字节, XXX.X 告警前C相电压
    {{0x20012201}, -3, 0x03920001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 告警前A相电流
    {{0x20012202}, -3, 0x03920001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 告警前B相电流
    {{0x20012203}, -3, 0x03920001,3,3,0xFFFF,  E_OOP_INT3V_T,   -1      }, // 3字节, XXXXX.X, 告警前C相电流
    {{0xFFFFFFFE},  0, 0x03920001,1,1,0xFFFF,  E_BIN, 0                 },  //1字节, 三相电流方向
    {{0x20220200},  0, 0x03810201,3,3,0xFFFF,  E_DOUBLE_LONG_UNSIGNED,0 },  //事件序号
    {{0xFFFFFFFF},0xFF,0xFFFFFFFF,0xFF,0xFF,0xFFFF,0xFF,0xFF            },
};
#endif
const EventMapOopTo645_t MapOopTo645_Event[]=
{
    //{{0x30000600},         0x10000002,    DI_SINGL,   MapOopTo645_Unit_1_0 },//总-电能表失压事件
    {{0x30000700},         0x1001FF01,    DI_SINGL,   MapOopTo645_Unit_1_1 },//A相-电能表失压事件
    {{0x30000800},         0x1002FF01,    DI_SINGL,   MapOopTo645_Unit_1_2},//B相-电能表失压事件
    {{0x30000900},         0x1003FF01,    DI_SINGL,   MapOopTo645_Unit_1_3},//C相-电能表失压事件
    {{0x30010700},         0x1101FF01,    DI_SINGL,   MapOopTo645_Unit_1_4},//A相-电能表欠压事件
    {{0x30010800},         0x1102FF01,    DI_SINGL,   MapOopTo645_Unit_1_5},//B相-电能表欠压事件
    {{0x30010900},         0x1103FF01,    DI_SINGL,   MapOopTo645_Unit_1_6},//C相-电能表欠压事件
    {{0x30020700},         0x1201FF01,    DI_SINGL,   MapOopTo645_Unit_1_7},//A相-电能表过压事件
    {{0x30020800},         0x1202FF01,    DI_SINGL,   MapOopTo645_Unit_1_8},//B相-电能表过压事件
    {{0x30020900},         0x1203FF01,    DI_SINGL,   MapOopTo645_Unit_1_9},//C相-电能表过压事件
    {{0x30030700},         0x1301FF01,    DI_SINGL,   MapOopTo645_Unit_1_10},//A相-电能表断相事件
    {{0x30030800},         0x1302FF01,    DI_SINGL,   MapOopTo645_Unit_1_11},//B相-电能表断相事件
    {{0x30030900},         0x1303FF01,    DI_SINGL,   MapOopTo645_Unit_1_12},//C相-电能表断相事件
    {{0x30040700},         0x1801FF01,    DI_SINGL,   MapOopTo645_Unit_5_1},//A相-电能表失流事件
    {{0x30040800},         0x1802FF01,    DI_SINGL,   MapOopTo645_Unit_5_2},//B相-电能表失流事件
    {{0x30040900},         0x1803FF01,    DI_SINGL,   MapOopTo645_Unit_5_3},//C相-电能表失流事件
    {{0x30050700},         0x1901FF01,    DI_SINGL,   MapOopTo645_Unit_5_4},//A相-电能表过流事件
    {{0x30050800},         0x1902FF01,    DI_SINGL,   MapOopTo645_Unit_5_5},//B相-电能表过流事件
    {{0x30050900},         0x1903FF01,    DI_SINGL,   MapOopTo645_Unit_5_6},//C相-电能表过流事件
    {{0x30060700},         0x1A01FF01,    DI_SINGL,   MapOopTo645_Unit_5_7},//A相-电能表断流事件
    {{0x30060800},         0x1A02FF01,    DI_SINGL,   MapOopTo645_Unit_5_8},//B相-电能表断流事件
    {{0x30060900},         0x1A03FF01,    DI_SINGL,   MapOopTo645_Unit_5_9},//C相-电能表断流事件
    {{0x30070700},         0x1B01FF01,    DI_SINGL,   MapOopTo645_Unit_2_1},//A相-电能表潮流反向事件
    {{0x30070800},         0x1B02FF01,    DI_SINGL,   MapOopTo645_Unit_2_2},//B相-电能表潮流反向事件
    {{0x30070900},         0x1B03FF01,    DI_SINGL,   MapOopTo645_Unit_2_3},//C相-电能表潮流反向事件
    {{0x30080700},         0x1C01FF01,    DI_SINGL,   MapOopTo645_Unit_2_4},//A相-电能表过载事件
    {{0x30080800},         0x1C02FF01,    DI_SINGL,   MapOopTo645_Unit_2_5},//B相-电能表过载事件
    {{0x30080900},         0x1C03FF01,    DI_SINGL,   MapOopTo645_Unit_2_6},//C相-电能表过载事件
    {{0x300B0600},         0x03120301,    DI_SINGL,   MapOopTo645_Unit_12},//I象限电能表无功需量超限事?
    {{0x300B0700},         0x03120401,    DI_SINGL,   MapOopTo645_Unit_13},//II象限电能表无功需量超限事
    {{0x300B0800},         0x03120501,    DI_SINGL,   MapOopTo645_Unit_14},//III象限电能表无功需量超限?
    {{0x300B0900},         0x03120601,    DI_SINGL,   MapOopTo645_Unit_15},//IV象限电能表无功需量超限事
    {{0x30090200},         0x03120101,    DI_SINGL,   MapOopTo645_Unit_10},//电能表正向有功需量超限事件
    {{0x300A0200},         0x03120201,    DI_SINGL,   MapOopTo645_Unit_11},//电能表反向有功需量超限事件
    {{0x300C0200},         0x1F000101,    DI_SINGL,   MapOopTo645_Unit_7 },//电能表功率因数超下限事件
    {{0x300D0200},         0x03050001,    DI_BLOCK,   MapOopTo645_Unit_8 },//电能表全失压事件
    {{0x300E0200},         0x03060001,    DI_BLOCK,   MapOopTo645_Unit_9 },//电能表辅助电源掉电事件
    {{0x300F0200},         0x1400FF01,    DI_SINGL,   MapOopTo645_Unit_2_7},//电能表电压逆相序事件
    {{0x30100200},         0x1500FF01,    DI_SINGL,   MapOopTo645_Unit_2_8},//电能表电流逆相序事件
    {{0x30110200},         0x03110001,    DI_BLOCK,   MapOopTo645_Unit_38},//电能表掉电事件
    {{0x30120200},         0x03300001,    DI_BLOCK,   MapOopTo645_Unit_16},//电能表编程事件
    {{0x30130200},         0x03300101,    DI_BLOCK,   MapOopTo645_Unit_17},//电能表清零事件
    {{0x30140200},         0x03300201,    DI_BLOCK,   MapOopTo645_Unit_19},//电能表需量清零事件
    {{0x30150200},         0x03300301,    DI_BLOCK,   MapOopTo645_Unit_20},//电能表事件清零事件
    {{0x30160200},         0x03300401,    DI_BLOCK,   MapOopTo645_Unit_21},//电能表校时事件
    {{0x30180200},         0x03300601,    DI_BLOCK,   MapOopTo645_Unit_23},//电能表时区表编程事件
    {{0x30190200},         0x03300701,    DI_BLOCK,   MapOopTo645_Unit_24},//电能表周休日编程事件
    {{0x301A0200},         0x03300C01,    DI_BLOCK,   MapOopTo645_Unit_25},//电能表结算日编程事件
    {{0x301B0200},         0x03300D01,    DI_BLOCK,   MapOopTo645_Unit_18},//电能表开盖事件
    {{0x301C0200},         0x03300E01,    DI_BLOCK,   MapOopTo645_Unit_39},//电能表开端钮盒事件
    {{0x301D0200},         0x1600FF01,    DI_SINGL,   MapOopTo645_Unit_3 },//电能表电压不平衡事件
    {{0x301E0200},         0x1700FF01,    DI_SINGL,   MapOopTo645_Unit_4 },//电能表电流不平衡事件
    {{0x301F0200},         0x1D00FF01,    DI_SINGL,   MapOopTo645_Unit_6_1},//电能表跳闸事件
    {{0x30200200},         0x1E00FF01,    DI_SINGL,   MapOopTo645_Unit_6_2},//电能表合闸事件
    {{0x30220200},         0x03300901,    DI_BLOCK,   MapOopTo645_Unit_27},//电能表有功组合方式编程事件
    {{0x30230200},         0x03300A01,    DI_BLOCK,   MapOopTo645_Unit_40},//电能表无功组合方式编程事件
    {{0x30250200},         0x03301001,    DI_BLOCK,   MapOopTo645_Unit_29},//电能表阶梯表编程事件
    {{0x30260200},         0x03301201,    DI_BLOCK,   MapOopTo645_Unit_30},//电能表密钥更新事件
    {{0x30270200},         0x03301301,    DI_BLOCK,   MapOopTo645_Unit_31},//电能表异常插卡事件
    //{{0x30280200},         0x0333FF01,    DI_SINGL,   MapOopTo645_Unit_32},//电能表购电记录
    //{{0x30290200},         0x03340001,    DI_BLOCK,   MapOopTo645_Unit_33},//电能表退费记录
    {{0x302A0200},         0x03350001,    DI_BLOCK,   MapOopTo645_Unit_34},//电能表恒定磁场干扰事件
    {{0x302B0200},         0x03360001,    DI_BLOCK,   MapOopTo645_Unit_35},//电能表负荷开关误动作事件
    {{0x302C0200},         0x03370001,    DI_BLOCK,   MapOopTo645_Unit_36},//电能表电源异常事件
    {{0x302D0200},         0x2000FF01,    DI_SINGL,   MapOopTo645_Unit_37},//电能表电流严重不平衡事件
#ifdef AREA_HUNAN
    {{0x35030200},         0x038E0001,    DI_BLOCK,   MapOopTo645_Unit_41},//断路器保护动作事件
    {{0x35040200},         0x03910001,    DI_BLOCK,   MapOopTo645_Unit_42},//断路器闸位变化事件
    {{0x35050200},         0x038F0001,    DI_BLOCK,   MapOopTo645_Unit_43},//断路器保护功能投退事件
    {{0x35060200},         0x03920001,    DI_BLOCK,   MapOopTo645_Unit_44},//断路器告警事件
#endif
    {{0x31BB0200},         0x038E0001,    DI_SINGL,   MapOopTo645_Unit_45},//漏保跳闸事件
    {{0xFFFFFFFF},         0xFFFFFFFF,    DI_SINGL,   NULL},//
};
const EventRcordNo  EventNo[] =
{
    //class7
    {{0x30090200},        {0x30090702}},        //电能表正向有功需量超限事件
    {{0x300A0200},        {0x300A0702}},        //电能表反向有功需量超限事件
    {{0x300C0200},        {0x300C0702}},        //电能表功率因数超下限事件
    {{0x300D0200},        {0x300D0702}},        //电能表全失压事件
    {{0x300E0200},        {0x300E0702}},        //电能表辅助电源掉电事件

    {{0x300F0200},        {0x300F0702}},        //电能表电压逆相序事件
    {{0x30100200},        {0x30100702}},        //电能表电流逆相序事件
    {{0x30110200},        {0x30110702}},        //电能表掉电事件
    {{0x30120200},        {0x30120702}},        //电能表编程事件
    {{0x30130200},        {0x30130702}},        //电能表清零事件

    {{0x30140200},        {0x30140702}},        //电能表需量清零事件
    {{0x30150200},        {0x30150702}},        //电能表事件清零事件
    {{0x30160200},        {0x30160702}},        //电能表校时事件
    {{0x30170200},        {0x30170702}},        //电能表时段表编程事件
    {{0x30180200},        {0x30180702}},        //电能表时区表编程事件

    {{0x30190200},        {0x30190702}},        //电能表周休日编程事件
    {{0x301A0200},        {0x301A0702}},        //电能表结算日编程事件
    {{0x301B0200},        {0x301B0702}},        //电能表开盖事件
    {{0x301C0200},        {0x301C0702}},        //电能表开端钮盒事件
    {{0x301D0200},        {0x301D0702}},        //电能表电压不平衡事件

    {{0x301E0200},        {0x301E0702}},        //电能表电流不平衡事件
    {{0x301F0200},        {0x301F0702}},        //电能表跳闸事件
    {{0x30200200},        {0x30200702}},        //电能表合闸事件
    {{0x30210200},        {0x30210702}},        //电能表节假日编程事件
    {{0x30220200},        {0x30220702}},        //电能表有功组合方式编程事件

    {{0x30230200},        {0x30230702}},        //电能表无功组合方式编程事件
    {{0x30240200},        {0x30240702}},        //电能表费率参数表编程事件
    {{0x30250200},        {0x30250702}},        //电能表阶梯表编程事件
    {{0x30260200},        {0x30260702}},        //电能表密钥更新事件
    {{0x30270200},        {0x30270702}},        //电能表异常插卡事件

    {{0x30280200},        {0x30280702}},        //电能表购电记录
    {{0x30290200},        {0x30290702}},        //电能表退费记录
    {{0x302A0200},        {0x302A0702}},        //电能表恒定磁场干扰事件
    {{0x302B0200},        {0x302B0702}},        //电能表负荷开关误动作事件
    {{0x302C0200},        {0x302C0702}},        //电能表电源异常事件

    {{0x302D0200},        {0x302D0702}},        //电能表电流严重不平衡事件
    {{0x302E0200},        {0x302E0702}},        //电能表时钟故障事件
    {{0x302F0200},        {0x302F0702}},        //电能表计量芯片故障事件

    //class24
    {{0x30000600},        {0x30000A00}},        //总 -电能表失压事件
    {{0x30000700},        {0x30000A01}},        //A相-电能表失压事件
    {{0x30000800},        {0x30000A02}},        //B相-电能表失压事件
    {{0x30000900},        {0x30000A03}},        //C相-电能表失压事件
                          
    {{0x30010600},        {0x30010A00}},        //总 -电能表欠压事件
    {{0x30010700},        {0x30010A01}},        //A相-电能表欠压事件
    {{0x30010800},        {0x30010A02}},        //B相-电能表欠压事件
    {{0x30010900},        {0x30010A03}},        //C相-电能表欠压事件
                          
    {{0x30020600},        {0x30020A00}},        //总 -电能表过压事件
    {{0x30020700},        {0x30020A01}},        //A相-电能表过压事件
    {{0x30020800},        {0x30020A02}},        //B相-电能表过压事件
    {{0x30020900},        {0x30020A03}},        //C相-电能表过压事件
                          
    {{0x30030600},        {0x30030A00}},        //总 -电能表断相事件
    {{0x30030700},        {0x30030A01}},        //A相-电能表断相事件
    {{0x30030800},        {0x30030A02}},        //B相-电能表断相事件
    {{0x30030900},        {0x30030A03}},        //C相-电能表断相事件
                          
    {{0x30040600},        {0x30040A00}},        //总 -电能表失流事件
    {{0x30040700},        {0x30040A01}},        //A相-电能表失流事件
    {{0x30040800},        {0x30040A02}},        //B相-电能表失流事件
    {{0x30040900},        {0x30040A03}},        //C相-电能表失流事件
                          
    {{0x30050600},        {0x30050A00}},        //总 -电能表过流事件
    {{0x30050700},        {0x30050A01}},        //A相-电能表过流事件
    {{0x30050800},        {0x30050A02}},        //B相-电能表过流事件
    {{0x30050900},        {0x30050A03}},        //C相-电能表过流事件
                          
    {{0x30060600},        {0x30060A00}},        //总 -电能表断流事件
    {{0x30060700},        {0x30060A01}},        //A相-电能表断流事件
    {{0x30060800},        {0x30060A02}},        //B相-电能表断流事件
    {{0x30060900},        {0x30060A03}},        //C相-电能表断流事件
                          
    {{0x30070600},        {0x30070A00}},        //总 -电能表潮流反向事件
    {{0x30070700},        {0x30070A01}},        //A相-电能表潮流反向事件
    {{0x30070800},        {0x30070A02}},        //B相-电能表潮流反向事件
    {{0x30070900},        {0x30070A03}},        //C相-电能表潮流反向事件
                          
    {{0x30080600},        {0x30080A00}},        //总 -电能表过载事件
    {{0x30080700},        {0x30080A01}},        //A相-电能表过载事件
    {{0x30080800},        {0x30080A02}},        //B相-电能表过载事件
    {{0x30080900},        {0x30080A03}},        //C相-电能表过载事件
                          
    {{0x300B0600},        {0x300B0A00}},        //I象限电能表无功需量超限事件
    {{0x300B0700},        {0x300B0A01}},        //II象限电能表无功需量超限事件
    {{0x300B0800},        {0x300B0A02}},        //III象限电能表无功需量超限事件
    {{0x300B0900},        {0x300B0A03}},        //IV象限电能表无功需量超限事件
                          
    {{0xFFFFFFFF},        {0xFFFFFFFF}},
};

//电表上报事件对应表
const EventID_To_DI EventIDToDI[] = {
    //负荷开关误动或拒动
    {0,   {0x302B0200},   0x03360000,   1,
    {0x03360001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开表盖次数
    {10,  {0x301B0200},    0x03300D00,  1,
    {0x03300D01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //开端钮盖次数
    {11,  {0x301C0200},    0x03300E00,  1,
    {0x03300E01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //磁场次数
    {12,  {0x302A0200},    0x03350000,  1,
    {0x03350001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电源异常
    {13,  {0x302C0200},    0x03370000,  1,
    {0x03370001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //跳闸成功
    {14,  {0x301F0200},    0x1D000001,  1,
    {0x1D000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //合闸成功
    {15,  {0x30200200},    0x1E000001,  1,
    {0x1E000101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失压
    {16,  {0x30000700},    0x10010001,  2,
    {0x10010101, 0x10012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相欠压
    {17,  {0x30010700},    0x11010001,  2,
    {0x11010101, 0x11012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过压
    {18,  {0x30020700},    0x12010001,  2,
    {0x12010101, 0x12012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相失流
    {19,  {0x30040700},    0x18010001,  2,
    {0x18010101, 0x18012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过流
    {20,  {0x30050700},    0x19010001,  2,
    {0x19010101, 0x19012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相过载
    {21,  {0x30080700},    0x1C010001,  2,
    {0x1C010101, 0x1C011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相功率反向
    {22,  {0x30070700},    0x1B010001,  2,
    {0x1B010101, 0x1B011201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断相
    {23,  {0x30030700},    0x13010001,  2,
    {0x13010101, 0x13012501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //A相断流
    {24,  {0x30060700},    0x1A010001,  2,
    {0x1A010101, 0x1A012101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#ifdef AREA_HUNAN
    //量测开关分闸
    {25,  {0x35040200},    0x03810204,  1,
    {0x03910001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
    
    //量测开关合闸
    {26,  {0x35040200},    0x03810204,  1,
    {0x03910001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#endif
    //B相失压
    {32,  {0x30000800},    0x10020001,  2,
    {0x10020101, 0x10022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相欠压
    {33,  {0x30010800},    0x11020001,  2,
    {0x11020101, 0x11022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过压
    {34,  {0x30020800},    0x12020001,  2,
    {0x12020101, 0x12022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相失流
    {35,  {0x30040800},    0x18020001,  2,
    {0x18020101, 0x18022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过流
    {36,  {0x30050800},    0x19020001,  2,
    {0x19020101, 0x19022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相过载
    {37,  {0x30080800},    0x1C020001,  2,
    {0x1C020101, 0x1C021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相功率反向
    {38,  {0x30070800},    0x1B020001,  2,
    {0x1B020101, 0x1B021201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断相
    {39,  {0x30030800},    0x13020001,  2,
    {0x13020101, 0x13022501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //B相断流
    {40,  {0x30060800},    0x1A020001,  2,
    {0x1A020101, 0x1A022101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#ifdef AREA_HUNAN

    //表箱内温度越上限
    {45,  {0x33610200},    0x03810207,  1,
    {0x03950001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //表箱内温度越下限
    {46,  {0x33610200},    0x03810207,  1,
    {0x03950001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#endif
    //C相失压
    {48,  {0x30000900},    0x10030001,  2,
    {0x10030101, 0x10032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相欠压
    {49,  {0x30010900},    0x11030001,  2,
    {0x11030101, 0x11032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过压
    {50,  {0x30020900},    0x12030001,  2,
    {0x12030101, 0x12032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相失流
    {51,  {0x30040900},    0x18030001,  2,
    {0x18030101, 0x18032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过流
    {52,  {0x30050900},    0x19030001,  2,
    {0x19030101, 0x19032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相过载
    {53,  {0x30080900},    0x1C030001,  2,
    {0x1C030101, 0x1C031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相功率反向
    {54,  {0x30070900},    0x1B030001,  2,
    {0x1B030101, 0x1B031201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断相
    {55,  {0x30030900},    0x13030001,  2,
    {0x13030101, 0x13032501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //C相断流
    {56,  {0x30060900},    0x1A030001,  2,
    {0x1A030101, 0x1A032101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压逆相序
    {64,  {0x300F0200},    0x14000001,  2,
    {0x14000101, 0x14001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流逆序
    {65,  {0x30100200},    0x15000001,  2,
    {0x15000101, 0x15001201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电压不平衡
    {66,  {0x301D0200},    0x16000001,  2,
    {0x16000101, 0x16001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流不平衡
    {67,  {0x301E0200},    0x17000001,  2,
    {0x17000101, 0x17001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //辅助电源失电
    {68,  {0x300E0200},    0x03060000,  1,
    {0x03060001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //掉电
    {69,  {0x30110200},    0x03110000,  1,
    {0x03110001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量超限 07中一个事件 698多个事件
    {70,  {0x30090200},    0x03120000,  6,
    {0x03120101, 0x03120201, 0x03120301, 0x03120401, 0x03120501, 0x03120601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //总功率因数超下限
    {71,  {0x300C0200},    0x1F000001,  2,
    {0x1F000101, 0x1F000601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电流严重不平衡
    {72,  {0x302D0200},    0x20000001,  2,
    {0x20000101, 0x20001301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //潮流反向
    {73,  {0xffffffff},    0x21000000,  1,
    {0x21000001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //全失压
    {74,  {0x300D0200},    0x03050000,  1,
    {0x03050001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#ifdef AREA_HUNAN

    //保护动作事件
    {76,  {0x35030200},    0x03810206,  1,
    {0x038E0001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#endif
#ifdef AREA_ZHEJIANG
    //拓扑识别结果
    {77,  {0x3E030200},    0x09080500,  1,
    {0x09080501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#endif
#ifdef AREA_JIANGXI
    //拓扑识别结果
    //{77,  {0x3E030200},    0x09080600,  1,
    //{0x09080601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},
#endif
    //编程次数
    {80,  {0x30120200},    0x03300000,  1,
    {0x03300001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //电表清零
    {81,  {0x30130200},    0x03300100,  1,
    {0x03300101, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //需量清零次数（数据项太长了，不抄读事件记录了）
    {82,  {0x30140200},    0x03300200,  1,
    {0x03300201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //事件清零
    {83,  {0x30150200},    0x03300300,  1,
    {0x03300301, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //校时
    {84,  {0x30160200},    0x03300400,  1,
    {0x03300401, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //时段表编程
    {85,  {0x30170200},    0x03300500,  1,
    {0x03300501, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //时区表编程
    {86,  {0x30180200},    0x03300600,  1,
    {0x03300601, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //周休日编程
    {87,  {0x30190200},    0x03300700,  1,
    {0x03300701, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //节假日编程
    {88,  {0xffffffff},    0x03300800,  1,
    {0x03300801, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //有功组合方式编程
    {89,  {0x30220200},    0x03300900,  1,
    {0x03300901, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式1编程
    {90,  {0x30230200},    0x03300A00,  1,
    {0x03300A01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //无功组合方式2编程
    {91,  {0x30230200},    0x03300B00,  1,
    {0x03300B01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //结算日编程
    {92,  {0x301A0200},    0x03300C00,  1,
    {0x03300C01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //费率参数表编程
    {93,  {0x30240200},    0x03300F00,  1,
    {0x03300F01, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //阶梯表编程
    {94,  {0x30250200},    0x03301000,  1,
    {0x03301001, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    //密钥更新
    {95,  {0x30260200},    0x03301200,  1,
    {0x03301201, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

    {0xff,  {0xffffffff},  0xffffffff,  0xff,
    {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}},

};

const MapOopTo645_NEW MapOopTo188_RT[]= //实时数据映射表
{
    {{0x25000200},  0,  0xFFFFFFFF,4,4,0x901F,E_DOUBLE_LONG_UNSIGNED,   0, },   // (当前)组合有功电能块
    {{0x25010200}, -2,  0xFFFFFFFF,4,4, 0x901F,E_DOUBLE_LONG_UNSIGNED,  -2, },   // 4字节, XXXXXX.XX(当前)组合有功总电能
    {{0x25020200}, -2,  0xFFFFFFFF,4,4, 0x901F,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 4字节, XXXXXX.XX(当前)费率1组合有功电能
    {{0x25030200}, -2,  0xFFFFFFFF,4,4, 0x901F,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 4字节, XXXXXX.XX(当前)费率2组合有功电能
    {{0x25040200}, -2,  0xFFFFFFFF,3,3, 0x901F,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 4字节, XXXXXX.XX(当前)费率3组合有功电能
    {{0x25050200}, -2,  0xFFFFFFFF,6,6, 0x901F,E_Water_Temperature_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率4组合有功电能 
    {{0x25050201}, -2,  0xFFFFFFFF,3,3, 0x901F,E_Water_Temperature_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率4组合有功电能  
    {{0x25050202}, -2,  0xFFFFFFFF,3,3, 0x901F,E_Water_Temperature_T,  -2, },  // 4字节, XXXXXX.XX(当前)费率4组合有功电能  
    // (当前)正向有功总电能
    {{0x25060200}, -2,  0xFFFFFFFF,2,2,0x901F,E_OOP_ST_T, -2, },    // (当前)正向有功电能块                                                                                                                                                                                                              }
    {{0x25060201}, -2,  0xFFFFFFFF,1,1,0x901F,E_OOP_ST_T, -2, },    // (当前)正向有功电能块
    {{0x25060202}, -2,  0xFFFFFFFF,1,1,0x901F,E_OOP_ST_T, -2, },    // (当前)正向有功电能块  
    {{0x20210200},  0,  0x40000200,7,7,0xffff,E_DATETIME_S, -2, },    //冻结时标
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,   0xFF,0xFF,0xFFFF,0xFF,0xFF      },
};

const MapOopTo645_NEW MapOopTo188_RT_New[]= //实时数据映射表
{
    {{0x25000200},  0,  0x0001FF00,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,   0, },   // 
    {{0x25010200}, -2,  0x0001FF00,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },   // 
    {{0x25020200}, -2,  0x0001FF00,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25030200}, -2,  0x0001FF00,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25040200}, -2,  0x0001FF00,3,3, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25050200}, -2,  0x0001FF00,6,6, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    {{0x25050201}, -2,  0x0001FF00,3,3, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    {{0x25050202}, -2,  0x0001FF00,3,3, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    // (当前)正向有功总电能
    {{0x25060200}, -2,  0x0001FF00,2,2,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块                                                                                                                                                                                                              }
    {{0x25060201}, -2,  0x0001FF00,1,1,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块
    {{0x25060202}, -2,  0x0001FF00,1,1,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块  
    {{0x20210200},  0,  0x40000200,7,7,0xffff,E_DATETIME_S, -2, },    //冻结时标
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,   0xFF,0xFF,0xFFFF,0xFF,0xFF      },
};

const MapOopTo645_NEW MapOopTo188_DAY[]= //实时数据映射表
{
    {{0x25000200},  0,  0x05060101,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,   0, },   // 
    {{0x25010200}, -2,  0x05060101,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },   // 
    {{0x25020200}, -2,  0x05060101,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25030200}, -2,  0x05060101,4,4, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25040200}, -2,  0x05060101,3,3, 0xFFFF,E_DOUBLE_LONG_UNSIGNED,  -2, },  // 
    {{0x25050200}, -2,  0x05060101,6,6, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    {{0x25050201}, -2,  0x05060101,3,3, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    {{0x25050202}, -2,  0x05060101,3,3, 0xFFFF,E_Water_Temperature_T,  -2, },  //
    // (当前)正向有功总电能
    {{0x25060200}, -2,  0x05060101,2,2,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块                                                                                                                                                                                                              }
    {{0x25060201}, -2,  0x05060101,1,1,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块
    {{0x25060202}, -2,  0x05060101,1,1,0xFFFF,E_OOP_ST_T, -2, },    // (当前)正向有功电能块  
    {{0x20210200},  0,  0x05060101,3,3,0xffff,E_DATETIME_S, -2, },    //冻结时标
    {{0xFFFFFFFF},0xFF, 0xFFFFFFFF,   0xFF,0xFF,0xFFFF,0xFF,0xFF      },
};


const EVENT_REPORT_STATE_TO_OAD_T gEventReportOad[] = 
{
    //普通校时
    {16,{0x30160200}},
    //广播校时
    {17,{0x303c0200}},
    //开表盖
    {18,{0x301b0200}},
    //开端钮盖
    {19,{0x301c0200}},
    //管理模组插拔
    {20,{0x30310200}},
    //管理模组升级
    {21,{0x30360200}},
};

const uint8 gEventReportOadNum = sizeof(gEventReportOad)/sizeof(EVENT_REPORT_STATE_TO_OAD_T);
/*******************************************************************************
* 函数名称: taskmng_2015_event_oad_get
* 函数功能:将上报的20150200状态字按位转换为事件oad
* 输入参数:reportBuf 上报的32位bitstring 大端00 00 20 00 代表bit18开表盖

* 输出参数: oadlist 转换出来的事件oad列表
* 返 回 值: oad个数
*******************************************************************************/
uint8 taskmng_2015_event_oad_get(uint8 *reportBuf,OOP_OAD_U *oadlist)
{
    uint8 i,j;
    uint8 data;
    uint8 len;
    uint8 num = 0;
    
    for(i=0;i<32;i++)
    {
        data = reportBuf[i/8];
        len = 7 - i%8;
        data = (data>>len)&0x01;
        if(data == 1)
        {
            for(j=0;j<gEventReportOadNum;j++)
            {
                if(i == gEventReportOad[j].bitIndex)
                {
                    oadlist[num++] = gEventReportOad[j].eventOad;
                    break;
                }
            }
        }
    }
    return num;
}


BOOL GetEventRecordNo(OOP_OAD_U Mainoad,OOP_OAD_U *eventnooad)
{
    const EventRcordNo *EventNolist;
    EventNolist=EventNo;
    
    while(EventNolist->mainOad.value!=0xFFFFFFFF)
    {

        if(Mainoad.value==EventNolist->mainOad.value)
        {
            *eventnooad=EventNolist->EventNoOad;
            return TRUE;
        }
    
        EventNolist++;
    }
    return FALSE;
}
const MapOopTo645_NEW* taskmng_map_get_from_mainoad(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,uint16 *MapNum)
{
    const MapOopTo645_NEW *pMap = NULL;
    uint8 PrtlMode = 0;
    uint16 Pn;
    uint8 logicid;
    OOP_OAD_U MainOAD;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
    taskmng_mtr_prtl_get(Pn,logicid,&PrtlMode);
    memcpy((uint8*)&MainOAD,(uint8*)&Tmp_AmrLink->CsdTask.RoadTask[0].MainOad,sizeof(OOP_OAD_U));
#ifdef AREA_SHANDONG
    uint8 CSDIndex ;
    OOP_OAD_U suboad ;
#endif
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_NULL || PrtlMode == PRTL_97)
    {
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_RT;
        return pMap;
    }
    //湖南日冻结时标不对时用实时数据转存
    if(MainOAD.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0xff && PrtlMode == PRTL_07)
    {
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_RT;
        return pMap;
    }
#ifdef AREA_HUNAN
    //湖南的抄读模块曲线 数据标识用实时的
    if(taskmng_HPLC_module_curve_flag_get()==1 && MainOAD.nObjID == 0x5002&&
        pAmrThreadArg->pMtrAmrInfo[MsIndex].stacurvestate ==1)
    {
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_RT;
        return pMap;
    }

#endif

    switch(MainOAD.nObjID)
    {
    case 0x0000:  //实时数据
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_RT;
        break;

    case 0x5000:  //瞬时冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_Instant)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_Instant;
        break;

    case 0x5002:  //分钟冻结(负荷曲线)
        {
        TASK_FMT_TRACE(Pipe, REC_LOG, "flag645 %d  \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 );
    #ifdef AREA_SHANDONG
        
        CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
        suboad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        TASK_FMT_TRACE(Pipe, REC_LOG, "5002 csdindex %d suboad %08x \n",CSDIndex,suboad);
        //如果第一个是冻结时标 就再看下一个
        if(suboad.value == 0x20210200)
        {
            CSDIndex++;
            suboad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
            TASK_FMT_TRACE(Pipe, REC_LOG, "5002 下一个 csdindex %d suboad %08x \n",CSDIndex,suboad);
        }
        //三相表抄负荷曲线时是块抄 解析的时候不能按实时来 会出问题
        if(taskmng_mtr_conntype_get(Pn,logicid)==1)
        {
            if(taskmng_energy_oad_check(suboad) == FALSE)
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
                pMap = MapOopTo645_RT;
                break;
            }
        }
    #endif
        //先判断是否是单相表
        if(taskmng_mtr_conntype_get(Pn,logicid)==1)
        {
            //判断flag
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_MinOne)/sizeof(MapOopTo645_NEW);
                pMap = MapOopTo645_MinOne;
            }else
            {
                //负荷曲线时标不对则按照实时抄读 
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 1 ||taskmng_task_period_check(Tmp_AmrLink)==TRUE)
                {
                    if(MapNum != NULL)
                        *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
                    pMap = MapOopTo645_RT;
                }else
                {
                    if(MapNum != NULL)
                        *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
                    pMap = MapOopTo645_Min;
                }
            }

        }else
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
                pMap =MapOopTo645_Min;
            }else
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
                pMap = MapOopTo645_RT;
            }
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE||taskmng_task_period_get(Tmp_AmrLink->TaskId,logicid) < 86400L)
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
//                pMap = MapOopTo645_RT;
//            }else
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
//                pMap =MapOopTo645_Min;
//            }
        }
        }
        break;

    case 0x5003:  //小时冻结
        if(taskmng_task_period_get(Tmp_AmrLink->TaskId,logicid) < 86400L)
        {
            if(MapNum != NULL)
                *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
            pMap = MapOopTo645_RT;
        }
        else      //周期大于1天仍通过负荷曲线抄读
        {
            if(MapNum != NULL)
                *MapNum = sizeof(MapOopTo645_Hour)/sizeof(MapOopTo645_NEW);
            pMap = MapOopTo645_Hour;
        }
        break;

    case 0x5004:  //日冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_DAY)/sizeof(MapOopTo645_NEW);
        pMap =MapOopTo645_DAY;
        break;

    case 0x5005:  //结算日
    case 0x5006:  //月冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_MON)/sizeof(MapOopTo645_NEW);
        pMap = MapOopTo645_MON;
        break;

    default:
        return NULL;
    }
    return pMap;

}

const EventMapOopTo645_t *taskmng_event_map_get(OOP_OAD_U mainOad)
{
    const EventMapOopTo645_t *pEventMap = MapOopTo645_Event; 
    while(pEventMap->Oad.value!=0xFFFFFFFF)
    {
        if(mainOad.value == pEventMap->Oad.value)
        {
            
            return pEventMap ;
        }
        pEventMap++;
    }
    return NULL;
}

/****************************************************************************
*模块编号：
*名称：Find645InfoPtrByOad
*功能：根据OAD查找645映射表
*输入参数：主OAD，从OAD
*输出参数：645映射表指针，OAD索引
*处理：
*返回：无
****************************************************************************/
const MapOopTo645_NEW* Find645InfoPtrByOad(OOP_OAD_U mainOad, OOP_OAD_U subOad)
{
    const MapOopTo645_NEW *pMapOopTo645 = NULL;

    if (mainOad.nObjID == 0x0000 && mainOad.attID == 0x00 && mainOad.nIndex == 0x00)//实时数据
    {
        pMapOopTo645 = MapOopTo645_RT;
    }
    else if (mainOad.nObjID == 0x5004 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//日冻结数据
    {
        pMapOopTo645 = MapOopTo645_DAY;
    }
    else if (mainOad.nObjID == 0x5002 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//分钟曲线
    {
        pMapOopTo645 = MapOopTo645_Min;
    }
    else if ((mainOad.nObjID == 0x5005 || mainOad.nObjID == 0x5006) && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//月结算数据
    {
        pMapOopTo645 = MapOopTo645_MON;//MapOopTo645_RT;
    }
    else if (mainOad.nObjID == 0x5003 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//分钟曲线
    {
        pMapOopTo645 = MapOopTo645_Hour;
    }
    else if (mainOad.nObjID == 0x5000 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//分钟曲线
    {
        pMapOopTo645 = MapOopTo645_Instant;
    }
    else if ((mainOad.nObjID&0xF000) == 0x3000)
    {
        //根据OAD查找
        const EventMapOopTo645_t *pEventMap = MapOopTo645_Event; 
        while(pEventMap->Oad.value!=0xFFFFFFFF)
        {
            if(mainOad.value == pEventMap->Oad.value)
            {
                pMapOopTo645 = pEventMap->EventMap;
                break;
            }
            pEventMap++;
        }
        if(pEventMap->Oad.value==0xFFFFFFFF)
            return NULL;
    }
    else
        return NULL;
    if(pMapOopTo645==NULL)
        return NULL;

    if(mainOad.nObjID == 0x0000&&mainOad.attID==0x00&&mainOad.nIndex==0x00)//实时数据
    {
        if(subOad.value==0x20210200)
            return NULL;
    }
    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(subOad.value == pMapOopTo645->Oad.value) 
        {
            return pMapOopTo645;
        }
        pMapOopTo645++;
    }
    return NULL;
}


/****************************************************************************
*模块编号：
*名称：Find645InfoPtrByOad
*功能：根据OAD查找645映射表
*输入参数：主OAD，从OAD
*输出参数：645映射表指针，OAD索引
*处理：
*返回：无
****************************************************************************/
const MapOopTo645_NEW* Find188InfoPtrByOad(OOP_OAD_U mainOad, OOP_OAD_U subOad)
{
    const MapOopTo645_NEW *pMapOopTo645 = NULL;

    if (mainOad.nObjID == 0x0000 && mainOad.attID == 0x00 && mainOad.nIndex == 0x00)//实时数据
    {
        pMapOopTo645 = MapOopTo188_RT;
    }
    else if ((mainOad.nObjID&0xF000) == 0x5000)
    {
        pMapOopTo645 = MapOopTo188_RT;
    }
    else
        return NULL;
    if(pMapOopTo645==NULL)
        return NULL;

//    if(mainOad.nObjID == 0x0000&&mainOad.attID==0x00&&mainOad.nIndex==0x00)//实时数据
//    {
//        if(subOad.value==0x20210200)
//            return NULL;
//    }
    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(subOad.value == pMapOopTo645->Oad.value) 
        {
            return pMapOopTo645;
        }
        pMapOopTo645++;
    }
    return NULL;
}



/****************************************************************************
*模块编号：
*名称：Find645InfoPtrByOad
*功能：根据OAD查找645映射表
*输入参数：主OAD，从OAD
*输出参数：645映射表指针，OAD索引
*处理：
*返回：无
****************************************************************************/
const MapOopTo645_NEW* Find188InfoPtrByOadNew(OOP_OAD_U mainOad, OOP_OAD_U subOad)
{
    const MapOopTo645_NEW *pMapOopTo645 = NULL;

    if (mainOad.nObjID == 0x0000 && mainOad.attID == 0x00 && mainOad.nIndex == 0x00)//实时数据
    {
        pMapOopTo645 = MapOopTo188_RT_New;
    }
    else if ((mainOad.nObjID&0xF000) == 0x5000)
    {
        pMapOopTo645 = MapOopTo188_DAY;
    }
    else
        return NULL;
    if(pMapOopTo645==NULL)
        return NULL;

    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(subOad.value == pMapOopTo645->Oad.value) 
        {
            return pMapOopTo645;
        }
        pMapOopTo645++;
    }
    return NULL;
}

/****************************************************************************
*模块编号：
*名称：
*功能：判断07的的数据标识是否在同一组中
*输入参数：DI
*输出参数：
*处理：
*返回：
    FALSE:不在同一组
    TRUE:
****************************************************************************/

BOOL IsGroupByDI07(uint32 sDI07,uint32 dDI07)
{
    if((sDI07&0xFFFF0000) != (dDI07&0xFFFF0000))
        return FALSE;
    if((uint8)sDI07 != (uint8)dDI07)
        return FALSE;
    if((sDI07&0x0000FF00) == 0x0000FF00  || (dDI07&0x0000FF00) == 0x0000FF00)
        return TRUE;
    else if((sDI07&0x0000FF00) == (dDI07&0x0000FF00))
        return TRUE;
    else
        return FALSE;
}



/****************************************************************************
*模块编号：
*名称：查找事件相应记录序号字典
*功能：
*输入参数：事件主OAD
*输出参数：
*处理：
*返回：无
****************************************************************************/
BOOL FindEvtRecNoIDByOad(OOP_OAD_U mainOad)
{
    int   i = 0;
    const EventMapOopTo645_t *pEventMap = MapOopTo645_Event;

    while(pEventMap->Oad.value!= 0xFFFFFFFF)
    {
        if(cmpOad(mainOad, pEventMap[i].Oad) == TRUE)
        {
            return TRUE;
        }
        i++;
        if(pEventMap[i].Oad.value==0xFFFFFFFF)
        {
            break;
        }
    }

    return FALSE;
}

/****************************************************************************
*模块编号：
*名称：GetDI07_485
*功能：根据面向对象和645-07数据的对应的表中,查找到645-07数据标识
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
uint8 GetDI07_485(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe)
{

    const MapOopTo645_NEW *pMapOopTo645;
    OadInfoMap pOadInfo;

    OOP_OAD_U mainOAD,subOAD;
    uint8  di07Num = 0;
    uint8  i,j,k;
    uint8  oadindex = 0;



    //TASK_FMT_TRACE( pipe, COMM_LOG,"GetDI07_485 csd->MainOad.value is %x\n",csd->MainOad.value);
        
    if(csd->MainOad.nObjID==0x0000)//OAD
    {
        mainOAD.nObjID = 0x0000;
        mainOAD.attID = 0x00;
        mainOAD.nIndex = 0x00;
        memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
        subOAD = csd->OadListInfo[subOadIndex];

    }
    else//ROAD
    {

        if(FALSE==taskmng_oad_info_get(csd->MainOad, &pOadInfo))//有疑问的地方？？？？？？？
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "该主OAD不存在 csd->MainOad is %x\n", csd->MainOad.value);
            return 0;
        }
        #ifdef AREA_HUNAN
        if(csd->MainOad.nObjID == 0x5002)
        {
            if(AcqInfo.Acqtype !=ACQTYP_NULL)
            {
                AcqInfo.Acqtype = ACQTYP_NULL;
                pCbTask->StoreTimeType = STRTMTYP_STARTTM;
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
//                subOadIndex++;
//                RunTaskInfo->SubOadIndex++;
            }
            
        }
        #endif
        #ifdef AREA_SHANDONG
        {
            if((csd->MainOad.nObjID == 0x5002)||(csd->MainOad.nObjID == 0x5003))
            {
                
                if(((taskmng_energy_oad_check(csd->OadListInfo[subOadIndex])==FALSE)&&(csd->OadListInfo[subOadIndex].value !=0x20210200))||(pCbTask->energynum==0x00))
                {
                    AcqInfo.Acqtype = ACQTYP_NULL;
                    pCbTask->StoreTimeType = STRTMTYP_STARTTM;
                    pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
                }
                
            }

        }
        #endif
        if(AcqInfo.Acqtype == ACQTYP_NULL || (csd->MainOad.nObjID == 0x5004 && endNum == 0xff))//日冻结实时转存
        {
            if((csd->MainOad.nObjID& 0xF000) != 0x5000)
            {
                //只有冻结支持通过实时数据转存
                RunTaskInfo->SubOadIndex = csd->SubOadNum;//全部抄完了
            }
            mainOAD.nObjID = 0x0000;
            mainOAD.attID = 0x00;
            mainOAD.nIndex = 0x00;
            memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
            if((csd->MainOad.nObjID == 0x5004 && endNum == 0xff))
            {
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
            }
        }
        else
        {
            mainOAD = csd->MainOad;
        }
       
        switch(pOadInfo.class)
        {
        case 7:
        case 9:     
        case 24:
            if(RunTaskInfo->SubOadIndex>=csd->SubOadNum)
                return 0;
            subOAD = csd->OadListInfo[subOadIndex];
            break;
        default:
            return 0;
        }
        }
    if(FALSE==taskmng_oad_info_get(csd->OadListInfo[subOadIndex], &pOadInfo))
    {
           TASK_FMT_TRACE( pipe, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[subOadIndex].value); 
            return 0;
    }

    if((mainOAD.nObjID&0xF000)==0x3000)
    {

        //根据OAD查找
        const EventMapOopTo645_t *pEventMap = MapOopTo645_Event;
        if(FALSE==taskmng_oad_info_get(mainOAD, &pOadInfo))
        {
            return 0;
        }

        while(pEventMap->Oad.value!=0xFFFFFFFF)
        {

            if(mainOAD.value == pEventMap->Oad.value)
            {

                pMapOopTo645 = pEventMap->EventMap;
                if(DI07Buff!=NULL)
                {
                    //先抄事件序号
                    //遍历ROAD是否需要抄读序号
                    for(oadindex = 0; oadindex < csd->SubOadNum; oadindex++)
                    {
                        if(csd->OadListInfo[oadindex].nObjID == 0x2022)
                        {
                            while(pMapOopTo645->Oad.value != 0xFFFFFFFF)
                            {
                                if(0x20220200 == pMapOopTo645->Oad.value)
                                {
                                    DI07Buff[di07Num++] = (pMapOopTo645->DI07);//事件发生序号
                                    TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读事件发生序号 di07Num is %d,DI07Buff[0] is %x\n", di07Num,DI07Buff[0]);
                                    break;
                                }
                                pMapOopTo645++;
                            }
                            break;
                        }
                    }

                    //抄事件具体内容
                    DI07Buff[di07Num++] = pEventMap->DI07;
                    TASK_FMT_TRACE( pipe, COMM_LOG, "要抄事件具体内容 di07Num is %d,DI07Buff[0] is %x\n", di07Num,DI07Buff[1]);
                }
                break;
            }
            pEventMap++;
        }
        TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读的事件数据项个数di07Num is %d\n", di07Num);
        return di07Num;
    }
    else
    {

        pMapOopTo645= Find645InfoPtrByOad(mainOAD,subOAD);//this return OAD_ID is "OAD_FREZZE_TM"

        if(pMapOopTo645==NULL)
            return 0;
        while(pMapOopTo645->Oad.value==subOAD.value)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "数据项 is %x ,%x\n", subOAD.value,pMapOopTo645->DI07);
            if(DI07Buff!=NULL)
                DI07Buff[di07Num]= pMapOopTo645->DI07;

            di07Num++;
            pMapOopTo645++;
        }

        for(i=0; i<di07Num; i++) //除去数组中重复的07数据标识
        {
            for(j=i+1; j<di07Num; j++)
            {
                if(DI07Buff[i] == DI07Buff[j])
                {
                    for(k=j+1; k<di07Num; k++)
                        DI07Buff[k-1] = DI07Buff[k];
                    di07Num--;
                    j--;
                }
            }
        }
    TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读的数据项个数di07Num is %d\n", di07Num);

        return di07Num;
    }
}


/****************************************************************************
*模块编号：
*名称：GetDI07_485
*功能：根据面向对象和645-07数据的对应的表中,查找到645-07数据标识
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
uint8 GetDI188(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe)
{

    const MapOopTo645_NEW *pMapOopTo645;
    OadInfoMap pOadInfo;

    OOP_OAD_U mainOAD,subOAD;
    uint8  di07Num = 0;
    uint8  i,j,k;
    //uint8  oadindex = 0;



    //TASK_FMT_TRACE( pipe, COMM_LOG,"GetDI07_485 csd->MainOad.value is %x\n",csd->MainOad.value);
        
    if(csd->MainOad.nObjID==0x0000)//OAD
    {
        mainOAD.nObjID = 0x0000;
        mainOAD.attID = 0x00;
        mainOAD.nIndex = 0x00;
        memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
        subOAD = csd->OadListInfo[subOadIndex];

    }
    else//ROAD
    {

        if(FALSE==taskmng_oad_info_get(csd->MainOad, &pOadInfo))//有疑问的地方？？？？？？？
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "该主OAD不存在 csd->MainOad is %x\n", csd->MainOad.value);
            return 0;
        }
        //#ifdef AREA_HUNAN
        if((csd->MainOad.nObjID&0xF000) == 0x5000)
        {
            if(AcqInfo.Acqtype !=ACQTYP_NULL)
            {
                AcqInfo.Acqtype = ACQTYP_NULL;
                pCbTask->StoreTimeType = STRTMTYP_STARTTM;
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
//                subOadIndex++;
//                RunTaskInfo->SubOadIndex++;
            }
            
        }
        //#endif
        if(AcqInfo.Acqtype == ACQTYP_NULL || (csd->MainOad.nObjID == 0x5004 && endNum == 0xff))//日冻结实时转存
        {
            if((csd->MainOad.nObjID& 0xF000) != 0x5000)
            {
                //只有冻结支持通过实时数据转存
                RunTaskInfo->SubOadIndex = csd->SubOadNum;//全部抄完了
            }
            mainOAD.nObjID = 0x0000;
            mainOAD.attID = 0x00;
            mainOAD.nIndex = 0x00;
            memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
            if((csd->MainOad.nObjID == 0x5004 && endNum == 0xff))
            {
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
            }
        }
        else
        {
            mainOAD = csd->MainOad;
        }
       
        switch(pOadInfo.class)
        {
        case 7:
        case 9:     
        case 24:
            if(RunTaskInfo->SubOadIndex>=csd->SubOadNum)
                return 0;
            subOAD = csd->OadListInfo[subOadIndex];
            break;
        default:
            return 0;
        }
        }
    if(FALSE==taskmng_oad_info_get(csd->OadListInfo[subOadIndex], &pOadInfo))
    {
           TASK_FMT_TRACE( pipe, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[subOadIndex].value); 
            return 0;
    }

    if((mainOAD.nObjID&0xF000)==0x3000)
    {

        return 0;
    }
    else
    {

        pMapOopTo645= Find188InfoPtrByOad(mainOAD,subOAD);//this return OAD_ID is "OAD_FREZZE_TM"

        if(pMapOopTo645==NULL)
            return 0;
        while(pMapOopTo645->Oad.value==subOAD.value)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "数据项 is %x ,%x\n", subOAD.value,pMapOopTo645->DI07);
            if(DI07Buff!=NULL)
                DI07Buff[di07Num]= pMapOopTo645->DI97;

            di07Num++;
            pMapOopTo645++;
        }

        for(i=0; i<di07Num; i++) //除去数组中重复的07数据标识
        {
            for(j=i+1; j<di07Num; j++)
            {
                if(DI07Buff[i] == DI07Buff[j])
                {
                    for(k=j+1; k<di07Num; k++)
                        DI07Buff[k-1] = DI07Buff[k];
                    di07Num--;
                    //j--;
                }
            }
        }
        TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读的数据项个数di07Num is %d\n", di07Num);

        return di07Num;
    }
}



/****************************************************************************
*模块编号：
*名称：GetDI07_485
*功能：根据面向对象和645-07数据的对应的表中,查找到645-07数据标识
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
uint8 GetDI188New(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe)
{

    const MapOopTo645_NEW *pMapOopTo645;
    OadInfoMap pOadInfo;

    OOP_OAD_U mainOAD,subOAD;
    uint8  di07Num = 0;
    uint8  i,j,k;
    //uint8  oadindex = 0;



    //TASK_FMT_TRACE( pipe, COMM_LOG,"GetDI07_485 csd->MainOad.value is %x\n",csd->MainOad.value);
        
    if(csd->MainOad.nObjID==0x0000)//OAD
    {
        mainOAD.nObjID = 0x0000;
        mainOAD.attID = 0x00;
        mainOAD.nIndex = 0x00;
        memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
        subOAD = csd->OadListInfo[subOadIndex];

    }
    else//ROAD
    {

        if(FALSE==taskmng_oad_info_get(csd->MainOad, &pOadInfo))//有疑问的地方？？？？？？？
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "该主OAD不存在 csd->MainOad is %x\n", csd->MainOad.value);
            return 0;
        }
        if(AcqInfo.Acqtype == ACQTYP_NULL || (csd->MainOad.nObjID == 0x5004 && endNum == 0xff))//日冻结实时转存
        {
            if((csd->MainOad.nObjID& 0xF000) != 0x5000)
            {
                //只有冻结支持通过实时数据转存
                RunTaskInfo->SubOadIndex = csd->SubOadNum;//全部抄完了
            }
            mainOAD.nObjID = 0x0000;
            mainOAD.attID = 0x00;
            mainOAD.nIndex = 0x00;
            memset((uint8*)&mainOAD,0x00,sizeof(OOP_OAD_U));
            if((csd->MainOad.nObjID == 0x5004 && endNum == 0xff))
            {
                pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
            }
        }
        else
        {
            mainOAD = csd->MainOad;
        }
       
        switch(pOadInfo.class)
        {
        case 7:
        case 9:     
        case 24:
            if(RunTaskInfo->SubOadIndex>=csd->SubOadNum)
                return 0;
            subOAD = csd->OadListInfo[subOadIndex];
            break;
        default:
            return 0;
        }
        }
    if(FALSE==taskmng_oad_info_get(csd->OadListInfo[subOadIndex], &pOadInfo))
    {
           TASK_FMT_TRACE( pipe, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[subOadIndex].value); 
            return 0;
    }

    if((mainOAD.nObjID&0xF000)==0x3000)
    {

        return 0;
    }
    else
    {

        pMapOopTo645= Find188InfoPtrByOadNew(mainOAD,subOAD);//this return OAD_ID is "OAD_FREZZE_TM"

        if(pMapOopTo645==NULL)
            return 0;
        while(pMapOopTo645->Oad.value==subOAD.value)
        {
            TASK_FMT_TRACE( pipe, COMM_LOG, "数据项 is %x ,%x\n", subOAD.value,pMapOopTo645->DI07);
            if(DI07Buff!=NULL)
                DI07Buff[di07Num]= pMapOopTo645->DI07;

            di07Num++;
            pMapOopTo645++;
        }

        for(i=0; i<di07Num; i++) //除去数组中重复的07数据标识
        {
            for(j=i+1; j<di07Num; j++)
            {
                if(DI07Buff[i] == DI07Buff[j])
                {
                    for(k=j+1; k<di07Num; k++)
                        DI07Buff[k-1] = DI07Buff[k];
                    di07Num--;
                    //j--;
                }
            }
        }
        TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读的数据项个数di07Num is %d\n", di07Num);

        return di07Num;
    }
}

/****************************************************************************
*模块编号：
*名称：GetDI07_485
*功能：根据面向对象和645-07数据的对应的表中,查找到645-07数据标识
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
uint8 GetCapDI(CSD_TASK_INFO  *csd,  uint8 mapindex)
{

    uint8  i,j=0;
    uint8 Idnum=0;
    uint8 len=0;
    const MapOopToCap  *mapinfo=NULL;

   for(i=0;i<csd->SubOadNum;i++)
   {
        j=0;
        switch(mapindex)
        {
            case 1:
            {
                mapinfo = MapOopToCap_RT_01;
                len=sizeof(MapOopToCap_RT_01)/sizeof(OadInfoMap);
            }
            break;
            case 2:
            {
                mapinfo = MapOopToCap_RT_02;
                len=sizeof(MapOopToCap_RT_02)/sizeof(OadInfoMap);
            }
            break;
            case 3:
            {
                TASK_FMT_TRACE( 2, COMM_LOG, "mapindex  is %d\n",mapindex);
                mapinfo = MapOopToCap_RT_03;
                len=sizeof(MapOopToCap_RT_03)/sizeof(OadInfoMap);
            }
            break;
            default:

            break;

        }    
        while(j<len)
           {
               TASK_FMT_TRACE( 2, COMM_LOG, "csd->OadListInfo[i].value  is %x,mapinfo->Oad.value is %x,i is %d,j is %d\n",csd->OadListInfo[i].value,mapinfo->Oad.value,i,j);
            if(csd->OadListInfo[i].value == mapinfo->Oad.value)
            {
                Idnum++;
            }
            j++;
            mapinfo++;
           }    
   }

    return Idnum;
  
}

///****************************************************************************
//*模块编号：
//*名称：GetDICap
//*功能：根据面向对象和电容器数据的对应的表中,查找到645-07数据标识
//*输入参数：
//*输出参数：
//*处理：
//*返回：无
//****************************************************************************/
//uint8 GetDICap(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe)
//{
//
//    const MapOopTo645_NEW *pMapOopTo645;
//    OadInfoMap pOadInfo;
//
//    OOP_OAD_U mainOAD,subOAD;
//    uint8  di07Num = 0;
//    uint8  i,j,k;
//    uint8  oadindex = 0;
//
//    TASK_FMT_TRACE( pipe, COMM_LOG,"GetDI07_485 csd->MainOad.value is %x\n",csd->MainOad.value);
//        
//    if(csd->MainOad.nObjID==0x0000)//OAD
//    {
//        mainOAD.nObjID = 0x0000;
//        mainOAD.attID = 0x00;
//        mainOAD.nIndex = 0x00;
//        subOAD = csd->OadListInfo[subOadIndex];
//
//    }
//    else//ROAD
//    {
//
//        if(FALSE==taskmng_oad_info_get(csd->MainOad, &pOadInfo))//有疑问的地方？？？？？？？
//        {
//            TASK_FMT_TRACE( pipe, COMM_LOG, "该主OAD不存在 csd->MainOad is %x\n", csd->MainOad.value);
//            return 0;
//        }
//
//        if(AcqInfo.Acqtype == ACQTYP_NULL || (csd->MainOad.nObjID == 0x5004 && endNum == 0xff))//日冻结实时转存
//        {
//            if((csd->MainOad.nObjID& 0xF000) != 0x5000)
//            {
//                //只有冻结支持通过实时数据转存
//                RunTaskInfo->SubOadIndex = csd->SubOadNum;//全部抄完了
//            }
//            mainOAD.nObjID = 0x0000;
//            mainOAD.attID = 0x00;
//            mainOAD.nIndex = 0x00;
//        }
//        else
//        {
//            mainOAD = csd->MainOad;
//        }
//        TASK_FMT_TRACE( pipe, COMM_LOG,"GetDI07_485 pOadInfo.class is %d,RunTaskInfo->SubOadIndex is %d\n,csd->SubOadNum is %d\n",pOadInfo.class,RunTaskInfo->SubOadIndex,csd->SubOadNum);
//        switch(pOadInfo.class)
//        {
//        case 7:
//        case 9:     
//        case 24:
//            if(RunTaskInfo->SubOadIndex>=csd->SubOadNum)
//                return 0;
//            subOAD = csd->OadListInfo[subOadIndex];
//            break;
//        default:
//            return 0;
//        }
//        }
//    if(FALSE==taskmng_oad_info_get(csd->OadListInfo[subOadIndex], &pOadInfo))
//    {
//           TASK_FMT_TRACE( pipe, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[subOadIndex].value); 
//            return 0;
//    }
//
//    if((mainOAD.nObjID&0xF000)==0x3000)
//    {
//        return 0;
//    }
//    else
//    {
//
//        pMapOopTo645= Find645InfoPtrByOad(mainOAD,subOAD);//this return OAD_ID is "OAD_FREZZE_TM"
//
//        if(pMapOopTo645==NULL)
//            return 0;
//        while(pMapOopTo645->Oad.value==subOAD.value)
//        {
//            if(DI07Buff!=NULL)
//                DI07Buff[di07Num]= pMapOopTo645->DI07;
//
//            di07Num++;
//            pMapOopTo645++;
//        }
//
//        for(i=0; i<di07Num; i++) //除去数组中重复的07数据标识
//        {
//            for(j=i+1; j<di07Num; j++)
//            {
//                if(DI07Buff[i] == DI07Buff[j])
//                {
//                    for(k=j+1; k<di07Num; k++)
//                        DI07Buff[k-1] = DI07Buff[k];
//                    di07Num--;
//                    j--;
//                }
//            }
//        }
//    TASK_FMT_TRACE( pipe, COMM_LOG, "要抄读的数据项个数di07Num is %d\n", di07Num);
//
//        return di07Num;
//    }
//}


/****************************************************************************
*模块编号：
*名称：
*功能：根据OAD计算 OAD的数据在数据块中的偏移，供07表负荷曲线，事件记录使用
*输入参数：主OAD，从OAD
*输出参数：数据在数据块中的偏移
*处理：
*返回：无
****************************************************************************/
uint16 CaclBiasByOad(OOP_OAD_U mainOad,OOP_OAD_U subOad)
{
    OadInfoMap pOadInfo;
    uint16 offSet=0;
    const MapOopTo645_NEW *pMapOopTo645=NULL;
   // uint8 oadTag=subOad.Tag;
   // subOad.Tag= 0;
    if(FALSE==taskmng_oad_info_get(subOad,&pOadInfo))
    {
        return 0xFFFF;
    }
  
    if(mainOad.nObjID == 0x5002&&mainOad.attID==0x02&&mainOad.nIndex==0x00)//分钟曲线
    {
        pMapOopTo645 = MapOopTo645_Min;
    }
    else if((mainOad.nObjID&0xF000)==0x3000)
    {
//        根据OAD查找
        const EventMapOopTo645_t *pEventMap = MapOopTo645_Event;
        if(FALSE==taskmng_oad_info_get(subOad,&pOadInfo))
        {
            return 0xFFFF;
        }
        while(pEventMap->Oad.value!=0xFFFFFFFF)
        {
            if(mainOad.value == pEventMap->Oad.value)
            {
                pMapOopTo645 = pEventMap->EventMap;
                break;
            }
            pEventMap++;
        }
        if(pEventMap->Oad.value==0xFFFFFFFF)
            return 0xFFFF;
        if(FALSE==taskmng_oad_info_get(subOad,&pOadInfo))
        {
            return 0xFFFF;
        }
    }
    else
        return 0xFFFF;
//  printf("subOad.value is%X\n",mainOad.value);
//  printf("subOad.value is%X\n",subOad.value);
//  printf("pMapOopTo645->Oad.value is%X\n",pMapOopTo645->Oad.value);
    while(pMapOopTo645->Oad.value!=0xFFFFFFFF)
    {
        if(subOad.value == pMapOopTo645->Oad.value)
        {
            break;
        }
        else
        {
            if(pMapOopTo645->Oad.value != 0xFFFFFFFF)
            {
                if((pMapOopTo645->Oad.nIndex ==0)&&(Check_Data_Struct(pMapOopTo645->CovertSruct)))
                {
                
                }
                else
                {    
                    offSet += pMapOopTo645->DataLen;
                }
//              printf("1111pMapOopTo645->Oad.value is %X\n",pMapOopTo645->Oad.value);
//              printf("offSet is %d\n",offSet);
            }
//            else
//            {
//                if(OopObjectList[pMapOopTo645->OadId].dataFormat != ARRAY && OopObjectList[pMapOopTo645->OadId].dataFormat!=STRUCTURE)
//                {
//                    offSet += pMapOopTo645->DataLen;
//                }
//            }
        }
        pMapOopTo645++;
    }
    if(pMapOopTo645->Oad.value==0xFFFFFFFF)
    return 0xFFFF;
//  printf("pMapOopTo645->DataLen is%d\n",pMapOopTo645->DataLen);
    return offSet;
}


/****************************************************************************
*模块编号：
*名称：
*功能：根据OAD计算 OAD的数据在数据块中的偏移，供07表负荷曲线，事件记录使用
*输入参数：主OAD，从OAD
*输出参数：数据在数据块中的偏移
*处理：
*返回：无
****************************************************************************/
uint16 CaclCapBiasByOad(OOP_OAD_U subOad,MapOopToCap *tmppMapOopToCap,uint8 mapindex)
{
//    OadInfoMap pOadInfo;
    uint16 offSet=0;
    const MapOopToCap *pMapOopToCap=NULL;
    switch(mapindex)
    {
        case 1:
        pMapOopToCap = MapOopToCap_RT_01;
        break;
        case 2:
        pMapOopToCap = MapOopToCap_RT_02;
        break;
        case 3:
        pMapOopToCap = MapOopToCap_RT_03;
        break;
        default:

        break;

    }
    
    TASK_FMT_TRACE( 2, COMM_LOG,"pMapOopToCap->Oad.value is %x\n",pMapOopToCap->Oad.value);
    while(pMapOopToCap->Oad.value!=0xFFFFFFFF)
    {
        TASK_FMT_TRACE( 2, COMM_LOG,"pMapOopToCap->Oad.value11 is %x\n",pMapOopToCap->Oad.value);
        if(subOad.value == pMapOopToCap->Oad.value)
        {
            break;
        }
        else
        {
            if(pMapOopToCap->Oad.value != 0xFFFFFFFF)
            {
                if((pMapOopToCap->Oad.nIndex ==0)&&(pMapOopToCap->Oad.nObjID<0xBBB0)&&(pMapOopToCap->Oad.nObjID !=0x202F))
                {
                
                }
                else
                {    
                    offSet += pMapOopToCap->DataLen;
                }

        }
        pMapOopToCap++;
    }
        }
    if(pMapOopToCap->Oad.value==0xFFFFFFFF)
    return 0xFFFF;
//  printf("pMapOopTo645->DataLen is%d\n",pMapOopTo645->DataLen);
    memcpy(tmppMapOopToCap,pMapOopToCap,sizeof(MapOopToCap));
   // *tmppMapOopToCap = *pMapOopToCap;
    return offSet;
}


#if(1 == OOP_CRV_ADDREAD_485)
/****************************************************************************
*模块编号：
*名称：
*功能：获取新的曲线补抄起始时间
*输入参数：
        PLC486Flag: 0(485); 1(PLC)
*输出参数：
        新的曲线补抄起始时间
*处理：
*返回：无
****************************************************************************/
time_t GetCrvNewStartTime(time_t OriStartTime, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, uint8 PLC485Flag)
{
        OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
        time_t StartTime = 0;
        time_t NewStartTime = 0;
        OOP_TI_T ti;
        int i = 0;
        int j = 0;
        DateTimeBcd_t tmpTime;
        int ret = 0;
        int AddReadMostDays = 0;
        uint8 Vacant_index = 0;
        char    pfile[100] = {0};
        uint8 data_type=2;//曲线数据
    
        memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime));
    
        sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
        ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
        if(ret < 0)
            memset(OopCrvAddReadTime,0x0,sizeof(OopCrvAddReadTime));
    
        ti.unit = TASK_CYCLE_FLAG_DAY;  //日
        ti.value = 1;
        StartTime = OriStartTime;
        if(0 == PLC485Flag)  //485
        {
            AddReadMostDays = OOP_CRV_ADDREAD_MOST_DAYS_485;
        }
        else if(1 == PLC485Flag)  //PLC
        {
            AddReadMostDays = OOP_CRV_ADDREAD_MOST_DAYS_PLC;
        }
        for(j = 0; j < AddReadMostDays; j++)
        {
            DT_TimeSubInv(&StartTime,ti);
        }
        NewStartTime = OriStartTime;
        Vacant_index = 0xff;
        for(i = 0; i < OOP_CRV_ADDREAD_TASKNUM; i++)
        {
            if(OopCrvAddReadTime[i].TaskId == TaskId)
            {
                DT_Time2DateTimeBCD(OopCrvAddReadTime[i].AddReadTime,&tmpTime);
                printf("TaskId--%d Read OopCrvAddReadTime[%d].AddReadTime--",OopCrvAddReadTime[i].TaskId,i);
                //commfun_printstring((uint8 *)&tmpTime,7);
                //补抄时间小于本轮的开始时间
                if(OopCrvAddReadTime[i].AddReadTime < OriStartTime)
                {
                    //补抄时间大于最大的补抄天数,那么采取记录的时间
                    if(OopCrvAddReadTime[i].AddReadTime >= StartTime)
                    {
                        //printf("AddReadTime >= OriStartTime-Nday:");       //用AddReadTime进行补抄
                    }
                    else
                    {
                        OopCrvAddReadTime[i].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
                        OopCrvAddReadTime[i].SchemeId = SchemeId;
                        write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
                    }
                }
                else if(OopCrvAddReadTime[i].AddReadTime == OriStartTime)
                {
                    //printf("AddReadTime == OriStartTime:");    //无需补抄
                }
                else
                {
                    //printf("AddReadTime > OriStartTime:");
                    OopCrvAddReadTime[i].AddReadTime = OriStartTime;  //无需补抄
                    //OopCrvAddReadTime[i].SchemeId = SchemeId;
                    //WriteMtrXData(MtrNo, RT_DATA, OOP_CRV_ADDREAD_TIME, (char *)&OopCrvAddReadTime);  //针对PLC此情况下不写数据字典
                }
                NewStartTime = OopCrvAddReadTime[i].AddReadTime;
                break;
            }
            if(Vacant_index == 0xff)
            {
                if(OopCrvAddReadTime[i].TaskId == 0)
                {
                    Vacant_index = i;
                }
            }
        }
    
        if(i >= OOP_CRV_ADDREAD_TASKNUM)
        {
            OopCrvAddReadTime[Vacant_index].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
            OopCrvAddReadTime[Vacant_index].SchemeId = SchemeId;
            OopCrvAddReadTime[Vacant_index].TaskId = TaskId;
            write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
            NewStartTime = OopCrvAddReadTime[Vacant_index].AddReadTime;
            i = Vacant_index;
        }
        if(0)
        {
            char            logbuf[1024];
            DateTimeBcd_t   stime_bcd;
    
            memset(logbuf,0x0,sizeof(logbuf));
            DT_Time2DateTimeBCD(NewStartTime,&stime_bcd);
            sprintf(logbuf,"curve MtrNo : %d  Index %d 任务: %d,方案 :%d,Init 曲线点:%02x-%02x-%02x %02x:%02x:%02x",MtrNo,i,OopCrvAddReadTime[i].TaskId,OopCrvAddReadTime[i].SchemeId
                ,stime_bcd.year_l,stime_bcd.month,stime_bcd.day,stime_bcd.hour,stime_bcd.min,
                stime_bcd.sec);
            TASK_FMT_TRACE(-1,"/ramdisk/crver.log", logbuf);     //写入日志文件
        }
        DT_Time2DateTimeBCD(NewStartTime,&tmpTime);
        //printf("TaskId--%d NewAddReadTime--",OopCrvAddReadTime[i].TaskId);
        //commfun_printstring((uint8 *)&tmpTime,7);
        return NewStartTime;
}


/****************************************************************************
*模块编号：
*名称：
*功能：更新曲线补抄起始时间
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
void UpdateCrvNewStartTime(uint16 OI, DateTimeBcd_t *pStoreTm, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, AcqInfo_t *pAcqInfo)
    {
        OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
        time_t AddTime = 0;             //补抄起始时间
        OOP_TI_T Ti;
        DateTimeBcd_t tmpTm;
        uint8 i = 0;
        int ret = 0;
        char    pfile[100] = {0};
        uint8 data_type=2;//曲线数据
    
        commfun_printstring((uint8 *)pStoreTm,7);
        DT_DateTimeBCD2Time(pStoreTm,&AddTime);
    
        memset(&Ti,0,sizeof(Ti));
        if((pAcqInfo->Acqtype == ACQTYP_TI)&&(0 != pAcqInfo->TI.value))  //按时标间隔采集
        {
            printf("+时标间隔\n");
            Ti.unit = pAcqInfo->TI.unit;
            Ti.value = pAcqInfo->TI.value;  //+时标间隔
        }
        else
        {
            printf("+任务频率\n");
            if(0x5002 == OI)
            {
                get_task_run_frequence(TaskId, &Ti);
                if((Ti.unit >= TASK_CYCLE_FLAG_HOUR)
                    ||((Ti.unit == TASK_CYCLE_FLAG_MIN)&&(Ti.value > 15)))
                {
                    printf("+15MIN\n");
                    Ti.unit = TASK_CYCLE_FLAG_MIN;
                    Ti.value = 15;
                }
            }
            else
            {
                Ti.unit = TASK_CYCLE_FLAG_HOUR;
                Ti.value = 1;
            }
        }
        DT_TimeAddInv(&AddTime,Ti);
    
        memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime));
        sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
        ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
        if(ret < 0)
            return;
    
        for(i = 0; i < OOP_CRV_ADDREAD_TASKNUM; i++)
        {
            if(OopCrvAddReadTime[i].TaskId == TaskId)
            {
                OopCrvAddReadTime[i].SchemeId = SchemeId;
                OopCrvAddReadTime[i].AddReadTime = AddTime;
                write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
                DT_Time2DateTimeBCD(OopCrvAddReadTime[i].AddReadTime,&tmpTm);
                printf("TaskId--%d UpdateAddReadTime--",OopCrvAddReadTime[i].TaskId);
                commfun_printstring((uint8 *)&tmpTm,7);
                if(0)
                {
                    char            logbuf[1024];
                    DateTimeBcd_t   stime_bcd;
    
                    memset(logbuf,0x0,sizeof(logbuf));
                    DT_Time2DateTimeBCD(AddTime,&stime_bcd);
                    sprintf(logbuf,"curve MtrNo : %d  Index %d 任务: %d,方案 :%d,更新曲线点:%02x-%02x-%02x %02x:%02x:%02x",MtrNo,i,OopCrvAddReadTime[i].TaskId,OopCrvAddReadTime[i].SchemeId
                        ,stime_bcd.year_l,stime_bcd.month,stime_bcd.day,stime_bcd.hour,stime_bcd.min,
                        stime_bcd.sec);
                    TASK_FMT_TRACE(-1,"/ramdisk/crver.log", logbuf);     //写入日志文件
                }
                break;
            }
       }
        return;
    }

#endif
BOOL JudgeNeedPatch(uint16 TaskId, uint16 MtrNo,uint32 *starttime)
{
	OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
	int ret = 0;
	char    pfile[100] = {0};
	uint8 data_type=2;//曲线数据
	uint8 i=0;
	uint16 tvnum=0;
		
	memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime));
    sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
    ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_CRV_ADDREAD_TASKNUM);
    if(ret < 0)
        return FALSE;

	for(i = 0; i < OOP_CRV_ADDREAD_TASKNUM; i++)
    {
        if(OopCrvAddReadTime[i].TaskId == TaskId)
        {
        	tvnum=taskmng_mtr_meter_patch(sizeof(OopCrvAddReadTime[i].patchresult)/sizeof(OopCrvAddReadTime[i].patchresult[0]),OopCrvAddReadTime[i].patchresult);
			if(0xFFFF !=tvnum)
			{
				*starttime= OopCrvAddReadTime[i].AddReadTime+tvnum*900;
				return TRUE;
			}
			return FALSE;
        }
   }
	return FALSE;
}

/****************************************************************************
*模块编号：
*名称：
*功能：获取新的曲线补抄起始时间
*输入参数：
        PLC486Flag: 0(485); 1(PLC)
*输出参数：
        新的曲线补抄起始时间
*处理：
*返回：无
****************************************************************************/
time_t GetCrvStartTime(time_t OriStartTime, uint16 TaskId, uint16 MtrNo, uint8 PLC485Flag)
{
    OopCrvAddReadTime_t OopCrvAddReadTime[OOP_CRV_ADDREAD_TASKNUM];
    time_t StartTime = 0;
    time_t NewStartTime = 0;
    OOP_TI_T ti;
    int i = 0;
    int j = 0;
    DateTimeBcd_t tmpTime;
    int ret = 0;
    int AddReadMostDays = 0;
    uint8 Vacant_index = 0;
    char    pfile[100] = {0};
    uint8 data_type=2;//曲线数据
    uint8 MtrAddr[6];

    memset(OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime));

    sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
    ret=read_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
    if(ret < 0)
        memset(OopCrvAddReadTime,0x0,sizeof(OopCrvAddReadTime));
	if(ret>=0)
	{
		taskmng_mtr_addr_get(MtrNo,0x00,MtrAddr);
	}
    ti.unit = TASK_CYCLE_FLAG_DAY;  //日
    ti.value = 1;
    StartTime = OriStartTime;
    if(0 == PLC485Flag)  //485
    {
        AddReadMostDays = OOP_CRV_ADDREAD_MOST_DAYS_485;
    }
    else if(1 == PLC485Flag)  //PLC
    {
        AddReadMostDays = OOP_CRV_ADDREAD_MOST_DAYS_PLC;
    }
    for(j = 0; j < AddReadMostDays; j++)
    {
        DT_TimeSubInv(&StartTime,ti);
    }
    NewStartTime = OriStartTime;
    Vacant_index = 0xff;
    for(i = 0; i < OOP_CRV_ADDREAD_TASKNUM; i++)
    {
        if(OopCrvAddReadTime[i].TaskId == TaskId)
        {
            //DT_Time2DateTimeBCD(OopCrvAddReadTime[i].AddReadTime,&tmpTime);
            printf("TaskId--%d Read OopCrvAddReadTime[%d].AddReadTime--",OopCrvAddReadTime[i].TaskId,i);
            //commfun_printstring((uint8 *)&tmpTime,7);
            //补抄时间小于本轮的开始时间
            if(memcmp(MtrAddr,OopCrvAddReadTime[i].addr,6)!=0)
            {
				
				OopCrvAddReadTime[i].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
				//OopCrvAddReadTime[i].SchemeId = SchemeId;
				memcpy(OopCrvAddReadTime[i].addr,MtrAddr,6);
				write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
				NewStartTime = OopCrvAddReadTime[i].AddReadTime;
				break;
            }
			OopCrvAddReadTime[i].AddReadTime = StartTime;
			right_shift(OopCrvAddReadTime[i].patchresult,sizeof(OopCrvAddReadTime[i].patchresult)/sizeof(OopCrvAddReadTime[i].patchresult[0]));
        }
        if(Vacant_index == 0xff)
        {
            if(OopCrvAddReadTime[i].TaskId == 0)
            {
                Vacant_index = i;
            }
        }
    }

    if(i >= OOP_CRV_ADDREAD_TASKNUM)
    {
        OopCrvAddReadTime[Vacant_index].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
        //OopCrvAddReadTime[Vacant_index].SchemeId = SchemeId;
        OopCrvAddReadTime[Vacant_index].TaskId = TaskId;
		memcpy(OopCrvAddReadTime[i].addr,MtrAddr,6);
        write_pdata_xram(pfile,(char *)&OopCrvAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
        NewStartTime = OopCrvAddReadTime[Vacant_index].AddReadTime;
        i = Vacant_index;
    }
    DT_Time2DateTimeBCD(NewStartTime,&tmpTime);

    return NewStartTime;
}

#if(1 == OOP_DAY_ADDREAD_485)

/****************************************************************************
*模块编号：
*名称：
*功能：获取新的日冻结补抄起始时间
*输入参数：
        PLC486Flag: 0(485); 1(PLC)
*输出参数：
        新的日冻结补抄起始时间
*处理：
*返回：无
****************************************************************************/
time_t GetDayNewStartTime(time_t OriStartTime, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, uint8 PLC485Flag)
{
    int    ret;
    OOP_TI_T   ti;
    time_t StartTime = 0;
    time_t NewStartTime = 0;
    int AddReadMostDays = 0;
    int i = 0;
    int j = 0;
    uint8 Vacant_index = 0;
    uint8 data_type=1;//日冻结数据
    DateTimeBcd_t tmpTime;
    OopCrvAddReadTime_t OopDayAddReadTime[OOP_DAY_ADDREAD_TASKNUM];
    char    pfile[100] = {0};

    memset(OopDayAddReadTime,0,sizeof(OopDayAddReadTime));
    sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
    ret=read_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);

    if(ret < 0)
        memset(OopDayAddReadTime,0x0,sizeof(OopDayAddReadTime));

    ti.unit = TASK_CYCLE_FLAG_DAY;  //日
    ti.value = 1;
    StartTime = OriStartTime;
    if(0 == PLC485Flag)  //485
    {
        AddReadMostDays = OOP_DAY_ADDREAD_MOST_DAYS_485;
    }
    else if(1 == PLC485Flag)  //PLC
    {
        AddReadMostDays = OOP_DAY_ADDREAD_MOST_DAYS_PLC;
    }
    for(j = 0; j < AddReadMostDays; j++)
    {
        DT_TimeSubInv(&StartTime,ti);
    }
    NewStartTime = OriStartTime;
//  printf("StartTime is %d\n",StartTime);
//  printf("AddReadMostDays is%d\n",AddReadMostDays);
    Vacant_index = 0xff;

    for(i = 0; i < OOP_DAY_ADDREAD_TASKNUM; i++)
    {
        if(OopDayAddReadTime[i].TaskId == TaskId)
        {
            DT_Time2DateTimeBCD(OopDayAddReadTime[i].AddReadTime,&tmpTime);
            //printf("TaskId--%d Read OopCrvAddReadTime[%d].AddReadTime--",OopCrvAddReadTime[i].TaskId,i);
            //commfun_printstring((uint8 *)&tmpTime,7);
            //补抄时间小于本轮的开始时间
            if(OopDayAddReadTime[i].AddReadTime < OriStartTime)
            {
                //补抄时间大于最大的补抄天数,那么采取记录的时间
                if(OopDayAddReadTime[i].AddReadTime >= StartTime)
                {
                    //printf("AddReadTime >= OriStartTime-Nday:");       //用AddReadTime进行补抄
                }
                else
                {
                    OopDayAddReadTime[i].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
                    OopDayAddReadTime[i].SchemeId = SchemeId;
                    write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
                }
            }
            else if(OopDayAddReadTime[i].AddReadTime == OriStartTime)
            {
                //printf("AddReadTime == OriStartTime:");    //无需补抄
            }
            else
            {
                //printf("AddReadTime > OriStartTime:");
                OopDayAddReadTime[i].AddReadTime = OriStartTime;  //无需补抄
                //OopCrvAddReadTime[i].SchemeId = SchemeId;
                //WriteMtrXData(MtrNo, RT_DATA, OOP_CRV_ADDREAD_TIME, (char *)&OopCrvAddReadTime);  //针对PLC此情况下不写数据字典
            }
            NewStartTime = OopDayAddReadTime[i].AddReadTime;
            break;
        }
        if(Vacant_index == 0xff)
        {
            if(OopDayAddReadTime[i].TaskId == 0)
            {
                Vacant_index = i;
            }
        }
    }

    if(i >= OOP_DAY_ADDREAD_TASKNUM)
    {
        OopDayAddReadTime[Vacant_index].AddReadTime = StartTime; //默认补抄起始时间的上N天内数据
        OopDayAddReadTime[Vacant_index].SchemeId = SchemeId;
        OopDayAddReadTime[Vacant_index].TaskId = TaskId;
        write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);

        NewStartTime = OopDayAddReadTime[Vacant_index].AddReadTime;
        i = Vacant_index;
    }

    if(0)
    {
        char            logbuf[1024];
        DateTimeBcd_t   stime_bcd;
        memset(logbuf,0x0,sizeof(logbuf));
        DT_Time2DateTimeBCD(NewStartTime,&stime_bcd);

        sprintf(logbuf,"day MtrNo : %d  Index %d 任务: %d,方案 :%d,Init 任务日冻结点:%02x-%02x-%02x %02x:%02x:%02x",MtrNo,i,OopDayAddReadTime[i].TaskId,OopDayAddReadTime[i].SchemeId
            ,stime_bcd.year_l,stime_bcd.month,stime_bcd.day,stime_bcd.hour,stime_bcd.min,
            stime_bcd.sec);
        TASK_FMT_TRACE(-1,"/ramdisk/day.log", logbuf);       //写入日志文件
    }
    return NewStartTime;
}

/****************************************************************************
*模块编号：
*名称：
*功能：更新日冻结补抄起始时间
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
void UpdateDayNewStartTime(uint16 OI, DateTimeBcd_t *pStoreTm, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, AcqInfo_t *pAcqInfo)
{
    OopCrvAddReadTime_t OopDayAddReadTime[OOP_DAY_ADDREAD_TASKNUM];
    time_t AddTime = 0;             //补抄起始时间
    OOP_TI_T Ti;
    DateTimeBcd_t tmpTm;
    uint8 i = 0;
    int ret = 0;
    char pfile[100] = {0};
    uint8 data_type=1;//日冻结数据
    
    printf("StoreTm:");
    commfun_printstring((uint8 *)pStoreTm,7);
    DT_DateTimeBCD2Time(pStoreTm,&AddTime);

    memset(&Ti,0,sizeof(Ti));
    if((pAcqInfo->Acqtype == ACQTYP_TI)&&(0 != pAcqInfo->TI.value))  //按时标间隔采集
    {
        printf("+时标间隔\n");
        Ti.unit = pAcqInfo->TI.unit;
        Ti.value = pAcqInfo->TI.value;  //+时标间隔
    }
    else
    {
        Ti.unit = TASK_CYCLE_FLAG_DAY;
        Ti.value = 1;
    }
    DT_TimeAddInv(&AddTime,Ti);

    memset(OopDayAddReadTime,0,sizeof(OopDayAddReadTime));
    sprintf(pfile,"%s/%s_%02d_MtrNo%02d",PrivateDir,PATCH_FILE,data_type,MtrNo);
    ret=read_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
   
    if(ret < 0)
        return;

    for(i = 0; i < OOP_DAY_ADDREAD_TASKNUM; i++)
    {
        if(OopDayAddReadTime[i].TaskId == TaskId)
        {
            OopDayAddReadTime[i].SchemeId = SchemeId;
            OopDayAddReadTime[i].AddReadTime = AddTime;
            write_pdata_xram(pfile,(char *)&OopDayAddReadTime,0,sizeof(OopCrvAddReadTime_t)*OOP_DAY_ADDREAD_TASKNUM);
            DT_Time2DateTimeBCD(OopDayAddReadTime[i].AddReadTime,&tmpTm);
            printf("TaskId--%d UpdateAddReadTime--",OopDayAddReadTime[i].TaskId);
            commfun_printstring((uint8 *)&tmpTm,7);
            if(0)
            {
                char            logbuf[1024];
                DateTimeBcd_t   stime_bcd;

                memset(logbuf,0x0,sizeof(logbuf));
                DT_Time2DateTimeBCD(AddTime,&stime_bcd);
                sprintf(logbuf,"day MtrNo : %d  Index %d 任务: %d,方案 :%d,更新日冻结点:%02x-%02x-%02x %02x:%02x:%02x",MtrNo,i,OopDayAddReadTime[i].TaskId,OopDayAddReadTime[i].SchemeId
                    ,stime_bcd.year_l,stime_bcd.month,stime_bcd.day,stime_bcd.hour,stime_bcd.min,
                    stime_bcd.sec);
                TASK_FMT_TRACE(-1,"/ramdisk/day.log", logbuf);       //写入日志文件
            }
            break;
        }
    }
    return;
}

#endif


/****************************************************************************
*模块编号：
*名称：
*功能：BCD串转BIN
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
uint8 CJT188_ComBcdToBin(uint8 *sBcd,uint8 sLen,uint8 *dBin,uint8 dLen,uint8 withSign)
{
    uint64 desBuff = 0U;
    uint8 i=sLen;
    uint8 sign=0U;//正数
    //printf("STD2007_ComBcdToBin:sLen=%d,dLen=%d,\n",sLen,dLen);

    if((dLen>8U)||(sLen>8U)||(dLen==0U)||(sLen==0U))
    {
        memset(dBin,0x00,(uint32)dLen);
        return FALSE;
    }
    while(i>0U)
    {
        desBuff *= 100U;
        if(((sBcd[i-1U]&0x0FU)>0x09U) || ((sBcd[i-1U]&0xF0U)>0x90U))
        {
            //不是BCD，返回0
            memset(dBin,0x00,(uint32)dLen);
            return FALSE;
        }
        if((i==sLen) && (withSign == TRUE))
        {
            if((sBcd[i-1U]&0x80U)!= 0U)
            {
                sign = 1U;
                sBcd[i-1U] &= 0x7FU;
            }
        }
        desBuff += (uint64)(sBcd[i-1U]&0x0FU) + ((uint64)sBcd[i-1U]>>4)*10U;
        i--;
        //printf("desBuff=%d\n",desBuff);
    }
    if(sign==1U)
    {
        desBuff = ~desBuff + 1U;
    }
    memcpy(dBin,(uint8*)(void*)&desBuff,(uint32)dLen);
    return TRUE;
}

/****************************************************************************
*模块编号：
*名称：Dlt645_ComBcdToBin
*功能：BCD串转BIN
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
BOOL Dlt645_ComBcdToBin(uint8 *sBcd,uint8 sLen,uint8 *dBin,uint8 dLen,BOOL withSign)
{
    uint64 desBuff = 0;
    uint8 i=sLen;
    uint8 sign=0;//正数
 

    if(dLen>8||sLen>8||dLen==0||sLen==0)
    {
        memset(dBin,0x00,dLen);
        return FALSE;
    }

    //单相表B/C相电流回FFFF，先特殊处理一下
    if(CheckFF(sBcd, sLen))
    {
        //memcpy(dBin, sBcd, sLen);
        memset(dBin,0xff,dLen);
        return FALSE;
    }
    while(i>0)
    {
        desBuff *= 100;
        if(i==sLen && withSign == TRUE)
        {
            if(sBcd[i-1]&0x80)
            {
                sign = 1;
                sBcd[i-1] &= 0x7F;
            }

        }
        if((sBcd[i-1]&0x0F)>0x09 || (sBcd[i-1]&0xF0)>0x90)
        {
            //不是BCD，返回0
            memset(dBin,0x00,dLen);
            return FALSE;
        }
        desBuff += (sBcd[i-1]&0x0F) + (sBcd[i-1]>>4)*10;
        i--;
      
    }
    if(sign==1)
        desBuff = ~desBuff + 1;
    memcpy(dBin,(uint8*)&desBuff,dLen);
    return TRUE;
}

/****************************************************************************
*函数名称：Convert_6457_97_To_Struct
*功能：     将97表返回数据转成成对应结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/
void Convert_6457_07_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo)
{
    //uint16 offset1;
    uint16 num;
    uint8 i=0;
    uint8 tmpBuff[30];
    bool  ret=TRUE;
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;

#ifdef AREA_HUNAN
    if(pOadInfo->Oad.value == 0x33420206)
    {
        uint8 data = sourcedata.sourcebuffer[0]>>4;
        *desLen = sizeof(uint8);
        memcpy(desbuffer,&data,sizeof(uint8));
        return;
    }
    if(pOadInfo->Oad.value == 0x33420207||pOadInfo->Oad.value == 0x33420208)
    {
        OOP_VARIANT_T data;
        memset(&data,0x00,sizeof(OOP_VARIANT_T));
        data.type = DT_UNSIGNED;
        if(pOadInfo->Oad.value == 0x33420207)
        {
            data.byVal = (sourcedata.sourcebuffer[0]>>2)&0x03;
        }else
        {
            data.byVal = sourcedata.sourcebuffer[0]&0x03;
        }
        *desLen = sizeof(OOP_VARIANT_T);
        memcpy(desbuffer,&data,sizeof(OOP_VARIANT_T));
        return;
    }
#endif  
    switch (pOadInfo->CovertSruct)
    {
    case E_OOP_ENERGY_T://普通精度电量
    {
        OOP_ENERGY_T Energy1;
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGY_T));
        num=sourcedata.reclen/pOadInfo->DataLen;
        TASK_FMT_TRACE(-1, REC_LOG,"num:%d reclen:%d\n",num, sourcedata.reclen);

        Energy1.nNum=OOP_FULL_RATES;
        //Energy1.nNum= num;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                set_bit_value((uint8*)&Energy1.rsv, 32, i);
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&Energy1.rsv, 32, index);
        }
        *desLen = sizeof(OOP_ENERGY_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGY_T));
    }
    break;

    case E_OOP_ENERGYS_T://普通精度电量
    {
        OOP_ENERGYS_T Energy1;

        num=sourcedata.reclen/pOadInfo->DataLen;
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGYS_T));

        Energy1.nNum=OOP_FULL_RATES;
        //Energy1.nNum= num;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                set_bit_value((uint8*)&Energy1.rsv, 32, i);
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&Energy1.rsv, 32, index);
        }
        *desLen = sizeof(OOP_ENERGYS_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGYS_T));
    }
    break;

    case E_OOP_HENERGY_T://高通精度电量不带符号
    {
        OOP_HENERGY_T HENERGY_T;
 
        num=sourcedata.reclen/pOadInfo->DataLen;
        memset((uint8*)&HENERGY_T,0xff,sizeof(OOP_HENERGY_T));

        HENERGY_T.nNum=OOP_FULL_RATES;
        //HENERGY_T.nNum= num;
        HENERGY_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(uint64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGY_T.nValue[i],tmpBuff,8);    
                set_bit_value((uint8*)&HENERGY_T.rsv, 32, i);
                i++;
            }
        }else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],4,tmpBuff,4,FALSE);
            (*(uint64*)tmpBuff) *= 100;
            memcpy((uint8 *)&HENERGY_T.nValue[index],tmpBuff,8);   
            set_bit_value((uint8*)&HENERGY_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_HENERGY_T);
        memcpy(desbuffer,&HENERGY_T,sizeof(OOP_HENERGY_T));
    }
    break;

    case E_OOP_HENERGYS_T://高通精度电量带符号
    {
        OOP_HENERGYS_T HENERGYS_T;

        num=sourcedata.reclen/pOadInfo->DataLen;
        memset((uint8*)&HENERGYS_T,0xff,sizeof(OOP_HENERGYS_T));

        HENERGYS_T.nNum=OOP_FULL_RATES;
        //HENERGYS_T.nNum= num;
        HENERGYS_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[i],tmpBuff,8);   
                set_bit_value((uint8*)&HENERGYS_T.rsv, 32, i);
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[index],tmpBuff,8); 
                set_bit_value((uint8*)&HENERGYS_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_HENERGYS_T);
        memcpy(desbuffer,&HENERGYS_T,sizeof(OOP_HENERGYS_T));
    }
    break;

    case E_OOP_DEMAND_T:  //需量
    {
        OOP_DEMAND_T DEMAND_T;

        num=sourcedata.reclen/pOadInfo->DataLen;
        memset((uint8*)&DEMAND_T,0xff,sizeof(OOP_DEMAND_T));

        DEMAND_T.nNum=OOP_FULL_RATES;
        //DEMAND_T.nNum= num;
        DEMAND_T.rsv=0;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*8],3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DEMAND_T.demand[i].nValue,tmpBuff,4);  
                set_bit_value((uint8*)&DEMAND_T.rsv, 32, i);
                i++;
            }
            i=0;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+3+i*8,5);
                tmpBuff[0] = 0x20;//年高字节补20
                if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
                {
                    memcpy(tmpTime,(uint8*)&DefaultTime,7);
                }
                memcpy(tmpBuff,tmpTime,7);
                memcpy_r((uint8 *)&DEMAND_T.demand[i].time.year,tmpTime,2);
                memcpy((uint8 *)&DEMAND_T.demand[i].time.month,tmpTime+2,5);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],3,tmpBuff,4,FALSE);
            memcpy((uint8 *)&DEMAND_T.demand[index].nValue,tmpBuff,4);
            memset(tmpBuff,0x00,30);
            memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+3,5);
            tmpBuff[0] = 0x20;//年高字节补20
            if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
            {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
            }
            memcpy(tmpBuff,tmpTime,7);
            memcpy_r((uint8 *)&DEMAND_T.demand[index].time.year,tmpTime,2);
            memcpy((uint8 *)&DEMAND_T.demand[index].time.month,tmpTime+2,5);
            set_bit_value((uint8*)&DEMAND_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_DEMAND_T);
        memcpy(desbuffer,&DEMAND_T,sizeof(OOP_DEMAND_T));
    }

    break;

    case E_OOP_VOLTQR_T://电压合格率
    {
        OOP_VOLTQR_T VOLTQR_T;
        //frame_printf(sourcedata.sourcebuffer,sourcedata.reclen);
        memset(tmpBuff,0x00,30);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+3,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTQR_T.wRate1,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+6,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTQR_T.wRate2,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+9,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime3,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+12,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime4,tmpBuff,4);
        *desLen = sizeof(OOP_VOLTQR_T);
        memcpy(desbuffer,&VOLTQR_T,sizeof(OOP_VOLTQR_T));
    }

    break;

     case E_OOP_VOLTSTAT_T://电压合格率
    {
        OOP_VOLTSTAT_T VOLTSTAT_T;
   
        memset(tmpBuff,0x00,30);
        //frame_printf(sourcedata.sourcebuffer,sourcedata.reclen);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+3,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.wRate1,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+6,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.wRate2,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+9,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime3,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+12,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime4,tmpBuff,4);
        *desLen = sizeof(OOP_VOLTSTAT_T);
        memcpy(desbuffer,&VOLTSTAT_T,sizeof(OOP_VOLTSTAT_T));
    }

    break;

    case E_OOP_DWORD3V_T://分相变量

    {
        OOP_DWORD3V_T DWORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&DWORD3V_T,0xff,sizeof(OOP_DWORD3V_T));

        DWORD3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        DWORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[i],tmpBuff,4);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&DWORD3V_T.rsv, 32, i);
                }
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[index],tmpBuff,4);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&DWORD3V_T.rsv, 32, index);
                }
        }
        *desLen = sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,&DWORD3V_T,sizeof(OOP_DWORD3V_T));
    }

    break;
    case E_OOP_LONG3V_T://温度

    {
        OOP_LONG3V_T LONG3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&LONG3V_T,0xff,sizeof(OOP_LONG3V_T));

        LONG3V_T.nNum=3;
        //DLONG3V_T.nNum=num;
        LONG3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&LONG3V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&LONG3V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&LONG3V_T.nValue[index],tmpBuff,2);
                set_bit_value((uint8*)&LONG3V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_LONG3V_T);
        memcpy(desbuffer,&LONG3V_T,sizeof(OOP_LONG3V_T));
    }

    break;    
    case E_OOP_WORD3V_T://电压

    {
        OOP_WORD3V_T WORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));

        if(sourcedata.pwrType == 1)
        {
            WORD3V_T.nNum=1;
        }
        else
        {
            WORD3V_T.nNum=3;
        }
        //DWORD3V_T.nNum=num;
        WORD3V_T.rsv=0;
        if(num>1)
        {
            if(num == 3)
                WORD3V_T.nNum=3;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[i],tmpBuff,2);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&WORD3V_T.rsv, 32, i);
                }
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[index],tmpBuff,2);
                set_bit_value((uint8*)&WORD3V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,&WORD3V_T,sizeof(OOP_WORD3V_T));
    }

    break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT3V_T,0xff,sizeof(OOP_INT3V_T));

        if(sourcedata.pwrType == 1)
        {
            INT3V_T.nNum=1;
        }
        else
        {
            INT3V_T.nNum=3;
        }

        INT3V_T.rsv=0;
        if(num>1)
        {
            if(num == 3)
                INT3V_T.nNum=3;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3,3,tmpBuff,4,TRUE);
                #if ZHEJIANG_FUN == 1      //浙江功能
                 memcpy((uint8 *)&INT3V_T.nValue[i],tmpBuff,4);
                 INT3V_T.nValue[i] = INT3V_T.nValue[i]*100;
                 TASK_FMT_TRACE(2, COMM_LOG,"INT3V_T.nValue[i] %d\n",INT3V_T.nValue[i]);
                #else
                 memcpy((uint8 *)&INT3V_T.nValue[i],tmpBuff,4);
                #endif
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&INT3V_T.rsv, 32, i);
                }
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT3V_T.nValue[index],tmpBuff,4);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&INT3V_T.rsv, 32, index);
                }
        }
        *desLen = sizeof(OOP_INT3V_T);
        memcpy(desbuffer,&INT3V_T,sizeof(OOP_INT3V_T));
    }
    break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T  INT4V_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT4V_T,0xff,sizeof(OOP_INT4V_T));

        if(sourcedata.pwrType == 1)
        {
            INT4V_T.nNum=2;
        }
        else
        {
            INT4V_T.nNum=4;
        }
        INT4V_T.rsv=0;
        if(num>1)
        {
            if(num == 4)
                INT4V_T.nNum=4;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[i],tmpBuff,4);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&INT4V_T.rsv, 32, i);
                }
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                ret=Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[index],tmpBuff,4);
                if(ret==TRUE)
                {
                    set_bit_value((uint8*)&INT4V_T.rsv, 32, index);
                }
        }
        *desLen = sizeof(OOP_INT4V_T);
        memcpy(desbuffer,&INT4V_T,sizeof(OOP_INT4V_T));
    }
    break;
    case E_FRZ_DATETIME_S://645_07电表的冻结时标
    { 
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        
        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer,5);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
            memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        
    }

        break;
    case E_REAL_DATETIME_S://07电能表日历时间
    {
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00     

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        tmpBuff[1] = *(sourcedata.sourcebuffer+3);
        tmpBuff[2] = *(sourcedata.sourcebuffer+2);
        tmpBuff[3] = *(sourcedata.sourcebuffer+1);
        tmpBuff[4] = *(sourcedata.sourcebuffer+6);
        tmpBuff[5] = *(sourcedata.sourcebuffer+5);
        tmpBuff[6] = *(sourcedata.sourcebuffer+4);
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
    }

        break;
    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
            *desLen = 4;
            memcpy(desbuffer,tmpBuff,4);
        }
    break;
    case E_DATETIME_S://
    { 
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer,6);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,tmpTime,7);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        
    }
    break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T METWORD_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&METWORD_T,0xff,sizeof(OOP_METWORD_T));

        METWORD_T.nNum=OOP_METWORD_NUM;
        //METWORD_T.nNum= num;
        METWORD_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&METWORD_T.metWord[i],tmpBuff,2);
                set_bit_value((uint8*)&METWORD_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&METWORD_T.metWord[index],tmpBuff,2);
                set_bit_value((uint8*)&METWORD_T.rsv, 32, index);
        }
        *desLen=sizeof(OOP_METWORD_T);
        memcpy(desbuffer,(uint8 *)&METWORD_T,sizeof(OOP_METWORD_T));

    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));

        if(sourcedata.pwrType == 1)
        {
            LONG4V_T.nNum=2;
        }
        else
        {
            LONG4V_T.nNum=4;
        }
        //LONG4V_T.nNum= num;
        LONG4V_T.rsv=0;
        if(num>1)
        {
            if(num == 4)
                LONG4V_T.nNum=4;
            while (i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,TRUE);
                memcpy((uint8 *)&LONG4V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&LONG4V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,TRUE);
            memcpy((uint8 *)&LONG4V_T.nValue[index],tmpBuff,2);
            set_bit_value((uint8*)&LONG4V_T.rsv, 32, index);
        }
        *desLen=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
    break;
    case E_LONG_UNSIGNED:
    case E_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
            *desLen = 2;
            memcpy(desbuffer,tmpBuff,2);
        }
    break;
#ifdef AREA_HUNAN
    case E_BIN:
    {
        //Dlt645_ComBcdToBin(sourcedata.sourcebuffer, pOadInfo->DataLen, tmpBuff, 1, FALSE);
        *desLen = 1;
        memcpy(desbuffer, sourcedata.sourcebuffer, 1);
        break;
    }
    case E_SWITCH_STATUS:
    {
        uint8 tmp = (sourcedata.sourcebuffer[0] & 0x60) >> 5;
        *desLen = 1;

        if (tmp == 0)
        {
            desbuffer[0] = 1;
        }
        else if (tmp == 1)
        {
            desbuffer[0] = 2;
        }
        else
        {
            desbuffer[0] = 0xff;
        }

        break;
    }
#endif
    case E_OOP_WALLET_T:
    {
        OOP_DWORD2_T DWORD2_T;

        memset(tmpBuff,0x00,30);
        memset((uint8*)&DWORD2_T,0x00,sizeof(OOP_DWORD2_T));
        
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
        memcpy_r((uint8 *)&DWORD2_T.nValue1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+4,2,tmpBuff,4,FALSE);
        memcpy_r((uint8 *)&DWORD2_T.nValue2,tmpBuff,2);

        *desLen = sizeof(OOP_DWORD2_T);
        memcpy(desbuffer,&DWORD2_T,sizeof(OOP_DWORD2_T));
    }
    break;
    case E_OOP_OCTETVAR16_T:
    {
        OOP_OCTETVAR16_T addr;

        memset((uint8*)&addr,0x00,sizeof(OOP_OCTETVAR16_T));
        addr.nNum =6;
        memcpy_r(addr.value,sourcedata.sourcebuffer,6);
        *desLen = sizeof(OOP_OCTETVAR16_T);
        memcpy(desbuffer,(uint8*)&addr,sizeof(OOP_OCTETVAR16_T));

    }
    break;
    case E_OOP_OCTETVAR64_T:
    {
        OOP_OCTETVAR64_T data;

        memset((uint8*)&data,0x00,sizeof(OOP_OCTETVAR64_T));
        data.nNum =pOadInfo->DataLen;
        memcpy(data.value,sourcedata.sourcebuffer,pOadInfo->DataLen);
        *desLen = sizeof(OOP_OCTETVAR64_T);
        memcpy(desbuffer,(uint8*)&data,sizeof(OOP_OCTETVAR64_T));

    }
    break;
    case E_OOP_PROTECTOR_31BB_T:
    {
        OOP_PROTECTOR_31BB_T protector;
        uint8 offset=0;
        uint8 j=0;
        protector.faultreason = sourcedata.sourcebuffer[offset++];//故障原因

        protector.faultphase =  sourcedata.sourcebuffer[offset++];//故障相别
        TASK_FMT_TRACE ( 2,COMM_LOG, "protector.faultreason is %d,protector.faultphase is %d\n",protector.faultreason,protector.faultphase );
         
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00

        memset((uint8*)&(protector.tmStart.year),0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+offset,6);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&protector.tmStart.year,tmpTime,2);
        memcpy((uint8*)&protector.tmStart.month,tmpTime+2,5);
        offset =offset+6;
        TASK_BUF_TRACE ( 2,COMM_LOG, (uint8*)&protector.tmStart,sizeof(OOP_DATETIME_S_T));
        //剩余电流
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2+offset,2,tmpBuff,2,FALSE);
        memcpy((uint8 *)&protector.residualcurrent,tmpBuff,2);
        offset =offset +2;
        TASK_FMT_TRACE ( 2,COMM_LOG, "protector.residualcurrent is %d\n",protector.residualcurrent);
        //电压
        while(j<3)
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2+offset,2,tmpBuff,2,FALSE);
            memcpy((uint8 *)&protector.voltage[i],tmpBuff,2);
            offset =offset +2;
            j++;
            TASK_FMT_TRACE ( 2,COMM_LOG, "protector.voltage[i] is %d\n",protector.voltage[i] );
        }
        j=0;
        //电流
        while(j<3)
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3+offset,3,tmpBuff,4,FALSE);
            memcpy((uint8 *)&protector.current[i],tmpBuff,4);
            offset =offset +3;
            j++;
            TASK_FMT_TRACE ( 2,COMM_LOG, "protector.current[i] is %d\n",protector.current[i] );
        }
             
        *desLen = sizeof(OOP_PROTECTOR_31BB_T);
        memcpy(desbuffer,(uint8*)&protector,sizeof(OOP_PROTECTOR_31BB_T));

    }
    break;
    default:
        break;
    }

}


/****************************************************************************
*函数名称：Convert_6457_97_To_Struct
*功能：     将97表返回数据转成成对应结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/
void Convert_std188_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo)
{
    //uint16 offset1;
//    uint16 num;
//  //  uint8 i=0;
    uint8 tmpBuff[30];
    uint32 value;
//    bool  ret=TRUE;
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;

    switch (pOadInfo->CovertSruct)
    {
         case E_DATETIME_S://时间
        {
            OOP_DATETIME_S_T DATETIME_S_T;

            memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
            memset(tmpBuff,0x00,30);
            memcpy(tmpBuff,sourcedata.sourcebuffer,7);
            memcpy_r((uint8*)&DATETIME_S_T.year,tmpBuff,2);
            memcpy((uint8*)&DATETIME_S_T.month,tmpBuff+2,5);
            if(DATETIME_S_T.second == 0xff)
                DATETIME_S_T.second =0;
            memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));   
            *desLen=sizeof(OOP_DATETIME_S_T);
        }
        break;

        case E_REAL_DATETIME_S://07电能表日历时间
        {
            OOP_DATETIME_S_T DATETIME_S_T;
            uint8 tmpTime[7];
            DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00     

            memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
            memset(tmpBuff,0x00,30);
            tmpBuff[1] = *(sourcedata.sourcebuffer+3);
            tmpBuff[2] = *(sourcedata.sourcebuffer+2);
            tmpBuff[3] = *(sourcedata.sourcebuffer+1);
            tmpBuff[4] = *(sourcedata.sourcebuffer+6);
            tmpBuff[5] = *(sourcedata.sourcebuffer+5);
            tmpBuff[6] = *(sourcedata.sourcebuffer+4);
            tmpBuff[0] = 0x20;//年高字节补20
            if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
            {
                    memcpy(tmpTime,(uint8*)&DefaultTime,7);
            }
            memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
            memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
            *desLen = sizeof(OOP_DATETIME_S_T);
            memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        }

            break;
        case E_DOUBLE_LONG_UNSIGNED:
        case E_DOUBLE_LONG:
            {
                if((pOadInfo->Oad.nObjID == 0x2500)||(pOadInfo->Oad.nObjID == 0x2501))
                {
                    CJT188_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
                    memcpy((uint8*)&value,tmpBuff,4);
                    value = value*100;
                     *desLen = 4;
                    memcpy(desbuffer,(uint8*)&value,4);
                }
                else
                {
                    CJT188_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
                    *desLen = 4;
                    memcpy(desbuffer,tmpBuff,4);
                }
               
            }
        break;
        case E_LONG_UNSIGNED:
        case E_LONG:
            {
                CJT188_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                *desLen = 2;
                memcpy(desbuffer,tmpBuff,2);
            }
        break;
        case E_Water_Temperature_T:
        {
            Water_Temperature_T WaterTemperature;

            memset(tmpBuff,0x00,30);
            memset((uint8*)&WaterTemperature,0x00,sizeof(WaterTemperature));
            if(pOadInfo->Oad.nIndex==0)
            {
                CJT188_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&WaterTemperature.supplytmp,tmpBuff,4);
                CJT188_ComBcdToBin(sourcedata.sourcebuffer+3,3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&WaterTemperature.flowtmp,tmpBuff,4);
            }
            else if(pOadInfo->Oad.nIndex==1)
            {
                CJT188_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&WaterTemperature.supplytmp,tmpBuff,4);
    
            }
            else if(pOadInfo->Oad.nIndex==2)
            {
                CJT188_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&WaterTemperature.flowtmp,tmpBuff,4);
    
            }

            *desLen = sizeof(WaterTemperature);
            memcpy(desbuffer,&WaterTemperature,sizeof(WaterTemperature));
        }
        break;
        case E_OOP_ST_T:
        {
            OOP_ST_T ST;

            memset(tmpBuff,0x00,30);
            memset((uint8*)&ST,0x00,sizeof(ST));
            
            if(pOadInfo->Oad.nIndex==0)
            {
                ST.nTap =sourcedata.sourcebuffer[0];
                ST.nBattery =sourcedata.sourcebuffer[1];
            }
            else if(pOadInfo->Oad.nIndex==1)
            {
                ST.nTap =sourcedata.sourcebuffer[0];
            }
            else if(pOadInfo->Oad.nIndex==2)
            {
               ST.nBattery =sourcedata.sourcebuffer[0];
            }

            *desLen = sizeof(ST);
            memcpy(desbuffer,&ST,sizeof(ST));
        }
        break;

    default:
        break;
    }

}

/****************************************************************************
*函数名称：Convert_6457_97_To_Struct
*功能：     将97表返回数据转成成对应结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/
void Convert_Protector_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo)
{
    //uint16 offset1;
    uint16 num;
    uint8 i=0;
    uint8 tmpBuff[30];
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
    {
    case E_OOP_ENERGY_T://普通精度电量
    {
        OOP_ENERGY_T Energy1;
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGY_T));
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;

        Energy1.nNum=OOP_FULL_RATES;
        //Energy1.nNum= num;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
        }
        *desLen = sizeof(OOP_ENERGY_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGY_T));
    }
    break;

    case E_OOP_ENERGYS_T://普通精度电量
    {
        OOP_ENERGYS_T Energy1;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGYS_T));

        Energy1.nNum=OOP_FULL_RATES;
        //Energy1.nNum= num;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);  
        }
        *desLen = sizeof(OOP_ENERGYS_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGYS_T));
    }
    break;

    case E_OOP_HENERGY_T://高通精度电量不带符号
    {
        OOP_HENERGY_T HENERGY_T;
 
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&HENERGY_T,0xff,sizeof(OOP_HENERGY_T));

        HENERGY_T.nNum=OOP_FULL_RATES;
        //HENERGY_T.nNum= num;
        HENERGY_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(uint64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGY_T.nValue[i],tmpBuff,8);    
                i++;
            }
        }else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],4,tmpBuff,4,FALSE);
            (*(uint64*)tmpBuff) *= 100;
            memcpy((uint8 *)&HENERGY_T.nValue[index],tmpBuff,8);    
        }
        *desLen = sizeof(OOP_HENERGY_T);
        memcpy(desbuffer,&HENERGY_T,sizeof(OOP_HENERGY_T));
    }
    break;

    case E_OOP_HENERGYS_T://高通精度电量带符号
    {
        OOP_HENERGYS_T HENERGYS_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&HENERGYS_T,0xff,sizeof(OOP_HENERGYS_T));

        HENERGYS_T.nNum=OOP_FULL_RATES;
        //HENERGYS_T.nNum= num;
        HENERGYS_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[i],tmpBuff,8);   
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[index],tmpBuff,8);   
        }
        *desLen = sizeof(OOP_HENERGYS_T);
        memcpy(desbuffer,&HENERGYS_T,sizeof(OOP_HENERGYS_T));
    }
    break;

    case E_OOP_DEMAND_T:  //需量
    {
        OOP_DEMAND_T DEMAND_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&DEMAND_T,0xff,sizeof(OOP_DEMAND_T));

        DEMAND_T.nNum=OOP_FULL_RATES;
        //DEMAND_T.nNum= num;
        DEMAND_T.rsv=0;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*8],3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DEMAND_T.demand[i].nValue,tmpBuff,4);  
                i++;
            }
            i=0;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+3+i*8,5);
                tmpBuff[0] = 0x20;//年高字节补20
                if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
                {
                    memcpy(tmpTime,(uint8*)&DefaultTime,7);
                }
                memcpy(tmpBuff,tmpTime,7);
                memcpy_r((uint8 *)&DEMAND_T.demand[i].time.year,tmpTime,2);
                memcpy((uint8 *)&DEMAND_T.demand[i].time.month,tmpTime+2,5);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[0],3,tmpBuff,4,FALSE);
            memcpy((uint8 *)&DEMAND_T.demand[index].nValue,tmpBuff,4);
            memset(tmpBuff,0x00,30);
            memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+3,5);
            tmpBuff[0] = 0x20;//年高字节补20
            if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
            {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
            }
            memcpy(tmpBuff,tmpTime,7);
            memcpy_r((uint8 *)&DEMAND_T.demand[index].time.year,tmpTime,2);
            memcpy((uint8 *)&DEMAND_T.demand[index].time.month,tmpTime+2,5);
        }
        *desLen = sizeof(OOP_DEMAND_T);
        memcpy(desbuffer,&DEMAND_T,sizeof(OOP_DEMAND_T));
    }

    break;

    case E_OOP_VOLTQR_T://电压合格率
    {
        OOP_VOLTQR_T VOLTQR_T;
        //frame_printf(sourcedata.sourcebuffer,sourcedata.reclen);
        memset(tmpBuff,0x00,30);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+3,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTQR_T.wRate1,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+6,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTQR_T.wRate2,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+9,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime3,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+12,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTQR_T.dwTime4,tmpBuff,4);
        *desLen = sizeof(OOP_VOLTQR_T);
        memcpy(desbuffer,&VOLTQR_T,sizeof(OOP_VOLTQR_T));
    }

    break;

     case E_OOP_VOLTSTAT_T://电压合格率
    {
        OOP_VOLTSTAT_T VOLTSTAT_T;
   
        memset(tmpBuff,0x00,30);
        //frame_printf(sourcedata.sourcebuffer,sourcedata.reclen);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+3,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.wRate1,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+6,3,tmpBuff,2,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.wRate2,tmpBuff,2);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+9,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime3,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+12,3,tmpBuff,4,FALSE);
        memcpy((uint8 *)&VOLTSTAT_T.mon.dwTime4,tmpBuff,4);
        *desLen = sizeof(OOP_VOLTSTAT_T);
        memcpy(desbuffer,&VOLTSTAT_T,sizeof(OOP_VOLTSTAT_T));
    }

    break;

    case E_OOP_DWORD3V_T://分相变量

    {
        OOP_DWORD3V_T DWORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&DWORD3V_T,0xff,sizeof(OOP_DWORD3V_T));

        DWORD3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        DWORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[i],tmpBuff,4);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[index],tmpBuff,4);
        }
        *desLen = sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,&DWORD3V_T,sizeof(OOP_DWORD3V_T));
    }

    break;
    case E_OOP_WORD3V_T://电压

    {
        OOP_WORD3V_T WORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));

        WORD3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        WORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[i],tmpBuff,2);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[index],tmpBuff,2);
        }
        *desLen = sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,&WORD3V_T,sizeof(OOP_WORD3V_T));
    }

    break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT3V_T,0xff,sizeof(OOP_INT3V_T));


        INT3V_T.nNum=3;

        INT3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3,3,tmpBuff,4,TRUE);
                 memcpy((uint8 *)&INT3V_T.nValue[i],tmpBuff,4);
                 INT3V_T.nValue[i] = INT3V_T.nValue[i]*100;
                 TASK_FMT_TRACE(2, COMM_LOG,"INT3V_T.nValue[i] %d\n",INT3V_T.nValue[i]);                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT3V_T.nValue[index],tmpBuff,4);
        }
        *desLen = sizeof(OOP_INT3V_T);
        memcpy(desbuffer,&INT3V_T,sizeof(OOP_INT3V_T));
    }
    break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T  INT4V_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT4V_T,0xff,sizeof(OOP_INT4V_T));

        if(pOadInfo->Oad.value==0x20010200)
        {
            INT4V_T.nNum=3;
        }
        else
        {
            INT4V_T.nNum=4;
        }
        //INT4V_T.nNum= num;
        INT4V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[i],tmpBuff,4);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[index],tmpBuff,4);
        }
        *desLen = sizeof(OOP_INT4V_T);
        memcpy(desbuffer,&INT4V_T,sizeof(OOP_INT4V_T));
    }
    break;
    case E_FRZ_DATETIME_S://645_07电表的冻结时标
    { 
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        
        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer,5);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
            memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        
    }

        break;
    case E_REAL_DATETIME_S://07电能表日历时间
    {
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00     

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        tmpBuff[1] = *(sourcedata.sourcebuffer+3);
        tmpBuff[2] = *(sourcedata.sourcebuffer+2);
        tmpBuff[3] = *(sourcedata.sourcebuffer+1);
        tmpBuff[4] = *(sourcedata.sourcebuffer+6);
        tmpBuff[5] = *(sourcedata.sourcebuffer+5);
        tmpBuff[6] = *(sourcedata.sourcebuffer+4);
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
    }

        break;
    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
            *desLen = 4;
            memcpy(desbuffer,tmpBuff,4);
        }
    break;
    case E_DATETIME_S://
    { 
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer,6);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,tmpTime,7);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        
    }
    break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T METWORD_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&METWORD_T,0xff,sizeof(OOP_METWORD_T));

        METWORD_T.nNum=OOP_METWORD_NUM;
        //METWORD_T.nNum= num;
        METWORD_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&METWORD_T.metWord[i],tmpBuff,2);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&METWORD_T.metWord[index],tmpBuff,2);
        }
        *desLen=sizeof(OOP_METWORD_T);
        memcpy(desbuffer,(uint8 *)&METWORD_T,sizeof(OOP_METWORD_T));

    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));

        LONG4V_T.nNum=4;
        //LONG4V_T.nNum= num;
        LONG4V_T.rsv=0;
        if(num>1)
        {
            while (i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,TRUE);
                memcpy((uint8 *)&LONG4V_T.nValue[i],tmpBuff,2);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,TRUE);
            memcpy((uint8 *)&LONG4V_T.nValue[index],tmpBuff,2);
        }
        *desLen=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
    break;
    case E_LONG_UNSIGNED:
    case E_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
            *desLen = 2;
            memcpy(desbuffer,tmpBuff,2);
        }
    break;
    case E_OOP_WALLET_T:
    {
        OOP_DWORD2_T DWORD2_T;

        memset(tmpBuff,0x00,30);
        memset((uint8*)&DWORD2_T,0x00,sizeof(OOP_DWORD2_T));
        
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
        memcpy_r((uint8 *)&DWORD2_T.nValue1,tmpBuff,4);
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+4,2,tmpBuff,4,FALSE);
        memcpy_r((uint8 *)&DWORD2_T.nValue2,tmpBuff,2);

        *desLen = sizeof(OOP_DWORD2_T);
        memcpy(desbuffer,&DWORD2_T,sizeof(OOP_DWORD2_T));
    }
    break;
    case E_OOP_OCTETVAR16_T:
    {
        OOP_OCTETVAR16_T addr;

        memset((uint8*)&addr,0x00,sizeof(OOP_OCTETVAR16_T));
        addr.nNum =6;
        memcpy_r(addr.value,sourcedata.sourcebuffer,6);
        *desLen = sizeof(OOP_OCTETVAR16_T);
        memcpy(desbuffer,(uint8*)&addr,sizeof(OOP_OCTETVAR16_T));

    }
    break;
    case E_OOP_OCTETVAR64_T:
    {
        OOP_OCTETVAR64_T data;
        i = 0;

        memset((uint8*)&data,0x00,sizeof(OOP_OCTETVAR64_T));
        data.nNum =pOadInfo->DataLen;
        if(pOadInfo->Oad.value == 0x201F0205)
        {
            for(i=0;i<10;i++)
            {
                if(TRUE==Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,FALSE))
                {
                    memcpy(data.value+i*2,tmpBuff,2);
                }
                else
                {
                    memcpy(data.value+i*2,sourcedata.sourcebuffer+i*2,2);
                }
                
            }
        }
        else
        {
            memcpy(data.value,sourcedata.sourcebuffer,pOadInfo->DataLen);
        }
        *desLen = sizeof(OOP_OCTETVAR64_T);
        memcpy(desbuffer,(uint8*)&data,sizeof(OOP_OCTETVAR64_T));
        //TASK_BUF_TRACE( 2,COMM_LOG, desbuffer,*desLen);
    }
    break;
    case E_OOP_PROTECTOR_31BB_T:
    {
        OOP_PROTECTOR_31BB_T protector;
        uint8 offset=0;
        uint8 j=0;
        protector.faultreason = sourcedata.sourcebuffer[offset++];//故障原因

        protector.faultphase =  sourcedata.sourcebuffer[offset++];//故障相别
        TASK_FMT_TRACE ( 2,COMM_LOG, "protector.faultreason is %d,protector.faultphase is %d\n",protector.faultreason,protector.faultphase );
         
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00

        memset((uint8*)&(protector.tmStart.year),0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer+offset,6);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&protector.tmStart.year,tmpTime,2);
        memcpy((uint8*)&protector.tmStart.month,tmpTime+2,5);
        offset =offset+6;
        TASK_BUF_TRACE ( 2,COMM_LOG, (uint8*)&protector.tmStart,sizeof(OOP_DATETIME_S_T));
        //剩余电流
        Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2+offset,2,tmpBuff,2,FALSE);
        memcpy((uint8 *)&protector.residualcurrent,tmpBuff,2);
        offset =offset +2;
        TASK_FMT_TRACE ( 2,COMM_LOG, "protector.residualcurrent is %d\n",protector.residualcurrent);
        //电压
        while(j<3)
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2+offset,2,tmpBuff,2,FALSE);
            memcpy((uint8 *)&protector.voltage[i],tmpBuff,2);
            offset =offset +2;
            j++;
            TASK_FMT_TRACE ( 2,COMM_LOG, "protector.voltage[i] is %d\n",protector.voltage[i] );
        }
        j=0;
        //电流
        while(j<3)
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3+offset,3,tmpBuff,4,FALSE);
            memcpy((uint8 *)&protector.current[i],tmpBuff,4);
            offset =offset +3;
            j++;
            TASK_FMT_TRACE ( 2,COMM_LOG, "protector.current[i] is %d\n",protector.current[i] );
        }
             
        *desLen = sizeof(OOP_PROTECTOR_31BB_T);
        memcpy(desbuffer,(uint8*)&protector,sizeof(OOP_PROTECTOR_31BB_T));

    }
    break;
        default:
        break;
    }

}

/****************************************************************************
*函数名称：Convert_6457_97_To_Struct
*功能：     将97表返回数据转成成对应结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/
void Convert_Cap_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopToCap *pOadInfo)
{
    //uint16 offset1;
    uint16 num;
    uint8 i=0;
    uint8 tmpBuff[30];
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
    {
    case E_OOP_ENERGY_T://普通精度电量
    {
        OOP_ENERGY_T Energy1;
        memset((uint8*)&Energy1,0x00,sizeof(OOP_ENERGY_T));
//        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
//
//        Energy1.nNum=OOP_FULL_RATES;
//        //Energy1.nNum= num;
//        Energy1.rsv=0;
//        if(num>1)
//        {
//            while(i<num)
//            {
//                memset(tmpBuff,0x00,30);
//                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
//                
//                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
//                
//                i++;
//            }
//        }else
//        {
//                memset(tmpBuff,0x00,30);
//                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
//                
//                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
//        }
        *desLen = sizeof(OOP_ENERGY_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGY_T));
    }
    break;
    case E_OOP_DWORD3V_T://电容投切次数

    {
        OOP_DWORD3V_T DWORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&DWORD3V_T,0x00,sizeof(OOP_DWORD3V_T));

        DWORD3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        DWORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[i],sourcedata.sourcebuffer+i*pOadInfo->DataLen,4);
                TASK_FMT_TRACE( 2, COMM_LOG,"电容器投切次数 DWORD3V_T.nValue[i] %x,i is %d\n",DWORD3V_T.nValue[i],i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DWORD3V_T.nValue[index],sourcedata.sourcebuffer,4);
        }
        *desLen = sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,&DWORD3V_T,sizeof(OOP_DWORD3V_T));
    }

    break;
    case E_OOP_WORD3V_T://电容器容量

    {
        OOP_WORD3V_T WORD3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&WORD3V_T,0x00,sizeof(OOP_WORD3V_T));

        WORD3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        WORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[i],sourcedata.sourcebuffer+i*pOadInfo->DataLen,2);
                TASK_FMT_TRACE( 2, COMM_LOG,"电容容量LONG3V_T.nValue[i] %x,i is %d\n",WORD3V_T.nValue[i],i);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&WORD3V_T.nValue[index],sourcedata.sourcebuffer,2);
        }
        *desLen = sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,&WORD3V_T,sizeof(OOP_WORD3V_T));
    }

    break;
    case E_OOP_LONG3V_T://电容电流和容量

    {
        OOP_LONG3V_T LONG3V_T;
   
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&LONG3V_T,0x00,sizeof(LONG3V_T));

        LONG3V_T.nNum=3;
        //DWORD3V_T.nNum=num;
        LONG3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*pOadInfo->DataLen,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&LONG3V_T.nValue[i],sourcedata.sourcebuffer+i*pOadInfo->DataLen,2);
                TASK_FMT_TRACE( 2, COMM_LOG,"电容电流LONG3V_T.nValue[i] %x,i is %d\n",LONG3V_T.nValue[i],i);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
                memcpy((uint8 *)&LONG3V_T.nValue[index],sourcedata.sourcebuffer,2);
        }
        *desLen = sizeof(LONG3V_T);
        memcpy(desbuffer,&LONG3V_T,sizeof(LONG3V_T));
    }

    break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;

        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT3V_T,0x00,sizeof(OOP_INT3V_T));


        INT3V_T.nNum=3;

        INT3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT3V_T.nValue[i],sourcedata.sourcebuffer+i*2,2);
                INT3V_T.nValue[i] = INT3V_T.nValue[i]*10;
                TASK_FMT_TRACE( 2, COMM_LOG,"电容器电容电流 INT3V_T.nValue[i] %x,i is %d\n",INT3V_T.nValue[i],i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT3V_T.nValue[index],sourcedata.sourcebuffer,2);
        }
        *desLen = sizeof(OOP_INT3V_T);
        memcpy(desbuffer,&INT3V_T,sizeof(OOP_INT3V_T));
    }
    break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T  INT4V_T;
        i =1;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&INT4V_T,0x00,sizeof(OOP_INT4V_T));

        if(pOadInfo->Oad.value==0x20010200)
        {
            INT4V_T.nNum=3;
        }
        else
        {
            INT4V_T.nNum=4;
        }
        //INT4V_T.nNum= num;
        INT4V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*3,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[i],sourcedata.sourcebuffer+i*2,2);
                INT4V_T.nValue[i] =INT4V_T.nValue[i]*10000;
                TASK_FMT_TRACE( 2, COMM_LOG,"电容器功率 INT4V_T.nValue[i] %x,i is %d\n",INT4V_T.nValue[i],i);
                
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                //Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,TRUE);
                memcpy((uint8 *)&INT4V_T.nValue[index],sourcedata.sourcebuffer+i*2,2);
        }
        *desLen = sizeof(OOP_INT4V_T);
        memcpy(desbuffer,&INT4V_T,sizeof(OOP_INT4V_T));
    }
    break;

    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
            *desLen = 4;
            memcpy(desbuffer,tmpBuff,4);
        }
    break;
    case E_OOP_OCTETVAR64_T:
    {
        OOP_OCTETVAR64_T OCTETVAR64 ;
        uint8 buf[10];
        uint8 tmpBuff[4];

        memset((uint8*)&OCTETVAR64,0x00,sizeof(OOP_OCTETVAR64_T));
        
        OCTETVAR64.nNum = 0x04;

        memset(buf,0x00,10);
        memset(tmpBuff,0x00,4);
        memcpy(buf,sourcedata.sourcebuffer,5);//特殊处理电容器运行状态字需要去拼；
        if(buf[2]==0x5A)
        {
            tmpBuff[1]=tmpBuff[1]|0x01;
        }
        else if(buf[2]==0xA5)
        {
            tmpBuff[1]=tmpBuff[1]|0x02;
        }
        if(buf[3]==0x5A)
        {
            tmpBuff[1]=tmpBuff[1]|0x04;
        }
        else if(buf[2]==0xA5)
        {
            tmpBuff[1]=tmpBuff[1]|0x08;
        }
        if(buf[4]==0x5A)
        {
            tmpBuff[1]=tmpBuff[1]|0x10;
        }
        else if(buf[2]==0xA5)
        {
            tmpBuff[1]=tmpBuff[1]|0x20;
        }
        tmpBuff[2]= buf[1]&0x1F;
        tmpBuff[3]= (buf[1]&0x20)+(buf[0]&0x7F);
        *desLen=sizeof(OCTETVAR64);
        memcpy(OCTETVAR64.value,tmpBuff,4);
        memcpy(desbuffer,(uint8 *)&OCTETVAR64,sizeof(OCTETVAR64));
    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));

        LONG4V_T.nNum=4;
        //LONG4V_T.nNum= num;
        LONG4V_T.rsv=0;
        if(num>1)
        {
            while (i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,TRUE);
                memcpy((uint8 *)&LONG4V_T.nValue[i],tmpBuff,2);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,TRUE);
            memcpy((uint8 *)&LONG4V_T.nValue[index],tmpBuff,2);
        }
        *desLen=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
    break;
    case E_LONG_UNSIGNED:
    case E_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,2,FALSE);
            *desLen = 2;
            memcpy(desbuffer,tmpBuff,2);
        }
    break;
    default:
        break;
    }

}

/****************************************************************************
*函数名称：Convert_6457_97_To_Struct
*功能：     将97表返回数据转成成对应结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void Convert_6457_97_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo)
{

        //uint16 offset1;
    uint16 num;
    uint8 i=0;
    uint8 tmpBuff[30];
    uint8 index = pOadInfo->Oad.nIndex ;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
    {
    case E_OOP_ENERGY_T://普通精度电量
    {
        OOP_ENERGY_T Energy1;
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGY_T));
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=5;
        }
        else
        {
            num=1;
        }
        Energy1.nNum=OOP_FULL_RATES;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                set_bit_value((uint8*)&Energy1.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&Energy1.rsv, 32, index);
        }
        *desLen = sizeof(OOP_ENERGY_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGY_T));
    }
    break;

    case E_OOP_ENERGYS_T://普通精度电量
    {
        OOP_ENERGYS_T Energy1;

        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=5;
        }
        else
        {
            num=1;
        }
        memset((uint8*)&Energy1,0xff,sizeof(OOP_ENERGYS_T));

        Energy1.nNum=OOP_FULL_RATES;
        Energy1.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*4,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[i],tmpBuff,4);  
                set_bit_value((uint8*)&Energy1.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                memcpy((uint8 *)&Energy1.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&Energy1.rsv, 32, index);
        }
        *desLen = sizeof(OOP_ENERGYS_T);
        memcpy(desbuffer,&Energy1,sizeof(OOP_ENERGYS_T));
    }
    break;

    case E_OOP_HENERGY_T://高通精度电量不带符号
    {
        OOP_HENERGY_T HENERGY_T;
 
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=5;
        }
        else
        {
            num=1;
        }
        memset((uint8*)&HENERGY_T,0xff,sizeof(OOP_HENERGY_T));

        HENERGY_T.nNum=OOP_FULL_RATES;
        HENERGY_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(uint64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGY_T.nValue[i],tmpBuff,8);  
                set_bit_value((uint8*)&HENERGY_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                (*(uint64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGY_T.nValue[index],tmpBuff,8);  
                set_bit_value((uint8*)&HENERGY_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_HENERGY_T);
        memcpy(desbuffer,&HENERGY_T,sizeof(OOP_HENERGY_T));
    }
    break;

    case E_OOP_HENERGYS_T://高通精度电量带符号
    {
        OOP_HENERGYS_T HENERGYS_T;

        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=5;
        }
        else
        {
            num=1;
        }
        memset((uint8*)&HENERGYS_T,0xff,sizeof(OOP_HENERGYS_T));

        HENERGYS_T.nNum=OOP_FULL_RATES;
        HENERGYS_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*4],4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[i],tmpBuff,8); 
                set_bit_value((uint8*)&HENERGYS_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,4,tmpBuff,4,FALSE);
                (*(int64*)tmpBuff) *= 100;
                memcpy((uint8 *)&HENERGYS_T.nValue[index],tmpBuff,8);
                set_bit_value((uint8*)&HENERGYS_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_HENERGYS_T);
        memcpy(desbuffer,&HENERGYS_T,sizeof(OOP_HENERGYS_T));
    }
    break;

    case E_OOP_DEMAND_T:  //需量
    {
        OOP_DEMAND_T DEMAND_T;

        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=5;
        }
        else
        {
            num=1;
        }
        memset((uint8*)&DEMAND_T,0xff,sizeof(OOP_DEMAND_T));

        DEMAND_T.nNum=OOP_FULL_RATES;
        DEMAND_T.rsv=0;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x07,0xD0,0x01,0x01,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(&sourcedata.sourcebuffer[i*3],3,tmpBuff,4,FALSE);
                memcpy((uint8 *)&DEMAND_T.demand[i].nValue,tmpBuff,4);  
                set_bit_value((uint8*)&DEMAND_T.rsv, 32, i);
                i++;
            }
            i=0;
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                memcpy_r(tmpBuff+2,sourcedata.sourcebuffer+num*3+i*4,4);
                tmpBuff[0] = 0x20;//年高字节补20
                if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
                {
                    memcpy(tmpTime,(uint8*)&DefaultTime,7);
                }
                memcpy(tmpBuff,tmpTime,7);
                tmpBuff[0]=0;
                tmpBuff[1]=0;
                memcpy_r((uint8 *)&DEMAND_T.demand[i].time.year,tmpBuff,2);
                memcpy((uint8 *)&DEMAND_T.demand[i].time.month,tmpBuff,6);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,3,tmpBuff,4,FALSE);
            memcpy((uint8 *)&DEMAND_T.demand[index].nValue,tmpBuff,4);  
            memset(tmpBuff,0x00,30);
            memcpy_r(tmpBuff+2,sourcedata.sourcebuffer+3,4);
            tmpBuff[0] = 0x20;//年高字节补20
            if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
            {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
            }
            memcpy(tmpBuff,tmpTime,7);
            tmpBuff[0]=0;
            tmpBuff[1]=0;
            memcpy_r((uint8 *)&DEMAND_T.demand[index].time.year,tmpBuff,2);
            memcpy((uint8 *)&DEMAND_T.demand[index].time.month,tmpBuff,6);
            set_bit_value((uint8*)&DEMAND_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_DEMAND_T);
        memcpy(desbuffer,&DEMAND_T,sizeof(OOP_DEMAND_T));
    }

    break;
    case E_OOP_DWORD3V_T://分相变量

    {
        OOP_DWORD3V_T DWORD3V_T;
        uint8 tmplen=0;
        tmplen=pOadInfo->DataLen;
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=3;
        }
        else
        {
            num=1;
        }
        if(((pOadInfo->DI97)&0xFFF0)==0xB620)
        {
            tmplen=2;
        }
        memset((uint8*)&DWORD3V_T,0xff,sizeof(OOP_DWORD3V_T));
        DWORD3V_T.nNum=3;
        DWORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*tmplen,tmplen,tmpBuff,4,FALSE);
                (*(uint32*)tmpBuff) *= 10;
                memcpy((uint8 *)&DWORD3V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&DWORD3V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,tmplen,tmpBuff,4,FALSE);
                (*(uint32*)tmpBuff) *= 10;
                memcpy((uint8 *)&DWORD3V_T.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&DWORD3V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,&DWORD3V_T,sizeof(OOP_DWORD3V_T));
    }

    break;
    case E_OOP_WORD3V_T://电压

    {
        OOP_WORD3V_T WORD3V_T;
        uint8 tmplen=0;
        tmplen=pOadInfo->DataLen;
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=3;
        }
        else
        {
            num=1;
        }

        memset((uint8*)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));
        WORD3V_T.nNum=3;
        WORD3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);

                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*tmplen,tmplen,tmpBuff,2,FALSE);
                (*(uint32*)tmpBuff) *= 10;
                memcpy((uint8 *)&WORD3V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&WORD3V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,tmplen,tmpBuff,2,FALSE);
                (*(uint32*)tmpBuff) *= 10;
                memcpy((uint8 *)&WORD3V_T.nValue[index],tmpBuff,2);
                set_bit_value((uint8*)&WORD3V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,&WORD3V_T,sizeof(OOP_WORD3V_T));
    }

    break;
    case E_OOP_INT3V_T://电流

    {
        OOP_INT3V_T  INT3V_T;
        uint8 tmplen=0;
        tmplen=2;
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=3;
        }
        else
        {
            num=1;
        }

        memset((uint8*)&INT3V_T,0xff,sizeof(OOP_INT3V_T));

        INT3V_T.nNum=3;
        INT3V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*tmplen,tmplen,tmpBuff,4,FALSE);
                if(((pOadInfo->DI97)&0xFFF0)==0xB620)
                {
                    (*(uint32*)tmpBuff) *= 10;
                }
                memcpy((uint8 *)&INT3V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&INT3V_T.rsv, 32, i);
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,tmplen,tmpBuff,4,FALSE);
                if(((pOadInfo->DI97)&0xFFF0)==0xB620)
                {
                    (*(uint32*)tmpBuff) *= 10;
                }
                memcpy((uint8 *)&INT3V_T.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&INT3V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_INT3V_T);
        memcpy(desbuffer,&INT3V_T,sizeof(OOP_INT3V_T));
    }

        break;

    case E_OOP_INT4V_T://功率变量

    {
        OOP_INT4V_T  INT4V_T;
        uint8 tmplen=0;
        tmplen=pOadInfo->DataLen;
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=4;
        }
        else
        {
            num=1;
        }
        if(((pOadInfo->DI97)&0xFFF0)==0xB640)
        {
            tmplen=2;
        }
        memset((uint8*)&INT4V_T,0xff,sizeof(OOP_INT4V_T));

        INT4V_T.nNum=4;
        INT4V_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*tmplen,tmplen,tmpBuff,4,FALSE);
                if(((pOadInfo->DI97)&0xFFF0)==0xB640)
                {
                    (*(uint32*)tmpBuff) *= 100;
                }
                memcpy((uint8 *)&INT4V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&INT4V_T.rsv, 32, i);
                i++;
            }
        }else
        {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer,tmplen,tmpBuff,4,FALSE);
                if(((pOadInfo->DI97)&0xFFF0)==0xB640)
                {
                    (*(uint32*)tmpBuff) *= 100;
                }
                memcpy((uint8 *)&INT4V_T.nValue[index],tmpBuff,4);
                set_bit_value((uint8*)&INT4V_T.rsv, 32, index);
        }
        *desLen = sizeof(OOP_INT4V_T);
        memcpy(desbuffer,&INT4V_T,sizeof(OOP_INT4V_T));
    }

        break;

    case E_REAL_DATETIME_S://97电能表日历时间
    {
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x07,0xD0,0x01,0x01,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00     

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        tmpBuff[1] = *(sourcedata.sourcebuffer+3);
        tmpBuff[2] = *(sourcedata.sourcebuffer+2);
        tmpBuff[3] = *(sourcedata.sourcebuffer+1);
        tmpBuff[4] = *(sourcedata.sourcebuffer+6);
        tmpBuff[5] = *(sourcedata.sourcebuffer+5);
        tmpBuff[6] = *(sourcedata.sourcebuffer+4);
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
    }

        break;
    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
        {
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,pOadInfo->DataLen,tmpBuff,4,FALSE);
            *desLen = 4;
            memcpy(desbuffer,tmpBuff,4);
        }
    break;
    case E_DATETIME_S://
    { 
        OOP_DATETIME_S_T DATETIME_S_T;
        uint8 tmpTime[7];
        DateTimeHex_t DefaultTime={0x07,0xD0,0x01,0x01,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        memcpy_r(tmpBuff+1,sourcedata.sourcebuffer,6);  
        tmpBuff[0] = 0x20;//年高字节补20
        if(FALSE == DT_DateTimeBCD2Hex((DateTimeBcd_t*)tmpBuff,(DateTimeHex_t*)tmpTime))
        {
                memcpy(tmpTime,(uint8*)&DefaultTime,7);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpTime,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpTime+2,5);
        *desLen = sizeof(OOP_DATETIME_S_T);
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));
        
    }
    break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T METWORD_T;
        i=0;
        num=pOadInfo->TotalDataLen/pOadInfo->DataLen;
        memset((uint8*)&METWORD_T,0xFF,sizeof(OOP_METWORD_T));

        METWORD_T.nNum=OOP_METWORD_NUM;
        METWORD_T.rsv=0;
        if(num>1)
        {
            while(i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&METWORD_T.metWord[i],tmpBuff,2);
                set_bit_value((uint8*)&METWORD_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,FALSE);
            memcpy((uint8 *)&METWORD_T.metWord[index],tmpBuff,2);
            set_bit_value((uint8*)&METWORD_T.rsv, 32, index);
        }
        *desLen=sizeof(OOP_METWORD_T);
        memcpy(desbuffer,(uint8 *)&METWORD_T,sizeof(OOP_METWORD_T));

    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        i=0;
        if(pOadInfo->Oad.nIndex==0x00)
        {
            num=4;
        }
        else
        {
            num=1;
        }
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));

        LONG4V_T.nNum=4;
        LONG4V_T.rsv = 0;
        if(num>1)
        {
            while (i<num)
            {
                memset(tmpBuff,0x00,30);
                Dlt645_ComBcdToBin(sourcedata.sourcebuffer+i*2,2,tmpBuff,2,FALSE);
                memcpy((uint8 *)&LONG4V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&LONG4V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            Dlt645_ComBcdToBin(sourcedata.sourcebuffer,2,tmpBuff,2,FALSE);
            memcpy((uint8 *)&LONG4V_T.nValue[index],tmpBuff,2);
            set_bit_value((uint8*)&LONG4V_T.rsv, 32, index);
        }
        *desLen=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
    break;
    default:
        break;
    }

}

/****************************************************************************
*函数名称：taskmng_oop_data_extract
*功能：     将面向对象报文转换成结构体
*输入参数：
*输出参数：
*返回：
****************************************************************************/

BOOL  taskmng_oop_data_extract(const uint8 *inData,uint16 *offset,uint16 inDataLen,uint8 *tembuffer,enum tag_OOP_DATATYPE DataType,BOOL bWithType)

{
    uint16 dataLen;
    uint8 nchioce;
    uint8 dataNum;
    uint8 i;
    uint16 offSet=*offset;
    enum tag_OOP_DATATYPE dataType;

    if(bWithType)
    {
        dataType = DataType;
    }
    else
    {
        dataType = (enum tag_OOP_DATATYPE)inData[offSet++];
    }

    if(dataType == DT_NULL)
    {
        *offset = offSet;
        return TRUE;
    }
    if((dataType == DT_STRUCTURE)
        ||(dataType == DT_ARRAY)
        ||(dataType == DT_OCTET_STRING)
        ||(dataType == DT_VISIBLE_STRING)
        ||(dataType == DT_UTF8_STRING)
        ||(dataType == DT_BIT_STRING)
        ||(dataType == DT_MAC)
        ||(dataType == DT_RN)
        ||(dataType == DT_TSA))
    {
        dataLen = inData[offSet++];
        if(dataLen == 0x81)
            dataLen = inData[offSet++];
        else if(dataLen == 0x82)
        {
            dataLen = inData[offSet++];
            dataLen <<= 8;
            dataLen += inData[offSet++];
        }
        else if(dataLen>0x82||dataLen==0x80)
        {
            return FALSE;
        }
        if((dataType != DT_ARRAY) && (dataType != DT_OCTET_STRING) && (dataType != DT_VISIBLE_STRING) \
            && (dataType != DT_MAC) &&(dataLen == 0))
        {
            return FALSE;
        }

        if(dataType == DT_BIT_STRING)
        {
            if(dataLen % 8 == 0)
                dataLen /= 8;
            else
                dataLen = (dataLen/8 + 1);
        }

        if((dataType == DT_STRUCTURE)
            ||(dataType == DT_ARRAY))
        {
            //uint16 eleNum = dataLen;

                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                {
                    return FALSE;
                }
        }
        else
        {
            memcpy(tembuffer,inData+offSet,dataLen);
            offSet+=dataLen;
        }
    }
    else if(dataType == DT_RSD)
    {
        uint8 eleNum=0;
        nchioce = inData[offSet++];
        switch(nchioce)
        {
            case 0://
                break;
            case 1:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 2:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_OAD,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                    return FALSE;
                break;
            case 3:
                eleNum = inData[offSet++];    //SEQUENCE OF Selector2
                for(i=0;i<eleNum;i++)
                {
                    if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_OAD,TRUE))
                        return FALSE;
                    if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                        return FALSE;
                    if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
                        return FALSE;
                }
                break;
            case 4:
            case 5:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_MS,TRUE))
                    return FALSE;
                break;
            case 6:
            case 7:
            case 8:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_DATETIME_S,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_TI,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_MS,TRUE))
                    return FALSE;
                break;
            case 9:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_UNSIGNED,TRUE))
                    return FALSE;
                break;
            case 10:
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_UNSIGNED,TRUE))
                    return FALSE;
                if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_MS,TRUE))
                    return FALSE;
                break;

        }
    }
    else if(dataType == DT_CSD)
    {
        nchioce = inData[offSet++];
        if(nchioce == 0)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
        }
        else if(nchioce == 1)
        {
            dataLen = getDataTypeLength(DT_OAD);
            offSet +=dataLen;
            dataNum = inData[offSet++];
            offSet +=dataLen*dataNum;
        }
        else
            return FALSE;

    }
    else if(dataType == DT_ROAD)
    {
        dataLen = getDataTypeLength(DT_OAD);
        offSet +=dataLen;
        dataNum = inData[offSet++];
        offSet +=dataLen*dataNum;
        printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
    }
    else if(dataType == DT_RCSD)
    {
        dataNum = inData[offSet++];
        for(i=0;i<dataNum;i++)
        {
            if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_CSD,TRUE))
                return FALSE;
        }
    }
    else if(dataType == DT_MS)
    {
        nchioce = inData[offSet++];
        if((nchioce == 0)||(nchioce == 1))
        {

        }
        else if(nchioce == 2)//一组用户类型
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength(DT_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 3)
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet += dataLen;
            }
        }
        else if(nchioce == 4)
        {
            dataNum = inData[offSet++];
            dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
            offSet +=dataLen*dataNum;
        }
        else if(nchioce == 5)//一组用户类型区间
        {
            dataNum = inData[offSet++];

            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else if(nchioce == 6)//一组用户地址区间
        {
            dataNum = inData[offSet++];
            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
                if(inData[offSet++]!=DT_TSA)
                    return FALSE;
                dataLen = inData[offSet++];
                if((dataLen>8)||(dataLen==0))
                    return FALSE;
                offSet +=dataLen;
            }
        }
        else if(nchioce == 7)//一组配置序号区间
        {
            dataNum = inData[offSet++];

            while(dataNum--)
            {
                if(inData[offSet++]>3)
                    return FALSE;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
                if(inData[offSet++]!=DT_LONG_UNSIGNED)
                    return FALSE;
                dataLen = getDataTypeLength(DT_LONG_UNSIGNED);
                offSet +=dataLen;
            }
        }
        else
            return FALSE;

    }
    else if(dataType == DT_REGION)
    {
        if(inData[offSet++]>3)
            return FALSE;
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
            return FALSE;
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_NULL,FALSE))
            return FALSE;
    }
    else if(dataType == DT_SID)
    {
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DOUBLE_LONG_UNSIGNED,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_OCTET_STRING,TRUE))
            return FALSE;
    }
    else if(dataType == DT_SIDMAC)
    {
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_SID,TRUE))
            return FALSE;
        if(FALSE == taskmng_oop_data_extract(inData,&offSet,inDataLen,tembuffer,DT_MAC,TRUE))
            return FALSE;
    }
    else if(dataType == DT_TI)
    {
        if(inData[offSet++]>5)
            return FALSE;
        offSet += 2;
    }
    else
    {
        //printf("AAAAAAAAAAAAAAdataType is:%d\n",dataType);
        dataLen = getDataTypeLength(dataType);
        memcpy(tembuffer,inData+offSet,dataLen);
        //printf("taskmng_oop_data_extract dataLen is:%d\n",dataLen);
        //printf("taskmng_oop_data_extract offSet is:%d\n",offSet);
        if(0xFF != dataLen)
            offSet +=dataLen;
        else
            return FALSE;
    }
    //printf("taskmng_oop_data_extract offSet is:%d\n",offSet);
    //printf("taskmng_oop_data_extract inDataLen is:%d\n",inDataLen);
    if(offSet>inDataLen)
        return FALSE;
    *offset = offSet;
    return TRUE;
}
/****************************************************************************
*函数名称：getDataTypeLength
*功能：     获取类型对应的数据长度
*输入参数：
*输出参数：
*返回：对应类型的数据长度
****************************************************************************/

uint8 getDataTypeLength(enum tag_OOP_DATATYPE dataType)
{
  switch(dataType)
  {
        case DT_NULL:
            return 0;
        case DT_BOOL:
        case DT_UNSIGNED:
        case DT_INTEGER:
        case DT_ENUM:
            return 1;

        case DT_LONG:
        case DT_LONG_UNSIGNED:
        case DT_OI:
        case DT_SCALER_UNIT:
            return 2;

        case DT_TIME:
            return 3;
        //4
        case DT_DOUBLE_LONG:
        case DT_DOUBLE_LONG_UNSIGNED:

        case DT_FLOAT32:
        case DT_OAD:
        case DT_OMD:

            return 4;
        //5
        case DT_DATE:
        case DT_COMDCB:
            return 5;

        //7
        case DT_DATETIME_S:
            return 7;
        //8
        case DT_LONG64:

        case DT_LONG64_UNSIGNED:
        case DT_FLOAT64:
            return 8;
        //9

        //10
        case DT_DATETIME:
            return 10;
        default:
            return 0xFF;
  }
}


void Convert_Oop_Data_To_Struct(uint8 *soucerbuffer,uint16 *offset1,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength,uint8 pwrType)
{
    
    //uint16 num;
    uint8 tmpBuff[30];
    uint8 i=0;
    uint16 temlen;
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
    {
        case E_OOP_PERIOD_T:
        {
            OOP_PERIOD_T period = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);

            if(index==0)
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                memcpy_r((uint8 *)&period.yPeriod, tmpBuff, 1);

            if(index==1)
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                memcpy_r((uint8 *)&period.dPeriodMet, tmpBuff, 1);

            if(index==2)
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                memcpy_r((uint8 *)&period.dPeriod, tmpBuff, 1);

            if(index==3)
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                memcpy_r((uint8 *)&period.rate, tmpBuff, 1);

            if(index==4)
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                memcpy_r((uint8 *)&period.pubHoliday, tmpBuff, 1);
                         
            *offset1 = offset;
            *deslength = sizeof(E_OOP_PERIOD_T);
            memcpy(desbuffer, (uint8 *)&period, sizeof(E_OOP_PERIOD_T));
            
            break;
        }
        case E_OOP_EVTCFG_3003_T:
        {
            OOP_EVTCFG_3003_T delay = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&delay.wVUp, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&delay.nAUp, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&delay.nDelay, tmpBuff, 1);
                         
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_3003_T);
            memcpy(desbuffer, (uint8 *)&delay, sizeof(OOP_EVTCFG_3003_T));
            
            break;
        }
        case E_OOP_EVTCFG_DELAY_T:
        {
            OOP_EVTCFG_DELAY_T delay = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&delay.nDelay, tmpBuff, 1);
                         
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_DELAY_T);
            memcpy(desbuffer, (uint8 *)&delay, sizeof(OOP_EVTCFG_DELAY_T));
            
            break;
        }
        case E_OOP_OTHERLIMIT_T:
        {
            OOP_OTHERLIMIT_T limit = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&limit.overChar, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&limit.cornorChar, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&limit.switchChar, tmpBuff, 4);
                         
            *offset1 = offset;
            *deslength = sizeof(OOP_OTHERLIMIT_T);
            memcpy(desbuffer, (uint8 *)&limit, sizeof(OOP_OTHERLIMIT_T));
            
            break;
        }
        case E_OOP_EVTCFG_3004_T:
        {
            OOP_EVTCFG_3004_T evtcfg_3004 = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3004.wVDown, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3004.nAUp, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3004.nADown, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3004.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_3004_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_3004, sizeof(OOP_EVTCFG_3004_T));
            
            break;
        }
        case E_OOP_FROZENSETS_T:
        {
            OOP_FROZENSETS_T frozensets;
            uint16 offset=0;
            uint16 i=0;
            memset((uint8*)&frozensets,0x00,sizeof(OOP_FROZENSETS_T));

            memset(tmpBuff,0x00,30);

            if(soucerbuffer[offset++]==DT_ARRAY)
             {
                frozensets.nNum = soucerbuffer[offset++];

                for(i=0;i<frozensets.nNum;i++)
                {
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&frozensets.objs[i].nPeriod,tmpBuff,2);

                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&frozensets.objs[i].oad.value,tmpBuff,4);
                    
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&frozensets.objs[i].nDepth,tmpBuff,2);

                    set_bit_value((uint8*)&frozensets.rsv, 32, i);
                }
             }
            
            *offset1 = offset;
            *deslength = sizeof(OOP_FROZENSETS_T);
            memcpy(desbuffer,(uint8*)&frozensets,sizeof(OOP_FROZENSETS_T));
        }
        break;
        case E_OOP_PUBHOLIDAY_T:
        {
            OOP_PUBHOLIDAY_T pubholiday;
            uint16 offset=0;
            uint16 i=0;
            memset((uint8*)&pubholiday,0x00,sizeof(OOP_PUBHOLIDAY_T));

            memset(tmpBuff,0x00,30);

            if(soucerbuffer[offset++]==DT_ARRAY)
             {
                pubholiday.nNum = soucerbuffer[offset++];

                for(i=0;i<pubholiday.nNum;i++)
                {
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&pubholiday.holiday[i].date.year,tmpBuff,2);
                    offset+=2;
                    pubholiday.holiday[i].date.month = soucerbuffer[offset++];
                    pubholiday.holiday[i].date.mday = soucerbuffer[offset++];
                    pubholiday.holiday[i].date.wday = soucerbuffer[offset++];

                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&pubholiday.holiday[i].meterNum,tmpBuff,1);

                    set_bit_value((uint8*)&pubholiday.rsv, 32, i);
                }
             }
            
            *offset1 = offset;
            *deslength = sizeof(OOP_PUBHOLIDAY_T);
            memcpy(desbuffer,(uint8*)&pubholiday,sizeof(OOP_PUBHOLIDAY_T));
        }
        break;
        case E_OOP_EVENT_T:
        {
            OOP_EVENT_T event;
            uint16 offset=0;
            uint16 i=0;
            memset((uint8*)&event,0x00,sizeof(OOP_EVENT_T));

            memset(tmpBuff,0x00,30);

            if(soucerbuffer[offset++]==DT_ARRAY)
             {
                event.state.nNum = soucerbuffer[offset++];

                for(i=0;i<event.state.nNum;i++)
                {
                    offset++;
                    memcpy_r((uint8 *)&event.state.state[i].oad,&soucerbuffer[offset],4);
                    offset+=4;
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_EVENT_T);
            memcpy(desbuffer,(uint8*)&event,sizeof(OOP_EVENT_T));
        }
        break;
        case E_OOP_OADS_T:
        {
            OOP_OADS_T oads;
            uint16 offset=0;
            uint16 i=0;


            memset((uint8*)&oads,0x00,sizeof(OOP_OADS_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                oads.nNum = soucerbuffer[offset++];

                for(i=0;i<oads.nNum;i++)
                {
                   offset++;
                   memcpy_r((uint8 *)&oads.oad[i],&soucerbuffer[offset],4);      
                   set_bit_value((uint8*)&oads.rsv, 32, i);
                   offset+=4;
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_OADS_T);
            memcpy(desbuffer,(uint8*)&oads,sizeof(OOP_OADS_T));
        }
        break;
        case E_OOP_TERMINAL_LIMIT_T:
        {
            OOP_TERMINAL_LIMIT_T terminal_limit = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&terminal_limit.ntmp1, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&terminal_limit.ntmp2, tmpBuff, 4);
            
            *offset1 = offset;
            *deslength = sizeof(OOP_TERMINAL_LIMIT_T);
            memcpy(desbuffer, (uint8 *)&terminal_limit, sizeof(OOP_TERMINAL_LIMIT_T));
            
            break;
        }
        case E_OOP_TELECON_PARAM_T:
        {
            OOP_TELECON_PARAM_T telecon_param = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&telecon_param.limit, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&telecon_param.delay, tmpBuff, 2);
            
            *offset1 = offset;
            *deslength = sizeof(OOP_TELECON_PARAM_T);
            memcpy(desbuffer, (uint8 *)&telecon_param, sizeof(OOP_TELECON_PARAM_T));
            
            break;
        }
        case E_OOP_RS485_T:
        {
            OOP_SERIALPORT_T serialport;
            memset(&serialport,0x00,sizeof(OOP_SERIALPORT_T));
            serialport.nNum = OOP_MAX_COM;
            uint16 offset = 0;
            uint16 i,num;
            memset(tmpBuff, 0x00, 30);
            if(soucerbuffer[offset] == DT_ARRAY)
            {
                offset++;
                num = soucerbuffer[offset++];
                for(i=0;i<num;i++)
                {
                    offset+=2;
                    serialport.com[i].name.nNum = soucerbuffer[offset+1];
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);          
                    memcpy(serialport.com[i].name.value,tmpBuff,serialport.com[i].name.nNum);
                    offset++;
                    serialport.com[i].param.baud = soucerbuffer[offset++];
                    serialport.com[i].param.parity =  soucerbuffer[offset++];
                    serialport.com[i].param.databits =  soucerbuffer[offset++];
                    serialport.com[i].param.stopbits =  soucerbuffer[offset++];
                    serialport.com[i].param.flowctrl =  soucerbuffer[offset++];
                    offset++;
                    serialport.com[i].func = soucerbuffer[offset++];
                    set_bit_value((uint8*)&serialport.rsv, 32, i);
                }
            }else
            {
                offset+=2;
                serialport.com[index].name.nNum = soucerbuffer[offset+1];
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);          
                memcpy(serialport.com[index].name.value,tmpBuff,serialport.com[index].name.nNum);
                offset++;
                serialport.com[index].param.baud = soucerbuffer[offset++];
                serialport.com[index].param.parity =  soucerbuffer[offset++];
                serialport.com[index].param.databits =  soucerbuffer[offset++];
                serialport.com[index].param.stopbits =  soucerbuffer[offset++];
                serialport.com[index].param.flowctrl =  soucerbuffer[offset++];
                offset++;
                serialport.com[index].func = soucerbuffer[offset++];
                set_bit_value((uint8*)&serialport.rsv, 32, index);

            }
            
            *offset1 = offset;
            *deslength = sizeof(OOP_SERIALPORT_T);
            memcpy(desbuffer, (uint8 *)&serialport, sizeof(OOP_SERIALPORT_T));
            
            break;
        }
        case E_OOP_ROUTEMODULE_T:
        {
            OOP_ROUTEMODULE_T data;
            memset(&data,0x00,sizeof(OOP_ROUTEMODULE_T));
            data.nNum = OOP_MAX_PLC;
            uint16 offset = 0;
            uint16 i,num;
            memset(tmpBuff, 0x00, 30);
            if(soucerbuffer[offset] == DT_ARRAY)
            {
                offset++;
                num = soucerbuffer[offset++];
                for(i=0;i<num;i++)
                {
                    offset+=2;
                    data.item[i].name.nNum = soucerbuffer[offset+1];
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);          
                    memcpy(data.item[i].name.value,tmpBuff,data.item[i].name.nNum);
                    offset++;
                    data.item[i].dcb.baud = soucerbuffer[offset++];
                    data.item[i].dcb.parity =  soucerbuffer[offset++];
                    data.item[i].dcb.databits =  soucerbuffer[offset++];
                    data.item[i].dcb.stopbits =  soucerbuffer[offset++];
                    data.item[i].dcb.flowctrl =  soucerbuffer[offset++];
                    offset+=2;
                    data.item[i].verInfo.factory.nNum = soucerbuffer[offset+1];
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                    memcpy(data.item[i].verInfo.factory.value,tmpBuff,data.item[i].verInfo.factory.nNum);
                    data.item[i].verInfo.ic.nNum = soucerbuffer[offset+1];
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                    memcpy(data.item[i].verInfo.ic.value,tmpBuff,data.item[i].verInfo.ic.nNum);
                    offset++;
                    memcpy_r(&data.item[i].verInfo.dtVer.year,&soucerbuffer[offset],2 );
                    offset+=2;
                    data.item[i].verInfo.dtVer.month = soucerbuffer[offset++];
                    data.item[i].verInfo.dtVer.mday = soucerbuffer[offset++];
                    data.item[i].verInfo.dtVer.wday = soucerbuffer[offset++];
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                    memcpy_r(&data.item[i].verInfo.wVer,tmpBuff,2);
                    
                    set_bit_value((uint8*)&data.resv, 32, i);
                }
            }else
            {
                offset+=2;
                data.item[index].name.nNum = soucerbuffer[offset+1];
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);          
                memcpy(data.item[index].name.value,tmpBuff,data.item[index].name.nNum);
                offset++;
                data.item[index].dcb.baud = soucerbuffer[offset++];
                data.item[index].dcb.parity =  soucerbuffer[offset++];
                data.item[index].dcb.databits =  soucerbuffer[offset++];
                data.item[index].dcb.stopbits =  soucerbuffer[offset++];
                data.item[index].dcb.flowctrl =  soucerbuffer[offset++];
                offset+=2;
                data.item[index].verInfo.factory.nNum = soucerbuffer[offset+1];
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                memcpy(data.item[index].verInfo.factory.value,tmpBuff,data.item[index].verInfo.factory.nNum);
                data.item[index].verInfo.ic.nNum = soucerbuffer[offset+1];
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                memcpy(data.item[index].verInfo.ic.value,tmpBuff,data.item[index].verInfo.ic.nNum);
                offset++;
                memcpy_r(&data.item[index].verInfo.dtVer.year,&soucerbuffer[offset],2 );
                offset+=2;
                data.item[index].verInfo.dtVer.month = soucerbuffer[offset++];
                data.item[index].verInfo.dtVer.mday = soucerbuffer[offset++];
                data.item[index].verInfo.dtVer.wday = soucerbuffer[offset++];
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);    
                memcpy_r(&data.item[index].verInfo.wVer,tmpBuff,2);
                
                set_bit_value((uint8*)&data.resv, 32, index);

            }
            
            *offset1 = offset;
            *deslength = sizeof(OOP_ROUTEMODULE_T);
            memcpy(desbuffer, (uint8 *)&data, sizeof(OOP_ROUTEMODULE_T));
            
            break;
        }

        
        case E_OOP_SWITCH_OUT_T:
        {
            OOP_SWITCHOUT_T data;
            uint16 offset=0;
            uint16 i=0;
            memset((uint8*)&data,0x00,sizeof(OOP_SWITCHOUT_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                data.nNum = soucerbuffer[offset++];

                for(i=0;i<data.nNum;i++)
                {
                   offset+=2;
                   memset(tmpBuff,0x00,30);
                   data.value[i].name.nNum = soucerbuffer[offset+1];
                   taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                   memcpy( data.value[i].name.value,tmpBuff,data.value[i].name.nNum);
                   offset++;
                   data.value[i].current = soucerbuffer[offset++];
                   offset++;
                   data.value[i].attribute = soucerbuffer[offset++];
                   offset++;
                   data.value[i].input = soucerbuffer[offset++];
                   set_bit_value((uint8*)&data.rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_SWITCHOUT_T);
            memcpy(desbuffer,(uint8*)&data,sizeof(OOP_SWITCHOUT_T));
        }
        break;
        case E_OOP_IR_T:
        {
            OOP_IR_T ir;
            uint16 offset=0;
            uint16 i=0;
            memset((uint8*)&ir,0x00,sizeof(OOP_IR_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                ir.nNum = soucerbuffer[offset++];

                for(i=0;i<ir.nNum;i++)
                {
                   offset+=2;
                   memset(tmpBuff,0x00,30);
                   ir.ir[i].name.nNum=soucerbuffer[offset+1];
                   taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                   memcpy(ir.ir[i].name.value,tmpBuff,ir.ir[i].name.nNum);
                   offset++;
                   ir.ir[i].param.baud =  soucerbuffer[offset++];
                   ir.ir[i].param.parity =  soucerbuffer[offset++];
                   ir.ir[i].param.databits =  soucerbuffer[offset++];
                   ir.ir[i].param.stopbits =  soucerbuffer[offset++];
                   ir.ir[i].param.flowctrl =  soucerbuffer[offset++];
                   set_bit_value((uint8*)&ir.rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_IR_T);
            memcpy(desbuffer,(uint8*)&ir,sizeof(OOP_IR_T));
        }
        break;
        case E_OOP_LCD_T:
        {
            OOP_LCD_T lcd = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.disTime, tmpBuff, 1);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.lightTime, tmpBuff, 2);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.reTime, tmpBuff, 2);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.noPow, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.eDotNum, tmpBuff, 1);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.pDotNum, tmpBuff, 1);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&lcd.nFlag, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_LCD_T);
            memcpy(desbuffer, (uint8 *)&lcd, sizeof(OOP_LCD_T));
            
            break;
        }
        case E_OOP_OBJ_SHOW_T:
        {
            OOP_OBJ_SHOW_T obj_show;
            uint16 offset=0;
            uint16 i=0,j;
            uint16 num,suboadnum;
            OOP_CSD_T csd;
            uint8 seq;
            memset((uint8*)&obj_show,0x00,sizeof(OOP_OBJ_SHOW_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                num = soucerbuffer[offset++];
                obj_show.nNum = MIN(num,MAX_OBJ_SHOW_NUM);
                for(i=0;i<num;i++)
                {
                    memset(&csd,0x00,sizeof(csd));
                    offset+=3;
                    //csd为0ad
                    if(soucerbuffer[offset++] == 0)
                    {
                        csd.choice = 0;
                        memcpy_r(&csd.oad.value,&soucerbuffer[offset],4);
                        offset+=4;
                    }else
                    {
                        //csd为road
                        csd.choice = 1;
                        memcpy_r(&csd.road.oadMain.value ,&soucerbuffer[offset],4);
                        offset+=4;
                        suboadnum = soucerbuffer[offset++];
                        csd.road.oadCols.nNum = suboadnum;
                        for(j=0;j<suboadnum;j++)
                        {
                            memcpy_r(&csd.road.oadCols.oad[j].value ,&soucerbuffer[offset],4);
                            offset+=4;
                        }
                    }
                    offset++;
                    seq = soucerbuffer[offset++];
                    if(i>=MAX_OBJ_SHOW_NUM)
                        continue;
                    memcpy(&obj_show.objs[i].cols,&csd,sizeof(OOP_CSD_T));
                    obj_show.objs[i].nNum = seq;
                    set_bit_value((uint8*)&obj_show.rsv, 32, i);

//                
//                   if(i>=MAX_OBJ_SHOW_NUM)
//                   {
//                        offset+=10;
//                        continue;
//                   }
//                   offset+=4;
//                   //memset(tmpBuff,0x00,30);
//                   //taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
//                   //memcpy_r((uint8 *)&obj_show.objs[i].cols.oad.value,tmpBuff,4); 
//                   //taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
//                   //memcpy_r((uint8 *)&obj_show.objs[i].nNum,tmpBuff,1); 
//                   memcpy_r((uint8 *)&obj_show.objs[i].cols.oad.value,&soucerbuffer[offset],4); 
//                   offset+=5;
//                   memcpy_r((uint8 *)&obj_show.objs[i].nNum,&soucerbuffer[offset++],1); 
//                   set_bit_value((uint8*)&obj_show.rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_OBJ_SHOW_T);
            memcpy(desbuffer,(uint8*)&obj_show,sizeof(OOP_OBJ_SHOW_T));
        }
        break;
        case E_OOP_OBJ_SHOW_PARAM_T:
        {
            OOP_OBJ_SHOW_PARAM_T data;
            uint16 offset = 0;
            memset(&data,0,sizeof(OOP_OBJ_SHOW_PARAM_T));
            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy(&data.objNumAll, tmpBuff, 1);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy(&data.objNumMax, tmpBuff, 1);

            *offset1 = offset;
            *deslength = sizeof(OOP_OBJ_SHOW_PARAM_T);
            memcpy(desbuffer, (uint8 *)&data, sizeof(OOP_OBJ_SHOW_PARAM_T));
            
            break;
        }
        case E_OOP_LOAD_I0_EVECFG_T:
        {
            OOP_LOAD_I0_EVECFG_T evtcfg_i0 = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_i0.nADown, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_i0.wValue, tmpBuff, 2);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_i0.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_LOAD_I0_EVECFG_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_i0, sizeof(OOP_LOAD_I0_EVECFG_T));
            
            break;
        }
        case E_OOP_EVTCFG_DD_T:
        {
            OOP_EVTCFG_DD_T evtcfg_dd = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_dd.dwValue, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_dd.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_DD_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_dd, sizeof(OOP_EVTCFG_DD_T));
            
            break;
        }
        case E_OOP_EVTCFG_3000_T:
        {
            OOP_EVTCFG_3000_T evtcfg_3000 = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3000.wVUp, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3000.wVDown, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3000.nADown, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3000.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_3000_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_3000, sizeof(OOP_EVTCFG_3000_T));
            
            break;
        }
        case E_OOP_EVTCFG_3006_T:
        {
            OOP_EVTCFG_3006_T evtcfg_3006 = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3006.wVDown, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3006.nAUp, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_3006.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_3006_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_3006, sizeof(OOP_EVTCFG_3006_T));
            
            break;
        }
        case E_OOP_EVTCFG_WD_T:
        {
            OOP_EVTCFG_WD_T evtcfg_wd = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_wd.wValue, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg_wd.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_WD_T);
            memcpy(desbuffer, (uint8 *)&evtcfg_wd, sizeof(OOP_EVTCFG_WD_T));
            
            break;
        }
        case E_OOP_VOLTPARAM_T:
        {
            OOP_VOLTPARAM_T voltparam = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&voltparam.uupVolt, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&voltparam.lldVolt, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&voltparam.upVolt, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&voltparam.ldVolt, tmpBuff, 2);
            *offset1 = offset;
            *deslength = sizeof(OOP_VOLTPARAM_T);
            memcpy(desbuffer, (uint8 *)&voltparam, sizeof(OOP_VOLTPARAM_T));
            
            break;
        }
        case E_OOP_PAYDAYS_T:
        {
            OOP_PAYDAYS_T paydays;
            uint16 offset=0;
            uint16 i=0;


            memset((uint8*)&paydays,0x00,sizeof(OOP_PAYDAYS_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                paydays.nNum = soucerbuffer[offset++];

                for(i=0;i<paydays.nNum;i++)
                {
                   memset(tmpBuff,0x00,30);
                   taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                   memcpy_r((uint8 *)&paydays.cols[i].day,tmpBuff,1);  
                   taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                   memcpy_r((uint8 *)&paydays.cols[i].hour,tmpBuff,1);
                   set_bit_value((uint8*)&paydays.rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_PAYDAYS_T);
            memcpy(desbuffer,(uint8*)&paydays,sizeof(OOP_PAYDAYS_T));
        }
        break;
        case E_OOP_CFG_LU_T:
        {
            OOP_CFG_LU_T evtcfg = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg.nShort, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg.nByte, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_CFG_LU_T);
            memcpy(desbuffer, (uint8 *)&evtcfg, sizeof(OOP_CFG_LU_T));
            
            break;
        }
        case E_OOP_EVTCFG_ID_T:
        {
            OOP_EVTCFG_ID_T evtcfg = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg.nValue, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&evtcfg.nDelay, tmpBuff, 1);
            *offset1 = offset;
            *deslength = sizeof(OOP_EVTCFG_ID_T);
            memcpy(desbuffer, (uint8 *)&evtcfg, sizeof(OOP_EVTCFG_ID_T));
            
            break;
        }
        case E_OOP_CHANNEL_T:
        {
            OOP_CHANNEL_T chanel;
            uint16 offset=0;
            uint16 i=0;


            memset((uint8*)&chanel,0x00,sizeof(OOP_CHANNEL_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                chanel.nNum = soucerbuffer[offset++];

                for(i=0;i<chanel.nNum;i++)
                {
                   memset(tmpBuff,0x00,30);
                   taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                   memcpy_r((uint8 *)&chanel.port[i].value,tmpBuff,4);      
                   set_bit_value((uint8*)&chanel.rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_CHANNEL_T);
            memcpy(desbuffer,(uint8*)&chanel,sizeof(OOP_CHANNEL_T));
        }
        break;
        case E_OOP_DAYPERIOD_E_T:
        {
            OOP_DAYPERIOD_T DAYPERIOD;
            uint16 offset=0;
            uint16 i=0;

            memset((uint8*)&DAYPERIOD,0x00,sizeof(OOP_DAYPERIOD_T));

             if(soucerbuffer[offset++]==DT_ARRAY)
             {
                DAYPERIOD.dPeriod[index].nNum = soucerbuffer[offset++];

                for(i=0;i<DAYPERIOD.dPeriod[index].nNum;i++)
                {
                   offset+=3;
                   memset(tmpBuff,0x00,30);
                   DAYPERIOD.dPeriod[index].period[i].hour = soucerbuffer[offset++];
                   offset++;
                   DAYPERIOD.dPeriod[index].period[i].min = soucerbuffer[offset++];
                   offset++;
                   DAYPERIOD.dPeriod[index].period[i].rateNO = soucerbuffer[offset++];
                   set_bit_value((uint8*)&DAYPERIOD.dPeriod[index].rsv, 32, i);
                }
             }
            *offset1 = offset;
            *deslength = sizeof(OOP_DAYPERIOD_T);
            memcpy(desbuffer,(uint8*)&DAYPERIOD,sizeof(OOP_DAYPERIOD_T));
        }
        break;
        case E_OOP_PROTOCOL_T:
        {
            OOP_PROTOCOL_T protocol;
            i=0;
            uint16 offset=0;
            memset((uint8 *)&protocol,0xff,sizeof(OOP_PROTOCOL_T));
            protocol.nNum = soucerbuffer[1];
            protocol.rsv=0;
            if(soucerbuffer[offset++]==DT_ARRAY)
            {
                protocol.nNum = soucerbuffer[offset++];
                for(i=0;i<protocol.nNum;i++)
                {
                    memset(tmpBuff,0x00,30);
                    protocol.value[i].nNum = soucerbuffer[offset+1];
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy((uint8 *)&protocol.value[i].value,tmpBuff,protocol.value[i].nNum);      
                    set_bit_value((uint8*)&protocol.rsv, 32, i);
                }
            
            }
           
            *offset1 = offset;
            *deslength=sizeof(OOP_PROTOCOL_T);
            memcpy(desbuffer,(uint8 *)&protocol,sizeof(OOP_PROTOCOL_T));
        }
        break;
        case E_OOP_ASSOCIATION_CONTEXT_T:
        {
            OOP_ASSOCIATION_CONTEXT_T association_context = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&association_context.VER, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&association_context.nRecvSize, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&association_context.nSendSize, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&association_context.nMaxPDUSize, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_bitrev((uint8 *)association_context.PCB, tmpBuff, 8);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_bitrev((uint8 *)association_context.FCB, tmpBuff, 16);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&association_context.nTimeout, tmpBuff, 4);
            *offset1 = offset;
            *deslength = sizeof(OOP_ASSOCIATION_CONTEXT_T);
            memcpy(desbuffer, (uint8 *)&association_context, sizeof(OOP_ASSOCIATION_CONTEXT_T));
            
            break;
        }
        case E_OOP_BROADCASTCLOCK_T:
        {
            OOP_BROADCASTCLOCK_T broadcastclock = {0};
            uint16 offset = 0;

            memset(tmpBuff, 0x00, 30);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&broadcastclock.nMinDeviate, tmpBuff, 2);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&broadcastclock.nMaxDeviate, tmpBuff, 2);
            *offset1 = offset;
            *deslength = sizeof(OOP_BROADCASTCLOCK_T);
            memcpy(desbuffer, (uint8 *)&broadcastclock, sizeof(OOP_BROADCASTCLOCK_T));
            
            break;
        }
        case E_OOP_ENERGY_T://普通精度电量
        {
            OOP_ENERGY_T ENERGY_T;
            i=0;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {
                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                
                    if(TRUE==taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE))
                    {
                        memcpy_r((uint8 *)&ENERGY_T.nValue[i],tmpBuff,4);  
                        set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);
                        
                    }
                    i++;
                }
            
            }
            else
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&ENERGY_T.nValue[index],tmpBuff,4);                       
                set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_ENERGY_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_ENERGY_T));
        }
    break;

    case E_OOP_ENERGYS_T://普通精度电量
        {
            OOP_ENERGYS_T ENERGYS_T;
            i=0;
            memset((uint8 *)&ENERGYS_T,0xff,sizeof(OOP_ENERGYS_T));
            ENERGYS_T.nNum=OOP_FULL_RATES;
            ENERGYS_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {
                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&ENERGYS_T.nValue[i],tmpBuff,4);      
                    set_bit_value((uint8*)&ENERGYS_T.rsv, 32, i);
                    i++;
                }
            
            }
            else
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&ENERGYS_T.nValue[index],tmpBuff,4); 
                set_bit_value((uint8*)&ENERGYS_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_ENERGYS_T);
            memcpy(desbuffer,(uint8 *)&ENERGYS_T,sizeof(OOP_ENERGYS_T));
        }
        break;

    case E_OOP_HENERGY_T://高通精度电量不带符号
        {
            OOP_HENERGY_T HENERGY_T;
            i=0;
            memset((uint8 *)&HENERGY_T,0xff,sizeof(OOP_HENERGY_T));
            HENERGY_T.nNum=OOP_FULL_RATES;
            HENERGY_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {

                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    //(*(int64*)tmpBuff) *= 100;
                    memcpy_r((uint8*)&HENERGY_T.nValue[i],tmpBuff,8);
                    set_bit_value((uint8*)&HENERGY_T.rsv, 32, i);
                    i++;
                }
            }
            else
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&HENERGY_T.nValue[index],tmpBuff,8); 
                set_bit_value((uint8*)&HENERGY_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_HENERGY_T);
            memcpy(desbuffer,(uint8 *)&HENERGY_T,sizeof(OOP_HENERGY_T));
        }
        break;

    case E_OOP_HENERGYS_T://高通精度电量带符号
        {
            OOP_HENERGYS_T HENERGYS_T;
            i=0;
            memset((uint8 *)&HENERGYS_T,0xff,sizeof(OOP_HENERGYS_T));
            HENERGYS_T.nNum=OOP_FULL_RATES;
            HENERGYS_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {
                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    //(*(int64*)tmpBuff) *= 100;
                    memcpy_r((uint8*)&HENERGYS_T.nValue[i],tmpBuff,8);
                    set_bit_value((uint8*)&HENERGYS_T.rsv, 32, i);
                    i++;
                }
            }
            else
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&HENERGYS_T.nValue[index],tmpBuff,8); 
                set_bit_value((uint8*)&HENERGYS_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_HENERGYS_T);
            memcpy(desbuffer,(uint8 *)&HENERGYS_T,sizeof(OOP_HENERGYS_T));
        }
    break;

    case E_OOP_DEMAND_T:  //需量
    case E_OOP_DEMANDS_T:
    {
        OOP_DEMAND_T DEMAND_T;
        i=0;
        memset((uint8 *)&DEMAND_T,0xff,sizeof(OOP_DEMAND_T));
        DEMAND_T.nNum=OOP_FULL_RATES;
        DEMAND_T.rsv=0;
        //uint8 tmpTime[7];
        //DateTimeHex_t DefaultTime={0x07,0xD0,0x01,0x01,0x00,0x00,0x00};//默认时间为2000-01-01 00:00:00
        if(soucerbuffer[0]==DT_ARRAY)
        {
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&DEMAND_T.demand[i].nValue,tmpBuff,4);
                if(FALSE == DT_DateTimeHexIsValid((DateTimeHex_t*)(soucerbuffer+(*offset1)+1)))
                {
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    i++;
                    continue;
                }
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&DEMAND_T.demand[i].time.year,tmpBuff,2);
                memcpy((uint8 *)&DEMAND_T.demand[i].time.month,tmpBuff+2,6);
                set_bit_value((uint8*)&DEMAND_T.rsv, 32,i);
                i++;
            }
        }
        else
        {

            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DEMAND_T.demand[index].nValue,tmpBuff,4);
            if(TRUE == DT_DateTimeHexIsValid((DateTimeHex_t*)(soucerbuffer+(*offset1)+1)))
            {
                set_bit_value((uint8*)&DEMAND_T.rsv, 32,index);
            }
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DEMAND_T.demand[index].time.year,tmpBuff,2);
            memcpy((uint8 *)&DEMAND_T.demand[index].time.month,tmpBuff+2,6);
            
        }
        *deslength=sizeof(OOP_DEMAND_T);
        memcpy(desbuffer,(uint8 *)&DEMAND_T,sizeof(OOP_DEMAND_T ));
    }
    break;

    case E_OOP_VOLTSTAT_T://电压合格率
    {
        OOP_VOLTSTAT_T VOLTSTAT;
        memset(tmpBuff,0x00,30);
        memset((uint8 *)&VOLTSTAT,0x00,sizeof(OOP_VOLTSTAT_T));
 
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.day.dwTime1,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.day.wRate1,tmpBuff,2);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.day.wRate2,tmpBuff,2);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.day.dwTime3,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.day.dwTime4,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.mon.dwTime1,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.mon.wRate1,tmpBuff,2);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.mon.wRate2,tmpBuff,2);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.mon.dwTime3,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTSTAT.mon.dwTime4,tmpBuff,4);
        memcpy(desbuffer,(uint8 *)&VOLTSTAT,sizeof(OOP_VOLTSTAT_T));
        *deslength=sizeof(OOP_VOLTSTAT_T);
    }

    break;
    case E_OOP_VOLTQR_T://电压合格率
    {
        OOP_VOLTQR_T VOLTQR;
        memset(tmpBuff,0x00,30);
        memset((uint8 *)&VOLTQR,0x00,sizeof(VOLTQR));
        
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTQR.dwTime1,tmpBuff,4);
       
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8 *)&VOLTQR.wRate1,tmpBuff,2);
        
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
      
        memcpy_r((uint8 *)&VOLTQR.wRate2,tmpBuff,2);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
       
        memcpy_r((uint8 *)&VOLTQR.dwTime3,tmpBuff,4);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
      
        memcpy_r((uint8 *)&VOLTQR.dwTime4,tmpBuff,4);
        
        memcpy(desbuffer,(uint8 *)&VOLTQR,sizeof(VOLTQR));
        *deslength=sizeof(VOLTQR);
    }

    break;
    case E_OOP_DWORD3V_T://分相变量

    {
        OOP_DWORD3V_T DWORD3V_T;
        i=0;
        memset((uint8 *)&DWORD3V_T,0xff,sizeof(OOP_DWORD3V_T));
        if(pwrType == 1)
        {
            DWORD3V_T.nNum=1;
        }
        else
        {
            DWORD3V_T.nNum=3;
        }
        DWORD3V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            temlen=getDataTypeLength(soucerbuffer[2]);
            if(temlen>4)
            {
                temlen=4;
            }
            if(soucerbuffer[1]==1)
                DWORD3V_T.nNum=1;
            if(soucerbuffer[1]==3)
                DWORD3V_T.nNum=3;
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&DWORD3V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&DWORD3V_T.rsv, 32, i);
                i++;

            }
            
        }
        else
        {
            temlen=getDataTypeLength(soucerbuffer[0]);
            if(temlen>4)
            {
                temlen=4;
            }
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DWORD3V_T.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&DWORD3V_T.rsv, 32,index);
        }
        *deslength=sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,(uint8 *)&DWORD3V_T,sizeof(OOP_DWORD3V_T));

    }

    break;
    case E_OOP_WORD3V_T://电压

    {
        OOP_WORD3V_T WORD3V_T;
        i=0;
        memset((uint8 *)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));
        if(pwrType == 1)
        {
            WORD3V_T.nNum=1;
        }
        else
        {
             WORD3V_T.nNum=3;
        }
        WORD3V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            if(soucerbuffer[1]==3)
                WORD3V_T.nNum=3;
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&WORD3V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&WORD3V_T.rsv, 32, i);
                i++;
            }
            
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&WORD3V_T.nValue[index],tmpBuff,2);
            set_bit_value((uint8*)&WORD3V_T.rsv, 32,index);
        }
        *deslength=sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,(uint8 *)&WORD3V_T,sizeof(OOP_WORD3V_T));

    }

    break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;
        i=0;
        memset((uint8 *)&INT3V_T,0xff,sizeof(OOP_INT3V_T));
        if(pwrType == 1)
        {
            INT3V_T.nNum=1;
        }
        else
        {
            INT3V_T.nNum=3;
        }
        INT3V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            if(soucerbuffer[1]==3)
                INT3V_T.nNum=3;
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&INT3V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&INT3V_T.rsv, 32, i);
                i++;

            }
            
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&INT3V_T.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&INT3V_T.rsv, 32, index);
        }
        *deslength=sizeof(OOP_INT3V_T);
        memcpy(desbuffer,(uint8 *)&INT3V_T,sizeof(OOP_INT3V_T));
    }
    break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T  INT4V_T;
        i=0;
        memset((uint8 *)&INT4V_T,0xff,sizeof(OOP_INT4V_T));
        if(pwrType == 1)
        {
            INT4V_T.nNum=2;
        }
        else
        {
            INT4V_T.nNum=4;
        }
        INT4V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            if(soucerbuffer[1]==4)
                INT4V_T.nNum=4;
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&INT4V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&INT4V_T.rsv, 32, i);
                i++;

            }
            
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&INT4V_T.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&INT4V_T.rsv, 32, index);
        }
        *deslength=sizeof(OOP_INT4V_T);
        memcpy(desbuffer,(uint8 *)&INT4V_T,sizeof(OOP_INT4V_T));
    }
    break;
    case E_DATETIME_S://时间
    {
        OOP_DATETIME_S_T DATETIME_S_T;

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpBuff,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpBuff+2,6);
        if(DATETIME_S_T.second == 0xff)
            DATETIME_S_T.second =0;
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));   
        *deslength=sizeof(OOP_DATETIME_S_T);
    }
    break;
    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
    {
        temlen=getDataTypeLength(soucerbuffer[0]);
        if(temlen>4)
        {
         temlen=4;
        }
        memset(tmpBuff,0x00,30);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
        memcpy_r(desbuffer,tmpBuff,4);    
        *deslength=4;
    }
    break;
    case E_LONG_UNSIGNED:
    case E_LONG:
    {
        temlen=getDataTypeLength(soucerbuffer[0]);
        if(temlen>2)
        {
         temlen=2;
        }
        memset(tmpBuff,0x00,30);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+2-temlen,NULL_DATA,FALSE);
        memcpy_r(desbuffer,tmpBuff,2);    
        *deslength=2;
    }
    break;
    case E_UNSIGNED:
    case E_ENUM:
    {
        temlen=getDataTypeLength(soucerbuffer[0]);
        if(temlen>1)
        {
         temlen=1;
        }
        memset(tmpBuff,0x00,30);
        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+1-temlen,NULL_DATA,FALSE);
        memcpy_r(desbuffer,tmpBuff,1);    
        *deslength=1;
    }
    break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T METWORD_T;
        i=0;
        memset((uint8*)&METWORD_T,0xff,sizeof(OOP_METWORD_T));
        METWORD_T.nNum=OOP_METWORD_NUM;
        METWORD_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_bitrev((uint8 *)&METWORD_T.metWord[i],tmpBuff,2);
                set_bit_value((uint8*)&METWORD_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_bitrev((uint8 *)&METWORD_T.metWord[index],tmpBuff,2);
            set_bit_value((uint8*)&METWORD_T.rsv, 32, index);
        }
        *deslength=sizeof(OOP_METWORD_T);
        memcpy(desbuffer,(uint8 *)&METWORD_T,sizeof(OOP_METWORD_T));

    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        i=0;
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));
        if(pwrType == 1)
        {
            LONG4V_T.nNum=2;
        }
        else
        {
            LONG4V_T.nNum=4;
        }
        LONG4V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            if(soucerbuffer[1] == 4)
                LONG4V_T.nNum=4;
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&LONG4V_T.nValue[i],tmpBuff,2);
                set_bit_value((uint8*)&LONG4V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {

            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&LONG4V_T.nValue[index],tmpBuff,2);
            set_bit_value((uint8*)&LONG4V_T.rsv, 32, index);
        }
        *deslength=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
        
    break;
    case E_OOP_EVTSTAT_T:
    {
        uint16 offset=0,i=0;
        OOP_EVTSTAT_T evtStat;
        uint8 offlen = 0; //数组数量长度
        uint8 num;
        memset((uint8*)&evtStat,0x00,sizeof(OOP_EVTSTAT_T));
        //evtStat.nNum = OOP_MAX_EVTSTAT;
        //抄的总

        if(soucerbuffer[0] == DT_ARRAY)
        {
            offset++;
            num = get_len_offset(&soucerbuffer[offset], &offlen);
            offset+=offlen;
            evtStat.nNum = num;
            for(i = 0;i<num && i< OOP_MAX_EVTSTAT;i++)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                    evtStat.stat[i].source.choice = soucerbuffer[offset++];
                    switch(evtStat.stat[i].source.choice)
                    {
                        case DT_OAD: 
                        {
                            memcpy(&evtStat.stat[i].source.oad.value, soucerbuffer+offset, sizeof(OOP_OAD_U));
                            offset=offset+sizeof(OOP_OAD_U);
                        
                        }break;

                        case DT_TSA: 
                        {
                            uint8 datalen=0;
                            datalen=soucerbuffer[offset++];
                            memcpy((uint8*)&evtStat.stat[i].source.add,soucerbuffer+offset,datalen);
                            offset=offset+datalen;
                        }break;  

                        case DT_OI: 
                        {
                            memcpy((uint8*)&evtStat.stat[i].source.oi,soucerbuffer+offset,sizeof(OI));
                            offset+=2;
                        }break;   

                        case DT_UNSIGNED: 
                        {
                            evtStat.stat[i].source.nValue=soucerbuffer[offset++];
                        }break; 

                        case DT_NULL: 
                            break;  //为空，没东西需要转换

                        default: 
                            break;
                    }
                    offset++;//结构体类型
                    offset++;//成员数量
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&evtStat.stat[i].dwCount,tmpBuff,4);
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&evtStat.stat[i].dwTimes,tmpBuff,4);
                   
                    set_bit_value((uint8*)&evtStat.index, 32, i);
                }
            }
        }
        else if(soucerbuffer[0] == DT_STRUCTURE)
        {
            //抄的分
            offset++;
            offset++;
            evtStat.nNum = 1;
            evtStat.stat[index].source.choice = soucerbuffer[offset++];
            switch(evtStat.stat[index].source.choice)
            {
                case DT_OAD: 
                {
                    memcpy(&evtStat.stat[index].source.oad.value, soucerbuffer+offset, sizeof(OOP_OAD_U));
                    offset=offset+sizeof(OOP_OAD_U);
                
                }break;

                case DT_TSA: 
                {
                    uint8 datalen=0;
                    datalen=soucerbuffer[offset++];
                    memcpy((uint8*)&evtStat.stat[index].source.add,soucerbuffer+offset,datalen);
                    offset=offset+datalen;
                }break;  

                case DT_OI: 
                {
                    memcpy((uint8*)&evtStat.stat[index].source.oi,soucerbuffer+offset,sizeof(OI));
                    offset+=2;
                }break;   

                case DT_UNSIGNED: 
                {
                    evtStat.stat[index].source.nValue=soucerbuffer[offset++];
                }break; 

                case DT_NULL: 
                    break;  //为空，没东西需要转换

                default: 
                    break;
            }
            offset++;//结构体类型
            offset++;//成员数量

            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&evtStat.stat[index].dwCount,tmpBuff,4);

            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&evtStat.stat[index].dwTimes,tmpBuff,4);

            set_bit_value((uint8*)&evtStat.index, 32, index);
        }

        *offset1 = offset;
        *deslength=sizeof(OOP_EVTSTAT_T);
        memcpy(desbuffer,(uint8 *)&evtStat,sizeof(OOP_EVTSTAT_T));
       
    }
    break;
    case E_OOP_EVTSTAT1_T:
    {
        uint16 offset=0,i=0;
        
        OOP_MEVTSTAT_T evtStat;
        uint8 offlen = 0; //数组数量长度
        uint16 num;
        memset((uint8*)&evtStat,0x00,sizeof(OOP_MEVTSTAT_T));
       // evtStat.nNum = OOP_MAX_EVTSTAT;
         if(soucerbuffer[0] == DT_ARRAY)
        {
            offset++;
            num = get_len_offset(&soucerbuffer[offset], &offlen);
            offset+=offlen;
            evtStat.nNum = num;
            for(i=0;i<num && i<OOP_MAX_EVTSTAT;i++)
            {
                if(soucerbuffer[offset++] == DT_STRUCTURE)
                {
                    offset++;
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&evtStat.stat[i].dwCount,tmpBuff,4);
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&evtStat.stat[i].dwTimes,tmpBuff,4);
                   
                    set_bit_value((uint8*)&evtStat.index, 32, i);
                }
            }
        }
        else if(soucerbuffer[0] == DT_STRUCTURE)
        {
            //抄的分
            evtStat.nNum =1;
            offset++;
            offset++;
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&evtStat.stat[index].dwCount,tmpBuff,4);
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&evtStat.stat[index].dwTimes,tmpBuff,4);
            
            set_bit_value((uint8*)&evtStat.index, 32, index);
        }
		*offset1 = offset;
        *deslength=sizeof(OOP_MEVTSTAT_T);
        memcpy(desbuffer,(uint8 *)&evtStat,sizeof(OOP_MEVTSTAT_T));
    }
    break;
    case E_OOP_EVENTTIME_T:
    {
        OOP_EVTTIME_T eventtime;
        memset(&eventtime,0x00,sizeof(OOP_EVTTIME_T));
        uint16 offset=0,i=0;
        uint8 offlen = 0; //数组数量长度
        uint8 num;

        //eventtime.nNum = OOP_MAX_EVTSTAT;
        //抄的总
        if(soucerbuffer[0] == DT_ARRAY)
        {
            offset++;
            num = get_len_offset(&soucerbuffer[offset], &offlen);
            offset+=offlen;
            eventtime.nNum = num;
            for(i = 0;i<num && i< OOP_MAX_EVTSTAT;i++)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                    eventtime.stat[i].source.choice = soucerbuffer[offset++];
                    switch(eventtime.stat[i].source.choice)
                    {
                        case DT_OAD: 
                        {
                            memcpy(&eventtime.stat[i].source.oad.value, soucerbuffer+offset, sizeof(OOP_OAD_U));
                            offset=offset+sizeof(OOP_OAD_U);
                        
                        }break;

                        case DT_TSA: 
                        {
                            uint8 datalen=0;
                            datalen=soucerbuffer[offset++];
                            memcpy((uint8*)&eventtime.stat[i].source.add,soucerbuffer+offset,datalen);
                            offset=offset+datalen;
                        }break;  

                        case DT_OI: 
                        {
                            memcpy((uint8*)&eventtime.stat[i].source.oi,soucerbuffer+offset,sizeof(OI));
                            offset+=2;
                        }break;   

                        case DT_UNSIGNED: 
                        {
                            eventtime.stat[i].source.nValue=soucerbuffer[offset++];
                        }break; 

                        case DT_NULL: 
                            break;  //为空，没东西需要转换

                        default: 
                            break;
                    }
                    offset++;//结构体类型
                    offset++;//成员数量
                    memset(tmpBuff,0x00,sizeof(tmpBuff));
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&eventtime.stat[i].tmStart.year,tmpBuff,2);
                    memcpy(&eventtime.stat[i].tmStart.month,tmpBuff+2,6);

                    memset(tmpBuff,0x00,sizeof(tmpBuff));
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&eventtime.stat[i].tmEnd.year,tmpBuff,2);
                    memcpy(&eventtime.stat[i].tmEnd.month,tmpBuff+2,6);

                    set_bit_value((uint8*)&eventtime.index, 32, i);
                }
            }
        }
        else if(soucerbuffer[0] == DT_STRUCTURE)
        {
            //抄的分
            eventtime.nNum = 1;
            offset++;
            offset++;
            eventtime.stat[index].source.choice = soucerbuffer[offset++];
            switch(eventtime.stat[index].source.choice)
            {
                case DT_OAD: 
                {
                    memcpy(&eventtime.stat[index].source.oad.value, soucerbuffer+offset, sizeof(OOP_OAD_U));
                    offset=offset+sizeof(OOP_OAD_U);
                
                }break;

                case DT_TSA: 
                {
                    uint8 datalen=0;
                    datalen=soucerbuffer[offset++];
                    memcpy((uint8*)&eventtime.stat[index].source.add,soucerbuffer+offset,datalen);
                    offset=offset+datalen;
                }break;  

                case DT_OI: 
                {
                    memcpy((uint8*)&eventtime.stat[index].source.oi,soucerbuffer+offset,sizeof(OI));
                    offset+=2;
                }break;   

                case DT_UNSIGNED: 
                {
                    eventtime.stat[index].source.nValue=soucerbuffer[offset++];
                }break; 

                case DT_NULL: 
                    break;  //为空，没东西需要转换

                default: 
                    break;
            }
            offset++;//结构体类型
            offset++;//成员数量

            memset(tmpBuff,0x00,sizeof(tmpBuff));
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);

            memcpy_r(&eventtime.stat[index].tmStart.year,tmpBuff,2);
            memcpy(&eventtime.stat[index].tmStart.month,tmpBuff+2,6);
            
            memset(tmpBuff,0x00,sizeof(tmpBuff));
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&eventtime.stat[index].tmEnd.year,tmpBuff,2);
            memcpy(&eventtime.stat[index].tmEnd.month,tmpBuff+2,6);

            set_bit_value((uint8*)&eventtime.index, 32, index);

        }

        *offset1 = offset;
        *deslength=sizeof(OOP_EVTTIME_T);
        memcpy(desbuffer,(uint8 *)&eventtime,sizeof(OOP_EVTTIME_T));
        break;

    }
    case E_OOP_EVENTTIME1_T:
    {
        OOP_MEVTTIME_T eventtime;
        memset(&eventtime,0x00,sizeof(OOP_MEVTTIME_T));
        uint16 offset=0,i=0;
        uint8 offlen = 0; //数组数量长度
        uint8 num;

        //eventtime.nNum = OOP_MAX_EVTSTAT;
        //抄的总
        if(soucerbuffer[0] == DT_ARRAY)
        {
            offset++;
            num = get_len_offset(&soucerbuffer[offset], &offlen);
            offset+=offlen;
            eventtime.nNum = num;
            for(i = 0;i<num && i< OOP_MAX_EVTSTAT;i++)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;//成员数量
                    memset(tmpBuff,0x00,sizeof(tmpBuff));
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&eventtime.stat[i].tmStart.year,tmpBuff,2);
                    memcpy(&eventtime.stat[i].tmStart.month,tmpBuff+2,6);

                    memset(tmpBuff,0x00,sizeof(tmpBuff));
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r(&eventtime.stat[i].tmEnd.year,tmpBuff,2);
                    memcpy(&eventtime.stat[i].tmEnd.month,tmpBuff+2,6);

                    set_bit_value((uint8*)&eventtime.index, 32, i);
                }
            }
        }
        else if(soucerbuffer[0] == DT_STRUCTURE)
        {
            //抄的分
            eventtime.nNum = 1;
            offset++;
            offset++;//成员数量
            memset(tmpBuff,0x00,sizeof(tmpBuff));
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&eventtime.stat[index].tmStart.year,tmpBuff,2);
            memcpy(&eventtime.stat[index].tmStart.month,tmpBuff+2,6);
            
            memset(tmpBuff,0x00,sizeof(tmpBuff));
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r(&eventtime.stat[index].tmEnd.year,tmpBuff,2);
            memcpy(&eventtime.stat[index].tmEnd.month,tmpBuff+2,6);
            
            set_bit_value((uint8*)&eventtime.index, 32, index);

        }

        *offset1 = offset;
        *deslength=sizeof(OOP_MEVTTIME_T);
        memcpy(desbuffer,(uint8 *)&eventtime,sizeof(OOP_MEVTTIME_T));
        break;

    }
    case E_OOP_BITSTR8_T:
    {
        OOP_BITSTR8_T BITSTR8;
   
        memset((uint8*)&BITSTR8,0x00,sizeof(OOP_BITSTR8_T));

        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_bitrev(&BITSTR8.nValue,tmpBuff,1);
        //BITSTR8.nValue=tmpBuff[0];
        *deslength=sizeof(OOP_BITSTR8_T);
        memcpy(desbuffer,(uint8 *)&BITSTR8,sizeof(OOP_BITSTR8_T));

    }       
    break;
    case E_OOP_BITSTR32_T:
    {
        OOP_BITSTR32_T BITSTR32;
   
        memset((uint8*)&BITSTR32,0x00,sizeof(BITSTR32));

        taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
        memcpy_bitrev(BITSTR32.nValue,tmpBuff,4);
        //memcpy(BITSTR32.nValue,tmpBuff,sizeof(BITSTR32));
        *deslength=sizeof(BITSTR32);
        memcpy(desbuffer,BITSTR32.nValue,sizeof(BITSTR32));

    }       
    break;
    case E_OOP_VISIBLEVAR32_T:
    {
        OOP_VISIBLEVAR32_T VISIBLEVAR32;
        uint8 offset=0;
   
        memset((uint8*)&VISIBLEVAR32,0x00,sizeof(OOP_VISIBLEVAR32_T));
        VISIBLEVAR32.nNum=get_len_offset(soucerbuffer+1, &offset);
        memcpy(VISIBLEVAR32.value,soucerbuffer+1+offset,VISIBLEVAR32.nNum);
        
        *deslength=sizeof(OOP_VISIBLEVAR32_T);
        memcpy(desbuffer,(uint8 *)&VISIBLEVAR32,sizeof(OOP_VISIBLEVAR32_T));

    }       
    break;
    case E_OOP_DWORD2_T:
    case E_OOP_LIMITCHARGE_T:
    case E_OOP_POWERLIMIT_T:
    {
        OOP_DWORD2_T DWORD2_T;
        
        memset(tmpBuff,0x00,30);
        memset((uint8 *)&DWORD2_T,0x00,sizeof(OOP_DWORD2_T));

        if(soucerbuffer[0]==DT_STRUCTURE)
        { 
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DWORD2_T.nValue1,tmpBuff,4);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DWORD2_T.nValue2,tmpBuff,4);
        }
        else
        {
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DWORD2_T.nValue2,tmpBuff,4);
        }
        
        
        memcpy(desbuffer,(uint8 *)&DWORD2_T,sizeof(OOP_DWORD2_T));
        *deslength=sizeof(OOP_DWORD2_T);
    }

    break;
    case E_OOP_LONG3V_T:
        {
            OOP_LONG3V_T LONG3V_T;
            i=0;
            memset((uint8*)&LONG3V_T,0xff,sizeof(OOP_LONG3V_T));
            if(pwrType == 1)
            {
                LONG3V_T.nNum=1;
            }
            else
            {
                LONG3V_T.nNum=3;
            }
            LONG3V_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {
                if(soucerbuffer[1] == 1)
                    LONG3V_T.nNum=1;
                if(soucerbuffer[1] == 3)
                    LONG3V_T.nNum=3;
                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&LONG3V_T.nValue[i],tmpBuff,2);
                    set_bit_value((uint8*)&LONG3V_T.rsv, 32, i);
                    i++;
                }
            }
            else
            {

                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&LONG3V_T.nValue[index],tmpBuff,2);
                set_bit_value((uint8*)&LONG3V_T.rsv, 32, index);
            }
            *deslength=sizeof(LONG3V_T);
            memcpy(desbuffer,(uint8 *)&LONG3V_T,sizeof(OOP_LONG3V_T));
        }
    break;
    case E_OOP_PILE_WORK_STATUS_T:
        {
            OOP_PILE_WORK_STATUS_T pileworkstatus;
            i=0;
            uint8 j=0;
            uint16 offset=0;
            uint16 tmpoffset=0;
            memset((uint8*)&pileworkstatus,0xff,sizeof(pileworkstatus));
            if(soucerbuffer[offset]==DT_ARRAY)
            {
                offset++;
                pileworkstatus.nNum = soucerbuffer[offset++];
                
                while(i<pileworkstatus.nNum)
                {
                    /*跳过结构体和元素个数*/
                    offset = offset+2;
                    memset(tmpBuff,0x00,30);
                    /*单号申请单号*/
                    tmpoffset = offset;
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    //去掉两个字节是octer_string+认为1个字节的长度
                    pileworkstatus.item[i].applicationNumber_len=offset-tmpoffset-2;
                   
                    memcpy(pileworkstatus.item[i].applicationNumber,tmpBuff,pileworkstatus.item[i].applicationNumber_len);
                   
                    /*路由器资产编号*/
                    tmpoffset = offset;
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    //去掉两个字节是octer_string+认为1个字节的长度
                    pileworkstatus.item[i].routingAssets_len=offset-tmpoffset-2;
                   
                    memcpy(pileworkstatus.item[i].routingAssets,tmpBuff,pileworkstatus.item[i].routingAssets_len);
                   
                    /*枪序号*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    pileworkstatus.item[i].charge_gun_index = tmpBuff[0];
                    
                    /*执行状态*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    pileworkstatus.item[i].actionState = tmpBuff[0];
                   
                    /*电能示值底值*/
                    j=0;
                    if(soucerbuffer[offset]==DT_ARRAY)
                    {
                        offset++;
                        pileworkstatus.item[i].bottomEn.nNum = soucerbuffer[offset++];
                        
                        while(j<pileworkstatus.item[i].bottomEn.nNum)
                        {
                            memset(tmpBuff,0x00,30);
                        
                            if(TRUE==taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE))
                            {
                                memcpy_r((uint8 *)&pileworkstatus.item[i].bottomEn.nValue[j],tmpBuff,4);  
                                                             
                            }
                            j++;
                        }
                    
                    }
                    /*当前电能示值*/
                    j=0;
                    if(soucerbuffer[offset]==DT_ARRAY)
                    {
                        offset++;
                        pileworkstatus.item[i].currentEn.nNum = soucerbuffer[offset++];
                       
                        while(j<pileworkstatus.item[i].currentEn.nNum)
                        {
                            memset(tmpBuff,0x00,30);
                        
                            if(TRUE==taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE))
                            {
                                memcpy_r((uint8 *)&pileworkstatus.item[i].currentEn.nValue[j],tmpBuff,4); 
                                                           
                            }
                            j++;
                        }
                    
                    }
                    /*已充电量*/
                    j=0;
                    if(soucerbuffer[offset]==DT_ARRAY)
                    {
                        offset++;
                        pileworkstatus.item[i].quantityEn.nNum = soucerbuffer[offset++];
                        
                        while(j<pileworkstatus.item[i].quantityEn.nNum)
                        {
                            memset(tmpBuff,0x00,30);
                        
                            if(TRUE==taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE))
                            {
                                memcpy_r((uint8 *)&pileworkstatus.item[i].quantityEn.nValue[j],tmpBuff,4);                                   
                            }
                            j++;
                        }
                    }
                    /*已充时间*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&pileworkstatus.item[i].haveChargedTime,tmpBuff,4);
                   
                    /*计划充电功率*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&pileworkstatus.item[i].planChargePower,tmpBuff,4);
                    
                    /*当前充电功率*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&pileworkstatus.item[i].curChargePower,tmpBuff,4);
                    
                    /*电压*/
                    j=0;
                    if(soucerbuffer[offset]==DT_ARRAY)
                    {
                        offset++;
                        pileworkstatus.item[i].volt.nNum = soucerbuffer[offset++];
                        
                        temlen=getDataTypeLength(soucerbuffer[offset]);
                        if(temlen>4)
                        {
                            temlen=4;
                        }
                        while(j<pileworkstatus.item[i].volt.nNum)
                        {
                            memset(tmpBuff,0x00,30);
                            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
                            memcpy_r((uint8 *)&pileworkstatus.item[i].volt.nValue[j],tmpBuff,4); 
                            j++;

                        }
                        
                    }
                    /*电流*/
                    j=0;
                    if(soucerbuffer[offset]==DT_ARRAY)
                    {
                        offset++;
                        pileworkstatus.item[i].curr.nNum = soucerbuffer[offset++];
                       
                        while(j<pileworkstatus.item[i].curr.nNum)
                        {
                            memset(tmpBuff,0x00,30);
                            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                            memcpy_r((uint8 *)&pileworkstatus.item[i].curr.nValue[i],tmpBuff,4);    
                            j++;

                        }
                        
                    }
                    /*充电桩状态*/
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    pileworkstatus.item[i].pileState = tmpBuff[0];
                    
                    /*用户id*/
                    tmpoffset = offset;
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    //去掉两个字节是octer_string+认为1个字节的长度
                    pileworkstatus.item[i].userid_len=offset-tmpoffset-2;
                    
                    memcpy(pileworkstatus.item[i].userid,tmpBuff,pileworkstatus.item[i].userid_len);
                    
                    /*保留*/
                    pileworkstatus.item[i].bak = 0;
                    i++;
                }
            }
            *deslength=sizeof(pileworkstatus);
            memcpy(desbuffer,(uint8 *)&pileworkstatus,sizeof(pileworkstatus));
        }
    break;
    case E_OOP_CHARGE_SERVICE_T:
        {

            OOP_CHARGE_SERVICE_T chargeservice;
            uint16 offset=0;
            uint8 n=0;
            if(soucerbuffer[offset]==DT_ARRAY)
            {
                offset++;
                chargeservice.nNum = soucerbuffer[offset++];
               
                while(n<chargeservice.nNum)
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    chargeservice.item[n].gunState = tmpBuff[0];  
                    
                    memset(chargeservice.item[n].bak,0x00,3);
                    n++;
                }
                
            }
            *deslength=sizeof(chargeservice);
            memcpy(desbuffer,(uint8 *)&chargeservice,sizeof(chargeservice));
        }
    break;
    case E_OOP_OCTETVAR16_T:
    {
        OOP_OCTETVAR16_T addr;
        uint16 offset=0;
        memset((uint8*)&addr,0x00,sizeof(OOP_OCTETVAR16_T));

        if(soucerbuffer[offset++]==DT_OCTET_STRING)
        {
            addr.nNum =soucerbuffer[offset++];
            if(addr.nNum<=16)
            {
                memcpy(addr.value,soucerbuffer+offset,addr.nNum);
            }
            offset+=addr.nNum;
        }
        *offset1 = offset;
        *deslength = sizeof(OOP_OCTETVAR16_T);
        memcpy(desbuffer,(uint8*)&addr,sizeof(OOP_OCTETVAR16_T));

    }
    break;
    case E_OOP_OCTETVAR64_T:
    {
        OOP_OCTETVAR64_T addr;
        uint16 offset=0;
        memset((uint8*)&addr,0x00,sizeof(OOP_OCTETVAR64_T));

        if(soucerbuffer[offset++]==DT_OCTET_STRING)
        {
            addr.nNum =soucerbuffer[offset++];
            if(addr.nNum<=64)
            {
                memcpy(addr.value,soucerbuffer+offset,addr.nNum);
            }
            offset+=addr.nNum;
            
        }
        *offset1 = offset;
        *deslength = sizeof(OOP_OCTETVAR64_T);
        memcpy(desbuffer,(uint8*)&addr,sizeof(OOP_OCTETVAR64_T));

    }
    break;
    case E_ESAM_COUNT:
    {
        OOP_ESAM_COUNT_T esam = {0};
        uint16 offset = 0;

        memset(tmpBuff, 0x00, 30);
        
        if (soucerbuffer[offset++] == DT_STRUCTURE)
        {
            offset++;
            memset(tmpBuff, 0x00, 30);
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&esam.negoCnt, tmpBuff, 4);

            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&esam.reportCnt, tmpBuff, 4);
            
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
            memcpy_r((uint8 *)&esam.seq, tmpBuff, 4);

        }
        *offset1 = offset;
        *deslength = sizeof(OOP_ESAM_COUNT_T);
        memcpy(desbuffer, (uint8 *)&esam, sizeof(OOP_ESAM_COUNT_T));
        
        break;
    }
    case E_SECURITY_FLAG:
    {
        OOP_SECURITYFLAG_E data;
        uint16 offset = 0;

        memset(tmpBuff, 0x00, 30);
        
        if (soucerbuffer[offset++] == DT_ENUM)
        {
            data = soucerbuffer[offset++];
        }
        *offset1 = offset;
        *deslength = sizeof(OOP_SECURITYFLAG_E);
        memcpy(desbuffer, (uint8 *)&data, sizeof(OOP_SECURITYFLAG_E));
        
        break;
    }
    case E_OOP_TIMEZONE_T:
    {
        OOP_TIMEZONE_T TIMEZONE;
        uint16 offset=0;
        uint16 i=0;

        memset((uint8*)&TIMEZONE,0x00,sizeof(OOP_TIMEZONE_T));

         if(soucerbuffer[offset++]==DT_ARRAY)
         {
            TIMEZONE.nNum = soucerbuffer[offset++];

            for(i=0;i<TIMEZONE.nNum;i++)
            {
                offset = offset+2;
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                TIMEZONE.period[i].mon = tmpBuff[0];
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                TIMEZONE.period[i].day = tmpBuff[0];
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                TIMEZONE.period[i].meterNum = tmpBuff[0];
            }
         }
        *offset1 = offset;
        *deslength = sizeof(TIMEZONE);
        memcpy(desbuffer,(uint8*)&TIMEZONE,sizeof(TIMEZONE));
    }
    break;
    case E_OOP_DAYPERIOD_T:
    {
        OOP_DAYPERIOD_T DAYPERIOD;
        uint16 offset=0;
        uint16 i=0;
        uint16 j=0;
        uint8 flg =0;

        memset((uint8*)&DAYPERIOD,0x00,sizeof(DAYPERIOD));

         if(soucerbuffer[offset++]==DT_ARRAY)
         {
            uint32 num = soucerbuffer[offset++];
            for(i=0;i<num;i++)
            {
               if(soucerbuffer[offset]==DT_ARRAY)
               {
                    DAYPERIOD.nNum = num;
                    offset++;
                    DAYPERIOD.dPeriod[i].nNum = soucerbuffer[offset++];
                    
                    for(j=0;j<DAYPERIOD.dPeriod[i].nNum;j++)
                    {
                        memset(tmpBuff,0x00,30);
                        offset = offset+2;
                        taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                        DAYPERIOD.dPeriod[i].period[j].hour = tmpBuff[0];
                        taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                        DAYPERIOD.dPeriod[i].period[j].min = tmpBuff[0];
                        taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                        DAYPERIOD.dPeriod[i].period[j].rateNO = tmpBuff[0];
                    }
               }
               else
               {
                    flg=1;  
                    offset+=3;
                    memset(tmpBuff,0x00,30);
                    DAYPERIOD.dPeriod[index].period[i].hour = soucerbuffer[offset++];
                    offset++;
                    DAYPERIOD.dPeriod[index].period[i].min = soucerbuffer[offset++];
                    offset++;
                    DAYPERIOD.dPeriod[index].period[i].rateNO = soucerbuffer[offset++];
                    set_bit_value((uint8*)&DAYPERIOD.dPeriod[index].rsv, 32, i);
               }
            }

            if(flg==1)
            {
                DAYPERIOD.nNum = OOP_MAX_DAYPERIODS;
                DAYPERIOD.dPeriod[index].nNum = num;
                set_bit_value((uint8*)&DAYPERIOD.rsv, 32, index);
            }
         }
  
        *offset1 = offset;
        *deslength = sizeof(DAYPERIOD);
        memcpy(desbuffer,(uint8*)&DAYPERIOD,sizeof(DAYPERIOD));
    }
    break;
    case E_OOP_RATETARIFF_T:
    {
        OOP_RATETARIFF_T RATETARIFF;
        uint16 offset=0;
        uint16 i=0;

        memset((uint8*)&RATETARIFF,0x00,sizeof(RATETARIFF));
        if(soucerbuffer[offset++]==DT_ARRAY)
        {
            RATETARIFF.nNum = soucerbuffer[offset++];

            for(i=0;i<RATETARIFF.nNum;i++)
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8*)&RATETARIFF.nValue[i],tmpBuff,4);
            }

        }
        *offset1 = offset;
        *deslength = sizeof(RATETARIFF);
        memcpy(desbuffer,(uint8*)&RATETARIFF,sizeof(RATETARIFF));
    }
    break;
    case E_OOP_STEPPRICE_T:
    {
        OOP_STEPPRICE_T STEPPRICE;
        uint16 offset=0;
        uint16 i=0;

        if(soucerbuffer[offset++]==DT_STRUCTURE)
        {
            offset++;
            if(soucerbuffer[offset++]==DT_ARRAY)
            {
                STEPPRICE.nNum1 = soucerbuffer[offset++];
                for(i=0;i<STEPPRICE.nNum1;i++)
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&STEPPRICE.sValue[i],tmpBuff,4);
                
                }
            }
            if(soucerbuffer[offset++]==DT_ARRAY)
            {
                STEPPRICE.nNum2 = soucerbuffer[offset++];
                for(i=0;i<STEPPRICE.nNum2;i++)
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&STEPPRICE.sPrice[i],tmpBuff,4);
                
                }
            }
            if(soucerbuffer[offset++]==DT_ARRAY)
            {
                STEPPRICE.nNum3 = soucerbuffer[offset++];
                for(i=0;i<STEPPRICE.nNum3;i++)
                {
                    memset(tmpBuff,0x00,30);
                  
                    offset = offset+2;
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    STEPPRICE.yAccDay[i].mon = tmpBuff[0];
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    STEPPRICE.yAccDay[i].day = tmpBuff[0];
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    STEPPRICE.yAccDay[i].hour = tmpBuff[0];                                  
                }
            }
        }
        *offset1 = offset;
        *deslength = sizeof(STEPPRICE);
        memcpy(desbuffer,(uint8*)&STEPPRICE,sizeof(STEPPRICE));
    }
    break;
    case E_OOP_HARMONIC_T:
    {
        OOP_HARMONIC_T harmonic;
        uint16 offset=0;
        uint16 i=0;
        uint8 tmpbuff[30];
        
        memset((uint8*)&harmonic,0x00,sizeof(harmonic));
        if(soucerbuffer[offset++]==DT_ARRAY)
        {
            harmonic.nNum = soucerbuffer[offset++];
            for(i=0;i<harmonic.nNum;i++)
            {
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpbuff,NULL_DATA,FALSE);
                memcpy_r((uint8*)&harmonic.nValue[i],tmpbuff,2);
                set_bit_value((uint8*)&harmonic.rsv, 32, i);
            }
        }
        *offset1 = offset;
        *deslength = sizeof(harmonic);
        memcpy(desbuffer,(uint8*)&harmonic,sizeof(harmonic));

    }
    break;
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)

    case E_OOP_LINE_LOSS_T:
    {
        OOP_Line_Loss_T lineloss;
        uint16 offset=0;
        uint8 tmpbuff[30];
        if(soucerbuffer[offset++]==DT_STRUCTURE)
        {
            offset++;
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpbuff,NULL_DATA,FALSE);
            memcpy_r((uint8*)&lineloss.linelossvalue,tmpbuff,4);
            taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpbuff,NULL_DATA,FALSE);
            memcpy_r((uint8*)&lineloss.linelossrate,tmpbuff,2);           
        }
        *offset1 = offset;
        *deslength = sizeof(lineloss);
        memcpy(desbuffer,(uint8*)&lineloss,sizeof(lineloss));
    }
    break;
    case E_OOP_SWITCH_IMPEDANCE_T:
    {
        OOP_Switch_Impedance_T switchimpedance;
        uint8 i=0;
        uint16 offset=0;
        uint8 tmpbuff[30];
        if(soucerbuffer[offset++]==DT_ARRAY)
        {
            switchimpedance.num = soucerbuffer[offset++];
            for(i=0;i<switchimpedance.num;i++)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                    if(soucerbuffer[offset++]==DT_TSA)
                    {
                        offset++;
                        switchimpedance.switchhindrance[i].addr.af = soucerbuffer[offset++];
                        memcpy(switchimpedance.switchhindrance[i].addr.add,soucerbuffer+offset,switchimpedance.switchhindrance[i].addr.len +1);
                        offset =offset + switchimpedance.switchhindrance[i].addr.len +1;
                    }
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpbuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&switchimpedance.switchhindrance[i].lineloss,tmpbuff,4);
                    
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&switchimpedance.switchhindrance[i].beforchangetime.year,tmpBuff,2);
                    memcpy((uint8*)&switchimpedance.switchhindrance[i].beforchangetime.month,tmpBuff+2,6);  

                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&switchimpedance.switchhindrance[i].afterchangetime.year,tmpBuff,2);
                    memcpy((uint8*)&switchimpedance.switchhindrance[i].afterchangetime.month,tmpBuff+2,6); 
                    
                    taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpbuff,NULL_DATA,FALSE);
                    memcpy_r((uint8*)&switchimpedance.switchhindrance[i].Current,tmpbuff,4);
                    
                }
            }
        }
        *offset1 = offset;
        *deslength = sizeof(switchimpedance);
        memcpy(desbuffer,(uint8*)&switchimpedance,sizeof(switchimpedance));
    }
    break;
#endif
#ifdef AREA_SHANDONG
    case E_OOP_HUMITURE_2E20_T:
    {
        OOP_HUMITURE_2E20_T humiture;
        uint16 offset = 0;
        uint8 tmpbuff[30];
        if(soucerbuffer[offset++] == DT_STRUCTURE)
        {
            offset++;
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpbuff, NULL_DATA, FALSE);
            memcpy_r((uint8*)&humiture.temp, tmpbuff, 4);
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpbuff, NULL_DATA, FALSE);
            memcpy_r((uint8*)&humiture.humidity, tmpbuff, 4);           
        }
        *offset1 = offset;
        *deslength = sizeof(humiture);
        memcpy(desbuffer, (uint8*)&humiture, sizeof(humiture));
    }
    break;
    case E_OOP_LOCK_STATE_2E21_T:
    {
        OOP_LOCK_STATE_2E21_T state;
        uint16 offset = 0;
        uint8 tmpbuff[30];
        if(soucerbuffer[offset++] == DT_STRUCTURE)
        {
            offset++;
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpbuff, NULL_DATA, FALSE);
            memcpy_r((uint8*)&state.door_state, tmpbuff, 1);
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpbuff, NULL_DATA, FALSE);
            memcpy_r((uint8*)&state.lock_state, tmpbuff, 1);           
        }
        *offset1 = offset;
        *deslength = sizeof(state);
        memcpy(desbuffer, (uint8*)&state, sizeof(state));
    }
    break;
    #endif
    case E_OOP_HARMONIC_VOLTAGE_T://谐波电压

    {
        OOP_HARMONIC_VOLTAGE_T harmonicVol;
        i=0;
        memset((uint8 *)&harmonicVol,0xff,sizeof(OOP_HARMONIC_VOLTAGE_T));
        harmonicVol.nNum = MAX_HARMONIC_VALUE_NUM;
        harmonicVol.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {

            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&harmonicVol.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&harmonicVol.rsv, 8, i);
                i++;
            }
            
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&harmonicVol.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&harmonicVol.rsv, 8,index);
        }
        *deslength=sizeof(OOP_HARMONIC_VOLTAGE_T);
        memcpy(desbuffer,(uint8 *)&harmonicVol,sizeof(OOP_HARMONIC_VOLTAGE_T));

    }

    break;
    case E_OOP_HARMONIC_CUR_POWER_T://谐波电流

    {
        OOP_HARMONIC_CUR_POWER_T harmonicCur;
        i=0;
        memset((uint8 *)&harmonicCur,0xff,sizeof(OOP_HARMONIC_CUR_POWER_T));
        harmonicCur.nNum = MAX_HARMONIC_VALUE_NUM;
        harmonicCur.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {

            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&harmonicCur.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&harmonicCur.rsv, 8, i);
                i++;
            }
            
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&harmonicCur.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&harmonicCur.rsv, 8,index);
        }
        *deslength=sizeof(OOP_HARMONIC_CUR_POWER_T);
        memcpy(desbuffer,(uint8 *)&harmonicCur,sizeof(OOP_HARMONIC_CUR_POWER_T));
    }

    break;

//    #ifdef AREA_ANHUI
    case E_OOP_DEVFREZDATAS_T:
    {
        OOP_DEVFREZDATAS_T  devFrezDatas = {0};
       
        uint16 offset = 0;
        uint16 i = 0;

        memset((uint8*)&devFrezDatas,0x00,sizeof(devFrezDatas));
        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            devFrezDatas.nNum = soucerbuffer[offset++];

            for (i = 0; i < devFrezDatas.nNum; i++)
            {
                memset(tmpBuff, 0x00, 30);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devFrezDatas.data[i].devClsID, tmpBuff, 2);
                
                memset(tmpBuff, 0x00, 30);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devFrezDatas.data[i].nEnergy, tmpBuff, 4);
                
                memset(tmpBuff, 0x00, 30);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devFrezDatas.data[i].nPower, tmpBuff, 4);
                if(soucerbuffer[offset]==DT_NULL)
                {
                    devFrezDatas.data[i].startTime = 0xFE;
                    offset++;
                }
                else
                {
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&devFrezDatas.data[i].startTime, tmpBuff, 1);
                }
                if(soucerbuffer[offset]==DT_NULL)
                {
                    devFrezDatas.data[i].stopTime = 0xFE;
                    offset++;
                }
                else
                {
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&devFrezDatas.data[i].stopTime, tmpBuff, 1);
                }
                if(soucerbuffer[offset]==DT_NULL)                     //异常状态
                {
                    devFrezDatas.data[i].nCount = 0xFE;
                    offset++;
                }
                else
                {
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&devFrezDatas.data[i].nCount, tmpBuff, 1);
                }
            }
        }
        *offset1 = offset;
        *deslength = sizeof(devFrezDatas);
        memcpy(desbuffer, (uint8 *)&devFrezDatas, sizeof(devFrezDatas));

        break;
    }
//    #else
//    case E_OOP_DEVFREZDATAS_T:
//    {
//        OOP_DEVFREZDATAS_T devFrezDatas = {0};
//        uint16 offset = 0;
//        uint16 i = 0;
//
//        if (soucerbuffer[offset++] == DT_ARRAY)
//        {
//            devFrezDatas.nNum = soucerbuffer[offset++];
//
//            for (i = 0; i < devFrezDatas.nNum; i++)
//            {
//                memset(tmpBuff, 0x00, 30);
//                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
//                memcpy_r((uint8 *)&devFrezDatas.data[i].devClsID, tmpBuff, 2);
//                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
//                memcpy_r((uint8 *)&devFrezDatas.data[i].nEnergy, tmpBuff, 4);
//            }
//        }
//        *offset1 = offset;
//        *deslength = sizeof(devFrezDatas);
//        memcpy(desbuffer, (uint8 *)&devFrezDatas, sizeof(devFrezDatas));
//
//        break;
//    }
//    #endif
    case E_OOP_DEVINSTANTDATAS_T:
    {
        OOP_DEVINSTANTDATAS_T devInstantDatas = {0};
        uint16 offset = 0;
        uint16 i = 0;

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            devInstantDatas.nNum = soucerbuffer[offset++];

            for (i = 0; i < devInstantDatas.nNum; i++)
            {
                memset(tmpBuff, 0x00, 30);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devInstantDatas.data[i].devClsID, tmpBuff, 2);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devInstantDatas.data[i].devStatus, tmpBuff, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devInstantDatas.data[i].power, tmpBuff, 4);
                //taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                //memcpy_r((uint8 *)&devInstantDatas.data[i].nEnergy, tmpBuff, 4);
            }
        }
        *offset1 = offset;
        *deslength = sizeof(devInstantDatas);
        memcpy(desbuffer, (uint8 *)&devInstantDatas, sizeof(devInstantDatas));

        break;
    }
    case E_OOP_DEVSAMPDATAS_T:
    {
        OOP_DEVSAMPDATAS_T devsampDatas = {0};
        uint16 offset = 0;
        uint16 i = 0;

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            devsampDatas.nNum = soucerbuffer[offset++];

            for (i = 0; i < devsampDatas.nNum; i++)
            {
                memset(tmpBuff, 0x00, 30);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devsampDatas.data[i].phaseinfo, tmpBuff, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devsampDatas.data[i].vlotageSamp, tmpBuff, 4);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&devsampDatas.data[i].currentSamp, tmpBuff, 4);
                
            }
        }
        *offset1 = offset;
        *deslength = sizeof(devsampDatas);
        memcpy(desbuffer, (uint8 *)&devsampDatas, sizeof(devsampDatas));
        
        break;
    }
    case E_TSA:
    {
        OOP_TSA_T tsa;
        uint16 offset = 0;

        memset((uint8*)&tsa,0x00,sizeof(tsa));
        if (soucerbuffer[offset++] == DT_TSA)
        {
            offset++;
            tsa.af =soucerbuffer[offset++];
            if(tsa.flag == 0)
            {
                memcpy(tsa.add,soucerbuffer+offset,tsa.len+1);
                offset =offset + tsa.len+1;

            }
            else
            {
                tsa.vxd = soucerbuffer[offset++];
                memcpy(tsa.add,soucerbuffer+offset,tsa.len);
                offset =offset + tsa.len;
                
            }

            
        }
        *offset1 = offset;
        *deslength = sizeof(tsa);
        memcpy(desbuffer, (uint8 *)&tsa, sizeof(tsa));
        
        break;
    }
    case E_OOP_APPURTENANT_METER_LIST_T:
    {
        OOP_APPURTENANT_METER_LIST_T meterlist = {0};
        uint16 offset = 0;
        uint16 i = 0;

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            meterlist.num = soucerbuffer[offset++];
            for(i =0;i<meterlist.num;i++)
            {
                if(soucerbuffer[offset++] == DT_STRUCTURE)
                {
                    offset++;
                    offset=offset+3;
                    
                    memcpy(meterlist.meterinfo[i].pipeaddr,soucerbuffer+offset,6);
                    offset=offset+9;
                    memcpy(meterlist.meterinfo[i].meteraddr,soucerbuffer+offset,6);
                    offset=offset+6;
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    meterlist.meterinfo[i].phaseinfo = tmpBuff[0];
                }
            }
           
        }
        *offset1 = offset;
        *deslength = sizeof(meterlist);
        memcpy(desbuffer, (uint8 *)&meterlist, sizeof(meterlist));
        
        break;
    }
    case E_OOP_METERMOD_LIST:
    {
        OOP_METERMOD_LIST meterMod = {0};
        uint16 offset = 0;
        uint16 tmpOffset = 0;
        uint16 i = 0;
        uint8  buf[128] = {0};

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            meterMod.nNum = soucerbuffer[offset++];

            for (i = 0; i < meterMod.nNum; i++)
            {
                memset(buf, 0x00, sizeof(buf));
                tmpOffset = offset;
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                meterMod.info[i].szName.nNum = offset - tmpOffset - 4; /* 4：结构体类型、结构体项数、字符串类型、字符串长度 */
                memcpy_r((uint8 *)&meterMod.info[i].szName, buf, meterMod.info[i].szName.nNum);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&meterMod.info[i].nVxd, buf, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&meterMod.info[i].modType, buf, 2);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&meterMod.info[i].serialNum, buf, 8);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&meterMod.info[i].portFlag, buf, 1);

                if (meterMod.info[i].portFlag == 1)
                {
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&meterMod.info[i].port, buf, 4);
                }
            }
        }

        *offset1 = offset;
        *deslength = sizeof(meterMod);
        memcpy(desbuffer, (uint8 *)&meterMod, sizeof(meterMod));

        break;
    }
    case E_OOP_MODVERSION_LIST:
    {
        OOP_MODVERSION_LIST modVersion = {0};
        uint16 offset = 0;
        uint16 tmpOffset = 0;
        uint16 i = 0;
        uint8  buf[128] = {0};

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            modVersion.nNum = soucerbuffer[offset++];

            for (i = 0; i < modVersion.nNum; i++)
            {
                memset(buf, 0x00, sizeof(buf));
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&modVersion.info[i].nVxd, buf, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&modVersion.info[i].szFactory, buf, 4);
                tmpOffset = offset;
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                modVersion.info[i].szSoftVer.nNum = offset - tmpOffset - 2; /* 2：字符串类型、字符串长度 */
                memcpy_r((uint8 *)&modVersion.info[i].szSoftVer, buf, modVersion.info[i].szSoftVer.nNum);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&modVersion.info[i].szSoftDate, buf, 6);
                tmpOffset = offset;
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                modVersion.info[i].szHardVer.nNum = offset - tmpOffset - 2; /* 2：字符串类型、字符串长度 */
                memcpy_r((uint8 *)&modVersion.info[i].szHardVer, buf, modVersion.info[i].szHardVer.nNum);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&modVersion.info[i].szHardDate, buf, 6);
                tmpOffset = offset;
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                modVersion.info[i].szExtend.nNum = offset - tmpOffset - 2; /* 2：字符串类型、字符串长度 */
                memcpy_r((uint8 *)&modVersion.info[i].szExtend, buf, modVersion.info[i].szExtend.nNum);
            }
        }

        *offset1 = offset;
        *deslength = sizeof(modVersion);
        memcpy(desbuffer, (uint8 *)&modVersion, sizeof(modVersion));

        break;
    }
    case E_OOP_SOFTREC_LIST:
    {
        OOP_SOFTREC_LIST softRec = {0};
        uint16 offset = 0;
        uint16 i = 0;
        uint8  buf[128] = {0};

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            softRec.nNum = soucerbuffer[offset++];

            for (i = 0; i < softRec.nNum; i++)
            {
                memset(buf, 0x00, sizeof(buf));
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy((uint8 *)&softRec.info[i].type, buf, 2);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy((uint8 *)&softRec.info[i].factory, buf, 4);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&softRec.info[i].softType, buf, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy((uint8 *)&softRec.info[i].date, buf, 8);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy_r((uint8 *)&softRec.info[i].seq, buf, 1);
                taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, buf, NULL_DATA, FALSE);
                memcpy((uint8 *)&softRec.info[i].no, buf, 16);
            }
        }

        *offset1 = offset;
        *deslength = sizeof(softRec);
        memcpy(desbuffer, (uint8 *)&softRec, sizeof(softRec));

        break;
    }
    case E_OOP_Power_Characteristic_Record:
    {
        OOP_Power_Characteristic_Record powerrecord = {0};
        uint16 offset = 0;
        uint16 i = 0;

        memset(tmpBuff, 0x00, 30);
        
        if (soucerbuffer[offset++] == DT_STRUCTURE)
        {
            offset++;
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
            memcpy_r((uint8 *)&powerrecord.devtype, tmpBuff, 1);
            
            if (soucerbuffer[offset++] == DT_ARRAY)
            {
                powerrecord.num = soucerbuffer[offset++];
                for (i = 0; i < powerrecord.num; i++)
                {
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&powerrecord.powerunit[i].phase, tmpBuff, 1);
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&powerrecord.powerunit[i].type, tmpBuff, 1);
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&powerrecord.powerunit[i].power, tmpBuff, 4);
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&powerrecord.powerunit[i].startminute, tmpBuff, 1);
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
                    memcpy_r((uint8 *)&powerrecord.powerunit[i].startsecond, tmpBuff, 1);
                    
                }
            }
        }
        *offset1 = offset;
        *deslength = sizeof(powerrecord);
        memcpy(desbuffer, (uint8 *)&powerrecord, sizeof(powerrecord));
        
        break;
    }
    case E_OOP_Energy_Characteristic_Record:
    {
        OOP_Energy_Characteristic_Record_Unit energyrecord = {0};
        uint16 offset = 0;
        uint16 i = 0;

        if (soucerbuffer[offset++] == DT_STRUCTURE)
        {
            offset++;
            taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);
            memcpy_r((uint8 *)&energyrecord.devtype, tmpBuff, 1);

            if(soucerbuffer[offset++] == DT_ARRAY)
            {
                energyrecord.vlotage.num = soucerbuffer[offset++];

                for (i = 0; i < energyrecord.vlotage.num; i++)
                {
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                    memcpy_r((uint8 *)&energyrecord.vlotage.voltageuint[i], tmpBuff, 2);                 
                    
                }
            }
            
            if(soucerbuffer[offset++] == DT_ARRAY)
            {
                energyrecord.energyuint.num = soucerbuffer[offset++];

                for (i = 0; i < energyrecord.energyuint.num ; i++)
                {
                    if (soucerbuffer[offset++] == DT_STRUCTURE)
                    {
                        offset++;
                        
                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].phase, tmpBuff, 1);  

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].energy, tmpBuff, 4);

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].activepower, tmpBuff, 4);

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].reactivepower, tmpBuff, 4);

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].current, tmpBuff, 4);

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].currentoddhaimonic, tmpBuff, 4);

                        memset(tmpBuff, 0x00, 30);
                        taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                        memcpy_r((uint8 *)&energyrecord.energyuint.energyuint[i].currentevenhaimonic, tmpBuff, 4);
                    }
                    
                }
            }
        }
        *offset1 = offset;
        *deslength = sizeof(energyrecord);
        memcpy(desbuffer, (uint8 *)&energyrecord, sizeof(energyrecord));
        
        break;
    }
    case E_OOP_REPORTS_T:
    {
        OOP_REPORTS_T report = {0};
        uint16 offset = 0;
        uint16 i = 0;

        if (soucerbuffer[offset++] == DT_ARRAY)
        {
            report.nNum = soucerbuffer[offset++];
            for(i =0;i<report.nNum;i++)
            {
                if (soucerbuffer[offset++] == DT_STRUCTURE)
                {
                    offset++;
                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                    memcpy_r((uint8 *)&report.state[i].oad.value, tmpBuff, 4);

                    memset(tmpBuff, 0x00, 30);
                    taskmng_oop_data_extract(soucerbuffer, &offset, soucerlen, tmpBuff, NULL_DATA, FALSE);                   
                    memcpy_r((uint8 *)&report.state[i].state, tmpBuff, 1);
                }

             }
        }
        *offset1 = offset;
        *deslength = sizeof(report);
        memcpy(desbuffer, (uint8 *)&report, sizeof(report));
        
        break;
    }
    case E_OOP_EVTSOURCE_T:
    {
        OOP_EVTSOURCE_T source;
        uint16 offset = 0;
        
        memset((uint8*)&source,0x00,sizeof(source));
        source.choice = soucerbuffer[offset++];
        switch(source.choice)
        {
            case DT_NULL:
            {
                break;
            }
            case DT_OAD:
            {
                memcpy_r((uint8*)&source.oad.value,soucerbuffer+offset,4);
                offset =offset +4;
            }
            break;
            default:
            break;
        }
        *offset1 = offset;
        *deslength = sizeof(source);
        memcpy(desbuffer, (uint8 *)&source, sizeof(source));
        
        break;
    }
    case E_OOP_BREAK_RUN_STATUS_T:
    {
        OOP_BREAK_RUN_STATUS_T status;
        uint16 offset =0;
        uint8 i=0;

        memset((uint8*)&status,0x00,sizeof(status));
        if(soucerbuffer[offset++] == DT_ARRAY)
        {
            status.nNum = soucerbuffer[offset++];
            for(i=0;i<status.nNum;i++)
            {
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                status.status[i].status1 = tmpBuff[0];
                taskmng_oop_data_extract(soucerbuffer,&offset,soucerlen,tmpBuff,NULL_DATA,FALSE);
                status.status[i].status2 = tmpBuff[0];
            }
        }
        *offset1 = offset;
        *deslength = sizeof(status);
        memcpy(desbuffer, (uint8 *)&status, sizeof(status));
    }
    default:   
    break;
    }
}



void Convert_Dlms_Data_To_Struct(uint8 *soucerbuffer,uint16 *offset1,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength,uint8 pwrType,uint8 Csdtype,uint8 dlmsnum)
{
    
    //uint16 num;
    uint8 tmpBuff[30];
    uint8 i=0;
    uint16 temlen;
    //uint8 scaler=0;
    //int8 unit=0;
    int64 value=0;
    //uint32 div=0;
    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
    {
        case E_OOP_ENERGY_T://普通精度电量
        {
            OOP_ENERGY_T ENERGY_T;
            uint16 offset= *offset1;
            i=0;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;

            if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
            {
                offset =offset+5;
                while(i<OOP_FULL_RATES)
                {
                    offset++;
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset = offset+8;
                    }
                    ENERGY_T.nValue[i] = value/100;
                    
                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);

                    i++;
                }
            
            }
            else
            {
               if(Csdtype==0)
               {
                //offset++;
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[index] = value/100;

                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
                }
               else
               {
                for(i=0;i<dlmsnum;i++)
                {
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[i] = value/100;

                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);
                }
               }
            }
            *deslength=sizeof(OOP_ENERGY_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_ENERGY_T));
        }
    break;

    case E_OOP_ENERGYS_T://普通精度电量带符号
        {
            OOP_ENERGYS_T ENERGY_T;
            uint16 offset= *offset1;
            i=0;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;

            
             if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
            {
                offset =offset+5;
                while(i<OOP_FULL_RATES)
                {
                    offset++;
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[i] = value/100;
                    
                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);

                    i++;
                }
            
            }
            else
            {
                if(Csdtype==0)
                {
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[index] = value/100;

                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
                }
                else
                {
                    for(i=0;i<dlmsnum;i++)
                    {
                        if(soucerbuffer[offset++]==DT_LONG64)
                        {
                            memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                            offset =offset+8;
                        }
                        ENERGY_T.nValue[index] = value/100;

                        set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);
                    }
                }
            }
            *deslength=sizeof(OOP_ENERGY_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_ENERGY_T));
        }
        break;

    case E_OOP_HENERGY_T://高通精度电量不带符号
        {
            OOP_HENERGY_T ENERGY_T;
            uint16 offset= *offset1;
            i=0;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;

   
            if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
            {
                offset =offset+5;
                while(i<OOP_FULL_RATES)
                {
                    offset++;
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[i] = value;
                    
                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);

                    i++;
                }
            
            }
            else
            {
                //offset++;
                if(Csdtype==0)
                {
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[index] = value;

                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
                }
                else
                {
                    for(i=0;i<dlmsnum;i++)
                    {
                        if(soucerbuffer[offset++]==DT_LONG64)
                        {
                            memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                            offset =offset+8;
                        }
                        ENERGY_T.nValue[i] = value;

                        set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);
                    }
                }
            }
            *deslength=sizeof(OOP_HENERGYS_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_HENERGYS_T));
        }
        break;

    case E_OOP_HENERGYS_T://高通精度电量带符号
        {
            OOP_HENERGYS_T ENERGY_T;
            uint16 offset= *offset1;
            i=0;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;

            if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
            {
                offset =offset+5;
                while(i<OOP_FULL_RATES)
                {
                    offset++;
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[i] = value;
                    
                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);

                    i++;
                }
            
            }
            else
            {
                //offset++;
                if(Csdtype==0)
                {
                    if(soucerbuffer[offset++]==DT_STRUCTURE)
                    {
                        offset++;
                         if(soucerbuffer[offset++]==DT_INTEGER)
                         {
                            if(soucerbuffer[offset]==0xFF)
                            {
                                //scaler = 1;
                            }
                            else if(soucerbuffer[offset]==0xFE)
                            {
                                //scaler = 2;
                            }
                         }
                         offset =offset+4;
                    }
                    if(soucerbuffer[offset++]==DT_LONG64)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                        offset =offset+8;
                    }
                    ENERGY_T.nValue[index] = value;

                    set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
                }
                else
                {
                    for(i=0;i<dlmsnum;i++)
                    {
                        if(soucerbuffer[offset++]==DT_LONG64)
                        {
                            memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],8);
                            offset =offset+8;
                        }
                        ENERGY_T.nValue[i] = value;

                        set_bit_value((uint8*)&ENERGY_T.rsv, 32, i);
                    }
                }
            }
            *deslength=sizeof(OOP_HENERGYS_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_HENERGYS_T));
        }
    break;

    case E_OOP_DEMAND_T:  //需量
    case E_OOP_DEMANDS_T:
    {
        OOP_DEMAND_T DEMAND_T;
        uint8 offset=*offset1;
        i=0;
        memset((uint8 *)&DEMAND_T,0xff,sizeof(OOP_DEMAND_T));
        DEMAND_T.nNum=OOP_FULL_RATES;
        DEMAND_T.rsv=0;

        //offset++; 
        memcpy((uint8*)&DEMAND_T.rsv,soucerbuffer+offset,4);
        offset = offset+5;
        
        if(pOadInfo->Oad.nIndex==0)
        {
            while(i<OOP_FULL_RATES)
            {
                
                memset(tmpBuff,0x00,30);
                offset++; //长度
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
    
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(DEMAND_T.demand[i].nValue),(uint8*)&soucerbuffer[offset],4);
                    offset =offset+4;
                }
                offset++; //长度
                if(soucerbuffer[offset++]==DT_OCTET_STRING)
                {
                    offset++;
                    
                    memcpy_r((uint8 *)&DEMAND_T.demand[i].time.year,&soucerbuffer[offset],2);
                    offset =offset +2;
                    DEMAND_T.demand[i].time.month = soucerbuffer[offset++];
                    DEMAND_T.demand[i].time.day = soucerbuffer[offset++];
                    offset++;
                    DEMAND_T.demand[i].time.hour = soucerbuffer[offset++];
                    DEMAND_T.demand[i].time.minute = soucerbuffer[offset++];
                    DEMAND_T.demand[i].time.second = soucerbuffer[offset++];
    //                memcpy((uint8 *)&DEMAND_T.demand[index].time.month,&soucerbuffer[offset],6);
    //                offset =offset +6;
                }

                i++;
            }
        }
        else
        {

            memset(tmpBuff,0x00,30);
            offset++; //长度
            if(soucerbuffer[offset++]==DT_STRUCTURE)
            {
                offset++;
                 if(soucerbuffer[offset++]==DT_INTEGER)
                 {
                    if(soucerbuffer[offset]==0xFF)
                    {
                        //scaler = 1;
                    }
                    else if(soucerbuffer[offset]==0xFE)
                    {
                        //scaler = 2;
                    }
                 }
                 offset =offset+4;
            }

            if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
            {
                memcpy_r((uint8*)&(DEMAND_T.demand[index].nValue),(uint8*)&soucerbuffer[offset],4);
                offset =offset+4;
            }
            offset++; //长度
            if(soucerbuffer[offset++]==DT_OCTET_STRING)
            {
                offset++;
                
                memcpy_r((uint8 *)&DEMAND_T.demand[index].time.year,&soucerbuffer[offset],2);
                offset =offset +2;
                DEMAND_T.demand[index].time.month = soucerbuffer[offset++];
                DEMAND_T.demand[index].time.day = soucerbuffer[offset++];
                offset++;
                DEMAND_T.demand[index].time.hour = soucerbuffer[offset++];
                DEMAND_T.demand[index].time.minute = soucerbuffer[offset++];
                DEMAND_T.demand[index].time.second = soucerbuffer[offset++];
//                memcpy((uint8 *)&DEMAND_T.demand[index].time.month,&soucerbuffer[offset],6);
//                offset =offset +6;
            }
            
        }
        *deslength=sizeof(OOP_DEMAND_T);
        memcpy(desbuffer,(uint8 *)&DEMAND_T,sizeof(OOP_DEMAND_T ));
    }
    break;
    case E_OOP_DWORD3V_T://分相变量

    {
        OOP_DWORD3V_T DWORD3V_T;
        i=0;
        memset((uint8 *)&DWORD3V_T,0xff,sizeof(OOP_DWORD3V_T));
        if(pwrType == 1)
        {
            DWORD3V_T.nNum=1;
        }
        else
        {
            DWORD3V_T.nNum=3;
        }
        DWORD3V_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            temlen=getDataTypeLength(soucerbuffer[2]);
            if(temlen>4)
            {
                temlen=4;
            }
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&DWORD3V_T.nValue[i],tmpBuff,4);
                set_bit_value((uint8*)&DWORD3V_T.rsv, 32, i);
                i++;

            }
            
        }
        else
        {
            temlen=getDataTypeLength(soucerbuffer[0]);
            if(temlen>4)
            {
                temlen=4;
            }
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff+4-temlen,NULL_DATA,FALSE);
            memcpy_r((uint8 *)&DWORD3V_T.nValue[index],tmpBuff,4);
            set_bit_value((uint8*)&DWORD3V_T.rsv, 32,index);
        }
        *deslength=sizeof(OOP_DWORD3V_T);
        memcpy(desbuffer,(uint8 *)&DWORD3V_T,sizeof(OOP_DWORD3V_T));

    }

    break;
    case E_OOP_WORD3V_T://电压

    {
        OOP_WORD3V_T WORD3V_T;
        uint16 offset=*offset1;
        i=0;
        memset((uint8 *)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));
        if(pwrType == 1)
        {
            WORD3V_T.nNum=1;
        }
        else
        {
            WORD3V_T.nNum=3;
        }
        WORD3V_T.rsv=0;
        
        if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
        {
            offset =offset+5;
            while(i<WORD3V_T.nNum)
            {
                offset++;
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    offset =offset+4;
                }
                WORD3V_T.nValue[i] = value/10;
                set_bit_value((uint8*)&WORD3V_T.rsv, 32,i);
                i++;
            }
        }
        else
        { 
            if((Csdtype==0))
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                }
                WORD3V_T.nValue[index] = value/10;
                set_bit_value((uint8*)&WORD3V_T.rsv, 32,index);
            }
            else
            {
                for(i=0;i<dlmsnum;i++)
                {
                    if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    }
                    WORD3V_T.nValue[i] = value/10;
                    set_bit_value((uint8*)&WORD3V_T.rsv, 32,i);
                }
            }

        }
        *deslength=sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,(uint8 *)&WORD3V_T,sizeof(OOP_WORD3V_T));

    }

    break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;
        uint16 offset = *offset1;
        i=0;
        memset((uint8 *)&INT3V_T,0xff,sizeof(OOP_INT3V_T));
        if(pwrType == 1)
        {
            INT3V_T.nNum=1;
        }
        else
        {
            INT3V_T.nNum=3;
        }
        INT3V_T.rsv=0;
        
        if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
        {
            offset =offset+5;
            while(i<INT3V_T.nNum)
            {
                offset++;
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    offset =offset+4;
                }
                    

                INT3V_T.nValue[i] = value*10;
                set_bit_value((uint8*)&INT3V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            if(Csdtype==0)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                }

                INT3V_T.nValue[index] = value*10;
                set_bit_value((uint8*)&INT3V_T.rsv, 32, index);
            }
            else
            {
                for(i=0;i<dlmsnum;i++)
                {
                    if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    }

                    INT3V_T.nValue[i] = value*10;
                    set_bit_value((uint8*)&INT3V_T.rsv, 32, i);
                }
            }
        }
        *deslength=sizeof(OOP_INT3V_T);
        memcpy(desbuffer,(uint8 *)&INT3V_T,sizeof(OOP_INT3V_T));
    }
    break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T  INT4V_T;
        uint16 offset = *offset1;
        i=0;
        memset((uint8 *)&INT4V_T,0xff,sizeof(OOP_INT4V_T));
        if(pwrType == 1)
        {
            INT4V_T.nNum=2;
        }
        else
        {
            INT4V_T.nNum=4;
        }
        INT4V_T.rsv=0;
        
        if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
        {
           offset =offset+5;
            while(i<INT4V_T.nNum)
            {
                offset++;
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    offset =offset+4;
                }
                    

                INT4V_T.nValue[i] = value*10;
                set_bit_value((uint8*)&INT4V_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            if(Csdtype==0)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                }

                INT4V_T.nValue[index] = value*10;
                set_bit_value((uint8*)&INT4V_T.rsv, 32, index);
            }
            else
            {
                for(i=0;i<dlmsnum;i++)
                {
                     if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    }

                    INT4V_T.nValue[i] = value*10;
                    set_bit_value((uint8*)&INT4V_T.rsv, 32, i);

                }
            }

        }
        *deslength=sizeof(INT4V_T);
        memcpy(desbuffer,(uint8 *)&INT4V_T,sizeof(INT4V_T));

    }
    break;
    case E_DATETIME_S://时间
    {
        OOP_DATETIME_S_T DATETIME_S_T;

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));
        memset(tmpBuff,0x00,30);
        if(soucerbuffer[0]==DT_OCTET_STRING)
        {
            *offset1 = *offset1+2;
            memcpy(tmpBuff,&soucerbuffer[*offset1],8);
        }
        memcpy_r((uint8*)&DATETIME_S_T.year,tmpBuff,2);
        memcpy((uint8*)&DATETIME_S_T.month,tmpBuff+2,6);
        if(DATETIME_S_T.second == 0xff)
            DATETIME_S_T.second =0;
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));   
        *deslength=sizeof(OOP_DATETIME_S_T);
    }
    break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T METWORD_T;
        i=0;
        memset((uint8*)&METWORD_T,0xff,sizeof(OOP_METWORD_T));
        METWORD_T.nNum=OOP_METWORD_NUM;
        METWORD_T.rsv=0;
        if(soucerbuffer[0]==DT_ARRAY)
        {
            while(i<soucerbuffer[1])
            {
                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_bitrev((uint8 *)&METWORD_T.metWord[i],tmpBuff,2);
                set_bit_value((uint8*)&METWORD_T.rsv, 32, i);
                i++;
            }
        }
        else
        {
            memset(tmpBuff,0x00,30);
            taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
            memcpy_bitrev((uint8 *)&METWORD_T.metWord[index],tmpBuff,2);
            set_bit_value((uint8*)&METWORD_T.rsv, 32, index);
        }
        *deslength=sizeof(OOP_METWORD_T);
        memcpy(desbuffer,(uint8 *)&METWORD_T,sizeof(OOP_METWORD_T));

    }       
    break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
        uint16 offset = *offset1;
        i=0;
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));
        if(pwrType == 1)
        {
            LONG4V_T.nNum=2;
        }
        else
        {
            LONG4V_T.nNum=4;
        }
        LONG4V_T.rsv=0;
        offset =offset+5;
        if((pOadInfo->Oad.nIndex==0)&&(Csdtype==0)&&(dlmsnum !=1))
        {
            while(i<LONG4V_T.nNum)
            {
                offset++;
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                }
                    

                LONG4V_T.nValue[i] = value*10;
                set_bit_value((uint8*)&LONG4V_T.rsv, 32, i);
            }
        }
        else
        {
            if(Csdtype==0)
            {
                if(soucerbuffer[offset++]==DT_STRUCTURE)
                {
                    offset++;
                     if(soucerbuffer[offset++]==DT_INTEGER)
                     {
                        if(soucerbuffer[offset]==0xFF)
                        {
                            //scaler = 1;
                        }
                        else if(soucerbuffer[offset]==0xFE)
                        {
                            //scaler = 2;
                        }
                     }
                     offset =offset+4;
                }
                if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                {
                    memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                }

                LONG4V_T.nValue[index] = value;
                set_bit_value((uint8*)&LONG4V_T.rsv, 32, index);
            }
            else
            {
                for(i=0;i<dlmsnum;i++)
                {
                    if(soucerbuffer[offset++]==DT_DOUBLE_LONG)
                    {
                        memcpy_r((uint8*)&(value),(uint8*)&soucerbuffer[offset],4);
                    }

                    LONG4V_T.nValue[i] = value*10;
                    set_bit_value((uint8*)&LONG4V_T.rsv, 32, i);
                }
            }
        }
        *deslength=sizeof(LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(LONG4V_T));

    }
        
    break;
    case E_OOP_LONG3V_T:
        {
            OOP_LONG3V_T LONG3V_T;
            i=0;
            memset((uint8*)&LONG3V_T,0xff,sizeof(OOP_LONG3V_T));
            if(pwrType == 1)
            {
                LONG3V_T.nNum=1;
            }
            else
            {
                LONG3V_T.nNum=3;
            }
            LONG3V_T.rsv=0;
            if(soucerbuffer[0]==DT_ARRAY)
            {

                while(i<soucerbuffer[1])
                {
                    memset(tmpBuff,0x00,30);
                    taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                    memcpy_r((uint8 *)&LONG3V_T.nValue[i],tmpBuff,2);
                    set_bit_value((uint8*)&LONG3V_T.rsv, 32, i);
                    i++;
                }
            }
            else
            {

                memset(tmpBuff,0x00,30);
                taskmng_oop_data_extract(soucerbuffer,offset1,soucerlen,tmpBuff,NULL_DATA,FALSE);
                memcpy_r((uint8 *)&LONG3V_T.nValue[index],tmpBuff,2);
                set_bit_value((uint8*)&LONG3V_T.rsv, 32, index);
            }
            *deslength=sizeof(LONG3V_T);
            memcpy(desbuffer,(uint8 *)&LONG3V_T,sizeof(OOP_LONG3V_T));
        }
    break;
    default:   
    break;
    }
}

void Convert_Minute_Data_To_Struct(uint8 *soucerbuffer,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength)
{
        

    uint8 index = pOadInfo->Oad.nIndex;
    if(index>0)
        index--;
    switch (pOadInfo->CovertSruct)
        {
        case E_OOP_ENERGY_T://普通精度电量
            {
            OOP_ENERGY_T ENERGY_T;
            memset((uint8 *)&ENERGY_T,0xff,sizeof(OOP_ENERGY_T));
            ENERGY_T.nNum=OOP_FULL_RATES;
            ENERGY_T.rsv=0;
            {
                if(soucerlen==4)
                {
                    ENERGY_T.nValue[index] = bin2_int32u(soucerbuffer);
                }                    
                set_bit_value((uint8*)&ENERGY_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_ENERGY_T);
            memcpy(desbuffer,(uint8 *)&ENERGY_T,sizeof(OOP_ENERGY_T));
        }
    break;

    case E_OOP_ENERGYS_T://普通精度电量
        {
            OOP_ENERGYS_T ENERGYS_T;

            memset((uint8 *)&ENERGYS_T,0xff,sizeof(OOP_ENERGYS_T));
            ENERGYS_T.nNum=OOP_FULL_RATES;
            ENERGYS_T.rsv=0;
            {
                
                if(soucerlen==4)
                {
                    ENERGYS_T.nValue[index] = bin2_int32u(soucerbuffer);
                } 
                
                set_bit_value((uint8*)&ENERGYS_T.rsv, 32, index);
            }
            *deslength=sizeof(OOP_ENERGYS_T);
            memcpy(desbuffer,(uint8 *)&ENERGYS_T,sizeof(OOP_ENERGYS_T));
        }
                break;

    case E_OOP_WORD3V_T://电压

            {
        OOP_WORD3V_T WORD3V_T;
                
        memset((uint8 *)&WORD3V_T,0xff,sizeof(OOP_WORD3V_T));
        WORD3V_T.nNum=3;
                
        WORD3V_T.rsv=0;

            {

            memcpy_r((uint8 *)&WORD3V_T.nValue[index],soucerbuffer,2);
            set_bit_value((uint8*)&WORD3V_T.rsv, 32,index);
            }
        *deslength=sizeof(OOP_WORD3V_T);
        memcpy(desbuffer,(uint8 *)&WORD3V_T,sizeof(OOP_WORD3V_T));

        }

        break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T  INT3V_T;

        memset((uint8 *)&INT3V_T,0xff,sizeof(OOP_INT3V_T));

        INT3V_T.nNum=3;
        
        INT3V_T.rsv=0;

                {
            memcpy_r((uint8 *)&INT3V_T.nValue[index],soucerbuffer,4);
            set_bit_value((uint8*)&INT3V_T.rsv, 32, index);
                }
        *deslength=sizeof(OOP_INT3V_T);
        memcpy(desbuffer,(uint8 *)&INT3V_T,sizeof(OOP_INT3V_T));
            }
    break;
    case E_OOP_INT4V_T://功率变量
            {
        OOP_INT4V_T  INT4V_T;

        memset((uint8 *)&INT4V_T,0xff,sizeof(OOP_INT4V_T));
                    
        INT4V_T.nNum=4;
                    
        INT4V_T.rsv=0;

        {
            memcpy_r((uint8 *)&INT4V_T.nValue[index],soucerbuffer,4);
            set_bit_value((uint8*)&INT4V_T.rsv, 32, index);
                }
        *deslength=sizeof(OOP_INT4V_T);
        memcpy(desbuffer,(uint8 *)&INT4V_T,sizeof(OOP_INT4V_T));
    }
    break;
    case E_DATETIME_S://时间
                {
        OOP_DATETIME_S_T DATETIME_S_T;

        memset((uint8*)&DATETIME_S_T,0x00,sizeof(OOP_DATETIME_S_T));

        memcpy_r((uint8*)&DATETIME_S_T.year,soucerbuffer,2);
        memcpy((uint8*)&DATETIME_S_T.month,soucerbuffer+2,6);
        if(DATETIME_S_T.second == 0xff)
            DATETIME_S_T.second =0;
        memcpy(desbuffer,(uint8*)&DATETIME_S_T,sizeof(OOP_DATETIME_S_T));   
        *deslength=sizeof(OOP_DATETIME_S_T);
    }
                    break;
    case E_DOUBLE_LONG_UNSIGNED:
    case E_DOUBLE_LONG:
    {

        memcpy_r(desbuffer,soucerbuffer,4);    
        *deslength=4;
                }
    break;
    case E_LONG_UNSIGNED:
    case E_LONG:
                {

        memcpy_r(desbuffer,soucerbuffer,2);    
        *deslength=2;
                }
    break;
    case E_UNSIGNED:
    case E_ENUM:
                {
        memcpy_r(desbuffer,soucerbuffer,1);    
        *deslength=1;
                }
        break;

    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T LONG4V_T;
  
        memset((uint8*)&LONG4V_T,0xff,sizeof(OOP_LONG4V_T));  
        LONG4V_T.nNum=4;     
        LONG4V_T.rsv=0;

        memcpy_r((uint8 *)&LONG4V_T.nValue[index],soucerbuffer,2);
        set_bit_value((uint8*)&LONG4V_T.rsv, 32, index);
        
        *deslength=sizeof(OOP_LONG4V_T);
        memcpy(desbuffer,(uint8 *)&LONG4V_T,sizeof(OOP_LONG4V_T));
    }
        
    break;

    case E_OOP_LONG3V_T:
        {
            OOP_LONG3V_T LONG3V_T;

            memset((uint8*)&LONG3V_T,0xff,sizeof(OOP_LONG3V_T));

            {
                LONG3V_T.nNum=3;
        }
            LONG3V_T.rsv=0;

            {
                memcpy_r((uint8 *)&LONG3V_T.nValue[index],soucerbuffer,2);
                set_bit_value((uint8*)&LONG3V_T.rsv, 32, index);
            }
            *deslength=sizeof(LONG3V_T);
            memcpy(desbuffer,(uint8 *)&LONG3V_T,sizeof(OOP_LONG3V_T));
        }
        break;

 
    default:   
    break;
    }
}

/****************************************************************************
*函数名称：taskmng_meter_data_save
*功能：     存储抄表数据
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void taskmng_meter_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe )
{
    WRITE_RECORD_T inData;

    int ret;
    uint32 storeNo=0;

    uint8 flag = 0;//0表示直接插入 1表示需要更新记录
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    OOP_OAD_U oad = StoreInfo.recordOAD.road.oadCols.oad[0];
    OOP_OAD_U saveoad = oad;
    //saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T ReadRecord;
    memset(&inData,0X00,sizeof(WRITE_RECORD_T));
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));
    //taskmng_oad_info_get( oad,&pOadInfo);
#if IS_NEED_ADD_8HOUR
    StoreInfo.colStoreTm+=8*60*60;
    StoreInfo.colStartTm+=8*60*60;
    StoreInfo.colEndTm+=8*60*60;
#endif
    //如果是array格式 则认为是总分格式的 如果oad是分 则存到总里面 先查询数据库 如果有该记录 则更新 如果没有则插入
    //每次抄都会带上冻结时标 如果多帧抄读就会存多个冻结时标 需要过滤
    if(TRUE == taskmng_oad_info_get( oad,&pOadInfo) &&(0x20210200==oad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
        //TASK_FMT_TRACE( pipe, REC_LOG, "saveoad is %x\n",saveoad);

        ReadRecord.recordOAD = StoreInfo.recordOAD;
        ReadRecord.recordOAD.road.oadCols.oad[0] = saveoad;
       // TASK_FMT_TRACE( pipe, REC_LOG, "总分数据项方案中oad %08x saveoad %08x \n",oad.value,saveoad.value);
        ReadRecord.cType = COL_TM_STORE; //以存储时间筛选

        ReadRecord.cStart = StoreInfo.colStoreTm;
        ReadRecord.cEnd = StoreInfo.colStoreTm;
        ReadRecord.sortType = 0;
        ReadRecord.MAC.nNum = 6;
        //MAC校验也用大端
        memcpy_r(ReadRecord.MAC.value,StoreInfo.MAC.nValue,6);
        ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret ==0)
        {
            //如果能查询到数据则说明需要更新
            if(FALSE == RecordMerge(inDataBuf,RecordOut.record[0].outDataBuf,mergebuf,&mergelen,&pOadInfo))
            {
                memcpy(mergebuf,inDataBuf,inDatalen);
                mergelen = inDatalen;
            }

            flag = 1;
        }
    }

    if(flag == 1)
    {
        update.recordOAD = StoreInfo.recordOAD;
        update.recordOAD.road.oadCols.oad[0] = saveoad;
        update.filter.choice = 1;

        update.filter.storeTm = StoreInfo.colStoreTm;
        update.MAC.nNum = 6;
        memcpy_r(update.MAC.value,StoreInfo.MAC.nValue,6);
        update.inDatalen = mergelen;
        memcpy(update.inDataBuf,mergebuf,mergelen);

        ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "更新错误 ret is %d\n",ret);
        }
    }else
    {
        inData.recordOAD=StoreInfo.recordOAD;
        inData.recordOAD.road.oadCols.oad[0] = saveoad;
        
        inData.MAC.nNum = 6;
        memcpy_r(inData.MAC.value,StoreInfo.MAC.nValue,6);

        inData.colStartTm=StoreInfo.colStartTm;
        inData.colStoreTm=StoreInfo.colStoreTm;
        inData.colEndTm=StoreInfo.colEndTm;
        //电压合格率需要特殊处理下
        if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
        {
            OOP_VOLTSTAT_T record;
            memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
            if(oad.nIndex ==1)
            {
                memcpy(&record.day,inDataBuf,inDatalen);
            }else
            {
                memcpy(&record.mon,inDataBuf,inDatalen);
            }
            inData.inDatalen=sizeof(OOP_VOLTSTAT_T);
            memcpy(inData.inDataBuf,&record,sizeof(OOP_VOLTSTAT_T));
        }else
        {
            inData.inDatalen=inDatalen;
            memcpy(inData.inDataBuf,inDataBuf,inDatalen);
        }
            //加点打印 统计下时钟超差的表
        if(inData.recordOAD.road.oadMain.value ==0 && inData.recordOAD.road.oadCols.oad[0].value == 0x40000200)
        {
            OOP_DATETIME_S_T datetimes;
            time_t tmptime = 0;
            memset(&datetimes,0,sizeof(datetimes));
            memcpy(&datetimes,inDataBuf,inDatalen);
            DT_DateTimeShort2Time(&datetimes,&tmptime);
            if(abs(time(NULL) - tmptime) > 300)
            {
                TASK_FMT_DEBUG(pipe,REC_LOG,"电表 %d 时钟超差 \n",inData.recordOAD.infoNum);
            }
        }

        ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);

        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "写失败 ret is %d\n",ret);
        }
    }
    

    
}


/****************************************************************************
*函数名称：taskmng_meter_data_save
*功能：     存储抄表数据
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void taskmng_meter_tg_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,DB_CLIENT      DBClientHandle)
{
    WRITE_RECORD_T inData;

    int ret;
    uint32 storeNo=0;

    uint8 flag = 0;//0表示直接插入 1表示需要更新记录
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    OOP_OAD_U oad = StoreInfo.recordOAD.road.oadCols.oad[0];
    OOP_OAD_U saveoad = oad;
    //saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T ReadRecord;
    memset(&inData,0X00,sizeof(WRITE_RECORD_T));
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));
    //taskmng_oad_info_get( oad,&pOadInfo);
#if IS_NEED_ADD_8HOUR
    //StoreInfo.colStoreTm+=8*60*60;
    //StoreInfo.colStartTm+=8*60*60;
    //StoreInfo.colEndTm+=8*60*60;
#endif
    //如果是array格式 则认为是总分格式的 如果oad是分 则存到总里面 先查询数据库 如果有该记录 则更新 如果没有则插入
    //每次抄都会带上冻结时标 如果多帧抄读就会存多个冻结时标 需要过滤
    if(TRUE == taskmng_oad_info_get( oad,&pOadInfo) &&(0x20210200==oad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
        //TASK_FMT_TRACE( pipe, REC_LOG, "saveoad is %x\n",saveoad);

        ReadRecord.recordOAD = StoreInfo.recordOAD;
        ReadRecord.recordOAD.road.oadCols.oad[0] = saveoad;
       // TASK_FMT_TRACE( pipe, REC_LOG, "总分数据项方案中oad %08x saveoad %08x \n",oad.value,saveoad.value);
        ReadRecord.cType = COL_TM_STORE; //以存储时间筛选

        ReadRecord.cStart = StoreInfo.colStoreTm;
        ReadRecord.cEnd = StoreInfo.colStoreTm;
        ReadRecord.sortType = 0;
        ReadRecord.MAC.nNum = 6;
        //MAC校验也用大端
        memcpy_r(ReadRecord.MAC.value,StoreInfo.MAC.nValue,6);
        ret = db_read_record(DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret ==0)
        {
            //如果能查询到数据则说明需要更新
            if(FALSE == RecordMerge(inDataBuf,RecordOut.record[0].outDataBuf,mergebuf,&mergelen,&pOadInfo))
            {
                memcpy(mergebuf,inDataBuf,inDatalen);
                mergelen = inDatalen;
            }

            flag = 1;
        }
    }

    if(flag == 1)
    {
        update.recordOAD = StoreInfo.recordOAD;
        update.recordOAD.road.oadCols.oad[0] = saveoad;
        update.filter.choice = 1;

        update.filter.storeTm = StoreInfo.colStoreTm;
        update.MAC.nNum = 6;
        memcpy_r(update.MAC.value,StoreInfo.MAC.nValue,6);
        update.inDatalen = mergelen;
        memcpy(update.inDataBuf,mergebuf,mergelen);

        ret = db_update_record(DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE ( -1,TG_LOG, "更新错误 ret is %d\n",ret);
        }
    }else
    {
        inData.recordOAD=StoreInfo.recordOAD;
        inData.recordOAD.road.oadCols.oad[0] = saveoad;
        
        inData.MAC.nNum = 6;
        memcpy_r(inData.MAC.value,StoreInfo.MAC.nValue,6);

        inData.colStartTm=StoreInfo.colStartTm;
        inData.colStoreTm=StoreInfo.colStoreTm;
        inData.colEndTm=StoreInfo.colEndTm;
        //电压合格率需要特殊处理下
        if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
        {
            OOP_VOLTSTAT_T record;
            memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
            if(oad.nIndex ==1)
            {
                memcpy(&record.day,inDataBuf,inDatalen);
            }else
            {
                memcpy(&record.mon,inDataBuf,inDatalen);
            }
            inData.inDatalen=sizeof(OOP_VOLTSTAT_T);
            memcpy(inData.inDataBuf,&record,sizeof(OOP_VOLTSTAT_T));
        }else
        {
            inData.inDatalen=inDatalen;
            memcpy(inData.inDataBuf,inDataBuf,inDatalen);
        }
    

        ret = db_write_record(DBClientHandle, &inData, &storeNo);
        if(ret != 0)
        {
            TASK_FMT_TRACE ( -1,TG_LOG, "写失败 ret is %d\n",ret);
        }
    }
    

    
}

/****************************************************************************
*函数名称：taskmng_meter_data_save
*功能：     存储抄表数据
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void taskmng_meter188_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe )
{
    WRITE_RECORD_T inData;

    int ret;
    uint32 storeNo=0;

    uint8 flag = 0;//0表示直接插入 1表示需要更新记录
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    OOP_OAD_U oad = StoreInfo.recordOAD.road.oadCols.oad[0];
    OOP_OAD_U saveoad = oad;
    //saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T ReadRecord;
    memset(&inData,0X00,sizeof(WRITE_RECORD_T));
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));
    //taskmng_oad_info_get( oad,&pOadInfo);
#if IS_NEED_ADD_8HOUR
    StoreInfo.colStoreTm+=8*60*60;
    StoreInfo.colStartTm+=8*60*60;
    StoreInfo.colEndTm+=8*60*60;
#endif
    //如果是array格式 则认为是总分格式的 如果oad是分 则存到总里面 先查询数据库 如果有该记录 则更新 如果没有则插入
    //每次抄都会带上冻结时标 如果多帧抄读就会存多个冻结时标 需要过滤
    if(TRUE == taskmng_oad_info_get( oad,&pOadInfo) &&(0x20210200==oad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
        //TASK_FMT_TRACE( pipe, REC_LOG, "saveoad is %x\n",saveoad);

        ReadRecord.recordOAD = StoreInfo.recordOAD;
        ReadRecord.recordOAD.road.oadCols.oad[0] = saveoad;
       // TASK_FMT_TRACE( pipe, REC_LOG, "总分数据项方案中oad %08x saveoad %08x \n",oad.value,saveoad.value);
        ReadRecord.cType = COL_TM_STORE; //以存储时间筛选

        ReadRecord.cStart = StoreInfo.colStoreTm;
        ReadRecord.cEnd = StoreInfo.colStoreTm;
        ReadRecord.sortType = 0;
        ReadRecord.MAC.nNum = 6;
        //MAC校验也用大端
        memcpy_r(ReadRecord.MAC.value,StoreInfo.MAC.nValue,6);
        ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret ==0)
        {
            //如果能查询到数据则说明需要更新
            if(FALSE == RecordMerge(inDataBuf,RecordOut.record[0].outDataBuf,mergebuf,&mergelen,&pOadInfo))
            {
                memcpy(mergebuf,inDataBuf,inDatalen);
                mergelen = inDatalen;
            }

            flag = 1;
        }
    }

    if(flag == 1)
    {
        update.recordOAD = StoreInfo.recordOAD;
        update.recordOAD.road.oadCols.oad[0] = saveoad;
        update.filter.choice = 1;

        update.filter.storeTm = StoreInfo.colStoreTm;
        update.MAC.nNum = 0;
        memcpy_r(update.MAC.value,StoreInfo.MAC.nValue,6);
        update.inDatalen = mergelen;
        memcpy(update.inDataBuf,mergebuf,mergelen);

        ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "更新错误 ret is %d\n",ret);
        }
    }else
    {
        inData.recordOAD=StoreInfo.recordOAD;
        inData.recordOAD.road.oadCols.oad[0] = saveoad;
        
        inData.MAC.nNum = 0;
        memcpy_r(inData.MAC.value,StoreInfo.MAC.nValue,6);

        inData.colStartTm=StoreInfo.colStartTm;
        inData.colStoreTm=StoreInfo.colStoreTm;
        inData.colEndTm=StoreInfo.colEndTm;
        //电压合格率需要特殊处理下
        if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
        {
            OOP_VOLTSTAT_T record;
            memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
            if(oad.nIndex ==1)
            {
                memcpy(&record.day,inDataBuf,inDatalen);
            }else
            {
                memcpy(&record.mon,inDataBuf,inDatalen);
            }
            inData.inDatalen=sizeof(OOP_VOLTSTAT_T);
            memcpy(inData.inDataBuf,&record,sizeof(OOP_VOLTSTAT_T));
        }else
        {
            inData.inDatalen=inDatalen;
            memcpy(inData.inDataBuf,inDataBuf,inDatalen);
        }
    
#if DBCENTER_MQTT
        ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
//      #if AREA_JIANGSU
//        min_gather_input(gAmrThreadArg[pipe].DBClientHandle, &inData, 1);
//      #endif
#else
        ret=record_data_write(gAmrThreadArg[pipe].DataCenterHandle, inData, &storeNo);
#endif
        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "写失败 ret is %d\n",ret);
        }
    }
    

    
}

/*******************************************************************************
* 函数名称: taskmng_oad_info_get
* 函数功能: 
* 输入参数: OOP_OAD_U oad      //要查询的OAD
            OAD_LIST_T *OadMap  //OAD信息结构体指针
* 输出参数: 返回在oad的信息；
* 返 回 值:  TRUE或者FALSE
*******************************************************************************/
BOOL taskmng_oad_info_get(OOP_OAD_U oad, OadInfoMap *OadMap)
{
    uint8 OIA1,OIA2;
    uint16 rowid=0;
    uint32 len;
    const OadInfoMap *tmpOadMap =NULL;
    const OadInfoMap *backOadMap =NULL;
    OIA1=(oad.nObjID>>12)&0x0F;
    OIA2=(oad.nObjID>>8)&0x0F;
//  printf("OIA1 is: %X\n",OIA1);
//  printf("OIA2 is: %X\n",OIA2);
    switch(OIA1)
    {
        case 0:
            tmpOadMap=EnergyList;
            len=sizeof(EnergyList)/sizeof(OadInfoMap);
//          printf("len is %d\n",len);
        break;
        case 1:
            tmpOadMap=DemandList;
            len=sizeof(DemandList)/sizeof(OadInfoMap);
        break;
        case 2:
            tmpOadMap=VariateList;
            len=sizeof(VariateList)/sizeof(OadInfoMap);
        break;
        case 3:
            tmpOadMap=EventList;
            len=sizeof(EventList)/sizeof(OadInfoMap);
        break;
        case 4:
            tmpOadMap=ParaList;
            len=sizeof(ParaList)/sizeof(OadInfoMap);
        break;
        case 5:
            tmpOadMap=FreezeList;
            len=sizeof(FreezeList)/sizeof(OadInfoMap);
        break;
        case 6:
            tmpOadMap=CollectList;
            len=sizeof(CollectList)/sizeof(OadInfoMap);
        break;
        case 7:
            tmpOadMap=GatherList;
            len=sizeof(GatherList)/sizeof(OadInfoMap);
        break;
        case 8:
            tmpOadMap=ControlList;
            len=sizeof(ControlList)/sizeof(OadInfoMap);
        break;
        case 9:
            tmpOadMap=OrderedchargeList;
            len=sizeof(OrderedchargeList)/sizeof(OadInfoMap);
        break;
        case 0x0F:
            if(OIA2==0x00)
            {
                tmpOadMap=FileList;
                len=sizeof(FileList)/sizeof(OadInfoMap);
            }
            else if(OIA2==0x01)
            {
                tmpOadMap=EsamList;
                len=sizeof(EsamList)/sizeof(OadInfoMap);
            }
            else if(OIA2==0x02)
            {
                tmpOadMap=IoList;
                len=sizeof(IoList)/sizeof(OadInfoMap);
            }
            else if(OIA2==0x03)
            {
                tmpOadMap=GuiList;
                len=sizeof(GuiList)/sizeof(OadInfoMap);
            }
            else
                return FALSE;
        break;
        default:
        return FALSE;
    }
    backOadMap = tmpOadMap;
    //先按照完全匹配筛选一遍
    while(rowid<len)
    {
        if((tmpOadMap->Oad.value==oad.value))
        {
            memcpy(OadMap,tmpOadMap,sizeof(OadInfoMap));
            OadMap->Oad = oad;       
            return TRUE;
        }
        rowid++;
        tmpOadMap++;
    }
    rowid=0;
    tmpOadMap = backOadMap;
    while(rowid<len)
    {
        if((tmpOadMap->Oad.nObjID==oad.nObjID)&&(tmpOadMap->Oad.attID==oad.attID))
        {
//          printf("rowid is %d\n",rowid);
            memcpy(OadMap,tmpOadMap,sizeof(OadInfoMap));
            OadMap->Oad = oad;       
            return TRUE;
        }
        rowid++;
        tmpOadMap++;
    }
    return FALSE;
}

//判断任务用哪张表  返回值判断是否用实时表抄 0表示实时 1表示非实时 -1表示未找到
 int CheckMapFromMainOAD(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,const MapOopTo645_NEW** pMap,uint16 *MapNum)
 {
    uint8 PrtlMode = 0;
    uint16 Pn;
    uint8 logicid;
    OOP_OAD_U MainOAD;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    logicid = pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
    taskmng_mtr_prtl_get(Pn,logicid,&PrtlMode);
    memcpy((uint8*)&MainOAD,(uint8*)&Tmp_AmrLink->CsdTask.RoadTask[0].MainOad,sizeof(OOP_OAD_U));
    int ret = 0 ;
    if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_NULL || PrtlMode == PRTL_97)
    {
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_RT;
        return ret;
    }
    if(MainOAD.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0xff && PrtlMode == PRTL_07)
    {
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_RT;
        return ret;
    }
    switch(MainOAD.nObjID)
    {
    case 0x0000:  //实时数据
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_RT;
        ret = 0;
        break;

    case 0x5000:  //瞬时冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_Instant)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_Instant;
        ret = 1;
        break;

    case 0x5002:  //分钟冻结(负荷曲线)
        //先判断是否是单相表
        if(taskmng_mtr_conntype_get(Pn,logicid)==1)
        {
           //判断flag
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_MinOne)/sizeof(MapOopTo645_NEW);
                *pMap = MapOopTo645_MinOne;
                ret = 1;
            }else
            {
                //负荷曲线时标不对则按照实时抄读 
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 1 ||taskmng_task_period_check(Tmp_AmrLink)==TRUE)
                {
                    if(MapNum != NULL)
                        *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
                    *pMap = MapOopTo645_RT;
                    ret = 0;
                }else
                {
                    if(MapNum != NULL)
                        *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
                    *pMap = MapOopTo645_Min;
                    ret = 1;
                }
            }
            
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE)
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
//                *pMap = MapOopTo645_RT;
//                ret = 0;
//            }else
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_MinOne)/sizeof(MapOopTo645_NEW);
//                *pMap = MapOopTo645_MinOne;
//                ret = 1;
//            }
        }else
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645 == 0)
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
                *pMap =MapOopTo645_Min;
                ret = 1;
            }else
            {
                if(MapNum != NULL)
                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
                *pMap = MapOopTo645_RT;
                ret = 0;
            }
        
//            if(taskmng_task_period_check(Tmp_AmrLink)==TRUE||taskmng_task_period_get(Tmp_AmrLink->TaskId,logicid) < 86400L)
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
//                *pMap = MapOopTo645_RT;
//                ret = 0;
//            }else
//            {
//                if(MapNum != NULL)
//                    *MapNum = sizeof(MapOopTo645_Min)/sizeof(MapOopTo645_NEW);
//                *pMap =MapOopTo645_Min;
//                ret = 1;
//            }
#ifdef AREA_ANHUI
            //ret = 1;
#endif
        }

        break;

    case 0x5003:  //小时冻结
        #if MET07_RT_DUMP_CURVE_MIN_EN == 1
        if(taskmng_task_period_get(Tmp_AmrLink->TaskId,logicid) < 86400L)
        {
            if(MapNum != NULL)
                *MapNum = sizeof(MapOopTo645_RT)/sizeof(MapOopTo645_NEW);
            *pMap = MapOopTo645_RT;
            ret = 0;
        }
        else      //周期大于1天仍通过负荷曲线抄读
        {
            if(MapNum != NULL)
                *MapNum = sizeof(MapOopTo645_Hour)/sizeof(MapOopTo645_NEW);
            *pMap = MapOopTo645_Hour;
            ret = 1;
        }
        #else
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_Hour)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_Hour;
        ret =1;
        #endif
        break;

    case 0x5004:  //日冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_DAY)/sizeof(MapOopTo645_NEW);
        *pMap =MapOopTo645_DAY;
        ret = 1;
        break;

    case 0x5005:  //结算日
    case 0x5006:  //月冻结
        if(MapNum != NULL)
            *MapNum = sizeof(MapOopTo645_MON)/sizeof(MapOopTo645_NEW);
        *pMap = MapOopTo645_MON;
        ret = 1;
        break;

    default:
        return -1;
    }
    return ret;
 }
/****************************************************************************
*函数名称：taskmng_need_merge_check
*功能：     查找那些类型需要分抄整存
*输入参数：
*输出参数：
*返回：TRUE 表示需要分抄整存
        FALSE 表示不需要分抄整存
****************************************************************************/

BOOL taskmng_need_merge_check(OadInfoMap *pOadInfo )
{

    switch(pOadInfo->CovertSruct)
    {
    case E_OOP_ENERGY_T:
    case E_OOP_ENERGYS_T:
    case E_OOP_HENERGY_T:
    case E_OOP_HENERGYS_T:
    case E_OOP_DEMAND_T:  //需量
    case E_OOP_DEMANDS_T:
    case E_OOP_DWORD3V_T://分相变量
    case E_OOP_INT4V_T://功率变量
    case E_OOP_WORD3V_T://电压
    case E_OOP_INT3V_T://电流
    case E_OOP_METWORD_T:
    case E_OOP_LONG4V_T:
    case E_OOP_VOLTQR_T:  //电压合格率数据
    case E_OOP_EVTSTAT_T:
    case E_OOP_EVTSTAT1_T:
    case E_OOP_EVENTTIME_T:
    case E_OOP_EVENTTIME1_T:
    case E_OOP_RS485_T:
    case E_OOP_ROUTEMODULE_T:
    case E_OOP_DAYPERIOD_T:
        return TRUE;
    
    default:
    return FALSE;
        
    }
}

/****************************************************************************
*函数名称：Check_Data_Struct
*功能：     查找那些OAD对应的结构体类型
*输入参数：
*输出参数：
*返回：TRUE 表示有分量
        FALSE 表示没有分量
****************************************************************************/

BOOL Check_Data_Struct(DATASTRUCT     DataStruct)
{

    switch(DataStruct)
    {
    case E_OOP_ENERGY_T:
    case E_OOP_ENERGYS_T:
    case E_OOP_HENERGY_T:
    case E_OOP_HENERGYS_T:
    case E_OOP_DEMAND_T:  //需量
    case E_OOP_DEMANDS_T:
    case E_OOP_DWORD3V_T://分相变量
    case E_OOP_INT4V_T://功率变量
    case E_OOP_WORD3V_T://电压
    case E_OOP_INT3V_T://电流
    case E_OOP_METWORD_T:
    case E_OOP_LONG4V_T:
        return TRUE;
    
    default:
    return FALSE;
        
    }
}


//将新插入的记录数据和数据中心中的记录合并
BOOL RecordMerge(uint8* inData,uint8* recordData,uint8* mergeData,uint16 *outLen,OadInfoMap *pOadInfo )
{
    uint8 index = pOadInfo->Oad.nIndex;
    uint8 i;
    switch(pOadInfo->CovertSruct)
    {
    case E_OOP_DAYPERIOD_T:
    {
        OOP_DAYPERIOD_T in;
        OOP_DAYPERIOD_T record;
        memcpy(&in,inData,sizeof(OOP_DAYPERIOD_T));
        memcpy(&record,recordData,sizeof(OOP_DAYPERIOD_T));
        for(i=0;i<OOP_MAX_DAYPERIODS;i++)
        {
           if(get_bit_value((uint8*)&in.rsv,32,i))
                record.dPeriod[i] = in.dPeriod[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_DAYPERIOD_T));
        *outLen = sizeof(OOP_DAYPERIOD_T);

    }
        break;
    case E_OOP_RS485_T:
    {
        OOP_SERIALPORT_T in;
        OOP_SERIALPORT_T record;
        memcpy(&in,inData,sizeof(OOP_SERIALPORT_T));
        memcpy(&record,recordData,sizeof(OOP_SERIALPORT_T));
        for(i=0;i<OOP_MAX_COM;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.com[i] = in.com[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_SERIALPORT_T));
        *outLen = sizeof(OOP_SERIALPORT_T);

    }
        break;
    case E_OOP_ENERGY_T:
    {
        OOP_ENERGY_T in;
        OOP_ENERGY_T record;
        memcpy(&in,inData,sizeof(OOP_ENERGY_T));
        memcpy(&record,recordData,sizeof(OOP_ENERGY_T));
        for(i=0;i<OOP_FULL_RATES;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_ENERGY_T));
        *outLen = sizeof(OOP_ENERGY_T);
    }
        break;
    case E_OOP_ENERGYS_T:
    {
        OOP_ENERGYS_T in;
        OOP_ENERGYS_T record;
        memcpy(&in,inData,sizeof(OOP_ENERGYS_T));
        memcpy(&record,recordData,sizeof(OOP_ENERGYS_T));
        for(i=0;i<OOP_FULL_RATES;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_ENERGYS_T));
        *outLen = sizeof(OOP_ENERGYS_T);
    }
        break;
    case E_OOP_HENERGY_T:
    {
        OOP_HENERGY_T in;
        OOP_HENERGY_T record;
        memcpy(&in,inData,sizeof(OOP_HENERGY_T));
        memcpy(&record,recordData,sizeof(OOP_HENERGY_T));
        for(i=0;i<OOP_FULL_RATES;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_HENERGY_T));
        *outLen = sizeof(OOP_HENERGY_T);
    }
        break;
    case E_OOP_HENERGYS_T:
    {
        OOP_HENERGYS_T in;
        OOP_HENERGYS_T record;
        memcpy(&in,inData,sizeof(OOP_HENERGYS_T));
        memcpy(&record,recordData,sizeof(OOP_HENERGYS_T));
        for(i=0;i<OOP_FULL_RATES;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_HENERGYS_T));
        *outLen = sizeof(OOP_HENERGYS_T);
    }
        break;
    case E_OOP_DEMAND_T:  //需量
    case E_OOP_DEMANDS_T:
    {
        OOP_DEMAND_T in;
        OOP_DEMAND_T record;
        memcpy(&in,inData,sizeof(OOP_DEMAND_T));
        memcpy(&record,recordData,sizeof(OOP_DEMAND_T));
        for(i=0;i<OOP_FULL_RATES;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.demand[i] = in.demand[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_DEMAND_T));
        *outLen = sizeof(OOP_DEMAND_T);
    }
        break;
    case E_OOP_DWORD3V_T://分相变量
    {
        OOP_DWORD3V_T in;
        OOP_DWORD3V_T record;
        memcpy(&in,inData,sizeof(OOP_DWORD3V_T));
        memcpy(&record,recordData,sizeof(OOP_DWORD3V_T));
        for(i=0;i<3;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_DWORD3V_T));
        *outLen = sizeof(OOP_DWORD3V_T);
    }
        break;
    case E_OOP_WORD3V_T://电压
    {
        OOP_WORD3V_T in;
        OOP_WORD3V_T record;
        memcpy(&in,inData,sizeof(OOP_WORD3V_T));
        memcpy(&record,recordData,sizeof(OOP_WORD3V_T));
        for(i=0;i<3;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_WORD3V_T));
        *outLen = sizeof(OOP_WORD3V_T);
    }
        break;
    case E_OOP_INT3V_T://电流
    {
        OOP_INT3V_T in;
        OOP_INT3V_T record;
        memcpy(&in,inData,sizeof(OOP_INT3V_T));
        memcpy(&record,recordData,sizeof(OOP_INT3V_T));
        for(i=0;i<3;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_INT3V_T));
        *outLen  = sizeof(OOP_INT3V_T);
    }
        break;
    case E_OOP_INT4V_T://功率变量
    {
        OOP_INT4V_T in;
        OOP_INT4V_T record;
        memcpy(&in,inData,sizeof(OOP_INT4V_T));
        memcpy(&record,recordData,sizeof(OOP_INT4V_T));
        for(i=0;i<4;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_INT4V_T));
        *outLen = sizeof(OOP_INT4V_T);
    }
        break;
    case E_OOP_METWORD_T:
    {
        OOP_METWORD_T in;
        OOP_METWORD_T record;
        memcpy(&in,inData,sizeof(OOP_METWORD_T));
        memcpy(&record,recordData,sizeof(OOP_METWORD_T));
        for(i=0;i<OOP_METWORD_NUM;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.metWord[i] = in.metWord[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_METWORD_T));
        *outLen = sizeof(OOP_METWORD_T);
    }
        break;
    case E_OOP_LONG4V_T:
    {
        OOP_LONG4V_T in;
        OOP_LONG4V_T record;
        memcpy(&in,inData,sizeof(OOP_LONG4V_T));
        memcpy(&record,recordData,sizeof(OOP_LONG4V_T));
        for(i=0;i<4;i++)
        {
            if(get_bit_value((uint8*)&in.rsv,32,i))
                record.nValue[i] = in.nValue[i];
        }
        record.rsv = record.rsv|in.rsv;
        memcpy(mergeData,&record,sizeof(OOP_LONG4V_T));
        *outLen = sizeof(OOP_LONG4V_T);
    }
        break;
    case E_OOP_VOLTQR_T:    //电压合格率
    {
        OOP_VOLTSTAT_T record;
        OOP_VOLTQR_T in;
        memcpy(&in,inData,sizeof(OOP_VOLTQR_T));
        memcpy(&record,recordData,sizeof(OOP_VOLTSTAT_T));
        if(index ==1)
        {
            memcpy(&record.day,&in,sizeof(OOP_VOLTQR_T));
        }else if(index ==2)
        {
            memcpy(&record.mon,&in,sizeof(OOP_VOLTQR_T));
        }else
        {
            return FALSE;   
        }
        memcpy(mergeData,&record,sizeof(OOP_VOLTSTAT_T));
        *outLen = sizeof(OOP_VOLTSTAT_T);
    }
        break;
    case E_OOP_EVTSTAT_T:
    {
        OOP_EVTSTAT_T in;
        OOP_EVTSTAT_T record;
        memcpy(&in,inData,sizeof(OOP_EVTSTAT_T));
        memcpy(&record,recordData,sizeof(OOP_EVTSTAT_T));
        for(i=0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(get_bit_value((uint8*)&in.index,32,i))
                record.stat[i] = in.stat[i];
        }
        record.index = record.index|in.index;
        memcpy(mergeData,&record,sizeof(OOP_EVTSTAT_T));
        *outLen = sizeof(OOP_EVTSTAT_T);
    }
        break;   
    case E_OOP_EVTSTAT1_T:
    {
        OOP_MEVTSTAT_T in;
        OOP_MEVTSTAT_T record;
        memcpy(&in,inData,sizeof(OOP_MEVTSTAT_T));
        memcpy(&record,recordData,sizeof(OOP_MEVTSTAT_T));
        for(i=0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(get_bit_value((uint8*)&in.index,32,i))
                record.stat[i] = in.stat[i];
        }
        record.index = record.index|in.index;
        memcpy(mergeData,&record,sizeof(OOP_MEVTSTAT_T));
        *outLen = sizeof(OOP_MEVTSTAT_T);
    }
        break;
    case E_OOP_EVENTTIME_T:
    {
        OOP_EVTTIME_T in;
        OOP_EVTTIME_T record;
        memcpy(&in,inData,sizeof(OOP_EVTTIME_T));
        memcpy(&record,recordData,sizeof(OOP_EVTTIME_T));
        for(i=0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(get_bit_value((uint8*)&in.index,32,i))
                record.stat[i] = in.stat[i];
        }
        record.index = record.index|in.index;
        memcpy(mergeData,&record,sizeof(OOP_EVTTIME_T));
        *outLen = sizeof(OOP_EVTTIME_T);
    }
        break;
    case E_OOP_EVENTTIME1_T:
    {
        OOP_MEVTTIME_T in;
        OOP_MEVTTIME_T record;
        memcpy(&in,inData,sizeof(OOP_MEVTTIME_T));
        memcpy(&record,recordData,sizeof(OOP_MEVTTIME_T));
        for(i=0;i<OOP_MAX_EVTSTAT;i++)
        {
            if(get_bit_value((uint8*)&in.index,32,i))
                record.stat[i] = in.stat[i];
        }
        record.index = record.index|in.index;
        memcpy(mergeData,&record,sizeof(OOP_MEVTTIME_T));
        *outLen = sizeof(OOP_MEVTTIME_T);
    }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

const EventID_To_DI *GetEventIDToDIByIndex(uint8 IDIndex)
{
    int DICount = (sizeof(EventIDToDI) / sizeof(EventIDToDI[0])) - 1;       //EventIDToDI成员的数量
    int i = 0;

    for(i = 0; i < DICount; i++)
    {
        if(EventIDToDI[i].WorkStatBit == IDIndex)     //状态字对应的位信息
            return &EventIDToDI[i];
    }
    return NULL;
}

/****************************************************************************
*函数名称：taskmng_meter_data_save
*功能：     存储抄表数据
*输入参数：
*输出参数：
*返回：
****************************************************************************/

void taskmng_leak_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe ,uint32 intv)
{
    WRITE_RECORD_T inData;

    int ret;
    //uint32 storeNo=0;

    uint8 flag = 0;//0表示直接插入 1表示需要更新记录
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    OOP_OAD_U oad = StoreInfo.recordOAD.road.oadCols.oad[0];
    OOP_OAD_U saveoad = oad;
    //saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T ReadRecord;
    memset(&inData,0X00,sizeof(WRITE_RECORD_T));
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    uint8 mergebuf[DATA_LEN_MAX]={0};
    uint16 mergelen=0;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));
    //taskmng_oad_info_get( oad,&pOadInfo);
#if IS_NEED_ADD_8HOUR
    StoreInfo.colStoreTm+=8*60*60;
    StoreInfo.colStartTm+=8*60*60;
    StoreInfo.colEndTm+=8*60*60;
#endif
    //如果是array格式 则认为是总分格式的 如果oad是分 则存到总里面 先查询数据库 如果有该记录 则更新 如果没有则插入
    //每次抄都会带上冻结时标 如果多帧抄读就会存多个冻结时标 需要过滤
    if(TRUE == taskmng_oad_info_get( oad,&pOadInfo) &&(0x20210200==oad.value || TRUE==taskmng_need_merge_check(&pOadInfo)))
    {
        saveoad.nIndex = 0;         //如果是分项 存储的时候使用总
        //TASK_FMT_TRACE( pipe, REC_LOG, "saveoad is %x\n",saveoad);

        ReadRecord.recordOAD = StoreInfo.recordOAD;
        ReadRecord.recordOAD.road.oadCols.oad[0] = saveoad;
       // TASK_FMT_TRACE( pipe, REC_LOG, "总分数据项方案中oad %08x saveoad %08x \n",oad.value,saveoad.value);
        ReadRecord.cType = COL_TM_STORE; //以存储时间筛选

        ReadRecord.cStart = StoreInfo.colStoreTm;
        ReadRecord.cEnd = StoreInfo.colStoreTm;
        ReadRecord.sortType = 0;
        ReadRecord.MAC.nNum = 6;
        //MAC校验也用大端
        memcpy_r(ReadRecord.MAC.value,StoreInfo.MAC.nValue,6);
        ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret ==0)
        {
            //如果能查询到数据则说明需要更新
            if(FALSE == RecordMerge(inDataBuf,RecordOut.record[0].outDataBuf,mergebuf,&mergelen,&pOadInfo))
            {
                memcpy(mergebuf,inDataBuf,inDatalen);
                mergelen = inDatalen;
            }
            else
            {
                memset(inDataBuf,0x00,inDatalen);
                memcpy(inDataBuf,mergebuf,inDatalen);
            }
            
            flag = 1;
        }
    }

    if(flag == 1)
    {
        update.recordOAD = StoreInfo.recordOAD;
        update.recordOAD.road.oadCols.oad[0] = saveoad;
        update.filter.choice = 1;

        update.filter.storeTm = StoreInfo.colStoreTm;
        update.MAC.nNum = 6;
        memcpy_r(update.MAC.value,StoreInfo.MAC.nValue,6);
        update.inDatalen = mergelen;
        memcpy(update.inDataBuf,mergebuf,mergelen);

        ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "更新错误 ret is %d\n",ret);
        }
    }
    /**这里更新一下数据，不新增
        else
    {
        inData.recordOAD=StoreInfo.recordOAD;
        inData.recordOAD.road.oadCols.oad[0] = saveoad;
        
        inData.MAC.nNum = 6;
        memcpy_r(inData.MAC.value,StoreInfo.MAC.nValue,6);

        inData.colStartTm=StoreInfo.colStartTm;
        inData.colStoreTm=StoreInfo.colStoreTm;
        inData.colEndTm=StoreInfo.colEndTm;
        //电压合格率需要特殊处理下
        if(pOadInfo.CovertSruct == E_OOP_VOLTQR_T)
        {
            OOP_VOLTSTAT_T record;
            memset(&record,0x00,sizeof(OOP_VOLTSTAT_T));
            if(oad.nIndex ==1)
            {
                memcpy(&record.day,inDataBuf,inDatalen);
            }else
            {
                memcpy(&record.mon,inDataBuf,inDatalen);
            }
            inData.inDatalen=sizeof(OOP_VOLTSTAT_T);
            memcpy(inData.inDataBuf,&record,sizeof(OOP_VOLTSTAT_T));
        }else
        {
            inData.inDatalen=inDatalen;
            memcpy(inData.inDataBuf,inDataBuf,inDatalen);
        }
    
#if DBCENTER_MQTT
        ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
        #if AREA_JIANGSU
        min_gather_input(gAmrThreadArg[pipe].DBClientHandle, &inData, intv);
        #endif
#else
        ret=record_data_write(gAmrThreadArg[pipe].DataCenterHandle, inData, &storeNo);
#endif
        if(ret != 0)
        {
            TASK_FMT_TRACE ( pipe,COMM_LOG, "写失败 ret is %d\n",ret);
        }
    }**/
    

    
}

