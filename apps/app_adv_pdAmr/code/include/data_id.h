/*
*******************************************************************************
 Copyright (c) 2006-2007,南京新联电子仪器有限公司项目部
             All rights reserved.
 程序名称：data_id.h
 功    能: 数据字典
*******************************************************************************
*/

#ifndef _DATA_ID_H
#define _DATA_ID_H

/***********************计量点数据项数组***************/

/******************--1计量点当前时刻采集数据项ID--*********************/
enum data_rt
{
    RT_ER_READ_TAG=0,        // 1字节, 字节数当前电量终端抄表标记
    RT_ER_READ_TIME,         // 6字节, BCD码, 字节终端抄表时间：秒分时日月年
    RT_POS_AE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)正向有功总电能
    RT_POS_AE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向有功电能
    RT_POS_AE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向有功电能

    RT_POS_AE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向有功电能
    RT_POS_AE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向有功电能
    RT_POS_AE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向有功电能
    RT_POS_AE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向有功电能
    RT_POS_AE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向有功电能

    RT_POS_AE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向有功电能
    RT_POS_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）正向有功电能
    RT_NEG_AE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)反向有功总电能
    RT_NEG_AE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1反向有功电能
    RT_NEG_AE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2反向有功电能

    RT_NEG_AE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3反向有功电能
    RT_NEG_AE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4反向有功电能
    RT_NEG_AE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5反向有功电能
    RT_NEG_AE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6反向有功电能
    RT_NEG_AE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7反向有功电能

    RT_NEG_AE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8反向有功电能
    RT_NEG_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）反向有功电能
    RT_POS_RE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)正向无功总电能(+RL,+RC)
    RT_POS_RE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向无功电能
    RT_POS_RE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向无功电能

    RT_POS_RE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向无功电能
    RT_POS_RE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向无功电能
    RT_POS_RE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向无功电能
    RT_POS_RE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向无功电能
    RT_POS_RE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向无功电能

    RT_POS_RE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向无功电能
    RT_POS_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）正向无功电能
    RT_NEG_RE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)反向无功总电能(-RL,-RC)
    RT_NEG_RE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1反向无功电能
    RT_NEG_RE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2反向无功电能

    RT_NEG_RE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3反向无功电能
    RT_NEG_RE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4反向无功电能
    RT_NEG_RE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5反向无功电能
    RT_NEG_RE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6反向无功电能
    RT_NEG_RE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7反向无功电能

    RT_NEG_RE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8反向无功电能
    RT_NEG_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）反向无功电能
    RT_FIRST_RE_Z_ER_ID,     // 8字节, BIN x 10000, (当前)一象限无功总电能(+RL)
    RT_FIRST_RE_1_ER_ID,     // 8字节, BIN x 10000, (当前)费率1一象限无功电能
    RT_FIRST_RE_2_ER_ID,     // 8字节, BIN x 10000, (当前)费率2一象限无功电能

    RT_FIRST_RE_3_ER_ID,     // 8字节, BIN x 10000, (当前)费率3一象限无功电能
    RT_FIRST_RE_4_ER_ID,     // 8字节, BIN x 10000, (当前)费率4一象限无功电能
    RT_FIRST_RE_5_ER_ID,     // 8字节, BIN x 10000, (当前)费率5一象限无功电能
    RT_FIRST_RE_6_ER_ID,     // 8字节, BIN x 10000, (当前)费率6一象限无功电能
    RT_FIRST_RE_7_ER_ID,     // 8字节, BIN x 10000, (当前)费率7一象限无功电能

    RT_FIRST_RE_8_ER_ID,     // 8字节, BIN x 10000, (当前)费率8一象限无功电能
    RT_FIRST_RE_EXT_ER_ID,   // 8字节x6, BIN x 10000, (当前)费率（9—14）一象限无功电能
    RT_FOURTH_RE_Z_ER_ID,    // 8字节, BIN x 10000, (当前)四象限无功总电能(-Rc)
    RT_FOURTH_RE_1_ER_ID,    // 8字节, BIN x 10000, (当前)费率1四象限无功电能
    RT_FOURTH_RE_2_ER_ID,    // 8字节, BIN x 10000, (当前)费率2四象限无功电能

    RT_FOURTH_RE_3_ER_ID,    // 8字节, BIN x 10000, (当前)费率3四象限无功电能
    RT_FOURTH_RE_4_ER_ID,    // 8字节, BIN x 10000, (当前)费率4四象限无功电能
    RT_FOURTH_RE_5_ER_ID,    // 8字节, BIN x 10000, (当前)费率5四象限无功电能
    RT_FOURTH_RE_6_ER_ID,    // 8字节, BIN x 10000, (当前)费率6四象限无功电能
    RT_FOURTH_RE_7_ER_ID,    // 8字节, BIN x 10000, (当前)费率7四象限无功电能

    RT_FOURTH_RE_8_ER_ID,    // 8字节, BIN x 10000, (当前)费率8四象限无功电能
    RT_FOURTH_RE_EXT_ER_ID,  // 8字节x6, BIN x 10000, (当前)费率（9—14）四象限无功电能
    RT_SECOND_RE_Z_ER_ID,    // 8字节, BIN x 10000, (当前)二象限无功总电能(+Rc)
    RT_SECOND_RE_1_ER_ID,    // 8字节, BIN x 10000, (当前)费率1二象限无功电能
    RT_SECOND_RE_2_ER_ID,    // 8字节, BIN x 10000, (当前)费率2二象限无功电能

    RT_SECOND_RE_3_ER_ID,    // 8字节, BIN x 10000, (当前)费率3二象限无功电能
    RT_SECOND_RE_4_ER_ID,    // 8字节, BIN x 10000, (当前)费率4二象限无功电能
    RT_SECOND_RE_5_ER_ID,    // 8字节, BIN x 10000, (当前)费率5二象限无功电能
    RT_SECOND_RE_6_ER_ID,    // 8字节, BIN x 10000, (当前)费率6二象限无功电能
    RT_SECOND_RE_7_ER_ID,    // 8字节, BIN x 10000, (当前)费率7二象限无功电能

    RT_SECOND_RE_8_ER_ID,    // 8字节, BIN x 10000, (当前)费率8二象限无功电能
    RT_SECOND_RE_EXT_ER_ID,  // 8字节x6, BIN x 10000, (当前)费率（9—14）二象限无功电能
    RT_THIRD_RE_Z_ER_ID,     // 8字节, BIN x 10000, (当前)三象限无功总电能(-RL)
    RT_THIRD_RE_1_ER_ID,     // 8字节, BIN x 10000, (当前)费率1三象限无功电能
    RT_THIRD_RE_2_ER_ID,     // 8字节, BIN x 10000, (当前)费率2三象限无功电能

    RT_THIRD_RE_3_ER_ID,     // 8字节, BIN x 10000, (当前)费率3三象限无功电能
    RT_THIRD_RE_4_ER_ID,     // 8字节, BIN x 10000, (当前)费率4三象限无功电能
    RT_THIRD_RE_5_ER_ID,     // 8字节, BIN x 10000, (当前)费率5三象限无功电能
    RT_THIRD_RE_6_ER_ID,     // 8字节, BIN x 10000, (当前)费率6三象限无功电能
    RT_THIRD_RE_7_ER_ID,     // 8字节, BIN x 10000, (当前)费率7三象限无功电能

    RT_THIRD_RE_8_ER_ID,     // 8字节, BIN x 10000, (当前)费率8三象限无功电能
    RT_THIRD_RE_EXT_ER_ID,   // 8字节x6, BIN x 10000, (当前)费率（9—14）三象限无功电能
    RT_L_RE_Z_ER_ID,         // 8字节, BIN x 10000, (当前)感性无功总电能(+RL,-RL)
    RT_L_RE_1_ER_ID,         // 8字节, BIN x 10000, (当前)费率1感性无功电能
    RT_L_RE_2_ER_ID,         // 8字节, BIN x 10000, (当前)费率2感性无功电能

    RT_L_RE_3_ER_ID,         // 8字节, BIN x 10000, (当前)费率3感性无功电能
    RT_L_RE_4_ER_ID,         // 8字节, BIN x 10000, (当前)费率4感性无功电能
    RT_L_RE_5_ER_ID,         // 8字节, BIN x 10000, (当前)费率5感性无功电能
    RT_L_RE_6_ER_ID,         // 8字节, BIN x 10000, (当前)费率6感性无功电能
    RT_L_RE_7_ER_ID,         // 8字节, BIN x 10000, (当前)费率7感性无功电能

    RT_L_RE_8_ER_ID,         // 8字节, BIN x 10000, (当前)费率8感性无功电能
    RT_L_RE_EXT_ER_ID,       // 8字节x6, BIN x 10000, (当前)费率（9—14）感性无功电能
    RT_C_RE_Z_ER_ID,         // 8字节, BIN x 10000, (当前)容性无功总电能(+Rc,-Rc)
    RT_C_RE_1_ER_ID,         // 8字节, BIN x 10000, (当前)费率1容性无功电能
    RT_C_RE_2_ER_ID,         // 8字节, BIN x 10000, (当前)费率2容性无功电能

    RT_C_RE_3_ER_ID,         // 8字节, BIN x 10000, (当前)费率3容性无功电能
    RT_C_RE_4_ER_ID,         // 8字节, BIN x 10000, (当前)费率4容性无功电能
    RT_C_RE_5_ER_ID,         // 8字节, BIN x 10000, (当前)费率5容性无功电能
    RT_C_RE_6_ER_ID,         // 8字节, BIN x 10000, (当前)费率6容性无功电能
    RT_C_RE_7_ER_ID,         // 8字节, BIN x 10000, (当前)费率7容性无功电能

    RT_C_RE_8_ER_ID,         // 8字节, BIN x 10000, (当前)费率8容性无功电能
    RT_C_RE_EXT_ER_ID,       // 8字节x6, BIN x 10000, (当前)费率（9—14）容性无功电能
    RT_POS_AE_A_ER_ID,       // 8字节, BIN x 10000, (当前)A相正向有功电能
    RT_POS_AE_B_ER_ID,       // 8字节, BIN x 10000, (当前)B相正向有功电能
    RT_POS_AE_C_ER_ID,       // 8字节, BIN x 10000, (当前)C相正向有功电能

    RT_NEG_AE_A_ER_ID,       // 8字节, BIN x 10000, (当前)A相反向有功电能
    RT_NEG_AE_B_ER_ID,       // 8字节, BIN x 10000, (当前)B相反向有功电能
    RT_NEG_AE_C_ER_ID,       // 8字节, BIN x 10000, (当前)C相反向有功电能
    RT_POS_RE_A_ER_ID,       // 8字节, BIN x 10000, (当前)A相感性无功电能
    RT_POS_RE_B_ER_ID,       // 8字节, BIN x 10000, (当前)B相感性无功电能

    RT_POS_RE_C_ER_ID,       // 8字节, BIN x 10000, (当前)C相感性无功电能
    RT_NEG_RE_A_ER_ID,       // 8字节, BIN x 10000, (当前)A相容性无功电能
    RT_NEG_RE_B_ER_ID,       // 8字节, BIN x 10000, (当前)B相容性无功电能
    RT_NEG_RE_C_ER_ID,       // 8字节, BIN x 10000, (当前)C相容性无功电能
    RT_DM_READ_TAG,          // 1字节, 终端抄表标记

    RT_DM_READ_TIME,         // 6字节, BCD码, 终端抄表时间：秒分时日月年
    RT_POS_Z_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)正向有功总最大需量
    RT_POS_1_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率1正向有功最大需量
    RT_POS_2_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率2正向有功最大需量
    RT_POS_3_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率3正向有功最大需量

    RT_POS_4_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率4正向有功最大需量
    RT_POS_5_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率5正向有功最大需量
    RT_POS_6_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率6正向有功最大需量
    RT_POS_7_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率7正向有功最大需量
    RT_POS_8_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率8正向有功最大需量

    RT_POS_EXT_AD_MAX_EV_ID, // 4字节x6, BIN x 10000, (当前)费率（9—14）正向有功最大需量
    RT_NEG_Z_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)反向有功总最大需量
    RT_NEG_1_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率1反向有功最大需量
    RT_NEG_2_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率2反向有功最大需量
    RT_NEG_3_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率3反向有功最大需量

    RT_NEG_4_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率4反向有功最大需量
    RT_NEG_5_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率5反向有功最大需量
    RT_NEG_6_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率6反向有功最大需量
    RT_NEG_7_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率7反向有功最大需量
    RT_NEG_8_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率8反向有功最大需量

    RT_NEG_EXT_AD_MAX_EV_ID, // 4字节x6, BIN x 10000, (当前)费率（9—14）反向有功最大需量
    RT_POS_Z_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)正向无功总最大需量
    RT_POS_1_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率1正向无功最大需量
    RT_POS_2_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率2正向无功最大需量
    RT_POS_3_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率3正向无功最大需量

    RT_POS_4_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率4正向无功最大需量
    RT_POS_5_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率5正向无功最大需量
    RT_POS_6_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率6正向无功最大需量
    RT_POS_7_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率7正向无功最大需量
    RT_POS_8_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率8正向无功最大需量

    RT_POS_EXT_RD_MAX_EV_ID, // 4字节x6, BIN x 10000, (当前)费率（9—14）正向无功最大需量
    RT_NEG_Z_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)反向无功总最大需量
    RT_NEG_1_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率1反向无功最大需量
    RT_NEG_2_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率2反向无功最大需量
    RT_NEG_3_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率3反向无功最大需量

    RT_NEG_4_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率4反向无功最大需量
    RT_NEG_5_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率5反向无功最大需量
    RT_NEG_6_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率6反向无功最大需量
    RT_NEG_7_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率7反向无功最大需量
    RT_NEG_8_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (当前)费率8反向无功最大需量

    RT_NEG_EXT_RD_MAX_EV_ID, // 4字节x6, BIN x 10000, (当前)费率（9—14）反向无功最大需量
    RT_FIRST_Z_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1无功总最大需量
    RT_FIRST_1_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率1无功最大需量
    RT_FIRST_2_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率2无功最大需量
    RT_FIRST_3_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率3无功最大需量

    RT_FIRST_4_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率4无功最大需量
    RT_FIRST_5_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率5无功最大需量
    RT_FIRST_6_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率6无功最大需量
    RT_FIRST_7_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率7无功最大需量
    RT_FIRST_8_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限1费率8无功最大需量

    RT_FIRST_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (当前)象限1费率(9-14)无功最大需量
    RT_SECOND_Z_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2无功总最大需量
    RT_SECOND_1_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率1无功最大需量
    RT_SECOND_2_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率2无功最大需量
    RT_SECOND_3_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率3无功最大需量

    RT_SECOND_4_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率4无功最大需量
    RT_SECOND_5_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率5无功最大需量
    RT_SECOND_6_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率6无功最大需量
    RT_SECOND_7_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率7无功最大需量
    RT_SECOND_8_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限2费率8无功最大需量

    RT_SECOND_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (当前)象限2费率(9-14)无功最大需量
    RT_THIRD_Z_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3无功总最大需量
    RT_THIRD_1_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率1无功最大需量
    RT_THIRD_2_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率2无功最大需量
    RT_THIRD_3_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率3无功最大需量

    RT_THIRD_4_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率4无功最大需量
    RT_THIRD_5_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率5无功最大需量
    RT_THIRD_6_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率6无功最大需量
    RT_THIRD_7_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率7无功最大需量
    RT_THIRD_8_RD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)象限3费率8无功最大需量

    RT_THIRD_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (当前)象限3费率(9-14)无功最大需量
    RT_FOURTH_Z_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4无功总最大需量
    RT_FOURTH_1_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率1无功最大需量
    RT_FOURTH_2_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率2无功最大需量
    RT_FOURTH_3_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率3无功最大需量

    RT_FOURTH_4_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率4无功最大需量
    RT_FOURTH_5_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率5无功最大需量
    RT_FOURTH_6_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率6无功最大需量
    RT_FOURTH_7_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率7无功最大需量
    RT_FOURTH_8_RD_MAX_EV_ID,// 4字节, BIN x 10000, (当前)象限4费率8无功最大需量

    RT_FOURTH_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (当前)象限4费率(9-14)无功最大需量
    RT_POS_Z_AD_MAX_TM_ID,   // 6字节,(当前)正向有功总最大需量发生时间, 6字节, BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_POS_1_AD_MAX_TM_ID,   // 6字节,(当前)费率1正向有功最大需量发生时间,格式同上
    RT_POS_2_AD_MAX_TM_ID,   // 6字节,(当前)费率2正向有功最大需量发生时间,格式同上
    RT_POS_3_AD_MAX_TM_ID,   // 6字节,(当前)费率3正向有功最大需量发生时间,格式同上

    RT_POS_4_AD_MAX_TM_ID,   // 6字节,(当前)费率4正向有功最大需量发生时间,格式同上
    RT_POS_5_AD_MAX_TM_ID,   // 6字节,(当前)费率5正向有功最大需量发生时间,格式同上
    RT_POS_6_AD_MAX_TM_ID,   // 6字节,(当前)费率6正向有功最大需量发生时间,格式同上
    RT_POS_7_AD_MAX_TM_ID,   // 6字节,(当前)费率7正向有功最大需量发生时间,格式同上
    RT_POS_8_AD_MAX_TM_ID,   // 6字节,(当前)费率8正向有功最大需量发生时间,格式同上

    RT_POS_EXT_AD_MAX_TM_ID, // 6*6字节,(当前)费率（9—14）正向有功最大需量发生时间,格式同上
    RT_NEG_Z_AD_MAX_TM_ID,   // 6字节,(当前)反向有功总最大需量发生时间,格式同上
    RT_NEG_1_AD_MAX_TM_ID,   // 6字节,(当前)费率1反向有功最大需量发生时间,格式同上
    RT_NEG_2_AD_MAX_TM_ID,   // 6字节,(当前)费率2反向有功最大需量发生时间,格式同上
    RT_NEG_3_AD_MAX_TM_ID,   // 6字节,(当前)费率3反向有功最大需量发生时间,格式同上

    RT_NEG_4_AD_MAX_TM_ID,   // 6字节,(当前)费率4反向有功最大需量发生时间,格式同上
    RT_NEG_5_AD_MAX_TM_ID,   // 6字节,(当前)费率5反向有功最大需量发生时间,格式同上
    RT_NEG_6_AD_MAX_TM_ID,   // 6字节,(当前)费率6反向有功最大需量发生时间,格式同上
    RT_NEG_7_AD_MAX_TM_ID,   // 6字节,(当前)费率7反向有功最大需量发生时间,格式同上
    RT_NEG_8_AD_MAX_TM_ID,   // 6字节,(当前)费率8反向有功最大需量发生时间,格式同上

    RT_NEG_EXT_AD_MAX_TM_ID, // 6*6字节,(当前)费率（9—14）反向有功最大需量发生时间,BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_POS_Z_RD_MAX_TM_ID,   // 6字节,(当前)正向无功总最大需量发生时间,格式同上
    RT_POS_1_RD_MAX_TM_ID,   // 6字节,(当前)费率1正向无功最大需量发生时间,格式同上
    RT_POS_2_RD_MAX_TM_ID,   // 6字节,(当前)费率2正向无功最大需量发生时间,格式同上
    RT_POS_3_RD_MAX_TM_ID,   // 6字节,(当前)费率3正向无功最大需量发生时间,格式同上

    RT_POS_4_RD_MAX_TM_ID,   // 6字节,(当前)费率4正向无功最大需量发生时间,格式同上
    RT_POS_5_RD_MAX_TM_ID,   // 6字节,(当前)费率5正向无功最大需量发生时间,格式同上
    RT_POS_6_RD_MAX_TM_ID,   // 6字节,(当前)费率6正向无功最大需量发生时间,格式同上
    RT_POS_7_RD_MAX_TM_ID,   // 6字节,(当前)费率7正向无功最大需量发生时间,格式同上
    RT_POS_8_RD_MAX_TM_ID,   // 6字节,(当前)费率8正向无功最大需量发生时间,格式同上

    RT_POS_EXT_RD_MAX_TM_ID, // 6*6字节,(当前)费率（9—14）正向无功最大需量发生时间,格式同上
    RT_NEG_Z_RD_MAX_TM_ID,   // 6字节,(当前)反向无功总最大需量发生时间,格式同上
    RT_NEG_1_RD_MAX_TM_ID,   // 6字节,(当前)费率1反向无功最大需量发生时间,格式同上
    RT_NEG_2_RD_MAX_TM_ID,   // 6字节,(当前)费率2反向无功最大需量发生时间,格式同上
    RT_NEG_3_RD_MAX_TM_ID,   // 6字节,(当前)费率3反向无功最大需量发生时间,格式同上

    RT_NEG_4_RD_MAX_TM_ID,   // 6字节,(当前)费率4反向无功最大需量发生时间,格式同上
    RT_NEG_5_RD_MAX_TM_ID,   // 6字节,(当前)费率5反向无功最大需量发生时间,格式同上
    RT_NEG_6_RD_MAX_TM_ID,   // 6字节,(当前)费率6反向无功最大需量发生时间,格式同上
    RT_NEG_7_RD_MAX_TM_ID,   // 6字节,(当前)费率7反向无功最大需量发生时间,格式同上
    RT_NEG_8_RD_MAX_TM_ID,   // 6字节,(当前)费率8反向无功最大需量发生时间,格式同上

    RT_NEG_EXT_RD_MAX_TM_ID, // 6*6字节,(当前)费率（9—14）反向无功最大需量发生时间,BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_FIRST_Z_RD_MAX_TM_ID, // 6字节,(当前)象限1无功总最大需量发生时间,格式同上
    RT_FIRST_1_RD_MAX_TM_ID, // 6字节,(当前)象限1费率1无功最大需量发生时间,格式同上
    RT_FIRST_2_RD_MAX_TM_ID, // 6字节,(当前)象限1费率2无功最大需量发生时间,格式同上
    RT_FIRST_3_RD_MAX_TM_ID, // 6字节,(当前)象限1费率3无功最大需量发生时间,格式同上

    RT_FIRST_4_RD_MAX_TM_ID, // 6字节,(当前)象限1费率4无功最大需量发生时间,格式同上
    RT_FIRST_5_RD_MAX_TM_ID, // 6字节,(当前)象限1费率5无功最大需量发生时间,格式同上
    RT_FIRST_6_RD_MAX_TM_ID, // 6字节,(当前)象限1费率6无功最大需量发生时间,格式同上
    RT_FIRST_7_RD_MAX_TM_ID, // 6字节,(当前)象限1费率7无功最大需量发生时间,格式同上
    RT_FIRST_8_RD_MAX_TM_ID, // 6字节,(当前)象限1费率8无功最大需量发生时间,格式同上

    RT_FIRST_EXT_RD_MAX_TM_ID,// 6*6字节,(当前)象限1费率(9-14)无功最大需量发生时间,格式同上
    RT_SECOND_Z_RD_MAX_TM_ID,// 6字节,(当前)象限2无功总最大需量发生时间,格式同上
    RT_SECOND_1_RD_MAX_TM_ID,// 6字节,(当前)象限2费率1无功最大需量发生时间,格式同上
    RT_SECOND_2_RD_MAX_TM_ID,// 6字节,(当前)象限2费率2无功最大需量发生时间,格式同上
    RT_SECOND_3_RD_MAX_TM_ID,// 6字节,(当前)象限2费率3无功最大需量发生时间,格式同上

    RT_SECOND_4_RD_MAX_TM_ID,// 6字节,(当前)象限2费率4无功最大需量发生时间,格式同上
    RT_SECOND_5_RD_MAX_TM_ID,// 6字节,(当前)象限2费率5无功最大需量发生时间,格式同上
    RT_SECOND_6_RD_MAX_TM_ID,// 6字节,(当前)象限2费率6无功最大需量发生时间,格式同上
    RT_SECOND_7_RD_MAX_TM_ID,// 6字节,(当前)象限2费率7无功最大需量发生时间,格式同上
    RT_SECOND_8_RD_MAX_TM_ID,// 6字节,(当前)象限2费率8无功最大需量发生时间,格式同上

    RT_SECOND_EXT_RD_MAX_TM_ID,// 6*6字节,(当前)象限2费率(9-14)无功最大需量发生时间,BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_THIRD_Z_RD_MAX_TM_ID, // 6字节,(当前)象限3无功总最大需量发生时间,格式同上
    RT_THIRD_1_RD_MAX_TM_ID, // 6字节,(当前)象限3费率1无功最大需量发生时间,格式同上
    RT_THIRD_2_RD_MAX_TM_ID, // 6字节,(当前)象限3费率2无功最大需量发生时间,格式同上
    RT_THIRD_3_RD_MAX_TM_ID, // 6字节,(当前)象限3费率3无功最大需量发生时间,格式同上

    RT_THIRD_4_RD_MAX_TM_ID, // 6字节,(当前)象限3费率4无功最大需量发生时间,格式同上
    RT_THIRD_5_RD_MAX_TM_ID, // 6字节,(当前)象限3费率5无功最大需量发生时间,格式同上
    RT_THIRD_6_RD_MAX_TM_ID, // 6字节,(当前)象限3费率6无功最大需量发生时间,格式同上
    RT_THIRD_7_RD_MAX_TM_ID, // 6字节,(当前)象限3费率7无功最大需量发生时间,格式同上
    RT_THIRD_8_RD_MAX_TM_ID, // 6字节,(当前)象限3费率8无功最大需量发生时间,格式同上

    RT_THIRD_EXT_RD_MAX_TM_ID,// 6*6字节,(当前)象限3费率(9-14)无功最大需量发生时间,格式同上
    RT_FOURTH_Z_RD_MAX_TM_ID,// 6字节,(当前)象限4无功总最大需量发生时间,格式同上
    RT_FOURTH_1_RD_MAX_TM_ID,// 6字节,(当前)象限4费率1无功最大需量发生时间,格式同上
    RT_FOURTH_2_RD_MAX_TM_ID,// 6字节,(当前)象限4费率2无功最大需量发生时间,格式同上
    RT_FOURTH_3_RD_MAX_TM_ID,// 6字节,(当前)象限4费率3无功最大需量发生时间,格式同上

    RT_FOURTH_4_RD_MAX_TM_ID,// 6字节,(当前)象限4费率4无功最大需量发生时间,格式同上
    RT_FOURTH_5_RD_MAX_TM_ID,// 6字节,(当前)象限4费率5无功最大需量发生时间,格式同上
    RT_FOURTH_6_RD_MAX_TM_ID,// 6字节,(当前)象限4费率6无功最大需量发生时间,格式同上
    RT_FOURTH_7_RD_MAX_TM_ID,// 6字节,(当前)象限4费率7无功最大需量发生时间,格式同上
    RT_FOURTH_8_RD_MAX_TM_ID,// 6字节,(当前)象限4费率8无功最大需量发生时间,格式同上

    RT_FOURTH_EXT_RD_MAX_TM_ID,// 6*6字节,(当前)象限4费率(9-14)无功最大需量发生时间,BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_POS_A_Z_AD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)A相正向有功总最大需量
    RT_POS_B_Z_AD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)B相正向有功总最大需量
    RT_POS_C_Z_AD_MAX_EV_ID, // 4字节, BIN x 10000, (当前)C相正向有功总最大需量
    RT_POS_A_Z_AD_MAX_TM_ID, // 6字节,(当前)A相正向有功总最大需量发生时间, 6字节, BCD码(秒占最低字节，依次下去，年低两位占最高字节)

    RT_POS_B_Z_AD_MAX_TM_ID, // 6字节,(当前)B相正向有功总最大需量发生时间, 6字节, BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_POS_C_Z_AD_MAX_TM_ID, // 6字节,(当前)C相正向有功总最大需量发生时间, 6字节, BCD码(秒占最低字节，依次下去，年低两位占最高字节)
    RT_CMB_AE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)组合有功总电能
    RT_CMB_AE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1组合有功电能
    RT_CMB_AE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2组合有功电能

    RT_CMB_AE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3组合有功电能
    RT_CMB_AE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4组合有功电能
    RT_CMB_AE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5组合有功电能
    RT_CMB_AE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6组合有功电能
    RT_CMB_AE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7组合有功电能

    RT_CMB_AE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8组合有功电能
    RT_CMB_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）组合有功电能
    RT_EV_READ_TAG,          // 1字节, 终端抄表标记
    RT_EV_READ_TIME,         // 6字节, BCD码, 终端抄表时间：(秒占最低字节，依次下去，年低两位占最高字节)
    RT_VLT_A_PHASE_EV_ID,    // 2字节, BIN x 10, A相电压/AB线电压/单向表电压

    RT_VLT_B_PHASE_EV_ID,    // 2字节, BIN x 10, B相电压/无/无
    RT_VLT_C_PHASE_EV_ID,    // 2字节, BIN x 10, C相电压/CB线电压/无
    RT_VLT_ZERO_EV_ID,       // 2字节, BIN x 10, 中性线电压/无/无
    RT_CUR_A_PHASE_EV_ID,    // 4字节, BIN x 1000, A相电流/A相电流/单向表电流
    RT_CUR_B_PHASE_EV_ID,    // 4字节, BIN x 1000, B相电流/无/无

    RT_CUR_C_PHASE_EV_ID,    // 4字节, BIN x 1000, C相电流/C相电流/无
    RT_CUR_ZERO_EV_ID,       // 4字节, BIN x 1000, 零序电流/无/无
    RT_CUR_REMAIN,           // 4字节, BIN x 1000, 剩余电流
    RT_NEUTER_REMAIN,        // 4字节, BIN x 1000, 中性线电流
    RT_A_PHASE_AG_ID,        // 2字节, BIN x 10, A相/AB相电压电流相角

    RT_B_PHASE_AG_ID,        // 2字节, BIN x 10, B相电压电流相角
    RT_C_PHASE_AG_ID,        // 2字节, BIN x 10, C相/CB相电压电流相角
    RT_VLT_A_PHASE_AG_ID,    // 2字节, BIN x 10, A相电压相角/AB相电压相角
    RT_VLT_B_PHASE_AG_ID,    // 2字节, BIN x 10, B相电压相角/无
    RT_VLT_C_PHASE_AG_ID,    // 2字节, BIN x 10, C相电压相角/CB相电压相角

    RT_CUR_A_PHASE_AG_ID,    // 2字节, BIN x 10, A相电流相角
    RT_CUR_B_PHASE_AG_ID,    // 2字节, BIN x 10, B相电流相角
    RT_CUR_C_PHASE_AG_ID,    // 2字节, BIN x 10, C相电流相角
    RT_CUR_ZERO_AG_ID,       // 2字节, BIN x 10, 零序电流相角
    RT_POS_Z_AP_EV_ID,       // 4字节, BIN x 10000, 瞬时正向有功功率,都为正，与RT_NEG_Z_AP_EV_ID 互斥

    RT_NEG_Z_AP_EV_ID,       // 4字节, BIN x 10000, 瞬时反向有功功率
    RT_AP_A_PHASE_EV_ID,     // 4字节, BIN x 10000, A相有功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    RT_AP_B_PHASE_EV_ID,     // 4字节, BIN x 10000, B相有功功率,有正负,格式同上
    RT_AP_C_PHASE_EV_ID,     // 4字节, BIN x 10000, C相有功功率,有正负,格式同上
    RT_Z_AP_AG_ID,           // 2字节, BIN x 10, 瞬时有功功率相位

    RT_POS_Z_RP_EV_ID,       // 4字节, BIN x 10000, 瞬时正向无功功率,都为正，与RT_NEG_Z_RP_EV_ID 互斥
    RT_NEG_Z_RP_EV_ID,       // 4字节, BIN x 10000, 瞬时反向无功功率
    RT_RP_A_PHASE_EV_ID,     // 4字节, BIN x 10000, A相无功功率,有正负,自定义格式，为负时只将最高字节的最高位置1,如0f 00 00 80 代表-15
    RT_RP_B_PHASE_EV_ID,     // 4字节, BIN x 10000, B相无功功率,格式同上
    RT_RP_C_PHASE_EV_ID,     // 4字节, BIN x 10000, C相无功功率,格式同上

    RT_Z_RP_AG_ID,           // 2字节, BIN x 10, 瞬时无功功率相位
    RT_Z_PF_EV_ID,           // 2字节, BIN x 1000, 瞬时总功率因数
    RT_PF_A_PHASE_EV_ID,     // 2字节, BIN x 1000, A相功率因数
    RT_PF_B_PHASE_EV_ID,     // 2字节, BIN x 1000, B相功率因数
    RT_PF_C_PHASE_EV_ID,     // 2字节, BIN x 1000, C相功率因数

    RT_Z_UI_EV_ID,           // 4字节, BIN x 10000, 瞬时视在功率
    RT_UI_A_PHASE_EV_ID,     // 4字节, BIN x 10000, A相视在功率
    RT_UI_B_PHASE_EV_ID,     // 4字节, BIN x 10000, B相视在功率
    RT_UI_C_PHASE_EV_ID,     // 4字节, BIN x 10000, C相视在功率
    RT_FREQ_EV_ID,           // 2字节, BIN x 100, 频率

    VLT_NO_BALANCE_RATE,     // 2字节，BIN x 1000,电压不平衡度
    CUR_NO_BALANCE_RATE,     // 2字节，BIN x 1000,电流不平横度
    RT_OPEN_SWITCH_ID,       // 1字节，BIN,断路器分闸操作状态，aa成功，bb失败
    RT_CLOSE_SWITCH_ID,      // 1字节，BIN,断路器合闸操作状态，aa成功，bb失败
    RT_SWITCHA_STATE_ID,     // 1字节，BIN,无功投切开关状态，  aa成功，bb失败

    RT_SWITCHB_STATE_ID,     // 1字节，BIN,无功投切开关状态，  aa成功，bb失败
    RT_SWITCHC_STATE_ID,     // 1字节，BIN,无功投切开关状态，  aa成功，bb失败
    ENERGE_CONSERVATION_ID,  // 8字节，BIN,节能量（总累计）
    TOTAL_CAP_CONDENSER_ID,  // 8字节，BIN,三相共补投入容量（总累计）
    PART_CAP_CONDENSER_ID,   // 8字节，BIN,分相补偿投入容量（总累计）

    PHASE_CAP_CONDENSER_ID,  // 8字节，BIN,相间补偿投入容量（总累计）
    ALL_CAP_CONDENSER_ID,    // 8字节，BIN,无功补偿装置投入总容量（总累计）
    CONSERV_PF_ID,           // 2字节，BIN,无功补偿计算补充前功率因数
    RT_MTR_TEMP_EV_ID,       // 2字节, BIN x 10, 表内温度
    RT_CLOCK_BAT_VOL_EV_ID,  // 2字节, BIN x 100, 时钟电池电压

    RT_CB_BAT_VOL_EV_ID,     // 2字节, BIN x 100, 抄表电池电压
    RT_ADEMAND_EV_ID,        // 4字节, BIN x 10000, 有功需量
    RT_RDEMAND_EV_ID,        // 4字节, BIN x 10000, 无功需量
    RT_SDEMAND_EV_ID,        // 4字节, BIN x 10000, 视在需量
    VLT_A_PHASE_HR_O_PEC_ID, // 2字节, BIN x 100, A相电压总奇次谐波含有率

    VLT_B_PHASE_HR_O_PEC_ID, // 2字节, BIN x 100, B相电压总奇次谐波含有率
    VLT_C_PHASE_HR_O_PEC_ID, // 2字节, BIN x 100, C相电压总奇次谐波含有率
    VLT_A_PHASE_HR_E_PEC_ID, // 2字节, BIN x 100, A相电压总偶次谐波含有率
    VLT_B_PHASE_HR_E_PEC_ID, // 2字节, BIN x 100, B相电压总偶次谐波含有率
    VLT_C_PHASE_HR_E_PEC_ID, // 2字节, BIN x 100, C相电压总偶次谐波含有率

    SY_ER_READ_TAG,          // 1字节, 终端抄表标记
    SY_ER_READ_TIME,         // 6字节, BCD码, 终端抄表时间：秒分时日月年(低字节->高字节)
    SY_POS_AE_Z_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)正向有功总电能(+A)
    SY_POS_AE_1_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率1正向有功电能
    SY_POS_AE_2_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率2正向有功电能

    SY_POS_AE_3_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率3正向有功电能
    SY_POS_AE_4_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率4正向有功电能
    SY_POS_AE_5_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率5正向有功电能
    SY_POS_AE_6_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率6正向有功电能
    SY_POS_AE_7_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率7正向有功电能

    SY_POS_AE_8_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率8正向有功电能
    SY_POS_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）正向有功电能
    SY_NEG_AE_Z_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)反向有功总电能(-A)
    SY_NEG_AE_1_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率1反向有功电能
    SY_NEG_AE_2_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率2反向有功电能

    SY_NEG_AE_3_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率3反向有功电能
    SY_NEG_AE_4_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率4反向有功电能
    SY_NEG_AE_5_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率5反向有功电能
    SY_NEG_AE_6_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率6反向有功电能
    SY_NEG_AE_7_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率7反向有功电能

    SY_NEG_AE_8_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率8反向有功电能
    SY_NEG_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）反向有功电能
    SY_POS_RE_Z_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)正向无功总电能(+RL,+RC)
    SY_POS_RE_1_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率1正向无功电能
    SY_POS_RE_2_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率2正向无功电能

    SY_POS_RE_3_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率3正向无功电能
    SY_POS_RE_4_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率4正向无功电能
    SY_POS_RE_5_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率5正向无功电能
    SY_POS_RE_6_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率6正向无功电能
    SY_POS_RE_7_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率7正向无功电能

    SY_POS_RE_8_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率8正向无功电能
    SY_POS_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）正向无功电能
    SY_NEG_RE_Z_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)反向无功总电能(-RL，-RC)
    SY_NEG_RE_1_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率1反向无功电能
    SY_NEG_RE_2_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率2反向无功电能

    SY_NEG_RE_3_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率3反向无功电能
    SY_NEG_RE_4_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率4反向无功电能
    SY_NEG_RE_5_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率5反向无功电能
    SY_NEG_RE_6_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率6反向无功电能
    SY_NEG_RE_7_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率7反向无功电能

    SY_NEG_RE_8_ER_ID,       // 8字节, BIN x 10000, (抄表日冻结)费率8反向无功电能
    SY_NEG_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）反向无功电能
    SY_FIRST_RE_Z_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)一象限无功总电能(+RL)
    SY_FIRST_RE_1_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率1一象限无功电能
    SY_FIRST_RE_2_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率2一象限无功电能

    SY_FIRST_RE_3_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率3一象限无功电能
    SY_FIRST_RE_4_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率4一象限无功电能
    SY_FIRST_RE_5_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率5一象限无功电能
    SY_FIRST_RE_6_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率6一象限无功电能
    SY_FIRST_RE_7_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率7一象限无功电能

    SY_FIRST_RE_8_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率8一象限无功电能
    SY_FIRST_RE_EXT_ER_ID,   // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）一象限无功电能
    SY_FOURTH_RE_Z_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)四象限无功总电能(-RC)
    SY_FOURTH_RE_1_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率1四象限无功电能
    SY_FOURTH_RE_2_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率2四象限无功电能

    SY_FOURTH_RE_3_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率3四象限无功电能
    SY_FOURTH_RE_4_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率4四象限无功电能
    SY_FOURTH_RE_5_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率5四象限无功电能
    SY_FOURTH_RE_6_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率6四象限无功电能
    SY_FOURTH_RE_7_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率7四象限无功电能

    SY_FOURTH_RE_8_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率8四象限无功电能
    SY_FOURTH_RE_EXT_ER_ID,  // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）四象限无功电能
    SY_SECOND_RE_Z_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)二象限无功总电能(+RC)
    SY_SECOND_RE_1_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率1二象限无功电能
    SY_SECOND_RE_2_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率2二象限无功电能

    SY_SECOND_RE_3_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率3二象限无功电能
    SY_SECOND_RE_4_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率4二象限无功电能
    SY_SECOND_RE_5_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率5二象限无功电能
    SY_SECOND_RE_6_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率6二象限无功电能
    SY_SECOND_RE_7_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率7二象限无功电能

    SY_SECOND_RE_8_ER_ID,    // 8字节, BIN x 10000, (抄表日冻结)费率8二象限无功电能
    SY_SECOND_RE_EXT_ER_ID,  // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）二象限无功电能
    SY_THIRD_RE_Z_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)三象限无功总电能(-RL)
    SY_THIRD_RE_1_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率1三象限无功电能
    SY_THIRD_RE_2_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率2三象限无功电能

    SY_THIRD_RE_3_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率3三象限无功电能
    SY_THIRD_RE_4_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率4三象限无功电能
    SY_THIRD_RE_5_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率5三象限无功电能
    SY_THIRD_RE_6_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率6三象限无功电能
    SY_THIRD_RE_7_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率7三象限无功电能

    SY_THIRD_RE_8_ER_ID,     // 8字节, BIN x 10000, (抄表日冻结)费率8三象限无功电能
    SY_THIRD_RE_EXT_ER_ID,   // 8字节x6, BIN x 10000, (抄表日冻结)费率（9—14）三象限无功电能
    SY_L_RE_Z_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)感性无功总电能(+RL,-RL)
    SY_L_RE_1_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率1感性无功电能
    SY_L_RE_2_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率2感性无功电能

    SY_L_RE_3_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率3感性无功电能
    SY_L_RE_4_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率4感性无功电能
    SY_L_RE_5_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率5感性无功电能
    SY_L_RE_6_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率6感性无功电能
    SY_L_RE_7_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率7感性无功电能

    SY_L_RE_8_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率8感性无功电能
    SY_L_RE_EXT_ER_ID,       // 8字节x6, BIN x 10000, (抄表冻结日)费率（9—14）感性无功电能
    SY_C_RE_Z_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)容性无功总电能(+Rc,-Rc)
    SY_C_RE_1_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率1容性无功电能
    SY_C_RE_2_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率2容性无功电能

    SY_C_RE_3_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率3容性无功电能
    SY_C_RE_4_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率4容性无功电能
    SY_C_RE_5_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率5容性无功电能
    SY_C_RE_6_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率6容性无功电能
    SY_C_RE_7_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率7容性无功电能

    SY_C_RE_8_ER_ID,         // 8字节, BIN x 10000, (抄表冻结日)费率8容性无功电能
    SY_C_RE_EXT_ER_ID,       // 8字节x6, BIN x 10000, (抄表冻结日)费率（9—14）容性无功电能
    SY_POS_AE_A_ER_ID,       // 8字节, BIN x 10000, (上月)A相正向有功电能
    SY_POS_AE_B_ER_ID,       // 8字节, BIN x 10000, (上月)B相正向有功电能
    SY_POS_AE_C_ER_ID,       // 8字节, BIN x 10000, (上月)C相正向有功电能

    SY_NEG_AE_A_ER_ID,       // 8字节, BIN x 10000, (上月)A相反向有功电能
    SY_NEG_AE_B_ER_ID,       // 8字节, BIN x 10000, (上月)B相反向有功电能
    SY_NEG_AE_C_ER_ID,       // 8字节, BIN x 10000, (上月)C相反向有功电能
    SY_POS_RE_A_ER_ID,       // 8字节, BIN x 10000, (上月)A相感性无功电能
    SY_POS_RE_B_ER_ID,       // 8字节, BIN x 10000, (上月)B相感性无功电能

    SY_POS_RE_C_ER_ID,       // 8字节, BIN x 10000, (上月)C相感性无功电能
    SY_NEG_RE_A_ER_ID,       // 8字节, BIN x 10000, (上月)A相容性无功电能
    SY_NEG_RE_B_ER_ID,       // 8字节, BIN x 10000, (上月)B相容性无功电能
    SY_NEG_RE_C_ER_ID,       // 8字节, BIN x 10000, (上月)C相容性无功电能
    SY_CMB_AE_Z_ER_ID,       // 8字节, BIN x 10000, (上1结算日)组合有功总电能

    SY_CMB_AE_1_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率1组合有功电能
    SY_CMB_AE_2_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率2组合有功电能
    SY_CMB_AE_3_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率3组合有功电能
    SY_CMB_AE_4_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率4组合有功电能
    SY_CMB_AE_5_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率5组合有功电能

    SY_CMB_AE_6_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率6组合有功电能
    SY_CMB_AE_7_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率7组合有功电能
    SY_CMB_AE_8_ER_ID,       // 8字节, BIN x 10000, (上1结算日)费率8组合有功电能
    SY_CMB_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9—14）组合有功电能
    SY_DM_READ_TAG,          // 1字节, 终端抄表标记

    SY_DM_READ_TIME,         // 6字节, 终端抄表时间：秒分时日月年(低字节->高字节)
    SY_POS_Z_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结) 正向有功总最大需量
    SY_POS_1_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率1正向有功最大需量
    SY_POS_2_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率2正向有功最大需量
    SY_POS_3_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率3正向有功最大需量

    SY_POS_4_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率4正向有功最大需量
    SY_POS_5_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率5正向有功最大需量
    SY_POS_6_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率6正向有功最大需量
    SY_POS_7_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率7正向有功最大需量
    SY_POS_8_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率8正向有功最大需量

    SY_POS_EXT_AD_MAX_EV_ID, // 4字节x6, BIN x 10000, (抄表日冻结)费率（9—14）正向有功最大需量
    SY_NEG_Z_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结) 反向有功总最大需量
    SY_NEG_1_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率1反向有功最大需量
    SY_NEG_2_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率2反向有功最大需量
    SY_NEG_3_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率3反向有功最大需量

    SY_NEG_4_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率4反向有功最大需量
    SY_NEG_5_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率5反向有功最大需量
    SY_NEG_6_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率6反向有功最大需量
    SY_NEG_7_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率7反向有功最大需量
    SY_NEG_8_AD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率8反向有功最大需量

    SY_NEG_EXT_AD_MAX_EV_ID, // 4字节x6, BIN x 10000, (抄表日冻结)费率（9—14）反向有功最大需量
    SY_POS_Z_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结) 正向无功总最大需量
    SY_POS_1_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率1正向无功最大需量
    SY_POS_2_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率2正向无功最大需量
    SY_POS_3_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率3正向无功最大需量

    SY_POS_4_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率4正向无功最大需量
    SY_POS_5_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率5正向无功最大需量
    SY_POS_6_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率6正向无功最大需量
    SY_POS_7_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率7正向无功最大需量
    SY_POS_8_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率8正向无功最大需量

    SY_POS_EXT_RD_MAX_EV_ID, // 4字节x6, BIN x 10000, (抄表日冻结)费率（9—14）正向无功最大需量
    SY_NEG_Z_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结) 反向无功总最大需量
    SY_NEG_1_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率1反向无功最大需量
    SY_NEG_2_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率2反向无功最大需量
    SY_NEG_3_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率3反向无功最大需量

    SY_NEG_4_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率4反向无功最大需量
    SY_NEG_5_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率5反向无功最大需量
    SY_NEG_6_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率6反向无功最大需量
    SY_NEG_7_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率7反向无功最大需量
    SY_NEG_8_RD_MAX_EV_ID,   // 4字节, BIN x 10000, (抄表日冻结)费率8反向无功最大需量

    SY_NEG_EXT_RD_MAX_EV_ID, // 4字节x6, BIN x 10000, (抄表日冻结)费率（9—14）反向无功最大需量
    SY_FIRST_Z_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1无功总最大需量
    SY_FIRST_1_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率1无功最大需量
    SY_FIRST_2_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率2无功最大需量
    SY_FIRST_3_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率3无功最大需量

    SY_FIRST_4_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率4无功最大需量
    SY_FIRST_5_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率5无功最大需量
    SY_FIRST_6_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率6无功最大需量
    SY_FIRST_7_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率7无功最大需量
    SY_FIRST_8_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限1费率8无功最大需量

    SY_FIRST_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (抄表日冻结)象限1费率(9-14)无功最大需量
    SY_SECOND_Z_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2无功总最大需量
    SY_SECOND_1_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率1无功最大需量
    SY_SECOND_2_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率2无功最大需量
    SY_SECOND_3_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率3无功最大需量

    SY_SECOND_4_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率4无功最大需量
    SY_SECOND_5_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率5无功最大需量
    SY_SECOND_6_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率6无功最大需量
    SY_SECOND_7_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率7无功最大需量
    SY_SECOND_8_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限2费率8无功最大需量

    SY_SECOND_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (抄表日冻结)象限2费率(9-14)无功最大需量
    SY_THIRD_Z_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3无功总最大需量
    SY_THIRD_1_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率1无功最大需量
    SY_THIRD_2_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率2无功最大需量
    SY_THIRD_3_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率3无功最大需量

    SY_THIRD_4_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率4无功最大需量
    SY_THIRD_5_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率5无功最大需量
    SY_THIRD_6_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率6无功最大需量
    SY_THIRD_7_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率7无功最大需量
    SY_THIRD_8_RD_MAX_EV_ID, // 4字节, BIN x 10000, (抄表日冻结)象限3费率8无功最大需量

    SY_THIRD_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (抄表日冻结)象限3费率(9-14)无功最大需量
    SY_FOURTH_Z_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4无功总最大需量
    SY_FOURTH_1_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率1无功最大需量
    SY_FOURTH_2_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率2无功最大需量
    SY_FOURTH_3_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率3无功最大需量

    SY_FOURTH_4_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率4无功最大需量
    SY_FOURTH_5_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率5无功最大需量
    SY_FOURTH_6_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率6无功最大需量
    SY_FOURTH_7_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率7无功最大需量
    SY_FOURTH_8_RD_MAX_EV_ID,// 4字节, BIN x 10000, (抄表日冻结)象限4费率8无功最大需量

    SY_FOURTH_EXT_RD_MAX_EV_ID,// 4字节x6, BIN x 10000, (抄表日冻结)象限4费率(9-14)无功最大需量
    SY_POS_Z_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)正向有功总最大需量发生时间,BCD码(低字节->高字节, 秒分时日月年)
    SY_POS_1_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率1正向有功最大需量发生时间,格式同上
    SY_POS_2_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率2正向有功最大需量发生时间,格式同上
    SY_POS_3_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率3正向有功最大需量发生时间,格式同上

    SY_POS_4_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率4正向有功最大需量发生时间,格式同上
    SY_POS_5_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率5正向有功最大需量发生时间,格式同上
    SY_POS_6_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率6正向有功最大需量发生时间,格式同上
    SY_POS_7_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率7正向有功最大需量发生时间,格式同上
    SY_POS_8_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率8正向有功最大需量发生时间,格式同上

    SY_POS_EXT_AD_MAX_TM_ID, // 6字节x6,(抄表日冻结)费率（9—14）正向有功最大需量发生时间,格式同上
    SY_NEG_Z_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)反向有功总最大需量发生时间,格式同上
    SY_NEG_1_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率1反向有功最大需量发生时间,格式同上
    SY_NEG_2_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率2反向有功最大需量发生时间,格式同上
    SY_NEG_3_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率3反向有功最大需量发生时间,格式同上

    SY_NEG_4_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率4反向有功最大需量发生时间,格式同上
    SY_NEG_5_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率5反向有功最大需量发生时间,格式同上
    SY_NEG_6_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率6反向有功最大需量发生时间,格式同上
    SY_NEG_7_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率7反向有功最大需量发生时间,格式同上
    SY_NEG_8_AD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率8反向有功最大需量发生时间,格式同上

    SY_NEG_EXT_AD_MAX_TM_ID, // 6字节x6,(抄表日冻结)费率（9—14）反向有功最大需量发生时间,格式同上
    SY_POS_Z_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)正向无功总最大需量发生时间,BCD码(低字节->高字节, 秒分时日月年)
    SY_POS_1_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率1正向无功最大需量发生时间,格式同上
    SY_POS_2_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率2正向无功最大需量发生时间,格式同上
    SY_POS_3_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率3正向无功最大需量发生时间,格式同上

    SY_POS_4_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率4正向无功最大需量发生时间,格式同上
    SY_POS_5_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率5正向无功最大需量发生时间,格式同上
    SY_POS_6_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率6正向无功最大需量发生时间,格式同上
    SY_POS_7_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率7正向无功最大需量发生时间,格式同上
    SY_POS_8_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率8正向无功最大需量发生时间,格式同上

    SY_POS_EXT_RD_MAX_TM_ID, // 6字节x6,(抄表日冻结)费率（9—14）正向无功最大需量发生时间,格式同上
    SY_NEG_Z_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)反向无功总最大需量发生时间,格式同上
    SY_NEG_1_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率1反向无功最大需量发生时间,格式同上
    SY_NEG_2_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率2反向无功最大需量发生时间,格式同上
    SY_NEG_3_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率3反向无功最大需量发生时间,格式同上

    SY_NEG_4_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率4反向无功最大需量发生时间,格式同上
    SY_NEG_5_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率5反向无功最大需量发生时间,格式同上
    SY_NEG_6_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率6反向无功最大需量发生时间,格式同上
    SY_NEG_7_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率7反向无功最大需量发生时间,格式同上
    SY_NEG_8_RD_MAX_TM_ID,   // 6字节,(抄表日冻结)费率8反向无功最大需量发生时间,格式同上

    SY_NEG_EXT_RD_MAX_TM_ID, // 6字节x6,(抄表日冻结)费率（9—14）反向无功最大需量发生时间,格式同上
    SY_FIRST_Z_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1无功总最大需量发生时间,BCD码(低字节->高字节, 秒分时日月年)
    SY_FIRST_1_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率1无功最大需量发生时间,格式同上
    SY_FIRST_2_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率2无功最大需量发生时间,格式同上
    SY_FIRST_3_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率3无功最大需量发生时间,格式同上

    SY_FIRST_4_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率4无功最大需量发生时间,格式同上
    SY_FIRST_5_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率5无功最大需量发生时间,格式同上
    SY_FIRST_6_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率6无功最大需量发生时间,格式同上
    SY_FIRST_7_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率7无功最大需量发生时间,格式同上
    SY_FIRST_8_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限1费率8无功最大需量发生时间,格式同上

    SY_FIRST_EXT_RD_MAX_TM_ID,// 6字节x6,(抄表日冻结)象限1费率(9-14)无功最大需量发生时间,格式同上
    SY_SECOND_Z_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2无功总最大需量发生时间,格式同上
    SY_SECOND_1_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率1无功最大需量发生时间,格式同上
    SY_SECOND_2_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率2无功最大需量发生时间,格式同上
    SY_SECOND_3_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率3无功最大需量发生时间,格式同上

    SY_SECOND_4_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率4无功最大需量发生时间,格式同上
    SY_SECOND_5_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率5无功最大需量发生时间,格式同上
    SY_SECOND_6_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率6无功最大需量发生时间,格式同上
    SY_SECOND_7_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率7无功最大需量发生时间,格式同上
    SY_SECOND_8_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限2费率8无功最大需量发生时间,格式同上

    SY_SECOND_EXT_RD_MAX_TM_ID,// 6字节x6,(抄表日冻结)象限2费率(9-14)无功最大需量发生时间,格式同上
    SY_THIRD_Z_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3无功总最大需量发生时间,BCD码(低字节->高字节, 秒分时日月年)
    SY_THIRD_1_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率1无功最大需量发生时间,格式同上
    SY_THIRD_2_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率2无功最大需量发生时间,格式同上
    SY_THIRD_3_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率3无功最大需量发生时间,格式同上

    SY_THIRD_4_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率4无功最大需量发生时间,格式同上
    SY_THIRD_5_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率5无功最大需量发生时间,格式同上
    SY_THIRD_6_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率6无功最大需量发生时间,格式同上
    SY_THIRD_7_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率7无功最大需量发生时间,格式同上
    SY_THIRD_8_RD_MAX_TM_ID, // 6字节,(抄表日冻结)象限3费率8无功最大需量发生时间,格式同上

    SY_THIRD_EXT_RD_MAX_TM_ID,// 6字节x6,(抄表日冻结)象限3费率(9-14)无功最大需量发生时间,格式同上
    SY_FOURTH_Z_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4无功总最大需量发生时间,格式同上
    SY_FOURTH_1_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率1无功最大需量发生时间,格式同上
    SY_FOURTH_2_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率2无功最大需量发生时间,格式同上
    SY_FOURTH_3_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率3无功最大需量发生时间,格式同上

    SY_FOURTH_4_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率4无功最大需量发生时间,格式同上
    SY_FOURTH_5_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率5无功最大需量发生时间,格式同上
    SY_FOURTH_6_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率6无功最大需量发生时间,格式同上
    SY_FOURTH_7_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率7无功最大需量发生时间,格式同上
    SY_FOURTH_8_RD_MAX_TM_ID,// 6字节,(抄表日冻结)象限4费率8无功最大需量发生时间,格式同上

    SY_FOURTH_EXT_RD_MAX_TM_ID,// 6字节x6,(抄表日冻结)象限4费率(9-14)无功最大需量发生时间,格式同上
    SEVER_READ_TAG,          // 1字节, 终端抄表标记
    SEVER_READ_TIME,         // 6字节, BCD码, 终端抄表时间：秒分时日月年(低字节->高字节)
    SEVER_VLT_Z_TMS_ID,      // 4字节, 总断相次数
    SEVER_VLT_A_PHASE_TMS_ID,// 4字节, A相断相次数

    SEVER_VLT_B_PHASE_TMS_ID,// 4字节, B相断相次数
    SEVER_VLT_C_PHASE_TMS_ID,// 4字节, C相断相次数
    SEVER_VLT_Z_TM_ID,       // 4字节, 断相时间累计值
    SEVER_VLT_A_PHASE_TM_ID, // 4字节, A断相时间累计值
    SEVER_VLT_B_PHASE_TM_ID, // 4字节, B断相时间累计值

    SEVER_VLT_C_PHASE_TM_ID, // 4字节, C断相时间累计值
    SEVER_VLT_Z_TD_ID,       // 4字节, BCD码(分时日月, 低字节->高字节), 最近一次断相起始时刻
    SEVER_VLT_A_PHASE_TD_ID, // 4字节, BCD码(分时日月, 低字节->高字节), A相最近断相起始时刻
    SEVER_VLT_B_PHASE_TD_ID, // 4字节, BCD码(分时日月, 低字节->高字节), B相最近断相起始时刻
    SEVER_VLT_C_PHASE_TD_ID, // 4字节, BCD码(分时日月, 低字节->高字节), C相最近断相起始时刻

    SEVER_VLT_LAST_OFF_TD_ID,// 4字节, BCD码(分时日月, 低字节->高字节), 最近一次断相的结束时刻
    SEVER_VLT_A_PHASE_LAST_OFF_TD_ID,// 4字节, BCD码(分时日月, 低字节->高字节), A相最近一次断相的结束时刻
    SEVER_VLT_B_PHASE_LAST_OFF_TD_ID,// 4字节, BCD码(分时日月, 低字节->高字节), B相最近一次断相的结束时刻
    SEVER_VLT_C_PHASE_LAST_OFF_TD_ID,// 4字节, BCD码(分时日月, 低字节->高字节), C相最近一次断相的结束时刻
    LOST_ER_READ_TAG,        // 1字节, 终端抄表标记

    LOST_ER_READ_TIME,       // 6字节, 终端抄表时间：秒分时日月年(低字节->高字节)
    LOST_CUR_AE_A_PHASE_ER_ID,// 8字节, BIN x 1000, A相失流电量
    LOST_CUR_AE_B_PHASE_ER_ID,// 8字节, BIN x 1000, B相失流电量
    LOST_CUR_AE_C_PHASE_ER_ID,// 8字节, BIN x 1000, C相失流电量
    SEVER_VLT_AE_A_PHASE_ER_ID,// 8字节, BIN x 1000, A相断相电量

    SEVER_VLT_AE_B_PHASE_ER_ID,// 8字节, BIN x 1000, B相断相电量
    SEVER_VLT_AE_C_PHASE_ER_ID,// 8字节, BIN x 1000, C相断相电量
    VLT_REC_READ_TAG,        // 1字节, 终端抄表标记
    VLT_REC_READ_TIME,       // 6字节, 终端抄表时间：秒分时日月年(低字节->高字节)
    VLT_LOST_REC_ID1,        // 25字节, 电压失压记录
                                // #1: 1字节, *******1：UAB（UA）, *****1**：UCB（UC）
                                // #2: 6字节BCD码(低字节->高字节, 秒分时日月年),(失压相)起始时间;
                                // #3: 6字节BCD码(低字节->高字节, 秒分时日月年),结束时间(恢复时间);
                                // #4: 4字节, BIN x 1000, 未失压相的正向有功总电能;
                                // #5: 4字节, BIN x 1000, 未失压相的正向无功总电能;
                                // #6: 4字节, BIN x 1000, 失压相安培小时数

    VLT_LOST_REC_ID2,        // 25字节, 同上, 电压失压记录（最近第二组）
    VLT_LOST_REC_ID3,        // 25字节, 同上, 电压失压记录（最近第三组）
    VLT_LOST_REC_ID4,        // 25字节, 同上, 电压失压记录（最近第四组）
    VLT_LOST_REC_ID5,        // 25字节, 同上, 电压失压记录（最近第五组）
    VLT_LOST_REC_ID6,        // 25字节, 同上, 电压失压记录（最近第六组）

    VLT_LOST_REC_ID7,        // 25字节, 同上, 电压失压记录（最近第七组）
    VLT_LOST_REC_ID8,        // 25字节, 同上, 电压失压记录（最近第八组）
    VLT_LOST_REC_ID9,        // 25字节, 同上, 电压失压记录（最近第九组）
    POWER_REC_READ_TAG,      // 1字节, 终端抄表标记
    POWER_REC_READ_TIME,     // 6字节, 终端抄表时间：秒分时日月年(低字节->高字节)

    MET_POWER_REC_ID1,       // 10字节,电表来电记录（最近九组)
                                // #1: 3字节BCD(日月年, 低字节->高字节), 来电日期;
                                // #2: 2字节BCD(分时, 低字节->高字节), 来电时间;
                                // #3: 3字节BCD(日月年, 低字节->高字节), 停电日期;
                                // #4: 2字节BCD(分时, 低字节->高字节), 停电时间
    MET_POWER_REC_ID2,       // 10字节, 同上, 电表来电记录（最近第二组）
    MET_POWER_REC_ID3,       // 10字节, 同上, 电表来电记录（最近第三组）
    MET_POWER_REC_ID4,       // 10字节, 同上, 电表来电记录（最近第四组）
    MET_POWER_REC_ID5,       // 10字节, 同上, 电表来电记录（最近第五组）

    MET_POWER_REC_ID6,       // 10字节, 同上, 电表来电记录（最近第六组）
    MET_POWER_REC_ID7,       // 10字节, 同上, 电表来电记录（最近第七组）
    MET_POWER_REC_ID8,       // 10字节, 同上, 电表来电记录（最近第八组）
    MET_POWER_REC_ID9,       // 10字节, 同上, 电表来电记录（最近第九组）
    MET_STATE_READ_TAG,      // 1字节, 终端抄表标记

    MET_STATE_READ_TIME,     // 6字节, 终端抄表时间, BCD码：秒分时日月年(低字节->高字节)
    MET_CUR_DATE_TIME_ID,    // 6字节, 电表当前日期、时间 BCD码(日期低一字节；星期占第二字节的高三位，月份占后五位；年(低二位）占最高字节
    MET_WORK_STATE_ID,       // 1字节, 电表运行状态字,计量点原始状态字 D0：抄表：自动(0)/手动(1);
                                // D1：最大需量积算方式：滑差(0)/区间(1);D2：电池电压：正常(0)/欠压(1); D3:恒为0; 
                                // D4：有功电能方向：正向(0)/反向(1); D5:无功电能方向:正向(0)/反向(1); D6：保留; D7：恒为0;
    ENET_WORK_STATE_ID,      // 1字节  电网运行状态字 D0：A相断电；D1：B相断电；D2：C相断电；D3：恒为0；
                                // D4：A相过压；D5：B相过压；D6：C相过压；D7：恒为0；(1:断电（过压）0：正常);
    MET_ALARM_STATE_ID,      // 1字节, 电表故障状态字

    BAT_WORK_STATE_ID,       // 4字节, BCD, 电表电池工作时间
    LAST_PRAM_TD_ID,         // 6字节, BCD(秒分时日月--星期年, 低字节->高字节), 最近一次编程时间
    LAST_CLR_DM_MAX_TD_ID,   // 6字节, BCD(分时日月, 低字节->高字节), 最近一次最大需量清零时间
    MTR_CLEAR_CNT,           // 4个字节，BCD(个十百千万十万)电表清零总次数
    MTR_LAST_CLEAR_TIME,     // 6个字节，BCD(秒分时日月--星期年, 低字节->高字节),最近一次电表清零发生时刻

    EVENT_CLEAR_CNT,         // 4个字节，BCD(个十百千万十万)事件清零总次数
    EVENT_LAST_CLEAR_TIME,   // 6个字节，BCD(秒分时日月--星期年, 低字节->高字节),最近一次事件清零发生时刻
    CHECK_MTR_CNT,           // 4个字节，BCD(个十百千万十万)校时总次数
    CHECK_LAST_MTR_TIME,     // 6个字节，BCD(秒分时日月--星期年, 低字节->高字节),最近一次校时发生时刻
    CHECK_MTR_BEFORE_TIME,   // 6个字节，BCD(秒分时日月--星期年, 低字节->高字节),校时之前的时间

    CHECK_MTR_AFTER_TIME,    // 6个字节，BCD(秒分时日月--星期年, 低字节->高字节),校时之后的时间
    MET_CONST_AP_ID,         // 2字节, 有功电表常数
    MET_CONST_RP_ID,         // 2字节, 无功电表常数
    FIRST_DAY_PD_TIME_TARIFF,// 3*8字节, #1: 第一日时段表第1时段起始时间及费率号 3字节, BCD码(低位在前，高位在后（费率号、分、时）费率号：1：尖、2：峰、3：平、4：谷),
                                // 依次 2-8时段
    TOTAL_VLT_LOST_TM_ID,    // 4字节, 总失压时间

    A_VLT_LOST_TM_ID,        // 4字节, A相失压时间
    B_VLT_LOST_TM_ID,        // 4字节, B相失压时间
    C_VLT_LOST_TM_ID,        // 4字节, C相失压时间
    A_VLT_LOST_ER_ID,        // 8字节, BIN x 1000, A相失压电量
    B_VLT_LOST_ER_ID,        // 8字节, BIN x 1000, B相失压电量

    C_VLT_LOST_ER_ID,        // 8字节, BIN x 1000, C相失压电量
    TOTAL_CUR_LOST_TM_ID,    // 4字节, 总失流时间
    A_CUR_LOST_TM_ID,        // 4字节, A相失流时间
    B_CUR_LOST_TM_ID,        // 4字节, B相失流时间
    C_CUR_LOST_TM_ID,        // 4字节, C相失流时间

    MET_HR_READ_TAG,         // 1字节, 终端抄表标记
    MET_HR_READ_TIME,        // 6字节, 终端抄表时间：秒分时日月年(低字节->高字节)
    VLT_A_PHASE_HR_X_VAL_ID, // 2字节x19, BIN x 100, A相电压基波、2~19次谐波有效值
    VLT_B_PHASE_HR_X_VAL_ID, // 2字节x19, BIN x 100, B相电压基波、2~19次谐波有效值
    VLT_C_PHASE_HR_X_VAL_ID, // 2字节x19, BIN x 100, C相电压基波、2~19次谐波有效值

    CUR_A_PHASE_HR_X_VAL_ID, // 2字节x19, BIN(有符号原码) x 100, A相电流基波(非总有效值)、2~19次谐波有效值
    CUR_B_PHASE_HR_X_VAL_ID, // 2字节x19, BIN(有符号原码) x 100, B相电流基波(非总有效值)、2~19次谐波有效值
    CUR_C_PHASE_HR_X_VAL_ID, // 2字节x19, BIN(有符号原码) x 100, C相电流基波(非总有效值)、2~19次谐波有效值
    VLT_A_PHASE_HR_Z_VAL_ID, // 2字节, BIN x 100, A相电压总谐波有效值
    VLT_B_PHASE_HR_Z_VAL_ID, // 2字节, BIN x 100, B相电压总谐波有效值

    VLT_C_PHASE_HR_Z_VAL_ID, // 2字节, BIN x 100, C相电压总谐波有效值
    CUR_A_PHASE_HR_Z_VAL_ID, // 2字节, BIN(有符号原码) x 100, A相电流总谐波有效值
    CUR_B_PHASE_HR_Z_VAL_ID, // 2字节, BIN(有符号原码) x 100, B相电流总谐波有效值
    CUR_C_PHASE_HR_Z_VAL_ID, // 2字节, BIN(有符号原码) x 100, C相电流总谐波有效值
    VLT_A_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, A相电压2~19次谐波含有率

    VLT_B_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, B相电压2~19次谐波含有率
    VLT_C_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, C相电压2~19次谐波含有率
    CUR_A_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, A相电流2~19次谐波含有率
    CUR_B_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, B相电流2~19次谐波含有率
    CUR_C_PHASE_HR_X_PEC_ID, // 2字节x18, BIN x 100, C相电流2~19次谐波含有率

    VLT_A_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, A相电压总谐波含有率
    VLT_B_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, B相电压总谐波含有率
    VLT_C_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, C相电压总谐波含有率
    CUR_A_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, A相电流总谐波含有率
    CUR_B_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, B相电流总谐波含有率

    CUR_C_PHASE_HR_Z_PEC_ID, // 2字节, BIN x 100, C相电流总谐波含有率
    CB_FAILURE_TIMES,        // 1字节, 抄表失败次数
    RT_DATA_TAG,             // 1字节, 当前数据区状态标志
                                // D0:抄表任务更新数据标志：0未在更新，1正在更新；
                                // D1:统计当前计量点数据标志：0未在统计，1正在统计；
                                // D2:召测当前计量点数据标志：0未在召测，1正在召测；
                                // D3-D5:备用；
                                // D6:最近统计时抄表标志；
                                // D7:最近抄表标志：0和1交替变化
    METER_NUM,               // 6字节, 表号
    USER_NUM,                // 6字节, 用户号

    MtrEsamSerialNo_ID,      // 8字节,电表ESAM序列号
    MtrSecModEn_ID,          // 1字节,电表安全模式使能开关
    DEVICE_NUM,              // 6字节, 设备号
    MIN15_POS_AE_Z_ER_LAST_ID,// 8字节, BIN x 10000, (15分钟前)正向有功总电能(+RL,+RC)
    MIN15_POS_AE_Z_ER_ID,    // 8字节, BIN x 10000, (15分钟)正向有功电能量(+RL,+RC)

    MIN15_NEG_AE_Z_ER_LAST_ID,// 8字节, BIN x 10000, (15分钟前)反向有功总电能(+RL,+RC)
    MIN15_NEG_AE_Z_ER_ID,    // 8字节, BIN x 10000, (15分钟)反向有功电能量(+RL,+RC)
    MIN15_POS_RE_Z_ER_LAST_ID,// 8字节, BIN x 10000, (15分钟前)正向无功总电能(+RL,+RC)
    MIN15_POS_RE_Z_ER_ID,    // 8字节, BIN x 10000, (15分钟)正向无功电能量(+RL,+RC)
    MIN15_NEG_RE_Z_ER_LAST_ID,// 8字节, BIN x 10000, (15分钟前)反向无功总电能(+RL,+RC)

    MIN15_NEG_RE_Z_ER_ID,    // 8字节, BIN x 10000, (15分钟)反向无功电能量(+RL,+RC)
    MIN_POS_AE_Z_ER_LAST_ID, // 8字节, BIN x 10000, (1分钟前)正向有功总电能(+RL,+RC)
    MIN_POS_AE_Z_ER_ID,      // 8字节, BIN x 10000, (1分钟)正向有功电能量(+RL,+RC)
    MIN_NEG_AE_Z_ER_LAST_ID, // 8字节, BIN x 10000, (1分钟前)反向有功总电能(+RL,+RC)
    MIN_NEG_AE_Z_ER_ID,      // 8字节, BIN x 10000, (1分钟)反向有功电能量(+RL,+RC)

    VIR_MIN_Z,               // 8字节, BIN x 10000(kwh), 前一分钟总加有功电能量（总）
    VIR_POWER_ACT_RT,        // 4字节, BIN x 100(kw), 当前总加有功功率
    VIR_POWER_REACT_RT,      // 4字节, BIN x 100(kw), 当前总加无功功率
    VIR_POWER_ACT_AVG,       // 4字节, BIN x 100(kw), 当前总加有功滑差功率
    VIR_POWER_REACT_AVG,     // 4字节, BIN x 100(kw), 当前总加无功滑差功率

    VIR_AE_RT_Z,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（总）
    VIR_AE_RT_1,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（尖）
    VIR_AE_RT_2,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（峰）
    VIR_AE_RT_3,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（平）
    VIR_AE_RT_4,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（谷）

    VIR_AE_RT_5,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（5费率）
    VIR_AE_RT_6,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（6费率）
    VIR_AE_RT_7,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（7费率）
    VIR_AE_RT_8,             // 8字节, BIN x 10000(kwh), 当前总加有功电能量（8费率）
    VIR_AE_RT_9_14,          // 8字节x6, BIN x 10000(kwh), 当前总加有功电能量（9~14费率）

    VIR_RE_RT_Z,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（总）
    VIR_RE_RT_1,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（尖）
    VIR_RE_RT_2,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（峰）
    VIR_RE_RT_3,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（平）
    VIR_RE_RT_4,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（谷）

    VIR_RE_RT_5,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（5费率）
    VIR_RE_RT_6,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（6费率）
    VIR_RE_RT_7,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（7费率）
    VIR_RE_RT_8,             // 8字节, BIN x 10000(kwh), 当前总加无功电能量（8费率）
    VIR_RE_RT_9_14,          // 8字节x6, BIN x 10000(kwh), 当前总加无功电能量（9~14费率）

    JLD_STATUS,              // 2字节, 计量点状态字  低字节：D3电流极性,0正1反 D2电流相序, 0正1逆 D1电压缺相,0正1缺 D0电压相序,0正1逆
                                // 高字节：D3功率因数,0合格1不合格 D2C相负荷,0正常1过载 D1B相负荷,0正常1过载 D0A相负荷,0正常1过载
    CB_DAY_ID,               // 2字节, BCD码, 抄表日
    ORIG_AE_ER_ID,           // 8字节, BIN x 10000(kwh), 有功电能表起始读数
    ORIG_RE_ER_ID,           // 8字节, BIN x 10000(kwh), 无功电能表起始读数
    MET_CUR_DATE_ID,         // 3字节, 电表当前日期

    MET_CUR_TIME_ID,         // 3字节, 电表当前时间
    VIR_AP_FROZEN_DN_CTRL,   // 4byte,控制组的当前功率下浮控控后总加有功功率冻结值
    VIR_BUY_EN_CTRL_INFO,    // 8byte 控制组的购电控剩余电量  一次值,单位:0.0001kwh
    RT_AVR_VLT_A_PHASE_ID,   // 2字节, A相分钟平均电压 BIN x 10
    RT_AVR_VLT_B_PHASE_ID,   // 2字节, B相分钟平均电压 BIN x 10

    RT_AVR_VLT_C_PHASE_ID,   // 2字节, C相分钟平均电压 BIN x 10
    RT_POS_AE_Z_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前正向总有功分钟电量
    RT_POS_AE_1_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（尖）正向有功分钟电量
    RT_POS_AE_2_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（峰）正向有功分钟电量
    RT_POS_AE_3_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（平）正向有功分钟电量

    RT_POS_AE_4_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（谷）正向有功分钟电量
    RT_NEG_AE_Z_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前反向总有功分钟电量
    RT_NEG_AE_1_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（尖）反向有功分钟电量
    RT_NEG_AE_2_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（峰）反向有功分钟电量
    RT_NEG_AE_3_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（平）反向有功分钟电量

    RT_NEG_AE_4_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（谷）反向有功分钟电量
    RT_POS_RE_Z_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前正向总无功分钟电量
    RT_POS_RE_1_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（尖）正向无功分钟电量
    RT_POS_RE_2_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（峰）正向无功分钟电量
    RT_POS_RE_3_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（平）正向无功分钟电量

    RT_POS_RE_4_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（谷）正向无功分钟电量
    RT_NEG_RE_Z_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前反向总无功分钟电量
    RT_NEG_RE_1_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（尖）反向无功分钟电量
    RT_NEG_RE_2_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（峰）反向无功分钟电量
    RT_NEG_RE_3_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（平）反向无功分钟电量

    RT_NEG_RE_4_ER_DAY_MIN,  // 8字节，BIN X 10000 （KWH），当前费率段（谷）反向无功分钟电量
    LH_ER_READ_TIME,         // 6字节，上小时电量抄表时间  BCD码, 秒分时日月年
    LH_POS_AE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)正向有功总电能
    LH_POS_AE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向有功电能
    LH_POS_AE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向有功电能

    LH_POS_AE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向有功电能
    LH_POS_AE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向有功电能
    LH_POS_AE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向有功电能
    LH_POS_AE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向有功电能
    LH_POS_AE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向有功电能

    LH_POS_AE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向有功电能
    LH_POS_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9-14）正向有功电能
    LH_POS_RE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)反向有功总电能
    LH_POS_RE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向有功电能
    LH_POS_RE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向有功电能

    LH_POS_RE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向有功电能
    LH_POS_RE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向有功电能
    LH_POS_RE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向有功电能
    LH_POS_RE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向有功电能
    LH_POS_RE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向有功电能

    LH_POS_RE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向有功电能
    LH_POS_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9-14）正向有功电能
    LH_NEG_AE_Z_ER_ID,       // 8字节, BIN x 10000, (当前)正向有功总电能
    LH_NEG_AE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向有功电能
    LH_NEG_AE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向有功电能

    LH_NEG_AE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向有功电能
    LH_NEG_AE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向有功电能
    LH_NEG_AE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向有功电能
    LH_NEG_AE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向有功电能
    LH_NEG_AE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向有功电能

    LH_NEG_AE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向有功电能
    LH_NEG_AE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9-14）正向有功电能
    EG_RE_Z_ER_ID,           // 8字节, BIN x 10000, (当前)正向有功总电能
    LH_NEG_RE_1_ER_ID,       // 8字节, BIN x 10000, (当前)费率1正向有功电能
    LH_NEG_RE_2_ER_ID,       // 8字节, BIN x 10000, (当前)费率2正向有功电能

    LH_NEG_RE_3_ER_ID,       // 8字节, BIN x 10000, (当前)费率3正向有功电能
    LH_NEG_RE_4_ER_ID,       // 8字节, BIN x 10000, (当前)费率4正向有功电能
    LH_NEG_RE_5_ER_ID,       // 8字节, BIN x 10000, (当前)费率5正向有功电能
    LH_NEG_RE_6_ER_ID,       // 8字节, BIN x 10000, (当前)费率6正向有功电能
    LH_NEG_RE_7_ER_ID,       // 8字节, BIN x 10000, (当前)费率7正向有功电能

    LH_NEG_RE_8_ER_ID,       // 8字节, BIN x 10000, (当前)费率8正向有功电能
    LH_NEG_RE_EXT_ER_ID,     // 8字节x6, BIN x 10000, (当前)费率（9-14）正向功电能
    SD_POS_AE_READ_TAG,      // 1字节, 终端抄表标记
    SD_POS_AE_READ_TIME,     // 6字节, BCD码, 终端抄表时间：秒分时日月年(低字节->高字节)
    SD_POS_AE_Z_ID,          // 8字节，BIN X 10000 （kWh），上日正向有功电量（总）

    SD_POS_AE_1_ID,          // 8字节，BIN X 10000 （kWh），上日正向有功电量 （尖）
    SD_POS_AE_2_ID,          // 8字节，BIN X 10000 （kWh），上日正向有功电量 （峰）
    SD_POS_AE_3_ID,          // 8字节，BIN X 10000 （kWh），上日正向有功电量 （平）
    SD_POS_AE_4_ID,          // 8字节，BIN X 10000 （kWh），上日正向有功电量 （谷）
    VLT_A_LOST_TIMES_ID,     // 2字节，BCD码，格式xxxx次,本月A相失压总次数

    VLT_B_LOST_TIMES_ID,     // 2字节，BCD码，格式xxxx次,本月B相失压总次数
    VLT_C_LOST_TIMES_ID,     // 2字节，BCD码，格式xxxx次,本月C相失压总次数
    CURR_A_LOST_TIMES_ID,    // 2字节，BCD码，格式xxxx次,本月A相失流总次数
    CURR_B_LOST_TIMES_ID,    // 2字节，BCD码，格式xxxx次,本月B相失流总次数
    CURR_C_LOST_TIMES_ID,    // 2字节，BCD码，格式xxxx次,本月C相失流总次数

    Mtr_Cur_IronCopper_Loss_Time,// 5个字节，BCD码分时日月年终端抄表时间
    Mtr_Cur_Copper_Loss_ID,  // 8字节, BIN x 10000,当前铜损有功总电能示值
    Mtr_Cur_Iron_Loss_ID,    // 8字节, BIN x 10000,当前铜损有功总电能示值
    Mtr_Last_IronCopper_Loss_Time,// 5个字节，BCD码分时日月年终端抄表时间
    Mtr_Last_Copper_Loss_ID, // 8字节, BIN x 10000,上一结算日铜损有功总电能补偿量

    Mtr_Last_Iron_Loss_ID,   // 8字节, BIN x 10000,上一结算日铜损有功总电能补偿量
    Mtr_Collect_Comm_Port,   // 1个字节，BIN码，所属通信端口号
    Mtr_Relay_Router_Level,  // 1个字节，BIN码，中继路由级数-*/
    Mtr_Collect_Phase,       // 1个字节，D6~D4抄读相位，D0~D2实际相位-*/
    Mtr_Collect_Signal_Qt,   // 1个字节， D7~D4编码表示电表发送载波信号品质，数值范围1~15
                                // D3~D0编码表示电表接收载波信号品质，数值范围1~15-*/

    Mtr_Collect_Succ_Flag,   // 1个字节，最近一次抄读成功标志１抄读成功，０抄读失败
    Mtr_Collect_Succ_Time,   // 6个字节BCD码秒分时日月星期年．最近一次抄表成功时间
    Mtr_Collect_Fail_Time,   // 6个字节BCD码秒分时日月星期年．最近一次抄表失败时间
    Mtr_Collect_ContFial_Num,// 1个字节，最近连续失败累计次数
    METER_SWITCH_STATE_ID,   // 1字节 电能表通断电状态	BIN		1

    LAST_SWITCH_CLOSE_TIME_ID,// 6字节BCD 最近一次电能表远程控制通电时间
    LAST_SWITCH_CUT_TIME_ID, // 6字节BCD 最近一次电能表远程控制断电时间	
    MET_PROGRAM_CNT,         // 3个字节, BCD编程开关次数 
    MET_LAST_PROGRAM_TIME,   // 6个字节,分时日月年,最近一次编程时间 
    MET_LAST_COVER_OPEN_TIME,// 6个字节,分时日月年,最近一次尾盖打开时间 

    MET_CLOCK_MODIF_CNT,     // 3个字节,电能表时钟修改次数-
    MET_LAST_CLOCK_MODIF_TIME,// 6个字节，电能表最近一次时钟修改时间
    MET_CLOCKCHIP_MODIF_CNT, // 3个字节,电能表时段参数修改次数-
    MET_LAST_CLOCKCHIP_TIME, // 6个字节，最近一次电能表时段参数修改时间-
    MET_PAY_ELECTR_TIME,     // 2个字节, BCD,购电次数-*/

    MET_REMAIN_MONEY,        // 5个字节,BCD,剩余金额-
    MET_TOTAL_USED_MONEY,    // 5个字节,BCD,累计购电金额-
    MET_REMAIN_ELETRIC,      // 4个字节,BCD,剩余电量-
    MET_OVERFRAFT_ELETRIC,   // 4个字节,BCD,透支电量
    MET_TOTAL_PAYED_ELETRIC, // 4个字节,BCD,累计购电量

    MET_TICK_FACK_ELETRIC,   // 4个字节,BCD,赊欠门限电量
    MET_ALARM_ELETRIC,       // 4个字节,BCD,报警电量
    MET_FAULT_ELETRIC,       // 4个字节,BCD,故障电量
    MET_BALANCE_AE_Z_ER_ID,  // 8字节, BIN x 10000, 已结有功总电能
    MET_BALANCE_AE_1_ER_ID,  // 8字节, BIN x 10000, (当前)费率1已结有功总电能

    MET_BALANCE_AE_2_ER_ID,  // 8字节, BIN x 10000, (当前)费率2已结有功总电能
    MET_BALANCE_AE_3_ER_ID,  // 8字节, BIN x 10000, (当前)费率3已结有功总电能
    MET_BALANCE_AE_4_ER_ID,  // 8字节, BIN x 10000, (当前)费率4已结有功总电能
    MET_BALANCE_AE_5_ER_ID,  // 8字节, BIN x 10000, (当前)费率5已结有功总电能
    MET_BALANCE_AE_6_ER_ID,  // 8字节, BIN x 10000, (当前)费率6已结有功总电能

    MET_BALANCE_AE_7_ER_ID,  // 8字节, BIN x 10000, (当前)费率7已结有功总电能
    MET_BALANCE_AE_8_ER_ID,  // 8字节, BIN x 10000, (当前)费率8已结有功总电能
    MET_BALANCE_AE_EXT_ER_ID,// 8字节x6, BIN x 10000, (当前)费率（9—14）已结有功总电能
    MET_UNBALANCE_AE_Z_ER_ID,// 8字节, BIN x 10000, 未结有功总电能
    MET_UNBALANCE_AE_1_ER_ID,// 8字节, BIN x 10000, (当前)费率1未结有功总电能

    MET_UNBALANCE_AE_2_ER_ID,// 8字节, BIN x 10000, (当前)费率2未结有功总电能
    MET_UNBALANCE_AE_3_ER_ID,// 8字节, BIN x 10000, (当前)费率3未结有功总电能
    MET_UNBALANCE_AE_4_ER_ID,// 8字节, BIN x 10000, (当前)费率4未结有功总电能
    MET_UNBALANCE_AE_5_ER_ID,// 8字节, BIN x 10000, (当前)费率5未结有功总电能
    MET_UNBALANCE_AE_6_ER_ID,// 8字节, BIN x 10000, (当前)费率6未结有功总电能

    MET_UNBALANCE_AE_7_ER_ID,// 8字节, BIN x 10000, (当前)费率7未结有功总电能
    MET_UNBALANCE_AE_8_ER_ID,// 8字节, BIN x 10000, (当前)费率8未结有功总电能
    MET_UNBALANCE_AE_EXT_ER_ID,// 8字节x6, BIN x 10000, (当前)费率（9—14）未结有功总电能
    TIME_ZERO_ER_READ_TAG,   // 1字节, 终端抄表标记
    TIME_ZERO_ER_READ_TIME,  // 6字节, BCD码, 字节终端抄表时间：秒分时日月年-

    TIME_ZERO_POS_AE_Z_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID1,// 8字节, BIN x 10000, (第一时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID1,// 8字节x6, BIN x 10000, (第一时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID2,// 8字节, BIN x 10000, (第二时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID2,// 8字节x6, BIN x 10000, (第二时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID3,// 8字节, BIN x 10000, (第三时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID3,// 8字节x6, BIN x 10000, (第三时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID4,// 8字节, BIN x 10000, (第四时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID4,// 8字节x6, BIN x 10000, (第四时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID5,// 8字节, BIN x 10000, (第五时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID5,// 8字节x6, BIN x 10000, (第五时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID6,// 8字节, BIN x 10000, (第六时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID6,// 8字节x6, BIN x 10000, (第六时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID7,// 8字节, BIN x 10000, (第七时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID7,// 8字节x6, BIN x 10000, (第七时区冻结)费率（9—14）正向有功电能

    TIME_ZERO_POS_AE_Z_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)正向有功总电能-
    TIME_ZERO_POS_AE_1_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率1正向有功电能-
    TIME_ZERO_POS_AE_2_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率2正向有功电能-
    TIME_ZERO_POS_AE_3_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率3正向有功电能-
    TIME_ZERO_POS_AE_4_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率4正向有功电能-

    TIME_ZERO_POS_AE_5_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率5正向有功电能-
    TIME_ZERO_POS_AE_6_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率6正向有功电能-
    TIME_ZERO_POS_AE_7_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率7正向有功电能-
    TIME_ZERO_POS_AE_8_ER_ID8,// 8字节, BIN x 10000, (第八时区冻结)费率8正向有功电能-
    TIME_ZERO_POS_AE_EXT_ER_ID8,// 8字节x6, BIN x 10000, (第八时区冻结)费率（9—14）正向有功电能

    THRO_REMOTE_CTRL_CMD_FLAG_ID,// 1字节,BIN,对电表拉合闸控制_命令标志(AAH:置标志 | 55H:清标志)
    THRO_REMOTE_CTRL_ALARM_FLAG_ID,// 1字节,BIN,对电表拉合闸控制_告警标志(AAH:置标志 | 55H:清标志)
    THRO_REMOTE_CTRL_PWD_ID, // 4字节,BCD,对电表拉合闸控制_电表密码
    THRO_REMOTE_CTRL_WORD_ID,// 1字节,BIN,对电表拉合闸控制_控制字(AAH：拉闸 | 55H：合闸)
    THRO_REMOTE_CTRL_FINISH_FLAG_ID,// 1字节,BIN,对电表拉合闸控制_完成标志(00H:未完成 | AAH:确认 | 55H:否认)

    PLC_MTR_COMM_FAILUR_FLAG,// 1字节，载波电表抄不通标记，BIN
    PLC_MTR_COMM_FAILUR_COUNT,// 1字节，载波电表抄不通计数器，BIN
    VLT_A_LOST_ID,           // 1字节，BIN码，当前A相失压标志，0x55有效，其它无效
    VLT_B_LOST_ID,           // 1字节，BIN码，当前B相失压标志，0x55有效，其它无效
    VLT_C_LOST_ID,           // 1字节，BIN码，当前C相失压标志，0x55有效，其它无效

    CURR_A_LOST_ID,          // 1字节，BIN码，当前A相失流标志，0x55有效，其它无效
    CURR_B_LOST_ID,          // 1字节，BIN码，当前B相失流标志，0x55有效，其它无效
    CURR_C_LOST_ID,          // 1字节，BIN码，当前C相失流标志，0x55有效，其它无效
    MET_WORK_STATE_2007_ID,  // 2*7字节,DLT2007电表的运行状态字
    MET_WORK_STATE_2007_LST_ID,// 2*7字节, 前一次抄表的 DLT2007电表的运行状态字

    WORKSTATE_2007_CHG_APP_ID,// 2*7字节,DLT2007电表的运行状态字变位标志,供规约调用
    SEVER_VLT_A_P_WITH_YEAR_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), A相最近断相起始时刻
    SEVER_VLT_B_P_WITH_YEAR_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), B相最近断相起始时刻
    SEVER_VLT_C_P_WITH_YEAR_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), C相最近断相起始时刻
    SEVER_VLT_A_P_WITH_YEAR_LAST_OFF_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), A相最近一次断相的结束时刻

    SEVER_VLT_B_P_WITH_YEAR_LAST_OFF_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), B相最近一次断相的结束时刻
    SEVER_VLT_C_P_WITH_YEAR_LAST_OFF_TD_ID,// 5字节, BCD码(分时日月, 低字节->高字节), C相最近一次断相的结束时刻
    PlC_PN_AMR_FAILURE_ACC_ID,// 1字节, BIN码，每一个测量点抄表失败累计次数
    MET_STATE_WORD_1_2007_ID,// 2字节,DLT2007电表的运行状态字1
    MET_STATE_WORD_2_2007_ID,// 2字节,DLT2007电表的运行状态字2

    MET_STATE_WORD_3_2007_ID,// 2字节,DLT2007电表的运行状态字3
    MET_STATE_WORD_4_2007_ID,// 2字节,DLT2007电表的运行状态字4
    MET_STATE_WORD_5_2007_ID,// 2字节,DLT2007电表的运行状态字5
    MET_STATE_WORD_6_2007_ID,// 2字节,DLT2007电表的运行状态字6
    MET_STATE_WORD_7_2007_ID,// 2字节,DLT2007电表的运行状态字7

    MET_STATE_WORD_DCFLAG_ID,// 1字节,DLT2007电表的运行状态字单抄标志, 1:单抄 0:块抄
    PRAM_TMS_ID,             // 4字节, BCD(低位在前，高位在后), 编程次数
    PRAM_CHANGE_TMS_ID,      // 2字节，编程变化次数变更标志,每BIT表示上几次发生变化
    LAST_PROGRAMRECORD_1,    // 6字节,（上1次）编程记录发生时刻
    LAST_PROGRAMRECORD_2,    // 6字节,（上2次）编程记录发生时刻

    LAST_PROGRAMRECORD_3,    // 6字节,（上3次）编程记录发生时刻
    LAST_PROGRAMRECORD_4,    // 6字节,（上4次）编程记录发生时刻
    LAST_PROGRAMRECORD_5,    // 6字节,（上5次）编程记录发生时刻
    LAST_PROGRAMRECORD_6,    // 6字节,（上6次）编程记录发生时刻
    LAST_PROGRAMRECORD_7,    // 6字节,（上7次）编程记录发生时刻

    LAST_PROGRAMRECORD_8,    // 6字节,（上8次）编程记录发生时刻
    LAST_PROGRAMRECORD_9,    // 6字节,（上9次）编程记录发生时刻
    LAST_PROGRAMRECORD_10,   // 6字节,（上10次）编程记录发生时刻
    CLR_DM_MAX_TMS_ID,       // 4字节，BCD(低位在前，高位在后), 最大需量清零次数
    CLR_DM_CHANGE_TMS_ID,    // 2字节,需量清零变化次数变更标志,每BIT表示上几次发生变化

    LAST_CLR_DM_MAX_ID_1,    // 6字节,（上1次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_2,    // 6字节,（上2次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_3,    // 6字节,（上3次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_4,    // 6字节,（上4次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_5,    // 6字节,（上5次）需量清零记录发生时刻

    LAST_CLR_DM_MAX_ID_6,    // 6字节,（上6次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_7,    // 6字节,（上7次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_8,    // 6字节,（上8次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_9,    // 6字节,（上9次）需量清零记录发生时刻
    LAST_CLR_DM_MAX_ID_10,   // 6字节,（上10次）需量清零记录发生时刻

    LOST_Z_VLT_NUM_ID,       // 3字节失压总次数，BIN
    LOST_A_VLT_TMS_ID,       // 6字节A相失压总次数,总累计时间
    LOST_A_VLT_CHANGE_TMS_ID,// 4字节,A相失压变化次数变更标志,D0:1上1次失压发生D1:1上1次失压恢复...
    LAST_A_LOST_VLT_RECORD_1,// 35字节,(上1次)A相失压记录
    LAST_A_LOST_VLT_RECORD_2,// 35字节,(上2次)A相失压记录

    LAST_A_LOST_VLT_RECORD_3,// 35字节,(上3次)A相失压记录
    LAST_A_LOST_VLT_RECORD_4,// 35字节,(上4次)A相失压记录
    LAST_A_LOST_VLT_RECORD_5,// 35字节,(上5次)A相失压记录
    LAST_A_LOST_VLT_RECORD_6,// 35字节,(上6次)A相失压记录
    LAST_A_LOST_VLT_RECORD_7,// 35字节,(上7次)A相失压记录

    LAST_A_LOST_VLT_RECORD_8,// 35字节,(上8次)A相失压记录
    LAST_A_LOST_VLT_RECORD_9,// 35字节,(上9次)A相失压记录
    LAST_A_LOST_VLT_RECORD_10,// 35字节,(上10次)A相失压记录
    LOST_B_VLT_TMS_ID,       // 6字节B相失压总次数,总累计时间
    LOST_B_VLT_CHANGE_TMS_ID,// 4字节,B相失压变化次数变更标志,D0:1上1次失压发生D1:1上1次失压恢复...

    LAST_B_LOST_VLT_RECORD_1,// 35字节,(上1次)B相失压记录
    LAST_B_LOST_VLT_RECORD_2,// 35字节,(上2次)B相失压记录
    LAST_B_LOST_VLT_RECORD_3,// 35字节,(上3次)B相失压记录
    LAST_B_LOST_VLT_RECORD_4,// 35字节,(上4次)B相失压记录
    LAST_B_LOST_VLT_RECORD_5,// 35字节,(上5次)B相失压记录

    LAST_B_LOST_VLT_RECORD_6,// 35字节,(上6次)B相失压记录
    LAST_B_LOST_VLT_RECORD_7,// 35字节,(上7次)B相失压记录
    LAST_B_LOST_VLT_RECORD_8,// 35字节,(上8次)B相失压记录
    LAST_B_LOST_VLT_RECORD_9,// 35字节,(上9次)B相失压记录
    LAST_B_LOST_VLT_RECORD_10,// 35字节,(上10次)B相失压记录

    LOST_C_VLT_TMS_ID,       // 6字节C相失压总次数,总累计时间
    LOST_C_VLT_CHANGE_TMS_ID,// 4字节,C相失压变化次数变更标志,D0:1上1次失压发生D1:1上1次失压恢复...
    LAST_C_LOST_VLT_RECORD_1,// 35字节,(上1次)C相失压记录
    LAST_C_LOST_VLT_RECORD_2,// 35字节,(上2次)C相失压记录
    LAST_C_LOST_VLT_RECORD_3,// 35字节,(上3次)C相失压记录

    LAST_C_LOST_VLT_RECORD_4,// 35字节,(上4次)C相失压记录
    LAST_C_LOST_VLT_RECORD_5,// 35字节,(上5次)C相失压记录
    LAST_C_LOST_VLT_RECORD_6,// 35字节,(上6次)C相失压记录
    LAST_C_LOST_VLT_RECORD_7,// 35字节,(上7次)C相失压记录
    LAST_C_LOST_VLT_RECORD_8,// 35字节,(上8次)C相失压记录

    LAST_C_LOST_VLT_RECORD_9,// 35字节,(上9次)C相失压记录
    LAST_C_LOST_VLT_RECORD_10,// 35字节,(上10次)C相失压记录
    BREAK_A_VLT_CHANGE_TMS_ID,// 4字节,A相断相变化次数变更标志,D0:1上1次断相发生D1:1上1次断相恢复...
    LAST_A_BREAK_VLT_RECORD_1,// 35字节,（上1次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_2,// 35字节,（上2次）A相断相记录

    LAST_A_BREAK_VLT_RECORD_3,// 35字节,（上3次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_4,// 35字节,（上4次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_5,// 35字节,（上5次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_6,// 35字节,（上6次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_7,// 35字节,（上7次）A相断相记录

    LAST_A_BREAK_VLT_RECORD_8,// 35字节,（上8次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_9,// 35字节,（上9次）A相断相记录
    LAST_A_BREAK_VLT_RECORD_10,// 35字节,（上10次）A相断相记录
    BREAK_B_VLT_CHANGE_TMS_ID,// 4字节,B相断相变化次数变更标志,D0:1上1次断相发生D1:1上1次断相恢复...
    LAST_B_BREAK_VLT_RECORD_1,// 35字节,（上1次）B相断相记录

    LAST_B_BREAK_VLT_RECORD_2,// 35字节,（上2次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_3,// 35字节,（上3次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_4,// 35字节,（上4次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_5,// 35字节,（上5次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_6,// 35字节,（上6次）B相断相记录

    LAST_B_BREAK_VLT_RECORD_7,// 35字节,（上7次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_8,// 35字节,（上8次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_9,// 35字节,（上9次）B相断相记录
    LAST_B_BREAK_VLT_RECORD_10,// 35字节,（上10次）B相断相记录
    BREAK_C_VLT_CHANGE_TMS_ID,// 4字节,C相断相变化次数变更标志,D0:1上1次断相发生D1:1上1次断相恢复...

    LAST_C_BREAK_VLT_RECORD_1,// 35字节,（上1次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_2,// 35字节,（上2次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_3,// 35字节,（上3次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_4,// 35字节,（上4次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_5,// 35字节,（上5次）C相断相记录

    LAST_C_BREAK_VLT_RECORD_6,// 35字节,（上6次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_7,// 35字节,（上7次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_8,// 35字节,（上8次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_9,// 35字节,（上9次）C相断相记录
    LAST_C_BREAK_VLT_RECORD_10,// 35字节,（上10次）C相断相记录

    VLT_REVERSE_TMS_ID,      // 6字节，电压逆相序总次数，总累计时间
    VLT_REVERSE_CHANGE_TMS_ID,// 4字节,电压逆相序变化次数变更标志,D0:1上1次逆相序发生D1:1上1次逆相序恢复...
    LAST_VLT_REVERSE_RECORD_1,// 20字节,（上1次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_2,// 20字节,（上2次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_3,// 20字节,（上3次）电压逆相序记录

    LAST_VLT_REVERSE_RECORD_4,// 20字节,（上4次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_5,// 20字节,（上5次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_6,// 20字节,（上6次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_7,// 20字节,（上7次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_8,// 20字节,（上8次）电压逆相序记录

    LAST_VLT_REVERSE_RECORD_9,// 20字节,（上9次）电压逆相序记录
    LAST_VLT_REVERSE_RECORD_10,// 20字节,（上10次）电压逆相序记录
    CUR_REVERSE_TMS_ID,      // 6字节，电流逆相序总次数，总累计时间
    CUR_REVERSE_CHANGE_TMS_ID,// 4字节,电流逆相序变化次数变更标志,D0:1上1次逆相序发生D1:1上1次逆相序恢复...
    LAST_CUR_REVERSE_RECORD_1,// 20字节,（上1次）电流逆相序记录

    LAST_CUR_REVERSE_RECORD_2,// 20字节,（上2次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_3,// 20字节,（上3次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_4,// 20字节,（上4次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_5,// 20字节,（上5次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_6,// 20字节,（上6次）电流逆相序记录

    LAST_CUR_REVERSE_RECORD_7,// 20字节,（上7次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_8,// 20字节,（上8次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_9,// 20字节,（上9次）电流逆相序记录
    LAST_CUR_REVERSE_RECORD_10,// 20字节,（上10次）电流逆相序记录
    VLT_IMBALANCE_TMS_ID,    // 6字节,电压不平衡总次数,总累计时间

    VLT_IMBALANCE_CHANGE_TMS_ID,// 4字节,电压不平衡变化次数变更标志,D0:1上1次不平衡发生D1:1上1次不平衡恢复...
    LAST_VLT_IMBALANCE_RECORD_1,// 15字节,（上1次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_2,// 15字节,（上2次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_3,// 15字节,（上3次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_4,// 15字节,（上4次）电压不平衡记录

    LAST_VLT_IMBALANCE_RECORD_5,// 15字节,（上5次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_6,// 15字节,（上6次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_7,// 15字节,（上7次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_8,// 15字节,（上8次）电压不平衡记录
    LAST_VLT_IMBALANCE_RECORD_9,// 15字节,（上9次）电压不平衡记录

    LAST_VLT_IMBALANCE_RECORD_10,// 15字节,（上10次）电压不平衡记录
    CUR_IMBALANCE_TMS_ID,    // 6字节，电流不平衡总次数，总累计时间
    CUR_IMBALANCE_CHANGE_TMS_ID,// 4字节,电流不平衡变化次数变更标志,D0:1上1次不平衡发生D1:1上1次不平衡恢复...
    LAST_CUR_IMBALANCE_RECORD_1,// 14字节,（上1次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_2,// 14字节,（上2次）电流不平衡记录

    LAST_CUR_IMBALANCE_RECORD_3,// 14字节,（上3次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_4,// 14字节,（上4次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_5,// 14字节,（上5次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_6,// 14字节,（上6次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_7,// 14字节,（上7次）电流不平衡记录

    LAST_CUR_IMBALANCE_RECORD_8,// 14字节,（上8次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_9,// 14字节,（上9次）电流不平衡记录
    LAST_CUR_IMBALANCE_RECORD_10,// 14字节,（上10次）电流不平衡记录
    A_VLT_OVER_TMS_ID,       // 6字节,A相过压总次数,总累计时间
    A_VLT_OVER_CHANGE_TMS_ID,// 4字节,A相过压变化次数变更标志,D0:1上1次发生D1:1上1次恢复...

    LAST_A_VLT_OVER_RECORD_1,// 35字节,（上1次）A相过压记录
    LAST_A_VLT_OVER_RECORD_2,// 35字节,（上2次）A相过压记录
    LAST_A_VLT_OVER_RECORD_3,// 35字节,（上3次）A相过压记录
    LAST_A_VLT_OVER_RECORD_4,// 35字节,（上4次）A相过压记录
    LAST_A_VLT_OVER_RECORD_5,// 35字节,（上5次）A相过压记录

    LAST_A_VLT_OVER_RECORD_6,// 35字节,（上6次）A相过压记录
    LAST_A_VLT_OVER_RECORD_7,// 35字节,（上7次）A相过压记录
    LAST_A_VLT_OVER_RECORD_8,// 35字节,（上8次）A相过压记录
    LAST_A_VLT_OVER_RECORD_9,// 35字节,（上9次）A相过压记录
    LAST_A_VLT_OVER_RECORD_10,// 35字节,（上10次）A相过压记录

    B_VLT_OVER_TMS_ID,       // 6字节,B相过压总次数,总累计时间
    B_VLT_OVER_CHANGE_TMS_ID,// 4字节,B相过压变化次数变更标志,D0:1上1次发生D1:1上1次恢复...
    LAST_B_VLT_OVER_RECORD_1,// 35字节,（上1次）B相过压记录
    LAST_B_VLT_OVER_RECORD_2,// 35字节,（上2次）B相过压记录
    LAST_B_VLT_OVER_RECORD_3,// 35字节,（上3次）B相过压记录

    LAST_B_VLT_OVER_RECORD_4,// 35字节,（上4次）B相过压记录
    LAST_B_VLT_OVER_RECORD_5,// 35字节,（上5次）B相过压记录
    LAST_B_VLT_OVER_RECORD_6,// 35字节,（上6次）B相过压记录
    LAST_B_VLT_OVER_RECORD_7,// 35字节,（上7次）B相过压记录
    LAST_B_VLT_OVER_RECORD_8,// 35字节,（上8次）B相过压记录

    LAST_B_VLT_OVER_RECORD_9,// 35字节,（上9次）B相过压记录
    LAST_B_VLT_OVER_RECORD_10,// 35字节,（上10次）B相过压记录
    C_VLT_OVER_TMS_ID,       // 6字节,C相过压总次数,总累计时间
    C_VLT_OVER_CHANGE_TMS_ID,// 4字节,C相过压变化次数变更标志,D0:1上1次发生D1:1上1次恢复...
    LAST_C_VLT_OVER_RECORD_1,// 35字节,（上1次）C相过压记录

    LAST_C_VLT_OVER_RECORD_2,// 35字节,（上2次）C相过压记录
    LAST_C_VLT_OVER_RECORD_3,// 35字节,（上3次）C相过压记录
    LAST_C_VLT_OVER_RECORD_4,// 35字节,（上4次）C相过压记录
    LAST_C_VLT_OVER_RECORD_5,// 35字节,（上5次）C相过压记录
    LAST_C_VLT_OVER_RECORD_6,// 35字节,（上6次）C相过压记录

    LAST_C_VLT_OVER_RECORD_7,// 35字节,（上7次）C相过压记录
    LAST_C_VLT_OVER_RECORD_8,// 35字节,（上8次）C相过压记录
    LAST_C_VLT_OVER_RECORD_9,// 35字节,（上9次）C相过压记录
    LAST_C_VLT_OVER_RECORD_10,// 35字节,（上10次）C相过压记录
    MET_CAP_OPEN_CNT,        // 3个字节,BIN电能表开盖打开次数 

    MET_CAP_OPEN_CHANGE_TMS_ID,// 4字节, 电能表开盖变化次数变更标志,D0:1上1次发生D1:1上1次恢复...
    LAST_MET_CAP_RECORD_1,   // 60字节,（上1次）开表盖记录
    LAST_MET_CAP_RECORD_2,   // 60字节,（上2次）开表盖记录
    LAST_MET_CAP_RECORD_3,   // 60字节,（上3次）开表盖记录
    LAST_MET_CAP_RECORD_4,   // 60字节,（上4次）开表盖记录

    LAST_MET_CAP_RECORD_5,   // 60字节,（上5次）开表盖记录
    LAST_MET_CAP_RECORD_6,   // 60字节,（上6次）开表盖记录
    LAST_MET_CAP_RECORD_7,   // 60字节,（上7次）开表盖记录
    LAST_MET_CAP_RECORD_8,   // 60字节,（上8次）开表盖记录
    LAST_MET_CAP_RECORD_9,   // 60字节,（上9次）开表盖记录

    LAST_MET_CAP_RECORD_10,  // 60字节,（上10次）开表盖记录
    MET_COVER_OPEN_CNT,      // 3个字节,BIN电能表尾盖打开次数 
    MET_COVER_OPEN_CHANGE_TMS_ID,// 4字节, 电能表尾盖变化次数变更标志,D0:1上1次发生D1:1上1次恢复...
    LAST_MET_COVER_RECORD_1, // 60字节,（上1次）开端钮盖记录
    LAST_MET_COVER_RECORD_2, // 60字节,（上2次）开端钮盖记录

    LAST_MET_COVER_RECORD_3, // 60字节,（上3次）开端钮盖记录
    LAST_MET_COVER_RECORD_4, // 60字节,（上4次）开端钮盖记录
    LAST_MET_COVER_RECORD_5, // 60字节,（上5次）开端钮盖记录
    LAST_MET_COVER_RECORD_6, // 60字节,（上6次）开端钮盖记录
    LAST_MET_COVER_RECORD_7, // 60字节,（上7次）开端钮盖记录

    LAST_MET_COVER_RECORD_8, // 60字节,（上8次）开端钮盖记录
    LAST_MET_COVER_RECORD_9, // 60字节,（上9次）开端钮盖记录
    LAST_MET_COVER_RECORD_10,// 60字节,（上10次）开端钮盖记录
    MAGNETIC_FIELD_CHANGE_CNT,// 3个字节,BIN磁场变化次数 
    MAGNETIC_FIELD_CHANGE_TMS_ID,// 4字节, 磁场变化次数变更标志,D0:1上1次发生D1:1上1次恢复...

    LAST_MAGNETIC_FIELD_RECORD_1,// 18字节,(上1次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_2,// 18字节,(上2次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_3,// 18字节,(上3次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_4,// 18字节,(上4次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_5,// 18字节,(上5次)磁场变化记录

    LAST_MAGNETIC_FIELD_RECORD_6,// 18字节,(上6次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_7,// 18字节,(上7次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_8,// 18字节,(上8次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_9,// 18字节,(上9次)磁场变化记录
    LAST_MAGNETIC_FIELD_RECORD_10,// 18字节,(上10次)磁场变化记录

    LAST_MET_ELEC,           // 3字节,bin,掉电总次数
    TMN_CUR_DATE_TIME_ID,    // 6字节, 终端抄电表时钟时候当前终端日期、时间 BCD码(日期低一字节；星期占第二字节的高三位，月份占后五位；年(低二位）占最高字节
    PLC_RELAY_DEGREE_ID,     // 1字节, BIN码，中继路由级数
    PLC_COMM_PHASE,          // 1字节, BIN码，载波通信相位
    PlC_SIGNAL_QUALITY_ID,   // 1字节, BIN码，载波抄读通信相位

    PLC_LATELY_AMR_FLAG_ID,  // 1字节, BIN码，最近一次抄表成功/失败标志(1:成功 / 0:失败)
    PLC_LATELY_AMR_SUCCEED_TIME_ID,// 6字节, BCD码，最近一次抄表成功时间(附录A.1)
    PLC_LATELY_AMR_FAILURE_TIME_ID,// 6字节, BCD码，最近一次抄表失败时间(附录A.1)
    PlC_LATELY_AMR_FAILURE_ACC_ID,// 1字节, BIN码，最近一次抄表失败累计次数
    PlC_AMR_CUR_DATA_TYPE_ID,// 1字节, BIN码，当前任务类型(RT_DATA/POINT_DATA_DAY/POINT_DATA_MON)

    PlC_AMR_CUR_RT_TASK_INDEX_ID,// 1字节, BIN码，当前实时任务索引
    PlC_AMR_CUR_DAY_TASK_INDEX_ID,// 1字节, BIN码，当前日任务索引
    PlC_AMR_CUR_MON_TASK_INDEX_ID,// 1字节, BIN码，当前月任务索引
    PlC_AMR_CUR_EVENT_TASK_INDEX_ID,// 1字节, BIN码，当前事件索引
    TMN_AD_MET_TIME_SNL,     // 6字节, 秒分时日月年,终端对电表的单址对时时的时间

    POINT_BC_NUM_ID ,        // 1字节, 485补抄标志， 1字节，最大62，上电、过日置0
    POINT_BC_TIME_ID,        // 6 字节,表回的定时冻结时间就是time_struct结构
    RT_ER_READ_TIME_LAST,    // 6字节, BCD码, 上一次终端抄表时间给统计用：秒分时日月年
    CB_FAIL_TIME_AND_CNT_ID, // 4字节,格式:低1字节次数,高3字节日月年
    DELETE_METER_ID,         // 1字节,0xaa:删除，其他无效

    PLC_DAY_AMR_FLAG_ID,     // 1字节, BIN码，日抄表成功/失败标志(1:成功 / 0:失败)
    PlC_AMR_CUR_PATCH_DAY_TASK_INDEX_ID,// 1字节, BIN码，当前补抄日任务索引
    EVENT_METERHALT_CNT,     // 3字节, 终端停电掉电/上电记录发生次数
    EVENT_METERHALT_INFO,    // 12*10字节, 上十条终端停电掉电/上电记录
                                // #1: 6字节,停电时刻（YYMMDDHHMM）
                                // #2: 6字节,上电时刻（YYMMDDHHMM）
    RT_ER_READ_TAG_PLC,      // 1字节, 载波当前电量终端抄表标记

    CB_DAY_ID_PLC,           // 2字节, BCD码, 载波抄表日
    MET_CONST_AP_ID_PLC,     // 2字节, 载波有功电表常数
    MET_CONST_RP_ID_PLC,     // 2字节, 载波无功电表常数
    FIRST_DAY_PD_TIME_TARIFF_PLC,// 3*8字节, 载波, #1: 第一日时段表第1时段起始时间及费率号 3字节, BCD码(低位在前，高位在后（费率号、分、时）费率号：1：尖、2：峰、3：平、4：谷),
                                // 依次 2-8时段
    E_EVENT_COUNT_00_ID,     // 4字节，BIN, 负荷开关误动或拒动

    E_EVENT_COUNT_01_ID,     // 4字节，BIN, ESAM错误
    E_EVENT_COUNT_02_ID,     // 4字节，BIN, 内卡初始化错误
    E_EVENT_COUNT_03_ID,     // 4字节，BIN, 时钟电池电压低
    E_EVENT_COUNT_04_ID,     // 4字节，BIN, 内部程序错误
    E_EVENT_COUNT_05_ID,     // 4字节，BIN, 存储器故障或损坏

    E_EVENT_COUNT_06_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_07_ID,     // 4字节，BIN, 时钟故障
    E_EVENT_COUNT_08_ID,     // 4字节，BIN, 停电抄表电池欠压
    E_EVENT_COUNT_09_ID,     // 4字节，BIN, 透支状态
    E_EVENT_COUNT_10_ID,     // 4字节，BIN, 开表盖

    E_EVENT_COUNT_11_ID,     // 4字节，BIN, 开端钮盖
    E_EVENT_COUNT_12_ID,     // 4字节，BIN, 恒定磁场干扰
    E_EVENT_COUNT_13_ID,     // 4字节，BIN, 电源异常
    E_EVENT_COUNT_14_ID,     // 4字节，BIN, 跳闸成功
    E_EVENT_COUNT_15_ID,     // 4字节，BIN, 合闸成功

    E_EVENT_COUNT_16_ID,     // 4字节，BIN, A相失压
    E_EVENT_COUNT_17_ID,     // 4字节，BIN, A相欠压
    E_EVENT_COUNT_18_ID,     // 4字节，BIN, A相过压
    E_EVENT_COUNT_19_ID,     // 4字节，BIN, A相失流
    E_EVENT_COUNT_20_ID,     // 4字节，BIN, A相过流

    E_EVENT_COUNT_21_ID,     // 4字节，BIN, A相过载
    E_EVENT_COUNT_22_ID,     // 4字节，BIN, A相功率反向
    E_EVENT_COUNT_23_ID,     // 4字节，BIN, A相断相
    E_EVENT_COUNT_24_ID,     // 4字节，BIN, A相断流
    E_EVENT_COUNT_25_ID,     // 4字节，BIN, 保留

    E_EVENT_COUNT_26_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_27_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_28_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_29_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_30_ID,     // 4字节，BIN, 保留

    E_EVENT_COUNT_31_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_32_ID,     // 4字节，BIN, B相失压
    E_EVENT_COUNT_33_ID,     // 4字节，BIN, B相欠压
    E_EVENT_COUNT_34_ID,     // 4字节，BIN, B相过压
    E_EVENT_COUNT_35_ID,     // 4字节，BIN, B相失流

    E_EVENT_COUNT_36_ID,     // 4字节，BIN, B相过流
    E_EVENT_COUNT_37_ID,     // 4字节，BIN, B相过载
    E_EVENT_COUNT_38_ID,     // 4字节，BIN, B相功率反向
    E_EVENT_COUNT_39_ID,     // 4字节，BIN, B相断相
    E_EVENT_COUNT_40_ID,     // 4字节，BIN, B相断流

    E_EVENT_COUNT_41_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_42_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_43_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_44_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_45_ID,     // 4字节，BIN, 保留

    E_EVENT_COUNT_46_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_47_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_48_ID,     // 4字节，BIN, C相失压
    E_EVENT_COUNT_49_ID,     // 4字节，BIN, C相欠压
    E_EVENT_COUNT_50_ID,     // 4字节，BIN, C相过压

    E_EVENT_COUNT_51_ID,     // 4字节，BIN, C相失流
    E_EVENT_COUNT_52_ID,     // 4字节，BIN, C相过流
    E_EVENT_COUNT_53_ID,     // 4字节，BIN, C相过载
    E_EVENT_COUNT_54_ID,     // 4字节，BIN, C相功率反向
    E_EVENT_COUNT_55_ID,     // 4字节，BIN, C相断相

    E_EVENT_COUNT_56_ID,     // 4字节，BIN, C相断流
    E_EVENT_COUNT_57_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_58_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_59_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_60_ID,     // 4字节，BIN, 保留

    E_EVENT_COUNT_61_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_62_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_63_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_64_ID,     // 4字节，BIN, 电压逆相序
    E_EVENT_COUNT_65_ID,     // 4字节，BIN, 电流逆相序

    E_EVENT_COUNT_66_ID,     // 4字节，BIN, 电压不平衡
    E_EVENT_COUNT_67_ID,     // 4字节，BIN, 电流不平衡
    E_EVENT_COUNT_68_ID,     // 4字节，BIN, 辅助电源失电
    E_EVENT_COUNT_69_ID,     // 4字节，BIN, 掉电
    E_EVENT_COUNT_70_ID,     // 4字节，BIN, 需量超限

    E_EVENT_COUNT_71_ID,     // 4字节，BIN, 总功率因数超下限
    E_EVENT_COUNT_72_ID,     // 4字节，BIN, 电流严重不平衡
    E_EVENT_COUNT_73_ID,     // 4字节，BIN, 潮流反向
    E_EVENT_COUNT_74_ID,     // 4字节，BIN, 全失压
    E_EVENT_COUNT_75_ID,     // 4字节，BIN, 保留

    E_EVENT_COUNT_76_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_77_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_78_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_79_ID,     // 4字节，BIN, 保留
    E_EVENT_COUNT_80_ID,     // 4字节，BIN, 编程

    E_EVENT_COUNT_81_ID,     // 4字节，BIN, 电表清零
    E_EVENT_COUNT_82_ID,     // 4字节，BIN, 需量清零
    E_EVENT_COUNT_83_ID,     // 4字节，BIN, 事件清零
    E_EVENT_COUNT_84_ID,     // 4字节，BIN, 校时
    E_EVENT_COUNT_85_ID,     // 4字节，BIN, 时段表编程

    E_EVENT_COUNT_86_ID,     // 4字节，BIN, 时区表编程
    E_EVENT_COUNT_87_ID,     // 4字节，BIN, 周休日编程
    E_EVENT_COUNT_88_ID,     // 4字节，BIN, 节假日编程
    E_EVENT_COUNT_89_ID,     // 4字节，BIN, 有功组合方式编程
    E_EVENT_COUNT_90_ID,     // 4字节，BIN, 无功组合方式1编程

    E_EVENT_COUNT_91_ID,     // 4字节，BIN, 无功组合方式2编程
    E_EVENT_COUNT_92_ID,     // 4字节，BIN, 结算日编程
    E_EVENT_COUNT_93_ID,     // 4字节，BIN, 费率参数表编程
    E_EVENT_COUNT_94_ID,     // 4字节，BIN, 阶梯表编程
    E_EVENT_COUNT_95_ID,     // 4字节，BIN, 密钥更新

    EVENT_METERHALT_INFO_1,  // 12字节, 上十条终端停电掉电/上电记录
                                // #1: 6字节,停电时刻（YYMMDDhhmm）
                                // #2: 6字节,上电时刻（YYMMDDhhmm）
    EVENT_METERHALT_INFO_2,  // 12字节, 上2条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_3,  // 12字节, 上3条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_4,  // 12字节, 上4条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_5,  // 12字节, 上5条终端停电掉电/上电记录

    EVENT_METERHALT_INFO_6,  // 12字节, 上6条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_7,  // 12字节, 上7条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_8,  // 12字节, 上8条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_9,  // 12字节, 上9条终端停电掉电/上电记录
    EVENT_METERHALT_INFO_10, // 12字节, 上10条终端停电掉电/上电记录

    LastSucc_3105EventRecNO, // 4字节,记录上次电能表时钟超差事件发生的序号
    LastSucc_310BEventRecNO, // 4字节,记录上次示度下降事件发生的事件序号
    LastSucc_310CEventRecNO, // 4字节,记录上次电能量超差发生的事件序号
    LastSucc_310DEventRecNO, // 4字节,记录上次电能量飞走事件发生的事件序号
    LastSucc_310EEventRecNO, // 4字节,记录上次电能表停走事件发生的事件序号

    LastSucc_310FEventRecNO, // 4字节,记录上次抄表失败事件发生的事件序号
    OOP_EVENT_CNT_3009,      // 4字节, 电能表正向有功需量超限事件记录序号
    OOP_EVENT_CNT_300A,      // 4字节, 电能表反向有功需量超限事件记录序号
    OOP_EVENT_CNT_300C,      // 4字节, 电能表功率因数超下限事件记录序号
    OOP_EVENT_CNT_300D,      // 4字节, 电能表全失压事件记录序号

    OOP_EVENT_CNT_300E,      // 4字节, 电能表辅助电源掉电事件记录序号
    OOP_EVENT_CNT_300F,      // 4字节, 电能表电压逆相序事件记录序号
    OOP_EVENT_CNT_3010,      // 4字节, 电能表电流逆相序事件记录序号
    OOP_EVENT_CNT_3011,      // 4字节, 电能表掉电事件记录序号
    OOP_EVENT_CNT_3012,      // 4字节, 电能表编程事件记录序号

    OOP_EVENT_CNT_3013,      // 4字节, 电能表清零事件记录序号
    OOP_EVENT_CNT_3014,      // 4字节, 电能表需量清零事件记录序号
    OOP_EVENT_CNT_3015,      // 4字节, 电能表事件清零事件记录序号
    OOP_EVENT_CNT_3016,      // 4字节, 电能表校时事件记录序号
    OOP_EVENT_CNT_3017,      // 4字节, 电能表时段表编程事件记录序号

    OOP_EVENT_CNT_3018,      // 4字节, 电能表时区表编程事件记录序号
    OOP_EVENT_CNT_3019,      // 4字节, 电能表周休日编程事件记录序号
    OOP_EVENT_CNT_301A,      // 4字节, 电能表结算日编程事件记录序号
    OOP_EVENT_CNT_301B,      // 4字节, 电能表开盖事件记录序号
    OOP_EVENT_CNT_301C,      // 4字节, 电能表开端钮盒事件记录序号

    OOP_EVENT_CNT_301D,      // 4字节, 电能表电压不平衡事件记录序号
    OOP_EVENT_CNT_301E,      // 4字节, 电能表电流不平衡事件记录序号
    OOP_EVENT_CNT_301F,      // 4字节, 电能表跳闸事件记录序号
    OOP_EVENT_CNT_3020,      // 4字节, 电能表合闸事件记录序号
    OOP_EVENT_CNT_3021,      // 4字节, 电能表节假日编程事件记录序号

    OOP_EVENT_CNT_3022,      // 4字节, 电能表有功组合方式编程事件记录序号
    OOP_EVENT_CNT_3023,      // 4字节, 电能表无功组合方式编程事件记录序号
    OOP_EVENT_CNT_3024,      // 4字节, 电能表费率参数表编程事件记录序号
    OOP_EVENT_CNT_3025,      // 4字节, 电能表阶梯表编程事件记录序号
    OOP_EVENT_CNT_3026,      // 4字节, 电能表密钥更新事件记录序号

    OOP_EVENT_CNT_3027,      // 4字节, 电能表异常插卡事件记录序号
    OOP_EVENT_CNT_3028,      // 4字节, 电能表购电记录记录序号
    OOP_EVENT_CNT_3029,      // 4字节, 电能表退费记录记录序号
    OOP_EVENT_CNT_302A,      // 4字节, 电能表恒定磁场干扰事件记录序号
    OOP_EVENT_CNT_302B,      // 4字节, 电能表负荷开关误动作事件记录序号

    OOP_EVENT_CNT_302C,      // 4字节, 电能表电源异常事件记录序号
    OOP_EVENT_CNT_302D,      // 4字节, 电能表电流严重不平衡事件记录序号
    OOP_EVENT_CNT_302E,      // 4字节, 电能表时钟故障事件记录序号
    OOP_EVENT_CNT_302F,      // 4字节, 电能表计量芯片故障事件记录序号
    OOP_EVENT_CNT_3000_T,    // 4字节, 总 -电能表失压事件记录序号

    OOP_EVENT_CNT_3000_A,    // 4字节, A相-电能表失压事件记录序号
    OOP_EVENT_CNT_3000_B,    // 4字节, B相-电能表失压事件记录序号
    OOP_EVENT_CNT_3000_C,    // 4字节, C相-电能表失压事件记录序号
    OOP_EVENT_CNT_3001_T,    // 4字节, 总 -电能表欠压事件记录序号
    OOP_EVENT_CNT_3001_A,    // 4字节, A相-电能表欠压事件记录序号

    OOP_EVENT_CNT_3001_B,    // 4字节, B相-电能表欠压事件记录序号
    OOP_EVENT_CNT_3001_C,    // 4字节, C相-电能表欠压事件记录序号
    OOP_EVENT_CNT_3002_T,    // 4字节, 总 -电能表过压事件记录序号
    OOP_EVENT_CNT_3002_A,    // 4字节, A相-电能表过压事件记录序号
    OOP_EVENT_CNT_3002_B,    // 4字节, B相-电能表过压事件记录序号

    OOP_EVENT_CNT_3002_C,    // 4字节, C相-电能表过压事件记录序号
    OOP_EVENT_CNT_3003_T,    // 4字节, 总 -电能表断相事件记录序号
    OOP_EVENT_CNT_3003_A,    // 4字节, A相-电能表断相事件记录序号
    OOP_EVENT_CNT_3003_B,    // 4字节, B相-电能表断相事件记录序号
    OOP_EVENT_CNT_3003_C,    // 4字节, C相-电能表断相事件记录序号

    OOP_EVENT_CNT_3004_T,    // 4字节, 总 -电能表失流事件记录序号
    OOP_EVENT_CNT_3004_A,    // 4字节, A相-电能表失流事件记录序号
    OOP_EVENT_CNT_3004_B,    // 4字节, B相-电能表失流事件记录序号
    OOP_EVENT_CNT_3004_C,    // 4字节, C相-电能表失流事件记录序号
    OOP_EVENT_CNT_3005_T,    // 4字节, 总 -电能表过流事件记录序号

    OOP_EVENT_CNT_3005_A,    // 4字节, A相-电能表过流事件记录序号
    OOP_EVENT_CNT_3005_B,    // 4字节, B相-电能表过流事件记录序号
    OOP_EVENT_CNT_3005_C,    // 4字节, C相-电能表过流事件记录序号
    OOP_EVENT_CNT_3006_T,    // 4字节, 总 -电能表断流事件记录序号
    OOP_EVENT_CNT_3006_A,    // 4字节, A相-电能表断流事件记录序号

    OOP_EVENT_CNT_3006_B,    // 4字节, B相-电能表断流事件记录序号
    OOP_EVENT_CNT_3006_C,    // 4字节, C相-电能表断流事件记录序号
    OOP_EVENT_CNT_3007_T,    // 4字节, 总 -电能表潮流反向事件记录序号
    OOP_EVENT_CNT_3007_A,    // 4字节, A相-电能表潮流反向事件记录序号
    OOP_EVENT_CNT_3007_B,    // 4字节, B相-电能表潮流反向事件记录序号

    OOP_EVENT_CNT_3007_C,    // 4字节, C相-电能表潮流反向事件记录序号
    OOP_EVENT_CNT_3008_T,    // 4字节, 总 -电能表过载事件记录序号
    OOP_EVENT_CNT_3008_A,    // 4字节, A相-电能表过载事件记录序号
    OOP_EVENT_CNT_3008_B,    // 4字节, B相-电能表过载事件记录序号
    OOP_EVENT_CNT_3008_C,    // 4字节, C相-电能表过载事件记录序号

    OOP_EVENT_CNT_300B_T,    // 4字节, I象限电能表无功需量超限事件记录序号
    OOP_EVENT_CNT_300B_A,    // 4字节, II象限电能表无功需量超限事件记录序号
    OOP_EVENT_CNT_300B_B,    // 4字节, III象限电能表无功需量超限事件记录序号
    OOP_EVENT_CNT_300B_C,    // 4字节, IV象限电能表无功需量超限事件记录序号
    DATA_RT_COMPLEX_AREA_TEST,// 各地区增补的实时数据（RT_DATA）（此ID必须放在第一个，新增ID在后面增加）

DATA_RT_END_ID,
};

/******************--2计量点日定点数据项ID--*********************/
enum data_point_day
{
    TOTAL_SEVER_TACC_TIME_FIX=0,// 4字节，BIN（min），总断相时间累计值，电表开始投用以来的断相时间累加
    P_SEVER_TACC_TIME_FIX,   // 12字节，BIN（min），分相断相时间累计值，电表开始投用以来分相断相时间累加
    P_SEVER_AE_ER_FIX,       // 24字节，BIN X 10000 （kWh），分相断相电量，电表开始投用以来分相断相期间的电能量累计值
    RD_MT_CLOCK_FIX,         // 6字节，BCD，测量点日定点数据抄表时间(秒、分、时、日、月、年)
    MT_TIME_FIX,             // 6字节，BCD，电表当前时间

    BATTERY_WORK_TACC_FIX,   // 6字节，BCD（min），电池累计工作时间
    MT_STATU_BYTE_FIX,       // 1字节，BIN，电表故障状态字
    MT_COPY_INFO_FIX,        // 共7字节，标记及抄表时间(抄表状态抄表时间) 抄表状态：1字节，BIN 抄表时间：6字节，BCD，(秒、分、时、日、月、年)
    TOTAL_PF_FIX,            // 2字节，BIN X 100 %，总功率因数，总有功功率与总视在功率的比值
    TOTAL_LOST_VOLT_TACC_FIX,// 4字节，BIN（min），总失压累计时间，电表开始投用以来的失压累计时间

    P_LOST_CURR_AE_ER_FIX,   // 24字节，BIN X 10000 （kWh），分相失流电量，电表开始投用以来的分相(A,B,C相)失流期间的电能量累计值
    RT_POS_AE_Z_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    RT_POS_AE_1_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量（尖）
    RT_POS_AE_2_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量（峰）
    RT_POS_AE_3_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量（平）

    RT_POS_AE_4_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量（谷）
    RT_POS_AE_5_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量费率5
    RT_POS_AE_6_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量费率6
    RT_POS_AE_7_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量费率7
    RT_POS_AE_8_FIX,         // 8字节，BIN X 10000 （kWh），当前正向有功电量费率8

    RT_POS_AE_EXT_FIX,       // 48字节，BIN X 10000 （kWh），当前正向有功电量（9~14费率）
    RT_NEG_AE_Z_FIX,         // 8字节，BIN X 10000 （kWh），当前反向有功电量 （总）
    RT_NEG_AE_1_FIX,         // 8字节，BIN X 10000 （kWh），当前反向有功电量 （尖）
    RT_NEG_AE_2_FIX,         // 8字节，BIN X 10000 （kWh），当前反向有功电量 （峰）
    RT_NEG_AE_3_FIX,         // 8字节，BIN X 10000 （kWh），当前反向有功电量 （平）

    RT_NEG_AE_4_FIX,         // 8字节，BIN X 10000 （kWh），当前反向有功电量 （谷）
    RT_NEG_AE_5_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向有功电量费率5
    RT_NEG_AE_6_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向有功电量费率6
    RT_NEG_AE_7_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向有功电量费率7
    RT_NEG_AE_8_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向有功电量费率8

    RT_NEG_AE_EXT_FIX,       // 48字节，BIN X 10000 （kWh），-当前反向有功电量（9~14费率）
    RT_POS_RE_Z_FIX,         // 8字节，BIN X 10000 （kWh），当前正向无功电量 （总）
    RT_POS_RE_1_FIX,         // 8字节，BIN X 10000 （kWh），当前正向无功电量 （尖）
    RT_POS_RE_2_FIX,         // 8字节，BIN X 10000 （kWh），当前正向无功电量 （峰）
    RT_POS_RE_3_FIX,         // 8字节，BIN X 10000 （kWh），当前正向无功电量 （平）

    RT_POS_RE_4_FIX,         // 8字节，BIN X 10000 （kWh），当前正向无功电量 （谷）
    RT_POS_RE_5_FIX,         // 8字节，BIN X 10000 （kWh），-当前正向无功电量费率5
    RT_POS_RE_6_FIX,         // 8字节，BIN X 10000 （kWh），-当前正向无功电量费率6
    RT_POS_RE_7_FIX,         // 8字节，BIN X 10000 （kWh），-当前正向无功电量费率7
    RT_POS_RE_8_FIX,         // 8字节，BIN X 10000 （kWh），-当前正向无功电量费率8

    RT_POS_RE_EXT_FIX,       // 48字节，BIN X 10000 （kWh），-当前正向无功电量（9~14费率）
    RT_NEG_RE_Z_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量 （总）
    RT_NEG_RE_1_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量 （尖）
    RT_NEG_RE_2_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量 （峰）
    RT_NEG_RE_3_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量 （平）

    RT_NEG_RE_4_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量 （谷）
    RT_NEG_RE_5_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量费率5
    RT_NEG_RE_6_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量费率6
    RT_NEG_RE_7_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量费率7
    RT_NEG_RE_8_FIX,         // 8字节，BIN X 10000 （kWh），-当前反向无功电量费率8

    RT_NEG_RE_EXT_FIX,       // 48字节，BIN X 10000 （kWh），-当前反向无功电量（9~14费率）
    RT_FIRST_RE_Z_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量 （总）
    RT_FIRST_RE_1_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量 （尖）
    RT_FIRST_RE_2_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量 （峰）
    RT_FIRST_RE_3_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量 （平）

    RT_FIRST_RE_4_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量 （谷）
    RT_FIRST_RE_5_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量费率5
    RT_FIRST_RE_6_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量费率6
    RT_FIRST_RE_7_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量费率7
    RT_FIRST_RE_8_FIX,       // 8字节，BIN X 10000 （kWh），-当前第一象限无功电量费率8

    RT_FIRST_RE_EXT_FIX,     // 48字节，BIN X 10000 （kWh），-当前第一象限无功电量（9~14费率）
    RT_FOURTH_RE_Z_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量 （总）
    RT_FOURTH_RE_1_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量 （尖）
    RT_FOURTH_RE_2_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量 （峰）
    RT_FOURTH_RE_3_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量 （平）

    RT_FOURTH_RE_4_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量 （谷）
    RT_FOURTH_RE_5_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量费率5
    RT_FOURTH_RE_6_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量费率6
    RT_FOURTH_RE_7_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量费率7
    RT_FOURTH_RE_8_FIX,      // 8字节，BIN X 10000 （kWh），-当前第四象限无功电量费率8

    RT_FOURTH_RE_EXT_FIX,    // 48字节，BIN X 10000 （kWh），-当前第四象限无功电量（9~14费率）
    RT_SECOND_RE_Z_FIX,      // 8字节，BIN X 10000 （kWh），-当前第二象限无功电量 （总）
    RT_SECOND_RE_1_FIX,      // 8字节，BIN X 10000 （kWh），-当前第二象限无功电量 （尖）
    RT_SECOND_RE_2_FIX,      // 8字节，BIN X 10000 （kWh），-当前第二象限无功电量 （峰）
    RT_SECOND_RE_3_FIX,      // 8字节，BIN X 10000 （kWh），-当前第二象限无功电量 （平）

    RT_SECOND_RE_4_FIX,      // 8字节，BIN X 10000 （kWh），-当前第二象限无功电量 （谷）
    RT_SECOND_RE_5_FIX,      // 8字节，BIN X 10000 （kWh），当前第二象限无功电量费率5
    RT_SECOND_RE_6_FIX,      // 8字节，BIN X 10000 （kWh），当前第二象限无功电量费率6
    RT_SECOND_RE_7_FIX,      // 8字节，BIN X 10000 （kWh），当前第二象限无功电量费率7
    RT_SECOND_RE_8_FIX,      // 8字节，BIN X 10000 （kWh），当前第二象限无功电量费率8

    RT_SECOND_RE_EXT_FIX,    // 48字节，BIN X 10000 （kWh），当前第二象限无功电量（9~14费率）
    RT_THIRD_RE_Z_FIX,       // 8字节，BIN X 10000 （kWh），-当前第三象限无功电量 （总）
    RT_THIRD_RE_1_FIX,       // 8字节，BIN X 10000 （kWh），-当前第三象限无功电量 （尖）
    RT_THIRD_RE_2_FIX,       // 8字节，BIN X 10000 （kWh），-当前第三象限无功电量 （峰）
    RT_THIRD_RE_3_FIX,       // 8字节，BIN X 10000 （kWh），-当前第三象限无功电量 （平）

    RT_THIRD_RE_4_FIX,       // 8字节，BIN X 10000 （kWh），-当前第三象限无功电量 （谷）
    RT_THIRD_RE_5_FIX,       // 8字节，BIN X 10000 （kWh），当前第三象限无功电量费率5
    RT_THIRD_RE_6_FIX,       // 8字节，BIN X 10000 （kWh），当前第三象限无功电量费率6
    RT_THIRD_RE_7_FIX,       // 8字节，BIN X 10000 （kWh），当前第三象限无功电量费率7
    RT_THIRD_RE_8_FIX,       // 8字节，BIN X 10000 （kWh），当前第三象限无功电量费率8

    RT_THIRD_RE_EXT_FIX,     // 48字节，BIN X 10000 （kWh），当前第三象限无功电量（9~14费率）
    RT_POS_AE_A_ER_FIX,      // 8字节，BIN X 10000 （kWh），A相正向有功电量
    RT_POS_AE_B_ER_FIX,      // 8字节，BIN X 10000 （kWh），B相正向有功电量
    RT_POS_AE_C_ER_FIX,      // 8字节，BIN X 10000 （kWh），C相正向有功电量
    RT_NEG_AE_A_ER_FIX,      // 8字节，BIN X 10000 （kWh），A相反向有功电量

    RT_NEG_AE_B_ER_FIX,      // 8字节，BIN X 10000 （kWh），B相反向有功电量
    RT_NEG_AE_C_ER_FIX,      // 8字节，BIN X 10000 （kWh），C相反向有功电量
    RT_L_RE_A_ER_FIX,        // 8字节，BIN X 10000 （kWh），A相感性无功电量
    RT_L_RE_B_ER_FIX,        // 8字节，BIN X 10000 （kWh），B相感性无功电量
    RT_L_RE_C_ER_FIX,        // 8字节，BIN X 10000 （kWh），C相感性无功电量

    RT_C_RE_A_ER_FIX,        // 8字节，BIN X 10000 （kWh），A相容性无功电量
    RT_C_RE_B_ER_FIX,        // 8字节，BIN X 10000 （kWh），B相容性无功电量
    RT_C_RE_C_ER_FIX,        // 8字节，BIN X 10000 （kWh），C相容性无功电量
    Z_SEVER_START_TIME_FIX,  // 6字节，BCD，总最近一次断相起始时间，(秒、分、时、日、月、年)
    P_SEVER_START_TIME_FIX,  // 18字节，BCD，（A,B,C）分相最近断相起始时间，(秒、分、时、日、月、年)

    Z_SEVER_END_TIME_FIX,    // 6字节，BCD，总最近一次断相结束时间，(秒、分、时、日、月、年)
    P_SEVER_END_TIME_FIX,    // 18字节，BCD,（A,B,C）分相最近断相结束时间，(秒、分、时、日、月、年)
    TOTAL_SEVER_TACC_FIX,    // 2字节，BIN，总断相次数
    P_SEVER_TACC_FIX,        // 6字节，BIN，（A，B，C）分相断相次数
    ZAD_POS_MAX_REC_FIX,     // 10字节，正向总有功需量最大值记录(最大值/最值出现时间）
                                // 最大值：4字节，BIN X 10000 （kWh）出现时间：6字节，BCD，(秒、分、时、日、月、年)

    POS_AD_MAX_REC1_FIX,     // 10字节，费率段（尖）正向有功需量最大值记录(最大值最值出现时间),格式同上
    POS_AD_MAX_REC2_FIX,     // 10字节，费率段（峰）正向有功需量最大值记录(最大值最值出现时间),格式同上
    POS_AD_MAX_REC3_FIX,     // 10字节，费率段（平）正向有功需量最大值记录(最大值最值出现时间),格式同上
    POS_AD_MAX_REC4_FIX,     // 10字节，费率段（谷）正向有功需量最大值记录(最大值最值出现时间),格式同上
    POS_AD_MAX_REC5_FIX,     // 10字节，费率段5正向有功需量最大值记录(最大值最值出现时间）,格式同上

    POS_AD_MAX_REC6_FIX,     // 10字节，费率段6正向有功需量最大值记录(最大值最值出现时间）,格式同上
    POS_AD_MAX_REC7_FIX,     // 10字节，费率段7正向有功需量最大值记录(最大值最值出现时间）,格式同上
    POS_AD_MAX_REC8_FIX,     // 10字节，费率段8正向有功需量最大值记录(最大值最值出现时间）,格式同上
    EXT_POS_AD_MAX_REC_FIX,  // 60字节，费率段（9~14）正向有功需量最大值记录(最大值最值出现时间）,格式同上
    ZAD_NEG_MAX_REC_FIX,     // 10字节，反向总有功需量最大值记录(最大值最值出现时间）
                                // 最大值：4字节，BIN X 10000 （kWh）出现时间：6字节，BCD，(秒、分、时、日、月、年)

    NEG_AD_MAX_REC1_FIX,     // 10字节，费率段（尖）反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC2_FIX,     // 10字节，费率段（峰）反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC3_FIX,     // 10字节，费率段（平）反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC4_FIX,     // 10字节，费率段（谷）反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC5_FIX,     // 10字节，费率段5反向有功需量最大值记录(最大值最值出现时间）,格式同上

    NEG_AD_MAX_REC6_FIX,     // 10字节，费率段6反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC7_FIX,     // 10字节，费率段7反向有功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_AD_MAX_REC8_FIX,     // 10字节，费率段8反向有功需量最大值记录(最大值最值出现时间）,格式同上
    EXT_NEG_AD_MAX_REC_FIX,  // 60字节，费率段（9~14）反向有功需量最大值记录(最大值最值出现时间）,格式同上
    ZRD_POS_MAX_REC_FIX,     // 10字节，正向总无功需量最大值记录(最大值最值出现时间）,格式同上

    POS_RD_MAX_REC1_FIX,     // 10字节，费率段（尖）正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC2_FIX,     // 10字节，费率段（峰）正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC3_FIX,     // 10字节，费率段（平）正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC4_FIX,     // 10字节，费率段（谷）正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC5_FIX,     // 10字节，费率段5正向无功需量最大值记录(最大值最值出现时间）,格式同上

    POS_RD_MAX_REC6_FIX,     // 10字节，费率段6正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC7_FIX,     // 10字节，费率段7正向无功需量最大值记录(最大值最值出现时间）,格式同上
    POS_RD_MAX_REC8_FIX,     // 10字节，费率段8正向无功需量最大值记录(最大值最值出现时间）,格式同上
    EXT_POS_RD_MAX_REC_FIX,  // 60字节，费率段（9~14）正向无功需量最大值记录(最大值最值出现时间）,格式同上
    ZRD_NEG_MAX_REC_FIX,     // 10字节，反向总无功需量最大值记录(最大值最值出现时间）,格式同上

    NEG_RD_MAX_REC1_FIX,     // 10字节，费率段（尖）反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC2_FIX,     // 10字节，费率段（峰）反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC3_FIX,     // 10字节，费率段（平）反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC4_FIX,     // 10字节，费率段（谷）反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC5_FIX,     // 10字节，费率段5反向无功需量最大值记录(最大值最值出现时间）,格式同上

    NEG_RD_MAX_REC6_FIX,     // 10字节，费率段6反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC7_FIX,     // 10字节，费率段7反向无功需量最大值记录(最大值最值出现时间）,格式同上
    NEG_RD_MAX_REC8_FIX,     // 10字节，费率段8反向无功需量最大值记录(最大值最值出现时间）,格式同上
    EXT_NEG_RD_MAX_REC_FIX,  // 60字节，费率段（9~14）反向无功需量最大值记录(最大值最值出现时间）,格式同上
    ZAD_POS_A_MAX_REC_FIX,   // 10字节，A相正向总有功需量最大值记录(最大值/最值出现时间）
                                // 最大值：4字节，BIN X 10000 （kWh）出现时间：6字节，BCD，(秒、分、时、日、月、年)

    ZAD_POS_B_MAX_REC_FIX,   // 10字节，B相正向总有功需量最大值记录(最大值/最值出现时间）
                                // 最大值：4字节，BIN X 10000 （kWh）出现时间：6字节，BCD，(秒、分、时、日、月、年)
    ZAD_POS_C_MAX_REC_FIX,   // 10字节，C相正向总有功需量最大值记录(最大值/最值出现时间）
                                // 最大值：4字节，BIN X 10000 （kWh）出现时间：6字节，BCD，(秒、分、时、日、月、年)
    Z_SEVER_ACC,             // 4字节，BIN，总断相累计次数
    P_SEVER_ACC_A,           // 4字节，BIN，A相断相累计次数
    P_SEVER_ACC_B,           // 4字节，BIN，B相断相累计次数

    P_SEVER_ACC_C,           // 4字节，BIN，C相断相累计次数
    Z_SEVER_TACC,            // 4字节，BIN，总断相累计时间
    P_SEVER_TACC_A,          // 4字节，BIN，A相断相累计时间
    P_SEVER_TACC_B,          // 4字节，BIN，B相断相累计时间
    P_SEVER_TACC_C,          // 4字节，BIN，C相断相累计时间

    Z_SEVER_START_TIME,      // 4字节，BCD，总最近一次断相起始时间，(分、时、日、月)
    P_SEVER_START_TIME_A,    // 4字节，BCD，A相最近断相起始时间，(分、时、日、月) 
    P_SEVER_START_TIME_B,    // 4字节，BCD，B相最近断相起始时间，(分、时、日、月) 
    P_SEVER_START_TIME_C,    // 4字节，BCD，C相最近断相起始时间，(分、时、日、月) 
    Z_SEVER_END_TIME,        // 4字节，BCD，总最近一次断相结束时间，(分、时、日、月)

    P_SEVER_END_TIME_A,      // 4字节，BCD，A相最近断相结束时间，(分、时、日、月)
    P_SEVER_END_TIME_B,      // 4字节，BCD，B相最近断相结束时间，(分、时、日、月)
    P_SEVER_END_TIME_C,      // 4字节，BCD，C相最近断相结束时间，(分、时、日、月)
    SEVER_READ_TIME_DAY,     // 6字节，BCD，终端断相抄表时间，(秒、分、时、日、月、年)
    PF_DAY_SEC_TIME_FIX_A,   // 6字节，BIN，A相日功率因数区段累计时间

    PF_DAY_SEC_TIME_FIX_B,   // 6字节，BIN，B相日功率因数区段累计时间
    PF_DAY_SEC_TIME_FIX_C,   // 6字节，BIN，C相日功率因数区段累计时间
    PF_DAY_SEC_TIME_FIX,     // 6字节，BIN，日功率因数区段累计时间
    VIR_AE_FIX_DAY_Z,        // 8字节，BIN X 10000（KWH），日总加有功电能量（总）
    VIR_AE_FIX_DAY_1,        // 8字节，BIN X 10000（KWH），日总加有功电能量（尖）

    VIR_AE_FIX_DAY_2,        // 8字节，BIN X 10000（KWH），日总加有功电能量（峰）
    VIR_AE_FIX_DAY_3,        // 8字节，BIN X 10000（KWH），日总加有功电能量（平）
    VIR_AE_FIX_DAY_4,        // 8字节，BIN X 10000（KWH），日总加有功电能量（谷）
    VIR_AE_FIX_DAY_5,        // 8字节，BIN X 10000（KWH），日总加有功电能量（5费率）
    VIR_AE_FIX_DAY_6,        // 8字节，BIN X 10000（KWH），日总加有功电能量（6费率）

    VIR_AE_FIX_DAY_7,        // 8字节，BIN X 10000（KWH），日总加有功电能量（7费率）
    VIR_AE_FIX_DAY_8,        // 8字节，BIN X 10000（KWH），日总加有功电能量（8费率）
    VIR_AE_FIX_DAY_9_14,     // 48字节，BIN X 10000（KWH），日总加有功电能量（9~14费率）
    VIR_RE_FIX_DAY_Z,        // 8字节，BIN X 10000（KWH），日总加无功电能量（总）
    VIR_RE_FIX_DAY_1,        // 8字节，BIN X 10000（KWH），日总加无功电能量（尖）

    VIR_RE_FIX_DAY_2,        // 8字节，BIN X 10000（KWH），日总加无功电能量（峰）
    VIR_RE_FIX_DAY_3,        // 8字节，BIN X 10000（KWH），日总加无功电能量（平）
    VIR_RE_FIX_DAY_4,        // 8字节，BIN X 10000（KWH），日总加无功电能量（谷）
    VIR_RE_FIX_DAY_5,        // 8字节，BIN X 10000（KWH），日总加无功电能量（5费率）
    VIR_RE_FIX_DAY_6,        // 8字节，BIN X 10000（KWH），日总加无功电能量（6费率）

    VIR_RE_FIX_DAY_7,        // 8字节，BIN X 10000（KWH），日总加无功电能量（7费率）
    VIR_RE_FIX_DAY_8,        // 8字节，BIN X 10000（KWH），日总加无功电能量（8费率）
    VIR_RE_FIX_DAY_9_14,     // 48字节，BIN X 10000（KWH），日总加无功电能量（9~14费率）
    VOLTHR_PHASE_A_UO_TACC,  // 36字节，BIN，A相2~19次电压谐波含有率超标日累计时间
    VOLTHR_PHASE_B_UO_TACC,  // 36字节，BIN，B相2~19次电压谐波含有率超标日累计时间

    VOLTHR_PHASE_C_UO_TACC,  // 36字节，BIN，C相2~19次电压谐波含有率超标日累计时间
    VOLTABR_PHASE_A_UO_TACC, // 2字节，BIN，A相总畸变电压含有率越限日累计时间
    VOLTABR_PHASE_B_UO_TACC, // 2字节，BIN，B相总畸变电压含有率越限日累计时间
    VOLTABR_PHASE_C_UO_TACC, // 2字节，BIN，C相总畸变电压含有率越限日累计时间
    CURHR_PHASE_A_UO_TACC,   // 36字节，BIN，A相2~19次电流谐波有效值超标日累计时间

    CURHR_PHASE_B_UO_TACC,   // 36字节，BIN，B相2~19次电流谐波有效值超标日累计时间
    CURHR_PHASE_C_UO_TACC,   // 36字节，BIN，C相2~19次电流谐波有效值超标日累计时间
    CURABR_PHASE_A_UO_TACC,  // 2字节，BIN，A相总畸变电流有效值越限日累计时间
    CURABR_PHASE_B_UO_TACC,  // 2字节，BIN，B相总畸变电流有效值越限日累计时间
    CURABR_PHASE_C_UO_TACC,  // 2字节，BIN，C相总畸变电流有效值越限日累计时间

    CUR_A_PHASE_ABR_VAL_MAX, // 8字节，A相总畸变电流日最大值记录
                                // 最大值：2字节，BIN(有符号原码)×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)
    CUR_B_PHASE_ABR_VAL_MAX, // 8字节，B相总畸变电流日最大值记录
                                // 最大值：2字节，BIN(有符号原码)×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)
    CUR_C_PHASE_ABR_VAL_MAX, // 8字节，C相总畸变电流日最大值记录
                                // 最大值：2字节，BIN(有符号原码)×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)
    VOLT_PHASE_A_HR_PEC_MAX, // 8*18字节，A相2~19次电压谐波含有率日最大值记录(最大值出现时间)
                                // 最大值：2字节，BIN×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)
    VOLT_PHASE_A_HR_PEC_MIN, // 8*18字节，A相2~19次电压谐波含有率日最小值记录(最小值出现时间),格式同上

    VOLT_PHASE_A_HR_PEC_AVG, // 2*18+2字节，A相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_B_HR_PEC_MAX, // 8*18字节，B相2~19次电压谐波含有率日最大值记录(最大值出现时间)
    VOLT_PHASE_B_HR_PEC_MIN, // 8*18字节，B相2~19次电压谐波含有率日最小值记录(最小值出现时间),格式同上
    VOLT_PHASE_B_HR_PEC_AVG, // 2*18+2字节，B相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_C_HR_PEC_MAX, // 8*18字节，C相2~19次电压谐波含有率日最大值记录(最大值出现时间)

    VOLT_PHASE_C_HR_PEC_MIN, // 8*18字节，C相2~19次电压谐波含有率日最小值记录(最小值出现时间),格式同上
    VOLT_PHASE_C_HR_PEC_AVG, // 2*18+2字节，C相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10
    CUR_A_PHASE_HR_PEC_MAX,  // 8*18字节，A相2~19次电流谐波含有率日最大值记录(最大值出现时间)
    CUR_A_PHASE_HR_PEC_MIN,  // 8*18字节，A相2~19次电流谐波含有率日最小值记录(最小值出现时间),格式同上
    CUR_A_PHASE_HR_PEC_AVG,  // 2*18+2字节，A相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10

    CUR_B_PHASE_HR_PEC_MAX,  // 8*18字节，B相2~19次电流谐波含有率日最大值记录(最大值出现时间)
    CUR_B_PHASE_HR_PEC_MIN,  // 8*18字节，B相2~19次电流谐波含有率日最小值记录(最小值出现时间),格式同上
    CUR_B_PHASE_HR_PEC_AVG,  // 2*18+2字节，B相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10
    CUR_C_PHASE_HR_PEC_MAX,  // 8*18字节，C相2~19次电流谐波含有率日最大值记录(最大值出现时间)
    CUR_C_PHASE_HR_PEC_MIN,  // 8*18字节，C相2~19次电流谐波含有率日最小值记录(最小值出现时间),格式同上

    CUR_C_PHASE_HR_PEC_AVG,  // 2*18+2字节，C相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_A_HR_VAL_MAX, // 8*18字节，A相2~19次电压谐波有效值日最大值记录(最大值出现时间)
    VOLT_PHASE_A_HR_VAL_MIN, // 8*18字节，A相2~19次电压谐波有效值日最小值记录(最小值出现时间)
    VOLT_PHASE_A_HR_VAL_AVG, // 2*18字节，A相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_B_HR_VAL_MAX, // 8*18字节，B相2~19次电压谐波有效值日最大值记录(最大值出现时间)

    VOLT_PHASE_B_HR_VAL_MIN, // 8*18字节，B相2~19次电压谐波有效值日最小值记录(最小值出现时间)
    VOLT_PHASE_B_HR_VAL_AVG, // 2*18+2字节，B相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_C_HR_VAL_MAX, // 8*18字节，C相2~19次电压谐波有效值日最大值记录(最大值出现时间)
    VOLT_PHASE_C_HR_VAL_MIN, // 8*18字节，C相2~19次电压谐波有效值日最小值记录(最小值出现时间)
    VOLT_PHASE_C_HR_VAL_AVG, // 2*18+2字节，C相2~19次电压谐波含有率日平均值,平均值：2字节，BIN×10

    CUR_A_PHASE_HR_VAL_MAX,  // 8*18字节，A相2~19次电流谐波有效值日最大值记录(最大值BIN(有符号原码)×100(2字节) + 出现时间(6字节))
    CUR_A_PHASE_HR_VAL_MIN,  // 8*18字节，A相2~19次电流谐波有效值日最小值记录(最小值出现时间)
    CUR_A_PHASE_HR_VAL_AVG,  // 2*18+2字节，A相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10
    CUR_B_PHASE_HR_VAL_MAX,  // 8*18字节，B相2~19次电流谐波有效值日最大值记录(最大值出现时间)
    CUR_B_PHASE_HR_VAL_MIN,  // 8*18字节，B相2~19次电流谐波有效值日最小值记录(最小值BIN(有符号原码)出现时间)

    CUR_B_PHASE_HR_VAL_AVG,  // 2*18+2字节，B相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10
    CUR_C_PHASE_HR_VAL_MAX,  // 8*18字节，C相2~19次电流谐波有效值日最大值记录(最大值出现时间)
    CUR_C_PHASE_HR_VAL_MIN,  // 8*18字节，C相2~19次电流谐波有效值日最小值记录(最小值出现时间)
    CUR_C_PHASE_HR_VAL_AVG,  // 2*18+2字节，C相2~19次电流谐波含有率日平均值,平均值：2字节，BIN×10
    VOLT_PHASE_A_ABR_MAX,    // 8字节，A相总畸变电压含有率日最大值记录
                                // 最大值：2字节，BIN×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)

    VOLT_PHASE_A_ABR_MIN,    // 8字节，A相总畸变电压含有率日最小值记录
    VOLT_PHASE_A_ABR_AVG,    // 2+2字节，A相总畸变电压含有率日平均值
    VOLT_PHASE_B_ABR_MAX,    // 8字节，B相总畸变电压含有率日最大值记录
    VOLT_PHASE_B_ABR_MIN,    // 8字节，B相总畸变电压含有率日最小值记录
    VOLT_PHASE_B_ABR_AVG,    // 2+2字节，B相总畸变电压含有率日平均值

    VOLT_PHASE_C_ABR_MAX,    // 8字节，C相总畸变电压含有率日最大值记录
    VOLT_PHASE_C_ABR_MIN,    // 8字节，C相总畸变电压含有率日最小值记录
    VOLT_PHASE_C_ABR_AVG,    // 2+2字节，C相总畸变电压含有率日平均值
    CUR_A_PHASE_ABR_MAX,     // 8字节，A相总畸变电流含有率日最大值记录
                                // 最大值：2字节，BIN×10 出现时间：6字节，BCD，(秒、分、时、日、月、年)
    CUR_A_PHASE_ABR_MIN,     // 8字节，A相总畸变电流含有率日最小值记录

    CUR_A_PHASE_ABR_AVG,     // 2+2字节，A相总畸变电流含有率日平均值
    CUR_B_PHASE_ABR_MAX,     // 8字节，B相总畸变电流含有率日最大值记录
    CUR_B_PHASE_ABR_MIN,     // 8字节，B相总畸变电流含有率日最小值记录
    CUR_B_PHASE_ABR_AVG,     // 2+2字节，B相总畸变电流含有率日平均值
    CUR_C_PHASE_ABR_MAX,     // 8字节，C相总畸变电流含有率日最大值记录

    CUR_C_PHASE_ABR_MIN,     // 8字节，C相总畸变电流含有率日最小值记录
    CUR_C_PHASE_ABR_AVG,     // 2+2字节，C相总畸变电流含有率日平均值
    CUR_ABNORMAL_REC_DAY,    // 70字节，电流回路异常记录
    VOLT_ABNORMAL_REC_DAY,   // 70字节，电压回路异常记录
    PHASE_ABNORMAL_REC_DAY,  // 67字节，相序异常记录

    HR_EXC_LMT_ALM_REC_DAY,  // 145字节，谐波越限告警记录
    NO_BALANCE_REC_DAY,      // 70字节，电压/电流不平衡度越限记录
    VOLT_EXC_LMT_REC_DAY,    // 40字节，电压越限记录
    CUR_EXC_LMT_REC_DAY,     // 40字节，电流越限记录
    ZAD_POS_MAX_FIX_REC,     // 日正向总有功需量最大值统计记录

    MTR_CONNECT_UNCONNECT_BIN,// 1字节，BIN，电能表通断电状态：为0表示断电，为11H表示通电
    MTR_REMOTE_CONNECT_TIME, // 6字节，BCD，最近一次电能表远程控制通电时间
    MTR_REMOTE_UNCONNECT_TIME,// 6字节，BCD，最近一次电能表远程控制断电时间
    POINT_DAY_BC_FLAG,       // 1字节，BIN，当日定点数据补抄成功标志。0:补抄失败；1:补抄成功
    POINT_DAY_BC_TIME,       // 6字节，BCD，电表回的定时冻结时间

    MET_PAY_ELECTR_DAY_FIX,  // 2个字节, BCD,购电次数-*/
    MET_REMAIN_DAY_FIX,      // 5个字节,BCD,剩余金额-
    MET_TOTAL_USED_DAY_FIX,  // 5个字节,BCD,累计购电金额-
    MET_REMAIN_ELETRIC_DAY_FIX,// 4个字节,BCD,剩余电量-
    MET_OVERFRAFT_ELETRIC_DAY_FIX,// 4个字节,BCD,透支电量

    MET_TOTAL_PAYED_ELETRIC_DAY_FIX,// 4个字节,BCD,累计购电量
    MET_TICK_FACK_ELETRIC_DAY_FIX,// 4个字节,BCD,赊欠门限电量
    MET_ALARM_ELETRIC_DAY_FIX,// 4个字节,BCD,报警电量
    MET_FAULT_ELETRIC_DAY_FIX,// 4个字节,BCD,故障电量
    D_Vol_Imbalance_95Probability_ID,// 2字节,BCD,日冻结电压不平衡度95%概率值

    D_Cur_Imbalance_95Probability_ID,// 2字节,BCD,日冻结电流不平衡度95%概率值 
    VOLT_A_Z_HR_PEC95_MAX,   // 2字节,BIN*100(0xCC60),A相谐波总畸变电压含有率95%概率大值
    VOLT_B_Z_HR_PEC95_MAX,   // 2字节,BIN*100(0xCC70),B相谐波总畸变电压含有率95%概率大值
    VOLT_C_Z_HR_PEC95_MAX,   // 2字节,BIN*100(0xCC80),C相谐波总畸变电压含有率95%概率大值
    CUR_A_Z_HR_VAL95_MAX,    // 2字节,BIN*100(0xCCE3),A相谐波电流总有效值95%概率大值

    CUR_B_Z_HR_VAL95_MAX,    // 2字节,BIN*100(0xCCE4),B相谐波电流总有效值95%概率大值
    CUR_C_Z_HR_VAL95_MAX,    // 2字节,BIN*100(0xCCE5),C相谐波电流总有效值95%概率大值
    VOLT_A_O_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCA6),A相谐波奇次电压总含有率95%概率大值
    VOLT_B_O_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCA7),B相谐波奇次电压总含有率95%概率大值
    VOLT_C_O_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCA8),C相谐波奇次电压总含有率95%概率大值

    VOLT_A_E_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCA9),A相谐波偶次电压总含有率95%概率大值
    VOLT_B_E_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCAA),B相谐波偶次电压总含有率95%概率大值
    VOLT_C_E_HR_PEC95_MAX,   // 2字节,BIN*100(0xCCAB),C相谐波偶次电压总含有率95%概率大值
    VOLT_A_X_HR_PEC95_MAX,   // 2*18字节,BIN*100(0xCC6X),A相谐波电压2-19次含有率95%概率大值
    VOLT_B_X_HR_PEC95_MAX,   // 2*18字节,BIN*100(0xCC7X),B相谐波电压2-19次含有率95%概率大值

    VOLT_C_X_HR_PEC95_MAX,   // 2*18字节,BIN*100(0xCC8X),C相谐波电压2-19次含有率95%概率大值
    CUR_A_X_HR_VAL95_MAX,    // 2*18字节,BIN*100(0xC4CX),A相谐波电流2-19次有效值95%概率大值
    CUR_B_X_HR_VAL95_MAX,    // 2*18字节,BIN*100(0xC4DX),B相谐波电流2-19次有效值95%概率大值
    CUR_C_X_HR_VAL95_MAX,    // 2*18字节,BIN*100(0xC4EX),C相谐波电流2-19次有效值95%概率大值
    MET_PAY_ELECTR_TIME_DAY, // 2个字节, BCD,购电次数-*/

    MET_REMAIN_MONEY_DAY,    // 5个字节,BCD,剩余金额-
    MET_TOTAL_USED_MONEY_DAY,// 5个字节,BCD,累计购电金额-
    BC_DATAID,               // 64字节, 补抄数据项标志位，每一个位对应一个补抄任务数据ID
    POINT_Td_D,              // 10字节, BIN ,电表内的冻结时标标记 0x55:抄读标记
    PATCH_SIXTY_TWO_DAYS_FLAG,// 1字节,62天补抄标记(补抄完毕0xAA, 上报完毕0x55, 其他无效)

    PATCH_DAYS_TASKREPORT_FLAG,// 8字节,D0-D63分别表示1-64号日任务的上报标志，1-已经上报，0-尚未上报
    PATCH_SIXTY_TWO_CUR_FLAG,// 1字节,62天补抄标记，曲线数据(补抄完毕0xAA, 上报完毕0x55, 其他无效)
    PATCH_CUR_TASKREPORT_FLAG,// 8字节,D0-D63分别表示1-64号曲线任务的上报标志，1-已经上报，0-尚未上报
    CAP_COMPENSATE_TIME_FIX_DAY,// 4*16字节，BIN，日补偿投入时间
    CAP_COMPENSATE_CNT_FIX_DAY,// 4*16字节，BIN，日补偿投入次数

    CAP_COMPENSATE_EN_FIX_DAY,// 8字节,BIN x 10000(kwh)，日累计补偿无功电量
    EVENT_METERHALT_CNT_DAY, // 3字节, 终端停电掉电/上电记录发生次数
    EVENT_METERHALT_INFO_DAY,// 12*10字节, 上十条终端停电掉电/上电记录
                                // #1: 6字节,停电时刻（YYMMDDhhmm）
                                // #2: 6字节,上电时刻（YYMMDDhhmm）

DATA_POINT_DAY_END_ID,
};

/******************--3计量点曲线数据项ID--*********************/
enum data_curve
{
    A_VOLT=0,                // 2字节，BIN X 10（V），A 相电压
    B_VOLT,                  // 2字节，BIN X 10（V），B 相电压
    C_VOLT,                  // 2字节，BIN X 10（V），C 相电压
    A_CUR,                   // 4字节，BIN X 1000（A），A 相电流
    B_CUR,                   // 4字节，BIN X 1000（A），B 相电流

    C_CUR,                   // 4字节，BIN X 1000（A），C 相电流
    G_CUR,                   // 4字节，BIN X 1000（A），零相电流
    A_VOLT_AG,               // 2字节，BIN X 10（V），A 相电压相角
    B_VOLT_AG,               // 2字节，BIN X 10（V），B 相电压相角
    C_VOLT_AG,               // 2字节，BIN X 10（V），C 相电压相角

    A_CUR_AG,                // 2字节，BIN X 100（A），A 相电流相角
    B_CUR_AG,                // 2字节，BIN X 100（A），B 相电流相角
    C_CUR_AG,                // 2字节，BIN X 100（A），C 相电流相角
    G_CUR_AG,                // 2字节，BIN X 100（A），零相电流相角
    A_AP,                    // 4字节，BIN X 10000 （KW），A 相有功功率

    B_AP,                    // 4字节，BIN X 10000 （KW），B 相有功功率
    C_AP,                    // 4字节，BIN X 10000 （KW），C 相有功功率
    Z_AP,                    // 4字节，BIN X 10000 （KW），总有功功率
    A_RP,                    // 4字节，BIN X 10000 （KW），A 相无功功率
    B_RP,                    // 4字节，BIN X 10000 （KW），B 相无功功率

    C_RP,                    // 4字节，BIN X 10000 （KW），C 相无功功率
    Z_RP,                    // 4字节，BIN X 10000 （KW），总无功功率
    AE_Z_ER,                 // 8字节，BIN X 10000 （KWH），有功电量（总）
    RE_Z_ER,                 // 8字节，BIN X 10000 （KWH），无功电量（总）
    AE_POS_Z_ER,             // 8字节，BIN X 10000 （KWH），正向有功电量（总）

    AE_NEG_Z_ER,             // 8字节，BIN X 10000 （KWH），反向有功电量（总）
    RE_POS_Z_ER,             // 8字节，BIN X 10000 （KWH），正向无功电量（总)
    RE_NEG_Z_ER,             // 8字节，BIN X 10000 （KWH），反向无功电量（总）
    RE_FIRST_Z_ER,           // 8字节，BIN X 10000 （KWH），第一象限无功电量（总）
    RE_FOURTH_Z_ER,          // 8字节，BIN X 10000 （KWH），第四象限无功电量（总）

    RE_SECOND_Z_ER,          // 8字节，BIN X 10000 （KWH），第二象限无功电量（总）
    RE_THIRD_Z_ER,           // 8字节，BIN X 10000 （KWH），第三象限无功电量（总）
    A_PF,                    // 2字节，BIN X 1000 %，A 相功率因数曲线
    B_PF,                    // 2字节，BIN X 1000 %，B 相功率因数曲线
    C_PF,                    // 2字节，BIN X 1000 %，C 相功率因数曲线

    Z_PF,                    // 2字节，BIN X 1000 %，总功率因数
    AE_EN,                   // 8字节，BIN X 10000 （KWH），自然日正向有功电量
    RE_EN,                   // 8字节，BIN X 10000 （KWH），自然日正向无功电量
    FREQ,                    // 2字节，BIN X 100（Hz）电网频率
    AVER_POS_AD,             // 4字节，BIN X 10 (W)，正向有功需量滑差

    HOUR_ER,                 // 8字节，BIN X 10000 （KWH），小时对照电量
    AE_POS_ER_HOUR_Z,        // 8字节，BIN X 10000 （KWH），15分钟冻结正向有功总电能量 测量点曲线(delta值)
    AE_NEG_ER_HOUR_Z,        // 8字节，BIN X 10000 （KWH），15分钟冻结反向有功总电能量 测量点曲线
    RE_POS_ER_HOUR_Z,        // 8字节，BIN X 10000 （KWH），15分钟冻结正向无功总电能量 测量点曲线
    RE_NEG_ER_HOUR_Z,        // 8字节，BIN X 10000 （KWH），15分钟冻结反向无功总电能量 测量点曲线

    VIR_POWER_ACT_CUR,       // 4字节，BIN X 100 （KW），总加组有功功率曲线
    VIR_POWER_REACT_CUR,     // 4字节，BIN X 100 （KW），总加组无功功率曲线
    VIR_AE_CUR,              // 8字节，BIN X 10000 （KWH），总加组有功电能量曲线
    VIR_RE_CUR,              // 8字节，BIN X 10000 （KWH），总加组无功电能量曲线
    VIR_AE_HOUR_CUR,         // 8字节，BIN X 10000 （KWH），15分钟冻结总加有功总电能量曲线(总加组delta值) 

    VIR_RE_HOUR_CUR,         // 8字节，BIN X 10000 （KWH），15分钟冻结总加无功总电能量曲线 (总加组)
    CURVE_Td_D,              // 5字节, BCD ,电表内的曲线时标
    AE_FIRST_Z_ER,           // 8字节，BIN X 10000 （KWH），第一象限有功电量（总）
    AE_FOURTH_Z_ER,          // 8字节，BIN X 10000 （KWH），第四象限有功电量（总）
    DATA_CURVE_COMPLEX_AREA_TEST,// 各地区增补的曲线数据（CURVE_DATA_DAY）

DATA_CURVE_END_ID,
};

/******************--4计量点日分钟曲线数据项ID--*********************/
/******************--未使用--*********************/
enum data_mincurve
{
    A_VOLT_MIN=0,            // 2字节，BIN X 10（V），A 相电压
    B_VOLT_MIN,              // 2字节，BIN X 10（V），B 相电压
    C_VOLT_MIN,              // 2字节，BIN X 10（V），C 相电压
    A_CUR_MIN,               // 4字节，BIN X 1000（A），A 相电流
    B_CUR_MIN,               // 4字节，BIN X 1000（A），B 相电流

    C_CUR_MIN,               // 4字节，BIN X 1000（A），C 相电流
    G_CUR_MIN,               // 4字节，BIN X 1000（A），零相电流
    Z_AP_MIN,                // 4字节，BIN X 10000，总有功功率
    Z_RP_MIN,                // 4字节，BIN X 10000，总无功功率
    DATA_MINCURVE_COMPLEX_AREA_TEST,// 各地区增补的分钟曲线数据（MINCURVE_DATA）

DATA_MINCURVE_END_ID,
};

/******************--5计量点日计算数据项ID--*********************/
enum data_cal_day
{
    F_OVER_AE_DAY=0,         // 8字节，BIN X 10000 （KWH），峰超电量(日)
    G_BELOW_AE_DAY,          // 8字节，BIN X 10000 （KWH），谷欠电量(日)
    G_SEG_AE_RATIO,          // 2字节，BIN X 100 %，日谷段用电比
    POS_AP_MAX_REC_Z,        // 10字节，总正向有功功率最大值记录,格式同上
    NEG_AP_MAX_REC_Z,        // 10字节，总反向有功功率最大值记录,格式同上

    PAP_MAX_REC_A,           // 10字节，A相有功功率最大值记录(有功功率最大值最值出现时间)
                                // 最大值：4字节，BIN X 10000（KW) 出现时间：6字节，BCD，（秒分时日月年）
    PAP_MAX_REC_B,           // 10字节，B相有功功率最大值记录(有功功率最大值最值出现时间),格式同上
    PAP_MAX_REC_C,           // 10字节，C相有功功率最大值记录(有功功率最大值最值出现时间),格式同上
    POS_AP_MIN_REC_Z,        // 10字节，总正向有功功率最小值记录
                                // 最小值：4字节，BIN X 10000（KW) 出现时间：6字节，BCD，（秒分时日月年）
    NEG_AP_MIN_REC_Z,        // 10字节，总反向有功功率最小值记录,格式同上

    PAP_MIN_REC_A,           // 10字节，A相有功功率最小值记录,格式同上
    PAP_MIN_REC_B,           // 10字节，B相有功功率最小值记录,格式同上
    PAP_MIN_REC_C,           // 10字节，C相有功功率最小值记录,格式同上
    PCRP_MAX_REC_A,          // 10字节，A相容性无功功率最大值记录,格式同上
    PCRP_MAX_REC_B,          // 10字节，B相容性无功功率最大值记录,格式同上

    PCRP_MAX_REC_C,          // 10字节，C相容性无功功率最大值记录,格式同上
    PLRP_MAX_REC_A,          // 10字节，A相感性无功功率最大值记录,格式同上
    PLRP_MAX_REC_B,          // 10字节，B相感性无功功率最大值记录,格式同上
    PLRP_MAX_REC_C,          // 10字节，C相感性无功功率最大值记录,格式同上
    ZRP_MAX_REC,             // 10字节，总无功功率最大值记录,格式同上

    PRP_MAX_REC_A,           // 10字节，A相无功功率最大值记录,格式同上
    PRP_MAX_REC_B,           // 10字节，B相无功功率最大值记录,格式同上
    PRP_MAX_REC_C,           // 10字节，C相无功功率最大值记录,格式同上
    ZRP_MIN_REC,             // 10字节，总无功功率最小值记录,格式同上
    PRP_MIN_REC_A,           // 10字节，A相无功功率最小值记录,格式同上

    PRP_MIN_REC_B,           // 10字节，B相无功功率最小值记录,格式同上
    PRP_MIN_REC_C,           // 10字节，C相无功功率最小值记录,格式同上
    F_SEG_MAX_P,             // 10字节，峰段最大功率记录,格式同上
    G_SEG_MIN_P,             // 10字节，谷段最小功率记录,格式同上
    HACK_FLAG,               // 4字节，BIN，窃电嫌疑标志

    Z_APP_MAX_REC,           // 10字节，总视在功率最大值记录
                                // 值：4字节，BIN X 10000（KW) 出现时间：6字节，BCD，（秒分时日月年）
    Z_APP_MIN_REC,           // 10字节，总视在功率最小值记录,格式同上
    P_PF_MAX_REC_A,          // 8字节，A相功率因数最大值记录 值：2字节，BIN 出现时间：6字节，BCD，（秒分时日月年）
    P_PF_MAX_REC_B,          // 8字节，B相功率因数最大值记录,格式同上
    P_PF_MAX_REC_C,          // 8字节，C相功率因数最大值记录,格式同上

    P_PF_MIN_REC_A,          // 8字节，A相功率因数最小值记录,格式同上
    P_PF_MIN_REC_B,          // 8字节，B相功率因数最小值记录,格式同上
    P_PF_MIN_REC_C,          // 8字节，C相功率因数最小值记录,格式同上
    Z_PF_MAX_REC,            // 8字节，总功率因数最大值记录,格式同上
    Z_PF_MIN_REC,            // 8字节，总功率因数最小值记录,格式同上

    A_CUR_AVG15_MAX_REC,     // 8字节，连续15分钟A电流最大值记录,格式同上
    A_CUR_AVG15_MIN_REC,     // 8字节，连续15分钟A电流最小值记录,格式同上
    B_CUR_AVG15_MAX_REC,     // 8字节，连续15分钟B电流最大值记录,格式同上
    B_CUR_AVG15_MIN_REC,     // 8字节，连续15分钟B电流最小值记录,格式同上
    C_CUR_AVG15_MAX_REC,     // 8字节，连续15分钟C电流最大值记录,格式同上

    C_CUR_AVG15_MIN_REC,     // 8字节，连续15分钟C电流最小值记录,格式同上
    ENET_FREQ_MAX_REC,       // 8字节，电网频率最大值记录,格式同上
    ENET_FREQ_MIN_REC,       // 8字节，电网频率最小值记录,格式同上
    POW_OUT_ACC,             // 2字节，BIN，自然日功率越限次数
    F_SEG_MAX_RP,            // 10字节，峰段最大功率记录(无功)
                                // 最小值：4字节，BIN X 10000（KW) 出现时间：6字节，BCD，（秒分时日月年）

    G_SEG_MIN_RP,            // 10字节，谷段最小功率记录(无功),格式同上
    ZAD_POS_MAX_REC,         // 10字节，正向总有功需量最大值记录,格式同上
    POS_AD_1_MAX_REC,        // 10字节，费率段（尖）正向有功需量最大值记录,格式同上
    POS_AD_2_MAX_REC,        // 10字节，费率段（峰）正向有功需量最大值记录,格式同上
    POS_AD_3_MAX_REC,        // 10字节，费率段（平）正向有功需量最大值记录,格式同上

    POS_AD_4_MAX_REC,        // 10字节，费率段（谷）正向有功需量最大值记录,格式同上
    POS_AD_5_MAX_REC,        // 10字节，费率段5正向有功需量最大值记录,格式同上
    POS_AD_6_MAX_REC,        // 10字节，费率段6正向有功需量最大值记录,格式同上
    POS_AD_7_MAX_REC,        // 10字节，费率段7正向有功需量最大值记录,格式同上
    POS_AD_8_MAX_REC,        // 10字节，费率段8正向有功需量最大值记录,格式同上

    EXT_POS_AD_MAX_REC,      // 60字节，费率段（9~14）正向有功需量最大值记录,格式同上
    ZAD_NEG_MAX_REC,         // 10字节，反向总有功需量最大值记录,格式同上
    NEG_AD_1_MAX_REC,        // 10字节，费率段（尖）反向有功需量最大值记录,格式同上
    NEG_AD_2_MAX_REC,        // 10字节，费率段（峰）反向有功需量最大值记录,格式同上
    NEG_AD_3_MAX_REC,        // 10字节，费率段（平）反向有功需量最大值记录,格式同上

    NEG_AD_4_MAX_REC,        // 10字节，费率段（谷）反向有功需量最大值记录,格式同上
    NEG_AD_5_MAX_REC,        // 10字节，费率段5反向有功需量最大值记录,格式同上
    NEG_AD_6_MAX_REC,        // 10字节，费率段6反向有功需量最大值记录,格式同上
    NEG_AD_7_MAX_REC,        // 10字节，费率段7反向有功需量最大值记录,格式同上
    NEG_AD_8_MAX_REC,        // 10字节，费率段8反向有功需量最大值记录,格式同上

    EXT_NEG_AD_MAX_REC,      // 60字节，费率段（9~14）反向有功需量最大值记录,格式同上
    ZRD_POS_MAX_REC,         // 10字节，正向总无功需量最大值记录
                                // 最小值：4字节，BIN X 10000（KW) 出现时间：6字节，BCD，（秒分时日月年）
    POS_RD_1_MAX_REC,        // 10字节，费率段（尖）正向无功需量最大值记录,格式同上
    POS_RD_2_MAX_REC,        // 10字节，费率段（峰）正向无功需量最大值记录,格式同上
    POS_RD_3_MAX_REC,        // 10字节，费率段（平）正向无功需量最大值记录,格式同上

    POS_RD_4_MAX_REC,        // 10字节，费率段（谷）正向无功需量最大值记录,格式同上
    POS_RD_5_MAX_REC,        // 10字节，费率段5正向无功需量最大值记录,格式同上
    POS_RD_6_MAX_REC,        // 10字节，费率段6正向无功需量最大值记录,格式同上
    POS_RD_7_MAX_REC,        // 10字节，费率段7正向无功需量最大值记录,格式同上
    POS_RD_8_MAX_REC,        // 10字节，费率段8正向无功需量最大值记录,格式同上

    EXT_POS_RD_MAX_REC,      // 60字节，费率段（9~14）正向无功需量最大值记录,格式同上
    ZRD_NEG_MAX_REC,         // 10字节，反向总无功需量最大值记录,格式同上
    NEG_RD_1_MAX_REC,        // 10字节，费率段（尖）反向无功需量最大值记录,格式同上
    NEG_RD_2_MAX_REC,        // 10字节，费率段（峰）反向无功需量最大值记录,格式同上
    NEG_RD_3_MAX_REC,        // 10字节，费率段（平）反向无功需量最大值记录,格式同上

    NEG_RD_4_MAX_REC,        // 10字节，费率段（谷）反向无功需量最大值记录,格式同上
    NEG_RD_5_MAX_REC,        // 10字节，费率段5反向无功需量最大值记录,格式同上
    NEG_RD_6_MAX_REC,        // 10字节，费率段6反向无功需量最大值记录,格式同上
    NEG_RD_7_MAX_REC,        // 10字节，费率段7反向无功需量最大值记录,格式同上
    NEG_RD_8_MAX_REC,        // 10字节，费率段8反向无功需量最大值记录,格式同上

    EXT_NEG_RD_MAX_REC,      // 60字节，费率段（9~14）反向无功需量最大值记录,格式同上
    RT_POS_AE_Z_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日正向总有功电量
    RT_POS_AE_1_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（尖）正向有功电量
    RT_POS_AE_2_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（峰）正向有功电量
    RT_POS_AE_3_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（平）正向有功电量

    RT_POS_AE_4_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（谷）正向有功电量-
    RT_POS_AE_5_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段5正向有功电量
    RT_POS_AE_6_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段6正向有功电量
    RT_POS_AE_7_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段7正向有功电量
    RT_POS_AE_8_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段8正向有功电量

    RT_POS_AE_EXT_ER_DAY,    // 48字节，BIN X 10000 （KWH），当前日9~14费率段正向有功电量
    RT_NEG_AE_Z_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日反向总有功电量
    RT_NEG_AE_1_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（尖）反向有功电量
    RT_NEG_AE_2_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（峰）反向有功电量
    RT_NEG_AE_3_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（平）反向有功电量

    RT_NEG_AE_4_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（谷）反向有功电量
    RT_NEG_AE_5_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段5反向有功电量
    RT_NEG_AE_6_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段6反向有功电量
    RT_NEG_AE_7_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段7反向有功电量
    RT_NEG_AE_8_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段8反向有功电量

    RT_NEG_AE_EXT_ER_DAY,    // 48字节，BIN X 10000 （KWH），当前日9~14费率段反向有功电量
    RT_POS_RE_Z_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日正向总无功电量
    RT_POS_RE_1_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（尖）正向无功电量
    RT_POS_RE_2_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（峰）正向无功电量
    RT_POS_RE_3_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（平）正向无功电量

    RT_POS_RE_4_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（谷）正向无功电量
    RT_POS_RE_5_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段5正向无功电量
    RT_POS_RE_6_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段6正向无功电量
    RT_POS_RE_7_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段7正向无功电量
    RT_POS_RE_8_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段8正向无功电量

    RT_POS_RE_EXT_ER_DAY,    // 48字节，BIN X 10000 （KWH），当前日9~14费率段正向无功电量
    RT_NEG_RE_Z_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日反向总无功电量
    RT_NEG_RE_1_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（尖）反向无功电量
    RT_NEG_RE_2_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（峰）反向无功电量
    RT_NEG_RE_3_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（平）反向无功电量

    RT_NEG_RE_4_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段（谷）反向无功电量
    RT_NEG_RE_5_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段5反向无功电量
    RT_NEG_RE_6_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段6反向无功电量
    RT_NEG_RE_7_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段7反向无功电量
    RT_NEG_RE_8_ER_DAY,      // 8字节，BIN X 10000 （KWH），当前日费率段8反向无功电量

    RT_NEG_RE_EXT_ER_DAY,    // 48字节，BIN X 10000 （KWH），当前日9~14费率段反向无功电量
    RT_FIRST_RE_Z_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限总无功电量
    RT_FIRST_RE_1_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段（尖）无功电量
    RT_FIRST_RE_2_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段（峰）无功电量
    RT_FIRST_RE_3_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段（平）无功电量

    RT_FIRST_RE_4_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段（谷）无功电量
    RT_FIRST_RE_5_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段5无功电量
    RT_FIRST_RE_6_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段6无功电量
    RT_FIRST_RE_7_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段7无功电量
    RT_FIRST_RE_8_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第一象限费率段8无功电量

    RT_FIRST_RE_EXT_ER_DAY,  // 48字节，BIN X 10000 （KWH），当前日第一象限9~14费率段无功电量
    RT_FOURTH_RE_Z_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限总无功电量
    RT_FOURTH_RE_1_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段（尖）无功电量
    RT_FOURTH_RE_2_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段（峰）无功电量
    RT_FOURTH_RE_3_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段（平）无功电量

    RT_FOURTH_RE_4_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段（谷）无功电量
    RT_FOURTH_RE_5_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段5无功电量
    RT_FOURTH_RE_6_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段6无功电量
    RT_FOURTH_RE_7_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段7无功电量
    RT_FOURTH_RE_8_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第四象限费率段8无功电量

    RT_FOURTH_RE_EXT_ER_DAY, // 48字节，BIN X 10000 （KWH），当前日第四象限9~14费率段无功电量
    RT_SECOND_RE_Z_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限总无功电量
    RT_SECOND_RE_1_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段（尖）无功电量
    RT_SECOND_RE_2_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段（峰）无功电量
    RT_SECOND_RE_3_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段（平）无功电量

    RT_SECOND_RE_4_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段（谷）无功电量
    RT_SECOND_RE_5_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段5无功电量
    RT_SECOND_RE_6_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段6无功电量
    RT_SECOND_RE_7_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段7无功电量
    RT_SECOND_RE_8_ER_DAY,   // 8字节，BIN X 10000 （KWH），当前日第二象限费率段8无功电量

    RT_SECOND_RE_EXT_ER_DAY, // 48字节，BIN X 10000 （KWH），当前日第二象限9~14费率段无功电量
    RT_THIRD_RE_Z_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限总无功电量
    RT_THIRD_RE_1_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段（尖）无功电量
    RT_THIRD_RE_2_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段（峰）无功电量
    RT_THIRD_RE_3_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段（平）无功电量

    RT_THIRD_RE_4_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段（谷）无功电量
    RT_THIRD_RE_5_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段5无功电量
    RT_THIRD_RE_6_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段6无功电量
    RT_THIRD_RE_7_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段7无功电量
    RT_THIRD_RE_8_ER_DAY,    // 8字节，BIN X 10000 （KWH），当前日第三象限费率段8无功电量

    RT_THIRD_RE_EXT_ER_DAY,  // 48字节，BIN X 10000 （KWH），当前日第三象限9~14费率段无功电量
    A_VOLT_HIGH_TACC_DAY,    // 2字节，BIN，A相电压越上限累计时间(日)
    B_VOLT_HIGH_TACC_DAY,    // 2字节，BIN，B相电压越上限累计时间(日)
    C_VOLT_HIGH_TACC_DAY,    // 2字节，BIN，C相电压越上限累计时间(日)
    A_VOLT_LOW_TACC_DAY,     // 2字节，BIN，A相电压越下限累计时间(日)

    B_VOLT_LOW_TACC_DAY,     // 2字节，BIN，B相电压越下限累计时间(日)
    C_VOLT_LOW_TACC_DAY,     // 2字节，BIN，C相电压越下限累计时间(日)
    A_VOLT_OU_TACC_DAY,      // 2字节，BIN，A相电压越异常上限累计时间(日)
    B_VOLT_OU_TACC_DAY,      // 2字节，BIN，B相电压越异常上限累计时间(日)
    C_VOLT_OU_TACC_DAY,      // 2字节，BIN，C相电压越异常上限累计时间(日)

    A_VOLT_OD_TACC_DAY,      // 2字节，BIN，A相电压越异常下限累计时间(日)
    B_VOLT_OD_TACC_DAY,      // 2字节，BIN，B相电压越异常下限累计时间(日)
    C_VOLT_OD_TACC_DAY,      // 2字节，BIN，C相电压越异常下限累计时间(日)
    VOLT_OU_TACC_Z_DAY,      // 2字节，BIN，-总电压越异常上限累计时间(日)
    VOLT_OD_TACC_Z_DAY,      // 2字节，BIN，-总电压越异常下限累计时间(日)

    VOLT_HIGH_TACC_Z_DAY,    // 2字节，BIN，-总电压越上限累计时间(日)
    VOLT_LOW_TACC_Z_DAY,     // 2字节，BIN，-总电压越下限累计时间(日)
    VOLT_REGULRA_TACC_Z_DAY, // 2字节，BIN，-总电压合格日累计时间（日）
    A_VOLT_REGULRA_TACC_DAY, // 2字节，BIN，A相电压合格日累计时间（日）
    B_VOLT_REGULRA_TACC_DAY, // 2字节，BIN，B相电压合格日累计时间（日）

    C_VOLT_REGULRA_TACC_DAY, // 2字节，BIN，C相电压合格日累计时间（日）
    CAL_DAY_A_VOLT_N_RATE,   // 4字节，BIN X 100%，A相电压合格率
    CAL_DAY_A_VOLT_O_RATE,   // 4字节，BIN X 100%，A相电压越上限率
    CAL_DAY_A_VOLT_D_RATE,   // 4字节，BIN X 100%，A相电压越下限率
    CAL_DAY_B_VOLT_N_RATE,   // 4字节，BIN X 100%，B相电压合格率

    CAL_DAY_B_VOLT_O_RATE,   // 4字节，BIN X 100%，B相电压越上限率
    CAL_DAY_B_VOLT_D_RATE,   // 4字节，BIN X 100%，B相电压越下限率
    CAL_DAY_C_VOLT_N_RATE,   // 4字节，BIN X 100%，C相电压合格率
    CAL_DAY_C_VOLT_O_RATE,   // 4字节，BIN X 100%，C相电压越上限率
    CAL_DAY_C_VOLT_D_RATE,   // 4字节，BIN X 100%，C相电压越下限率

    CUR_OU_TACC_Z_DAY,       // 2字节，BIN，总电流越异常上限累计时间（日）
    A_CUR_OVER_TIME_DAY,     // 2字节，BIN，A相电流越上限累计时间（日）
    B_CUR_OVER_TIME_DAY,     // 2字节，BIN，B相电流越上限累计时间（日）
    C_CUR_OVER_TIME_DAY,     // 2字节，BIN，C相电流越上限累计时间（日）
    Z_CUR_OVER_TIME_DAY,     // 2字节，BIN，零序电流越上限累计时间（日）

    A_CUR_OU_TACC_DAY,       // 2字节，BIN，A相电流越异常上限累计时间（日）
    B_CUR_OU_TACC_DAY,       // 2字节，BIN，B相电流越异常上限累计时间（日）
    C_CUR_OU_TACC_DAY,       // 2字节，BIN，C相电流越异常上限累计时间（日）
    UI_OU_TIME_DAY,          // 2字节，BIN，视在功率越上上限累计时间（日）
    UI_OVER_TIME_DAY,        // 2字节，BIN，视在功率越上限累计时间（日）

    VOLT_MAX_REC_A,          // 8字节，A相电压最大值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    VOLT_MIN_REC_A,          // 8字节，A相电压最小值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    VOLT_MAX_REC_B,          // 8字节，B相电压最大值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    VOLT_MIN_REC_B,          // 8字节，B相电压最小值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    VOLT_MAX_REC_C,          // 8字节，C相电压最大值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)

    VOLT_MIN_REC_C,          // 8字节，C相电压最小值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    A_CUR_MAX_REC,           // 10字节，A相电流最大值记录,值：4字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    A_CUR_MIN_REC,           // 10字节，A相电流最小值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    B_CUR_MAX_REC,           // 10字节，B相电流最大值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    B_CUR_MIN_REC,           // 10字节，B相电流最小值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)

    C_CUR_MAX_REC,           // 10字节，C相电流最大值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    C_CUR_MIN_REC,           // 10字节，C相电流最小值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    G_CUR_MAX_REC,           // 10字节，零相电流最小值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    G_CUR_MIN_REC,           // 10字节，零相电流最小值记录,值：4字节，BIN X 1000(A)出现时间：6字节，BCD(秒分时日月年)
    AVER_A_VOLT,             // 2字节，BIN X 10（V），A相平均电压

    AVER_B_VOLT,             // 2字节，BIN X 10（V），B相平均电压
    AVER_C_VOLT,             // 2字节，BIN X 10（V），C相平均电压
    AVER_A_CURR,             // 2字节，BIN X 10（V），A相平均电流
    AVER_B_CURR,             // 2字节，BIN X 10（V），B相平均电流
    AVER_C_CURR,             // 2字节，BIN X 10（V），C相平均电流

    ALL_PF_AVG_DAY,          // 2字节，总功率因数日平均值，1位小数
    CUR_NO_BALANCE_ACCT,     // 2字节，BIN ，电流不平衡日累计时间
    VOLT_NO_BALANCE_ACCT,    // 2字节，BIN ，电压不平衡日累计时间
    MAX_BALANCE_REC,         // 8字节，三相不平衡率最大值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)
    MIN_BALANCE_REC,         // 8字节，三相不平衡率最小值记录,值：2字节，BIN X 10(V)出现时间：6字节，BCD(秒分时日月年)

    Z_POW_ZERO_TACC,         // 2字节，BIN，总自然日功率为零时间
    P_POW_ZERO_TACC_A,       // 2字节，BIN，A相自然日功率为零时间
    P_POW_ZERO_TACC_B,       // 2字节，BIN，B相自然日功率为零时间
    P_POW_ZERO_TACC_C,       // 2字节，BIN，C相自然日功率为零时间
    A_AD_MAX_REC,            // 10字节，A相有功最大需量记录,值：4字节，BIN X 10000（KW）出现时间：6字节，BCD(秒分时日月年)

    B_AD_MAX_REC,            // 10字节，B相有功最大需量记录,值：4字节，BIN X 10000（KW）出现时间：6字节，BCD(秒分时日月年)
    C_AD_MAX_REC,            // 10字节，C相有功最大需量记录,值：4字节，BIN X 10000（KW）出现时间：6字节，BCD(秒分时日月年)
    PF_Z_CALC,               // 2字节，BIN X 100%，日功率因数（总）
    PF_1_CALC,               // 2字节，BIN X 100%，日功率因数（尖）
    PF_2_CALC,               // 2字节，BIN X 100%，日功率因数（峰）

    PF_3_CALC,               // 2字节，BIN X 100%，日功率因数（平）
    PF_4_CALC,               // 2字节，BIN X 100%，日功率因数（谷）
    PF_5_CALC,               // 2字节，BIN X 100%，费率段5日功率因数
    PF_6_CALC,               // 2字节，BIN X 100%，费率段6日功率因数
    PF_7_CALC,               // 2字节，BIN X 100%，费率段7日功率因数

    PF_8_CALC,               // 2字节，BIN X 100%，费率段8日功率因数
    PF_EXT_CALC,             // 12字节，BIN X 100%，扩展费率段（9~14）日功率因数
    LF_Z_CALC,               // 2字节，BIN X 100%，-日负荷率（总）
    LF_1_CALC,               // 2字节，BIN X 100%，-日负荷率（尖）
    LF_2_CALC,               // 2字节，BIN X 100%，-日负荷率（峰）

    LF_3_CALC,               // 2字节，BIN X 100%，-日负荷率（平）
    LF_4_CALC,               // 2字节，BIN X 100%，-日负荷率（谷）
    LF_5_CALC,               // 2字节，BIN X 100%，-费率段5日负荷率
    LF_6_CALC,               // 2字节，BIN X 100%，-费率段6日负荷率
    LF_7_CALC,               // 2字节，BIN X 100%，-费率段7日负荷率

    LF_8_CALC,               // 2字节，BIN X 100%，-费率段8日负荷率
    LF_EXT_CALC,             // 12字节，BIN X 100%，-扩展费率段（9~14）日负荷率
    A_VOLT_HIGH_ACC_DAY,     // 2字节，BIN，A相电压越上限累计次数(日)
    A_VOLT_LOW_ACC_DAY,      // 2字节，BIN，A相电压越下限累计次数(日)
    B_VOLT_HIGH_ACC_DAY,     // 2字节，BIN，B相电压越上限累计次数(日)

    B_VOLT_LOW_ACC_DAY,      // 2字节，BIN，B相电压越下限累计次数(日)
    C_VOLT_HIGH_ACC_DAY,     // 2字节，BIN，C相电压越上限累计次数(日)
    C_VOLT_LOW_ACC_DAY,      // 2字节，BIN，C相电压越下限累计次数(日)
    CUR_OVRLD_DAY,           // 2字节，BIN，电流过负荷比例
    RT_POS_AD_ID,            // 4字节，BIN X 10000（KW），正向有功需量

    RT_NEG_AD_ID,            // 4字节，BIN X 10000（KW），反向有功需量
    RT_POS_RD_ID,            // 4字节，BIN X 10000（KW），正向无功需量
    RT_NEG_RD_ID,            // 4字节，BIN X 10000（KW），反向无功需量
    AP_POS_4HOUR,            // 4字节，BIN X 10000（KW），4小时平均负荷（正向有功总）
    RP_POS_4HOUR,            // 4字节，BIN X 10000（KW），4小时平均负荷（正向无功总）

    AP_POS_DAY,              // 4字节，BIN X 10000（KW），日平均负荷（正向有功总）
    RP_POS_DAY,              // 4字节，BIN X 10000（KW），日平均负荷（正向无功总）
    VIR_AE_CAL_DAY_Z,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（总）
    VIR_AE_CAL_DAY_1,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（尖）
    VIR_AE_CAL_DAY_2,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（峰）

    VIR_AE_CAL_DAY_3,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（平）
    VIR_AE_CAL_DAY_4,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（谷）
    VIR_AE_CAL_DAY_5,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（5费率）
    VIR_AE_CAL_DAY_6,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（6费率）
    VIR_AE_CAL_DAY_7,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（7费率）

    VIR_AE_CAL_DAY_8,        // 8字节，BIN X 10000（KWH），当日总加有功电能量（8费率）
    VIR_AE_CAL_DAY_9_14,     // 48字节，BIN X 10000（KWH），当日总加有功电能量（9~14费率）
    VIR_RE_CAL_DAY_Z,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（总）
    VIR_RE_CAL_DAY_1,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（尖）
    VIR_RE_CAL_DAY_2,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（峰）

    VIR_RE_CAL_DAY_3,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（平）
    VIR_RE_CAL_DAY_4,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（谷）
    VIR_RE_CAL_DAY_5,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（5费率）
    VIR_RE_CAL_DAY_6,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（6费率）
    VIR_RE_CAL_DAY_7,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（7费率）

    VIR_RE_CAL_DAY_8,        // 8字节，BIN X 10000（KWH），当日总加无功电能量（8费率）
    VIR_RE_CAL_DAY_9_14,     // 48字节，BIN X 10000（KWH），当日总加无功电能量（9~14费率）
    VIR_AP_MAX_DAY,          // 10字节，总加组日最大有功功率及其发生时间 值：4字节BIN X 100（KW）发生之间：6字节BCD（秒分时日月年）
    VIR_AP_MIN_DAY,          // 10字节，总加组日最小有功功率及其发生时间 值：4字节BIN X 100（KW）发生之间：6字节BCD（秒分时日月年）
    VIR_AP_ZERO_ACCT_DAY,    // 2字节，BIN，有功功率为零日累计时间

    VOLT_A_DisQuali_UpRate,  // 2字节，BIN，BIN X 1000，A相电压偏上不合格率
    VOLT_B_DisQuali_UpRate,  // 2字节，BIN，BIN X 1000，B相电压偏上不合格率
    VOLT_C_DisQuali_UpRate,  // 2字节，BIN，BIN X 1000，C相电压偏上不合格率
    VOLT_A_DisQuali_DnRate,  // 2字节，BIN，BIN X 1000，A相电压偏下不合格率
    VOLT_B_DisQuali_DnRate,  // 2字节，BIN，BIN X 1000，B相电压偏下不合格率

    VOLT_C_DisQuali_DnRate,  // 2字节，BIN，BIN X 1000，C相电压偏下不合格率
    VLT_A_LOST_ACCT,         // 2字节，A相电压失压日累计时间
    VLT_B_LOST_ACCT,         // 2字节，B相电压失压日累计时间
    VLT_C_LOST_ACCT,         // 2字节，C相电压失压日累计时间
    VLT_LOST_POS_AE,         // 8字节，BIN X 10000，日失压电量：正向有功总电能

    VLT_LOST_NEG_AE,         // 8字节，BIN X 10000，日失压电量：反向有功总电能
    VLT_LOST_POS_RE,         // 8字节，BIN X 10000，日失压电量：正向无功总电能
    VLT_LOST_NEG_RE,         // 8字节，BIN X 10000，日失压电量：反向无功总电能
    CB_SUCCESS_COUNT_DAY,    // 2字节，日抄表成功次数(485测量点）bin
    CB_TOTAL_COUNT_DAY,      // 2字节，日抄表次数 bin

    VIR_POWER_UO_ACCT_DAY,   // 2字节，总加组超功率定值的日累计电能量 bin
    VIR_POWER_UO_ACCEN_DAY,  // 8字节，总加组超功率定值的日累计电能量
    VIR_UI_MAX_REC_DAY,      // 10字节，总视在功率日最大值记录 值4byte bin 出现时间 6byte 秒分时日月年
    VIR_UI_MIN_REC_DAY,      // 10字节，总视在功率日最小值记录 值4byte bin 出现时间 6byte 秒分时日月年
    CUR_NO_BALANCE_VAL,      // 8字节，日电流不平衡最大值
                                // 最大值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）

    VOLT_NO_BALANCE_VAL,     // 8字节,日电压不平衡最大值
                                // 最大值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）
    Load_Rate_Max_ID,        // 8字节,日负载率最大值
                                // 最大值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）
    Load_Rate_Min_ID,        // 8字节,日负载率最小值
                                // 最小值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）
    Copper_Loss_Power_DAY_ID,// 8位字节longlong 铜损有功总电能示值
    Iron_Loss_Power_DAY_ID,  // 8位字节longlong铁损有功总电能示值

    DATA_CAL_DAY_COMPLEX_AREA_TEST,// 各地区增补的历史日数据（CALC_DATA_DAY）

DATA_CAL_DAY_END_ID,
};

/******************--6计量点日记录数据项ID--*********************/
enum data_rec_day
{
    VOLT_OU_REC_DAY=0,       // 15字节，电压越上限记录(出现时间,结束时间,极值,相值)
                                // 出现时间：6字节，BCD，秒分时日月年 结束时间：6字节，BCD，秒分时日月年
                                // 极值：2字节，BIN X 10（V） 相值：1字节，BIN 
    VOLT_OD_REC_DAY,         // 15字节，电压越下限记录(出现时间,结束时间,极值,相值)
    CUR_OU_REC_DAY,          // 15字节，电流越上限记录(出现时间,结束时间,极值,相值)
    A_SEVER_REC_DAY,         // 12字节，A相断相记录(出现时间,结束时间)
    B_SEVER_REC_DAY,         // 12字节，B相断相记录(出现时间,结束时间)

    C_SEVER_REC_DAY,         // 12字节，C相断相记录(出现时间,结束时间)
    CUR_ALM_REC_DAY,         // 15字节，过电流门限记录(出现时间,结束时间,极值,相值)
    CUR_NO_BALANCD_REC_DAY,  // 14字节，电流不平衡越限记录(越限发生时间,发生时的不平衡率,发生时的Ia,发生时的Ib,发生时的Ic）
                                // 越限发生时间：6字节，BCD，秒分时日月年 发生时的不平衡率：2字节，BCD
                                // 发生时的Ia：2字节，BIN 发生时的Ib：2字节，BIN 发生时的Ic：2字节，BIN
    VOLT_NO_BALANCE_REC_DAY, // 14字节，电压不平衡越限记录,格式同上
    TV_SEVER_REC_DAY,        // 13字节，TV断相记录
                                // 发生时间：6字节，BCD，秒分时日月年 相别标志：1字节，BIN
                                // 发生时的Ua/Uab：2字节，BIN 发生时的Ub：2字节，BIN 发生时的Uc/Ucb：2字节，BIN

    TV_LOST_VOLT_REC_DAY,    // 13字节，TV失压记录,格式同上
    ELEC_LOST_REC_DAY,       // 12字节，失电记录（来电时间,停电时间）
    LOST_VOLT_REC_DAY,       // 23字节，失压记录（日）失压相：1字节，BIN 
                                // 起始时间：6字节，BCD，秒分时日月年 恢复时间：6字节，BCD，秒分时日月年
                                // 未失压相的正向有功总电能：4字节，BIN 
                                // 未失压相的反向有功总电能：4字节，BIN 
                                // 失压相安培小时数：2字节，BIN

DATA_REC_DAY_END_ID,
};

/******************--7计量点月定点数据项ID--*********************/
enum data_point_mon
{
    RT_ER_READ_TIME_MON_FIX=0,// 6字节, 终端抄表时间：秒分时日月年
    RT_POS_AE_Z_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量 （总）
    RT_POS_AE_1_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量 （尖）
    RT_POS_AE_2_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量 （峰）
    RT_POS_AE_3_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量 （平）

    RT_POS_AE_4_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量 （谷）
    RT_POS_AE_5_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量费率5
    RT_POS_AE_6_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量费率6
    RT_POS_AE_7_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量费率7
    RT_POS_AE_8_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向有功电量费率8

    RT_POS_AE_EXT_FIX_MON,   // 8字节x6, BIN x 10000(kwh), 当前正向有功电量（9~14费率）
    RT_NEG_AE_Z_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量 （总）
    RT_NEG_AE_1_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量 （尖）
    RT_NEG_AE_2_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量 （峰）
    RT_NEG_AE_3_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量 （平）

    RT_NEG_AE_4_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量 （谷）
    RT_NEG_AE_5_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量费率5
    RT_NEG_AE_6_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量费率6
    RT_NEG_AE_7_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量费率7
    RT_NEG_AE_8_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向有功电量费率8

    RT_NEG_AE_EXT_FIX_MON,   // 8字节x6, BIN x 10000(kwh), 当前反向有功电量（9~14费率）
    RT_POS_RE_Z_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量 （总）
    RT_POS_RE_1_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量 （尖）
    RT_POS_RE_2_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量 （峰）
    RT_POS_RE_3_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量 （平）

    RT_POS_RE_4_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量 （谷）
    RT_POS_RE_5_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量费率5
    RT_POS_RE_6_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量费率6
    RT_POS_RE_7_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量费率7
    RT_POS_RE_8_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前正向无功电量费率8

    RT_POS_RE_EXT_FIX_MON,   // 8字节x6, BIN x 10000(kwh), 当前正向无功电量（9~14费率）
    RT_NEG_RE_Z_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量 （总）
    RT_NEG_RE_1_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量 （尖）
    RT_NEG_RE_2_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量 （峰）
    RT_NEG_RE_3_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量 （平）

    RT_NEG_RE_4_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量 （谷）
    RT_NEG_RE_5_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量费率5
    RT_NEG_RE_6_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量费率6
    RT_NEG_RE_7_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量费率7
    RT_NEG_RE_8_FIX_MON,     // 8字节, BIN x 10000(kwh), 当前反向无功电量费率8

    RT_NEG_RE_EXT_FIX_MON,   // 8字节x6, BIN x 10000(kwh), 当前反向无功电量（9~14费率）
    RT_FIRST_RE_Z_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （总）
    RT_FIRST_RE_1_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （尖）
    RT_FIRST_RE_2_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （峰）
    RT_FIRST_RE_3_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （平）

    RT_FIRST_RE_4_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （谷）
    RT_FIRST_RE_5_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量费率5
    RT_FIRST_RE_6_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量费率6
    RT_FIRST_RE_7_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量费率7
    RT_FIRST_RE_8_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第一象限无功电量费率8

    RT_FIRST_RE_EXT_FIX_MON, // 8字节x6, BIN x 10000(kwh), 当前第一象限无功电量（9~14费率）
    RT_FOURTH_RE_Z_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （总）
    RT_FOURTH_RE_1_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （尖）
    RT_FOURTH_RE_2_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （峰）
    RT_FOURTH_RE_3_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （平）

    RT_FOURTH_RE_4_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （谷）
    RT_FOURTH_RE_5_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量费率5
    RT_FOURTH_RE_6_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量费率6
    RT_FOURTH_RE_7_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量费率7
    RT_FOURTH_RE_8_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第四象限无功电量费率8

    RT_FOURTH_RE_EXT_FIX_MON,// 8字节x6, BIN x 10000(kwh), 当前第四象限无功电量（9~14费率）
    RT_SECOND_RE_Z_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （总）
    RT_SECOND_RE_1_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （尖）
    RT_SECOND_RE_2_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （峰）
    RT_SECOND_RE_3_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （平）

    RT_SECOND_RE_4_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （谷）
    RT_SECOND_RE_5_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量费率5
    RT_SECOND_RE_6_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量费率6
    RT_SECOND_RE_7_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量费率7
    RT_SECOND_RE_8_FIX_MON,  // 8字节, BIN x 10000(kwh), 当前第二象限无功电量费率8

    RT_SECOND_RE_EXT_FIX_MON,// 8字节x6, BIN x 10000(kwh), 当前第二象限无功电量（9~14费率）
    RT_THIRD_RE_Z_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （总）
    RT_THIRD_RE_1_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （尖）
    RT_THIRD_RE_2_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （峰）
    RT_THIRD_RE_3_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （平）

    RT_THIRD_RE_4_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （谷）
    RT_THIRD_RE_5_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量费率5
    RT_THIRD_RE_6_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量费率6
    RT_THIRD_RE_7_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量费率7
    RT_THIRD_RE_8_FIX_MON,   // 8字节, BIN x 10000(kwh), 当前第三象限无功电量费率8

    RT_THIRD_RE_EXT_FIX_MON, // 8字节x6, BIN x 10000(kwh), 当前第三象限无功电量（9~14费率）
    POS_AE_A_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), A相正向有功电量
    POS_AE_B_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), B相正向有功电量
    POS_AE_C_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), C相正向有功电量
    NEG_AE_A_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), A相反向有功电量

    NEG_AE_B_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), B相反向有功电量
    NEG_AE_C_ER_FIX_MON,     // 8字节, BIN x 10000(kwh), C相反向有功电量
    L_RE_A_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), A相感性无功电量
    L_RE_B_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), B相感性无功电量
    L_RE_C_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), C相感性无功电量

    C_RE_A_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), A相容性无功电量
    C_RE_B_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), B相容性无功电量
    C_RE_C_ER_FIX_MON,       // 8字节, BIN x 10000(kwh), C相容性无功电量
    CLEAR_DM_TIME,           // 4字节, BCD(分时日月,低字节->高字节), 需量清零时间
    FRZ_MON_AE_Z_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（总）

    FRZ_MON_AE_1_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（尖）
    FRZ_MON_AE_2_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（峰）
    FRZ_MON_AE_3_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（平）
    FRZ_MON_AE_4_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（谷）
    FRZ_MON_AE_5_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（费率5）

    FRZ_MON_AE_6_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（费率6）
    FRZ_MON_AE_7_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（费率7）
    FRZ_MON_AE_8_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（费率8）
    FRZ_MON_AE_EXT_FRZING,   // 8字节x6, BIN x 10000(kwh), 冻结日正向有功电能表的冻结数（9~14费率）
    FRZ_MON_RE_Z_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（总）

    FRZ_MON_RE_1_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（尖）
    FRZ_MON_RE_2_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（峰）
    FRZ_MON_RE_3_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（平）
    FRZ_MON_RE_4_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（谷）
    FRZ_MON_RE_5_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（费率5）

    FRZ_MON_RE_6_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（费率6）
    FRZ_MON_RE_7_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（费率7）
    FRZ_MON_RE_8_FRZING,     // 8字节, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（费率8）
    FRZ_MON_RE_EXT_FRZING,   // 8字节x6, BIN x 10000(kwh), 冻结日正向无功电能表的冻结数（9~14费率）
    FRZ_ST_END_TIME_MON,     // 12字节x2, BCD码(秒分时日月年, 低字节->高字节),冻结日的起止时间（起始时间，终止时间）

    PF_MON_SEC_TIME_FIX,     // 6字节, BIN, 月功率因数区段累计时间
    SEVER_READ_TIME_MON,     // 6字节, BCD(秒分时日月年, 低字节->高字节), 终端抄表时间：秒分时日月年
    Z_SEVER_ACC_MON,         // 4字节, 总断相累计次数
    P_SEVER_ACC_A_MON,       // 4字节, BIN, A相断相累计次数
    P_SEVER_ACC_B_MON,       // 4字节, BIN, B相断相累计次数

    P_SEVER_ACC_C_MON,       // 4字节, BIN, C相断相累计次数
    Z_SEVER_TACC_MON,        // 4字节, 总断相累计时间
    P_SEVER_TACC_A_MON,      // 4字节, BIN, A相断相累计时间
    P_SEVER_TACC_B_MON,      // 4字节, BIN, B相断相累计时间
    P_SEVER_TACC_C_MON,      // 4字节, BIN, C相断相累计时间

    Z_SEVER_START_TIME_MON,  // 4字节, BCD(分时日月, 低字节->高字节), 总最近一次断相起始时间
    P_SEVER_START_TIME_A_MON,// 4字节, BCD(分时日月, 低字节->高字节), A相最近断相起始时间
    P_SEVER_START_TIME_B_MON,// 4字节, BCD(分时日月, 低字节->高字节), B相最近断相起始时间
    P_SEVER_START_TIME_C_MON,// 4字节, BCD(分时日月, 低字节->高字节), C相最近断相起始时间
    Z_SEVER_END_TIME_MON,    // 4字节, BCD(分时日月, 低字节->高字节), 总最近一次断相结束时间

    P_SEVER_END_TIME_A_MON,  // 4字节, BCD(分时日月, 低字节->高字节), A相最近断相结束时间
    P_SEVER_END_TIME_B_MON,  // 4字节, BCD(分时日月, 低字节->高字节), B相最近断相结束时间
    P_SEVER_END_TIME_C_MON,  // 4字节, BCD(分时日月, 低字节->高字节), C相最近断相结束时间
    ZAD_POS_MAX_REC_FIX_MON, // 10字节, 正向总有功需量最大值记录
                                // 值4字节, BIN X 10000 (KW), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    POS_AD_MAX_REC1_FIX_MON, // 10字节, 费率段（尖）正向有功需量最大值记录,格式同上

    POS_AD_MAX_REC2_FIX_MON, // 10字节, 费率段（峰）正向有功需量最大值记录,格式同上
    POS_AD_MAX_REC3_FIX_MON, // 10字节, 费率段（平）正向有功需量最大值记录,格式同上
    POS_AD_MAX_REC4_FIX_MON, // 10字节, 费率段（谷）正向有功需量最大值记录,格式同上
    POS_AD_MAX_REC5_FIX_MON, // 10字节, 费率段5正向有功需量最大值记录,格式同上
    POS_AD_MAX_REC6_FIX_MON, // 10字节, 费率段6正向有功需量最大值记录,格式同上

    POS_AD_MAX_REC7_FIX_MON, // 10字节, 费率段7正向有功需量最大值记录,格式同上
    POS_AD_MAX_REC8_FIX_MON, // 10字节, 费率段8正向有功需量最大值记录,格式同上
    EXT_POS_AD_MAX_REC_FIX_MON,// 10字节x6, 费率段（9~14）正向有功需量最大值记录,格式同上
    ZAD_NEG_MAX_REC_FIX_MON, // 10字节, 反向总有功需量最大值记录
                                // 值4字节, BIN X 10000 (KW), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    NEG_AD_MAX_REC1_FIX_MON, // 10字节, 费率段（尖）反向有功需量最大值记录,格式同上

    NEG_AD_MAX_REC2_FIX_MON, // 10字节, 费率段（峰）反向有功需量最大值记录,格式同上
    NEG_AD_MAX_REC3_FIX_MON, // 10字节, 费率段（平）反向有功需量最大值记录,格式同上
    NEG_AD_MAX_REC4_FIX_MON, // 10字节, 费率段（谷）反向有功需量最大值记录,格式同上
    NEG_AD_MAX_REC5_FIX_MON, // 10字节, 费率段5反向有功需量最大值记录,格式同上
    NEG_AD_MAX_REC6_FIX_MON, // 10字节, 费率段6反向有功需量最大值记录,格式同上

    NEG_AD_MAX_REC7_FIX_MON, // 10字节, 费率段7反向有功需量最大值记录,格式同上
    NEG_AD_MAX_REC8_FIX_MON, // 10字节, 费率段8反向有功需量最大值记录,格式同上
    EXT_NEG_AD_MAX_REC_FIX_MON,// 10字节x6, 费率段（9~14）反向有功需量最大值记录,格式同上
    ZRD_POS_MAX_REC_FIX_MON, // 10字节, 正向总无功需量最大值记录,格式同上
    POS_RD_MAX_REC1_FIX_MON, // 10字节, 费率段（尖）正向无功需量最大值记录,格式同上

    POS_RD_MAX_REC2_FIX_MON, // 10字节, 费率段（峰）正向无功需量最大值记录,格式同上
    POS_RD_MAX_REC3_FIX_MON, // 10字节, 费率段（平）正向无功需量最大值记录,格式同上
    POS_RD_MAX_REC4_FIX_MON, // 10字节, 费率段（谷）正向无功需量最大值记录,格式同上
    POS_RD_MAX_REC5_FIX_MON, // 10字节, 费率段5正向无功需量最大值记录,格式同上
    POS_RD_MAX_REC6_FIX_MON, // 10字节, 费率段6正向无功需量最大值记录,格式同上

    POS_RD_MAX_REC7_FIX_MON, // 10字节, 费率段7正向无功需量最大值记录,格式同上
    POS_RD_MAX_REC8_FIX_MON, // 10字节, 费率段8正向无功需量最大值记录,格式同上
    EXT_POS_RD_MAX_REC_FIX_MON,// 10字节x6, 费率段（9~14）正向无功需量最大值记录,格式同上
    ZRD_NEG_MAX_REC_FIX_MON, // 10字节, 反向总无功需量最大值记录,格式同上
    NEG_RD_MAX_REC1_FIX_MON, // 10字节, 费率段（尖）反向无功需量最大值记录,格式同上

    NEG_RD_MAX_REC2_FIX_MON, // 10字节, 费率段（峰）反向无功需量最大值记录,格式同上
    NEG_RD_MAX_REC3_FIX_MON, // 10字节, 费率段（平）反向无功需量最大值记录,格式同上
    NEG_RD_MAX_REC4_FIX_MON, // 10字节, 费率段（谷）反向无功需量最大值记录,格式同上
    NEG_RD_MAX_REC5_FIX_MON, // 10字节, 费率段5反向无功需量最大值记录,格式同上
    NEG_RD_MAX_REC6_FIX_MON, // 10字节, 费率段6反向无功需量最大值记录,格式同上

    NEG_RD_MAX_REC7_FIX_MON, // 10字节, 费率段7反向无功需量最大值记录,格式同上
    NEG_RD_MAX_REC8_FIX_MON, // 10字节, 费率段8反向无功需量最大值记录,格式同上
    EXT_NEG_RD_MAX_REC_FIX_MON,// 10字节x6, 费率段（9~14）反向无功需量最大值记录,格式同上
    ZAD_POS_A_MAX_REC_FIX_MON,// 10字节, A相正向总有功需量最大值记录
                                // 值4字节, BIN X 10000 (KW), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    ZAD_POS_B_MAX_REC_FIX_MON,// 10字节, B相正向总有功需量最大值记录
                                // 值4字节, BIN X 10000 (KW), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    ZAD_POS_C_MAX_REC_FIX_MON,// 10字节, C相正向总有功需量最大值记录
                                // 值4字节, BIN X 10000 (KW), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VIR_AE_FIX_MON_Z,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（总）
    VIR_AE_FIX_MON_1,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（尖）
    VIR_AE_FIX_MON_2,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（峰）
    VIR_AE_FIX_MON_3,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（平）

    VIR_AE_FIX_MON_4,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（谷）
    VIR_AE_FIX_MON_5,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（5费率）
    VIR_AE_FIX_MON_6,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（6费率）
    VIR_AE_FIX_MON_7,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（7费率）
    VIR_AE_FIX_MON_8,        // 8字节, BIN x 10000(kwh), 月总加有功电能量（8费率）

    VIR_AE_FIX_MON_9_14,     // 8*6字节, BIN x 10000(kwh), 月总加有功电能量（9~14费率）
    VIR_RE_FIX_MON_Z,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（总）
    VIR_RE_FIX_MON_1,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（尖）
    VIR_RE_FIX_MON_2,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（峰）
    VIR_RE_FIX_MON_3,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（平）

    VIR_RE_FIX_MON_4,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（谷）
    VIR_RE_FIX_MON_5,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（5费率）
    VIR_RE_FIX_MON_6,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（6费率）
    VIR_RE_FIX_MON_7,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（7费率）
    VIR_RE_FIX_MON_8,        // 8字节, BIN x 10000(kwh), 月总加无功电能量（8费率）

    VIR_RE_FIX_MON_9_14,     // 8*6字节, BIN x 10000(kwh), 月总加无功电能量（9~14费率）
    ZAD_POS_MAX_REC_FIX_MON_CAL,// 月正向总有功需量最大值统计记录
    MET_PROGRAM_CNT_FIX,     // 3个字节, BCD编程开关次数 
    MET_LAST_PROGRAM_TIME_FIX,// 6个字节,分时日月年,最近一次编程时间 
    MET_COVER_OPEN_CNT_FIX,  // 3个字节,BIN电能表尾盖打开次数 

    MET_LAST_COVER_OPEN_TIME_FIX,// 6个字节,分时日月年,最近一次尾盖打开时间 
    MET_CLOCK_MODIF_CNT_FIX, // 3个字节,电能表时钟修改次数-
    MET_LAST_CLOCK_MODIF_TIME_FIX,// 6个字节，电能表最近一次时钟修改时间
    MET_CLOCKCHIP_MODIF_CNT_FIX,// 3个字节,电能表时段参数修改次数-
    MET_LAST_CLOCKCHIP_TIME_FIX,// 6个字节，最近一次电能表时段参数修改时间-

    MET_PAY_ELECTR_TIME_FIX, // 2个字节, BCD,购电次数-*/
    MET_REMAIN_MONEY_FIX,    // 5个字节,BCD,剩余金额-
    MET_TOTAL_USED_MONEY_FIX,// 5个字节,BCD,累计购电金额-
    MET_REMAIN_ELETRIC_FIX,  // 4个字节,BCD,剩余电量-
    MET_OVERFRAFT_ELETRIC_FIX,// 4个字节,BCD,透支电量

    MET_TOTAL_PAYED_ELETRIC_FIX,// 4个字节,BCD,累计购电量
    MET_TICK_FACK_ELETRIC_FIX,// 4个字节,BCD,赊欠门限电量
    MET_ALARM_ELETRIC_FIX,   // 4个字节,BCD,报警电量
    MET_FAULT_ELETRIC_FIX,   // 4个字节,BCD,故障电量
    MET_BALANCE_AE_Z_ER_ID_FIX,// 8字节, BIN x 10000, 已结有功总电能

    MET_BALANCE_AE_1_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率1已结有功总电能
    MET_BALANCE_AE_2_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率2已结有功总电能
    MET_BALANCE_AE_3_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率3已结有功总电能
    MET_BALANCE_AE_4_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率4已结有功总电能
    MET_BALANCE_AE_5_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率5已结有功总电能

    MET_BALANCE_AE_6_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率6已结有功总电能
    MET_BALANCE_AE_7_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率7已结有功总电能
    MET_BALANCE_AE_8_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率8已结有功总电能
    MET_BALANCE_AE_EXT_ER_ID_FIX,// 8字节x6, BIN x 10000, (当前)费率（9—14）已结有功总电能
    MET_UNBALANCE_AE_Z_ER_ID_FIX,// 8字节, BIN x 10000, 未结有功总电能

    MET_UNBALANCE_AE_1_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率1未结有功总电能
    MET_UNBALANCE_AE_2_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率2未结有功总电能
    MET_UNBALANCE_AE_3_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率3未结有功总电能
    MET_UNBALANCE_AE_4_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率4未结有功总电能
    MET_UNBALANCE_AE_5_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率5未结有功总电能

    MET_UNBALANCE_AE_6_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率6未结有功总电能
    MET_UNBALANCE_AE_7_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率7未结有功总电能
    MET_UNBALANCE_AE_8_ER_ID_FIX,// 8字节, BIN x 10000, (当前)费率8未结有功总电能
    MET_UNBALANCE_AE_EXT_ER_ID_FIX,// 8字节x6, BIN x 10000, (当前)费率（9—14）未结有功总电能
    RT_POS_AE_Z_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前正向有功电量 （总）

    RT_POS_AE_1_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前正向有功电量 （尖）
    RT_POS_AE_2_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前正向有功电量 （峰）
    RT_POS_AE_3_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前正向有功电量 （平）
    RT_POS_AE_4_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前正向有功电量 （谷）
    RT_FIRST_RE_Z_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （总）

    RT_FIRST_RE_1_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （尖）
    RT_FIRST_RE_2_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （峰）
    RT_FIRST_RE_3_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （平）
    RT_FIRST_RE_4_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第一象限无功电量 （谷）
    RT_SECOND_RE_Z_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （总）

    RT_SECOND_RE_1_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （尖）
    RT_SECOND_RE_2_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （峰）
    RT_SECOND_RE_3_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （平）
    RT_SECOND_RE_4_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第二象限无功电量 （谷）
    RT_THIRD_RE_Z_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （总）

    RT_THIRD_RE_1_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （尖）
    RT_THIRD_RE_2_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （峰）
    RT_THIRD_RE_3_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （平）
    RT_THIRD_RE_4_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第三象限无功电量 （谷）
    RT_FOURTH_RE_Z_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （总）

    RT_FOURTH_RE_1_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （尖）
    RT_FOURTH_RE_2_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （峰）
    RT_FOURTH_RE_3_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （平）
    RT_FOURTH_RE_4_READ_DAY_FIX_MON,// 8字节, BIN x 10000(kwh), 当前第四象限无功电量 （谷）
    ZAD_POS_MAX_REC_READ_DAY_FIX_MON,// 10字节, 正向总有功需量最大值记录:
                                // #1: 4字节, BIN X 10000 (KW), 最大值
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 最值出现时间

    POS_AD_MAX_REC1_READ_DAY_FIX_MON,// 10字节, 费率段（尖）正向有功需量最大值记录:
                                // #1: 4字节, BIN X 10000 (KW), 最大值;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 最值出现时间
    POS_AD_MAX_REC2_READ_DAY_FIX_MON,// 10字节, 费率段（峰）正向有功需量最大值记录:
                                // #1: 4字节, BIN X 10000 (KW), 最大值;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 最值出现时间
    POS_AD_MAX_REC3_READ_DAY_FIX_MON,// 10字节, 费率段（平）正向有功需量最大值记录:
                                // #1: 4字节, BIN X 10000 (KW), 最大值;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 最值出现时间
    POS_AD_MAX_REC4_READ_DAY_FIX_MON,// 10字节, 费率段（谷）正向有功需量最大值记录:
                                // #1: 4字节, BIN X 10000 (KW), 最大值;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 最值出现时间
    TIME_ZERO_MT_CLOCK_MON1, // 6 字节，BCD，抄表时间(秒、分、时、日、月、年)

    TIME_ZERO_POS_AE_Z_MON1, // 8 字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量 （谷）

    TIME_ZERO_POS_AE_5_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量费率5 
    TIME_ZERO_POS_AE_6_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON1, // 8 字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量费率8 
    TIME_ZERO_POS_AE_EXT_MON1,// 48字节，BIN X 10000 （kWh），月冻结第一时区冻结正向有功电量（9~14费率）

    TIME_ZERO_MT_CLOCK_MON2, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON2, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量 （平）

    TIME_ZERO_POS_AE_4_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量费率5 
    TIME_ZERO_POS_AE_6_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON2, // 8字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量费率8 

    TIME_ZERO_POS_AE_EXT_MON2,// 48字节，BIN X 10000 （kWh），月冻结第二时区冻结正向有功电量（9~14费率）
    TIME_ZERO_MT_CLOCK_MON3, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON3, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量 （峰）

    TIME_ZERO_POS_AE_3_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量费率5
    TIME_ZERO_POS_AE_6_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量费率7

    TIME_ZERO_POS_AE_8_MON3, // 8字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量费率8
    TIME_ZERO_POS_AE_EXT_MON3,// 48字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量（9~14费率）
    TIME_ZERO_MT_CLOCK_MON4, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON4, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量 （尖）

    TIME_ZERO_POS_AE_2_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量费率5 
    TIME_ZERO_POS_AE_6_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量费率6

    TIME_ZERO_POS_AE_7_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON4, // 8字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量费率8 
    TIME_ZERO_POS_AE_EXT_MON4,// 48字节，BIN X 10000 （kWh），月冻结第四时区冻结正向有功电量（9~14费率）
    TIME_ZERO_MT_CLOCK_MON5, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON5, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）

    TIME_ZERO_POS_AE_1_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量费率5 

    TIME_ZERO_POS_AE_6_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量费率6 
    TIME_ZERO_POS_AE_7_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON5, // 8字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量费率8 
    TIME_ZERO_POS_AE_EXT_MON5,// 48字节，BIN X 10000 （kWh），月冻结第五时区冻结正向有功电量（9~14费率）
    TIME_ZERO_MT_CLOCK_MON6, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)

    TIME_ZERO_POS_AE_Z_MON6, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）-
    TIME_ZERO_POS_AE_1_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量 （谷）

    TIME_ZERO_POS_AE_5_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量费率5 
    TIME_ZERO_POS_AE_6_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON6, // 8字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量费率8 
    TIME_ZERO_POS_AE_EXT_MON6,// 48字节，BIN X 10000 （kWh），月冻结第六时区冻结正向有功电量（9~14费率）

    TIME_ZERO_MT_CLOCK_MON7, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON7, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量 （峰）
    TIME_ZERO_POS_AE_3_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量 （平）

    TIME_ZERO_POS_AE_4_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量费率5
    TIME_ZERO_POS_AE_6_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量费率7 
    TIME_ZERO_POS_AE_8_MON7, // 8字节，BIN X 10000 （kWh），月冻结第七时区冻结正向有功电量费率8 

    TIME_ZERO_POS_AE_EXT_MON7,// 48字节，BIN X 10000 （kWh），月冻结第三时区冻结正向有功电量（9~14费率）
    TIME_ZERO_MT_CLOCK_MON8, // 6字节，BCD，抄表时间(秒、分、时、日、月、年)
    TIME_ZERO_POS_AE_Z_MON8, // 8字节，BIN X 10000 （kWh），当前正向有功电量（总）
    TIME_ZERO_POS_AE_1_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量 （尖）
    TIME_ZERO_POS_AE_2_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量 （峰）

    TIME_ZERO_POS_AE_3_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量 （平）
    TIME_ZERO_POS_AE_4_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量 （谷）
    TIME_ZERO_POS_AE_5_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量费率5 
    TIME_ZERO_POS_AE_6_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量费率6
    TIME_ZERO_POS_AE_7_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量费率7

    TIME_ZERO_POS_AE_8_MON8, // 8字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量费率8
    TIME_ZERO_POS_AE_EXT_MON8,// 48字节，BIN X 10000 （kWh），月冻结第八时区冻结正向有功电量（9~14费率）
    MON_POINT_DATA_SAVE_FLAG,// 1字节,用于07表抄日冻结结数据是否转存为月冻结数据，1为已经转存，0为没有转存
    MON_CHECK_MTR_BEFORE_TIME,//  6个字节，BCD(秒分时日月--星期年, 低字节->高字节),校时之前的时间
    MON_CHECK_MTR_AFTER_TIME,//  6个字节，BCD(秒分时日月--星期年, 低字节->高字节),校时之后的时间

    MON_Vol_Imbalance_95Probability_ID,// 2字节,BCD,月冻结电压不平衡度95%概率值
    MON_Cur_Imbalance_95Probability_ID,// 2字节,BCD,月冻结电流不平衡度95%概率值 

DATA_POINT_MON_END_ID,
};

/******************--8计量点月计算数据项ID--*********************/
enum data_cal_mon
{
    F_OVER_AE_MON=0,         // 8字节, BIN x 10000(kwh), 峰超电量(月)
    G_BELOW_AE_MON,          // 8字节, BIN x 10000(kwh), 谷欠电量(月)
    GSEG_AE_RATIO_MON,       // 2字节, BIN x 100%, 月谷段用电比
    POS_AP_MAX_MON,          // 10字节, 总正向有功功率最大值记录(月),格式同上
    NEG_AP_MAX_MON,          // 10字节, 总反向有功功率最大值记录,格式同上

    A_PAP_MAX_REC_MON,       // 10字节, A相有功功率最大值记录（月）
                                // 值4字节, BIN x 10000(kwh) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    B_PAP_MAX_REC_MON,       // 10字节, B相有功功率最大值记录(月),格式同上
    C_PAP_MAX_REC_MON,       // 10字节, C相有功功率最大值记录(月),格式同上
    POS_AP_MIN_MON,          // 10字节, 总正向有功功率最小值记录,格式同上
    NEG_AP_MIN_MON,          // 10字节, 总反向有功功率最小值记录,格式同上

    A_PAP_MIN_REC_MON,       // 10字节, A相有功功率最小值记录（月）,格式同上
    B_PAP_MIN_REC_MON,       // 10字节, B相有功功率最小值记录（月）,格式同上
    C_PAP_MIN_REC_MON,       // 10字节, C相有功功率最小值记录（月）,格式同上
    A_PCRP_MAX_REC_MON,      // 10字节, A相容性无功功率最大值记录,格式同上
    B_PCRP_MAX_REC_MON,      // 10字节, B相容性无功功率最大值记录,格式同上

    C_PCRP_MAX_REC_MON,      // 10字节, C相容性无功功率最大值记录,格式同上
    A_PLRP_MAX_REC_MON,      // 10字节, A相感性无功功率最大值记录,格式同上
    B_PLRP_MAX_REC_MON,      // 10字节, B相感性无功功率最大值记录,格式同上
    C_PLRP_MAX_REC_MON,      // 10字节, C相感性无功功率最大值记录,格式同上
    ZRP_MAX_REC_MON,         // 10字节, 总无功功率最大值记录
                                // 值4字节, BIN x 10000(kwh) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    PRP_MAX_REC_A_MON,       // 10字节, A相无功功率最大值记录,格式同上
    PRP_MAX_REC_B_MON,       // 10字节, B相无功功率最大值记录,格式同上
    PRP_MAX_REC_C_MON,       // 10字节, C相无功功率最大值记录,格式同上
    ZRP_MIN_REC_MON,         // 10字节, 总无功功率最小值记录,格式同上
    PRP_MIN_REC_A_MON,       // 10字节, A相无功功率最小值记录,格式同上

    PRP_MIN_REC_B_MON,       // 10字节, B相无功功率最小值记录,格式同上
    PRP_MIN_REC_C_MON,       // 10字节, C相无功功率最小值记录,格式同上
    F_SEG_MAX_P_MON,         // 10字节, 峰段最大功率记录
    G_SEG_MIN_P_MON,         // 10字节, 谷段最小功率记录
    RT_POS_AE_Z_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月正向总有功电量

    RT_POS_AE_1_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（尖）正向有功电量
    RT_POS_AE_2_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（峰）正向有功电量
    RT_POS_AE_3_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（平）正向有功电量
    RT_POS_AE_4_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（谷）正向有功电量
    RT_POS_AE_5_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段5正向有功电量

    RT_POS_AE_6_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段6正向有功电量
    RT_POS_AE_7_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段7正向有功电量
    RT_POS_AE_8_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段8正向有功电量
    RT_POS_AE_EXT_ER_MON,    // 8字节x6, BIN x 10000(kwh), 当前月9~14费率段正向有功电量
    RT_NEG_AE_Z_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月反向总有功电量

    RT_NEG_AE_1_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（尖）反向有功电量
    RT_NEG_AE_2_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（峰）反向有功电量
    RT_NEG_AE_3_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（平）反向有功电量
    RT_NEG_AE_4_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（谷）反向有功电量
    RT_NEG_AE_5_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段5反向有功电量

    RT_NEG_AE_6_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段6反向有功电量
    RT_NEG_AE_7_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段7反向有功电量
    RT_NEG_AE_8_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段8反向有功电量
    RT_NEG_AE_EXT_ER_MON,    // 8字节x6, BIN x 10000(kwh), 当前月9~14费率段反向有功电量
    RT_POS_RE_Z_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月正向总无功电量

    RT_POS_RE_1_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（尖）正向无功电量
    RT_POS_RE_2_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（峰）正向无功电量
    RT_POS_RE_3_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（平）正向无功电量
    RT_POS_RE_4_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（谷）正向无功电量
    RT_POS_RE_5_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段5正向无功电量

    RT_POS_RE_6_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段6正向无功电量
    RT_POS_RE_7_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段7正向无功电量
    RT_POS_RE_8_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段8正向无功电量
    RT_POS_RE_EXT_ER_MON,    // 8字节x6, BIN x 10000(kwh), 当前月9~14费率段正向无功电量
    RT_NEG_RE_Z_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月反向总无功电量

    RT_NEG_RE_1_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（尖）反向无功电量
    RT_NEG_RE_2_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（峰）反向无功电量
    RT_NEG_RE_3_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（平）反向无功电量
    RT_NEG_RE_4_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段（谷）反向无功电量
    RT_NEG_RE_5_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段5反向无功电量

    RT_NEG_RE_6_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段6反向无功电量
    RT_NEG_RE_7_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段7反向无功电量
    RT_NEG_RE_8_ER_MON,      // 8字节, BIN x 10000(kwh), 当前月费率段8反向无功电量
    RT_NEG_RE_EXT_ER_MON,    // 8字节x6, BIN x 10000(kwh), 当前月9~14费率段反向无功电量
    RT_FIRST_RE_Z_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限总无功电量

    RT_FIRST_RE_1_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段（尖）无功电量
    RT_FIRST_RE_2_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段（峰）无功电量
    RT_FIRST_RE_3_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段（平）无功电量
    RT_FIRST_RE_4_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段（谷）无功电量
    RT_FIRST_RE_5_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段5无功电量

    RT_FIRST_RE_6_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段6无功电量
    RT_FIRST_RE_7_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段7无功电量
    RT_FIRST_RE_8_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第一象限费率段8无功电量
    RT_FIRST_RE_EXT_ER_MON,  // 8字节x6, BIN x 10000(kwh), 当前月第一象限9~14费率段无功电量
    RT_FOURTH_RE_Z_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限总无功电量

    RT_FOURTH_RE_1_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段（尖）无功电量
    RT_FOURTH_RE_2_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段（峰）无功电量
    RT_FOURTH_RE_3_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段（平）无功电量
    RT_FOURTH_RE_4_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段（谷）无功电量
    RT_FOURTH_RE_5_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段5无功电量

    RT_FOURTH_RE_6_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段6无功电量
    RT_FOURTH_RE_7_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段7无功电量
    RT_FOURTH_RE_8_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第四象限费率段8无功电量
    RT_FOURTH_RE_EXT_ER_MON, // 8字节x6, BIN x 10000(kwh), 当前月第四象限9~14费率段无功电量
    RT_SECOND_RE_Z_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限总无功电量

    RT_SECOND_RE_1_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段（尖）无功电量
    RT_SECOND_RE_2_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段（峰）无功电量
    RT_SECOND_RE_3_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段（平）无功电量
    RT_SECOND_RE_4_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段（谷）无功电量
    RT_SECOND_RE_5_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段5无功电量

    RT_SECOND_RE_6_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段6无功电量
    RT_SECOND_RE_7_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段7无功电量
    RT_SECOND_RE_8_ER_MON,   // 8字节, BIN x 10000(kwh), 当前月第二象限费率段8无功电量
    RT_SECOND_RE_EXT_ER_MON, // 8字节x6, BIN x 10000(kwh), 当前月第二象限9~14费率段无功电量
    RT_THIRD_RE_Z_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限总无功电量

    RT_THIRD_RE_1_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段（尖）无功电量
    RT_THIRD_RE_2_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段（峰）无功电量
    RT_THIRD_RE_3_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段（平）无功电量
    RT_THIRD_RE_4_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段（谷）无功电量
    RT_THIRD_RE_5_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段5无功电量

    RT_THIRD_RE_6_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段6无功电量
    RT_THIRD_RE_7_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段7无功电量
    RT_THIRD_RE_8_ER_MON,    // 8字节, BIN x 10000(kwh), 当前月第三象限费率段8无功电量
    RT_THIRD_RE_EXT_ER_MON,  // 8字节x6, BIN x 10000(kwh), 当前月第三象限9~14费率段无功电量
    ZAD_POS_MAX_REC_MON,     // 10字节, 正向总有功需量最大值记录 值4字节, BIN x 10000(kw), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    POS_AD_1_MAX_REC_MON,    // 10字节, 费率段（尖）正向有功需量最大值记录,格式同上
    POS_AD_2_MAX_REC_MON,    // 10字节, 费率段（峰）正向有功需量最大值记录,格式同上
    POS_AD_3_MAX_REC_MON,    // 10字节, 费率段（平）正向有功需量最大值记录,格式同上
    POS_AD_4_MAX_REC_MON,    // 10字节, 费率段（谷）正向有功需量最大值记录,格式同上
    POS_AD_5_MAX_REC_MON,    // 10字节, 费率段5正向有功需量最大值记录,格式同上

    POS_AD_6_MAX_REC_MON,    // 10字节, 费率段6正向有功需量最大值记录,格式同上
    POS_AD_7_MAX_REC_MON,    // 10字节, 费率段7正向有功需量最大值记录,格式同上
    POS_AD_8_MAX_REC_MON,    // 10字节, 费率段8正向有功需量最大值记录,格式同上
    EXT_POS_AD_MAX_REC_MON,  // 10字节x6, 费率段（9~14）正向有功需量最大值记录,格式同上
    ZAD_NEG_MAX_REC_MON,     // 10字节, 反向总有功需量最大值记录,格式同上

    NEG_AD_1_MAX_REC_MON,    // 10字节, 费率段（尖）反向有功需量最大值记录,格式同上
    NEG_AD_2_MAX_REC_MON,    // 10字节, 费率段（峰）反向有功需量最大值记录,格式同上
    NEG_AD_3_MAX_REC_MON,    // 10字节, 费率段（平）反向有功需量最大值记录,格式同上
    NEG_AD_4_MAX_REC_MON,    // 10字节, 费率段（谷）反向有功需量最大值记录,格式同上
    NEG_AD_5_MAX_REC_MON,    // 10字节, 费率段5反向有功需量最大值记,格式同上

    NEG_AD_6_MAX_REC_MON,    // 10字节, 费率段65反向有功需量最大值记,格式同上
    NEG_AD_7_MAX_REC_MON,    // 10字节, 费率段7反向有功需量最大值记,格式同上
    NEG_AD_8_MAX_REC_MON,    // 10字节, 费率段8反向有功需量最大值记 值4字节, BIN x 10000(kw), 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    EXT_NEG_AD_MAX_REC_MON,  // 10字节x6, 费率段（9~14）反向有功需量最大值记录,格式同上
    ZRD_POS_MAX_REC_MON,     // 10字节, 正向总无功需量最大值记录,格式同上

    POS_RD_1_MAX_REC_MON,    // 10字节, 费率段（尖）正向无功需量最大值记录,格式同上
    POS_RD_2_MAX_REC_MON,    // 10字节, 费率段（峰）正向无功需量最大值记录,格式同上
    POS_RD_3_MAX_REC_MON,    // 10字节, 费率段（平）正向无功需量最大值记录,格式同上
    POS_RD_4_MAX_REC_MON,    // 10字节, 费率段（谷）正向无功需量最大值记录,格式同上
    POS_RD_5_MAX_REC_MON,    // 10字节, 费率段5正向无功需量最大值记录,格式同上

    POS_RD_6_MAX_REC_MON,    // 10字节, 费率段6正向无功需量最大值记录,格式同上
    POS_RD_7_MAX_REC_MON,    // 10字节, 费率段7正向无功需量最大值记录,格式同上
    POS_RD_8_MAX_REC_MON,    // 10字节, 费率段8正向无功需量最大值记录,格式同上
    EXT_POS_RD_MAX_REC_MON,  // 10字节x6, 费率段（9~14）正向无功需量最大值记录,格式同上
    ZRD_NEG_MAX_REC_MON,     // 10字节, 反向总无功需量最大值记录,格式同上

    NEG_RD_1_MAX_REC_MON,    // 10字节, 费率段（尖）反向无功需量最大值记录,格式同上
    NEG_RD_2_MAX_REC_MON,    // 10字节, 费率段（峰）反向无功需量最大值记录,格式同上
    NEG_RD_3_MAX_REC_MON,    // 10字节, 费率段（平）反向无功需量最大值记录,格式同上
    NEG_RD_4_MAX_REC_MON,    // 10字节, 费率段（谷）反向无功需量最大值记录,格式同上
    NEG_RD_5_MAX_REC_MON,    // 10字节, 费率段5反向无功需量最大值记录,格式同上

    NEG_RD_6_MAX_REC_MON,    // 10字节, 费率段6反向无功需量最大值记录,格式同上
    NEG_RD_7_MAX_REC_MON,    // 10字节, 费率段7反向无功需量最大值记录,格式同上
    NEG_RD_8_MAX_REC_MON,    // 10字节, 费率段8反向无功需量最大值记录,格式同上
    EXT_NEG_RD_MAX_REC_MON,  // 10字节x6, 费率段（9~14）反向无功需量最大值记录,格式同上
    A_VOLT_HIGH_TACC_MON,    // 2字节, BIN [A]相, A相电压越上限累计时间(月)

    B_VOLT_HIGH_TACC_MON,    // 2字节, BIN [B]相, B相电压越上限累计时间(月)
    C_VOLT_HIGH_TACC_MON,    // 2字节, BIN [C]相, C相电压越上限累计时间(月)
    A_VOLT_LOW_TACC_MON,     // 2字节, BIN [A]相, A相电压越下限累计时间(月)
    B_VOLT_LOW_TACC_MON,     // 2字节, BIN [B]相, B相电压越下限累计时间(月)
    C_VOLT_LOW_TACC_MON,     // 2字节, BIN [C]相, C相电压越下限累计时间(月)

    A_VOLT_OU_TACC_MON,      // 2字节, BIN [A]相, A相电压越异常上限累计时间(月)
    B_VOLT_OU_TACC_MON,      // 2字节, BIN [B]相, B相电压越异常上限累计时间(月)
    C_VOLT_OU_TACC_MON,      // 2字节, BIN [C]相, C相电压越异常上限累计时间(月)
    A_VOLT_OD_TACC_MON,      // 2字节, BIN [A]相, A相电压越异常下限累计时间(月)
    B_VOLT_OD_TACC_MON,      // 2字节, BIN [B]相, B相电压越异常下限累计时间(月)

    C_VOLT_OD_TACC_MON,      // 2字节, BIN [C]相, C相电压越异常下限累计时间(月)
    A_CUR_OVER_TIME_MON,     // 2字节, BIN, A相电流越上限累计时间（月）
    B_CUR_OVER_TIME_MON,     // 2字节, BIN, B相电流越上限累计时间（月）
    C_CUR_OVER_TIME_MON,     // 2字节, BIN, C相电流越上限累计时间（月）
    Z_CUR_OVER_TIME_MON,     // 2字节, BIN, 零序电流越上限累计时间（月）

    A_CUR_OU_TACC_MON,       // 2字节, A相电流越异常上限累计时间(月)
    B_CUR_OU_TACC_MON,       // 2字节, B相电流越异常上限累计时间(月)
    C_CUR_OU_TACC_MON,       // 2字节, C相电流越异常上限累计时间(月)
    CUR_OU_TACC_Z_MON,       // 2字节, 总电流越异常上限累计时间（月）
    A_VOLT_REGULRA_TACC_MON, // 2字节, BIN [A]相, A相电压合格日累计时间(月)

    B_VOLT_REGULRA_TACC_MON, // 2字节, BIN [B]相, B相电压合格日累计时间(月)
    C_VOLT_REGULRA_TACC_MON, // 2字节, BIN [C]相, C相电压合格日累计时间(月)
    VOLT_OU_TACC_Z_MON,      // 2字节, BIN, 总电压越异常上限累计时间(月)
    VOLT_OD_TACC_Z_MON,      // 2字节, BIN, 总电压越异常下限累计时间(月)
    VOLT_HIGH_TACC_Z_MON,    // 2字节, BIN, 总电压越上限累计时间(月)

    VOLT_LOW_TACC_Z_MON,     // 2字节, BIN, 总电压越下限累计时间(月)
    VOLT_REGULRA_TACC_Z_MON, // 2字节, BIN, 总电压合格月累计时间（月）
    CAL_MON_A_VOLT_N_RATE,   // 4字节，BIN X 100%，A相电压合格率
    CAL_MON_A_VOLT_O_RATE,   // 4字节，BIN X 100%，A相电压越上限率
    CAL_MON_A_VOLT_D_RATE,   // 4字节，BIN X 100%，A相电压越下限率

    CAL_MON_B_VOLT_N_RATE,   // 4字节，BIN X 100%，B相电压合格率
    CAL_MON_B_VOLT_O_RATE,   // 4字节，BIN X 100%，B相电压越上限率
    CAL_MON_B_VOLT_D_RATE,   // 4字节，BIN X 100%，B相电压越下限率
    CAL_MON_C_VOLT_N_RATE,   // 4字节，BIN X 100%，C相电压合格率
    CAL_MON_C_VOLT_O_RATE,   // 4字节，BIN X 100%，C相电压越上限率

    CAL_MON_C_VOLT_D_RATE,   // 4字节，BIN X 100%，C相电压越下限率
    VOLT_MAX_REC_A_MON,      // 8字节, A相电压最大值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VOLT_MIN_REC_A_MON,      // 8字节, A相电压最小值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VOLT_MAX_REC_B_MON,      // 8字节, B相电压最大值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VOLT_MIN_REC_B_MON,      // 8字节, B相电压最小值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    VOLT_MAX_REC_C_MON,      // 8字节, C相电压最大值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VOLT_MIN_REC_C_MON,      // 8字节, C相电压最小值记录 值2字节, BIN x 10(v) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    A_CUR_MAX_REC_MON,       // 10字节, A相电流最大值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    A_CUR_MIN_REC_MON,       // 10字节, A相电流最小值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    B_CUR_MAX_REC_MON,       // 10字节, B相电流最大值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    B_CUR_MIN_REC_MON,       // 10字节, B相电流最小值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    C_CUR_MAX_REC_MON,       // 10字节, C相电流最大值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    C_CUR_MIN_REC_MON,       // 10字节, C相电流最小值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    G_CUR_MAX_REC_MON,       // 10字节, 零相电流最大值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    G_CUR_MIN_REC_MON,       // 10字节, 零相电流最小值记录 值4字节, BIN x 1000(A) 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)

    AVER_A_VOLT_MON,         // 2字节, BIN x 10(v), A相平均电压
    AVER_B_VOLT_MON,         // 2字节, BIN x 10(v), B相平均电压
    AVER_C_VOLT_MON,         // 2字节, BIN x 10(v), C相平均电压
    CUR_NO_BALANCE_ACCT_MON, // 2字节, BIN, 电流不平衡月累计时间
    VOLT_NO_BALANCE_ACCT_MON,// 2字节, BIN, 电压不平衡月累计时间

    PF_Z_MON,                // 2字节, BIN x 100%, 月功率因数（总）月功率因数=月有功电量/月有功电量与无功电量平方和开根方
    PF_1_MON,                // 2字节, BIN x 100%, 月功率因数（尖）
    PF_2_MON,                // 2字节, BIN x 100%, 月功率因数（峰）
    PF_3_MON,                // 2字节, BIN x 100%, 月功率因数（平）
    PF_4_MON,                // 2字节, BIN x 100%, 月功率因数（谷）

    PF_5_MON,                // 2字节, BIN x 100%, 费率段5月功率因数
    PF_6_MON,                // 2字节, BIN x 100%, 费率段6月功率因数
    PF_7_MON,                // 2字节, BIN x 100%, 费率段7月功率因数
    PF_8_MON,                // 2字节, BIN x 100%, 费率段8月功率因数
    PF_EXT_MON,              // 2字节x6, BIN x 100%, 扩展费率段（9~14）月功率因数

    LF_Z_MON,                // 2字节, BIN x 100%, 月负荷率（总）
    LF_1_MON,                // 2字节, BIN x 100%, 月负荷率（尖）
    LF_2_MON,                // 2字节, BIN x 100%, 月负荷率（峰）
    LF_3_MON,                // 2字节, BIN x 100%, 月负荷率（平）
    LF_4_MON,                // 2字节, BIN x 100%, 月负荷率（谷）

    LF_5_MON,                // 2字节, BIN x 100%, 费率段5月负荷率
    LF_6_MON,                // 2字节, BIN x 100%, 费率段6月负荷率
    LF_7_MON,                // 2字节, BIN x 100%, 费率段7月负荷率
    LF_8_MON,                // 2字节, BIN x 100%, 费率段8月负荷率
    LF_EXT_MON,              // 2字节x6, BIN x 100%, 扩展费率段（9~14）月负荷率

    POW_OUT_ACC_MON,         // 2字节, 自然月功率越限次数
    POW_ZERO_TACC_MON,       // 2字节, 自然月功率为零时间
    A_POW_ZERO_TACC_MON,     // 2字节, BIN, A相有功功率为零时间
    B_POW_ZERO_TACC_MON,     // 2字节, BIN, B相有功功率为零时间
    C_POW_ZERO_TACC_MON,     // 2字节, BIN, C相有功功率为零时间

    UI_OU_TIME_MON,          // 2字节, BIN, 视在功率越上上限累计时间（月）
    UI_OVER_TIME_MON,        // 2字节, BIN, 视在功率越上限累计时间（月）
    A_AD_MAX_REC_MON,        // 10字节, A相有功最大需量及发生时间 值4字节, BIN x 10000(kw) 发生时间6字节, BCD(秒分时日月年, 低字节->高字节)
    B_AD_MAX_REC_MON,        // 10字节, B相有功最大需量及发生时间 值4字节, BIN x 10000(kw) 发生时间6字节, BCD(秒分时日月年, 低字节->高字节)
    C_AD_MAX_REC_MON,        // 10字节, C相有功最大需量及发生时间 值4字节, BIN x 10000(kw) 发生时间6字节, BCD(秒分时日月年, 低字节->高字节)

    VIR_AE_CAL_MON_Z,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（总）
    VIR_AE_CAL_MON_1,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（尖）
    VIR_AE_CAL_MON_2,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（峰）
    VIR_AE_CAL_MON_3,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（平）
    VIR_AE_CAL_MON_4,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（谷）

    VIR_AE_CAL_MON_5,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（5费率）
    VIR_AE_CAL_MON_6,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（6费率）
    VIR_AE_CAL_MON_7,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（7费率）
    VIR_AE_CAL_MON_8,        // 8字节, BIN x 10000(kwh), 当月总加有功电能量（8费率）
    VIR_RE_CAL_MON_Z,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（总）

    VIR_RE_CAL_MON_1,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（尖）
    VIR_RE_CAL_MON_2,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（峰）
    VIR_RE_CAL_MON_3,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（平）
    VIR_RE_CAL_MON_4,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（谷）
    VIR_RE_CAL_MON_5,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（5费率）

    VIR_RE_CAL_MON_6,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（6费率）
    VIR_RE_CAL_MON_7,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（7费率）
    VIR_RE_CAL_MON_8,        // 8字节, BIN x 10000(kwh), 当月总加无功电能量（8费率）
    VIR_AP_MAX_MON,          // 10字节, 总加组月最大有功功率及其发生时间 值4字节, BIN x 100(kw) 发生时间6字节, BCD(秒分时日月年, 低字节->高字节)
    VIR_AP_MIN_MON,          // 10字节, 总加组月最小有功功率及其发生时间

    VIR_AP_ZERO_ACCT_MON,    // 2字节, 有功功率为零月累计时间
    VIR_TMN_EN_REMAIN,       // 8字节, BIN x 10000(kwh), 终端当前剩余电量（费）
    VIR_POWER_UO_ACCT_MON,   // 2字节, 总加组超功率定值的月累计时间
    VIR_POWER_UO_ACCEN_MON,  // 8字节, BIN x 10000(kwh), 总加组超功率定值的月累计电能量
    VIR_EN_UO_ACCT_MON,      // 2字节, 总加组超月电能量定值的月累计时间

    VIR_EN_UO_ACCEN_MON,     // 8字节, BIN x 10000(kwh), 总加组超月电能量定值的月累计电能量
    VOLT_A_DisQuali_UpRate_MON,// 2字节, BIN x 100%, A相电压偏上不合格率
    VOLT_B_DisQuali_UpRate_MON,// 2字节, BIN x 100%, B相电压偏上不合格率
    VOLT_C_DisQuali_UpRate_MON,// 2字节, BIN x 100%, C相电压偏上不合格率
    VOLT_A_DisQuali_DnRate_MON,// 2字节, BIN x 100%, A相电压偏下不合格率

    VOLT_B_DisQuali_DnRate_MON,// 2字节, BIN x 100%, B相电压偏下不合格率
    VOLT_C_DisQuali_DnRate_MON,// 2字节, BIN x 100%, C相电压偏下不合格率
    VLT_A_LOST_ACCT_MON,     // 4字节, BIN, A相电压失压月累计时间
    VLT_B_LOST_ACCT_MON,     // 4字节, BIN, B相电压失压月累计时间
    VLT_C_LOST_ACCT_MON,     // 4字节, BIN, C相电压失压月累计时间

    VLT_LOST_POS_AE_MON,     // 8字节, BIN x 10000(kwh), 月失压电量：正向有功总电能
    VLT_LOST_NEG_AE_MON,     // 8字节, BIN x 10000(kwh), 月失压电量：反向有功总电能
    VLT_LOST_POS_RE_MON,     // 8字节, BIN x 10000(kwh), 月失压电量：正向无功总电能
    VLT_LOST_NEG_RE_MON,     // 8字节, BIN x 10000(kwh), 月失压电量：反向无功总电能
    CUR_NO_BALANCE_MAX_MON,  // 8字节，月冻结电流不平衡最大值
                                // 最大值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）

    VOLT_NO_BALANCE_MAX_MON, // 8字节，月冻结电压不平衡最大值
                                // 最大值：2字节，BIN %扩大10倍
                                // 出现时间：6字节，BCD，（秒分时日月年）
    Load_Rate_Max_MON_ID,    // 8个字节，包括数值2个字节和BCD码时间，秒分时日月年,月负载率最大值
    Load_Rate_Min_MON_ID,    // 8个字节，包括数值2个字节和BCD码时间，秒分时日月年,月负载率最小值
    Copper_Loss_Power_Mon_ID,// 8位字节longlong 铜损有功总电能示值
    Iron_Loss_Power_Mon_ID,  // 8位字节longlong 铁损有功总电能示值

    DATA_CAL_MON_COMPLEX_AREA_TEST,// 各地区增补的历史月数据（CALC_DATA_MON）

DATA_CAL_MON_END_ID,
};

/******************--9计量点月记录数据项ID--*********************/
enum data_rec_mon
{
    VOLT_OU_REC_MON=0,       // 15字节, 电压越上限记录
                                // #1: 6字节, BCD(秒分时日月年), 出现时间;
                                // #2: 6字节, BCD(秒分时日月年), 结束时间;
                                // #3: 2字节, BIN x 10(v), 极值;
                                // #4: 1字节, BIN, 相值
    VOLT_OD_REC_MON,         // 15字节, 电压越下限记录,格式同上
    CUR_OU_REC_MON,          // 15字节, 电流越上限记录,格式同上
    A_SEVER_REC_MON,         // 12字节, A相断相记录
                                // #1: 6字节, BCD(秒分时日月年), 出现时间;
                                // #2: 6字节, BCD(秒分时日月年), 结束时间
    B_SEVER_REC_MON,         // 12字节, B相断相记录

    C_SEVER_REC_MON,         // 12字节, C相断相记录
    CUR_ALM_REC_MON,         // 15字节, 过电流门限记录(出现时间,结束时间,极值,相值)
                                // #1: 6字节, BCD(秒分时日月年), 出现时间;
                                // #2: 6字节, BCD(秒分时日月年), 结束时间;
                                // #3: 2字节, BIN x 10(v), 极值;
                                // #4: 1字节, BIN, 相值
    CUR_NO_BALANCD_REC_MON,  // 14字节, 电流不平衡越限记录
                                // #1: 6字节, BCD(秒分时日月年), 越限发生时间;
                                // #2: 2字节, BCD, 发生时的不平衡率;
                                // #3: 2字节, BIN, 发生时的Ia;
                                // #4: 2字节, BIN, 发生时的Ib;
                                // #5: 2字节, BIN, 发生时的Ic
    VOLT_NO_BALANCE_REC_MON, // 14字节, 电压不平衡越限记录
                                // #1: 6字节, BCD(秒分时日月年), 发生时间;
                                // #2: 2字节, BCD, 发生时的不平衡率(%);
                                // #3: 2字节, BIN, 发生时的Ua/Uab;
                                // #4: 2字节, BIN, 发生时的Ub;
                                // #5: 2字节, BIN, 发生时的Uc/Ucb
    TV_SEVER_REC_MON,        // 13字节, TV断相记录
                                // #1: 6字节, BCD(秒分时日月年), 发生时间;
                                // #2: 1字节, BIN, 相别标志;
                                // #3: 2字节, BIN, 发生时的Ua/Uab;
                                // #4: 2字节, BIN, 发生时的Ub;
                                // #5: 2字节, BIN, 发生时的Uc/Ucb

    TV_LOST_VOLT_REC_MON,    // 13字节, TV失压记录
                                // #1: 6字节, BCD(秒分时日月年), 发生时间;
                                // #2: 1字节, BIN, 相别标志;
                                // #3: 2字节, BIN, 发生时的Ua/Uab;
                                // #4: 2字节, BIN, 发生时的Ub;
                                // #5: 2字节, BIN, 发生时的Uc/Ucb
    AVER_AP_REC_MON,         // 4字节x31, BIN x 10(w), 31天每日总平均有功功率
    CUR_OV_REC_MON,          // 15字节, 电流过负荷记录
                                // #1: 1字节, BIN, 相别;
                                // #2: 2字节, BIN x 100(A), 最大值;
                                // #3: 6字节, BCD(秒分时日月年), 出现时间;
                                // #4: 6字节, BCD(秒分时日月年), 结束时间

DATA_REC_MON_END_ID,
};

/******************--1 终端当前时刻采集数据项--*********************/
enum data_rt_term
{
    ABNRM_REC=0,             // 终端异常记录( mid_ph1字节，BIN,属性码1字节，BIN,出现时间6字节，BCD，秒分时日月年,结束时间6字节)
    BUY_EN_CTRL_INFO,        // 8字节，BIN X 10000（KWH）剩余电量
    SWITCH_XCH_STATU,        // 4字节，BIN，开关及变位状态，开关及变位当前的状态标,
                                // 字节1的D0~D7按顺序表示1~8路状态量的状态 置0 分闸，置1合闸
                                // 字节2表示9~16路开关状态；
                                // 字节3的D0~D7按顺序表示1~8路状态量的变位状态 置0 无，置1至少变化了一次;字节4表示9~16路变位状态
    SWITCH_XCH_STATU2,       // 4字节，BIN，开关及变位状态，开关及变位当前的状态标
    SWITCH_XCH_STATU3,       // 4字节，BIN，开关及变位状态，开关及变位当前的状态标

    CAP_SWITCH_STATE,        // 4字节，BIN， 当前时刻装置复合开关投切的1-16开入状态及变位信息
    CAP_CONTROL_STATE,       // 2字节，BIN，当前时刻控制器电容投切的1-16控制过程状态
    SWITCH_XCH_STATU_APP,    // 4字节，BIN，开关及变位状态，给应用层使用，定义同上
    CLOSE_LOOP_CTRL_STATU,   // 4字节，BIN，当前闭环控制状态标记
    SLAVE_BOARD_INFO,        // 25字节，BIN，天津接口板采集量

    ALARM_DISP_INFO_ID,      // 5字节，BIN，设参数弹告警画面数量
    TONE_COM_STATU,          // 3字节通话状态标记.第1字节:通话状态 =3通话空闲,=1通话打开,=0通话关闭,=2通话处理中;
                                // 第2,3字节表示允许通话倒计时,bin(单位:秒)
    INT_CTRL_INFO,           // 2字节，BIN，综合轮次控制
    BAT_WORK_TACC,           // 4字节，BIN，电池工作时间
    VER_CHG_FLG,             // 1字节，BCD，终端软件版本变更标记，0x55表示有变更

    SHORT_MSG_ONE,           // 200字节，BIN，短信服务的缓存区_1
    SHORT_MSG_TWO,           // 200字节，BIN，短信服务的缓存区_2
    SHORT_MSG_THREE,         // 200字节，BIN，短信服务的缓存区_3
    SHORT_MSG_FOUR,          // 200字节，BIN，短信服务的缓存区_4
    SHORT_MSG_FIVE,          // 200字节，BIN，短信服务的缓存区_5

    SHORT_MSG_SIX,           // 200字节，BIN，短信服务的缓存区_6
    SHORT_MSG_SEVEN,         // 200字节，BIN，短信服务的缓存区_7
    SHORT_MSG_EIGHT,         // 200字节，BIN，短信服务的缓存区_8
    SHORT_MSG_NINE,          // 200字节，BIN，短信服务的缓存区_9
    SHORT_MSG_TEN,           // 200字节，BIN，短信服务的缓存区_10

    YK_ACC_EVER,             // 1字节，BIN，遥控次数
    JD_ACC_EVER,             // 1字节，BIN，拒动次数
    REST_SWT_ACC_EVER,       // 1字节，BIN，厂休控跳闸次数
    PC_SWT_ACC_EVER,         // 1字节，BIN，功控跳闸次数
    TMR_RESET_ACC_EVER,      // 2字节，BIN，当前时刻为止终端复位的次数

    YX_CHANGE_ACC_EVER,      // 1字节，BIN，遥信变位次数
    EC_SWT_ACC_DAY_EVER,     // 1字节，BIN，日电控跳闸次数
    EC_SWT_ACC_MON_EVER,     // 1字节，BIN，月电控跳闸次
    BC_SWT_ACC_EVER,         // 1字节，BIN，购电控跳闸次数
    LMT_SWT_ACC_EVER,        // 1字节，BIN，临时限电跳闸次数

    TMN_TIME,                // 6字节，BCD，终端时钟，秒分时日月年
    PARA_STATUS,             // 31字节，BIN，参数状态映射表
    TMN_STATUS,              // 2字节，BIN，终端当前状态字:字节1 的D0~D7按顺序表示1~8路状态量的状态 置1 分闸，置0合闸，字节2预留
    NORMAL_EVENT_COUNT,      // 1字节，BIN，一般事件计数器
    IMPORTANT_EVENT_COUNT,   // 1字节，BIN，重要事件计数器

    SAVE_STAUS,              // 1字节，BIN，保电/剔除状态
    PC_PERIOD_STATUS,        // 1字节，BIN，功控时段投入/解除状态
    PC_TACTIC,               // 2字节，BIN，当前功控定值方案号
    PC_STATUS,               // 1字节，BIN，功控投入解除状态
    BC_STATUS,               // 1字节，BIN，电控投入解除状态

    EVENT_STATUS,            // 8字节，BIN，事件状态标志
    CONTROL_OVER_ALARM,      // 1字节，BIN，控制越限报警
    YK_SWT_STATUS,           // 1字节，BIN，遥控跳闸输出状态
    PC_SWT_STATUS,           // 1字节，BIN，功控跳闸输出状态
    EC_SWT_MON_STATUS,       // 1字节，BIN，月电控跳闸输出状态

    BC_SWT_STATUS,           // 1字节，BIN，购电控跳闸输出状态
    CHANNEL_STATUS,          // 1字节，BIN，终端电台信道通话运行状态D3=1 通话空闲态,D2=1正在通话态
    TMN_TROUBLE_INFO,        // 8字节，BIN，终端故障自检信息
    SHORT_MSG_ONE_TIME,      // 8字节，短信息1下发时间:6+2byte 秒分时日月年,短信种类，短信长度
                                // 下发时间：6字节，BCD，秒分时日月年 短信种类，短信长度：2字节，BIN
    SHORT_MSG_TWO_TIME,      // 8字节，短信息2下发时间,格式同上

    SHORT_MSG_THREE_TIME,    // 8字节，短信息3下发时间,格式同上
    SHORT_MSG_FOUR_TIME,     // 8字节，短信息4下发时间,格式同上
    SHORT_MSG_FIVE_TIME,     // 8字节，短信息5下发时间,格式同上
    SHORT_MSG_SIX_TIME,      // 8字节，短信息6下发时间,格式同上
    SHORT_MSG_SEVEN_TIME,    // 8字节，短信息7下发时间,格式同上

    SHORT_MSG_EIGHT_TIME,    // 8字节，短信息8下发时间,格式同上
    SHORT_MSG_NINE_TIME,     // 8字节，短信息9下发时间,格式同上
    SHORT_MSG_TEN_TIME,      // 8字节，短信息10下发时间,格式同上
    FIELD_INTENSITY_ID,      // 1字节，BIN，公网场强信息
    DC_Anology1,             // 8字节，BIN X 10000,直流模拟量1

    DC_Anology2,             // 8字节，BIN X 10000,直流模拟量2
    DC_Anology3,             // 8字节，BIN X 10000,直流模拟量3
    DC_Anology4,             // 8字节，BIN X 10000,直流模拟量4
    DC_Anology5,             // 8字节，BIN X 10000,直流模拟量5
    DC_Anology6,             // 8字节，BIN X 10000,直流模拟量6

    DC_Anology7,             // 8字节，BIN X 10000,直流模拟量7
    DC_Anology8,             // 8字节，BIN X 10000,直流模拟量8
    RTU_RUNTIME_DELAY,       // 4字节，BINX1，从装置投入到现在总的运行时间，单位秒
    Tmn_Stop_Collect_Port,   // 1字节,命令指定通信端口暂停抄表最高位标识抄作
    Tmn_ReSume_Collect_Port, // 1字节,命令指定通信端口恢复抄表最高位标识抄作

    Tmn_ReStart_Collect_Port,// 1字节,命令指定通信端口重新抄表最高位标识抄作
    Tmn_Initial_Collect_Port,// 1字节,初始化指定通信端口下的全部中继路由信息最高位标识抄作
    Tmn_DelAllMtr_Collect_Port,// 1字节,删除指定通信端口下的全部电表最高位标识抄作
    THRO_PLC_ROUTER_ALLOW_FLAG,// 1字节,允许抄表标志,0x55表示允许抄表,0xaa表示禁止抄表,0xcc 表示重新抄表
    THRO_PLC_CLEAR_ROUTER_INFO,// 1字节,清除路由信息,0x55表示全清,其它无效

    THRO_REMOTE_CTRL_CMD_SUM_FLAG_ID,// 1字节,BIN,对电表拉合闸控制_命令标志(AAH:置标志 | 55H:清标志)
    THRO_REMOTE_CTRL_ALARM_SUM_FLAG_ID,// 1字节,BIN,对电表拉合闸控制_告警标志(AAH:置标志 | 55H:清标志)
    METER_FILE_MODIFY_FLAG_ID,// 1字节,BIN码,档案修改标志(AAH:修改标志 | 0x55:全清标志)
    PLC_COMM_FAILURE_FLAG,   // 1字节，载波通信端口异常标志
    PLC_COMM_FAILURE_COUNT,  // 1字节，载波通信端口异常计数器

    PLC_FIND_UNKNOWN_MTR_FLAG,// 1字节，发现未知电表标志
    METER_FILE_MODIFY_FLAG_ID_EX,// 32字节,BIN码,档案修改标志(AAH:修改标志 | 0x55:全清标志 给485抄表调度用)
    PLC_COMM_STATE_FLAG0,    // 32个字节
    PLC_COMM_STATE_FLAG1,    // 32个字节
    PLC_COMM_STATE_FLAG2,    // 32个字节

    PLC_COMM_STATE_FLAG3,    // 32个字节
    PLC_COMM_STATE_FLAG4,    // 32个字节
    PLC_COMM_STATE_FLAG5,    // 32个字节
    PLC_COMM_STATE_FLAG6,    // 32个字节
    PLC_COMM_STATE_FLAG7,    // 32个字节

    PLC_MR_TOTAL_MRTER_NUM,  // 2字节，要抄电表总数，BIN
    PLC_MR_WORK_STATE_FLAG,  // 1字节，当前抄表工作状态标志，D1抄表任务时段内完成/未完成标志位：1/0完成/未完成；D0抄表忙/闲标志位：1/0正在抄表/未抄表
    PLC_MR_SUCC_MRTER_NUM,   // 2字节，抄表成功块数，BIN
    PLC_MR_SUCC_IMP_MRTER_NUM,// 1字节，抄重点表成功块数，BIN
    PLC_MR_START_TIME,       // 6字节，抄表开始时间

    PLC_MR_END_TIME,         // 6字节，抄表结束时间
    PLC_FIND_METER_FLAG,     // 1个字节,0x55有效,其它无效
    PLC_TEST_MODE_FLAG_ID,   // 1字节,BIN码,测试模式标志(1:测试模式 / 0:出厂模式)
    PLC_PRE_AMR_PATCH_RUN_DAYS_ID,// 1字节,BIN码,预补抄运行天数
    COM_1_MR_TOTAL_MRTER_NUM,// 2字节，要抄电表总数，BIN

    COM_1_MR_WORK_STATE_FLAG,// 1字节，当前抄表工作状态标志，D1抄表任务时段内完成/未完成标志位：1/0完成/未完成；D0抄表忙/闲标志位：1/0正在抄表/未抄表
    COM_1_MR_SUCC_MRTER_NUM, // 2字节，抄表成功块数，BIN
    COM_1_MR_SUCC_IMP_MRTER_NUM,// 1字节，抄重点表成功块数，BIN
    COM_1_MR_START_TIME,     // 6字节，抄表开始时间
    COM_1_MR_END_TIME,       // 6字节，抄表结束时间

    COM_2_MR_TOTAL_MRTER_NUM,// 2字节，要抄电表总数，BIN
    COM_2_MR_WORK_STATE_FLAG,// 1字节，当前抄表工作状态标志，D1抄表任务时段内完成/未完成标志位：1/0完成/未完成；D0抄表忙/闲标志位：1/0正在抄表/未抄表
    COM_2_MR_SUCC_MRTER_NUM, // 2字节，抄表成功块数，BIN
    COM_2_MR_SUCC_IMP_MRTER_NUM,// 1字节，抄重点表成功块数，BIN
    COM_2_MR_START_TIME,     // 6字节，抄表开始时间

    COM_2_MR_END_TIME,       // 6字节，抄表结束时间
    COM_3_MR_TOTAL_MRTER_NUM,// 2字节，要抄电表总数，BIN
    COM_3_MR_WORK_STATE_FLAG,// 1字节，当前抄表工作状态标志，D1抄表任务时段内完成/未完成标志位：1/0完成/未完成；D0抄表忙/闲标志位：1/0正在抄表/未抄表
    COM_3_MR_SUCC_MRTER_NUM, // 2字节，抄表成功块数，BIN
    COM_3_MR_SUCC_IMP_MRTER_NUM,// 1字节，抄重点表成功块数，BIN

    COM_3_MR_START_TIME,     // 6字节，抄表开始时间
    COM_3_MR_END_TIME,       // 6字节，抄表结束时间
    SEARCH_MET_NUM_ID,       // 2字节，搜索到电表的总个数
    MYGMR_INFO_ID,           // 60字节，远程模块的模块信息
    DELETE_METER_FLAG_485_ID,// 1字节，当前485有需要删除电表

    DELETE_METER_FLAG_PLC_ID,// 1字节，当前载波有需要删除电表
    TMN_POWEROFF_FLAG_ID,    // 1字节，终端停电标志，0x55-停电告警已发生
    EVENT_TMNHALT_CNT,       // 2字节, 终端停电掉电/上电记录发生次数
    EVENT_TMNHALT_INFO,      // 12*10字节, 上十条终端停电掉电/上电记录
                                // #1: 6字节,停电时刻（YYMMDDhhmm）
    EVENT_COUNTS,            // 64字节，终端64个事件数据，每个事件占一字节

    DATA_ERC1_REC1,          // 1~64类事件数据内容，每类15条
    DATA_ERC1_REC2,          // 
    DATA_ERC1_REC3,          // 
    DATA_ERC1_REC4,          // 
    DATA_ERC1_REC5,          // 

    DATA_ERC1_REC6,          // 
    DATA_ERC1_REC7,          // 
    DATA_ERC1_REC8,          // 
    DATA_ERC1_REC9,          // 
    DATA_ERC1_REC10,         // 

    DATA_ERC1_REC11,         // 
    DATA_ERC1_REC12,         // 
    DATA_ERC1_REC13,         // 
    DATA_ERC1_REC14,         // 
    DATA_ERC1_REC15,         // 

    DATA_ERC2_REC1,          // 
    DATA_ERC2_REC2,          // 
    DATA_ERC2_REC3,          // 
    DATA_ERC2_REC4,          // 
    DATA_ERC2_REC5,          // 

    DATA_ERC2_REC6,          // 
    DATA_ERC2_REC7,          // 
    DATA_ERC2_REC8,          // 
    DATA_ERC2_REC9,          // 
    DATA_ERC2_REC10,         // 

    DATA_ERC2_REC11,         // 
    DATA_ERC2_REC12,         // 
    DATA_ERC2_REC13,         // 
    DATA_ERC2_REC14,         // 
    DATA_ERC2_REC15,         // 

    DATA_ERC3_REC1,          // 
    DATA_ERC3_REC2,          // 
    DATA_ERC3_REC3,          // 
    DATA_ERC3_REC4,          // 
    DATA_ERC3_REC5,          // 

    DATA_ERC3_REC6,          // 
    DATA_ERC3_REC7,          // 
    DATA_ERC3_REC8,          // 
    DATA_ERC3_REC9,          // 
    DATA_ERC3_REC10,         // 

    DATA_ERC3_REC11,         // 
    DATA_ERC3_REC12,         // 
    DATA_ERC3_REC13,         // 
    DATA_ERC3_REC14,         // 
    DATA_ERC3_REC15,         // 

    DATA_ERC4_REC1,          // 
    DATA_ERC4_REC2,          // 
    DATA_ERC4_REC3,          // 
    DATA_ERC4_REC4,          // 
    DATA_ERC4_REC5,          // 

    DATA_ERC4_REC6,          // 
    DATA_ERC4_REC7,          // 
    DATA_ERC4_REC8,          // 
    DATA_ERC4_REC9,          // 
    DATA_ERC4_REC10,         // 

    DATA_ERC4_REC11,         // 
    DATA_ERC4_REC12,         // 
    DATA_ERC4_REC13,         // 
    DATA_ERC4_REC14,         // 
    DATA_ERC4_REC15,         // 

    DATA_ERC5_REC1,          // 
    DATA_ERC5_REC2,          // 
    DATA_ERC5_REC3,          // 
    DATA_ERC5_REC4,          // 
    DATA_ERC5_REC5,          // 

    DATA_ERC5_REC6,          // 
    DATA_ERC5_REC7,          // 
    DATA_ERC5_REC8,          // 
    DATA_ERC5_REC9,          // 
    DATA_ERC5_REC10,         // 

    DATA_ERC5_REC11,         // 
    DATA_ERC5_REC12,         // 
    DATA_ERC5_REC13,         // 
    DATA_ERC5_REC14,         // 
    DATA_ERC5_REC15,         // 

    DATA_ERC6_REC1,          // 
    DATA_ERC6_REC2,          // 
    DATA_ERC6_REC3,          // 
    DATA_ERC6_REC4,          // 
    DATA_ERC6_REC5,          // 

    DATA_ERC6_REC6,          // 
    DATA_ERC6_REC7,          // 
    DATA_ERC6_REC8,          // 
    DATA_ERC6_REC9,          // 
    DATA_ERC6_REC10,         // 

    DATA_ERC6_REC11,         // 
    DATA_ERC6_REC12,         // 
    DATA_ERC6_REC13,         // 
    DATA_ERC6_REC14,         // 
    DATA_ERC6_REC15,         // 

    DATA_ERC7_REC1,          // 
    DATA_ERC7_REC2,          // 
    DATA_ERC7_REC3,          // 
    DATA_ERC7_REC4,          // 
    DATA_ERC7_REC5,          // 

    DATA_ERC7_REC6,          // 
    DATA_ERC7_REC7,          // 
    DATA_ERC7_REC8,          // 
    DATA_ERC7_REC9,          // 
    DATA_ERC7_REC10,         // 

    DATA_ERC7_REC11,         // 
    DATA_ERC7_REC12,         // 
    DATA_ERC7_REC13,         // 
    DATA_ERC7_REC14,         // 
    DATA_ERC7_REC15,         // 

    DATA_ERC8_REC1,          // 
    DATA_ERC8_REC2,          // 
    DATA_ERC8_REC3,          // 
    DATA_ERC8_REC4,          // 
    DATA_ERC8_REC5,          // 

    DATA_ERC8_REC6,          // 
    DATA_ERC8_REC7,          // 
    DATA_ERC8_REC8,          // 
    DATA_ERC8_REC9,          // 
    DATA_ERC8_REC10,         // 

    DATA_ERC8_REC11,         // 
    DATA_ERC8_REC12,         // 
    DATA_ERC8_REC13,         // 
    DATA_ERC8_REC14,         // 
    DATA_ERC8_REC15,         // 

    DATA_ERC9_REC1,          // 
    DATA_ERC9_REC2,          // 
    DATA_ERC9_REC3,          // 
    DATA_ERC9_REC4,          // 
    DATA_ERC9_REC5,          // 

    DATA_ERC9_REC6,          // 
    DATA_ERC9_REC7,          // 
    DATA_ERC9_REC8,          // 
    DATA_ERC9_REC9,          // 
    DATA_ERC9_REC10,         // 

    DATA_ERC9_REC11,         // 
    DATA_ERC9_REC12,         // 
    DATA_ERC9_REC13,         // 
    DATA_ERC9_REC14,         // 
    DATA_ERC9_REC15,         // 

    DATA_ERC10_REC1,         // 
    DATA_ERC10_REC2,         // 
    DATA_ERC10_REC3,         // 
    DATA_ERC10_REC4,         // 
    DATA_ERC10_REC5,         // 

    DATA_ERC10_REC6,         // 
    DATA_ERC10_REC7,         // 
    DATA_ERC10_REC8,         // 
    DATA_ERC10_REC9,         // 
    DATA_ERC10_REC10,        // 

    DATA_ERC10_REC11,        // 
    DATA_ERC10_REC12,        // 
    DATA_ERC10_REC13,        // 
    DATA_ERC10_REC14,        // 
    DATA_ERC10_REC15,        // 

    DATA_ERC11_REC1,         // 
    DATA_ERC11_REC2,         // 
    DATA_ERC11_REC3,         // 
    DATA_ERC11_REC4,         // 
    DATA_ERC11_REC5,         // 

    DATA_ERC11_REC6,         // 
    DATA_ERC11_REC7,         // 
    DATA_ERC11_REC8,         // 
    DATA_ERC11_REC9,         // 
    DATA_ERC11_REC10,        // 

    DATA_ERC11_REC11,        // 
    DATA_ERC11_REC12,        // 
    DATA_ERC11_REC13,        // 
    DATA_ERC11_REC14,        // 
    DATA_ERC11_REC15,        // 

    DATA_ERC12_REC1,         // 
    DATA_ERC12_REC2,         // 
    DATA_ERC12_REC3,         // 
    DATA_ERC12_REC4,         // 
    DATA_ERC12_REC5,         // 

    DATA_ERC12_REC6,         // 
    DATA_ERC12_REC7,         // 
    DATA_ERC12_REC8,         // 
    DATA_ERC12_REC9,         // 
    DATA_ERC12_REC10,        // 

    DATA_ERC12_REC11,        // 
    DATA_ERC12_REC12,        // 
    DATA_ERC12_REC13,        // 
    DATA_ERC12_REC14,        // 
    DATA_ERC12_REC15,        // 

    DATA_ERC13_REC1,         // 
    DATA_ERC13_REC2,         // 
    DATA_ERC13_REC3,         // 
    DATA_ERC13_REC4,         // 
    DATA_ERC13_REC5,         // 

    DATA_ERC13_REC6,         // 
    DATA_ERC13_REC7,         // 
    DATA_ERC13_REC8,         // 
    DATA_ERC13_REC9,         // 
    DATA_ERC13_REC10,        // 

    DATA_ERC13_REC11,        // 
    DATA_ERC13_REC12,        // 
    DATA_ERC13_REC13,        // 
    DATA_ERC13_REC14,        // 
    DATA_ERC13_REC15,        // 

    DATA_ERC14_REC1,         // 
    DATA_ERC14_REC2,         // 
    DATA_ERC14_REC3,         // 
    DATA_ERC14_REC4,         // 
    DATA_ERC14_REC5,         // 

    DATA_ERC14_REC6,         // 
    DATA_ERC14_REC7,         // 
    DATA_ERC14_REC8,         // 
    DATA_ERC14_REC9,         // 
    DATA_ERC14_REC10,        // 

    DATA_ERC14_REC11,        // 
    DATA_ERC14_REC12,        // 
    DATA_ERC14_REC13,        // 
    DATA_ERC14_REC14,        // 
    DATA_ERC14_REC15,        // 

    DATA_ERC15_REC1,         // 
    DATA_ERC15_REC2,         // 
    DATA_ERC15_REC3,         // 
    DATA_ERC15_REC4,         // 
    DATA_ERC15_REC5,         // 

    DATA_ERC15_REC6,         // 
    DATA_ERC15_REC7,         // 
    DATA_ERC15_REC8,         // 
    DATA_ERC15_REC9,         // 
    DATA_ERC15_REC10,        // 

    DATA_ERC15_REC11,        // 
    DATA_ERC15_REC12,        // 
    DATA_ERC15_REC13,        // 
    DATA_ERC15_REC14,        // 
    DATA_ERC15_REC15,        // 

    DATA_ERC16_REC1,         // 
    DATA_ERC16_REC2,         // 
    DATA_ERC16_REC3,         // 
    DATA_ERC16_REC4,         // 
    DATA_ERC16_REC5,         // 

    DATA_ERC16_REC6,         // 
    DATA_ERC16_REC7,         // 
    DATA_ERC16_REC8,         // 
    DATA_ERC16_REC9,         // 
    DATA_ERC16_REC10,        // 

    DATA_ERC16_REC11,        // 
    DATA_ERC16_REC12,        // 
    DATA_ERC16_REC13,        // 
    DATA_ERC16_REC14,        // 
    DATA_ERC16_REC15,        // 

    DATA_ERC17_REC1,         // 
    DATA_ERC17_REC2,         // 
    DATA_ERC17_REC3,         // 
    DATA_ERC17_REC4,         // 
    DATA_ERC17_REC5,         // 

    DATA_ERC17_REC6,         // 
    DATA_ERC17_REC7,         // 
    DATA_ERC17_REC8,         // 
    DATA_ERC17_REC9,         // 
    DATA_ERC17_REC10,        // 

    DATA_ERC17_REC11,        // 
    DATA_ERC17_REC12,        // 
    DATA_ERC17_REC13,        // 
    DATA_ERC17_REC14,        // 
    DATA_ERC17_REC15,        // 

    DATA_ERC18_REC1,         // 
    DATA_ERC18_REC2,         // 
    DATA_ERC18_REC3,         // 
    DATA_ERC18_REC4,         // 
    DATA_ERC18_REC5,         // 

    DATA_ERC18_REC6,         // 
    DATA_ERC18_REC7,         // 
    DATA_ERC18_REC8,         // 
    DATA_ERC18_REC9,         // 
    DATA_ERC18_REC10,        // 

    DATA_ERC18_REC11,        // 
    DATA_ERC18_REC12,        // 
    DATA_ERC18_REC13,        // 
    DATA_ERC18_REC14,        // 
    DATA_ERC18_REC15,        // 

    DATA_ERC19_REC1,         // 
    DATA_ERC19_REC2,         // 
    DATA_ERC19_REC3,         // 
    DATA_ERC19_REC4,         // 
    DATA_ERC19_REC5,         // 

    DATA_ERC19_REC6,         // 
    DATA_ERC19_REC7,         // 
    DATA_ERC19_REC8,         // 
    DATA_ERC19_REC9,         // 
    DATA_ERC19_REC10,        // 

    DATA_ERC19_REC11,        // 
    DATA_ERC19_REC12,        // 
    DATA_ERC19_REC13,        // 
    DATA_ERC19_REC14,        // 
    DATA_ERC19_REC15,        // 

    DATA_ERC20_REC1,         // 
    DATA_ERC20_REC2,         // 
    DATA_ERC20_REC3,         // 
    DATA_ERC20_REC4,         // 
    DATA_ERC20_REC5,         // 

    DATA_ERC20_REC6,         // 
    DATA_ERC20_REC7,         // 
    DATA_ERC20_REC8,         // 
    DATA_ERC20_REC9,         // 
    DATA_ERC20_REC10,        // 

    DATA_ERC20_REC11,        // 
    DATA_ERC20_REC12,        // 
    DATA_ERC20_REC13,        // 
    DATA_ERC20_REC14,        // 
    DATA_ERC20_REC15,        // 

    DATA_ERC21_REC1,         // 
    DATA_ERC21_REC2,         // 
    DATA_ERC21_REC3,         // 
    DATA_ERC21_REC4,         // 
    DATA_ERC21_REC5,         // 

    DATA_ERC21_REC6,         // 
    DATA_ERC21_REC7,         // 
    DATA_ERC21_REC8,         // 
    DATA_ERC21_REC9,         // 
    DATA_ERC21_REC10,        // 

    DATA_ERC21_REC11,        // 
    DATA_ERC21_REC12,        // 
    DATA_ERC21_REC13,        // 
    DATA_ERC21_REC14,        // 
    DATA_ERC21_REC15,        // 

    DATA_ERC22_REC1,         // 
    DATA_ERC22_REC2,         // 
    DATA_ERC22_REC3,         // 
    DATA_ERC22_REC4,         // 
    DATA_ERC22_REC5,         // 

    DATA_ERC22_REC6,         // 
    DATA_ERC22_REC7,         // 
    DATA_ERC22_REC8,         // 
    DATA_ERC22_REC9,         // 
    DATA_ERC22_REC10,        // 

    DATA_ERC22_REC11,        // 
    DATA_ERC22_REC12,        // 
    DATA_ERC22_REC13,        // 
    DATA_ERC22_REC14,        // 
    DATA_ERC22_REC15,        // 

    DATA_ERC23_REC1,         // 
    DATA_ERC23_REC2,         // 
    DATA_ERC23_REC3,         // 
    DATA_ERC23_REC4,         // 
    DATA_ERC23_REC5,         // 

    DATA_ERC23_REC6,         // 
    DATA_ERC23_REC7,         // 
    DATA_ERC23_REC8,         // 
    DATA_ERC23_REC9,         // 
    DATA_ERC23_REC10,        // 

    DATA_ERC23_REC11,        // 
    DATA_ERC23_REC12,        // 
    DATA_ERC23_REC13,        // 
    DATA_ERC23_REC14,        // 
    DATA_ERC23_REC15,        // 

    DATA_ERC24_REC1,         // 
    DATA_ERC24_REC2,         // 
    DATA_ERC24_REC3,         // 
    DATA_ERC24_REC4,         // 
    DATA_ERC24_REC5,         // 

    DATA_ERC24_REC6,         // 
    DATA_ERC24_REC7,         // 
    DATA_ERC24_REC8,         // 
    DATA_ERC24_REC9,         // 
    DATA_ERC24_REC10,        // 

    DATA_ERC24_REC11,        // 
    DATA_ERC24_REC12,        // 
    DATA_ERC24_REC13,        // 
    DATA_ERC24_REC14,        // 
    DATA_ERC24_REC15,        // 

    DATA_ERC25_REC1,         // 
    DATA_ERC25_REC2,         // 
    DATA_ERC25_REC3,         // 
    DATA_ERC25_REC4,         // 
    DATA_ERC25_REC5,         // 

    DATA_ERC25_REC6,         // 
    DATA_ERC25_REC7,         // 
    DATA_ERC25_REC8,         // 
    DATA_ERC25_REC9,         // 
    DATA_ERC25_REC10,        // 

    DATA_ERC25_REC11,        // 
    DATA_ERC25_REC12,        // 
    DATA_ERC25_REC13,        // 
    DATA_ERC25_REC14,        // 
    DATA_ERC25_REC15,        // 

    DATA_ERC26_REC1,         // 
    DATA_ERC26_REC2,         // 
    DATA_ERC26_REC3,         // 
    DATA_ERC26_REC4,         // 
    DATA_ERC26_REC5,         // 

    DATA_ERC26_REC6,         // 
    DATA_ERC26_REC7,         // 
    DATA_ERC26_REC8,         // 
    DATA_ERC26_REC9,         // 
    DATA_ERC26_REC10,        // 

    DATA_ERC26_REC11,        // 
    DATA_ERC26_REC12,        // 
    DATA_ERC26_REC13,        // 
    DATA_ERC26_REC14,        // 
    DATA_ERC26_REC15,        // 

    DATA_ERC27_REC1,         // 
    DATA_ERC27_REC2,         // 
    DATA_ERC27_REC3,         // 
    DATA_ERC27_REC4,         // 
    DATA_ERC27_REC5,         // 

    DATA_ERC27_REC6,         // 
    DATA_ERC27_REC7,         // 
    DATA_ERC27_REC8,         // 
    DATA_ERC27_REC9,         // 
    DATA_ERC27_REC10,        // 

    DATA_ERC27_REC11,        // 
    DATA_ERC27_REC12,        // 
    DATA_ERC27_REC13,        // 
    DATA_ERC27_REC14,        // 
    DATA_ERC27_REC15,        // 

    DATA_ERC28_REC1,         // 
    DATA_ERC28_REC2,         // 
    DATA_ERC28_REC3,         // 
    DATA_ERC28_REC4,         // 
    DATA_ERC28_REC5,         // 

    DATA_ERC28_REC6,         // 
    DATA_ERC28_REC7,         // 
    DATA_ERC28_REC8,         // 
    DATA_ERC28_REC9,         // 
    DATA_ERC28_REC10,        // 

    DATA_ERC28_REC11,        // 
    DATA_ERC28_REC12,        // 
    DATA_ERC28_REC13,        // 
    DATA_ERC28_REC14,        // 
    DATA_ERC28_REC15,        // 

    DATA_ERC29_REC1,         // 
    DATA_ERC29_REC2,         // 
    DATA_ERC29_REC3,         // 
    DATA_ERC29_REC4,         // 
    DATA_ERC29_REC5,         // 

    DATA_ERC29_REC6,         // 
    DATA_ERC29_REC7,         // 
    DATA_ERC29_REC8,         // 
    DATA_ERC29_REC9,         // 
    DATA_ERC29_REC10,        // 

    DATA_ERC29_REC11,        // 
    DATA_ERC29_REC12,        // 
    DATA_ERC29_REC13,        // 
    DATA_ERC29_REC14,        // 
    DATA_ERC29_REC15,        // 

    DATA_ERC30_REC1,         // 
    DATA_ERC30_REC2,         // 
    DATA_ERC30_REC3,         // 
    DATA_ERC30_REC4,         // 
    DATA_ERC30_REC5,         // 

    DATA_ERC30_REC6,         // 
    DATA_ERC30_REC7,         // 
    DATA_ERC30_REC8,         // 
    DATA_ERC30_REC9,         // 
    DATA_ERC30_REC10,        // 

    DATA_ERC30_REC11,        // 
    DATA_ERC30_REC12,        // 
    DATA_ERC30_REC13,        // 
    DATA_ERC30_REC14,        // 
    DATA_ERC30_REC15,        // 

    DATA_ERC31_REC1,         // 
    DATA_ERC31_REC2,         // 
    DATA_ERC31_REC3,         // 
    DATA_ERC31_REC4,         // 
    DATA_ERC31_REC5,         // 

    DATA_ERC31_REC6,         // 
    DATA_ERC31_REC7,         // 
    DATA_ERC31_REC8,         // 
    DATA_ERC31_REC9,         // 
    DATA_ERC31_REC10,        // 

    DATA_ERC31_REC11,        // 
    DATA_ERC31_REC12,        // 
    DATA_ERC31_REC13,        // 
    DATA_ERC31_REC14,        // 
    DATA_ERC31_REC15,        // 

    DATA_ERC32_REC1,         // 
    DATA_ERC32_REC2,         // 
    DATA_ERC32_REC3,         // 
    DATA_ERC32_REC4,         // 
    DATA_ERC32_REC5,         // 

    DATA_ERC32_REC6,         // 
    DATA_ERC32_REC7,         // 
    DATA_ERC32_REC8,         // 
    DATA_ERC32_REC9,         // 
    DATA_ERC32_REC10,        // 

    DATA_ERC32_REC11,        // 
    DATA_ERC32_REC12,        // 
    DATA_ERC32_REC13,        // 
    DATA_ERC32_REC14,        // 
    DATA_ERC32_REC15,        // 

    DATA_ERC33_REC1,         // 
    DATA_ERC33_REC2,         // 
    DATA_ERC33_REC3,         // 
    DATA_ERC33_REC4,         // 
    DATA_ERC33_REC5,         // 

    DATA_ERC33_REC6,         // 
    DATA_ERC33_REC7,         // 
    DATA_ERC33_REC8,         // 
    DATA_ERC33_REC9,         // 
    DATA_ERC33_REC10,        // 

    DATA_ERC33_REC11,        // 
    DATA_ERC33_REC12,        // 
    DATA_ERC33_REC13,        // 
    DATA_ERC33_REC14,        // 
    DATA_ERC33_REC15,        // 

    DATA_ERC34_REC1,         // 
    DATA_ERC34_REC2,         // 
    DATA_ERC34_REC3,         // 
    DATA_ERC34_REC4,         // 
    DATA_ERC34_REC5,         // 

    DATA_ERC34_REC6,         // 
    DATA_ERC34_REC7,         // 
    DATA_ERC34_REC8,         // 
    DATA_ERC34_REC9,         // 
    DATA_ERC34_REC10,        // 

    DATA_ERC34_REC11,        // 
    DATA_ERC34_REC12,        // 
    DATA_ERC34_REC13,        // 
    DATA_ERC34_REC14,        // 
    DATA_ERC34_REC15,        // 

    DATA_ERC35_REC1,         // 
    DATA_ERC35_REC2,         // 
    DATA_ERC35_REC3,         // 
    DATA_ERC35_REC4,         // 
    DATA_ERC35_REC5,         // 

    DATA_ERC35_REC6,         // 
    DATA_ERC35_REC7,         // 
    DATA_ERC35_REC8,         // 
    DATA_ERC35_REC9,         // 
    DATA_ERC35_REC10,        // 

    DATA_ERC35_REC11,        // 
    DATA_ERC35_REC12,        // 
    DATA_ERC35_REC13,        // 
    DATA_ERC35_REC14,        // 
    DATA_ERC35_REC15,        // 

    DATA_ERC36_REC1,         // 
    DATA_ERC36_REC2,         // 
    DATA_ERC36_REC3,         // 
    DATA_ERC36_REC4,         // 
    DATA_ERC36_REC5,         // 

    DATA_ERC36_REC6,         // 
    DATA_ERC36_REC7,         // 
    DATA_ERC36_REC8,         // 
    DATA_ERC36_REC9,         // 
    DATA_ERC36_REC10,        // 

    DATA_ERC36_REC11,        // 
    DATA_ERC36_REC12,        // 
    DATA_ERC36_REC13,        // 
    DATA_ERC36_REC14,        // 
    DATA_ERC36_REC15,        // 

    DATA_ERC37_REC1,         // 
    DATA_ERC37_REC2,         // 
    DATA_ERC37_REC3,         // 
    DATA_ERC37_REC4,         // 
    DATA_ERC37_REC5,         // 

    DATA_ERC37_REC6,         // 
    DATA_ERC37_REC7,         // 
    DATA_ERC37_REC8,         // 
    DATA_ERC37_REC9,         // 
    DATA_ERC37_REC10,        // 

    DATA_ERC37_REC11,        // 
    DATA_ERC37_REC12,        // 
    DATA_ERC37_REC13,        // 
    DATA_ERC37_REC14,        // 
    DATA_ERC37_REC15,        // 

    DATA_ERC38_REC1,         // 
    DATA_ERC38_REC2,         // 
    DATA_ERC38_REC3,         // 
    DATA_ERC38_REC4,         // 
    DATA_ERC38_REC5,         // 

    DATA_ERC38_REC6,         // 
    DATA_ERC38_REC7,         // 
    DATA_ERC38_REC8,         // 
    DATA_ERC38_REC9,         // 
    DATA_ERC38_REC10,        // 

    DATA_ERC38_REC11,        // 
    DATA_ERC38_REC12,        // 
    DATA_ERC38_REC13,        // 
    DATA_ERC38_REC14,        // 
    DATA_ERC38_REC15,        // 

    DATA_ERC39_REC1,         // 
    DATA_ERC39_REC2,         // 
    DATA_ERC39_REC3,         // 
    DATA_ERC39_REC4,         // 
    DATA_ERC39_REC5,         // 

    DATA_ERC39_REC6,         // 
    DATA_ERC39_REC7,         // 
    DATA_ERC39_REC8,         // 
    DATA_ERC39_REC9,         // 
    DATA_ERC39_REC10,        // 

    DATA_ERC39_REC11,        // 
    DATA_ERC39_REC12,        // 
    DATA_ERC39_REC13,        // 
    DATA_ERC39_REC14,        // 
    DATA_ERC39_REC15,        // 

    DATA_ERC40_REC1,         // 
    DATA_ERC40_REC2,         // 
    DATA_ERC40_REC3,         // 
    DATA_ERC40_REC4,         // 
    DATA_ERC40_REC5,         // 

    DATA_ERC40_REC6,         // 
    DATA_ERC40_REC7,         // 
    DATA_ERC40_REC8,         // 
    DATA_ERC40_REC9,         // 
    DATA_ERC40_REC10,        // 

    DATA_ERC40_REC11,        // 
    DATA_ERC40_REC12,        // 
    DATA_ERC40_REC13,        // 
    DATA_ERC40_REC14,        // 
    DATA_ERC40_REC15,        // 

    DATA_ERC41_REC1,         // 
    DATA_ERC41_REC2,         // 
    DATA_ERC41_REC3,         // 
    DATA_ERC41_REC4,         // 
    DATA_ERC41_REC5,         // 

    DATA_ERC41_REC6,         // 
    DATA_ERC41_REC7,         // 
    DATA_ERC41_REC8,         // 
    DATA_ERC41_REC9,         // 
    DATA_ERC41_REC10,        // 

    DATA_ERC41_REC11,        // 
    DATA_ERC41_REC12,        // 
    DATA_ERC41_REC13,        // 
    DATA_ERC41_REC14,        // 
    DATA_ERC41_REC15,        // 

    DATA_ERC42_REC1,         // 
    DATA_ERC42_REC2,         // 
    DATA_ERC42_REC3,         // 
    DATA_ERC42_REC4,         // 
    DATA_ERC42_REC5,         // 

    DATA_ERC42_REC6,         // 
    DATA_ERC42_REC7,         // 
    DATA_ERC42_REC8,         // 
    DATA_ERC42_REC9,         // 
    DATA_ERC42_REC10,        // 

    DATA_ERC42_REC11,        // 
    DATA_ERC42_REC12,        // 
    DATA_ERC42_REC13,        // 
    DATA_ERC42_REC14,        // 
    DATA_ERC42_REC15,        // 

    DATA_ERC43_REC1,         // 
    DATA_ERC43_REC2,         // 
    DATA_ERC43_REC3,         // 
    DATA_ERC43_REC4,         // 
    DATA_ERC43_REC5,         // 

    DATA_ERC43_REC6,         // 
    DATA_ERC43_REC7,         // 
    DATA_ERC43_REC8,         // 
    DATA_ERC43_REC9,         // 
    DATA_ERC43_REC10,        // 

    DATA_ERC43_REC11,        // 
    DATA_ERC43_REC12,        // 
    DATA_ERC43_REC13,        // 
    DATA_ERC43_REC14,        // 
    DATA_ERC43_REC15,        // 

    DATA_ERC44_REC1,         // 
    DATA_ERC44_REC2,         // 
    DATA_ERC44_REC3,         // 
    DATA_ERC44_REC4,         // 
    DATA_ERC44_REC5,         // 

    DATA_ERC44_REC6,         // 
    DATA_ERC44_REC7,         // 
    DATA_ERC44_REC8,         // 
    DATA_ERC44_REC9,         // 
    DATA_ERC44_REC10,        // 

    DATA_ERC44_REC11,        // 
    DATA_ERC44_REC12,        // 
    DATA_ERC44_REC13,        // 
    DATA_ERC44_REC14,        // 
    DATA_ERC44_REC15,        // 

    DATA_ERC45_REC1,         // 
    DATA_ERC45_REC2,         // 
    DATA_ERC45_REC3,         // 
    DATA_ERC45_REC4,         // 
    DATA_ERC45_REC5,         // 

    DATA_ERC45_REC6,         // 
    DATA_ERC45_REC7,         // 
    DATA_ERC45_REC8,         // 
    DATA_ERC45_REC9,         // 
    DATA_ERC45_REC10,        // 

    DATA_ERC45_REC11,        // 
    DATA_ERC45_REC12,        // 
    DATA_ERC45_REC13,        // 
    DATA_ERC45_REC14,        // 
    DATA_ERC45_REC15,        // 

    DATA_ERC46_REC1,         // 
    DATA_ERC46_REC2,         // 
    DATA_ERC46_REC3,         // 
    DATA_ERC46_REC4,         // 
    DATA_ERC46_REC5,         // 

    DATA_ERC46_REC6,         // 
    DATA_ERC46_REC7,         // 
    DATA_ERC46_REC8,         // 
    DATA_ERC46_REC9,         // 
    DATA_ERC46_REC10,        // 

    DATA_ERC46_REC11,        // 
    DATA_ERC46_REC12,        // 
    DATA_ERC46_REC13,        // 
    DATA_ERC46_REC14,        // 
    DATA_ERC46_REC15,        // 

    DATA_ERC47_REC1,         // 
    DATA_ERC47_REC2,         // 
    DATA_ERC47_REC3,         // 
    DATA_ERC47_REC4,         // 
    DATA_ERC47_REC5,         // 

    DATA_ERC47_REC6,         // 
    DATA_ERC47_REC7,         // 
    DATA_ERC47_REC8,         // 
    DATA_ERC47_REC9,         // 
    DATA_ERC47_REC10,        // 

    DATA_ERC47_REC11,        // 
    DATA_ERC47_REC12,        // 
    DATA_ERC47_REC13,        // 
    DATA_ERC47_REC14,        // 
    DATA_ERC47_REC15,        // 

    DATA_ERC48_REC1,         // 
    DATA_ERC48_REC2,         // 
    DATA_ERC48_REC3,         // 
    DATA_ERC48_REC4,         // 
    DATA_ERC48_REC5,         // 

    DATA_ERC48_REC6,         // 
    DATA_ERC48_REC7,         // 
    DATA_ERC48_REC8,         // 
    DATA_ERC48_REC9,         // 
    DATA_ERC48_REC10,        // 

    DATA_ERC48_REC11,        // 
    DATA_ERC48_REC12,        // 
    DATA_ERC48_REC13,        // 
    DATA_ERC48_REC14,        // 
    DATA_ERC48_REC15,        // 

    DATA_ERC49_REC1,         // 
    DATA_ERC49_REC2,         // 
    DATA_ERC49_REC3,         // 
    DATA_ERC49_REC4,         // 
    DATA_ERC49_REC5,         // 

    DATA_ERC49_REC6,         // 
    DATA_ERC49_REC7,         // 
    DATA_ERC49_REC8,         // 
    DATA_ERC49_REC9,         // 
    DATA_ERC49_REC10,        // 

    DATA_ERC49_REC11,        // 
    DATA_ERC49_REC12,        // 
    DATA_ERC49_REC13,        // 
    DATA_ERC49_REC14,        // 
    DATA_ERC49_REC15,        // 

    DATA_ERC50_REC1,         // 
    DATA_ERC50_REC2,         // 
    DATA_ERC50_REC3,         // 
    DATA_ERC50_REC4,         // 
    DATA_ERC50_REC5,         // 

    DATA_ERC50_REC6,         // 
    DATA_ERC50_REC7,         // 
    DATA_ERC50_REC8,         // 
    DATA_ERC50_REC9,         // 
    DATA_ERC50_REC10,        // 

    DATA_ERC50_REC11,        // 
    DATA_ERC50_REC12,        // 
    DATA_ERC50_REC13,        // 
    DATA_ERC50_REC14,        // 
    DATA_ERC50_REC15,        // 

    DATA_ERC51_REC1,         // 
    DATA_ERC51_REC2,         // 
    DATA_ERC51_REC3,         // 
    DATA_ERC51_REC4,         // 
    DATA_ERC51_REC5,         // 

    DATA_ERC51_REC6,         // 
    DATA_ERC51_REC7,         // 
    DATA_ERC51_REC8,         // 
    DATA_ERC51_REC9,         // 
    DATA_ERC51_REC10,        // 

    DATA_ERC51_REC11,        // 
    DATA_ERC51_REC12,        // 
    DATA_ERC51_REC13,        // 
    DATA_ERC51_REC14,        // 
    DATA_ERC51_REC15,        // 

    DATA_ERC52_REC1,         // 
    DATA_ERC52_REC2,         // 
    DATA_ERC52_REC3,         // 
    DATA_ERC52_REC4,         // 
    DATA_ERC52_REC5,         // 

    DATA_ERC52_REC6,         // 
    DATA_ERC52_REC7,         // 
    DATA_ERC52_REC8,         // 
    DATA_ERC52_REC9,         // 
    DATA_ERC52_REC10,        // 

    DATA_ERC52_REC11,        // 
    DATA_ERC52_REC12,        // 
    DATA_ERC52_REC13,        // 
    DATA_ERC52_REC14,        // 
    DATA_ERC52_REC15,        // 

    DATA_ERC53_REC1,         // 
    DATA_ERC53_REC2,         // 
    DATA_ERC53_REC3,         // 
    DATA_ERC53_REC4,         // 
    DATA_ERC53_REC5,         // 

    DATA_ERC53_REC6,         // 
    DATA_ERC53_REC7,         // 
    DATA_ERC53_REC8,         // 
    DATA_ERC53_REC9,         // 
    DATA_ERC53_REC10,        // 

    DATA_ERC53_REC11,        // 
    DATA_ERC53_REC12,        // 
    DATA_ERC53_REC13,        // 
    DATA_ERC53_REC14,        // 
    DATA_ERC53_REC15,        // 

    DATA_ERC54_REC1,         // 
    DATA_ERC54_REC2,         // 
    DATA_ERC54_REC3,         // 
    DATA_ERC54_REC4,         // 
    DATA_ERC54_REC5,         // 

    DATA_ERC54_REC6,         // 
    DATA_ERC54_REC7,         // 
    DATA_ERC54_REC8,         // 
    DATA_ERC54_REC9,         // 
    DATA_ERC54_REC10,        // 

    DATA_ERC54_REC11,        // 
    DATA_ERC54_REC12,        // 
    DATA_ERC54_REC13,        // 
    DATA_ERC54_REC14,        // 
    DATA_ERC54_REC15,        // 

    DATA_ERC55_REC1,         // 
    DATA_ERC55_REC2,         // 
    DATA_ERC55_REC3,         // 
    DATA_ERC55_REC4,         // 
    DATA_ERC55_REC5,         // 

    DATA_ERC55_REC6,         // 
    DATA_ERC55_REC7,         // 
    DATA_ERC55_REC8,         // 
    DATA_ERC55_REC9,         // 
    DATA_ERC55_REC10,        // 

    DATA_ERC55_REC11,        // 
    DATA_ERC55_REC12,        // 
    DATA_ERC55_REC13,        // 
    DATA_ERC55_REC14,        // 
    DATA_ERC55_REC15,        // 

    DATA_ERC56_REC1,         // 
    DATA_ERC56_REC2,         // 
    DATA_ERC56_REC3,         // 
    DATA_ERC56_REC4,         // 
    DATA_ERC56_REC5,         // 

    DATA_ERC56_REC6,         // 
    DATA_ERC56_REC7,         // 
    DATA_ERC56_REC8,         // 
    DATA_ERC56_REC9,         // 
    DATA_ERC56_REC10,        // 

    DATA_ERC56_REC11,        // 
    DATA_ERC56_REC12,        // 
    DATA_ERC56_REC13,        // 
    DATA_ERC56_REC14,        // 
    DATA_ERC56_REC15,        // 

    DATA_ERC57_REC1,         // 
    DATA_ERC57_REC2,         // 
    DATA_ERC57_REC3,         // 
    DATA_ERC57_REC4,         // 
    DATA_ERC57_REC5,         // 

    DATA_ERC57_REC6,         // 
    DATA_ERC57_REC7,         // 
    DATA_ERC57_REC8,         // 
    DATA_ERC57_REC9,         // 
    DATA_ERC57_REC10,        // 

    DATA_ERC57_REC11,        // 
    DATA_ERC57_REC12,        // 
    DATA_ERC57_REC13,        // 
    DATA_ERC57_REC14,        // 
    DATA_ERC57_REC15,        // 

    DATA_ERC58_REC1,         // 
    DATA_ERC58_REC2,         // 
    DATA_ERC58_REC3,         // 
    DATA_ERC58_REC4,         // 
    DATA_ERC58_REC5,         // 

    DATA_ERC58_REC6,         // 
    DATA_ERC58_REC7,         // 
    DATA_ERC58_REC8,         // 
    DATA_ERC58_REC9,         // 
    DATA_ERC58_REC10,        // 

    DATA_ERC58_REC11,        // 
    DATA_ERC58_REC12,        // 
    DATA_ERC58_REC13,        // 
    DATA_ERC58_REC14,        // 
    DATA_ERC58_REC15,        // 

    DATA_ERC59_REC1,         // 
    DATA_ERC59_REC2,         // 
    DATA_ERC59_REC3,         // 
    DATA_ERC59_REC4,         // 
    DATA_ERC59_REC5,         // 

    DATA_ERC59_REC6,         // 
    DATA_ERC59_REC7,         // 
    DATA_ERC59_REC8,         // 
    DATA_ERC59_REC9,         // 
    DATA_ERC59_REC10,        // 

    DATA_ERC59_REC11,        // 
    DATA_ERC59_REC12,        // 
    DATA_ERC59_REC13,        // 
    DATA_ERC59_REC14,        // 
    DATA_ERC59_REC15,        // 

    DATA_ERC60_REC1,         // 
    DATA_ERC60_REC2,         // 
    DATA_ERC60_REC3,         // 
    DATA_ERC60_REC4,         // 
    DATA_ERC60_REC5,         // 

    DATA_ERC60_REC6,         // 
    DATA_ERC60_REC7,         // 
    DATA_ERC60_REC8,         // 
    DATA_ERC60_REC9,         // 
    DATA_ERC60_REC10,        // 

    DATA_ERC60_REC11,        // 
    DATA_ERC60_REC12,        // 
    DATA_ERC60_REC13,        // 
    DATA_ERC60_REC14,        // 
    DATA_ERC60_REC15,        // 

    DATA_ERC61_REC1,         // 
    DATA_ERC61_REC2,         // 
    DATA_ERC61_REC3,         // 
    DATA_ERC61_REC4,         // 
    DATA_ERC61_REC5,         // 

    DATA_ERC61_REC6,         // 
    DATA_ERC61_REC7,         // 
    DATA_ERC61_REC8,         // 
    DATA_ERC61_REC9,         // 
    DATA_ERC61_REC10,        // 

    DATA_ERC61_REC11,        // 
    DATA_ERC61_REC12,        // 
    DATA_ERC61_REC13,        // 
    DATA_ERC61_REC14,        // 
    DATA_ERC61_REC15,        // 

    DATA_ERC62_REC1,         // 
    DATA_ERC62_REC2,         // 
    DATA_ERC62_REC3,         // 
    DATA_ERC62_REC4,         // 
    DATA_ERC62_REC5,         // 

    DATA_ERC62_REC6,         // 
    DATA_ERC62_REC7,         // 
    DATA_ERC62_REC8,         // 
    DATA_ERC62_REC9,         // 
    DATA_ERC62_REC10,        // 

    DATA_ERC62_REC11,        // 
    DATA_ERC62_REC12,        // 
    DATA_ERC62_REC13,        // 
    DATA_ERC62_REC14,        // 
    DATA_ERC62_REC15,        // 

    DATA_ERC63_REC1,         // 
    DATA_ERC63_REC2,         // 
    DATA_ERC63_REC3,         // 
    DATA_ERC63_REC4,         // 
    DATA_ERC63_REC5,         // 

    DATA_ERC63_REC6,         // 
    DATA_ERC63_REC7,         // 
    DATA_ERC63_REC8,         // 
    DATA_ERC63_REC9,         // 
    DATA_ERC63_REC10,        // 

    DATA_ERC63_REC11,        // 
    DATA_ERC63_REC12,        // 
    DATA_ERC63_REC13,        // 
    DATA_ERC63_REC14,        // 
    DATA_ERC63_REC15,        // 

    DATA_ERC64_REC1,         // 
    DATA_ERC64_REC2,         // 
    DATA_ERC64_REC3,         // 
    DATA_ERC64_REC4,         // 
    DATA_ERC64_REC5,         // 

    DATA_ERC64_REC6,         // 
    DATA_ERC64_REC7,         // 
    DATA_ERC64_REC8,         // 
    DATA_ERC64_REC9,         // 
    DATA_ERC64_REC10,        // 

    DATA_ERC64_REC11,        // 
    DATA_ERC64_REC12,        // 
    DATA_ERC64_REC13,        // 
    DATA_ERC64_REC14,        // 
    DATA_ERC64_REC15,        // 

    Radio_Update_Try_Count,  // 1字节，电台升级失败尝试次数
    Modem_Run_Ver,           // 4字节，modem运行版本	ASCII 4字节 当前版本
    Modem_Run_BSP_Ver,       // 4字节，modem运行硬件版本	ASCII 4字节 当前版本
    MODU_ADRESS_ID,          // 6字节，BCD，主节点模块地址
    Chip_Code_ID,            // 2字节，BCD，芯片代码

    Route_Code_ID,           // 2字节，BCD，路由模块厂商编码
    VERSION_ID,              // 2字节，BCD，版本
    DATA_RT_TERM_COMPLEX_AREA_TEST,// 各地区增补的实时数据（RT_DATA）——终端

DATA_RT_TERM_END_ID,
};

/******************--2 终端日计算数据项--*********************/
enum data_cal_day_term
{
    OIL_MAX_REC=0,           // 2+6字节,油温最大值记录(最值,2个字节, 最值出现时间BCD,6个字节)
    OIL_MIN_REC,             // 2+6字节,油温最小值记录(最值,2个字节, 最值出现时间BCD,6个字节)
    OIL_OU_TACC,             // 2字节,油温越限累计时间,包括越上限和越下限
    CAP_SWT_TACC,            // 20字节,电容投运累计时间,BIN [1..10] （分）
    TMR_RUN_TACC,            // 4字节,终端日供电时间

    TMR_RESET_ACC,           // 2字节,终端日复位次数
    DAY_YK_ACC,              // 1字节,日遥控次数
    DAY_JD_ACC,              // 1字节,日拒动次数
    DAY_REST_SWT_ACC,        // 1字节,日厂休控跳闸次数
    YX_CHANGE_ACC,           // 2字节,遥信变位次数

    EC_SWT_DAY_ACC,          // 1字节,日日电控跳闸次数
    EC_SWT_MON_ACC,          // 1字节,日月电控跳闸次数
    BC_SWT_ACC,              // 1字节,日购电控跳闸次数
    PC_SWT_ACC,              // 1字节,日功控跳闸次数
    AP_OU_ACC,               // 1字节,日功率越限次数

    TMP_LMT_SWT_ACC,         // 1字节,日临时限电次数
    RE_COMP_PUT_ACC,         // 1字节,日无功投切投入次数
    RE_COMP_CUT_ACC,         // 1字节,日无功投切切除次数
    TMN_HALT_TMS_DAY,        // 2字节,终端日停电次数
    TMN_HALT_ACCT_DAY,       // 4字节,终端日停电累积时间

    DC_ANOLOGY1_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY1_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY1_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY1_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY2_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte

    DC_ANOLOGY2_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY2_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY2_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY3_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY3_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte

    DC_ANOLOGY3_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY3_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY4_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY4_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY4_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6

    DC_ANOLOGY4_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY5_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY5_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY5_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY5_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte

    DC_ANOLOGY6_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY6_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY6_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY6_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY7_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte

    DC_ANOLOGY7_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY7_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY7_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    DC_ANOLOGY8_HIGH_TACC_DAY,// 日模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY8_LOW_TACC_DAY,// 日模拟量越下限累计时间(day) 2byte

    DC_ANOLOGY8_MAX_REC_DAY, // 日模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年） 2＋6
    DC_ANOLOGY8_MIN_REC_DAY, // 日模拟量最小值及其发生时间8byte
    Tmn_Traffic_Cnt,         // 4字节，终端当日通信流量,BIN
    DATA_CAL_DAY_TERM_COMPLEX_AREA_TEST,// 各地区增补的历史日数据（CALC_DATA_DAY）——终端

DATA_CAL_DAY_TERM_END_ID,
};

/******************--3 终端日曲线数据项--*********************/
enum data_curve_term
{
    OIL_TEMP_VAL=0,          // 2字节,变压器油温值
    HUMIDITY_VAL,            // 2字节,变压器湿度值
    DC_Anology1_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量1曲线
    DC_Anology2_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量2曲线
    DC_Anology3_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量3曲线

    DC_Anology4_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量4曲线
    DC_Anology5_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量5曲线
    DC_Anology6_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量6曲线
    DC_Anology7_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量7曲线
    DC_Anology8_Curve,       // 2字节，其数据格式参见04规约数据格式02.,直流模拟量8曲线

    NOISE_VOICE_RATE,        // 1个字节，BIN噪声比值
    NOISE_COLOR_RATE,        // 1个字节，BIN噪声比值
    MAIN_NODE_ADDR,          // 6个字节，BCD对应噪声比值1的主节点地址

DATA_CURVE_TERM_END_ID,
};

/******************--4 终端记录数据项--*********************/
enum data_rec_term
{
    OIL_OU_REC_DAY=0,        // 2+6+6=14字节,油温超上限记录(最大值,2个字节,出现时间BCD,6个字节,结束时间BCD,6个字节）
    SWH_CAP_REC_DAY,         // 1+4+2+4+2=13字节,电容投切记录
                                // 投切定位及动作类型1字节,动作前无功功率4字节
                                // 动作前电压2字节,动作前无功功率4字节,动作前电压2字节
    HALT_REC_DAY,            // 6+6=12字节,终端失电纪录(开始时间BCD,6字节,结束时间BCD,6字节）
    LMT_SWT_REC_DAY,         // 1+6+4+4+4=19字节,临时限电跳闸记录
                                // 跳闸轮次1字节,跳闸时间6字节,跳闸前功率4字节 跳闸后功率4字节,跳闸时功率定值4字节
    TMR_RESET_REC_DAY,       // 6+6=12字节,终端复位记录(开始时间BCD6字节,结束时间BCD6字节

    DATA_INIT_REC_DAY,       // 6+2=8字节,数据初始化记录(初始化时间BCD6字节,终端软件版本号2字节）
    PARA_LOST_REC_DAY,       // 6+2=8字节,参数丢失记录（参数丢失记录时间BCD6字节,参数标志2字节）
    IMP_PARA_CHANGE_REC_DAY, // 6+1+1+2+4+4=18字节,重要参数变更记录
                                // 参数更新时间BCD6字节,参数代码1字节 信息点号1字节,参数长度2字节,变更前参数4字节,变更后参数4字节
    STATUS_CHANGE_REC_DAY,   // 49字节,状态量变位记录(变位时间BCD6字节,信息点号（开关号1-8）1字节
                                // 变位后开关状态1字节,相关电能表信息点(表序号1-8)1字节
                                // 相关电能表正向有功总读数4字节,相关电能表费率1正向有功读数4字节
                                // 相关电能表费率2正向有功读数4字节,相关电能表费率3正向有功读数4字节
                                // 相关电能表费率4正向有功读数4字节,相关电能表反向有功总读数4字节
                                // 相关电能表费率1反向有功读数4字节,相关电能表费率2反向有功读数4字节
                                // 相关电能表费率3反向有功读数4字节,相关电能表费率4反向有功读数4字节)
    RC_SWT_REC_DAY,          // 16字节,遥控跳闸记录(跳闸时间BCD6字节,跳闸轮次1字节,跳闸轮次组1字节
                                // 跳闸前功率4字节,跳闸后2分钟的功率4字节

    PC_SWT_REC_DAY,          // 20字节,功控跳闸记录(跳闸时间BCD6字节,跳闸轮次1字节,跳闸原因及轮次组1字节
                                // 跳闸前功率4字节,跳闸后2分钟的功率4字节,跳闸时功率定值4字节）
    EC_SWT_REC_DAY,          // 20字节,电控跳闸记录(跳闸时间BCD6字节,跳闸轮次1字节,跳闸原因及轮次组1字节
                                // 跳闸前功率4字节,跳闸时的电量（日电量/月电量/剩余电量（费））4字节
                                // 跳闸时电量定值（日电量定值/月电量定值/最近
                                // 一次购电量（费））4字节
    TMN_VER_CHANGE_DAY,      // 10字节,终端软件版本变更记录
                                // 发生时间BCD6字节,变更前软件版本号2字节    变更后软件版本号2字节
    ERROR_PASS_REC_DAY,      // 8字节,口令错误记录（发生时间BCD6字节,错误口令2字节）
    TMN_TROUBLE_REC_DAY,     // 7字节,终端故障记录（发生时间BCD6字节,终端故障编码1字节）

    YAOXIN_SWH_REC_DAY,      // 8字节,遥信变位记录(变位前的开关状态1字节,变位后的开关状态1字节,变位时间BCD6字节）
    BC_SWT_REC_DAY,          // 20字节,购电控跳闸记录(跳闸时间BCD6字节,跳闸轮次1字节
                                // 跳闸原因及轮次组1字节,跳闸前功率4字节
                                // 跳闸时的电量（日电量/月电量/剩余电量（费））4字节
                                // 跳闸时电量定值（日电量定值/月电量定值/最近一次购电量（费））4字节）

DATA_REC_TERM_END_ID,
};

/******************--5 终端月计算数据项--*********************/
enum data_cal_mon_term
{
    YK_ACC_MON=0,            // 1字节, BIN, 月遥控次数
    JD_ACC_MON,              // 1字节, BIN, 月拒动次数
    REST_SWT_ACC_MON,        // 1字节, BIN, 月厂休控跳闸次数
    PC_SWT_ACC_MON,          // 1字节, BIN, 月功控跳闸次数
    TMR_RUN_TACC_MON,        // 4字节, BIN(分), 终端月供电时间

    TMR_RESET_ACC_MON,       // 2字节, BIN, 终端月复位次数
    YX_CHANGE_ACC_MON,       // 2字节, BIN, 遥信变位次数(月)
    EC_SWT_DAY_ACC_MON,      // 1字节, BIN, 月日电控跳闸次数
    EC_SWT_MONTH_ACC_MON,    // 1字节, BIN, 月月电控跳闸次数
    BC_SWT_ACC_MON,          // 1字节, BIN, 月购电控跳闸次数

    TMP_LMT_SWT_MACC,        // 1字节, BIN, 月临时限电次数
    OIL_MAX_REC_MON,         // 8字节, 油温最大值记录(月) 最值2字节 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    OIL_MIN_REC_MON,         // 8字节, 油温最小值记录(月) 最值2字节 出现时间6字节, BCD(秒分时日月年, 低字节->高字节)
    OIL_OU_TACC_MON,         // 2字节, BIN(分), 油温超上限时间
    OIL_OD_TACC_MON,         // 2字节, BIN(分), 油温超下限时间

    CAP_SWT_PACC,            // 24字节, BCD(秒分时日月年, 低字节->高字节), 分相电容器投切次数
    CAP_SWT_ZACC,            // 8字节, BCD(秒分时日月年, 低字节->高字节), 合相电容器投切次数
    CAP_TOTAL_ACC,           // 1字节, 电容器动作总次数
    TMN_HALT_TMS_MON,        // 2字节, 终端月停电次数
    TMN_HALT_ACCT_MON,       // 4字节, 终端月停电累积时间

    DC_ANOLOGY1_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY1_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY1_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY1_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY2_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte

    DC_ANOLOGY2_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY2_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY2_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY3_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY3_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte

    DC_ANOLOGY3_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY3_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY4_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY4_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY4_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6

    DC_ANOLOGY4_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY5_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY5_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY5_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY5_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte

    DC_ANOLOGY6_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY6_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY6_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY6_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY7_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte

    DC_ANOLOGY7_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte
    DC_ANOLOGY7_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY7_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    DC_ANOLOGY8_HIGH_TACC_MON,// 月模拟量越上限累计时间(day) bin  2byte
    DC_ANOLOGY8_LOW_TACC_MON,// 月模拟量越下限累计时间(day) 2byte

    DC_ANOLOGY8_MAX_REC_MON, // 月模拟量最大值(其数据格式参见04规约数据格式02)及其发生时间（秒分时日月年）  2＋6
    DC_ANOLOGY8_MIN_REC_MON, // 月模拟量最小值及其发生时间8byte
    TMN_Traffic_FLUX_MON,    // 4字节，终端当月通信流量,BIN
    DATA_CAL_MON_TERM_COMPLEX_AREA_TEST,// 各地区增补的历史月数据（CALC_DATA_MON）——终端

DATA_CAL_MON_TERM_END_ID,
};

/******************--6 终端月记录数据项--*********************/
enum data_rec_mon_term
{
    HALT_REC_MON=0,          // 12字节, 终端失电纪录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 开始时间;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 结束时间
    LMT_SWT_REC_MON,         // 19字节, 临时限电跳闸记录
                                // #1: 1字节, 跳闸轮次;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 跳闸时间;
                                // #3: 4字节, BIN x 10000(kw), 跳闸前功率;
                                // #4: 4字节, BIN x 10000(kw), 跳闸后功率;
                                // #5: 4字节, BIN x 10000(kw), 跳闸时功率定值
    TMR_RESET_REC_MON,       // 12字节, 终端复位记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 开始时间;
                                // #2: 6字节, BCD(秒分时日月年, 低字节->高字节), 结束时间
    DATA_INIT_REC_MON,       // 8字节, 数据初始化记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 初始化时间;
                                // #2: 2字节, 终端软件版本号
    PARA_LOST_REC_MON,       // 8字节, 参数丢失记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 参数丢失记录时间;
                                // #2: 2字节，参数标志

    IMP_PARA_CHANGE_REC_MON, // 18字节, 重要参数变更记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 参数更新时间;
                                // #2:	1字节, 参数代码;
                                // #3: 1字节, 信息点号;
                                // #4: 2字节, 参数长度;
                                // #5: 4字节, 变更前参数;
                                // #6: 4字节, 变更后参数
    STATUS_CHANGE_REC_MON,   // 49字节, 状态量变位记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 变位时间;
                                // #2: 1字节, 信息点号（开关号1-8）;
                                // #3: 1字节, 变位后开关状态;
                                // #4: 1字节, 相关电能表信息点(表序号1-8);
                                // #5: 4字节, 相关电能表正向有功总读数;
                                // #6: 4字节, 相关电能表费率1正向有功读数;
                                // #7: 4字节, 相关电能表费率2正向有功读数;
                                // #8: 4字节, 相关电能表费率3正向有功读数;
                                // #9: 4字节, 相关电能表费率4正向有功读数;
                                // #10:4字节, 相关电能表反向有功总读数;
                                // #11:4字节, 相关电能表费率1反向有功读数;
                                // #12:4字节, 相关电能表费率2反向有功读数;
                                // #13:4字节, 相关电能表费率3反向有功读数;
                                // #14:4字节, 相关电能表费率4反向有功读数
    RC_SWT_REC_MON,          // 16字节, 遥控跳闸记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 跳闸时间;
                                // #2: 1字节, 跳闸轮次;
                                // #3: 1字节, 跳闸轮次组;
                                // #4: 4字节, BIN x 10000(kw), 跳闸前功率;
                                // #5: 4字节, BIN x 10000(kw), 跳闸后2分钟的功率
    PC_SWT_REC_MON,          // 20字节, 功控跳闸记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 跳闸时间;
                                // #2: 1字节, 跳闸轮次;
                                // #3: 1字节, 跳闸原因及轮次组;
                                // #4: 4字节, BIN x 10000(kw), 跳闸前功率;
                                // #5: 4字节, BIN x 10000(kw), 跳闸后2分钟的功率;
                                // #6: 4字节, BIN x 10000(kw), 跳闸时功率定值
    EC_SWT_REC_MON,          // 20字节, 电控跳闸记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 跳闸时间;
                                // #2: 1字节, 跳闸轮次;
                                // #3: 1字节, 跳闸原因及轮次组;
                                // #4: 4字节, BIN x 10000(kw), 跳闸前功率;
                                // #5: 4字节, BIN x 1000(kwh), 跳闸时的电量（日电量/月电量/剩余电量（费））;
                                // #6: 4字节, BIN x 1000(kwh), 跳闸时电量定值（日电量定值/月电量定值/最近一次购电量（费）））

    TMN_VER_CHANGE_REC_MON,  // 10字节, 终端软件版本变更记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 发生时间;
                                // #2: 2字节, 变更前软件版本号;
                                // #3: 2字节, 变更后软件版本号
    ERROR_PASS_REC_MON,      // 8字节, 口令错误记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 发生时间;
                                // #2: 2字节, 错误口令
    TMN_TROUBLE_REC_MON,     // 7字节, 终端故障记录
                                // #1: 6字节, BCD(秒分时日月年, 低字节->高字节), 发生时间;
                                // #2: 1字节 ,终端故障编码
    YAOXIN_SWH_REC_MON,      // 8字节, 遥信变位记录
                                // #1: 1字节, 变位前的开关状态;
                                // #2: 1字节, 变位后的开关状态;
                                // #3: 6字节, 变位时间
    BC_SWT_REC_MON,          // 20字节, 购电控跳闸记录
                                // #1: 6字节, 跳闸时间;
                                // #2: 1字节, 跳闸轮次;
                                // #3: 1字节, 跳闸原因及轮次组;
                                // #4: 4字节, BIN x 10000(kw), 跳闸前功率;
                                // #5: 4字节, BIN x 1000(kwh), 跳闸时的电量（日电量/月电量/剩余电量（费））;
                                // #6: 4字节, BIN x 1000(kwh), 跳闸时电量定值（日电量定值/月电量定值/最近一次购电量（费）））


DATA_REC_MON_TERM_END_ID,
};



/**********************-各类型数据项数目定义-**************************/

/************************-计量点各类型数据数目-**************************/
#define JL_CURRENT_NUM         DATA_RT_END_ID		    //--计量点当前时刻采集数据项个数--
#define JL_POINT_NUM           DATA_POINT_DAY_END_ID   //--计量点日定点数据项个数--
#define JL_CURVE_NUM           DATA_CURVE_END_ID       //--计量点曲线数据项个数--
#define JL_CALC_NUM            DATA_CAL_DAY_END_ID     //--计量点日计算数据项个数--
#define JL_REC_NUM             DATA_REC_DAY_END_ID     //--计量点日记录数据项个数--
#define JL_MINCURVE_NUM        DATA_MINCURVE_END_ID    //--计量点日分钟曲线数据项个数--
#define JL_POINT_MON_NUM       DATA_POINT_MON_END_ID   //--计量点月定点数据项个数--
#define JL_CALC_MON_NUM        DATA_CAL_MON_END_ID     //--计量点月计算数据项个数--
#define JL_REC_MON_NUM         DATA_REC_MON_END_ID     //--计量点月记录数据项个数--

/************************-终端各类型数据数目-**************************/
#define TMN_CURRENT_NUM        DATA_RT_TERM_END_ID     //- 终端当前时刻采集数据项个数 -
#define TMN_CALC_NUM           DATA_CAL_DAY_TERM_END_ID //- 终端日计算数据项个数 -
#define TMN_CURVE_NUM          DATA_CURVE_TERM_END_ID  //- 终端日曲线数据项个数 -
#define TMN_MINCURVE_NUM       0                       //- 终端日分钟曲线数据项个数 -
#define TMN_REC_NUM            DATA_REC_TERM_END_ID    //- 终端记录数据项个数 -
#define TMN_CALC_MON_NUM       DATA_CAL_MON_TERM_END_ID //- 终端月计算数据项个数 -
#define TMN_REC_MON_NUM        DATA_REC_MON_TERM_END_ID //- 终端月记录数据项个数 -

#define MTR_RT_DATE_AREA_BEGIN_ID	DATA_RT_COMPLEX_AREA_TEST	//- 各地区增补的实时数据起始ID（测量点） -
#define RTU_RT_DATE_AREA_BEGIN_ID	DATA_RT_TERM_COMPLEX_AREA_TEST		//- 各地区增补的实时数据起始ID（终端） -

extern const uint16 jl_current_len[];
extern const uint16 jl_calc_len[];
extern const uint16 jl_point_len[];
extern const uint16 jl_curve_len[];
extern const uint16 jl_mincurve_len[];
extern const uint16 jl_rec_len[];
extern const uint16 jl_calc_mon_len[];
extern const uint16 jl_point_mon_len[];
extern const uint16 jl_rec_mon_len[];
extern const uint16 tmn_current_len[TMN_CURRENT_NUM];
extern const uint16 tmn_calc_len[TMN_CALC_NUM];
extern const uint16 tmn_curve_len[TMN_CURVE_NUM];
extern const uint16 tmn_rec_len[TMN_REC_NUM];
extern const uint16 tmn_calc_mon_len[TMN_CALC_MON_NUM];
extern const uint16 tmn_rec_mon_len[TMN_REC_MON_NUM];

#endif /* #ifndef _DATA_ID_H */

