/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：参数项定义
* @date： 2019.10.26
*********************************************************************
*/

#include "oadlist.h"

/* 电量量类OAD列表 OIA1 = 0x00 */
const OAD_LIST_T oadEnergyList[] = 
{
    /*OAD          class    A1      A2    lenType dataType  len*/
    {{0x00000200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //组合有功电能 - 电能量
    {{0x00000400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //组合有功电能 - 高精度电能量
    {{0x00100200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //正向有功电能 - 电能量
    {{0x00100400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //正向有功电能 - 高精度电能量
    {{0x00110200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相正向有功电能 - 电能量
    {{0x00110400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相正向有功电能 - 高精度电能量
    {{0x00120200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相正向有功电能 - 电能量
    {{0x00120400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相正向有功电能 - 高精度电能量
    {{0x00130200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相正向有功电能 - 电能量
    {{0x00130400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相正向有功电能 - 高精度电能量
    {{0x00200200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //反向有功电能 - 电能量
    {{0x00200400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //反向有功电能 - 高精度电能量
    {{0x00210200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相反向有功电能 - 电能量
    {{0x00210400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相反向有功电能 - 高精度电能量
    {{0x00220200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相反向有功电能 - 电能量
    {{0x00220400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相反向有功电能 - 高精度电能量
    {{0x00230200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相反向有功电能 - 电能量
    {{0x00230400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相反向有功电能 - 高精度电能量
    {{0x00300200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //组合无功1电能 - 电能量
    {{0x00300400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //组合无功1电能 - 高精度电能量
    {{0x00310200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //A相组合无功1电能 - 电能量
    {{0x00310400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //A相组合无功1电能 - 高精度电能量
    {{0x00320200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //B相组合无功1电能 - 电能量
    {{0x00320400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //B相组合无功1电能 - 高精度电能量
    {{0x00330200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //C相组合无功1电能 - 电能量
    {{0x00330400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //C相组合无功1电能 - 高精度电能量
    {{0x00400200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //组合无功2电能 - 电能量
    {{0x00400400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //组合无功2电能 - 高精度电能量
    {{0x00410200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //A相组合无功2电能 - 电能量
    {{0x00410400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //A相组合无功2电能 - 高精度电能量
    {{0x00420200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //B相组合无功2电能 - 电能量
    {{0x00420400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //B相组合无功2电能 - 高精度电能量
    {{0x00430200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},          //C相组合无功2电能 - 电能量
    {{0x00430400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},         //C相组合无功2电能 - 高精度电能量
    {{0x00500200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //第一象限无功电能 - 电能量
    {{0x00500400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //第一象限无功电能 - 高精度电能量
    {{0x00510200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相第一象限无功电能 - 电能量
    {{0x00510400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相第一象限无功电能 - 高精度电能量
    {{0x00520200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相第一象限无功电能 - 电能量
    {{0x00520400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相第一象限无功电能 - 高精度电能量
    {{0x00530200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相第一象限无功电能 - 电能量
    {{0x00530400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相第一象限无功电能 - 高精度电能量
    {{0x00600200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //第二象限无功电能 - 电能量
    {{0x00600400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //第二象限无功电能 - 高精度电能量
    {{0x00610200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相第二象限无功电能 - 电能量
    {{0x00610400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相第二象限无功电能 - 高精度电能量
    {{0x00620200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相第二象限无功电能 - 电能量
    {{0x00620400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相第二象限无功电能 - 高精度电能量
    {{0x00630200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相第二象限无功电能 - 电能量
    {{0x00630400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相第二象限无功电能 - 高精度电能量
    {{0x00700200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //第三象限无功电能 - 电能量
    {{0x00700400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //第三象限无功电能 - 高精度电能量
    {{0x00710200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相第三象限无功电能 - 电能量
    {{0x00710400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相第三象限无功电能 - 高精度电能量
    {{0x00720200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相第三象限无功电能 - 电能量
    {{0x00720400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相第三象限无功电能 - 高精度电能量
    {{0x00730200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相第三象限无功电能 - 电能量
    {{0x00730400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相第三象限无功电能 - 高精度电能量
    {{0x00800200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //第四象限无功电能 - 电能量
    {{0x00800400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //第四象限无功电能 - 高精度电能量
    {{0x00810200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相第四象限无功电能 - 电能量
    {{0x00810400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相第四象限无功电能 - 高精度电能量
    {{0x00820200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相第四象限无功电能 - 电能量
    {{0x00820400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相第四象限无功电能 - 高精度电能量
    {{0x00830200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相第四象限无功电能 - 电能量
    {{0x00830400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相第四象限无功电能 - 高精度电能量
    {{0x00900200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //正向视在电能 - 电能量
    {{0x00900400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //正向视在电能 - 高精度电能量
    {{0x00910200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相正向视在电能 - 电能量
    {{0x00910400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相正向视在电能 - 高精度电能量
    {{0x00920200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相正向视在电能 - 电能量
    {{0x00920400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相正向视在电能 - 高精度电能量
    {{0x00930200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相正向视在电能 - 电能量
    {{0x00930400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相正向视在电能 - 高精度电能量
    {{0x00A00200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //反向视在电能 - 电能量
    {{0x00A00400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //反向视在电能 - 高精度电能量
    {{0x00A10200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相反向视在电能 - 电能量
    {{0x00A10400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相反向视在电能 - 高精度电能量
    {{0x00A20200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相反向视在电能 - 电能量
    {{0x00A20400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相反向视在电能 - 高精度电能量
    {{0x00A30200}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相反向视在电能 - 电能量
    {{0x00A30400}, 1,       0x00,   0x00, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相反向视在电能 - 高精度电能量
    {{0x01100200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //正向有功基波总电能 - 电能量
    {{0x01100400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //正向有功基波总电能 - 高精度电能量
    {{0x01110200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相正向有功基波总电能 - 电能量
    {{0x01110400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相正向有功基波总电能 - 高精度电能量
    {{0x01120200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相正向有功基波总电能 - 电能量
    {{0x01120400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相正向有功基波总电能 - 高精度电能量
    {{0x01130200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相正向有功基波总电能 - 电能量
    {{0x01130400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相正向有功基波总电能 - 
    {{0x01200200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //反向有功基波总电能 - 电能量
    {{0x01200400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //反向有功基波总电能 - 高精度电能量
    {{0x01210200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相反向有功基波总电能 - 电能量
    {{0x01210400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相反向有功基波总电能 - 高精度电能量
    {{0x01220200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相反向有功基波总电能 - 电能量
    {{0x01220400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相反向有功基波总电能 - 高精度电能量
    {{0x01230200}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相反向有功基波总电能 - 电能量
    {{0x01230400}, 1,       0x00,   0x01, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相反向有功基波总电能 - 高精度电能量
    {{0x02100200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //正向有功谐波总电能 - 电能量
    {{0x02100400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //正向有功谐波总电能 - 高精度电能量
    {{0x02110200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相正向有功谐波总电能 - 电能量
    {{0x02110400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相正向有功谐波总电能 - 高精度电能量
    {{0x02120200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相正向有功谐波总电能 - 电能量
    {{0x02120400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相正向有功谐波总电能 - 高精度电能量
    {{0x02130200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相正向有功谐波总电能 - 电能量
    {{0x02130400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相正向有功谐波总电能 - 高精度电能量
    {{0x02200200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //反向有功谐波总电能 - 电能量
    {{0x02200400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //反向有功谐波总电能 - 高精度电能量
    {{0x02210200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相反向有功谐波总电能 - 电能量
    {{0x02210400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相反向有功谐波总电能 - 高精度电能量
    {{0x02220200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相反向有功谐波总电能 - 电能量
    {{0x02220400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相反向有功谐波总电能 - 高精度电能量
    {{0x02230200}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相反向有功谐波总电能 - 电能量
    {{0x02230400}, 1,       0x00,   0x02, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相反向有功谐波总电能 - 高精度电能量
    {{0x03000200}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //铜损有功总电能补偿量 - 电能量
    {{0x03000400}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //铜损有功总电能补偿量 - 高精度电能量
    {{0x03010200}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相铜损有功总电能补偿量 - 电能量
    {{0x03010400}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相铜损有功总电能补偿量 - 高精度电能量
    {{0x03020200}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相铜损有功总电能补偿量 - 电能量
    {{0x03020400}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相铜损有功总电能补偿量 - 高精度电能量
    {{0x03030200}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相铜损有功总电能补偿量 - 电能量
    {{0x03030400}, 1,       0x00,   0x03, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相铜损有功总电能补偿量 - 高精度电能量
    {{0x04000200}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //铁损有功总电能补偿量 - 电能量
    {{0x04000400}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //铁损有功总电能补偿量 - 高精度电能量
    {{0x04010200}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相铁损有功总电能补偿量 - 电能量
    {{0x04010400}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相铁损有功总电能补偿量 - 高精度电能量
    {{0x04020200}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相铁损有功总电能补偿量 - 电能量
    {{0x04020400}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相铁损有功总电能补偿量 - 高精度电能量
    {{0x04030200}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相铁损有功总电能补偿量 - 电能量
    {{0x04030400}, 1,       0x00,   0x04, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相铁损有功总电能补偿量 - 高精度电能量
    {{0x05000200}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //关联总电能 - 电能量
    {{0x05000400}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //关联总电能 - 高精度电能量
    {{0x05010200}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //A相关联总电能 - 电能量
    {{0x05010400}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //A相关联总电能 - 高精度电能量
    {{0x05020200}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //B相关联总电能 - 电能量
    {{0x05020400}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //B相关联总电能 - 高精度电能量
    {{0x05030200}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_ENERGYS_T),OOP_MAX_RATES+1},           //C相关联总电能 - 电能量
    {{0x05030400}, 1,       0x00,   0x05, 0,      DT_ARRAY, sizeof(OOP_HENERGYS_T),OOP_MAX_RATES+1},          //C相关联总电能 - 高精度电能量
};
const uint32 oadEnergyListLen = sizeof(oadEnergyList)/sizeof(oadEnergyList[0]);

/* 最大需量类OAD列表 OIA1 = 0x01 */
const OAD_LIST_T oadDemandList[] = 
{
    /*OAD          class    A1      A2    lenType dataType  len*/
    {{0x10100200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //正向有功最大需量
    {{0x10110200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相正向有功最大需量
    {{0x10120200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相正向有功最大需量
    {{0x10130200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相正向有功最大需量
    {{0x10200200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //反向有功最大需量
    {{0x10210200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相反向有功最大需量
    {{0x10220200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相反向有功最大需量
    {{0x10230200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相反向有功最大需量
    {{0x10300200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //组合无功1最大需量
    {{0x10310200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //A相组合无功1最大需量
    {{0x10320200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //B相组合无功1最大需量
    {{0x10330200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //C相组合无功1最大需量
    {{0x10400200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //组合无功2最大需量
    {{0x10410200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //A相组合无功2最大需量
    {{0x10420200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //B相组合无功2最大需量
    {{0x10430200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //C相组合无功2最大需量
    {{0x10500200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //第一象限最大需量
    {{0x10510200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相第一象限最大需量
    {{0x10520200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相第一象限最大需量
    {{0x10530200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相第一象限最大需量
    {{0x10600200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //第二象限最大需量
    {{0x10610200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相第二象限最大需量
    {{0x10620200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相第二象限最大需量
    {{0x10630200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相第二象限最大需量
    {{0x10700200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //第三象限最大需量
    {{0x10710200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相第三象限最大需量
    {{0x10720200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相第三象限最大需量
    {{0x10730200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相第三象限最大需量
    {{0x10800200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //第四象限最大需量
    {{0x10810200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相第四象限最大需量
    {{0x10820200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相第四象限最大需量
    {{0x10830200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相第四象限最大需量
    {{0x10900200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //正向视在最大需量
    {{0x10910200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相正向视在最大需量
    {{0x10920200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相正向视在最大需量
    {{0x10930200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相正向视在最大需量
    {{0x10A00200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //反向视在最大需量
    {{0x10A10200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //A相反向视在最大需量
    {{0x10A20200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //B相反向视在最大需量
    {{0x10A30200}, 2,       0x01,   0x00, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //C相反向视在最大需量
    {{0x11100200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内正向有功最大需量
    {{0x11110200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相正向有功最大需量
    {{0x11120200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相正向有功最大需量
    {{0x11130200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相正向有功最大需量
    {{0x11200200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内反向有功最大需量
    {{0x11210200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相反向有功最大需量
    {{0x11220200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相反向有功最大需量
    {{0x11230200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相反向有功最大需量
    {{0x11300200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内组合无功1最大需量
    {{0x11310200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内A相组合无功1最大需量
    {{0x11320200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内B相组合无功1最大需量
    {{0x11330200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内C相组合无功1最大需量
    {{0x11400200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内组合无功2最大需量
    {{0x11410200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内A相组合无功2最大需量
    {{0x11420200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内B相组合无功2最大需量
    {{0x11430200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMANDS_T),OOP_MAX_RATES+1},         //冻结周期内C相组合无功2最大需量
    {{0x11500200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内第一象限最大需量
    {{0x11510200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相第一象限最大需量
    {{0x11520200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相第一象限最大需量
    {{0x11530200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相第一象限最大需量
    {{0x11600200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内第二象限最大需量
    {{0x11610200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相第二象限最大需量
    {{0x11620200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相第二象限最大需量
    {{0x11630200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相第二象限最大需量
    {{0x11700200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内第三象限最大需量
    {{0x11710200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相第三象限最大需量
    {{0x11720200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相第三象限最大需量
    {{0x11730200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相第三象限最大需量
    {{0x11800200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内第四象限最大需量
    {{0x11810200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相第四象限最大需量
    {{0x11820200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相第四象限最大需量
    {{0x11830200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相第四象限最大需量
    {{0x11900200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内正向视在最大需量
    {{0x11910200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相正向视在最大需量
    {{0x11920200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相正向视在最大需量
    {{0x11930200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相正向视在最大需量
    {{0x11A00200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内反向视在最大需量
    {{0x11A10200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内A相反向视在最大需量
    {{0x11A20200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内B相反向视在最大需量
    {{0x11A30200}, 2,       0x01,   0x01, 0,      DT_ARRAY, sizeof(OOP_DEMAND_T),OOP_MAX_RATES+1},          //冻结周期内C相反向视在最大需量
};
const uint32 oadDemandListLen = sizeof(oadDemandList)/sizeof(oadDemandList[0]);

/* 变量类OAD列表 OIA1 = 0x02 */
const OAD_LIST_T oadVariateList[] = 
{
    /*OAD          class    A1      A2    lenType dataType                 len*/
    {{0x20000200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T)},          //电压  OOP_DWORD3V_T分相变量都用这个结构体？？？
    {{0x20000201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //A相电压
    {{0x20000202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //B相电压
    {{0x20000203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //C相电压
    {{0x20010200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_INT3V_T)},              //电流
    {{0x20010201}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //A相电流
    {{0x20010202}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //B相电流
    {{0x20010203}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //C相电流
    {{0x20020200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD4V_T)},           //电压相角  
    {{0x20020201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //A相电压相角
    {{0x20020202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //B相电压相角
    {{0x20020203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //C相电压相角
    {{0x20030200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD4V_T)},           //电压电流相角  
    {{0x20030201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //A相电压电流相角
    {{0x20030202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //B相电压电流相角
    {{0x20030203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //C相电压电流相角
    {{0x20040200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //有功功率
    {{0x20040201}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //A相有功功率
    {{0x20040202}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //B相有功功率
    {{0x20040203}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //C相有功功率
    {{0x20050200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //无功功率
    {{0x20050201}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //A相无功功率
    {{0x20050202}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //B相无功功率
    {{0x20050203}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //C相无功功率
    {{0x20060200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //视在功率
    {{0x20060201}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //A相视在功率
    {{0x20060202}, 4,        0x02,    0x00, 0,      DT_DOUBLE_LONG,           sizeof(int32)},                    //B相视在功率
    {{0x20060203}, 4,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //C相视在功率
    {{0x20070200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //一分钟平均有功功率
    {{0x20080200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //一分钟平均无功功率
    {{0x20090200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_INT4V_T)},            //一分钟平均视在功率
    {{0x200A0200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD4V_T)},           //功率因数
    {{0x200B0200}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //电压波形失真度
    {{0x200B0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //A相电压波形失真度
    {{0x200B0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //B相电压波形失真度
    {{0x200B0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //C相电压波形失真度
    {{0x200C0200}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //电流波形失真度
    {{0x200C0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //A相电流波形失真度
    {{0x200C0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //B相电流波形失真度
    {{0x200C0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //C相电流波形失真度
    {{0x200D0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //A相电压谐波含有量（总及2…n次）
    {{0x200D0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //B相电压谐波含有量（总及2…n次）
    {{0x200D0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //C相电压谐波含有量（总及2…n次）
    {{0x200D0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //电压谐波含有量最高谐波次数
    {{0x200E0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //A相电流谐波含有量（总及2…n次）
    {{0x200E0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //B相电流谐波含有量（总及2…n次）
    {{0x200E0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T)},         //C相电流谐波含有量（总及2…n次）
    {{0x200E0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //电流谐波含有量最高谐波次数
    {{0x200F0200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //电网频率
    {{0x20100200}, 6,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16)},                  //表内温度
    {{0x20110200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //时钟电池电压
    {{0x20120200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //停电抄表电池电压
    {{0x20130200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //时钟电池工作时间
    {{0x20140200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_METWORD_T)},          //电能表运行状态字
    {{0x20170200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //当前有功需量
    {{0x20180200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //当前无功需量
    {{0x20190200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //当前视在需量
    {{0x201A0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //当前电价
    {{0x201B0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //当前费率电价
    {{0x201C0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //当前阶梯电价
    {{0x20260200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //电压不平衡率
    {{0x20270200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //电流不平衡率
    {{0x20280200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                 //负载率
    {{0x20290200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                4*sizeof(uint32)},               //安时值????找不到对应结构体
    {{0x202D0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //（当前）透支金额
    {{0x202E0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //累计购电金额
    {{0x20310200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //月度用电量
    {{0x20320200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //阶梯结算用电量
    {{0x20400200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T)},         //控制命令执行状态字
    {{0x20410200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T)},         //控制命令错误状态字
    {{0x21000200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T)},           //分钟区间统计 - 统计周期单位为分钟 - 统计结果表
    {{0x21000300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T)},          //分钟区间统计 - 统计周期单位为分钟 - 关联对象属性表
    {{0x21010200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T)},           //小时区间统计 - 统计周期单位为小时 - 统计结果表
    {{0x21010300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T)},          //小时区间统计 - 统计周期单位为小时 - 关联对象属性表
    {{0x21020200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T)},           //日区间统计 - 统计周期单位为日 - 统计结果表
    {{0x21020300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T)},          //日区间统计 - 统计周期单位为日 - 关联对象属性表
    {{0x21030200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T)},           //月区间统计 - 统计周期单位为月 - 统计结果表
    {{0x21030300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T)},          //月区间统计 - 统计周期单位为月 - 关联对象属性表
    {{0x21040200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T)},           //年区间统计 - 统计周期单位为年 - 统计结果表
    {{0x21040300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T)},          //年区间统计 - 统计周期单位为年 - 关联对象属性表
    {{0x21100200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T)},         //分钟平均  - 统计结果表
    {{0x21100300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //分钟平均  - 关联对象属性表
    {{0x21110200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T)},         //小时平均  - 统计结果表
    {{0x21110300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //小时平均  - 关联对象属性表
    {{0x21120200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T)},         //日平均  - 统计结果表
    {{0x21120300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //日平均  - 关联对象属性表
    {{0x21130200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T)},         //月平均  - 统计结果表
    {{0x21130300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //月平均  - 关联对象属性表
    {{0x21140200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T)},         //年平均  - 统计结果表
    {{0x21140300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //年平均  - 关联对象属性表
    {{0x21200200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T)},       //分钟极值  - 统计结果表
    {{0x21200300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //分钟极值  - 关联对象属性表
    {{0x21210200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T)},       //小时极值  - 统计结果表
    {{0x21210300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //小时极值  - 关联对象属性表
    {{0x21220200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T)},       //日极值  - 统计结果表
    {{0x21220300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //日极值  - 关联对象属性表
    {{0x21230200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T)},       //月极值  - 统计结果表
    {{0x21230300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //月极值  - 关联对象属性表
    {{0x21240200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T)},       //年极值  - 统计结果表
    {{0x21240300}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T)},         //年极值  - 关联对象属性表
    {{0x21310200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T)},         //当月A相电压合格率
    {{0x21320200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T)},         //当月B相电压合格率
    {{0x21330200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T)},         //当月B相电压合格率
    {{0x21400200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T)},           //日最大有功功率及发生时间
    {{0x21410200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T)},           //月最大有功功率及发生时间
    {{0x22000200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T)},           //通信流量
    {{0x22030200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_WORD2_T)},            //供电时间
    {{0x22040200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_WORD2_T)},            //复位次数
    {{0x23010200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组1 - 总加组配置表
    {{0x23010300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 总加有功功率
    {{0x23010400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 总加无功功率
    {{0x23010500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 总加滑差时间内平均有功功率
    {{0x23010600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 总加滑差时间内平均无功功率
    {{0x23010700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组1 - 总加日有功电量
    {{0x23010800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组1 - 总加日无功电量
    {{0x23010900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组1 - 总加日有功电量
    {{0x23010A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组1 - 总加日无功电量
    {{0x23010B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 总加剩余电量(费)
    {{0x23010C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组1 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23010D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组1 - 当总加组滑差时间周期
    {{0x23010E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组1 - 总加组功控轮次配置
    {{0x23010F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组1 - 总加组电控轮次配置
    {{0x23011000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组1 - 总加组控制设置状态
    {{0x23011100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组1 - 总加组当前控制状态
    {{0x23011200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组1 - 换算及单位
    {{0x23020200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组2 - 总加组配置表
    {{0x23020300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 总加有功功率
    {{0x23020400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 总加无功功率
    {{0x23020500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 总加滑差时间内平均有功功率
    {{0x23020600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 总加滑差时间内平均无功功率
    {{0x23020700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组2 - 总加日有功电量
    {{0x23020800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组2 - 总加日无功电量
    {{0x23020900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组2 - 总加日有功电量
    {{0x23020A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组2 - 总加日无功电量
    {{0x23020B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 总加剩余电量(费)
    {{0x23020C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组2 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23020D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组2 - 当总加组滑差时间周期
    {{0x23020E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组2 - 总加组功控轮次配置
    {{0x23020F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组2 - 总加组电控轮次配置
    {{0x23021000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组2 - 总加组控制设置状态
    {{0x23021100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组2 - 总加组当前控制状态
    {{0x23021200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组2 - 换算及单位
    {{0x23030200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组3 - 总加组配置表
    {{0x23030300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 总加有功功率
    {{0x23030400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 总加无功功率
    {{0x23030500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 总加滑差时间内平均有功功率
    {{0x23030600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 总加滑差时间内平均无功功率
    {{0x23030700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组3 - 总加日有功电量
    {{0x23030800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组3 - 总加日无功电量
    {{0x23030900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组3 - 总加日有功电量
    {{0x23030A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组3 - 总加日无功电量
    {{0x23030B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 总加剩余电量(费)
    {{0x23030C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组3 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23030D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组3 - 当总加组滑差时间周期
    {{0x23030E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组3 - 总加组功控轮次配置
    {{0x23030F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组3 - 总加组电控轮次配置
    {{0x23031000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组3 - 总加组控制设置状态
    {{0x23031100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组3 - 总加组当前控制状态
    {{0x23031200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组3 - 换算及单位
    {{0x23040200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组4 - 总加组配置表
    {{0x23040300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 总加有功功率
    {{0x23040400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 总加无功功率
    {{0x23040500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 总加滑差时间内平均有功功率
    {{0x23040600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 总加滑差时间内平均无功功率
    {{0x23040700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组4 - 总加日有功电量
    {{0x23040800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组4 - 总加日无功电量
    {{0x23040900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组4 - 总加日有功电量
    {{0x23040A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组4 - 总加日无功电量
    {{0x23040B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 总加剩余电量(费)
    {{0x23040C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组4 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23040D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组4 - 当总加组滑差时间周期
    {{0x23040E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组4 - 总加组功控轮次配置
    {{0x23040F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组4 - 总加组电控轮次配置
    {{0x23041000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组4 - 总加组控制设置状态
    {{0x23041100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组4 - 总加组当前控制状态
    {{0x23041200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组4 - 换算及单位
    {{0x23050200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组5 - 总加组配置表
    {{0x23050300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 总加有功功率
    {{0x23050400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 总加无功功率
    {{0x23050500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 总加滑差时间内平均有功功率
    {{0x23050600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 总加滑差时间内平均无功功率
    {{0x23050700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组5 - 总加日有功电量
    {{0x23050800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组5 - 总加日无功电量
    {{0x23050900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组5 - 总加日有功电量
    {{0x23050A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组5 - 总加日无功电量
    {{0x23050B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 总加剩余电量(费)
    {{0x23050C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组5 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23050D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组5 - 当总加组滑差时间周期
    {{0x23050E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组5 - 总加组功控轮次配置
    {{0x23050F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组5 - 总加组电控轮次配置
    {{0x23051000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组5 - 总加组控制设置状态
    {{0x23051100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组5 - 总加组当前控制状态
    {{0x23051200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组5 - 换算及单位
    {{0x23060200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组6 - 总加组配置表
    {{0x23060300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 总加有功功率
    {{0x23060400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 总加无功功率
    {{0x23060500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 总加滑差时间内平均有功功率
    {{0x23060600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 总加滑差时间内平均无功功率
    {{0x23060700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组6 - 总加日有功电量
    {{0x23060800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组6 - 总加日无功电量
    {{0x23060900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组6 - 总加日有功电量
    {{0x23060A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组6 - 总加日无功电量
    {{0x23060B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 总加剩余电量(费)
    {{0x23060C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组6 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23060D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组6 - 当总加组滑差时间周期
    {{0x23060E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组6 - 总加组功控轮次配置
    {{0x23060F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组6 - 总加组电控轮次配置
    {{0x23061000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组6 - 总加组控制设置状态
    {{0x23061100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组6 - 总加组当前控制状态
    {{0x23061200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组6 - 换算及单位
    {{0x23070200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组7 - 总加组配置表
    {{0x23070300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 总加有功功率
    {{0x23070400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 总加无功功率
    {{0x23070500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 总加滑差时间内平均有功功率
    {{0x23070600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 总加滑差时间内平均无功功率
    {{0x23070700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组7 - 总加日有功电量
    {{0x23070800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组7 - 总加日无功电量
    {{0x23070900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组7 - 总加日有功电量
    {{0x23070A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组7 - 总加日无功电量
    {{0x23070B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 总加剩余电量(费)
    {{0x23070C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组7 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23070D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组7 - 当总加组滑差时间周期
    {{0x23070E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组7 - 总加组功控轮次配置
    {{0x23070F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组7 - 总加组电控轮次配置
    {{0x23071000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组7 - 总加组控制设置状态
    {{0x23071100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组7 - 总加组当前控制状态
    {{0x23071200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组7 - 换算及单位
    {{0x23080200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T)},           //总加组8 - 总加组配置表
    {{0x23080300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 总加有功功率
    {{0x23080400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 总加无功功率
    {{0x23080500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 总加滑差时间内平均有功功率
    {{0x23080600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 总加滑差时间内平均无功功率
    {{0x23080700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组8 - 总加日有功电量
    {{0x23080800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组8 - 总加日无功电量
    {{0x23080900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组8 - 总加日有功电量
    {{0x23080A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T)},         //总加组8 - 总加日无功电量
    {{0x23080B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 总加剩余电量(费)
    {{0x23080C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64)},                  //总加组8 - 当前功率下浮控控后总加有功功率冻结值
    {{0x23080D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8)},                  //总加组8 - 当总加组滑差时间周期
    {{0x23080E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组8 - 总加组功控轮次配置
    {{0x23080F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T)},          //总加组8 - 总加组电控轮次配置
    {{0x23081000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T)},        //总加组8 - 总加组控制设置状态
    {{0x23081100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T)},       //总加组8 - 总加组当前控制状态
    {{0x23081200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T)},           //总加组8 - 换算及单位
    {{0x24010200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量1 - 通信地址
    {{0x24010300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量1 - 互感器倍率
    {{0x24010400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量1 - 脉冲配置
    {{0x24010500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量1 - 有功功率
    {{0x24010600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量1 - 无功功率
    {{0x24010700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当日正向有功电量
    {{0x24010800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当月正向有功电量
    {{0x24010900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当日反向有功电量
    {{0x24010A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当月反向有功电量
    {{0x24010B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当日正向无功电量
    {{0x24010C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当月正向无功电量
    {{0x24010D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当日反向无功电量
    {{0x24010E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 当月反向无功电量
    {{0x24010F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 正向有功电能示值
    {{0x24011000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 反向有功电能示值
    {{0x24011100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 正向无功电能示值
    {{0x24011200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量1 - 反向无功电能示值
    {{0x24011300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量1 - 单位及换算
    {{0x24011C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量1 - 脉冲配置
    {{0x24020200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量2 - 通信地址
    {{0x24020300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量2 - 互感器倍率
    {{0x24020400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量2 - 脉冲配置
    {{0x24020500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量2 - 有功功率
    {{0x24020600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量2 - 无功功率
    {{0x24020700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当日正向有功电量
    {{0x24020800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当月正向有功电量
    {{0x24020900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当日反向有功电量
    {{0x24020A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当月反向有功电量
    {{0x24020B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当日正向无功电量
    {{0x24020C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当月正向无功电量
    {{0x24020D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当日反向无功电量
    {{0x24020E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 当月反向无功电量
    {{0x24020F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 正向有功电能示值
    {{0x24021000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 反向有功电能示值
    {{0x24021100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 正向无功电能示值
    {{0x24021200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量2 - 反向无功电能示值
    {{0x24021300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量2 - 单位及换算
    {{0x24021C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量2 - 脉冲配置
    {{0x24030200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量3 - 通信地址
    {{0x24030300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量3 - 互感器倍率
    {{0x24030400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量3 - 脉冲配置
    {{0x24030500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量3 - 有功功率
    {{0x24030600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量3 - 无功功率
    {{0x24030700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当日正向有功电量
    {{0x24030800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当月正向有功电量
    {{0x24030900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当日反向有功电量
    {{0x24030A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当月反向有功电量
    {{0x24030B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当日正向无功电量
    {{0x24030C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当月正向无功电量
    {{0x24030D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当日反向无功电量
    {{0x24030E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 当月反向无功电量
    {{0x24030F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 正向有功电能示值
    {{0x24031000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 反向有功电能示值
    {{0x24031100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 正向无功电能示值
    {{0x24031200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量3 - 反向无功电能示值
    {{0x24031300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量3 - 单位及换算
    {{0x24031C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量3 - 脉冲配置
    {{0x24040200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量4 - 通信地址
    {{0x24040300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量4 - 互感器倍率
    {{0x24040400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量4 - 脉冲配置
    {{0x24040500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量4 - 有功功率
    {{0x24040600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量4 - 无功功率
    {{0x24040700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当日正向有功电量
    {{0x24040800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当月正向有功电量
    {{0x24040900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当日反向有功电量
    {{0x24040A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当月反向有功电量
    {{0x24040B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当日正向无功电量
    {{0x24040C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当月正向无功电量
    {{0x24040D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当日反向无功电量
    {{0x24040E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 当月反向无功电量
    {{0x24040F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 正向有功电能示值
    {{0x24041000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 反向有功电能示值
    {{0x24041100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 正向无功电能示值
    {{0x24041200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量4 - 反向无功电能示值
    {{0x24041300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量4 - 单位及换算
    {{0x24041C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量4 - 脉冲配置
    {{0x24050200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量5 - 通信地址
    {{0x24050300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量5 - 互感器倍率
    {{0x24050400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量5 - 脉冲配置
    {{0x24050500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量5 - 有功功率
    {{0x24050600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量5 - 无功功率
    {{0x24050700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当日正向有功电量
    {{0x24050800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当月正向有功电量
    {{0x24050900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当日反向有功电量
    {{0x24050A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当月反向有功电量
    {{0x24050B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当日正向无功电量
    {{0x24050C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当月正向无功电量
    {{0x24050D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当日反向无功电量
    {{0x24050E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 当月反向无功电量
    {{0x24050F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 正向有功电能示值
    {{0x24051000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 反向有功电能示值
    {{0x24051100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 正向无功电能示值
    {{0x24051200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量5 - 反向无功电能示值
    {{0x24051300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量5 - 单位及换算
    {{0x24051C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量5 - 脉冲配置
    {{0x24060200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量6 - 通信地址
    {{0x24060300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量6 - 互感器倍率
    {{0x24060400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量6 - 脉冲配置
    {{0x24060500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量6 - 有功功率
    {{0x24060600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量6 - 无功功率
    {{0x24060700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当日正向有功电量
    {{0x24060800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当月正向有功电量
    {{0x24060900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当日反向有功电量
    {{0x24060A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当月反向有功电量
    {{0x24060B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当日正向无功电量
    {{0x24060C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当月正向无功电量
    {{0x24060D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当日反向无功电量
    {{0x24060E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 当月反向无功电量
    {{0x24060F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 正向有功电能示值
    {{0x24061000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 反向有功电能示值
    {{0x24061100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 正向无功电能示值
    {{0x24061200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量6 - 反向无功电能示值
    {{0x24061300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量6 - 单位及换算
    {{0x24061C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量6 - 脉冲配置
    {{0x24070200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量7 - 通信地址
    {{0x24070300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量7 - 互感器倍率
    {{0x24070400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量7 - 脉冲配置
    {{0x24070500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量7 - 有功功率
    {{0x24070600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量7 - 无功功率
    {{0x24070700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当日正向有功电量
    {{0x24070800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当月正向有功电量
    {{0x24070900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当日反向有功电量
    {{0x24070A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当月反向有功电量
    {{0x24070B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当日正向无功电量
    {{0x24070C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当月正向无功电量
    {{0x24070D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当日反向无功电量
    {{0x24070E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 当月反向无功电量
    {{0x24070F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 正向有功电能示值
    {{0x24071000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 反向有功电能示值
    {{0x24071100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 正向无功电能示值
    {{0x24071200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量7 - 反向无功电能示值
    {{0x24071300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量7 - 单位及换算
    {{0x24071C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量7 - 脉冲配置
    {{0x24080200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_VISIBLEVAR_T)},       //脉冲计量8 - 通信地址
    {{0x24080300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T)},             //脉冲计量8 - 互感器倍率
    {{0x24080400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T)},        //脉冲计量8 - 脉冲配置
    {{0x24080500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量8 - 有功功率
    {{0x24080600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32)},                  //脉冲计量8 - 无功功率
    {{0x24080700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当日正向有功电量
    {{0x24080800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当月正向有功电量
    {{0x24080900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当日反向有功电量
    {{0x24080A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当月反向有功电量
    {{0x24080B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当日正向无功电量
    {{0x24080C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当月正向无功电量
    {{0x24080D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当日反向无功电量
    {{0x24080E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 当月反向无功电量
    {{0x24080F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 正向有功电能示值
    {{0x24081000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 反向有功电能示值
    {{0x24081100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 正向无功电能示值
    {{0x24081200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T)},           //脉冲计量8 - 反向无功电能示值
    {{0x24081300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T)},        //脉冲计量8 - 单位及换算
    {{0x24081C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T)},      //脉冲计量8 - 脉冲配置
    {{0x25000200}, 12,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //累计水（热）流量
    {{0x25010200}, 12,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //累计气流量
    {{0x25020200}, 12,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //累计热量
    {{0x25030200}, 12,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //热功率
    {{0x25040200}, 12,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                 //累计工作时间
    {{0x25050200}, 12,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_WORD2_T)},            //水温
    {{0x25060200}, 12,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ST_T)},               //（仪表）状态ST
};
const uint32 oadVariateListLen = sizeof(oadVariateList)/sizeof(oadVariateList[0]);

/* 事件类OAD列表 OIA1 = 0x03 */
const OAD_LIST_T oadEvnetList[] =
{
    /*OAD        class     A1      A2   lenType dataType                 len*/
    {{0x30000200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表失压事件 - 关联对象属性表
    {{0x30000300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表失压事件 - 当前记录数
    {{0x30000400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表失压事件 - 最大记录数
    {{0x30000500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3000_T)},        //电能表失压事件 - 配置参数
    {{0x30000600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失压事件 - 事件记录单元1
    {{0x30000700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失压事件 - 事件记录单元2
    {{0x30000800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失压事件 - 事件记录单元3
    {{0x30000900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失压事件 - 事件记录单元4
    {{0x30000A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表失压事件 - 当前值记录表
    {{0x30000B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表失压事件 - 上报标识
    {{0x30000C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表失压事件 - 有效标识
    {{0x30000D00}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOSTVSTAT_T)},          //电能表失压事件 - 失压统计
    {{0x30000E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表失压事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30010200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表欠压事件 - 关联对象属性表
    {{0x30010300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表欠压事件 - 当前记录数
    {{0x30010400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表欠压事件 - 最大记录数
    {{0x30010500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T)},          //电能表欠压事件 - 配置参数
    {{0x30010600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表欠压事件 - 事件记录单元1
    {{0x30010700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表欠压事件 - 事件记录单元2
    {{0x30010800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表欠压事件 - 事件记录单元3
    {{0x30010900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表欠压事件 - 事件记录单元4
    {{0x30010A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表欠压事件 - 当前值记录表
    {{0x30010B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表欠压事件 - 上报标识
    {{0x30010C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表欠压事件 - 有效标识
    {{0x30010E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表欠压事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30020200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表过压事件 - 关联对象属性表
    {{0x30020300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表过压事件 - 当前记录数
    {{0x30020400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表过压事件 - 最大记录数
    {{0x30020500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T)},          //电能表过压事件 - 配置参数
    {{0x30020600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过压事件 - 事件记录单元1
    {{0x30020700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过压事件 - 事件记录单元2
    {{0x30020800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过压事件 - 事件记录单元3
    {{0x30020900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过压事件 - 事件记录单元4
    {{0x30020A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表过压事件 - 当前值记录表
    {{0x30020B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表过压事件 - 上报标识
    {{0x30020C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表过压事件 - 有效标识
    {{0x30020E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表过压事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30030200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表断相事件 - 关联对象属性表
    {{0x30030300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表断相事件 - 当前记录数
    {{0x30030400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表断相事件 - 最大记录数
    {{0x30030500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3003_T)},        //电能表断相事件 - 配置参数
    {{0x30030600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断相事件 - 事件记录单元1
    {{0x30030700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断相事件 - 事件记录单元2
    {{0x30030800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断相事件 - 事件记录单元3
    {{0x30030900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断相事件 - 事件记录单元4
    {{0x30030A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表断相事件 - 当前值记录表
    {{0x30030B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表断相事件 - 上报标识
    {{0x30030C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表断相事件 - 有效标识
    {{0x30030E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表断相事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30040200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表失流事件 - 关联对象属性表
    {{0x30040300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表失流事件 - 当前记录数
    {{0x30040400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表失流事件 - 最大记录数
    {{0x30040500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3004_T)},        //电能表失流事件 - 配置参数
    {{0x30040600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失流事件 - 事件记录单元1
    {{0x30040700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失流事件 - 事件记录单元2
    {{0x30040800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失流事件 - 事件记录单元3
    {{0x30040900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表失流事件 - 事件记录单元4
    {{0x30040A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表失流事件 - 当前值记录表
    {{0x30040B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表失流事件 - 上报标识
    {{0x30040C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表失流事件 - 有效标识
    {{0x30040E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表失流事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30050200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表过流事件 - 关联对象属性表
    {{0x30050300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表过流事件 - 当前记录数
    {{0x30050400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表过流事件 - 最大记录数
    {{0x30050500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T)},          //电能表过流事件 - 配置参数
    {{0x30050600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过流事件 - 事件记录单元1
    {{0x30050700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过流事件 - 事件记录单元2
    {{0x30050800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过流事件 - 事件记录单元3
    {{0x30050900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过流事件 - 事件记录单元4
    {{0x30050A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表过流事件 - 当前值记录表
    {{0x30050B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表过流事件 - 上报标识
    {{0x30050C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表过流事件 - 有效标识
    {{0x30050E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表过流事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30060200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表断流事件 - 关联对象属性表
    {{0x30060300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表断流事件 - 当前记录数
    {{0x30060400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表断流事件 - 最大记录数
    {{0x30060500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3006_T)},        //电能表断流事件 - 配置参数
    {{0x30060600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断流事件 - 事件记录单元1
    {{0x30060700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断流事件 - 事件记录单元2
    {{0x30060800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断流事件 - 事件记录单元3
    {{0x30060900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表断流事件 - 事件记录单元4
    {{0x30060A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表断流事件 - 当前值记录表
    {{0x30060B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表断流事件 - 上报标识
    {{0x30060C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表断流事件 - 有效标识
    {{0x30060E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表断流事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30070200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表功率反向事件 - 关联对象属性表
    {{0x30070300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表功率反向事件 - 当前记录数
    {{0x30070400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表功率反向事件 - 最大记录数
    {{0x30070500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T)},          //电能表功率反向事件 - 配置参数
    {{0x30070600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表功率反向事件 - 事件记录单元1
    {{0x30070700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表功率反向事件 - 事件记录单元2
    {{0x30070800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表功率反向事件 - 事件记录单元3
    {{0x30070900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表功率反向事件 - 事件记录单元4
    {{0x30070A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表功率反向事件 - 当前值记录表
    {{0x30070B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表功率反向事件 - 上报标识
    {{0x30070C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表功率反向事件 - 有效标识
    {{0x30070E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表功率反向事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30080200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表过载事件 - 关联对象属性表
    {{0x30080300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表过载事件 - 当前记录数
    {{0x30080400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表过载事件 - 最大记录数
    {{0x30080500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T)},          //电能表过载事件 - 配置参数
    {{0x30080600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过载事件 - 事件记录单元1
    {{0x30080700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过载事件 - 事件记录单元2
    {{0x30080800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过载事件 - 事件记录单元3
    {{0x30080900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T)},             //电能表过载事件 - 事件记录单元4
    {{0x30080A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表过载事件 - 当前值记录表
    {{0x30080B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表过载事件 - 上报标识
    {{0x30080C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表过载事件 - 有效标识
    {{0x30080E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表过载事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x30090200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表正向有功需量超限事件 - 事件记录表
    {{0x30090300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表正向有功需量超限事件 - 关联对象属性表
    {{0x30090400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表正向有功需量超限事件 - 当前记录数
    {{0x30090500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表正向有功需量超限事件 - 最大记录数
    {{0x30090600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T)},          //电能表正向有功需量超限事件 - 配置参数
    {{0x30090700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表正向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30090800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表正向有功需量超限事件 - 上报标识
    {{0x30090900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表正向有功需量超限事件 - 有效标识
    {{0x30090A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表正向有功需量超限事件 - 时间状态记录表

    {{0x300A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表反向有功需量超限事件 - 事件记录表
    {{0x300A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表反向有功需量超限事件 - 关联对象属性表
    {{0x300A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表反向有功需量超限事件 - 当前记录数
    {{0x300A0500}, 7,       0x03,   0x00, 1,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表反向有功需量超限事件 - 最大记录数
    {{0x300A0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T)},          //电能表反向有功需量超限事件 - 配置参数
    {{0x300A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表反向有功需量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表反向有功需量超限事件 - 上报标识
    {{0x300A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表反向有功需量超限事件 - 有效标识
    {{0x300A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表反向有功需量超限事件 - 时间状态记录表

    {{0x300B0200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表无功需量超限事件 - 关联对象属性表
    {{0x300B0300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16)},                 //电能表无功需量超限事件 - 当前记录数
    {{0x300B0400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表无功需量超限事件 - 最大记录数
    {{0x300B0500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T)},          //电能表无功需量超限事件 - 配置参数
    {{0x300B0600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表无功需量超限事件 - 事件记录单元1
    {{0x300B0700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表无功需量超限事件 - 事件记录单元2
    {{0x300B0800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表无功需量超限事件 - 事件记录单元3
    {{0x300B0900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T)},          //电能表无功需量超限事件 - 事件记录单元4
    {{0x300B0A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32)},                 //电能表无功需量超限事件 - 当前值记录表
    {{0x300B0B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表无功需量超限事件 - 上报标识
    {{0x300B0C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表无功需量超限事件 - 有效标识
    {{0x300B0E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T)},           //电能表无功需量超限事件 - 时间状态记录表,????是不是用这个结构体，待确认

    {{0x300C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表功率因数超下限事件 - 事件记录表
    {{0x300C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表功率因数超下限事件 - 关联对象属性表
    {{0x300C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表功率因数超下限事件 - 当前记录数
    {{0x300C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表功率因数超下限事件 - 最大记录数
    {{0x300C0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T)},          //电能表功率因数超下限事件 - 配置参数
    {{0x300C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表功率因数超下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表功率因数超下限事件 - 上报标识
    {{0x300C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表功率因数超下限事件 - 有效标识
    {{0x300C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表功率因数超下限事件 - 时间状态记录表

    {{0x300D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表全失压事件 - 事件记录表????
    {{0x300D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表全失压事件 - 关联对象属性表
    {{0x300D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表全失压事件 - 当前记录数
    {{0x300D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表全失压事件 - 最大记录数
    {{0x300D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表全失压事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表全失压事件 - 上报标识
    {{0x300D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表全失压事件 - 有效标识
    {{0x300D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表全失压事件 - 时间状态记录表

    {{0x300E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表辅助电源掉电事件 - 事件记录表
    {{0x300E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表辅助电源掉电事件 - 关联对象属性表
    {{0x300E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表辅助电源掉电事件 - 当前记录数
    {{0x300E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表辅助电源掉电事件 - 最大记录数
    {{0x300E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表辅助电源掉电事件 - 配置参数
    {{0x300E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表辅助电源掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表辅助电源掉电事件 - 上报标识
    {{0x300E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表辅助电源掉电事件 - 有效标识
    {{0x300E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表辅助电源掉电事件 - 时间状态记录表

    {{0x300F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电压逆相序事件 - 事件记录表
    {{0x300F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电压逆相序事件 - 关联对象属性表
    {{0x300F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电压逆相序事件 - 当前记录数
    {{0x300F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电压逆相序事件 - 最大记录数
    {{0x300F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表电压逆相序事件 - 配置参数
    {{0x300F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电压逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x300F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电压逆相序事件 - 上报标识
    {{0x300F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电压逆相序事件 - 有效标识
    {{0x300F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电压逆相序事件 - 时间状态记录表

    {{0x30100200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电流逆相序事件 - 事件记录表
    {{0x30100300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电流逆相序事件 - 关联对象属性表
    {{0x30100400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流逆相序事件 - 当前记录数
    {{0x30100500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流逆相序事件 - 最大记录数
    {{0x30100600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表电流逆相序事件 - 配置参数
    {{0x30100700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电流逆相序事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30100800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电流逆相序事件 - 上报标识
    {{0x30100900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电流逆相序事件 - 有效标识
    {{0x30100A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电流逆相序事件 - 时间状态记录表

    {{0x30110200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表掉电事件 - 事件记录表
    {{0x30110300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表掉电事件 - 关联对象属性表
    {{0x30110400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表掉电事件 - 当前记录数
    {{0x30110500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表掉电事件 - 最大记录数
    {{0x30110600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表掉电事件 - 配置参数
    {{0x30110700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表掉电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30110800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表掉电事件 - 上报标识
    {{0x30110900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表掉电事件 - 有效标识
    {{0x30110A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表掉电事件 - 时间状态记录表

    {{0x30120200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T)},         //电能表编程事件 - 事件记录表
    {{0x30120300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表编程事件 - 关联对象属性表
    {{0x30120400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表编程事件 - 当前记录数
    {{0x30120500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表编程事件 - 最大记录数
    {{0x30120700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30120800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表编程事件 - 上报标识
    {{0x30120900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表编程事件 - 有效标识
    {{0x30120A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表编程事件 - 时间状态记录表

    {{0x30130200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表清零事件 - 事件记录表
    {{0x30130300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表清零事件 - 关联对象属性表
    {{0x30130400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表清零事件 - 当前记录数
    {{0x30130500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表清零事件 - 最大记录数
    {{0x30130700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30130800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表清零事件 - 上报标识
    {{0x30130900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表清零事件 - 有效标识
    {{0x30130A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表清零事件 - 时间状态记录表

    {{0x30140200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表需量清零事件 - 事件记录表
    {{0x30140300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表需量清零事件 - 关联对象属性表
    {{0x30140400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表需量清零事件 - 当前记录数
    {{0x30140500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表需量清零事件 - 最大记录数
    {{0x30140700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表需量清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30140800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表需量清零事件 - 上报标识
    {{0x30140900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表需量清零事件 - 有效标识
    {{0x30140A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表需量清零事件 - 时间状态记录表

    {{0x30150200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENTRESETLOG_T)},      //电能表事件清零事件 - 事件记录表
    {{0x30150300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表事件清零事件 - 关联对象属性表
    {{0x30150400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表事件清零事件 - 当前记录数
    {{0x30150500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表事件清零事件 - 最大记录数
    {{0x30150700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表事件清零事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30150800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表事件清零事件 - 上报标识
    {{0x30150900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表事件清零事件 - 有效标识
    {{0x30150A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表事件清零事件 - 时间状态记录表

    {{0x30160200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表校时事件 - 事件记录表
    {{0x30160300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表校时事件 - 关联对象属性表
    {{0x30160400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表校时事件 - 当前记录数
    {{0x30160500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表校时事件 - 最大记录数
    {{0x30160700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表校时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30160800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表校时事件 - 上报标识
    {{0x30160900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表校时事件 - 有效标识
    {{0x30160A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表校时事件 - 时间状态记录表

    {{0x30170200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGPERIODLOG_T)},      //电能表时段表编程事件 - 事件记录表
    {{0x30170300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表时段表编程事件 - 关联对象属性表
    {{0x30170400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时段表编程事件 - 当前记录数
    {{0x30170500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时段表编程事件 - 最大记录数
    {{0x30170700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表时段表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30170800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表时段表编程事件 - 上报标识
    {{0x30170900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表时段表编程事件 - 有效标识
    {{0x30170A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表时段表编程事件 - 时间状态记录表

    {{0x30180200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表时区表编程事件 - 事件记录表
    {{0x30180300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表时区表编程事件 - 关联对象属性表
    {{0x30180400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时区表编程事件 - 当前记录数
    {{0x30180500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时区表编程事件 - 最大记录数
    {{0x30180700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表时区表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30180800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表时区表编程事件 - 上报标识
    {{0x30180900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表时区表编程事件 - 有效标识
    {{0x30180A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表时区表编程事件 - 时间状态记录表

    {{0x30190200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表周休日编程事件 - 事件记录表
    {{0x30190300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表周休日编程事件 - 关联对象属性表
    {{0x30190400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表周休日编程事件 - 当前记录数
    {{0x30190500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表周休日编程事件 - 最大记录数
    {{0x30190700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表周休日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30190800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表周休日编程事件 - 上报标识
    {{0x30190900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表周休日编程事件 - 有效标识
    {{0x30190A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表周休日编程事件 - 时间状态记录表

    {{0x301A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表结算日编程事件 - 事件记录表
    {{0x301A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表结算日编程事件 - 关联对象属性表
    {{0x301A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表结算日编程事件 - 当前记录数
    {{0x301A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表结算日编程事件 - 最大记录数
    {{0x301A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表结算日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表结算日编程事件 - 上报标识
    {{0x301A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表结算日编程事件 - 有效标识
    {{0x301A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表结算日编程事件 - 时间状态记录表

    {{0x301B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表开盖事件 - 事件记录表
    {{0x301B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表开盖事件 - 关联对象属性表
    {{0x301B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                  //电能表开盖事件 - 当前记录数
    {{0x301B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表开盖事件 - 最大记录数
    {{0x301B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表开盖事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表开盖事件 - 上报标识
    {{0x301B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表开盖事件 - 有效标识
    {{0x301B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表开盖事件 - 时间状态记录表

    {{0x301C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表开端钮盒事件 - 事件记录表
    {{0x301C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表开端钮盒事件 - 关联对象属性表
    {{0x301C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表开端钮盒事件 - 当前记录数
    {{0x301C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表开端钮盒事件 - 最大记录数
    {{0x301C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表开端钮盒事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表开端钮盒事件 - 上报标识
    {{0x301C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表开端钮盒事件 - 有效标识
    {{0x301C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表开端钮盒事件 - 时间状态记录表

    {{0x301D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电压不平衡事件 - 事件记录表
    {{0x301D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电压不平衡事件 - 关联对象属性表
    {{0x301D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电压不平衡事件 - 当前记录数
    {{0x301D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电压不平衡事件 - 最大记录数
    {{0x301D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T)},          //电能表电压不平衡事件 - 配置参数
    {{0x301D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电压不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电压不平衡事件 - 上报标识
    {{0x301D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电压不平衡事件 - 有效标识
    {{0x301D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电压不平衡事件 - 时间状态记录表

    {{0x301E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电流不平衡事件 - 事件记录表
    {{0x301E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电流不平衡事件 - 关联对象属性表
    {{0x301E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流不平衡事件 - 当前记录数
    {{0x301E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流不平衡事件 - 最大记录数
    {{0x301E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T)},          //电能表电流不平衡事件 - 配置参数
    {{0x301E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电流不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电流不平衡事件 - 上报标识
    {{0x301E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电流不平衡事件 - 有效标识
    {{0x301E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电流不平衡事件 - 时间状态记录表

    {{0x301F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表跳闸事件 - 事件记录表
    {{0x301F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表跳闸事件 - 关联对象属性表
    {{0x301F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表跳闸事件 - 当前记录数
    {{0x301F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表跳闸事件 - 最大记录数
    {{0x301F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表跳闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x301F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表跳闸事件 - 上报标识
    {{0x301F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表跳闸事件 - 有效标识
    {{0x301F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表跳闸事件 - 时间状态记录表

    {{0x30200200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表合闸事件 - 事件记录表
    {{0x30200300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表合闸事件 - 关联对象属性表
    {{0x30200400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表合闸事件 - 当前记录数
    {{0x30200500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表合闸事件 - 最大记录数
    {{0x30200700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表合闸事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30200800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表合闸事件 - 上报标识
    {{0x30200900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表合闸事件 - 有效标识
    {{0x30200A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表合闸事件 - 时间状态记录表

    {{0x30210200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGHOLIDAYLOG_T)},     //电能表节假日编程事件 - 事件记录表
    {{0x30210300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表节假日编程事件 - 关联对象属性表
    {{0x30210400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表节假日编程事件 - 当前记录数
    {{0x30210500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表节假日编程事件 - 最大记录数
    {{0x30210700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表节假日编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30210800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表节假日编程事件 - 上报标识
    {{0x30210900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表节假日编程事件 - 有效标识
    {{0x30210A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表节假日编程事件 - 时间状态记录表

    {{0x30220200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表有功组合方式编程事件 - 事件记录表
    {{0x30220300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表有功组合方式编程事件 - 关联对象属性表
    {{0x30220400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表有功组合方式编程事件 - 当前记录数
    {{0x30220500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表有功组合方式编程事件 - 最大记录数
    {{0x30220700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表有功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30220800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表有功组合方式编程事件 - 上报标识
    {{0x30220900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表有功组合方式编程事件 - 有效标识
    {{0x30220A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表有功组合方式编程事件 - 时间状态记录表

    {{0x30230200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表无功组合方式编程事件 - 事件记录表
    {{0x30230300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表无功组合方式编程事件 - 关联对象属性表
    {{0x30230400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表无功组合方式编程事件 - 当前记录数
    {{0x30230500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表无功组合方式编程事件 - 最大记录数
    {{0x30230700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表无功组合方式编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30230800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表无功组合方式编程事件 - 上报标识
    {{0x30230900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表无功组合方式编程事件 - 有效标识
    {{0x30230A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表无功组合方式编程事件 - 时间状态记录表

    {{0x30240200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表费率参数表编程事件 - 事件记录表
    {{0x30240300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表费率参数表编程事件 - 关联对象属性表
    {{0x30240400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表费率参数表编程事件 - 当前记录数
    {{0x30240500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表费率参数表编程事件 - 最大记录数
    {{0x30240700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表费率参数表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30240800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表费率参数表编程事件 - 上报标识
    {{0x30240900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表费率参数表编程事件 - 有效标识
    {{0x30240A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表费率参数表编程事件 - 时间状态记录表

    {{0x30250200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表阶梯表编程事件 - 事件记录表
    {{0x30250300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表阶梯表编程事件 - 关联对象属性表
    {{0x30250400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表阶梯表编程事件 - 当前记录数
    {{0x30250500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表阶梯表编程事件 - 最大记录数
    {{0x30250700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表阶梯表编程事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30250800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表阶梯表编程事件 - 上报标识
    {{0x30250900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表阶梯表编程事件 - 有效标识
    {{0x30250A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表阶梯表编程事件 - 时间状态记录表

    {{0x30260200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表密钥更新事件 - 事件记录表
    {{0x30260300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表密钥更新事件 - 关联对象属性表
    {{0x30260400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表密钥更新事件 - 当前记录数
    {{0x30260500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表密钥更新事件 - 最大记录数
    {{0x30260700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表密钥更新事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30260800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表密钥更新事件 - 上报标识
    {{0x30260900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表密钥更新事件 - 有效标识
    {{0x30260A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表密钥更新事件 - 时间状态记录表

    {{0x30270200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCARD_T)},       //电能表异常插卡事件 - 事件记录表
    {{0x30270300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表异常插卡事件 - 关联对象属性表
    {{0x30270400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表异常插卡事件 - 当前记录数
    {{0x30270500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表异常插卡事件 - 最大记录数
    {{0x30270700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表异常插卡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30270800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表异常插卡事件 - 上报标识
    {{0x30270900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表异常插卡事件 - 有效标识
    {{0x30270A00}, 7,       0x03,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32)},                   //电能表异常插卡事件 - 非法插卡总次数

    {{0x30280200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表购电记录 - 事件记录表
    {{0x30280300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表购电记录 - 关联对象属性表
    {{0x30280400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表购电记录 - 当前记录数
    {{0x30280500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表购电记录 - 最大记录数
    {{0x30280700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表购电记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30280800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表购电记录 - 上报标识
    {{0x30280900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表购电记录 - 有效标识
    {{0x30280A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表购电记录 - 时间状态记录表

    {{0x30290200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_REBATELOG_T)},          //电能表退费记录 - 事件记录表
    {{0x30290300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表退费记录 - 关联对象属性表
    {{0x30290400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表退费记录 - 当前记录数
    {{0x30290500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表退费记录 - 最大记录数
    {{0x30290700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表退费记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30290800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表退费记录 - 上报标识
    {{0x30290900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表退费记录 - 有效标识
    {{0x30290A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表退费记录 - 时间状态记录表

    {{0x302A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表恒定磁场干扰事件 - 事件记录表
    {{0x302A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表恒定磁场干扰事件 - 关联对象属性表
    {{0x302A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表恒定磁场干扰事件 - 当前记录数
    {{0x302A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表恒定磁场干扰事件 - 最大记录数
    {{0x302A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表恒定磁场干扰事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表恒定磁场干扰事件 - 上报标识
    {{0x302A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表恒定磁场干扰事件 - 有效标识
    {{0x302A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表恒定磁场干扰事件 - 时间状态记录表

    {{0x302B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表负荷开关误动作事件 - 事件记录表
    {{0x302B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表负荷开关误动作事件 - 关联对象属性表
    {{0x302B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表负荷开关误动作事件 - 当前记录数
    {{0x302B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表负荷开关误动作事件 - 最大记录数
    {{0x302B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表负荷开关误动作事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表负荷开关误动作事件 - 上报标识
    {{0x302B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表负荷开关误动作事件 - 有效标识
    {{0x302B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表负荷开关误动作事件 - 时间状态记录表

    {{0x302C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电源异常事件 - 事件记录表
    {{0x302C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电源异常事件 - 关联对象属性表
    {{0x302C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电源异常事件 - 当前记录数
    {{0x302C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电源异常事件 - 最大记录数
    {{0x302C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电源异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电源异常事件 - 上报标识
    {{0x302C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电源异常事件 - 有效标识
    {{0x302C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电源异常事件 - 时间状态记录表

    {{0x302D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表电流严重不平衡事件 - 事件记录表
    {{0x302D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表电流严重不平衡事件 - 关联对象属性表
    {{0x302D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流严重不平衡事件 - 当前记录数
    {{0x302D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表电流严重不平衡事件 - 最大记录数
    {{0x302D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T)},          //电能表电流严重不平衡事件 - 配置参数
    {{0x302D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表电流严重不平衡事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表电流严重不平衡事件 - 上报标识
    {{0x302D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表电流严重不平衡事件 - 有效标识
    {{0x302D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表电流严重不平衡事件 - 时间状态记录表

    {{0x302E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表时钟故障事件 - 事件记录表
    {{0x302E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表时钟故障事件 - 关联对象属性表
    {{0x302E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时钟故障事件 - 当前记录数
    {{0x302E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时钟故障事件 - 最大记录数
    {{0x302E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表时钟故障事件 - 配置参数
    {{0x302E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表时钟故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表时钟故障事件 - 上报标识
    {{0x302E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表时钟故障事件 - 有效标识
    {{0x302E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表时钟故障事件 - 时间状态记录表

    {{0x302F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表计量芯片故障事件 - 事件记录表
    {{0x302F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表计量芯片故障事件 - 关联对象属性表
    {{0x302F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表计量芯片故障事件 - 当前记录数
    {{0x302F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表计量芯片故障事件 - 最大记录数
    {{0x302F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //电能表计量芯片故障事件 - 配置参数
    {{0x302F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表计量芯片故障事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x302F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表计量芯片故障事件 - 上报标识
    {{0x302F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表计量芯片故障事件 - 有效标识
    {{0x302F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表计量芯片故障事件 - 时间状态记录表

    {{0x30300200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MODULELOG_T)},          //通信模块变更事件 - 事件记录表
    {{0x30300300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //通信模块变更事件 - 关联对象属性表
    {{0x30300400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //通信模块变更事件 - 当前记录数
    {{0x30300500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //通信模块变更事件 - 最大记录数
    {{0x30300600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T)},       //通信模块变更事件 - 配置参数
    {{0x30300700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //通信模块变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x30300800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8)},                    //通信模块变更事件 - 上报标识
    {{0x30300900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8)},                    //通信模块变更事件 - 有效标识
    {{0x30300A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //通信模块变更事件 - 时间状态记录表

    {{0x31000200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端初始化事件 - 事件记录表
    {{0x31000300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端初始化事件 - 关联对象属性表
    {{0x31000400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端初始化事件 - 当前记录数
    {{0x31000500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端初始化事件 - 最大记录数
    {{0x31000700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端初始化事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31000800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端初始化事件 - 上报标识
    {{0x31000900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端初始化事件 - 有效标识
    {{0x31000A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端初始化事件 - 时间状态记录表

    {{0x31010200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端版本变更事件 - 事件记录表
    {{0x31010300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端版本变更事件 - 关联对象属性表
    {{0x31010400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端版本变更事件 - 当前记录数
    {{0x31010500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端版本变更事件 - 最大记录数
    {{0x31010700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端版本变更事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31010800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端版本变更事件 - 上报标识
    {{0x31010900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端版本变更事件 - 有效标识
    {{0x31010A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端版本变更事件 - 时间状态记录表

    {{0x31040200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端状态量变位事件 - 事件记录表
    {{0x31040300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端状态量变位事件 - 关联对象属性表
    {{0x31040400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端状态量变位事件 - 当前记录数
    {{0x31040500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端状态量变位事件 - 最大记录数
    {{0x31040700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},           //终端状态量变位事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31040800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                             //终端状态量变位事件 - 上报标识
    {{0x31040900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                             //终端状态量变位事件 - 有效标识
    {{0x31040A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},           //终端状态量变位事件 - 时间状态记录表

    {{0x31050200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_CLOCKFLYLOG_T)},        //电能表时钟超差事件 - 事件记录表
    {{0x31050300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表时钟超差事件 - 关联对象属性表
    {{0x31050400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时钟超差事件 - 当前记录数
    {{0x31050500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表时钟超差事件 - 最大记录数
    {{0x31050600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3105_T)},        //电能表时钟超差事件 - 配置参数
    {{0x31050700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表时钟超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31050800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表时钟超差事件 - 上报标识
    {{0x31050900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表时钟超差事件 - 有效标识
    {{0x31050A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表时钟超差事件 - 时间状态记录表

    {{0x31060200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_POWERLOG_T)},           //终端停/上电事件 - 事件记录表
    {{0x31060300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端停/上电事件 - 关联对象属性表
    {{0x31060400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端停/上电事件 - 当前记录数
    {{0x31060500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端停/上电事件 - 最大记录数
    {{0x31060600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_POWERONCUT_T)},         //终端停/上电事件 - 配置参数
    {{0x31060700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端停/上电事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31060800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端停/上电事件 - 上报标识
    {{0x31060900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端停/上电事件 - 有效标识
    {{0x31060A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端停/上电事件 - 时间状态记录表

    {{0x31070200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端直流模拟量越上限事件 - 事件记录表
    {{0x31070300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端直流模拟量越上限事件 - 关联对象属性表
    {{0x31070400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端直流模拟量越上限事件 - 当前记录数
    {{0x31070500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端直流模拟量越上限事件 - 最大记录数
    {{0x31070600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T)},         //终端直流模拟量越上限事件 - 配置参数
    {{0x31070700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端直流模拟量越上限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31070800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端直流模拟量越上限事件 - 上报标识
    {{0x31070900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端直流模拟量越上限事件 - 有效标识
    {{0x31070A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端直流模拟量越上限事件 - 时间状态记录表

    {{0x31080200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端直流模拟量越下限事件 - 事件记录表
    {{0x31080300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端直流模拟量越下限事件 - 关联对象属性表
    {{0x31080400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端直流模拟量越下限事件 - 当前记录数
    {{0x31080500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端直流模拟量越下限事件 - 最大记录数
    {{0x31080600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T)},         //终端直流模拟量越下限事件 - 配置参数
    {{0x31080700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端直流模拟量越下限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31080800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端直流模拟量越下限事件 - 上报标识
    {{0x31080900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端直流模拟量越下限事件 - 有效标识
    {{0x31080A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端直流模拟量越下限事件 - 时间状态记录表

    {{0x31090200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端消息认证错误事件 - 事件记录表
    {{0x31090300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端消息认证错误事件 - 关联对象属性表
    {{0x31090400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端消息认证错误事件 - 当前记录数
    {{0x31090500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端消息认证错误事件 - 最大记录数
    {{0x31090700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端消息认证错误事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31090800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端消息认证错误事件 - 上报标识
    {{0x31090900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端消息认证错误事件 - 有效标识
    {{0x31090A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端消息认证错误事件 - 时间状态记录表

    {{0x310A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //设备故障记录 - 事件记录表
    {{0x310A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //设备故障记录 - 关联对象属性表
    {{0x310A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //设备故障记录 - 当前记录数
    {{0x310A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //设备故障记录 - 最大记录数
    {{0x310A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //设备故障记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //设备故障记录 - 上报标识
    {{0x310A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //设备故障记录 - 有效标识
    {{0x310A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //设备故障记录 - 时间状态记录表

    {{0x310B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表示度下降事件 - 事件记录表
    {{0x310B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表示度下降事件 - 关联对象属性表
    {{0x310B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表示度下降事件 - 当前记录数
    {{0x310B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表示度下降事件 - 最大记录数
    {{0x310B0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T)},        //电能表示度下降事件 - 配置参数
    {{0x310B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表示度下降事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表示度下降事件 - 上报标识
    {{0x310B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表示度下降事件 - 有效标识
    {{0x310B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表示度下降事件 - 时间状态记录表

    {{0x310C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能量超差事件 - 事件记录表
    {{0x310C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能量超差事件 - 关联对象属性表
    {{0x310C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能量超差事件 - 当前记录数
    {{0x310C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能量超差事件 - 最大记录数
    {{0x310C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T)},        //电能量超差事件 - 配置参数
    {{0x310C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能量超差事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能量超差事件 - 上报标识
    {{0x310C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能量超差事件 - 有效标识
    {{0x310C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能量超差事件 - 时间状态记录表

    {{0x310D0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表飞走事件 - 事件记录表
    {{0x310D0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表飞走事件 - 关联对象属性表
    {{0x310D0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表飞走事件 - 当前记录数
    {{0x310D0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表飞走事件 - 最大记录数
    {{0x310D0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T)},        //电能表飞走事件 - 配置参数
    {{0x310D0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表飞走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310D0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表飞走事件 - 上报标识
    {{0x310D0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表飞走事件 - 有效标识
    {{0x310D0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表飞走事件 - 时间状态记录表

    {{0x310E0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //电能表停走事件 - 事件记录表
    {{0x310E0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表停走事件 - 关联对象属性表
    {{0x310E0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表停走事件 - 当前记录数
    {{0x310E0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表停走事件 - 最大记录数
    {{0x310E0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310E_T)},        //电能表停走事件 - 配置参数
    {{0x310E0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表停走事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310E0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表停走事件 - 上报标识
    {{0x310E0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表停走事件 - 有效标识
    {{0x310E0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表停走事件 - 时间状态记录表

    {{0x310F0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端抄表失败事件 - 事件记录表
    {{0x310F0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端抄表失败事件 - 关联对象属性表
    {{0x310F0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端抄表失败事件 - 当前记录数
    {{0x310F0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端抄表失败事件 - 最大记录数
    {{0x310F0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310F_T)},        //终端抄表失败事件 - 配置参数
    {{0x310F0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端抄表失败事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x310F0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端抄表失败事件 - 上报标识
    {{0x310F0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端抄表失败事件 - 有效标识
    {{0x310F0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端抄表失败事件 - 时间状态记录表

    {{0x31100200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //月通信流量超限事件 - 事件记录表
    {{0x31100300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //月通信流量超限事件 - 关联对象属性表
    {{0x31100400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //月通信流量超限事件 - 当前记录数
    {{0x31100500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //月通信流量超限事件 - 最大记录数
    {{0x31100600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3110_T)},        //月通信流量超限事件 - 配置参数
    {{0x31100700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //月通信流量超限事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31100800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //月通信流量超限事件 - 上报标识
    {{0x31100900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //月通信流量超限事件 - 有效标识
    {{0x31100A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //月通信流量超限事件 - 时间状态记录表

    {{0x31110200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_SMLOG_T)},              //发现未知电能表事件 - 事件记录表
    {{0x31110300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //发现未知电能表事件 - 关联对象属性表
    {{0x31110400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //发现未知电能表事件 - 当前记录数
    {{0x31110500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //发现未知电能表事件 - 最大记录数
    {{0x31110700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //发现未知电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31110800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //发现未知电能表事件 - 上报标识
    {{0x31110900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //发现未知电能表事件 - 有效标识
    {{0x31110A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //发现未知电能表事件 - 时间状态记录表

    {{0x31120200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_UNMETERLOG_T)},         //跨台区电能表事件 - 事件记录表
    {{0x31120300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //跨台区电能表事件 - 关联对象属性表
    {{0x31120400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //跨台区电能表事件 - 当前记录数
    {{0x31120500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //跨台区电能表事件 - 最大记录数
    {{0x31120700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //跨台区电能表事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31120800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //跨台区电能表事件 - 上报标识
    {{0x31120900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //跨台区电能表事件 - 有效标识
    {{0x31120A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //跨台区电能表事件 - 时间状态记录表

    {{0x31140200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端对时事件 - 事件记录表
    {{0x31140300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端对时事件 - 关联对象属性表
    {{0x31140400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端对时事件 - 当前记录数
    {{0x31140500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端对时事件 - 最大记录数
    {{0x31140700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端对时事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31140800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端对时事件 - 上报标识
    {{0x31140900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端对时事件 - 有效标识
    {{0x31140A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端对时事件 - 时间状态记录表

    {{0x31150200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_RCTRLLOG_T)},           //遥控跳闸记录 - 事件记录表
    {{0x31150300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //遥控跳闸记录 - 关联对象属性表
    {{0x31150400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //遥控跳闸记录 - 当前记录数
    {{0x31150500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //遥控跳闸记录 - 最大记录数
    {{0x31150700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //遥控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31150800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //遥控跳闸记录 - 上报标识
    {{0x31150900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //遥控跳闸记录 - 有效标识
    {{0x31150A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //遥控跳闸记录 - 时间状态记录表

    {{0x31160200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_DCLOG_T)},              //有功总电能量差动越限事件记录 - 事件记录表
    {{0x31160300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //有功总电能量差动越限事件记录 - 关联对象属性表
    {{0x31160400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //有功总电能量差动越限事件记录 - 当前记录数
    {{0x31160500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //有功总电能量差动越限事件记录 - 最大记录数
    {{0x31160600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3116_T)},        //有功总电能量差动越限事件记录 - 配置参数
    {{0x31160700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //有功总电能量差动越限事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31160800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //有功总电能量差动越限事件记录 - 上报标识
    {{0x31160900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //有功总电能量差动越限事件记录 - 有效标识
    {{0x31160A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //有功总电能量差动越限事件记录 - 时间状态记录表

    {{0x31170200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //输出回路接入状态变位事件记录 - 事件记录表
    {{0x31170300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //输出回路接入状态变位事件记录 - 关联对象属性表
    {{0x31170400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //输出回路接入状态变位事件记录 - 当前记录数
    {{0x31170500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //输出回路接入状态变位事件记录 - 最大记录数
    {{0x31170700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //输出回路接入状态变位事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31170800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //输出回路接入状态变位事件记录 - 上报标识
    {{0x31170900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //输出回路接入状态变位事件记录 - 有效标识
    {{0x31170A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //输出回路接入状态变位事件记录 - 时间状态记录表

    {{0x31180200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T)},         //终端编程记录 - 事件记录表
    {{0x31180300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端编程记录 - 关联对象属性表
    {{0x31180400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端编程记录 - 当前记录数
    {{0x31180500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端编程记录 - 最大记录数
    {{0x31180700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端编程记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31180800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端编程记录 - 上报标识
    {{0x31180900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端编程记录 - 有效标识
    {{0x31180A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端编程记录 - 时间状态记录表

    {{0x31190200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //终端电流回路异常事件 - 事件记录表
    {{0x31190300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端电流回路异常事件 - 关联对象属性表
    {{0x31190400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端电流回路异常事件 - 当前记录数
    {{0x31190500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端电流回路异常事件 - 最大记录数
    {{0x31190700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端电流回路异常事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x31190800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端电流回路异常事件 - 上报标识
    {{0x31190900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端电流回路异常事件 - 有效标识
    {{0x31190A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端电流回路异常事件 - 时间状态记录表

    {{0x311A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METEROFFLOG_T)},        //电能表在网状态切换事件 - 事件记录表
    {{0x311A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表在网状态切换事件 - 关联对象属性表
    {{0x311A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表在网状态切换事件 - 当前记录数
    {{0x311A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表在网状态切换事件 - 最大记录数
    {{0x311A0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_311A_T)},        //电能表在网状态切换事件 - 配置参数
    {{0x311A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表在网状态切换事件 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表在网状态切换事件 - 上报标识
    {{0x311A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表在网状态切换事件 - 有效标识
    {{0x311A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表在网状态切换事件 - 时间状态记录表

    {{0x311B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_P2PCLOCKLOG_T)},        //终端对电能表校时记录 - 事件记录表
    {{0x311B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //终端对电能表校时记录 - 关联对象属性表
    {{0x311B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端对电能表校时记录 - 当前记录数
    {{0x311B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //终端对电能表校时记录 - 最大记录数
    {{0x311B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //终端对电能表校时记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //终端对电能表校时记录 - 上报标识
    {{0x311B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //终端对电能表校时记录 - 有效标识
    {{0x311B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //终端对电能表校时记录 - 时间状态记录表

    {{0x311C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METERDIRTYLOG_T)},      //电能表数据变更监控记录 - 事件记录表
    {{0x311C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电能表数据变更监控记录 - 关联对象属性表
    {{0x311C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表数据变更监控记录 - 当前记录数
    {{0x311C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电能表数据变更监控记录 - 最大记录数
    {{0x311C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T)},        //电能表数据变更监控记录 - 配置参数
    {{0x311C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电能表数据变更监控记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x311C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8)},                    //电能表数据变更监控记录 - 上报标识
    {{0x311C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8)},                    //电能表数据变更监控记录 - 有效标识
    {{0x311C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电能表数据变更监控记录 - 时间状态记录表

    {{0x32000200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_PCTRLLOG_T)},           //功控跳闸记录 - 事件记录表
    {{0x32000300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //功控跳闸记录 - 关联对象属性表
    {{0x32000400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //功控跳闸记录 - 当前记录数
    {{0x32000500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //功控跳闸记录 - 最大记录数
    {{0x32000700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //功控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32000800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8)},                    //功控跳闸记录 - 上报标识
    {{0x32000900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8)},                    //功控跳闸记录 - 有效标识
    {{0x32000A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //功控跳闸记录 - 时间状态记录表

    {{0x32010200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_ECTRLLOG_T)},           //电控跳闸记录 - 事件记录表
    {{0x32010300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电控跳闸记录 - 关联对象属性表
    {{0x32010400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电控跳闸记录 - 当前记录数
    {{0x32010500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电控跳闸记录 - 最大记录数
    {{0x32010700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电控跳闸记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32010800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8)},                    //电控跳闸记录 - 上报标识
    {{0x32010900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8)},                    //电控跳闸记录 - 有效标识
    {{0x32010A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电控跳闸记录 - 时间状态记录表

    {{0x32020200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T)},              //购电参数设置记录 - 事件记录表
    {{0x32020300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //购电参数设置记录 - 关联对象属性表
    {{0x32020400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //购电参数设置记录 - 当前记录数
    {{0x32020500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //购电参数设置记录 - 最大记录数
    {{0x32020700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //购电参数设置记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32020800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8)},                    //购电参数设置记录 - 上报标识
    {{0x32020900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8)},                    //购电参数设置记录 - 有效标识
    {{0x32020A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //购电参数设置记录 - 时间状态记录表

    {{0x32030200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EWARNLOG_T)},           //电控告警事件记录 - 事件记录表
    {{0x32030300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T)},               //电控告警事件记录 - 关联对象属性表
    {{0x32030400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电控告警事件记录 - 当前记录数
    {{0x32030500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16)},                   //电控告警事件记录 - 最大记录数
    {{0x32030700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T)},            //电控告警事件记录 - 当前值记录表????是不是用这个结构体，待确认
    {{0x32030800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8)},                    //电控告警事件记录 - 上报标识
    {{0x32030900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8)},                    //电控告警事件记录 - 有效标识
    {{0x32030A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T)},            //电控告警事件记录 - 时间状态记录表
};
const uint32 oadEvnetListLen = sizeof(oadEvnetList)/sizeof(oadEvnetList[0]);

/* 参变量类OAD列表 OIA1 = 0x04 */
const OAD_LIST_T oadParaList[] =
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0x40000200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //短时间格式 日期-时间
    {{0x40000300}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8)},                    //校时模式
    {{0x40000400}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCCLOCK_T)},          //精准校时参数
    {{0x40010200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T)},         //通信地址
    {{0x40020200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T)},         //表号
    {{0x40030200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T)},         //客户编号
    {{0x40040200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_GEOGADD_T)},            //设备地理位置
    {{0x40050200}, 8,      0x04,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_GRPADDR_T)},            //组地址
    {{0x40060200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CLOCK_T)},              //时钟源参数
    {{0x40070200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LCD_T)},                //LCD参数
    {{0x40080200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //备用套时区表切换时间
    {{0x40090200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //备用套时区表切换时间
    {{0x400A0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //备用套时区表切换时间
    {{0x400B0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //备用套时区表切换时间
    {{0x400C0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PERIOD_T)},             //时区时段数
    {{0x400D0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //阶梯数
    {{0x400E0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //谐波分析次数
    {{0x400F0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //密钥总条数
    {{0x40100200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //计量元件数
    {{0x40110200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PUBHOLIDAY_T)},         //公共假日表
    {{0x40120200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLESTR8_T)},        //周休日特征字
    {{0x40130200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //周休日釆用的日时段表号
    {{0x40140200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T)},           //当前套时区表
    {{0x40150200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T)},           //备用套时区表
    {{0x40160200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T)},          //当前套日时段表
    {{0x40170200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T)},          //备用套日时段表
    {{0x40180200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T)},         //当前套费率电价
    {{0x40190200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T)},         //备用套费率电价
    {{0x401A0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T)},          //当前套阶梯电价
    {{0x401B0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T)},          //备用套阶梯电价
    {{0x401C0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //电流互感器变比
    {{0x401D0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //电压互感器变比
    {{0x401E0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LIMITCHARGE_T)},        //报警金额限值
    {{0x401F0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERLIMIT_T)},         //其它金额限值
    {{0x40200200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_POWERLIMIT_T)},         //报警电量限值
    {{0x40210200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERPOWERLIMIT_T)},    //其它电量限值
    {{0x40220200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLESTR16_T)},       //插卡状态字
    {{0x40240200}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8)},                    //剔除
    {{0x40250200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DC_UPDATE_T)},          //升级结果列表
    {{0x40250300}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_DCU_CONFIG_T)},         //采集器升级控制参数
    {{0x40300200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_VOLTPARAM_T)},          //电压合格率参数
    {{0x41000200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //最大需量周期
    {{0x41010200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //滑差时间
    {{0x41020200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //校表脉冲宽度
    {{0x41030200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //资产管理编码
    {{0x41040200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T)},        //额定电压
    {{0x41050200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T)},        //额定电流/基本电流
    {{0x41060200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T)},        //最大电流
    {{0x41070200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T)},        //有功准确度等级
    {{0x41080200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T)},        //无功准确度等级
    {{0x41090200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //电能表有功常数
    {{0x410A0200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //电能表无功常数
    {{0x410B0200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //电能表型号
    {{0x410C0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T)},        //ABC各相电导系数
    {{0x410D0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T)},        //ABC各相电抗系数
    {{0x410E0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T)},        //ABC各相电阻系数
    {{0x410F0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T)},        //ABC各相电纳系数
    {{0x41110200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR8S_T)},       //软件备案号
    {{0x41120200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLEVAR8S_T)},       //有功组合方式特征字
    {{0x41130200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLEVAR8S_T)},       //无功组合方式1特征字
    {{0x41140200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLEVAR8S_T)},       //无功组合方式2特征字
    {{0x41160200}, 8,      0x04,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_PAYDAYS_T)},            //结算日日期
    {{0x41170200}, 8,      0x04,   0x01, 0,      DT_TI,                    sizeof(OOP_TI_T)},                 //期间需量冻结周期
    {{0x42020200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_CASCADE_T)},            //级联通信参数
    {{0x42040200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCTIME_T)},           //终端广播校时参数
    {{0x42040300}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_P2PSYNCTIME_T)},        //终端单地址广播校时参数
    {{0x43000200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //电气设备-设备描述符
    {{0x43000300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //电气设备-版本信息
    {{0x43000400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //电气设备-生产日期
    {{0x43000500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T)},                //电气设备-子设备列表
    {{0x43000600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //电气设备-支持规约列表
    {{0x43000700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //电气设备-允许跟随上报
    {{0x43000800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //电气设备-允许主动上报
    {{0x43000900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //电气设备-允许与主站通话
    {{0x43000A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T)},            //电气设备-上报通道
    {{0x43070200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //水表-设备描述符
    {{0x43070300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //水表-版本信息
    {{0x43070400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //水表-生产日期
    {{0x43070500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T)},                //水表-子设备列表
    {{0x43070600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //水表-支持规约列表
    {{0x43070700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //水表-允许跟随上报
    {{0x43070800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //水表-允许主动上报
    {{0x43070900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //水表-允许与主站通话
    {{0x43070A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T)},            //水表-上报通道
    {{0x43080200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //气表-设备描述符
    {{0x43080300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //气表-版本信息
    {{0x43080400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //气表-生产日期
    {{0x43080500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T)},                //气表-子设备列表
    {{0x43080600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //气表-支持规约列表
    {{0x43080700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //气表-允许跟随上报
    {{0x43080800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //气表-允许主动上报
    {{0x43080900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //气表-允许与主站通话
    {{0x43080A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T)},            //气表-上报通道
    {{0x43090200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T)},       //热表-设备描述符
    {{0x43090300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //热表-版本信息
    {{0x43090400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T)},         //热表-生产日期
    {{0x43090500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T)},                //热表-子设备列表
    {{0x43090600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //热表-支持规约列表
    {{0x43090700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //热表-允许跟随上报
    {{0x43090800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //热表-允许主动上报
    {{0x43090900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8)},                    //热表-允许与主站通话
    {{0x43090A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T)},            //热表-上报通道
    //{{0x44000200}, 20,      0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_P2PSYNCTIME_T)},      //对象列表???没有找到对应结构体
    {{0x44000300}, 20,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_ASSOCIATION_CONTEXT_T)},//应用语境信息
    {{0x44000400}, 20,     0x04,   0x04, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //当前连接的客户机地址
    {{0x44000500}, 20,     0x04,   0x04, 0,      DT_ENUM,                  sizeof(uint8)},                    //连接认证机制
    {{0x44010200}, 25,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_VISIBLEVAR32_T)},       //认证密码
    {{0x45000200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T)},         //公网通信模块参数1 - GPRS通信配置
    {{0x45000300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //公网通信模块参数1 - 主站通信参数表
    {{0x45000400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T)},                //公网通信模块参数1 - 短信通信参数
    {{0x45000500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //公网通信模块参数1 - 版本信息
    {{0x45000600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //公网通信模块参数1 - 支持规约列表
    {{0x45000700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR20_T)},       //公网通信模块参数1 - SIM卡的ICCID
    {{0x45000800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T)},       //公网通信模块参数1 - IMSI
    {{0x45000900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16)},                    //公网通信模块参数1 - 信号强度
    {{0x45000A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T)},       //公网通信模块参数1 - SIM卡号码
    {{0x45000B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T)},         //公网通信模块参数1 - 拨号IP
    {{0x45000C00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VISIBLEVAR_T)},         //设备描述符    
    {{0x45000D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T)},            //运营商及网络制式
    {{0x45000E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //多网络配置         
    {{0x45010200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T)},         //公网通信模块参数2 - GPRS通信配置
    {{0x45010300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //公网通信模块参数2 - 主站通信参数表
    {{0x45010400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T)},                //公网通信模块参数2 - 短信通信参数
    {{0x45010500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T)},            //公网通信模块参数2 - 版本信息
    {{0x45010600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T)},           //公网通信模块参数2 - 支持规约列表
    {{0x45010700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR20_T)},       //公网通信模块参数2 - SIM卡的ICCID
    {{0x45010800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T)},       //公网通信模块参数2 - IMSI
    {{0x45010900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16)},                    //公网通信模块参数2 - 信号强度
    {{0x45010A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T)},       //公网通信模块参数2 - SIM卡号码
    {{0x45010B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T)},         //公网通信模块参数2 - 拨号IP
    {{0x45010C00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VISIBLEVAR_T)},         //设备描述符
    {{0x45010D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T)},            //运营商及网络制式
    {{0x45010E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //多网络配置    
    
    {{0x45100200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数1 - 以太网通信配置
    {{0x45100300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数1 - 主站通信参数表
    {{0x45100400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数1 - 以太网本地IP配置
    {{0x45100500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数1 - MAC地址
    {{0x45110200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数2 - 以太网通信配置
    {{0x45110300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数2 - 主站通信参数表
    {{0x45110400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数2 - 以太网本地IP配置
    {{0x45110500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数2 - MAC地址
    {{0x45120200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数3 - 以太网通信配置
    {{0x45120300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数3 - 主站通信参数表
    {{0x45120400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数3 - 以太网本地IP配置
    {{0x45120500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数3 - MAC地址
    {{0x45130200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数4 - 以太网通信配置
    {{0x45130300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数4 - 主站通信参数表
    {{0x45130400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数4 - 以太网本地IP配置
    {{0x45130500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数4 - MAC地址
    {{0x45140200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数5 - 以太网通信配置
    {{0x45140300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数5 - 主站通信参数表
    {{0x45140400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数5 - 以太网本地IP配置
    {{0x45140500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数5 - MAC地址
    {{0x45150200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数6 - 以太网通信配置
    {{0x45150300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数6 - 主站通信参数表
    {{0x45150400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数6 - 以太网本地IP配置
    {{0x45150500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数6 - MAC地址
    {{0x45160200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数7 - 以太网通信配置
    {{0x45160300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数7 - 主站通信参数表
    {{0x45160400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数7 - 以太网本地IP配置
    {{0x45160500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数7 - MAC地址
    {{0x45170200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},          //以太网通信模块参数8 - 以太网通信配置
    {{0x45170300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T)},       //以太网通信模块参数8 - 主站通信参数表
    {{0x45170400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},              //以太网通信模块参数8 - 以太网本地IP配置
    {{0x45170500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数8 - MAC地址
    {{0x45200200}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //公网远程通信多接入点备用通道

    //{{0x60030200}, 8,      0x06,    0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T)},    //以太网通信模块参数7 - 以太网通信配置
    //{{0x60030300}, 8,      0x06,    0x00, 0,      DT_ARRAY,                     sizeof(OOP_MASTERPARAMS_T)},         //以太网通信模块参数7 - 主站通信参数表
    //{{0x60030400}, 8,      0x06,    0x00, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},                    //以太网通信模块参数7 - 以太网本地IP配置
    //{{0x60030500}, 8,      0x06,    0x00, 0,      DT_OCTET_STRING,        sizeof(OOP_ETHMAC_T)},              //以太网通信模块参数7 - MAC地址
    //{{0x60030200}, 8,      0x06,    0x00, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T)},           //以太网通信模块参数8 - 以太网通信配置
    //{{0x60030300}, 8,      0x06,    0x00, 0,      DT_ARRAY,                     sizeof(OOP_MASTERPARAMS_T)},        //以太网通信模块参数8 - 主站通信参数表
    //{{0x60030400}, 8,      0x06,    0x00, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T)},                   //以太网通信模块参数8 - 以太网本地IP配置
    //{{0x60030500}, 8,      0x06,    0x00, 0,      DT_OCTET_STRING,        sizeof(OOP_ETHMAC_T)},             //以太网通信模块参数8 - MAC地址
    //{{0x60030200}, 8,      0x06,    0x00, 0,      DT_ARRAY,                     sizeof(OOP_APNGROUP_T)},              //公网远程通信多接入点备用通道
    //
};
const uint32 oadParaListLen = sizeof(oadParaList)/sizeof(oadParaList[0]);

/* 冻结类OAD列表 OIA1 = 0x05 */
const OAD_LIST_T oadFreezeList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    //{{0x50000200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //瞬时冻结 - 冻结数据表
    {{0x50000300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //瞬时冻结 - 关联对象属性表
    //{{0x50010200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //秒冻结 - 冻结数据表
    {{0x50010300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //秒冻结 - 关联对象属性表
    //{{0x50020200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //分钟冻结 - 冻结数据表
    {{0x50020300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //分钟冻结 - 关联对象属性表
    //{{0x50030200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //小时冻结 - 冻结数据表
    {{0x50030300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //小时冻结 - 关联对象属性表
    //{{0x50040200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //日冻结 - 冻结数据表
    {{0x50040300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //日冻结 - 关联对象属性表
    //{{0x50050200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //结算日冻结 - 冻结数据表
    {{0x50050300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //结算日冻结 - 关联对象属性表
    //{{0x50060200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //月冻结 - 冻结数据表
    {{0x50060300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //月冻结 - 关联对象属性表
    //{{0x50070200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //年冻结 - 冻结数据表
    {{0x50070300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //年冻结 - 关联对象属性表
    //{{0x50080200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //时区表切换冻结 - 冻结数据表
    {{0x50080300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //时区表切换冻结 - 关联对象属性表
    //{{0x50090200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //日时段表切换冻结 - 冻结数据表
    {{0x50090300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //日时段表切换冻结 - 关联对象属性表
    //{{0x500A0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //费率电价切换冻结 - 冻结数据表
    {{0x500A0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //费率电价切换冻结 - 关联对象属性表
    //{{0x500B0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //阶梯切换冻结 - 冻结数据表
    {{0x500B0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //阶梯切换冻结 - 关联对象属性表
    //{{0x50110200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T)},           //阶梯结算冻结 - 冻结数据表
    {{0x50110300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T)},           //阶梯结算冻结 - 关联对象属性表
};
const uint32 oadFreezeListLen = sizeof(oadFreezeList)/sizeof(oadFreezeList[0]);

/* 采集类OAD列表 OIA1 = 0x06 */
const OAD_LIST_T oadCollectList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0x60000200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_METER_T)},              //采集档案配置表
    {{0x60000300}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //采集档案配置表 - 当前元素个数
    {{0x60000400}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //采集档案配置表 - 最大元素个数
    {{0x60020200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_SEARCHMET_RES_T)},      //所有搜表结果
    {{0x60020500}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACROSSAREA_RES_T)},     //一个跨台区结果
    {{0x60020600}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //所有搜表结果记录数
    {{0x60020600}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //跨台区搜表结果记录数
    {{0x60020800}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHPARAM_T)},        //搜表参数
    {{0x60020900}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TIMINGSEARCH_T)},       //定时搜表参数集合
    
    
    
    {{0x60120200}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TASK_T)},               //任务配置表
    {{0x60140200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_NORMAL_T)},        //普通采集方案集
    {{0x60160200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_EVENT_T)},         //事件采集方案集
    {{0x60180200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_TRANS_T)},         //透明方案集
    {{0x601A0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TRANSRES_T)},           //透明方案结果集
    {{0x601C0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_REPORT_T)},        //上报方案集
    {{0x601E0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T)},        //采集规则库
    {{0x60320200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_RLUESTATE_T)},      //采集状态集
    {{0x60340200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T)},        //采集任务监控集
};
const uint32 oadCollectListLen = sizeof(oadCollectList)/sizeof(oadCollectList[0]);

/* 集合类OAD列表 OIA1 = 0x07 */
const OAD_LIST_T oadGatherList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0x70000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //文件集合 - 当前元素个数
    {{0x70000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //文件集合 - 最大元素个数
    {{0x70100300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //脚本集合 - 当前元素个数
    {{0x70100400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //脚本集合 - 最大元素个数
    {{0x70120300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //脚本执行结果集 - 当前元素个数
    {{0x70120400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //脚本执行结果集 - 最大元素个数
    {{0x71000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //扩展变量对象集合 - 当前元素个数
    {{0x71000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //扩展变量对象集合 - 最大元素个数
    {{0x71010300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //扩展参变量对象集合 - 当前元素个数
    {{0x71010400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //扩展参变量对象集合 - 最大元素个数
};
const uint32 oadGatherListLen = sizeof(oadGatherList)/sizeof(oadGatherList[0]);

/* 控制接口类OAD列表 OIA1 = 0x08 */
const OAD_LIST_T oadControlList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0x80000200}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TELECON_PARAM_T)},      //远程控制 - 配置参数
    {{0x80000400}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T)},            //远程控制 - 告警状态
    {{0x80000500}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T)},            //远程控制 - 命令状态
    {{0x80010200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8)},                    //保电状态
    {{0x80010300}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //允许与主站最大无通信时长（分钟），0表示不自动保电
    {{0x80010400}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16)},                   //上电自动保电时长（分钟），0表示上电不自动保电。
    {{0x80010500}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_PRTTIME_T)},            //自动保电时段
    {{0x80020200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8)},                    //催费告警状态，只读
    {{0x80020300}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CALLFEE_T)},            //催费告警参数，只读
    {{0x80030200}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(CHINESEINFO)},              //一般中文信息
    {{0x80040200}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(CHINESEINFO)},              //重要中文信息
    {{0x81000200}, 8,      0x08,   0x00, 0,      DT_LONG64,                sizeof(int64)},                    //终端保安定值 
    {{0x81010200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_TIME_T)},        //终端功控时段 
    {{0x81020200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_WARNING_T)},     //功控告警时间
    {{0x81030200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_8103_CONFIG_T)},        //时段功控   
    {{0x81040200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_8104_CONFIG_T)},        //厂休控   
    {{0x81050200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_8105_CONFIG_T)},        //营业报停控  
    {{0x81070200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_8107_CONFIG_T)},        //购电控   
    {{0x81080200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_8108_CONFIG_T)},        //月电控 
};
const uint32 oadControlListLen = sizeof(oadControlList)/sizeof(oadControlList[0]);

/* 文件传输接口类OAD列表 OIA1 = 0x0F OIA2 = 0x00 */
const OAD_LIST_T oadFileList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0xF0000200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T)},              //文件分帧传输管理- 文件信息
    {{0xF0000300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E)},       //文件分帧传输管理 - 命令结果
    {{0xF0000400}, 18,     0x0F,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},        //文件分帧传输管理 - 文件内容
    {{0xF0000500}, 18,     0x0F,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                  //文件分帧传输管理 - 当前指针
    {{0xF0010200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T)},              //文件分块传输管理 - 文件信息
    {{0xF0010300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E)},       //文件分块传输管理 - 命令结果
    {{0xF0010400}, 18,     0x0F,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T)},           //文件分块传输管理 - 传输块状态字
    {{0xF0020200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T)},              //文件扩展传输管理 - 文件信息
    {{0xF0020300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E)},       //文件扩展传输管理 - 命令结果
    {{0xF0020400}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FTPSERVER_T)},         //文件扩展传输管理 - 服务器信息
};
const uint32 oadFileListLen = sizeof(oadFileList)/sizeof(oadFileList[0]);


/* ESAM接口类OAD列表 OIA1 = 0x0F OIA2 = 0x01 */
const OAD_LIST_T oadEsamList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0xF1000200}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //ESAM序列号
    {{0xF1000200}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //ESAM版本号
    {{0xF1000400}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //对称密钥版本
    {{0xF1000500}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //会话时效门限
    {{0xF1000600}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //会话时效剩余时间
    {{0xF1000700}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             3*sizeof(uint32)},                 //当前计数器(没找到对应的结构体)
    {{0xF1000800}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_LICENSEVER_T)},         //证书版本
    {{0xF1000900}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //终端证书序列号
    {{0xF1000A00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //终端证书
    {{0xF1000B00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //主站证书序列号
    {{0xF1000C00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T)},         //主站证书
    {{0xF1000D00}, 21,     0x0F,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_OADS_T)},               //ESAM 安全存储对象列表
    {{0xF1000E00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //红外认证时效门限
    {{0xF1000F00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32)},                   //红外认证剩余时间
    {{0xF1010200}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SECURITYFLAG_E)},       //安全模式开关 
    {{0xF1010300}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SCURITYMODE_T)},        //全模式参数 
    {{0xF1010400}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(uint8)},                    //SAL安全应用数据链路层参数 
};
const uint32 oadEsamListLen = sizeof(oadEsamList)/sizeof(oadEsamList[0]);

/* 输入输出类OAD列表 OIA1 = 0x0F OIA2 = 0x02 */
const OAD_LIST_T oadIoList[] = 
{
    /*OAD          class   A1      A2    lenType dataType                  len*/
    {{0xF2000200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SERIALPORT_T)},         //RS232列表参数
    {{0xF2000300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //RS232设备对象数量
    {{0xF2010200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RS485_T)},              //RS485列表参数
    {{0xF2010300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //RS485设备对象数量
    {{0xF2020200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_IR_T)},                 //红外列表参数
    {{0xF2020300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //红外设备对象数量
    {{0xF2030200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_STATE_T)},     //信号量列表参数
    {{0xF2030300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //信号量设备对象数量
    {{0xF2030400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T)},      //信号量接入属性
    {{0xF2031800}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T)},      //信号量接入属性
    {{0xF2040200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_T)},             //直流模拟量
    {{0xF2040300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //直流模拟量设备对象数量
    {{0xF2040400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_CONFIG_T)},      //直流模拟量配置
    {{0xF2050200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHOUT_T)},          //继电器
    {{0xF2050300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //继电器设备对象数量
    {{0xF2060200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGOUT_T)},         //告警输出
    {{0xF2060300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //告警设备对象数量
    {{0xF2060400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGTIME_T)},        //允许告警时段
    {{0xF2070200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_MUTIFUNCTERMINAL_T)},   //多功能端子
    {{0xF2070300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //多功能端子设备对象数量
    {{0xF2080200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_AC_T)},                 //交采接口
    {{0xF2080300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //交采设备对象数量   
    {{0xF2090200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ROUTEMODULE_T)},        //载波/微功率无线接口，本地通信模块单元参数
    {{0xF2090300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //载波/微功率无线接口设备对象数量
    {{0xF2090500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_CHILDNODE_T)},          //载波/微功率无线接口，从节点对象列表
    {{0xF2090600}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_TI_T)},                 //载波/微功率无线接口，从节点对象列表更新周期
    {{0xF20A0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_PULSE_T)},              //脉冲输入端口描述
    {{0xF20A0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //脉冲输入设备对象数量
    //{{0xF20B0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 0xffffffff},                       //蓝牙模块,目前没有对应结构体
    {{0xF20B0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //蓝牙模块设备对象数量
    {{0xF20C0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_T)},              //230无线专网模块
    {{0xF20C0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //230无线专网模块设备对象数量
    {{0xF20C0400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_CHS_T)},          //频道设置
    {{0xF20C0500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_SIGS_T)},         //有效信号强度
    {{0xF20E0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_USB_T)},                //USB设备
    {{0xF20E0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8)},                    //USB设备对象数量
    //{{0xF2100300}, 8,      0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(uint8)},                  //从节点单元，目前没有对应结构体
};
const uint32 oadIoListLen = sizeof(oadIoList)/sizeof(oadIoList[0]);

/* 临时在这里建表，为了读记录数据,只做表名和表结构 最后在具体的class里建表*/
/* 防止后面编译错误 */
/* 电量量类OAD列表 OIA1 = 0x00 */
OAD_RECORD_LIST oadEnergyRecordList[] = 
{
    {{0xF20E0300}, 22,   sizeof(uint8)},                    //USB设备对象数量
};
uint32 oadEnergyRecordListLen;
