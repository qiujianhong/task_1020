/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief����Լ������
* @date�� 2019.12.03
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"

//
//OAD�б�
//

/* ��������OAD�б� OIA1 = 0x00 */
const OAD_TAB_T oadEnergyInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                    len                          eData*/
    {{0x00000200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //����й����� - ������
    {{0x00000201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //����й�����-����1 - ������
    {{0x00000400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //����й����� - �߾��ȵ�����
    {{0x00000401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //����й�����-����1 - ������
    {{0x00100200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й����� - ������
    {{0x00100201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й�����-����1 - ������
    {{0x00100400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й����� - �߾��ȵ�����
    {{0x00100401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й�����-����1 - ������
    {{0x00110200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�������й����� - ������
    {{0x00110201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�������й�����-����1 - ������
    {{0x00110400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�������й����� - �߾��ȵ�����
    {{0x00110401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�������й�����-����1 - ������
    {{0x00120200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�������й����� - ������
    {{0x00120201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�������й�����-����1 - ������
    {{0x00120400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�������й����� - �߾��ȵ�����
    {{0x00120401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�������й�����-����1 - ������
    {{0x00130200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�������й����� - ������
    {{0x00130201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�������й�����-����1 - ������
    {{0x00130400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�������й����� - �߾��ȵ�����
    {{0x00130401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�������й�����-����1 - ������
    {{0x00200200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й����� - ������
    {{0x00200201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й�����-����1 - ������
    {{0x00200400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й����� - �߾��ȵ�����
    {{0x00200401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й�����-����1 - ������
    {{0x00210200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�෴���й����� - ������
    {{0x00210201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�෴���й�����-����1 - ������
    {{0x00210400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�෴���й����� - �߾��ȵ�����
    {{0x00210401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�෴���й�����-����1 - ������
    {{0x00220200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�෴���й����� - ������
    {{0x00220201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�෴���й�����-����1 - ������
    {{0x00220400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�෴���й����� - �߾��ȵ�����
    {{0x00220401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�෴���й�����-����1 - ������
    {{0x00230200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�෴���й����� - ������
    {{0x00230201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�෴���й�����-����1 - ������
    {{0x00230400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�෴���й����� - �߾��ȵ�����
    {{0x00230401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�෴���й�����-����1 - ������
    {{0x00300200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //����޹�1���� - ������
    {{0x00300201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //����޹�1����-����1 - ������
    {{0x00300400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //����޹�1���� - �߾��ȵ�����
    {{0x00300401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //����޹�1����-����1 - ������
    {{0x00310200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //A������޹�1���� - ������
    {{0x00310201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //A������޹�1����-����1 - ������
    {{0x00310400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //A������޹�1���� - �߾��ȵ�����
    {{0x00310401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //A������޹�1����-����1 - ������
    {{0x00320200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //B������޹�1���� - ������
    {{0x00320201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //B������޹�1����-����1 - ������
    {{0x00320400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //B������޹�1���� - �߾��ȵ�����
    {{0x00320401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //B������޹�1����-����1 - ������
    {{0x00330200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //C������޹�1���� - ������
    {{0x00330201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //C������޹�1����-����1 - ������
    {{0x00330400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //C������޹�1���� - �߾��ȵ�����
    {{0x00330401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //C������޹�1����-����1 - ������
    {{0x00400200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //����޹�2���� - ������
    {{0x00400201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //����޹�2����-����1 - ������
    {{0x00400400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //����޹�2���� - �߾��ȵ�����
    {{0x00400401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //����޹�2����-����1 - ������
    {{0x00410200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //A������޹�2���� - ������
    {{0x00410201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //A������޹�2����-����1 - ������
    {{0x00410400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //A������޹�2���� - �߾��ȵ�����
    {{0x00410401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //A������޹�2����-����1 - ������
    {{0x00420200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //B������޹�2���� - ������
    {{0x00420201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //B������޹�2����-����1 - ������
    {{0x00420400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //B������޹�2���� - �߾��ȵ�����
    {{0x00420401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //B������޹�2����-����1 - ������
    {{0x00430200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGYS_T},         //C������޹�2���� - ������
    {{0x00430201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGYS_T),       E_OOP_ENERGYS_T},         //C������޹�2����-����1 - ������
    {{0x00430400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGYS_T},        //C������޹�2���� - �߾��ȵ�����
    {{0x00430401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGYS_T),      E_OOP_HENERGYS_T},        //C������޹�2����-����1 - ������
    {{0x00500200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //��һ�����޹����� - ������
    {{0x00500201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //��һ�����޹�����-����1 - ������
    {{0x00500400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //��һ�����޹����� - �߾��ȵ�����
    {{0x00500401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //��һ�����޹�����-����1 - ������
    {{0x00510200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A���һ�����޹����� - ������
    {{0x00510201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A���һ�����޹�����-����1 - ������
    {{0x00510400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A���һ�����޹����� - �߾��ȵ�����
    {{0x00510401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A���һ�����޹�����-����1 - ������
    {{0x00520200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B���һ�����޹����� - ������
    {{0x00520201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B���һ�����޹�����-����1 - ������
    {{0x00520400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B���һ�����޹����� - �߾��ȵ�����
    {{0x00520401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B���һ�����޹�����-����1 - ������
    {{0x00530200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C���һ�����޹����� - ������
    {{0x00530201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C���һ�����޹�����-����1 - ������
    {{0x00530400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C���һ�����޹����� - �߾��ȵ�����
    {{0x00530401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C���һ�����޹�����-����1 - ������
    {{0x00600200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�ڶ������޹����� - ������
    {{0x00600201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�ڶ������޹�����-����1 - ������
    {{0x00600400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�ڶ������޹����� - �߾��ȵ�����
    {{0x00600401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�ڶ������޹�����-����1 - ������
    {{0x00610200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A��ڶ������޹����� - ������
    {{0x00610201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A��ڶ������޹�����-����1 - ������
    {{0x00610400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A��ڶ������޹����� - �߾��ȵ�����
    {{0x00610401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A��ڶ������޹�����-����1 - ������
    {{0x00620200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B��ڶ������޹����� - ������
    {{0x00620201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B��ڶ������޹�����-����1 - ������
    {{0x00620400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B��ڶ������޹����� - �߾��ȵ�����
    {{0x00620401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B��ڶ������޹�����-����1 - ������
    {{0x00630200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C��ڶ������޹����� - ������
    {{0x00630201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C��ڶ������޹�����-����1 - ������
    {{0x00630400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C��ڶ������޹����� - �߾��ȵ�����
    {{0x00630401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C��ڶ������޹�����-����1 - ������
    {{0x00700200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //���������޹����� - ������
    {{0x00700201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //���������޹�����-����1 - ������
    {{0x00700400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //���������޹����� - �߾��ȵ�����
    {{0x00700401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //���������޹�����-����1 - ������
    {{0x00710200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A����������޹����� - ������
    {{0x00710201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A����������޹�����-����1 - ������
    {{0x00710400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A����������޹����� - �߾��ȵ�����
    {{0x00710401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A����������޹�����-����1 - ������
    {{0x00720200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B����������޹����� - ������
    {{0x00720201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B����������޹�����-����1 - ������
    {{0x00720400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B����������޹����� - �߾��ȵ�����
    {{0x00720401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B����������޹�����-����1 - ������
    {{0x00730200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C����������޹����� - ������
    {{0x00730201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C����������޹�����-����1 - ������
    {{0x00730400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C����������޹����� - �߾��ȵ�����
    {{0x00730401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C����������޹�����-����1 - ������
    {{0x00800200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //���������޹����� - ������
    {{0x00800201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_ENERGY_T},          //���������޹�����-����1 - ������
    {{0x00800400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //���������޹����� - �߾��ȵ�����
    {{0x00800401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //���������޹�����-����1 - ������
    {{0x00810200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A����������޹����� - ������
    {{0x00810201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A����������޹�����-����1 - ������
    {{0x00810400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A����������޹����� - �߾��ȵ�����
    {{0x00810401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A����������޹�����-����1 - ������
    {{0x00820200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B����������޹����� - ������
    {{0x00820201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B����������޹�����-����1 - ������
    {{0x00820400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B����������޹����� - �߾��ȵ�����
    {{0x00820401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B����������޹�����-����1 - ������
    {{0x00830200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C����������޹����� - ������
    {{0x00830201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C����������޹�����-����1 - ������
    {{0x00830400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C����������޹����� - �߾��ȵ�����
    {{0x00830401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C����������޹�����-����1 - ������
    {{0x00900200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�������ڵ��� - ������
    {{0x00900201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�������ڵ���-����1 - ������
    {{0x00900400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�������ڵ��� - �߾��ȵ�����
    {{0x00900401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�������ڵ���-����1 - ������
    {{0x00910200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A���������ڵ��� - ������
    {{0x00910201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A���������ڵ���-����1 - ������
    {{0x00910400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A���������ڵ��� - �߾��ȵ�����
    {{0x00910401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A���������ڵ���-����1 - ������
    {{0x00920200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B���������ڵ��� - ������
    {{0x00920201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B���������ڵ���-����1 - ������
    {{0x00920400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B���������ڵ��� - �߾��ȵ�����
    {{0x00920401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B���������ڵ���-����1 - ������
    {{0x00930200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C���������ڵ��� - ������
    {{0x00930201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C���������ڵ���-����1 - ������
    {{0x00930400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C���������ڵ��� - �߾��ȵ�����
    {{0x00930401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C���������ڵ���-����1 - ������
    {{0x00A00200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�������ڵ��� - ������
    {{0x00A00201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�������ڵ���-����1 - ������
    {{0x00A00400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�������ڵ��� - �߾��ȵ�����
    {{0x00A00401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�������ڵ���-����1 - ������
    {{0x00A10200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�෴�����ڵ��� - ������
    {{0x00A10201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�෴�����ڵ���-����1 - ������
    {{0x00A10400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�෴�����ڵ��� - �߾��ȵ�����
    {{0x00A10401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�෴�����ڵ���-����1 - ������
    {{0x00A20200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�෴�����ڵ��� - ������
    {{0x00A20201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�෴�����ڵ���-����1 - ������
    {{0x00A20400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�෴�����ڵ��� - �߾��ȵ�����
    {{0x00A20401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�෴�����ڵ���-����1 - ������
    {{0x00A30200}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�෴�����ڵ��� - ������
    {{0x00A30201}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�෴�����ڵ���-����1 - ������
    {{0x00A30400}, 1,       0x00,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�෴�����ڵ��� - �߾��ȵ�����
    {{0x00A30401}, 1,       0x00,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�෴�����ڵ���-����1 - ������
    {{0x01100200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й������ܵ��� - ������
    {{0x01100201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й������ܵ���-����1 - ������
    {{0x01100400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й������ܵ��� - �߾��ȵ�����
    {{0x01100401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й������ܵ���-����1 - ������
    {{0x01110200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�������й������ܵ��� - ������
    {{0x01110201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�������й������ܵ���-����1 - ������
    {{0x01110400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�������й������ܵ��� - �߾��ȵ�����
    {{0x01110401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�������й������ܵ���-����1 - ������
    {{0x01120200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�������й������ܵ��� - ������
    {{0x01120201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�������й������ܵ���-����1 - ������
    {{0x01120400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�������й������ܵ��� - �߾��ȵ�����
    {{0x01120401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�������й������ܵ���-����1 - ������
    {{0x01130200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�������й������ܵ��� - ������
    {{0x01130201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�������й������ܵ���-����1 - ������
    {{0x01130400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�������й������ܵ���
    {{0x01130401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�������й������ܵ���-����1 - ������
    {{0x01200200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й������ܵ��� - ������
    {{0x01200201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й������ܵ���-����1 - ������
    {{0x01200400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й������ܵ��� - �߾��ȵ�����
    {{0x01200401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й������ܵ���-����1 - ������
    {{0x01210200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�෴���й������ܵ��� - ������
    {{0x01210201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�෴���й������ܵ���-����1 - ������
    {{0x01210400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�෴���й������ܵ��� - �߾��ȵ�����
    {{0x01210401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�෴���й������ܵ���-����1 - ������
    {{0x01220200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�෴���й������ܵ��� - ������
    {{0x01220201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�෴���й������ܵ���-����1 - ������
    {{0x01220400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�෴���й������ܵ��� - �߾��ȵ�����
    {{0x01220401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�෴���й������ܵ���-����1 - ������
    {{0x01230200}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�෴���й������ܵ��� - ������
    {{0x01230201}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�෴���й������ܵ���-����1 - ������
    {{0x01230400}, 1,       0x00,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�෴���й������ܵ��� - �߾��ȵ�����
    {{0x01230401}, 1,       0x00,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�෴���й������ܵ���-����1 - ������
    {{0x02100200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й�г���ܵ��� - ������
    {{0x02100201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й�г���ܵ���-����1 - ������
    {{0x02100400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й�г���ܵ��� - �߾��ȵ�����
    {{0x02100401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й�г���ܵ���-����1 - ������
    {{0x02110200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�������й�г���ܵ��� - ������
    {{0x02110201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�������й�г���ܵ���-����1 - ������
    {{0x02110400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�������й�г���ܵ��� - �߾��ȵ�����
    {{0x02110401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�������й�г���ܵ���-����1 - ������
    {{0x02120200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�������й�г���ܵ��� - ������
    {{0x02120201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�������й�г���ܵ���-����1 - ������
    {{0x02120400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�������й�г���ܵ��� - �߾��ȵ�����
    {{0x02120401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�������й�г���ܵ���-����1 - ������
    {{0x02130200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�������й�г���ܵ��� - ������
    {{0x02130201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�������й�г���ܵ���-����1 - ������
    {{0x02130400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�������й�г���ܵ��� - �߾��ȵ�����
    {{0x02130401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�������й�г���ܵ���-����1 - ������
    {{0x02200200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й�г���ܵ��� - ������
    {{0x02200201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й�г���ܵ���-����1 - ������
    {{0x02200400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й�г���ܵ��� - �߾��ȵ�����
    {{0x02200401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й�г���ܵ���-����1 - ������
    {{0x02210200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�෴���й�г���ܵ��� - ������
    {{0x02210201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�෴���й�г���ܵ���-����1 - ������
    {{0x02210400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�෴���й�г���ܵ��� - �߾��ȵ�����
    {{0x02210401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�෴���й�г���ܵ���-����1 - ������
    {{0x02220200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�෴���й�г���ܵ��� - ������
    {{0x02220201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�෴���й�г���ܵ���-����1 - ������
    {{0x02220400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�෴���й�г���ܵ��� - �߾��ȵ�����
    {{0x02220401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�෴���й�г���ܵ���-����1 - ������
    {{0x02230200}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�෴���й�г���ܵ��� - ������
    {{0x02230201}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�෴���й�г���ܵ���-����1 - ������
    {{0x02230400}, 1,       0x00,   0x02, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�෴���й�г���ܵ��� - �߾��ȵ�����
    {{0x02230401}, 1,       0x00,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�෴���й�г���ܵ���-����1 - ������
    {{0x03000200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //ͭ���й��ܵ��ܲ����� - ������
    {{0x03000201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03000400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //ͭ���й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x03000401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03010200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A��ͭ���й��ܵ��ܲ����� - ������
    {{0x03010201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03010400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A��ͭ���й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x03010401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03020200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B��ͭ���й��ܵ��ܲ����� - ������
    {{0x03020201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03020400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B��ͭ���й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x03020401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03030200}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C��ͭ���й��ܵ��ܲ����� - ������
    {{0x03030201}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x03030400}, 1,       0x00,   0x03, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C��ͭ���й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x03030401}, 1,       0x00,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C��ͭ���й��ܵ��ܲ�����-����1 - ������
    {{0x04000200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����й��ܵ��ܲ����� - ������
    {{0x04000201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����й��ܵ��ܲ�����-����1 - ������
    {{0x04000400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x04000401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����й��ܵ��ܲ�����-����1 - ������
    {{0x04010200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A�������й��ܵ��ܲ����� - ������
    {{0x04010201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A�������й��ܵ��ܲ�����-����1 - ������
    {{0x04010400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A�������й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x04010401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A�������й��ܵ��ܲ�����-����1 - ������
    {{0x04020200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B�������й��ܵ��ܲ����� - ������
    {{0x04020201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B�������й��ܵ��ܲ�����-����1 - ������
    {{0x04020400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B�������й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x04020401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B�������й��ܵ��ܲ�����-����1 - ������
    {{0x04030200}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C�������й��ܵ��ܲ����� - ������
    {{0x04030201}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C�������й��ܵ��ܲ�����-����1 - ������
    {{0x04030400}, 1,       0x00,   0x04, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C�������й��ܵ��ܲ����� - �߾��ȵ�����
    {{0x04030401}, 1,       0x00,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C�������й��ܵ��ܲ�����-����1 - ������
    {{0x05000200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //�����ܵ��� - ������
    {{0x05000201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //�����ܵ���-����1 - ������
    {{0x05000400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //�����ܵ��� - �߾��ȵ�����
    {{0x05000401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //�����ܵ���-����1 - ������
    {{0x05010200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //A������ܵ��� - ������
    {{0x05010201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //A������ܵ���-����1 - ������
    {{0x05010400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //A������ܵ��� - �߾��ȵ�����
    {{0x05010401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //A������ܵ���-����1 - ������
    {{0x05020200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //B������ܵ��� - ������
    {{0x05020201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //B������ܵ���-����1 - ������
    {{0x05020400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //B������ܵ��� - �߾��ȵ�����
    {{0x05020401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //B������ܵ���-����1 - ������
    {{0x05030200}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_ENERGY_T},          //C������ܵ��� - ������
    {{0x05030201}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ENERGY_T),        E_OOP_ENERGY_T},          //C������ܵ���-����1 - ������
    {{0x05030400}, 1,       0x00,   0x05, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,           E_OOP_HENERGY_T},         //C������ܵ��� - �߾��ȵ�����
    {{0x05030401}, 1,       0x00,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_HENERGY_T),       E_OOP_HENERGY_T},         //C������ܵ���-����1 - ������
};               
const uint32 oadEnergyInfoListLen = sizeof(oadEnergyInfoList)/sizeof(oadEnergyInfoList[0]);

/* ���������OAD�б� OIA1 = 0x01 */
const OAD_TAB_T oadDemandInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                         len                    eData*/
    {{0x10100200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //�����й��������
    {{0x10100201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //�����й��������-����
    {{0x10110200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A�������й��������
    {{0x10110201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A�������й��������-����
    {{0x10120200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B�������й��������
    {{0x10120201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B�������й��������-����
    {{0x10130200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C�������й��������
    {{0x10130201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C�������й��������-����
    {{0x10200200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //�����й��������
    {{0x10200201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //�����й��������-����
    {{0x10210200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A�෴���й��������
    {{0x10210201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A�෴���й��������-����
    {{0x10220200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B�෴���й��������
    {{0x10220201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B�෴���й��������-����
    {{0x10230200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C�෴���й��������
    {{0x10230201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C�෴���й��������-����
    {{0x10300200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����޹�1�������
    {{0x10300201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����޹�1�������-����
    {{0x10310200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //A������޹�1�������
    {{0x10310201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //A������޹�1�������-����
    {{0x10320200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //B������޹�1�������
    {{0x10320201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //B������޹�1�������-����
    {{0x10330200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //C������޹�1�������
    {{0x10330201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //C������޹�1�������-����
    {{0x10400200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����޹�2�������
    {{0x10400201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����޹�2�������-����
    {{0x10410200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //A������޹�2�������
    {{0x10410201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //A������޹�2�������-����
    {{0x10420200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //B������޹�2�������
    {{0x10420201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //B������޹�2�������-����
    {{0x10430200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //C������޹�2�������
    {{0x10430201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //C������޹�2�������-����
    {{0x10500200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //��һ�����������
    {{0x10500201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //��һ�����������-����
    {{0x10510200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A���һ�����������
    {{0x10510201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A���һ�����������-����
    {{0x10520200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B���һ�����������
    {{0x10520201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B���һ�����������-����
    {{0x10530200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C���һ�����������
    {{0x10530201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C���һ�����������-����
    {{0x10600200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //�ڶ������������
    {{0x10600201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //�ڶ������������-����
    {{0x10610200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A��ڶ������������
    {{0x10610201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A��ڶ������������-����
    {{0x10620200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B��ڶ������������
    {{0x10620201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B��ڶ������������-����
    {{0x10630200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C��ڶ������������
    {{0x10630201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C��ڶ������������-����
    {{0x10700200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������������
    {{0x10700201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������������-����
    {{0x10710200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A����������������
    {{0x10710201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A����������������-����
    {{0x10720200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B����������������
    {{0x10720201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B����������������-����
    {{0x10730200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C����������������
    {{0x10730201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C����������������-����
    {{0x10800200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������������
    {{0x10800201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������������-����
    {{0x10810200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A����������������
    {{0x10810201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A����������������-����
    {{0x10820200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B����������������
    {{0x10820201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B����������������-����
    {{0x10830200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C����������������
    {{0x10830201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C����������������-����
    {{0x10900200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������������
    {{0x10900201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������������-����
    {{0x10910200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A�����������������
    {{0x10910201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A�����������������-����
    {{0x10920200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B�����������������
    {{0x10920201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B�����������������-����
    {{0x10930200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C�����������������
    {{0x10930201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C�����������������-����
    {{0x10A00200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������������
    {{0x10A00201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������������-����
    {{0x10A10200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //A�෴�������������
    {{0x10A10201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //A�෴�������������-����
    {{0x10A20200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //B�෴�������������
    {{0x10A20201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //B�෴�������������-����
    {{0x10A30200}, 2,       0x01,   0x00, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //C�෴�������������
    {{0x10A30201}, 2,       0x01,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //C�෴�������������-����
    {{0x11100200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������������й��������
    {{0x11100201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������������й��������-����
    {{0x11110200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A�������й��������
    {{0x11110201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A�������й��������-����
    {{0x11120200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B�������й��������
    {{0x11120201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B�������й��������-����
    {{0x11130200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C�������й��������
    {{0x11130201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C�������й��������-����
    {{0x11200200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڷ����й��������
    {{0x11200201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڷ����й��������-����
    {{0x11210200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A�෴���й��������
    {{0x11210201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A�෴���й��������-����
    {{0x11220200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B�෴���й��������
    {{0x11220201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B�෴���й��������-����
    {{0x11230200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C�෴���й��������
    {{0x11230201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C�෴���й��������-����
    {{0x11300200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //��������������޹�1�������
    {{0x11300201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //��������������޹�1�������-����
    {{0x11310200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������A������޹�1�������
    {{0x11310201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������A������޹�1�������-����
    {{0x11320200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������B������޹�1�������
    {{0x11320201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������B������޹�1�������-����
    {{0x11330200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������C������޹�1�������
    {{0x11330201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������C������޹�1�������-����
    {{0x11400200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //��������������޹�2�������
    {{0x11400201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //��������������޹�2�������-����
    {{0x11410200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������A������޹�2�������
    {{0x11410201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������A������޹�2�������-����
    {{0x11420200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������B������޹�2�������
    {{0x11420201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������B������޹�2�������-����
    {{0x11430200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMANDS_T},         //����������C������޹�2�������
    {{0x11430201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMANDS_T),      E_OOP_DEMANDS_T},          //����������C������޹�2�������-����
    {{0x11500200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڵ�һ�����������
    {{0x11500201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڵ�һ�����������-����
    {{0x11510200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A���һ�����������
    {{0x11510201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A���һ�����������-����
    {{0x11520200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B���һ�����������
    {{0x11520201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B���һ�����������-����
    {{0x11530200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C���һ�����������
    {{0x11530201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C���һ�����������-����
    {{0x11600200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڵڶ������������
    {{0x11600201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڵڶ������������-����
    {{0x11610200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A��ڶ������������
    {{0x11610201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A��ڶ������������-����
    {{0x11620200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B��ڶ������������
    {{0x11620201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B��ڶ������������-����
    {{0x11630200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C��ڶ������������
    {{0x11630201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C��ڶ������������-����
    {{0x11700200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڵ��������������
    {{0x11700201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڵ��������������-����
    {{0x11710200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A����������������
    {{0x11710201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A����������������-����
    {{0x11720200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B����������������
    {{0x11720201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B����������������-����
    {{0x11730200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C����������������
    {{0x11730201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C����������������-����
    {{0x11800200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڵ��������������
    {{0x11800201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڵ��������������-����
    {{0x11810200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A����������������
    {{0x11810201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A����������������-����
    {{0x11820200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B����������������
    {{0x11820201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B����������������-����
    {{0x11830200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C����������������
    {{0x11830201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C����������������-����
    {{0x11900200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //�������������������������
    {{0x11900201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //�������������������������-����
    {{0x11910200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A�����������������
    {{0x11910201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A�����������������-����
    {{0x11920200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B�����������������
    {{0x11920201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B�����������������-����
    {{0x11930200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C�����������������
    {{0x11930201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C�����������������-����
    {{0x11A00200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //���������ڷ��������������
    {{0x11A00201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //���������ڷ��������������-����
    {{0x11A10200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������A�෴�������������
    {{0x11A10201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������A�෴�������������-����
    {{0x11A20200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������B�෴�������������
    {{0x11A20201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������B�෴�������������-����
    {{0x11A30200}, 2,       0x01,   0x01, 0,      DT_ARRAY,                OOP_MAX_RATES + 1,          E_OOP_DEMAND_T},          //����������C�෴�������������
    {{0x11A30201}, 2,       0x01,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_DEMAND_T),       E_OOP_DEMAND_T},          //����������C�෴�������������-����
};
const uint32 oadDemandInfoListLen = sizeof(oadDemandInfoList)/sizeof(oadDemandInfoList[0]);

/* ������OAD�б� OIA1 = 0x02 */
const OAD_TAB_T oadVariateInfoList[] = 
{
    /*OAD          class    A1      A2    rsv     dataType                 len*/
    {{0x20000200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},  
    {{0x20000400}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(uint16),          E_LONG_UNSIGNED},
    {{0x20000600}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(uint16),          E_LONG_UNSIGNED},  //��ѹ  OOP_WORD3V_T���������������ṹ�壿����
//    {{0x20000201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A���ѹ
//    {{0x20000202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B���ѹ
//    {{0x20000203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C���ѹ
    {{0x20010200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_INT3V_T),     E_OOP_INT3V_T},                 //����
//    {{0x20010201}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //A�����
//    {{0x20010202}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //B�����
//    {{0x20010203}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //C�����
    {{0x20010400}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //N�����
    {{0x20010500}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //ʣ�����
    {{0x20010600}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //ʣ�����
    {{0x20010800}, 3,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //ʣ�����

    {{0x20020200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                 //��ѹ���  
//    {{0x20020201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A���ѹ���
//    {{0x20020202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B���ѹ���
//    {{0x20020203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C���ѹ���
    {{0x20030200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                 //��ѹ�������  
//    {{0x20030201}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //A���ѹ�������
//    {{0x20030202}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //B���ѹ�������
//    {{0x20030203}, 3,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //C���ѹ�������
    {{0x20040200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //�й�����
    {{0x20050200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //�޹�����
    {{0x20060200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //���ڹ���
    {{0x20070200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //һ����ƽ���й�����
    {{0x20080200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //һ����ƽ���޹�����
    {{0x20090200}, 4,       0x02,   0x00, 0,      DT_ARRAY,          sizeof(OOP_INT4V_T),           E_OOP_INT4V_T},                   //һ����ƽ�����ڹ���
    {{0x200A0200}, 4,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_INT4V_T),     E_OOP_LONG4V_T},                          //��������
    {{0x200B0200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                          //��ѹ����ʧ���
//    {{0x200B0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //A���ѹ����ʧ���
//    {{0x200B0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //B���ѹ����ʧ���
//    {{0x200B0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //C���ѹ����ʧ���
    {{0x200C0200}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                          //��������ʧ���
//    {{0x200C0201}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //A���������ʧ���
//    {{0x200C0202}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //B���������ʧ���
//    {{0x200C0203}, 3,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //C���������ʧ���
    {{0x200D0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //A���ѹг�����������ܼ�2��n�Σ�
    {{0x200D0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //B���ѹг�����������ܼ�2��n�Σ�
    {{0x200D0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //C���ѹг�����������ܼ�2��n�Σ�
    {{0x200D0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //��ѹг�����������г������
    {{0x200E0200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //A�����г�����������ܼ�2��n�Σ�
    {{0x200E0300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //B�����г�����������ܼ�2��n�Σ�
    {{0x200E0400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_T),  E_OOP_HARMONIC_T},                //C�����г�����������ܼ�2��n�Σ�
    {{0x200E0500}, 5,       0x02,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //����г�����������г������
    {{0x200F0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //����Ƶ��
    {{0x200F0A00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //A��Ƶ��
    {{0x200F0B00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //B��Ƶ��
    {{0x200F0C00}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //C��Ƶ��
    {{0x20100200}, 6,       0x02,   0x00, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //�����¶�
    {{0x20100400}, 6,       0x02,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_LONG3V_T),   E_OOP_LONG3V_T},                          //�����¶�

    {{0x20110200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //ʱ�ӵ�ص�ѹ
    {{0x20120200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //ͣ�糭���ص�ѹ
    {{0x20130200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //ʱ�ӵ�ع���ʱ��
    {{0x20140200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_METWORD_T),   E_OOP_METWORD_T},                 //���ܱ�����״̬��
    {{0x20140400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //����ģ��ϵͳ״̬��
    {{0x20140500}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //����ϵͳ״̬��
    {{0x20150200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //���ܱ�����ϱ�״̬��
    {{0x20150400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                 //���ܱ�����ϱ�ģʽ��
    {{0x20150500}, 6,       0x02,   0x00, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                           //�ϱ���ʽ
    {{0x20170200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //��ǰ�й�����
    {{0x20180200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //��ǰ�޹�����
    {{0x20190200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //��ǰ��������
    {{0x201A0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //��ǰ���
    {{0x201B0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //��ǰ���ʵ��
    {{0x201C0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //��ǰ���ݵ��
    {{0x201E0200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //�¼�����ʱ��
    {{0x20200200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //�¼�����ʱ��
    {{0x20210200}, 8,       0x02,   0x00, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),E_DATETIME_S},                    //���ݶ���ʱ��
    {{0x20220200}, 8,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�¼���¼��� 
    {{0x20230200}, 8,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�����¼��� 
    {{0x20240200}, 8,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTSOURCE_T), E_OOP_EVTSOURCE_T},               //�¼�����Դ     
    {{0x20260200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //��ѹ��ƽ����
    {{0x20270200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //������ƽ����
    {{0x20280200}, 6,       0x02,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),          E_LONG_UNSIGNED},                 //������
    {{0x20290200}, 6,       0x02,   0x00, 1,      DT_ARRAY,                4*sizeof(uint32),        E_DOUBLE_LONG_UNSIGNED},          //��ʱֵ????�Ҳ�����Ӧ�ṹ��
    {{0x202A0200}, 8,       0x02,   0x00, 0,      DT_TSA,                  sizeof(OOP_TSA_T),       E_TSA},  
    {{0x202C0200}, 8,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},  
    {{0x202D0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //����ǰ��͸֧���
    {{0x202E0200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ۼƹ�����
    {{0x20310200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�¶��õ���
    {{0x20320200}, 6,       0x02,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //���ݽ����õ���
    {{0x20330200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //A���ѹг����������2��n�Σ�
    {{0x20330300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //B���ѹг����������2��n�Σ�
    {{0x20330400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_U_VALUE_T),  E_OOP_HARMONIC_U_VALUE_T},                //C���ѹг����������2��n�Σ�
    {{0x20340200}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //A�����г����������2��n�Σ�
    {{0x20340300}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //B�����г����������2��n�Σ�
    {{0x20340400}, 5,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_HARMONIC_I_VALUE_T),  E_OOP_HARMONIC_I_VALUE_T},                //C�����г����������2��n�Σ�
    {{0x20350200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20360200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20370200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  
    {{0x20380200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_DWORD3V_T),    E_OOP_DWORD3V_T},  


    {{0x20400200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T),  E_OOP_BITSTR16_T},                //��������ִ��״̬��
    {{0x20410200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR16_T),  E_OOP_BITSTR16_T},                //�����������״̬��
    {{0x20420200}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�����������ɿ���״̬��
    {{0x20420400}, 6,       0x02,   0x00, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR32_T),  E_OOP_BITSTR32_T},                //��������ģ��״̬��
    {{0x20500200}, 6,       0x02,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOOPSTAT_T),  E_OOP_LOOPSTAT_T},                //������·״̬
    {{0x21000200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //��������ͳ�� - ͳ�����ڵ�λΪ���� - ͳ�ƽ����
    {{0x21000300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //��������ͳ�� - ͳ�����ڵ�λΪ���� - �����������Ա�
    {{0x21010200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //Сʱ����ͳ�� - ͳ�����ڵ�λΪСʱ - ͳ�ƽ����
    {{0x21010300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //Сʱ����ͳ�� - ͳ�����ڵ�λΪСʱ - �����������Ա�
    {{0x21020200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //������ͳ�� - ͳ�����ڵ�λΪ�� - ͳ�ƽ����
    {{0x21020300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //������ͳ�� - ͳ�����ڵ�λΪ�� - �����������Ա�
    {{0x21030200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //������ͳ�� - ͳ�����ڵ�λΪ�� - ͳ�ƽ����
    {{0x21030300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //������ͳ�� - ͳ�����ڵ�λΪ�� - �����������Ա�
    {{0x21040200}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGION_T),    E_OOP_REGION_T},                  //������ͳ�� - ͳ�����ڵ�λΪ�� - ͳ�ƽ����
    {{0x21040300}, 14,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_REGIONS_T),   E_OOP_REGIONS_T},                 //������ͳ�� - ͳ�����ڵ�λΪ�� - �����������Ա�
    {{0x21100200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //����ƽ��  - ͳ�ƽ����
    {{0x21100300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //����ƽ��  - �����������Ա�
    {{0x21110200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //Сʱƽ��  - ͳ�ƽ����
    {{0x21110300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //Сʱƽ��  - �����������Ա�
    {{0x21120200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //��ƽ��  - ͳ�ƽ����
    {{0x21120300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //��ƽ��  - �����������Ա�
    {{0x21130200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //��ƽ��  - ͳ�ƽ����
    {{0x21130300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //��ƽ��  - �����������Ա�
    {{0x21140200}, 15,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_TOTALAVG_T),  E_OOP_TOTALAVG_T},                //��ƽ��  - ͳ�ƽ����
    {{0x21140300}, 15,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //��ƽ��  - �����������Ա�
    {{0x21200200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //���Ӽ�ֵ  - ͳ�ƽ����
    {{0x21200300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //���Ӽ�ֵ  - �����������Ա�
    {{0x21210200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //Сʱ��ֵ  - ͳ�ƽ����
    {{0x21210300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //Сʱ��ֵ  - �����������Ա�
    {{0x21220200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //�ռ�ֵ  - ͳ�ƽ����
    {{0x21220300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //�ռ�ֵ  - �����������Ա�
    {{0x21230200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //�¼�ֵ  - ͳ�ƽ����
    {{0x21230300}, 16,      0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //�¼�ֵ  - �����������Ա�
    {{0x21240200}, 16,      0x02,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EXTREMERES_T),E_OOP_EXTREMERES_T},              //�꼫ֵ  - ͳ�ƽ����
    {{0x21240300}, 16,       0x02,   0x01, 0,      DT_ARRAY,               sizeof(OOP_COGNATES_T),  E_OOP_COGNATES_T},                //�꼫ֵ  - �����������Ա�
    {{0x21310200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //����A���ѹ�ϸ���
    {{0x21320200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //����B���ѹ�ϸ���
    {{0x21330200}, 6,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_VOLTSTAT_T),  E_OOP_VOLTSTAT_T},                //����B���ѹ�ϸ���
    {{0x21400200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T),    E_OOP_DEMAND_T},                  //������й����ʼ�����ʱ��
    {{0x21410200}, 2,       0x02,   0x01, 0,      DT_ARRAY,                sizeof(OOP_DEMAND_T),    E_OOP_DEMAND_T},                  //������й����ʼ�����ʱ��
    {{0x22000200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},                  //ͨ������
    {{0x22030200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_DWORD2_T),    E_OOP_DWORD2_T},                   //����ʱ��
    {{0x22040200}, 6,       0x02,   0x02, 0,      DT_STRUCTURE,            sizeof(OOP_WORD2_T),     E_OOP_WORD2_T},                   //��λ����
    {{0x23010200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���1 - �ܼ������ñ�
    {{0x23010300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - �ܼ��й�����
    {{0x23010400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - �ܼ��޹�����
    {{0x23010500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23010600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23010700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���1 - �ܼ����й�����
    {{0x23010800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���1 - �ܼ����޹�����
    {{0x23010900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���1 - �ܼ����й�����
    {{0x23010A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���1 - �ܼ����޹�����
    {{0x23010B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - �ܼ�ʣ�����(��)
    {{0x23010C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���1 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23010D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���1 - ���ܼ��黬��ʱ������
    {{0x23010E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���1 - �ܼ��鹦���ִ�����
    {{0x23010F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���1 - �ܼ������ִ�����
    {{0x23011000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���1 - �ܼ����������״̬
    {{0x23011100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���1 - �ܼ��鵱ǰ����״̬
    {{0x23011200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���1 - ���㼰��λ
    {{0x23020200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���2 - �ܼ������ñ�
    {{0x23020300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - �ܼ��й�����
    {{0x23020400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - �ܼ��޹�����
    {{0x23020500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23020600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23020700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���2 - �ܼ����й�����
    {{0x23020800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���2 - �ܼ����޹�����
    {{0x23020900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���2 - �ܼ����й�����
    {{0x23020A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���2 - �ܼ����޹�����
    {{0x23020B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - �ܼ�ʣ�����(��)
    {{0x23020C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���2 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23020D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���2 - ���ܼ��黬��ʱ������
    {{0x23020E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���2 - �ܼ��鹦���ִ�����
    {{0x23020F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���2 - �ܼ������ִ�����
    {{0x23021000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���2 - �ܼ����������״̬
    {{0x23021100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���2 - �ܼ��鵱ǰ����״̬
    {{0x23021200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���2 - ���㼰��λ
    {{0x23030200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���3 - �ܼ������ñ�
    {{0x23030300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - �ܼ��й�����
    {{0x23030400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - �ܼ��޹�����
    {{0x23030500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23030600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23030700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���3 - �ܼ����й�����
    {{0x23030800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���3 - �ܼ����޹�����
    {{0x23030900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���3 - �ܼ����й�����
    {{0x23030A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���3 - �ܼ����޹�����
    {{0x23030B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - �ܼ�ʣ�����(��)
    {{0x23030C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���3 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23030D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���3 - ���ܼ��黬��ʱ������
    {{0x23030E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���3 - �ܼ��鹦���ִ�����
    {{0x23030F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���3 - �ܼ������ִ�����
    {{0x23031000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���3 - �ܼ����������״̬
    {{0x23031100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���3 - �ܼ��鵱ǰ����״̬
    {{0x23031200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���3 - ���㼰��λ
    {{0x23040200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���4 - �ܼ������ñ�
    {{0x23040300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - �ܼ��й�����
    {{0x23040400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - �ܼ��޹�����
    {{0x23040500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23040600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23040700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���4 - �ܼ����й�����
    {{0x23040800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���4 - �ܼ����޹�����
    {{0x23040900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���4 - �ܼ����й�����
    {{0x23040A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���4 - �ܼ����޹�����
    {{0x23040B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - �ܼ�ʣ�����(��)
    {{0x23040C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���4 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23040D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���4 - ���ܼ��黬��ʱ������
    {{0x23040E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���4 - �ܼ��鹦���ִ�����
    {{0x23040F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���4 - �ܼ������ִ�����
    {{0x23041000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���4 - �ܼ����������״̬
    {{0x23041100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���4 - �ܼ��鵱ǰ����״̬
    {{0x23041200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���4 - ���㼰��λ
    {{0x23050200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���5 - �ܼ������ñ�
    {{0x23050300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - �ܼ��й�����
    {{0x23050400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - �ܼ��޹�����
    {{0x23050500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23050600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23050700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���5 - �ܼ����й�����
    {{0x23050800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���5 - �ܼ����޹�����
    {{0x23050900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���5 - �ܼ����й�����
    {{0x23050A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���5 - �ܼ����޹�����
    {{0x23050B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - �ܼ�ʣ�����(��)
    {{0x23050C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���5 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23050D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���5 - ���ܼ��黬��ʱ������
    {{0x23050E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���5 - �ܼ��鹦���ִ�����
    {{0x23050F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���5 - �ܼ������ִ�����
    {{0x23051000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���5 - �ܼ����������״̬
    {{0x23051100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���5 - �ܼ��鵱ǰ����״̬
    {{0x23051200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���5 - ���㼰��λ
    {{0x23060200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���6 - �ܼ������ñ�
    {{0x23060300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - �ܼ��й�����
    {{0x23060400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - �ܼ��޹�����
    {{0x23060500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23060600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23060700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���6 - �ܼ����й�����
    {{0x23060800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���6 - �ܼ����޹�����
    {{0x23060900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���6 - �ܼ����й�����
    {{0x23060A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���6 - �ܼ����޹�����
    {{0x23060B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - �ܼ�ʣ�����(��)
    {{0x23060C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���6 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23060D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���6 - ���ܼ��黬��ʱ������
    {{0x23060E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���6 - �ܼ��鹦���ִ�����
    {{0x23060F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���6 - �ܼ������ִ�����
    {{0x23061000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���6 - �ܼ����������״̬
    {{0x23061100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���6 - �ܼ��鵱ǰ����״̬
    {{0x23061200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���6 - ���㼰��λ
    {{0x23070200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���7 - �ܼ������ñ�
    {{0x23070300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - �ܼ��й�����
    {{0x23070400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - �ܼ��޹�����
    {{0x23070500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23070600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23070700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���7 - �ܼ����й�����
    {{0x23070800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���7 - �ܼ����޹�����
    {{0x23070900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���7 - �ܼ����й�����
    {{0x23070A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���7 - �ܼ����޹�����
    {{0x23070B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - �ܼ�ʣ�����(��)
    {{0x23070C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���7 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23070D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���7 - ���ܼ��黬��ʱ������
    {{0x23070E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���7 - �ܼ��鹦���ִ�����
    {{0x23070F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���7 - �ܼ������ִ�����
    {{0x23071000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���7 - �ܼ����������״̬
    {{0x23071100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���7 - �ܼ��鵱ǰ����״̬
    {{0x23071200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���7 - ���㼰��λ
    {{0x23080200}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_TGSETS_T),    E_OOP_TGSETS_T},                  //�ܼ���8 - �ܼ������ñ�
    {{0x23080300}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - �ܼ��й�����
    {{0x23080400}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - �ܼ��޹�����
    {{0x23080500}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - �ܼӻ���ʱ����ƽ���й�����
    {{0x23080600}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - �ܼӻ���ʱ����ƽ���޹�����
    {{0x23080700}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���8 - �ܼ����й�����
    {{0x23080800}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���8 - �ܼ����޹�����
    {{0x23080900}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���8 - �ܼ����й�����
    {{0x23080A00}, 23,      0x02,   0x03, 0,      DT_ARRAY,                sizeof(OOP_HENERGYS_T),  E_OOP_HENERGYS_T},                //�ܼ���8 - �ܼ����޹�����
    {{0x23080B00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - �ܼ�ʣ�����(��)
    {{0x23080C00}, 23,      0x02,   0x03, 0,      DT_LONG64,               sizeof(int64),           E_LONG64},                        //�ܼ���8 - ��ǰ�����¸��ؿغ��ܼ��й����ʶ���ֵ
    {{0x23080D00}, 23,      0x02,   0x03, 0,      DT_UNSIGNED,             sizeof(uint8),           E_UNSIGNED},                      //�ܼ���8 - ���ܼ��黬��ʱ������
    {{0x23080E00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���8 - �ܼ��鹦���ִ�����
    {{0x23080F00}, 23,      0x02,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),   E_OOP_BITSTR8_T},                 //�ܼ���8 - �ܼ������ִ�����
    {{0x23081000}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGSETTING_T), E_OOP_TGSETTING_T},               //�ܼ���8 - �ܼ����������״̬
    {{0x23081100}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGCURSTATE_T),E_OOP_TGCURSTATE_T},              //�ܼ���8 - �ܼ��鵱ǰ����״̬
    {{0x23081200}, 23,      0x02,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_TGUNIT_T),    E_OOP_TGUNIT_T},                  //�ܼ���8 - ���㼰��λ
    {{0x24010200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������1 - ͨ�ŵ�ַ
    {{0x24010300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������1 - ����������
    {{0x24010400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������1 - ��������
    {{0x24010500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������1 - �й�����
    {{0x24010600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������1 - �޹�����
    {{0x24010700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���������й�����
    {{0x24010800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���������й�����
    {{0x24010900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���շ����й�����
    {{0x24010A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���·����й�����
    {{0x24010B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���������޹�����
    {{0x24010C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���������޹�����
    {{0x24010D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���շ����޹�����
    {{0x24010E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - ���·����޹�����
    {{0x24010F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - �����й�����ʾֵ
    {{0x24011000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - �����й�����ʾֵ
    {{0x24011100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - �����޹�����ʾֵ
    {{0x24011200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������1 - �����޹�����ʾֵ
    {{0x24011300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������1 - ��λ������
    {{0x24011C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������1 - ��������
    {{0x24020200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������2 - ͨ�ŵ�ַ
    {{0x24020300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������2 - ����������
    {{0x24020400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������2 - ��������
    {{0x24020500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������2 - �й�����
    {{0x24020600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������2 - �޹�����
    {{0x24020700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���������й�����
    {{0x24020800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���������й�����
    {{0x24020900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���շ����й�����
    {{0x24020A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���·����й�����
    {{0x24020B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���������޹�����
    {{0x24020C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���������޹�����
    {{0x24020D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���շ����޹�����
    {{0x24020E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - ���·����޹�����
    {{0x24020F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - �����й�����ʾֵ
    {{0x24021000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - �����й�����ʾֵ
    {{0x24021100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - �����޹�����ʾֵ
    {{0x24021200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������2 - �����޹�����ʾֵ
    {{0x24021300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������2 - ��λ������
    {{0x24021C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������2 - ��������
    {{0x24030200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������3 - ͨ�ŵ�ַ
    {{0x24030300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������3 - ����������
    {{0x24030400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������3 - ��������
    {{0x24030500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������3 - �й�����
    {{0x24030600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������3 - �޹�����
    {{0x24030700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���������й�����
    {{0x24030800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���������й�����
    {{0x24030900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���շ����й�����
    {{0x24030A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���·����й�����
    {{0x24030B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���������޹�����
    {{0x24030C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���������޹�����
    {{0x24030D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���շ����޹�����
    {{0x24030E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - ���·����޹�����
    {{0x24030F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - �����й�����ʾֵ
    {{0x24031000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - �����й�����ʾֵ
    {{0x24031100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - �����޹�����ʾֵ
    {{0x24031200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������3 - �����޹�����ʾֵ
    {{0x24031300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������3 - ��λ������
    {{0x24031C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������3 - ��������
    {{0x24040200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������4 - ͨ�ŵ�ַ
    {{0x24040300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������4 - ����������
    {{0x24040400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������4 - ��������
    {{0x24040500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������4 - �й�����
    {{0x24040600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������4 - �޹�����
    {{0x24040700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���������й�����
    {{0x24040800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���������й�����
    {{0x24040900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���շ����й�����
    {{0x24040A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���·����й�����
    {{0x24040B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���������޹�����
    {{0x24040C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���������޹�����
    {{0x24040D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���շ����޹�����
    {{0x24040E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - ���·����޹�����
    {{0x24040F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - �����й�����ʾֵ
    {{0x24041000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - �����й�����ʾֵ
    {{0x24041100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - �����޹�����ʾֵ
    {{0x24041200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������4 - �����޹�����ʾֵ
    {{0x24041300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������4 - ��λ������
    {{0x24041C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������4 - ��������
    {{0x24050200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������5 - ͨ�ŵ�ַ
    {{0x24050300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������5 - ����������
    {{0x24050400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������5 - ��������
    {{0x24050500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������5 - �й�����
    {{0x24050600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������5 - �޹�����
    {{0x24050700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���������й�����
    {{0x24050800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���������й�����
    {{0x24050900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���շ����й�����
    {{0x24050A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���·����й�����
    {{0x24050B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���������޹�����
    {{0x24050C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���������޹�����
    {{0x24050D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���շ����޹�����
    {{0x24050E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - ���·����޹�����
    {{0x24050F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - �����й�����ʾֵ
    {{0x24051000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - �����й�����ʾֵ
    {{0x24051100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - �����޹�����ʾֵ
    {{0x24051200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������5 - �����޹�����ʾֵ
    {{0x24051300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������5 - ��λ������
    {{0x24051C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������5 - ��������
    {{0x24060200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������6 - ͨ�ŵ�ַ
    {{0x24060300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������6 - ����������
    {{0x24060400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������6 - ��������
    {{0x24060500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������6 - �й�����
    {{0x24060600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������6 - �޹�����
    {{0x24060700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���������й�����
    {{0x24060800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���������й�����
    {{0x24060900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���շ����й�����
    {{0x24060A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���·����й�����
    {{0x24060B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���������޹�����
    {{0x24060C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���������޹�����
    {{0x24060D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���շ����޹�����
    {{0x24060E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - ���·����޹�����
    {{0x24060F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - �����й�����ʾֵ
    {{0x24061000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - �����й�����ʾֵ
    {{0x24061100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - �����޹�����ʾֵ
    {{0x24061200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������6 - �����޹�����ʾֵ
    {{0x24061300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������6 - ��λ������
    {{0x24061C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������6 - ��������
    {{0x24070200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������7 - ͨ�ŵ�ַ
    {{0x24070300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������7 - ����������
    {{0x24070400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������7 - ��������
    {{0x24070500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������7 - �й�����
    {{0x24070600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������7 - �޹�����
    {{0x24070700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���������й�����
    {{0x24070800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���������й�����
    {{0x24070900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���շ����й�����
    {{0x24070A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���·����й�����
    {{0x24070B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���������޹�����
    {{0x24070C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���������޹�����
    {{0x24070D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���շ����޹�����
    {{0x24070E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - ���·����޹�����
    {{0x24070F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - �����й�����ʾֵ
    {{0x24071000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - �����й�����ʾֵ
    {{0x24071100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - �����޹�����ʾֵ
    {{0x24071200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������7 - �����޹�����ʾֵ
    {{0x24071300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������7 - ��λ������
    {{0x24071C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������7 - ��������
    {{0x24080200}, 12,      0x02,   0x04, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),E_OOP_OCTETVAR16_T},              //�������8 - ͨ�ŵ�ַ
    {{0x24080300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PTCT_T),      E_OOP_PTCT_T},                    //�������8 - ����������
    {{0x24080400}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_T), E_OOP_PULSESETS_T},               //�������8 - ��������
    {{0x24080500}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������8 - �й�����
    {{0x24080600}, 12,      0x02,   0x04, 0,      DT_DOUBLE_LONG,          sizeof(int32),           E_DOUBLE_LONG},                   //�������8 - �޹�����
    {{0x24080700}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���������й�����
    {{0x24080800}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���������й�����
    {{0x24080900}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���շ����й�����
    {{0x24080A00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���·����й�����
    {{0x24080B00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���������޹�����
    {{0x24080C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���������޹�����
    {{0x24080D00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���շ����޹�����
    {{0x24080E00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - ���·����޹�����
    {{0x24080F00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - �����й�����ʾֵ
    {{0x24081000}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - �����й�����ʾֵ
    {{0x24081100}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - �����޹�����ʾֵ
    {{0x24081200}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_ENERGY_T),    E_OOP_ENERGY_T},                  //�������8 - �����޹�����ʾֵ
    {{0x24081300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSEUNIT_T), E_OOP_PULSEUNIT_T},               //�������8 - ��λ������
    {{0x24081C00}, 12,      0x02,   0x04, 0,      DT_ARRAY,                sizeof(OOP_PULSESETS_B_T), E_OOP_PULSEUNIT_T},             //�������1 - ��������
    {{0x25000200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ۼ�ˮ���ȣ�����
    {{0x25010200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ۼ�������
    {{0x25020200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ۼ�����
    {{0x25030200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ȹ���
    {{0x25040200}, 6,      0x02,   0x05, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),          E_DOUBLE_LONG_UNSIGNED},          //�ۼƹ���ʱ��
    {{0x25050200}, 6,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(Water_Temperature_T),     E_Water_Temperature_T},                   //ˮ��
    {{0x25060200}, 6,      0x02,   0x05, 0,      DT_STRUCTURE,            sizeof(OOP_ST_T),        E_OOP_ST_T},                      //���Ǳ�״̬ST
    {{0x26000200}, 6,       0x02,   0x06, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //�¶�
    {{0x26010200}, 6,       0x02,   0x06, 0,      DT_ARRAY,                sizeof(OOP_WORD3V_T),    E_OOP_WORD3V_T},                  //ʪ��
    {{0x20100400}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //�����¶�
    {{0x20430200}, 6,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //�����¶ȷ��ӱ仯��
    {{0x20440200}, 3,       0x02,   0x00, 0,      DT_ARRAY,                sizeof(OOP_LONG3V_T),    E_OOP_LONG3V_T},                  //����Լ��
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)
    {{0x28110200}, 6,       0x02,   0x08, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //�̸�״̬
    {{0x29000200}, 6,       0x02,   0x09, 0,      DT_LONG,                 sizeof(int16),           E_LONG},                          //�������¶�
    {{0x29040200}, 6,       0x02,   0x09, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //ˮ��״̬
    {{0x29050200}, 6,       0x02,   0x09, 0,      DT_ENUM,                 sizeof(uint8),           E_ENUM},                          //ˮ��״̬
#endif
#ifdef AREA_SHANDONG
    {{0x2E200200}, 6,       0x02,   0x0E, 0,      DT_STRUCTURE,            sizeof(OOP_HUMITURE_2E20_T), E_OOP_HUMITURE_2E20_T},       //��ʪ��
    {{0x2E210200}, 6,       0x02,   0x0E, 0,      DT_STRUCTURE,            sizeof(OOP_LOCK_STATE_2E21_T), E_OOP_LOCK_STATE_2E21_T},   //����״̬
#endif
};          
const uint32 oadVariateInfoListLen = sizeof(oadVariateInfoList)/sizeof(oadVariateInfoList[0]);

/* �¼���OAD�б� OIA1 = 0x03 */
const OAD_TAB_T oadEvnetInfoList[] =
{
    /*OAD        class     A1      A2   rsv       dataType                   len*/
    {{0x30000200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʧѹ�¼� - �����������Ա�
    {{0x30000300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�ʧѹ�¼� - ��ǰ��¼��
    {{0x30000400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʧѹ�¼� - ����¼��
    {{0x30000500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3000_T),    E_OOP_EVTCFG_3000_T},            //���ܱ�ʧѹ�¼� - ���ò���
    {{0x30000600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧѹ�¼� - �¼���¼��Ԫ1
    {{0x30000700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧѹ�¼� - �¼���¼��Ԫ2
    {{0x30000800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧѹ�¼� - �¼���¼��Ԫ3
    {{0x30000900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧѹ�¼� - �¼���¼��Ԫ4
    {{0x30000A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�ʧѹ�¼� - ��ǰֵ��¼��
    {{0x30000B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʧѹ�¼� - �ϱ���ʶ
    {{0x30000C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʧѹ�¼� - ��Ч��ʶ
    {{0x30000D00}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOSTVSTAT_T),      E_OOP_LOSTVSTAT_T},              //���ܱ�ʧѹ�¼� - ʧѹͳ��
    {{0x30000E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�ʧѹ�¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30010200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�Ƿѹ�¼� - �����������Ա�
    {{0x30010300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�Ƿѹ�¼� - ��ǰ��¼��
    {{0x30010400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�Ƿѹ�¼� - ����¼��
    {{0x30010500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T),      E_OOP_EVTCFG_WD_T},              //���ܱ�Ƿѹ�¼� - ���ò���
    {{0x30010600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�Ƿѹ�¼� - �¼���¼��Ԫ1
    {{0x30010700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�Ƿѹ�¼� - �¼���¼��Ԫ2
    {{0x30010800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�Ƿѹ�¼� - �¼���¼��Ԫ3
    {{0x30010900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�Ƿѹ�¼� - �¼���¼��Ԫ4
    {{0x30010A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�Ƿѹ�¼� - ��ǰֵ��¼��
    {{0x30010B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�Ƿѹ�¼� - �ϱ���ʶ
    {{0x30010C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�Ƿѹ�¼� - ��Ч��ʶ
    {{0x30010E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�Ƿѹ�¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30020200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��ѹ�¼� - �����������Ա�
    {{0x30020300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ��ѹ�¼� - ��ǰ��¼��
    {{0x30020400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ѹ�¼� - ����¼��
    {{0x30020500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_WD_T),      E_OOP_EVTCFG_WD_T},              //���ܱ��ѹ�¼� - ���ò���
    {{0x30020600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ѹ�¼� - �¼���¼��Ԫ1
    {{0x30020700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ѹ�¼� - �¼���¼��Ԫ2
    {{0x30020800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ѹ�¼� - �¼���¼��Ԫ3
    {{0x30020900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ѹ�¼� - �¼���¼��Ԫ4
    {{0x30020A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ��ѹ�¼� - ��ǰֵ��¼��
    {{0x30020B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ�¼� - �ϱ���ʶ
    {{0x30020C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ�¼� - ��Ч��ʶ
    {{0x30020E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ��ѹ�¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30030200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x30030300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x30030400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x30030500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3003_T),    E_OOP_EVTCFG_3003_T},            //���ܱ�����¼� - ���ò���
    {{0x30030600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ1
    {{0x30030700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ2
    {{0x30030800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ3
    {{0x30030900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ4
    {{0x30030A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�����¼� - ��ǰֵ��¼��
    {{0x30030B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x30030C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x30030E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�����¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30040200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʧ���¼� - �����������Ա�
    {{0x30040300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�ʧ���¼� - ��ǰ��¼��
    {{0x30040400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʧ���¼� - ����¼��
    {{0x30040500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3004_T),    E_OOP_EVTCFG_3004_T},            //���ܱ�ʧ���¼� - ���ò���
    {{0x30040600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧ���¼� - �¼���¼��Ԫ1
    {{0x30040700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧ���¼� - �¼���¼��Ԫ2
    {{0x30040800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧ���¼� - �¼���¼��Ԫ3
    {{0x30040900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�ʧ���¼� - �¼���¼��Ԫ4
    {{0x30040A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�ʧ���¼� - ��ǰֵ��¼��
    {{0x30040B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʧ���¼� - �ϱ���ʶ
    {{0x30040C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʧ���¼� - ��Ч��ʶ
    {{0x30040E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�ʧ���¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30050200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x30050300}, 24,      0x03,   0x00, 1,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x30050400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x30050500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //���ܱ�����¼� - ���ò���
    {{0x30050600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ1
    {{0x30050700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ2
    {{0x30050800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ3
    {{0x30050900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ4
    {{0x30050A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�����¼� - ��ǰֵ��¼��
    {{0x30050B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x30050C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x30050E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�����¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30060200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x30060300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x30060400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x30060500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3006_T),    E_OOP_EVTCFG_3006_T},            //���ܱ�����¼� - ���ò���
    {{0x30060600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ1
    {{0x30060700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ2
    {{0x30060800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ3
    {{0x30060900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ4
    {{0x30060A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�����¼� - ��ǰֵ��¼��
    {{0x30060B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x30060C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x30060E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�����¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30070200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��ʷ����¼� - �����������Ա�
    {{0x30070300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ��ʷ����¼� - ��ǰ��¼��
    {{0x30070400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ʷ����¼� - ����¼��
    {{0x30070500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //���ܱ��ʷ����¼� - ���ò���
    {{0x30070600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ʷ����¼� - �¼���¼��Ԫ1
    {{0x30070700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ʷ����¼� - �¼���¼��Ԫ2
    {{0x30070800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ʷ����¼� - �¼���¼��Ԫ3
    {{0x30070900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ��ʷ����¼� - �¼���¼��Ԫ4
    {{0x30070A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ��ʷ����¼� - ��ǰֵ��¼��
    {{0x30070B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ʷ����¼� - �ϱ���ʶ
    {{0x30070C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ʷ����¼� - ��Ч��ʶ
    {{0x30070E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ��ʷ����¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30080200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x30080300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x30080400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x30080500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_ID_T),      E_OOP_EVTCFG_ID_T},              //���ܱ�����¼� - ���ò���
    {{0x30080600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ1
    {{0x30080700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ2
    {{0x30080800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ3
    {{0x30080900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVENT_T),         E_OOP_MEVENT_T},                 //���ܱ�����¼� - �¼���¼��Ԫ4
    {{0x30080A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MEVTSTAT_T),       E_OOP_MEVTSTAT_T},         //���ܱ�����¼� - ��ǰֵ��¼��
    {{0x30080B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x30080C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x30080E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ�����¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x30090200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ������й����������¼� - �¼���¼��
    {{0x30090300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ������й����������¼� - �����������Ա�
    {{0x30090400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������й����������¼� - ��ǰ��¼��
    {{0x30090500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������й����������¼� - ����¼��
    {{0x30090600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //���ܱ������й����������¼� - ���ò���
    {{0x30090700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ������й����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30090800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������й����������¼� - �ϱ���ʶ
    {{0x30090900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������й����������¼� - ��Ч��ʶ
    {{0x30090A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ������й����������¼� - ʱ��״̬��¼��
       
    {{0x300A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ����й����������¼� - �¼���¼��
    {{0x300A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����й����������¼� - �����������Ա�
    {{0x300A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����й����������¼� - ��ǰ��¼��
    {{0x300A0500}, 7,       0x03,   0x00, 1,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����й����������¼� - ����¼��
    {{0x300A0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //���ܱ����й����������¼� - ���ò���
    {{0x300A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����й����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x300A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����й����������¼� - �ϱ���ʶ
    {{0x300A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����й����������¼� - ��Ч��ʶ
    {{0x300A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����й����������¼� - ʱ��״̬��¼��
       
    {{0x300B0200}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��޹����������¼� - �����������Ա�
    {{0x300B0300}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            4*sizeof(uint16),             E_LONG_UNSIGNED},                //���ܱ��޹����������¼� - ��ǰ��¼��
    {{0x300B0400}, 24,      0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��޹����������¼� - ����¼��
    {{0x300B0500}, 24,      0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DD_T),      E_OOP_EVTCFG_DD_T},              //���ܱ��޹����������¼� - ���ò���
    {{0x300B0600}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ��޹����������¼� - �¼���¼��Ԫ1
    {{0x300B0700}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ��޹����������¼� - �¼���¼��Ԫ2
    {{0x300B0800}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ��޹����������¼� - �¼���¼��Ԫ3
    {{0x300B0900}, 24,      0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_DEMANDLOG_T),      E_OOP_DEMANDLOG_T},              //���ܱ��޹����������¼� - �¼���¼��Ԫ4
    {{0x300B0A00}, 24,      0x03,   0x00, 1,      DT_ARRAY,                2*sizeof(uint32),             E_DOUBLE_LONG_UNSIGNED},         //���ܱ��޹����������¼� - ��ǰֵ��¼��
    {{0x300B0B00}, 24,      0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��޹����������¼� - �ϱ���ʶ
    {{0x300B0C00}, 24,      0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��޹����������¼� - ��Ч��ʶ
    {{0x300B0E00}, 24,      0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_MEVTTIME_T),       E_OOP_MEVTTIME_T},               //���ܱ��޹����������¼� - ʱ��״̬��¼��,????�ǲ���������ṹ�壬��ȷ��
       
    {{0x300C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��������������¼� - �¼���¼��
    {{0x300C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��������������¼� - �����������Ա�
    {{0x300C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��������������¼� - ��ǰ��¼��
    {{0x300C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��������������¼� - ����¼��
    {{0x300C0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //���ܱ��������������¼� - ���ò���
    {{0x300C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��������������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x300C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��������������¼� - �ϱ���ʶ
    {{0x300C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��������������¼� - ��Ч��ʶ
    {{0x300C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��������������¼� - ʱ��״̬��¼��
       
    {{0x300D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�ȫʧѹ�¼� - �¼���¼��????
    {{0x300D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ȫʧѹ�¼� - �����������Ա�
    {{0x300D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ȫʧѹ�¼� - ��ǰ��¼��
    {{0x300D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ȫʧѹ�¼� - ����¼��
    {{0x300D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ȫʧѹ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x300D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ȫʧѹ�¼� - �ϱ���ʶ
    {{0x300D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ȫʧѹ�¼� - ��Ч��ʶ
    {{0x300D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ȫʧѹ�¼� - ʱ��״̬��¼��
       
    {{0x300E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�����Դ�����¼� - �¼���¼��
    {{0x300E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����Դ�����¼� - �����������Ա�
    {{0x300E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����Դ�����¼� - ��ǰ��¼��
    {{0x300E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����Դ�����¼� - ����¼��
    {{0x300E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ�����Դ�����¼� - ���ò���
    {{0x300E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����Դ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x300E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����Դ�����¼� - �ϱ���ʶ
    {{0x300E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����Դ�����¼� - ��Ч��ʶ
    {{0x300E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����Դ�����¼� - ʱ��״̬��¼��
       
    {{0x300F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��ѹ�������¼� - �¼���¼��
    {{0x300F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��ѹ�������¼� - �����������Ա�
    {{0x300F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ѹ�������¼� - ��ǰ��¼��
    {{0x300F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ѹ�������¼� - ����¼��
    {{0x300F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ��ѹ�������¼� - ���ò���
    {{0x300F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��ѹ�������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x300F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ�������¼� - �ϱ���ʶ
    {{0x300F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ�������¼� - ��Ч��ʶ
    {{0x300F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��ѹ�������¼� - ʱ��״̬��¼��
       
    {{0x30100200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�����������¼� - �¼���¼��
    {{0x30100300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����������¼� - �����������Ա�
    {{0x30100400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����������¼� - ��ǰ��¼��
    {{0x30100500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����������¼� - ����¼��
    {{0x30100600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ�����������¼� - ���ò���
    {{0x30100700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30100800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����������¼� - �ϱ���ʶ
    {{0x30100900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����������¼� - ��Ч��ʶ
    {{0x30100A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����������¼� - ʱ��״̬��¼��
    {{0x30110B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //�����¼��ϱ���ʶ   
    {{0x30110200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�����¼� - �¼���¼��
    {{0x30110300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x30110400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x30110500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x30110600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ�����¼� - ���ò���
    {{0x30110700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30110800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x30110900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x30110A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����¼� - ʱ��״̬��¼��
       
    {{0x30120200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T),     E_OOP_PROGRAMLOG_T},             //���ܱ����¼� - �¼���¼��
    {{0x30120300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����¼� - �����������Ա�
    {{0x30120400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼� - ��ǰ��¼��
    {{0x30120500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼� - ����¼��
    {{0x30120700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30120800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼� - �ϱ���ʶ
    {{0x30120900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼� - ��Ч��ʶ
    {{0x30120A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����¼� - ʱ��״̬��¼��
       
    {{0x30130200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ������¼� - �¼���¼��
    {{0x30130300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ������¼� - �����������Ա�
    {{0x30130400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������¼� - ��ǰ��¼��
    {{0x30130500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������¼� - ����¼��
    {{0x30130700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30130800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������¼� - �ϱ���ʶ
    {{0x30130900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������¼� - ��Ч��ʶ
    {{0x30130A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ������¼� - ʱ��״̬��¼��
       
    {{0x30140200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ����������¼� - �¼���¼��
    {{0x30140300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����������¼� - �����������Ա�
    {{0x30140400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����������¼� - ��ǰ��¼��
    {{0x30140500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����������¼� - ����¼��
    {{0x30140700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30140800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����������¼� - �ϱ���ʶ
    {{0x30140900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����������¼� - ��Ч��ʶ
    {{0x30140A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����������¼� - ʱ��״̬��¼��
       
    {{0x30150200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENTRESETLOG_T),  E_OOP_EVENTRESETLOG_T},          //���ܱ��¼������¼� - �¼���¼��
    {{0x30150300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��¼������¼� - �����������Ա�
    {{0x30150400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��¼������¼� - ��ǰ��¼��
    {{0x30150500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��¼������¼� - ����¼��
    {{0x30150700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��¼������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30150800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��¼������¼� - �ϱ���ʶ
    {{0x30150900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��¼������¼� - ��Ч��ʶ
    {{0x30150A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��¼������¼� - ʱ��״̬��¼��
       
    {{0x30160200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�Уʱ�¼� - �¼���¼��
    {{0x30160300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�Уʱ�¼� - �����������Ա�
    {{0x30160400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�Уʱ�¼� - ��ǰ��¼��
    {{0x30160500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�Уʱ�¼� - ����¼��
    {{0x30160700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�Уʱ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30160800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�Уʱ�¼� - �ϱ���ʶ
    {{0x30160900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�Уʱ�¼� - ��Ч��ʶ
    {{0x30160A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�Уʱ�¼� - ʱ��״̬��¼��
       
    {{0x30170200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGPERIODLOG_T),  E_OOP_PROGPERIODLOG_T},          //���ܱ�ʱ�α����¼� - �¼���¼��
    {{0x30170300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʱ�α����¼� - �����������Ա�
    {{0x30170400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�α����¼� - ��ǰ��¼��
    {{0x30170500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�α����¼� - ����¼��
    {{0x30170700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ʱ�α����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30170800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�α����¼� - �ϱ���ʶ
    {{0x30170900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�α����¼� - ��Ч��ʶ
    {{0x30170A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ʱ�α����¼� - ʱ��״̬��¼��
       
    {{0x30180200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�ʱ�������¼� - �¼���¼��
    {{0x30180300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʱ�������¼� - �����������Ա�
    {{0x30180400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�������¼� - ��ǰ��¼��
    {{0x30180500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�������¼� - ����¼��
    {{0x30180700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ʱ�������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30180800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�������¼� - �ϱ���ʶ
    {{0x30180900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�������¼� - ��Ч��ʶ
    {{0x30180A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ʱ�������¼� - ʱ��״̬��¼��
       
    {{0x30190200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ������ձ���¼� - �¼���¼��
    {{0x30190300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ������ձ���¼� - �����������Ա�
    {{0x30190400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������ձ���¼� - ��ǰ��¼��
    {{0x30190500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������ձ���¼� - ����¼��
    {{0x30190700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ������ձ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30190800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������ձ���¼� - �ϱ���ʶ
    {{0x30190900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������ձ���¼� - ��Ч��ʶ
    {{0x30190A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ������ձ���¼� - ʱ��״̬��¼��
       
    {{0x301A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�����ձ���¼� - �¼���¼��
    {{0x301A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����ձ���¼� - �����������Ա�
    {{0x301A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����ձ���¼� - ��ǰ��¼��
    {{0x301A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����ձ���¼� - ����¼��
    {{0x301A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����ձ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����ձ���¼� - �ϱ���ʶ
    {{0x301A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����ձ���¼� - ��Ч��ʶ
    {{0x301A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����ձ���¼� - ʱ��״̬��¼��
    {{0x301B0B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //�����¼��ϱ���ʶ   
    {{0x301B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ����¼� - �¼���¼��
    {{0x301B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����¼� - �����������Ա�
    {{0x301B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼� - ��ǰ��¼��
    {{0x301B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼� - ����¼��
    {{0x301B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼� - �ϱ���ʶ
    {{0x301B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼� - ��Ч��ʶ
    {{0x301B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����¼� - ʱ��״̬��¼��
       
    {{0x301C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ���ť���¼� - �¼���¼��
    {{0x301C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ���ť���¼� - �����������Ա�
    {{0x301C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ť���¼� - ��ǰ��¼��
    {{0x301C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ť���¼� - ����¼��
    {{0x301C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ���ť���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ť���¼� - �ϱ���ʶ
    {{0x301C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ť���¼� - ��Ч��ʶ
    {{0x301C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ���ť���¼� - ʱ��״̬��¼��
       
    {{0x301D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��ѹ��ƽ���¼� - �¼���¼��
    {{0x301D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��ѹ��ƽ���¼� - �����������Ա�
    {{0x301D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ѹ��ƽ���¼� - ��ǰ��¼��
    {{0x301D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ѹ��ƽ���¼� - ����¼��
    {{0x301D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //���ܱ��ѹ��ƽ���¼� - ���ò���
    {{0x301D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��ѹ��ƽ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ��ƽ���¼� - �ϱ���ʶ
    {{0x301D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ѹ��ƽ���¼� - ��Ч��ʶ
    {{0x301D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��ѹ��ƽ���¼� - ʱ��״̬��¼��
       
    {{0x301E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ������ƽ���¼� - �¼���¼��
    {{0x301E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ������ƽ���¼� - �����������Ա�
    {{0x301E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������ƽ���¼� - ��ǰ��¼��
    {{0x301E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ������ƽ���¼� - ����¼��
    {{0x301E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //���ܱ������ƽ���¼� - ���ò���
    {{0x301E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ������ƽ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������ƽ���¼� - �ϱ���ʶ
    {{0x301E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ������ƽ���¼� - ��Ч��ʶ
    {{0x301E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ������ƽ���¼� - ʱ��״̬��¼��
       
    {{0x301F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ���բ�¼� - �¼���¼��
    {{0x301F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ���բ�¼� - �����������Ա�
    {{0x301F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���բ�¼� - ��ǰ��¼��
    {{0x301F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���բ�¼� - ����¼��
    {{0x301F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ���բ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x301F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���բ�¼� - �ϱ���ʶ
    {{0x301F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���բ�¼� - ��Ч��ʶ
    {{0x301F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ���բ�¼� - ʱ��״̬��¼��
       
    {{0x30200200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��բ�¼� - �¼���¼��
    {{0x30200300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��բ�¼� - �����������Ա�
    {{0x30200400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��բ�¼� - ��ǰ��¼��
    {{0x30200500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��բ�¼� - ����¼��
    {{0x30200700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��բ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30200800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��բ�¼� - �ϱ���ʶ
    {{0x30200900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��բ�¼� - ��Ч��ʶ
    {{0x30200A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��բ�¼� - ʱ��״̬��¼��
       
    {{0x30210200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_PROGHOLIDAYLOG_T), E_OOP_PROGHOLIDAYLOG_T},         //���ܱ�ڼ��ձ���¼� - �¼���¼��
    {{0x30210300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ڼ��ձ���¼� - �����������Ա�
    {{0x30210400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ڼ��ձ���¼� - ��ǰ��¼��
    {{0x30210500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ڼ��ձ���¼� - ����¼��
    {{0x30210700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ڼ��ձ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30210800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ڼ��ձ���¼� - �ϱ���ʶ
    {{0x30210900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ڼ��ձ���¼� - ��Ч��ʶ
    {{0x30210A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ڼ��ձ���¼� - ʱ��״̬��¼��
       
    {{0x30220200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��й���Ϸ�ʽ����¼� - �¼���¼��
    {{0x30220300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��й���Ϸ�ʽ����¼� - �����������Ա�
    {{0x30220400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��й���Ϸ�ʽ����¼� - ��ǰ��¼��
    {{0x30220500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��й���Ϸ�ʽ����¼� - ����¼��
    {{0x30220700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��й���Ϸ�ʽ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30220800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��й���Ϸ�ʽ����¼� - �ϱ���ʶ
    {{0x30220900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��й���Ϸ�ʽ����¼� - ��Ч��ʶ
    {{0x30220A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��й���Ϸ�ʽ����¼� - ʱ��״̬��¼��
       
    {{0x30230200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��޹���Ϸ�ʽ����¼� - �¼���¼��
    {{0x30230300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��޹���Ϸ�ʽ����¼� - �����������Ա�
    {{0x30230400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��޹���Ϸ�ʽ����¼� - ��ǰ��¼��
    {{0x30230500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��޹���Ϸ�ʽ����¼� - ����¼��
    {{0x30230700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��޹���Ϸ�ʽ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30230800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��޹���Ϸ�ʽ����¼� - �ϱ���ʶ
    {{0x30230900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��޹���Ϸ�ʽ����¼� - ��Ч��ʶ
    {{0x30230A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��޹���Ϸ�ʽ����¼� - ʱ��״̬��¼��
       
    {{0x30240200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ���ʲ��������¼� - �¼���¼��
    {{0x30240300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ���ʲ��������¼� - �����������Ա�
    {{0x30240400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ʲ��������¼� - ��ǰ��¼��
    {{0x30240500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ʲ��������¼� - ����¼��
    {{0x30240700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ���ʲ��������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30240800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ʲ��������¼� - �ϱ���ʶ
    {{0x30240900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ʲ��������¼� - ��Ч��ʶ
    {{0x30240A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ���ʲ��������¼� - ʱ��״̬��¼��
       
    {{0x30250200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ���ݱ����¼� - �¼���¼��
    {{0x30250300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ���ݱ����¼� - �����������Ա�
    {{0x30250400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ݱ����¼� - ��ǰ��¼��
    {{0x30250500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���ݱ����¼� - ����¼��
    {{0x30250700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ���ݱ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30250800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ݱ����¼� - �ϱ���ʶ
    {{0x30250900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���ݱ����¼� - ��Ч��ʶ
    {{0x30250A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ���ݱ����¼� - ʱ��״̬��¼��
       
    {{0x30260200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ���Կ�����¼� - �¼���¼��
    {{0x30260300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ���Կ�����¼� - �����������Ա�
    {{0x30260400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���Կ�����¼� - ��ǰ��¼��
    {{0x30260500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ���Կ�����¼� - ����¼��
    {{0x30260700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ���Կ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30260800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���Կ�����¼� - �ϱ���ʶ
    {{0x30260900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ���Կ�����¼� - ��Ч��ʶ
    {{0x30260A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ���Կ�����¼� - ʱ��״̬��¼��
       
    {{0x30270200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCARD_T),   E_OOP_ABNORMALCARD_T},           //���ܱ��쳣�忨�¼� - �¼���¼��
    {{0x30270300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��쳣�忨�¼� - �����������Ա�
    {{0x30270400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��쳣�忨�¼� - ��ǰ��¼��
    {{0x30270500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��쳣�忨�¼� - ����¼��
    {{0x30270700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��쳣�忨�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30270800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��쳣�忨�¼� - �ϱ���ʶ
    {{0x30270900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��쳣�忨�¼� - ��Ч��ʶ
    {{0x30270A00}, 7,       0x03,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED, sizeof(uint32),               E_DOUBLE_LONG_UNSIGNED},         //���ܱ��쳣�忨�¼� - �Ƿ��忨�ܴ���
       
    {{0x30280200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ����¼ - �¼���¼��
    {{0x30280300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����¼ - �����������Ա�
    {{0x30280400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼ - ��ǰ��¼��
    {{0x30280500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����¼ - ����¼��
    {{0x30280700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30280800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼ - �ϱ���ʶ
    {{0x30280900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����¼ - ��Ч��ʶ
    {{0x30280A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����¼ - ʱ��״̬��¼��
       
    {{0x30290200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_REBATELOG_T),      E_OOP_REBATELOG_T},              //���ܱ��˷Ѽ�¼ - �¼���¼��
    {{0x30290300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��˷Ѽ�¼ - �����������Ա�
    {{0x30290400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��˷Ѽ�¼ - ��ǰ��¼��
    {{0x30290500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��˷Ѽ�¼ - ����¼��
    {{0x30290700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��˷Ѽ�¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30290800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��˷Ѽ�¼ - �ϱ���ʶ
    {{0x30290900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��˷Ѽ�¼ - ��Ч��ʶ
    {{0x30290A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��˷Ѽ�¼ - ʱ��״̬��¼��
       
    {{0x302A0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�㶨�ų������¼� - �¼���¼��
    {{0x302A0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�㶨�ų������¼� - �����������Ա�
    {{0x302A0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�㶨�ų������¼� - ��ǰ��¼��
    {{0x302A0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�㶨�ų������¼� - ����¼��
    {{0x302A0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�㶨�ų������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302A0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�㶨�ų������¼� - �ϱ���ʶ
    {{0x302A0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�㶨�ų������¼� - ��Ч��ʶ
    {{0x302A0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�㶨�ų������¼� - ʱ��״̬��¼��
       
    {{0x302B0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��ɿ��������¼� - �¼���¼��
    {{0x302B0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��ɿ��������¼� - �����������Ա�
    {{0x302B0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ɿ��������¼� - ��ǰ��¼��
    {{0x302B0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��ɿ��������¼� - ����¼��
    {{0x302B0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��ɿ��������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302B0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ɿ��������¼� - �ϱ���ʶ
    {{0x302B0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��ɿ��������¼� - ��Ч��ʶ
    {{0x302B0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��ɿ��������¼� - ʱ��״̬��¼��
       
    {{0x302C0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ��Դ�쳣�¼� - �¼���¼��
    {{0x302C0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ��Դ�쳣�¼� - �����������Ա�
    {{0x302C0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��Դ�쳣�¼� - ��ǰ��¼��
    {{0x302C0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ��Դ�쳣�¼� - ����¼��
    {{0x302C0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ��Դ�쳣�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302C0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��Դ�쳣�¼� - �ϱ���ʶ
    {{0x302C0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ��Դ�쳣�¼� - ��Ч��ʶ
    {{0x302C0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ��Դ�쳣�¼� - ʱ��״̬��¼��
       
    {{0x302D0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�������ز�ƽ���¼� - �¼���¼��
    {{0x302D0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�������ز�ƽ���¼� - �����������Ա�
    {{0x302D0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�������ز�ƽ���¼� - ��ǰ��¼��
    {{0x302D0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�������ز�ƽ���¼� - ����¼��
    {{0x302D0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_CFG_LU_T),      E_OOP_CFG_LU_T},              //���ܱ�������ز�ƽ���¼� - ���ò���
    {{0x302D0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�������ز�ƽ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302D0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�������ز�ƽ���¼� - �ϱ���ʶ
    {{0x302D0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�������ز�ƽ���¼� - ��Ч��ʶ
    {{0x302D0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�������ز�ƽ���¼� - ʱ��״̬��¼��
       
    {{0x302E0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�ʱ�ӹ����¼� - �¼���¼��
    {{0x302E0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʱ�ӹ����¼� - �����������Ա�
    {{0x302E0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�ӹ����¼� - ��ǰ��¼��
    {{0x302E0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�ӹ����¼� - ����¼��
    {{0x302E0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ�ʱ�ӹ����¼� - ���ò���
    {{0x302E0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ʱ�ӹ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302E0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�ӹ����¼� - �ϱ���ʶ
    {{0x302E0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�ӹ����¼� - ��Ч��ʶ
    {{0x302E0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ʱ�ӹ����¼� - ʱ��״̬��¼��
    {{0x302E0B00}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_ENUM},                    //ʱ�ӹ����¼��ϱ���ʶ   
    {{0x302F0200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ����оƬ�����¼� - �¼���¼��
    {{0x302F0300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����оƬ�����¼� - �����������Ա�
    {{0x302F0400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����оƬ�����¼� - ��ǰ��¼��
    {{0x302F0500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����оƬ�����¼� - ����¼��
    {{0x302F0600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //���ܱ����оƬ�����¼� - ���ò���
    {{0x302F0700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����оƬ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x302F0800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����оƬ�����¼� - �ϱ���ʶ
    {{0x302F0900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����оƬ�����¼� - ��Ч��ʶ
    {{0x302F0A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����оƬ�����¼� - ʱ��״̬��¼��
       
    {{0x30300200}, 7,       0x03,   0x00, 1,      DT_ARRAY,                sizeof(OOP_MODULELOG_T),      E_OOP_MODULELOG_T},              //ͨ��ģ�����¼� - �¼���¼��
    {{0x30300300}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ͨ��ģ�����¼� - �����������Ա�
    {{0x30300400}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ͨ��ģ�����¼� - ��ǰ��¼��
    {{0x30300500}, 7,       0x03,   0x00, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ͨ��ģ�����¼� - ����¼��
    {{0x30300600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_DELAY_T),   E_OOP_EVTCFG_DELAY_T},           //ͨ��ģ�����¼� - ���ò���
    {{0x30300700}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ͨ��ģ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30300800}, 7,       0x03,   0x00, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ͨ��ģ�����¼� - �ϱ���ʶ
    {{0x30300900}, 7,       0x03,   0x00, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ͨ��ģ�����¼� - ��Ч��ʶ
    {{0x30300A00}, 7,       0x03,   0x00, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ͨ��ģ�����¼� - ʱ��״̬��¼��

    {{0x30310200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //����ģ�����¼� - �¼���¼��
    {{0x30310300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //����ģ�����¼� - �����������Ա�
    {{0x30310400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����ģ�����¼� - ��ǰ��¼��
    {{0x30310500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����ģ�����¼� - ����¼��
    {{0x30310700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //����ģ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30310800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //����ģ�����¼� - �ϱ���ʶ
    {{0x30310900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //����ģ�����¼� - ��Ч��ʶ
    {{0x30310A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //����ģ�����¼� - ʱ��״̬��¼��

    {{0x30400600}, 7,       0x03,   0x00, 0,      DT_STRUCTURE,            sizeof(OOP_LOAD_I0_EVECFG_T), E_OOP_LOAD_I0_EVECFG_T},
    
    {{0x30320200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���������ȱ����¼� - �¼���¼��
    {{0x30320300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���������ȱ����¼� - �����������Ա�
    {{0x30320400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���������ȱ����¼� - ��ǰ��¼��
    {{0x30320500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���������ȱ����¼� - ����¼��
    {{0x30320700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���������ȱ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x30320800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���������ȱ����¼� - �ϱ���ʶ
    {{0x30320900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���������ȱ����¼� - ��Ч��ʶ
    {{0x30320A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���������ȱ����¼� - ʱ��״̬��¼��
    {{0x303B0500}, 7,       0x03,   0x01, 0,   DT_STRUCTURE,               sizeof(OOP_CFG_LU_T),            E_OOP_CFG_LU_T},
    {{0x31000200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն˳�ʼ���¼� - �¼���¼��
    {{0x31000300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˳�ʼ���¼� - �����������Ա�
    {{0x31000400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˳�ʼ���¼� - ��ǰ��¼��
    {{0x31000500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˳�ʼ���¼� - ����¼��
    {{0x31000700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˳�ʼ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31000800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˳�ʼ���¼� - �ϱ���ʶ
    {{0x31000900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˳�ʼ���¼� - ��Ч��ʶ
    {{0x31000A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˳�ʼ���¼� - ʱ��״̬��¼��
       
    {{0x31010200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն˰汾����¼� - �¼���¼��
    {{0x31010300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˰汾����¼� - �����������Ա�
    {{0x31010400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˰汾����¼� - ��ǰ��¼��
    {{0x31010500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˰汾����¼� - ����¼��
    {{0x31010700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˰汾����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31010800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˰汾����¼� - �ϱ���ʶ
    {{0x31010900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˰汾����¼� - ��Ч��ʶ
    {{0x31010A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˰汾����¼� - ʱ��״̬��¼��
       
    {{0x31040200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն�״̬����λ�¼� - �¼���¼��
    {{0x31040300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն�״̬����λ�¼� - �����������Ա�
    {{0x31040400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�״̬����λ�¼� - ��ǰ��¼��
    {{0x31040500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�״̬����λ�¼� - ����¼��
    {{0x31040700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն�״̬����λ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31040800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�״̬����λ�¼� - �ϱ���ʶ
    {{0x31040900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�״̬����λ�¼� - ��Ч��ʶ
    {{0x31040A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն�״̬����λ�¼� - ʱ��״̬��¼��
       
    {{0x31050200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_CLOCKFLYLOG_T),    E_OOP_CLOCKFLYLOG_T},            //���ܱ�ʱ�ӳ����¼� - �¼���¼��
    {{0x31050300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʱ�ӳ����¼� - �����������Ա�
    {{0x31050400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�ӳ����¼� - ��ǰ��¼��
    {{0x31050500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʱ�ӳ����¼� - ����¼��
    {{0x31050600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3105_T),    E_OOP_EVTCFG_3105_T},            //���ܱ�ʱ�ӳ����¼� - ���ò���
    {{0x31050700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ʱ�ӳ����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31050800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�ӳ����¼� - �ϱ���ʶ
    {{0x31050900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʱ�ӳ����¼� - ��Ч��ʶ
    {{0x31050A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ʱ�ӳ����¼� - ʱ��״̬��¼��
       
    {{0x31060200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_POWERLOG_T),       E_OOP_POWERLOG_T},               //�ն�ͣ/�ϵ��¼� - �¼���¼��
    {{0x31060300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն�ͣ/�ϵ��¼� - �����������Ա�
    {{0x31060400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ͣ/�ϵ��¼� - ��ǰ��¼��
    {{0x31060500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ͣ/�ϵ��¼� - ����¼��
    {{0x31060600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_POWERONCUT_T),     E_OOP_POWERONCUT_T},             //�ն�ͣ/�ϵ��¼� - ���ò���
    {{0x31060700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն�ͣ/�ϵ��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31060800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ͣ/�ϵ��¼� - �ϱ���ʶ
    {{0x31060900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ͣ/�ϵ��¼� - ��Ч��ʶ
    {{0x31060A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն�ͣ/�ϵ��¼� - ʱ��״̬��¼��
    {{0x31060B00}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //�ն�ͣ/�ϵ��¼� - �����ɼ������
       
    {{0x31070200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն�ֱ��ģ����Խ�����¼� - �¼���¼��
    {{0x31070300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն�ֱ��ģ����Խ�����¼� - �����������Ա�
    {{0x31070400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ֱ��ģ����Խ�����¼� - ��ǰ��¼��
    {{0x31070500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ֱ��ģ����Խ�����¼� - ����¼��
    {{0x31070600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T),     E_OOP_EVTCFG_INT_T},             //�ն�ֱ��ģ����Խ�����¼� - ���ò���
    {{0x31070700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն�ֱ��ģ����Խ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31070800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ֱ��ģ����Խ�����¼� - �ϱ���ʶ
    {{0x31070900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ֱ��ģ����Խ�����¼� - ��Ч��ʶ
    {{0x31070A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն�ֱ��ģ����Խ�����¼� - ʱ��״̬��¼��
       
    {{0x31080200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն�ֱ��ģ����Խ�����¼� - �¼���¼��
    {{0x31080300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն�ֱ��ģ����Խ�����¼� - �����������Ա�
    {{0x31080400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ֱ��ģ����Խ�����¼� - ��ǰ��¼��
    {{0x31080500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն�ֱ��ģ����Խ�����¼� - ����¼��
    {{0x31080600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_INT_T),     E_OOP_EVTCFG_INT_T},             //�ն�ֱ��ģ����Խ�����¼� - ���ò���
    {{0x31080700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն�ֱ��ģ����Խ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31080800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ֱ��ģ����Խ�����¼� - �ϱ���ʶ
    {{0x31080900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն�ֱ��ģ����Խ�����¼� - ��Ч��ʶ
    {{0x31080A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն�ֱ��ģ����Խ�����¼� - ʱ��״̬��¼��
       
    {{0x31090200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն���Ϣ��֤�����¼� - �¼���¼��
    {{0x31090300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն���Ϣ��֤�����¼� - �����������Ա�
    {{0x31090400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն���Ϣ��֤�����¼� - ��ǰ��¼��
    {{0x31090500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն���Ϣ��֤�����¼� - ����¼��
    {{0x31090700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն���Ϣ��֤�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31090800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն���Ϣ��֤�����¼� - �ϱ���ʶ
    {{0x31090900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն���Ϣ��֤�����¼� - ��Ч��ʶ
    {{0x31090A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն���Ϣ��֤�����¼� - ʱ��״̬��¼��
       
    {{0x310A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�豸���ϼ�¼ - �¼���¼��
    {{0x310A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�豸���ϼ�¼ - �����������Ա�
    {{0x310A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�豸���ϼ�¼ - ��ǰ��¼��
    {{0x310A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�豸���ϼ�¼ - ����¼��
    {{0x310A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�豸���ϼ�¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�豸���ϼ�¼ - �ϱ���ʶ
    {{0x310A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�豸���ϼ�¼ - ��Ч��ʶ
    {{0x310A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�豸���ϼ�¼ - ʱ��״̬��¼��
       
    {{0x310B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�ʾ���½��¼� - �¼���¼��
    {{0x310B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ʾ���½��¼� - �����������Ա�
    {{0x310B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʾ���½��¼� - ��ǰ��¼��
    {{0x310B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ʾ���½��¼� - ����¼��
    {{0x310B0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //���ܱ�ʾ���½��¼� - ���ò���
    {{0x310B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ʾ���½��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʾ���½��¼� - �ϱ���ʶ
    {{0x310B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ʾ���½��¼� - ��Ч��ʶ
    {{0x310B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ʾ���½��¼� - ʱ��״̬��¼��
       
    {{0x310C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�����������¼� - �¼���¼��
    {{0x310C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�����������¼� - �����������Ա�
    {{0x310C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����������¼� - ��ǰ��¼��
    {{0x310C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����������¼� - ����¼��
    {{0x310C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T),    E_OOP_EVTCFG_310X_T},            //�����������¼� - ���ò���
    {{0x310C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�����������¼� - �ϱ���ʶ
    {{0x310C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�����������¼� - ��Ч��ʶ
    {{0x310C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�����������¼� - ʱ��״̬��¼��
       
    {{0x310D0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�����¼� - �¼���¼��
    {{0x310D0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����¼� - �����������Ա�
    {{0x310D0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ��ǰ��¼��
    {{0x310D0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����¼� - ����¼��
    {{0x310D0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310X_T),    E_OOP_EVTCFG_310X_T},            //���ܱ�����¼� - ���ò���
    {{0x310D0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310D0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - �ϱ���ʶ
    {{0x310D0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����¼� - ��Ч��ʶ
    {{0x310D0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����¼� - ʱ��״̬��¼��
       
    {{0x310E0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ܱ�ͣ���¼� - �¼���¼��
    {{0x310E0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�ͣ���¼� - �����������Ա�
    {{0x310E0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ͣ���¼� - ��ǰ��¼��
    {{0x310E0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�ͣ���¼� - ����¼��
    {{0x310E0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310E_T),    E_OOP_EVTCFG_310E_T},            //���ܱ�ͣ���¼� - ���ò���
    {{0x310E0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�ͣ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310E0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ͣ���¼� - �ϱ���ʶ
    {{0x310E0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�ͣ���¼� - ��Ч��ʶ
    {{0x310E0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�ͣ���¼� - ʱ��״̬��¼��
       
    {{0x310F0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն˳���ʧ���¼� - �¼���¼��
    {{0x310F0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˳���ʧ���¼� - �����������Ա�
    {{0x310F0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˳���ʧ���¼� - ��ǰ��¼��
    {{0x310F0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˳���ʧ���¼� - ����¼��
    {{0x310F0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310F_T),    E_OOP_EVTCFG_310F_T},            //�ն˳���ʧ���¼� - ���ò���
    {{0x310F0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˳���ʧ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x310F0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˳���ʧ���¼� - �ϱ���ʶ
    {{0x310F0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˳���ʧ���¼� - ��Ч��ʶ
    {{0x310F0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˳���ʧ���¼� - ʱ��״̬��¼��
       
    {{0x31100200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��ͨ�����������¼� - �¼���¼��
    {{0x31100300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��ͨ�����������¼� - �����������Ա�
    {{0x31100400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ͨ�����������¼� - ��ǰ��¼��
    {{0x31100500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ͨ�����������¼� - ����¼��
    {{0x31100600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3110_T),    E_OOP_EVTCFG_3110_T},            //��ͨ�����������¼� - ���ò���
    {{0x31100700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��ͨ�����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31100800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��ͨ�����������¼� - �ϱ���ʶ
    {{0x31100900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��ͨ�����������¼� - ��Ч��ʶ
    {{0x31100A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��ͨ�����������¼� - ʱ��״̬��¼��
       
    {{0x31110200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_SMLOG_T),          E_OOP_SMLOG_T},                  //����δ֪���ܱ��¼� - �¼���¼��
    {{0x31110300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //����δ֪���ܱ��¼� - �����������Ա�
    {{0x31110400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����δ֪���ܱ��¼� - ��ǰ��¼��
    {{0x31110500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����δ֪���ܱ��¼� - ����¼��
    {{0x31110700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //����δ֪���ܱ��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31110800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //����δ֪���ܱ��¼� - �ϱ���ʶ
    {{0x31110900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //����δ֪���ܱ��¼� - ��Ч��ʶ
    {{0x31110A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //����δ֪���ܱ��¼� - ʱ��״̬��¼��
       
    {{0x31120200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_UNMETERLOG_T),     E_OOP_UNMETERLOG_T},             //��̨�����ܱ��¼� - �¼���¼��
    {{0x31120300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��̨�����ܱ��¼� - �����������Ա�
    {{0x31120400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��̨�����ܱ��¼� - ��ǰ��¼��
    {{0x31120500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��̨�����ܱ��¼� - ����¼��
    {{0x31120700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��̨�����ܱ��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31120800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��̨�����ܱ��¼� - �ϱ���ʶ
    {{0x31120900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��̨�����ܱ��¼� - ��Ч��ʶ
    {{0x31120A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��̨�����ܱ��¼� - ʱ��״̬��¼��
       
    {{0x31140200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն˶�ʱ�¼� - �¼���¼��
    {{0x31140300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˶�ʱ�¼� - �����������Ա�
    {{0x31140400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˶�ʱ�¼� - ��ǰ��¼��
    {{0x31140500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˶�ʱ�¼� - ����¼��
    {{0x31140700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˶�ʱ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31140800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˶�ʱ�¼� - �ϱ���ʶ
    {{0x31140900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˶�ʱ�¼� - ��Ч��ʶ
    {{0x31140A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˶�ʱ�¼� - ʱ��״̬��¼��
       
    {{0x31150200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_RCTRLLOG_T),       E_OOP_RCTRLLOG_T},               //ң����բ��¼ - �¼���¼��
    {{0x31150300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ң����բ��¼ - �����������Ա�
    {{0x31150400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң����բ��¼ - ��ǰ��¼��
    {{0x31150500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң����բ��¼ - ����¼��
    {{0x31150700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ң����բ��¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31150800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ң����բ��¼ - �ϱ���ʶ
    {{0x31150900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ң����բ��¼ - ��Ч��ʶ
    {{0x31150A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ң����բ��¼ - ʱ��״̬��¼��
       
    {{0x31160200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_DCLOG_T),          E_OOP_DCLOG_T},                  //�й��ܵ������Խ���¼���¼ - �¼���¼��
    {{0x31160300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�й��ܵ������Խ���¼���¼ - �����������Ա�
    {{0x31160400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�й��ܵ������Խ���¼���¼ - ��ǰ��¼��
    {{0x31160500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�й��ܵ������Խ���¼���¼ - ����¼��
    {{0x31160600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3116_T),    E_OOP_EVTCFG_3116_T},            //�й��ܵ������Խ���¼���¼ - ���ò���
    {{0x31160700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�й��ܵ������Խ���¼���¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31160800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�й��ܵ������Խ���¼���¼ - �ϱ���ʶ
    {{0x31160900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�й��ܵ������Խ���¼���¼ - ��Ч��ʶ
    {{0x31160A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�й��ܵ������Խ���¼���¼ - ʱ��״̬��¼��
       
    {{0x31170200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�����·����״̬��λ�¼���¼ - �¼���¼��
    {{0x31170300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�����·����״̬��λ�¼���¼ - �����������Ա�
    {{0x31170400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����·����״̬��λ�¼���¼ - ��ǰ��¼��
    {{0x31170500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����·����״̬��λ�¼���¼ - ����¼��
    {{0x31170700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�����·����״̬��λ�¼���¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31170800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�����·����״̬��λ�¼���¼ - �ϱ���ʶ
    {{0x31170900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�����·����״̬��λ�¼���¼ - ��Ч��ʶ
    {{0x31170A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�����·����״̬��λ�¼���¼ - ʱ��״̬��¼��
       
    {{0x31180200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_PROGRAMLOG_T),     E_OOP_PROGRAMLOG_T},             //�ն˱�̼�¼ - �¼���¼��
    {{0x31180300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˱�̼�¼ - �����������Ա�
    {{0x31180400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˱�̼�¼ - ��ǰ��¼��
    {{0x31180500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˱�̼�¼ - ����¼��
    {{0x31180700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˱�̼�¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31180800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˱�̼�¼ - �ϱ���ʶ
    {{0x31180900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˱�̼�¼ - ��Ч��ʶ
    {{0x31180A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˱�̼�¼ - ʱ��״̬��¼��
       
    {{0x31190200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ն˵�����·�쳣�¼� - �¼���¼��
    {{0x31190300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˵�����·�쳣�¼� - �����������Ա�
    {{0x31190400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˵�����·�쳣�¼� - ��ǰ��¼��
    {{0x31190500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˵�����·�쳣�¼� - ����¼��
    {{0x31190700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˵�����·�쳣�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31190800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˵�����·�쳣�¼� - �ϱ���ʶ
    {{0x31190900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˵�����·�쳣�¼� - ��Ч��ʶ
    {{0x31190A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˵�����·�쳣�¼� - ʱ��״̬��¼��
       
    {{0x311A0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METEROFFLOG_T),    E_OOP_METEROFFLOG_T},            //���ܱ�����״̬�л��¼� - �¼���¼��
    {{0x311A0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����״̬�л��¼� - �����������Ա�
    {{0x311A0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����״̬�л��¼� - ��ǰ��¼��
    {{0x311A0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����״̬�л��¼� - ����¼��
    {{0x311A0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_311A_T),    E_OOP_EVTCFG_311A_T},            //���ܱ�����״̬�л��¼� - ���ò���
    {{0x311A0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����״̬�л��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311A0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����״̬�л��¼� - �ϱ���ʶ
    {{0x311A0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����״̬�л��¼� - ��Ч��ʶ
    {{0x311A0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����״̬�л��¼� - ʱ��״̬��¼��
       
    {{0x311B0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_P2PCLOCKLOG_T),    E_OOP_P2PCLOCKLOG_T},            //�ն˶Ե��ܱ�Уʱ��¼ - �¼���¼��
    {{0x311B0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˶Ե��ܱ�Уʱ��¼ - �����������Ա�
    {{0x311B0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˶Ե��ܱ�Уʱ��¼ - ��ǰ��¼��
    {{0x311B0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ն˶Ե��ܱ�Уʱ��¼ - ����¼��
    {{0x311B0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ն˶Ե��ܱ�Уʱ��¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311B0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˶Ե��ܱ�Уʱ��¼ - �ϱ���ʶ
    {{0x311B0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ն˶Ե��ܱ�Уʱ��¼ - ��Ч��ʶ
    {{0x311B0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ն˶Ե��ܱ�Уʱ��¼ - ʱ��״̬��¼��
       
    {{0x311C0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METERDIRTYLOG_T),  E_OOP_METERDIRTYLOG_T},          //���ܱ����ݱ����ؼ�¼ - �¼���¼��
    {{0x311C0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ����ݱ����ؼ�¼ - �����������Ա�
    {{0x311C0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����ݱ����ؼ�¼ - ��ǰ��¼��
    {{0x311C0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ����ݱ����ؼ�¼ - ����¼��
    {{0x311C0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //���ܱ����ݱ����ؼ�¼ - ���ò���
    {{0x311C0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ����ݱ����ؼ�¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311C0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����ݱ����ؼ�¼ - �ϱ���ʶ
    {{0x311C0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ����ݱ����ؼ�¼ - ��Ч��ʶ
    {{0x311C0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ����ݱ����ؼ�¼ - ʱ��״̬��¼��

    {{0x311D0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METOFFLINELOG_T),  E_OOP_METOFFLINELOG_T},          //���ܱ�����״̬�仯�¼� - �¼���¼��
    {{0x311D0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ܱ�����״̬�仯�¼� - �����������Ա�
    {{0x311D0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����״̬�仯�¼� - ��ǰ��¼��
    {{0x311D0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ܱ�����״̬�仯�¼� - ����¼��
    {{0x311D0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //���ܱ�����״̬�仯�¼� - ���ò���
    {{0x311D0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ܱ�����״̬�仯�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311D0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����״̬�仯�¼� - �ϱ���ʶ
    {{0x311D0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ܱ�����״̬�仯�¼� - ��Ч��ʶ
    {{0x311D0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ܱ�����״̬�仯�¼� - ʱ��״̬��¼��

    {{0x311E0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_METERDIRTYLOG_T),  E_OOP_METERDIRTYLOG_T},          //��������¼� - �¼���¼��
    {{0x311E0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��������¼� - �����������Ա�
    {{0x311E0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��������¼� - ��ǰ��¼��
    {{0x311E0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��������¼� - ����¼��
    {{0x311E0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //��������¼� - ���ò���
    {{0x311E0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311E0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��������¼� - �ϱ���ʶ
    {{0x311E0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��������¼� - ��Ч��ʶ
    {{0x311E0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��������¼� - ʱ��״̬��¼��

    {{0x311F0200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_REFUSENODEACCESSLOG_T),  E_OOP_REFUSENODEACCESSLOG_T},          //�ܾ��ӽڵ����� - �¼���¼��
    {{0x311F0300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ܾ��ӽڵ����� - �����������Ա�
    {{0x311F0400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ܾ��ӽڵ����� - ��ǰ��¼��
    {{0x311F0500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�ܾ��ӽڵ����� - ����¼��
    {{0x311F0600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_310B_T),    E_OOP_EVTCFG_310B_T},            //�ܾ��ӽڵ����� - ���ò���
    {{0x311F0700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�ܾ��ӽڵ����� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x311F0800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�ܾ��ӽڵ����� - �ϱ���ʶ
    {{0x311F0900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�ܾ��ӽڵ����� - ��Ч��ʶ
    {{0x311F0A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�ܾ��ӽڵ����� - ʱ��״̬��¼��

    {{0x31200200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //�����������쳣�¼���¼ - �¼���¼��
    {{0x31200300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�����������쳣�¼���¼ - �����������Ա�
    {{0x31200400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����������쳣�¼���¼ - ��ǰ��¼��
    {{0x31200500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����������쳣�¼���¼ - ����¼��
    {{0x31200700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�����������쳣�¼���¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31200800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�����������쳣�¼���¼ - �ϱ���ʶ
    {{0x31200900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�����������쳣�¼���¼ - ��Ч��ʶ
    {{0x31200A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�����������쳣�¼���¼ - ʱ��״̬��¼��

    {{0x31210200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //TAר��ģ�鹤���¼� - �¼���¼��
    {{0x31210300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //TAר��ģ�鹤���¼� - �����������Ա�
    {{0x31210400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //TAר��ģ�鹤���¼� - ��ǰ��¼��
    {{0x31210500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //TAר��ģ�鹤���¼� - ����¼��
    {{0x31210700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //TAר��ģ�鹤���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31210800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //TAר��ģ�鹤���¼� - �ϱ���ʶ
    {{0x31210900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //TAר��ģ�鹤���¼� - ��Ч��ʶ
    {{0x31210A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //TAר��ģ�鹤���¼� - ʱ��״̬��¼��
    
    {{0x31400200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_SECURITYLOG_T),    E_OOP_SECURITYLOG_T},            //��ȫ�����¼ - �¼���¼��
    {{0x31400300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��ȫ�����¼ - �����������Ա�
    {{0x31400400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ȫ�����¼ - ��ǰ��¼��
    {{0x31400500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ȫ�����¼ - ����¼��
    {{0x31400600}, 7,       0x03,   0x01, 0,      DT_STRUCTURE,            2*sizeof(uint32),             E_OOP_FLOW_PARAM_T},             //��ȫ�����¼ - �����·�
    {{0x31400700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��ȫ�����¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31400800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��ȫ�����¼ - �ϱ���ʶ
    {{0x31400900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��ȫ�����¼ - ��Ч��ʶ
    {{0x31400A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��ȫ�����¼ - ʱ��״̬��¼��

    {{0x31410200}, 7,       0x03,   0x01, 1,      DT_ARRAY,                sizeof(OOP_ABNORMALCTLOG_T),  E_OOP_ABNORMALCTLOG_T},          //STA�����¼� - �¼���¼��
    {{0x31410300}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //STA�����¼� - �����������Ա�
    {{0x31410400}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //STA�����¼� - ��ǰ��¼��
    {{0x31410500}, 7,       0x03,   0x01, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //STA�����¼� - ����¼��
    {{0x31410700}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //STA�����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31410800}, 7,       0x03,   0x01, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //STA�����¼� - �ϱ���ʶ
    {{0x31410900}, 7,       0x03,   0x01, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //STA�����¼� - ��Ч��ʶ
    {{0x31410A00}, 7,       0x03,   0x01, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //STA�����¼� - ʱ��״̬��¼��

#ifdef AREA_SHAANXI

	{{0x31990200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_ICCID_CHANGE_EVENT_T), E_OOP_ICCIDLOG_T},               //ICCID����¼� - �¼���¼��
    {{0x31990300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ICCID����¼� - �����������Ա�
    {{0x31990400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ICCID����¼� - ��ǰ��¼��
    {{0x31990500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ICCID����¼� - ����¼��
    {{0x31990700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ICCID����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x31990800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ICCID����¼� - �ϱ���ʶ
    {{0x31990900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ICCID����¼� - ��Ч��ʶ
    {{0x31990A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ICCID����¼� - ʱ��״̬��¼��
#endif

    {{0x32000200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_PCTRLLOG_T),       E_OOP_PCTRLLOG_T},               //������բ��¼ - �¼���¼��
    {{0x32000300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //������բ��¼ - �����������Ա�
    {{0x32000400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //������բ��¼ - ��ǰ��¼��
    {{0x32000500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //������բ��¼ - ����¼��
    {{0x32000700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //������բ��¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x32000800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //������բ��¼ - �ϱ���ʶ
    {{0x32000900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //������բ��¼ - ��Ч��ʶ
    {{0x32000A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //������բ��¼ - ʱ��״̬��¼��
       
    {{0x32010200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_ECTRLLOG_T),       E_OOP_ECTRLLOG_T},               //�����բ��¼ - �¼���¼��
    {{0x32010300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�����բ��¼ - �����������Ա�
    {{0x32010400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����բ��¼ - ��ǰ��¼��
    {{0x32010500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //�����բ��¼ - ����¼��
    {{0x32010700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //�����բ��¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x32010800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //�����բ��¼ - �ϱ���ʶ
    {{0x32010900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //�����բ��¼ - ��Ч��ʶ
    {{0x32010A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //�����բ��¼ - ʱ��״̬��¼��
       
    {{0x32020200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //����������ü�¼ - �¼���¼��
    {{0x32020300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //����������ü�¼ - �����������Ա�
    {{0x32020400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����������ü�¼ - ��ǰ��¼��
    {{0x32020500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����������ü�¼ - ����¼��
    {{0x32020700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //����������ü�¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x32020800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //����������ü�¼ - �ϱ���ʶ
    {{0x32020900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //����������ü�¼ - ��Ч��ʶ
    {{0x32020A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //����������ü�¼ - ʱ��״̬��¼��
       
    {{0x32030200}, 7,       0x03,   0x02, 1,      DT_ARRAY,                sizeof(OOP_EWARNLOG_T),       E_OOP_EWARNLOG_T},               //��ظ澯�¼���¼ - �¼���¼��
    {{0x32030300}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��ظ澯�¼���¼ - �����������Ա�
    {{0x32030400}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ظ澯�¼���¼ - ��ǰ��¼��
    {{0x32030500}, 7,       0x03,   0x02, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��ظ澯�¼���¼ - ����¼��
    {{0x32030700}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��ظ澯�¼���¼ - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x32030800}, 7,       0x03,   0x02, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��ظ澯�¼���¼ - �ϱ���ʶ
    {{0x32030900}, 7,       0x03,   0x02, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��ظ澯�¼���¼ - ��Ч��ʶ
    {{0x32030A00}, 7,       0x03,   0x02, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��ظ澯�¼���¼ - ʱ��״̬��¼��
    {{0x33000200}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_REPORTS_T),        E_OOP_REPORTS_T},                //�¼��ϱ�״̬
    {{0x33030206}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_SEARCHMET_RES_T),  E_OOP_SEARCHMET_RES_T},          //����δ֪���ܱ��¼�-һ���ѱ���
    {{0x33050206}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //������բ��Ԫ-������2min����
    {{0x33050209}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //������բ��Ԫ-���ض���
    {{0x3305020A}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //������բ��Ԫ-��բ����ǰ����
    {{0x33060209}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //�����բ��Ԫ-��բ����ʱ�ܼ��������
    {{0x330a0206}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //ң����բ��Ԫ-������2min����
    {{0x330a0207}, 8,       0x03,   0x03, 0,      DT_LONG64,               sizeof(int64),                E_LONG64},                       //ң����բ��Ԫ-����ǰ����
    {{0x33090204}, 8,       0x03,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_EVTSOURCE_T),      E_OOP_EVTSOURCE_T},              //�ն�ͣ�ϵ��¼�-�¼�����Դ
    {{0x33090206}, 8,       0x03,   0x03, 0,      DT_BIT_STRING,           sizeof(OOP_BITSTR8_T),        E_OOP_BITSTR8_T},                //�ն�ͣ�ϵ��¼�-���Ա�־
    {{0x33020206}, 8,       0x03,   0x03, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //�ն˱�̼�¼-��̶����б�
    {{0x33120206}, 8,       0x03,   0x03, 0,      DT_OCTET_STRING,         sizeof(OOP_OCTETVAR16_T),     E_OOP_OCTETVAR16_T},             //ģ������¼-ģ��������ͨ�ŵ�ַ
    {{0x33120207}, 8,       0x03,   0x03, 0,      DT_VISIBLE_STRING,       sizeof(OOP_VISIBLEVAR256_T),  E_VISIBLE_STRING},               //ģ������¼-���ǰ��ģ��������
    {{0x33120208}, 8,       0x03,   0x03, 0,      DT_VISIBLE_STRING,       sizeof(OOP_VISIBLEVAR256_T),  E_VISIBLE_STRING},               //ģ������¼-������ģ��������
    {{0x33130206}, 8,       0x03,   0x03, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),     E_DATETIME_S},                   //���ܱ�ʱ�ӳ���-���ܱ�ʱ��
    {{0x33130207}, 8,       0x03,   0x03, 0,      DT_DATETIME_S,           sizeof(OOP_DATETIME_S_T),     E_DATETIME_S},                   //���ܱ�ʱ�ӳ���-�ն˵�ǰʱ��
    {{0x33180204}, 8,       0x03,   0x03, 0,      DT_STRUCTURE,            sizeof(OOP_EVTSOURCE_T),      E_OOP_EVTSOURCE_T},              //�¼�����Դ

    {{0x3E030200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_IdentifyResult_T), E_OOP_IdentifyResult_T},    
    {{0x3E030800}, 7,       0x03,   0x0E, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //����ʶ���¼� - �ϱ���ʶ
    {{0x3E030900}, 7,       0x03,   0x0E, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED}, 

    {{0x34010200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ƻ�����ʧ���¼� - �¼���¼��
    {{0x34010300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ƻ�����ʧ���¼� - �����������Ա�
    {{0x34010400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ�����ʧ���¼� - ��ǰ��¼��
    {{0x34010500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ�����ʧ���¼� - ����¼��
    {{0x34010700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ƻ�����ʧ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34010800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ�����ʧ���¼� - �ϱ���ʶ
    {{0x34010900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ�����ʧ���¼� - ��Ч��ʶ
    {{0x34010A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ƻ�����ʧ���¼� - ʱ��״̬��¼��

    {{0x34020200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ƻ��ϱ��¼� - �¼���¼��
    {{0x34020300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ƻ��ϱ��¼� - �����������Ա�
    {{0x34020400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ��ϱ��¼� - ��ǰ��¼��
    {{0x34020500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ��ϱ��¼� - ����¼��
    {{0x34020700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ƻ��ϱ��¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34020800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ��ϱ��¼� - �ϱ���ʶ
    {{0x34020900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ��ϱ��¼� - ��Ч��ʶ
    {{0x34020A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ƻ��ϱ��¼� - ʱ��״̬��¼��

    {{0x34030200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��������¼� - �¼���¼��
    {{0x34030300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��������¼� - �����������Ա�
    {{0x34030400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��������¼� - ��ǰ��¼��
    {{0x34030500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��������¼� - ����¼��
    {{0x34030700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34030800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��������¼� - �ϱ���ʶ
    {{0x34030900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��������¼� - ��Ч��ʶ
    {{0x34030A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��������¼� - ʱ��״̬��¼��

    {{0x34040200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ִ���¼� - �¼���¼��
    {{0x34040300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ִ���¼� - �����������Ա�
    {{0x34040400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ִ���¼� - ��ǰ��¼��
    {{0x34040500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ִ���¼� - ����¼��
    {{0x34040700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ִ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34040800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ִ���¼� - �ϱ���ʶ
    {{0x34040900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ִ���¼� - ��Ч��ʶ
    {{0x34040A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ִ���¼� - ʱ��״̬��¼��

    {{0x34050200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���׮�쳣�¼� - �¼���¼��
    {{0x34050300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���׮�쳣�¼� - �����������Ա�
    {{0x34050400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���׮�쳣�¼� - ��ǰ��¼��
    {{0x34050500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���׮�쳣�¼� - ����¼��
    {{0x34050700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���׮�쳣�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34050800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���׮�쳣�¼� - �ϱ���ʶ
    {{0x34050900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���׮�쳣�¼� - ��Ч��ʶ
    {{0x34050A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���׮�쳣�¼� - ʱ��״̬��¼��

    {{0x34060200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��綩�������¼� - �¼���¼��
    {{0x34060300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��綩�������¼� - �����������Ա�
    {{0x34060400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��綩�������¼� - ��ǰ��¼��
    {{0x34060500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��綩�������¼� - ����¼��
    {{0x34060700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��綩�������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34060800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��綩�������¼� - �ϱ���ʶ
    {{0x34060900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��綩�������¼� - ��Ч��ʶ
    {{0x34060A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��綩�������¼� - ʱ��״̬��¼��

    {{0x34070200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //���ƻ��·�ʧ���¼� - �¼���¼��
    {{0x34070300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //���ƻ��·�ʧ���¼� - �����������Ա�
    {{0x34070400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ��·�ʧ���¼� - ��ǰ��¼��
    {{0x34070500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //���ƻ��·�ʧ���¼� - ����¼��
    {{0x34070700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //���ƻ��·�ʧ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x34070800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ��·�ʧ���¼� - �ϱ���ʶ
    {{0x34070900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //���ƻ��·�ʧ���¼� - ��Ч��ʶ
    {{0x34070A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //���ƻ��·�ʧ���¼� - ʱ��״̬��¼��

    {{0x35000200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //ң�ű�λ�¼� - �¼���¼��
    {{0x35000300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ң�ű�λ�¼� - �����������Ա�
    {{0x35000400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң�ű�λ�¼� - ��ǰ��¼��
    {{0x35000500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң�ű�λ�¼� - ����¼��
    {{0x35000700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ң�ű�λ�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35000800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ң�ű�λ�¼� - �ϱ���ʶ
    {{0x35000900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ң�ű�λ�¼� - ��Ч��ʶ
    {{0x35000A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ң�ű�λ�¼� - ʱ��״̬��¼��

    {{0x35010200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //ң��Խ���¼� - �¼���¼��
    {{0x35010300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //ң��Խ���¼� - �����������Ա�
    {{0x35010400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң��Խ���¼� - ��ǰ��¼��
    {{0x35010500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //ң��Խ���¼� - ����¼��
    {{0x35010700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //ң��Խ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35010800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //ң��Խ���¼� - �ϱ���ʶ
    {{0x35010900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //ң��Խ���¼� - ��Ч��ʶ
    {{0x35010A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //ң��Խ���¼� - ʱ��״̬��¼��

    {{0x35020200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //����˥����Խ���¼� - �¼���¼��
    {{0x35020300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //����˥����Խ���¼� - �����������Ա�
    {{0x35020400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����˥����Խ���¼� - ��ǰ��¼��
    {{0x35020500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //����˥����Խ���¼� - ����¼��
    {{0x35020700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //����˥����Խ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35020800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //����˥����Խ���¼� - �ϱ���ʶ
    {{0x35020900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //����˥����Խ���¼� - ��Ч��ʶ
    {{0x35020A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //����˥����Խ���¼� - ʱ��״̬��¼��

    {{0x35030200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��·�����������¼� - �¼���¼��
    {{0x35030300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��·�����������¼� - �����������Ա�
    {{0x35030400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·�����������¼� - ��ǰ��¼��
    {{0x35030500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·�����������¼� - ����¼��
    {{0x35030700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��·�����������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35030800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��·�����������¼� - �ϱ���ʶ
    {{0x35030900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��·�����������¼� - ��Ч��ʶ
    {{0x35030A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��·�����������¼� - ʱ��״̬��¼�� 

    {{0x35040200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��·��բλ�仯�¼�- �¼���¼��
    {{0x35040300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��·��բλ�仯�¼� - �����������Ա�
    {{0x35040400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·��բλ�仯�¼� - ��ǰ��¼��
    {{0x35040500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·��բλ�仯�¼�- ����¼��
    {{0x35040700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��·��բλ�仯�¼�- ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35040800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��·��բλ�仯�¼� - �ϱ���ʶ
    {{0x35040900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��·��բλ�仯�¼� - ��Ч��ʶ
    {{0x35040A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��·��բλ�仯�¼� - ʱ��״̬��¼�� 

    {{0x35050200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_PF_INPUT_EXIT_T),  E_OOP_PF_INPUT_EXIT_T},          //��·����������Ͷ���¼� - �¼���¼��
    {{0x35050300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��·����������Ͷ���¼� - �����������Ա�
    {{0x35050400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·����������Ͷ���¼� - ��ǰ��¼��
    {{0x35050500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·����������Ͷ���¼� - ����¼��
    {{0x35050700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��·����������Ͷ���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35050800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��·����������Ͷ���¼� - �ϱ���ʶ
    {{0x35050900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��·����������Ͷ���¼� - ��Ч��ʶ
    {{0x35050A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��·����������Ͷ���¼� - ʱ��״̬��¼��

    {{0x35060200}, 7,       0x03,   0x05, 1,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //��·���澯�¼� - �¼���¼��
    {{0x35060300}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_OADS_T),           E_OOP_OADS_T},                   //��·���澯�¼� - �����������Ա�
    {{0x35060400}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·���澯�¼� - ��ǰ��¼��
    {{0x35060500}, 7,       0x03,   0x05, 0,      DT_LONG_UNSIGNED,        sizeof(uint16),               E_LONG_UNSIGNED},                //��·���澯�¼�- ����¼��
    {{0x35060700}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTSTAT_T),        E_OOP_EVTSTAT_T},                //��·���澯�¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
    {{0x35060800}, 7,       0x03,   0x05, 0,      DT_ENUM,                 sizeof(uint8),                E_UNSIGNED},                     //��·���澯�¼�- �ϱ���ʶ
    {{0x35060900}, 7,       0x03,   0x05, 0,      DT_BOOL,                 sizeof(uint8),                E_UNSIGNED},                     //��·���澯�¼� - ��Ч��ʶ
    {{0x35060A00}, 7,       0x03,   0x05, 0,      DT_ARRAY,                sizeof(OOP_EVTTIME_T),        E_OOP_EVTTIME_T},                //��·���澯�¼� - ʱ��״̬��¼��

	{{0x36000200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		      //�豸����¼� - �¼���¼��
	{{0x36000300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //�豸����¼� - �����������Ա�
	{{0x36000400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //�豸����¼� - ��ǰ��¼��
	{{0x36000500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //�豸����¼� - ����¼��
	{{0x36000600}, 7,       0x03,   0x06, 0,      DT_STRUCTURE,            sizeof(OOP_EVTCFG_3600_T),    E_OOP_EVTCFG_3600_T},            //���ܱ�����״̬�仯�¼� - ���ò���
	{{0x36000700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //�豸����¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
	{{0x36000800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //�豸����¼� - �ϱ���ʶ
	{{0x36000900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //�豸����¼� - ��Ч��ʶ
	{{0x36000A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //�豸����¼� - ʱ��״̬��¼��
	
	{{0x36010200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		  	  //��������¼� - �¼���¼��
	{{0x36010300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //��������¼� - �����������Ա�
	{{0x36010400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //��������¼� - ��ǰ��¼��
	{{0x36010500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //��������¼� - ����¼��
	{{0x36010700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //��������¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
	{{0x36010800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //��������¼� - �ϱ���ʶ
	{{0x36010900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //��������¼� - ��Ч��ʶ
	{{0x36010A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //��������¼� - ʱ��״̬��¼��
	
	{{0x36020200}, 7,		0x03,	0x06, 1,	  DT_ARRAY, 			   sizeof(OOP_MONITORLOG_T),  E_OOP_SYSTEMMONITOR_T},		      //Ӧ�ü���¼� - �¼���¼��
	{{0x36020300}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_OADS_T),			 E_OOP_OADS_T}, 				  //Ӧ�ü���¼� - �����������Ա�
	{{0x36020400}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //Ӧ�ü���¼� - ��ǰ��¼��
	{{0x36020500}, 7,		0x03,	0x06, 0,	  DT_LONG_UNSIGNED, 	   sizeof(uint16),				 E_LONG_UNSIGNED},				  //Ӧ�ü���¼� - ����¼��
	{{0x36020700}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTSTAT_T),		 E_OOP_EVTSTAT_T},				  //Ӧ�ü���¼� - ��ǰֵ��¼��????�ǲ���������ṹ�壬��ȷ��
	{{0x36020800}, 7,		0x03,	0x06, 0,	  DT_ENUM,				   sizeof(uint8),				 E_UNSIGNED},					  //Ӧ�ü���¼� - �ϱ���ʶ
	{{0x36020900}, 7,		0x03,	0x06, 0,	  DT_BOOL,				   sizeof(uint8),				 E_UNSIGNED},					  //Ӧ�ü���¼� - ��Ч��ʶ
	{{0x36020A00}, 7,		0x03,	0x06, 0,	  DT_ARRAY, 			   sizeof(OOP_EVTTIME_T),		 E_OOP_EVTTIME_T},				  //Ӧ�ü���¼� - ʱ��״̬��¼��
	
#ifdef AREA_SHANDONG
    {{0x3E040200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�¶ȳ����¼�
    {{0x3E010200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //ʪ�ȳ����¼�
    {{0x3E020200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�ſ����¼�
    {{0x3E030200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�������¼�
    {{0x3E400200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //�����Ե��¼�
    {{0x3E410200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //ͼƬ��Ƶ�ɼ��¼�
    {{0x3E420200}, 7,       0x03,   0x0E, 0,      DT_ARRAY,                sizeof(OOP_EVENT_T),          E_OOP_EVENT_T},                  //ǿ�ų��¼�
#endif
};
const uint32 oadEvnetInfoListLen = sizeof(oadEvnetInfoList)/sizeof(oadEvnetInfoList[0]);

/* �α�����OAD�б� OIA1 = 0x04 */
const OAD_TAB_T oadParaInfoList[] =
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x40000200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //��ʱ���ʽ ����-ʱ��
    {{0x40000300}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_ENUM},                     //Уʱģʽ
    {{0x40000400}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCCLOCK_T),          E_OOP_SYNCCLOCK_T},          //��׼Уʱ����
    {{0x40000500}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_BROADCASTCLOCK_T),          E_OOP_BROADCASTCLOCK_T},          //�㲥Уʱ����
    {{0x40000600}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCCYCLE_T),          E_NULL},                     //���Ƕ�ʱ����
    {{0x40010200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //ͨ�ŵ�ַ
    {{0x40020200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //���
    {{0x40030200}, 8,      0x04,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //�ͻ����
    {{0x40040200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_GEOGADD_T),            E_OOP_GEOGADD_T},            //�豸����λ��
    {{0x40050200}, 8,      0x04,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_GRPADDR_T),            E_OOP_GRPADDR_T},            //���ַ
    {{0x40060200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CLOCK_T),              E_OOP_CLOCK_T},              //ʱ��Դ����
    {{0x40070200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LCD_T),                E_OOP_LCD_T},                //LCD����
    {{0x40080200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //������ʱ�����л�ʱ��
    {{0x40090200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //������ʱ�����л�ʱ��
    {{0x400A0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //������ʱ�����л�ʱ��
    {{0x400B0200}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //������ʱ�����л�ʱ��
    {{0x400C0201}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //ʱ��ʱ����
    {{0x400C0202}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //ʱ��ʱ����
    {{0x400C0203}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //ʱ��ʱ����
    {{0x400C0204}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //ʱ��ʱ����
    {{0x400C0205}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,             sizeof(uint8),             E_UNSIGNED},             //ʱ��ʱ����
    {{0x400C0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PERIOD_T),             E_OOP_PERIOD_T},             //ʱ��ʱ����
    {{0x400D0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //������
    {{0x400E0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //г����������
    {{0x400F0200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //��Կ������
    {{0x40100200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //����Ԫ����
    {{0x40110200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_PUBHOLIDAY_T),         E_OOP_PUBHOLIDAY_T},         //�������ձ�
    {{0x40120200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),        E_OOP_BITSTR8_T},        //������������
    {{0x40130200}, 8,      0x04,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //��������õ���ʱ�α��
    {{0x40140200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T),           E_OOP_TIMEZONE_T},           //��ǰ��ʱ����
    {{0x40150200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TIMEZONE_T),           E_OOP_TIMEZONE_T},           //������ʱ����
    {{0x40160200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T),          E_OOP_DAYPERIOD_T},          //��ǰ����ʱ�α�
    {{0x40170200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DAYPERIOD_T),          E_OOP_DAYPERIOD_T},          //��������ʱ�α�
    {{0x40180200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T),         E_OOP_RATETARIFF_T},         //��ǰ�׷��ʵ��
    {{0x40190200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_RATETARIFF_T),         E_OOP_RATETARIFF_T},         //�����׷��ʵ��
    {{0x401A0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T),          E_OOP_STEPPRICE_T},          //��ǰ�׽��ݵ��
    {{0x401B0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_STEPPRICE_T),          E_OOP_STEPPRICE_T},          //�����׽��ݵ��
    {{0x401C0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //�������������
    {{0x401D0200}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //��ѹ���������
    //{{0x401E0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_LIMITCHARGE_T),        E_OOP_LIMITCHARGE_T},        //���������ֵ
    {{0x401E0202}, 8,      0x04,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,             sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},        //���������ֵ
    {{0x401F0200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERLIMIT_T),         E_OOP_OTHERLIMIT_T},         //���������ֵ
    {{0x40200200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_POWERLIMIT_T),         E_OOP_POWERLIMIT_T},         //����������ֵ
    {{0x40210200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_OTHERPOWERLIMIT_T),    E_OOP_OTHERPOWERLIMIT_T},    //����������ֵ
    {{0x40220200}, 8,      0x04,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //�忨״̬��
    {{0x40240200}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //�޳�
    {{0x40250200}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_DC_UPDATE_T),          E_OOP_DC_UPDATE_T},          //��������б�
    {{0x40250300}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_DCU_CONFIG_T),         E_OOP_DCU_CONFIG_T},         //�ɼ����������Ʋ���
    {{0x40270200}, 8,      0x04,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},         

    {{0x40300200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_VOLTPARAM_T),          E_OOP_VOLTPARAM_T},          //��ѹ�ϸ��ʲ���
    {{0x40400200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TASPCUSEMOD_T),        E_OOP_TASPCUSEMOD_T},        //TAģ����Ϣ
    {{0x40400300}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_WORKINFO_T),           E_OOP_WORKINFO_T},           //TAģ������Ϣ
    {{0x40400400}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TAUNIT_T),             E_OOP_TAUNIT_T},             //TAģ�黻�㼰��λ
    {{0x40400500}, 8,      0x04,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_CTINFO_T),             E_OOP_CTINFO_T},             //TAģ��汾��Ϣ
    {{0x40410200}, 8,      0x04,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CURLOOPCKENABLE_T),    E_OOP_CURLOOPCKENABLE_T},    //������·���ʹ��

    {{0x41000200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�����������
    {{0x41010200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //����ʱ��
    {{0x41020200}, 8,      0x04,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //У��������
    {{0x41030200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //�ʲ��������
    {{0x41040200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //���ѹ
    {{0x41050200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //�����/��������
    {{0x41060200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR6_T),        E_OOP_VISIBLESTR6_T},        //������
    {{0x41070200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //�й�׼ȷ�ȵȼ�
    {{0x41080200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //�޹�׼ȷ�ȵȼ�
    {{0x41090200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //���ܱ��й�����
    {{0x410A0200}, 8,      0x04,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //���ܱ��޹�����
    {{0x410B0200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //���ܱ��ͺ�
    {{0x410C0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC����絼ϵ��
    {{0x410D0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC����翹ϵ��
    {{0x410E0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC�������ϵ��
    {{0x410F0200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COEFFICIENT_T),        E_OOP_COEFFICIENT_T},        //ABC�������ϵ��
    {{0x41100200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //���ܱ����з�ʽ������
    {{0x41110200}, 8,      0x04,   0x01, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //���������
    {{0x41110300}, 8,      0x04,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_SOFTREC_LIST),       E_OOP_SOFTREC_LIST},       //����������б�
    {{0x41120200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //�й���Ϸ�ʽ������
    {{0x41130200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //�޹���Ϸ�ʽ1������
    {{0x41140200}, 8,      0x04,   0x01, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),            E_OOP_BITSTR8_T},            //�޹���Ϸ�ʽ2������
    {{0x41160200}, 8,      0x04,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_PAYDAYS_T),            E_OOP_PAYDAYS_T},            //����������
    {{0x41170200}, 8,      0x04,   0x01, 0,      DT_TI,                    sizeof(OOP_TI_T),                 E_OOP_TI_T}, //�ڼ�������������
    {{0x41210200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_Power_Characteristic_Record),                 E_OOP_Power_Characteristic_Record},
    {{0x41220200}, 8,      0x04,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_Energy_Characteristic_Record_Unit),                 E_OOP_Energy_Characteristic_Record},
    {{0x42020200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_CASCADE_T),            E_OOP_CASCADE_T},            //����ͨ�Ų���
    {{0x42040200}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_SYNCTIME_T),           E_OOP_SYNCTIME_T},           //�ն˹㲥Уʱ����
    {{0x42040300}, 8,      0x04,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_P2PSYNCTIME_T),        E_OOP_P2PSYNCTIME_T},        //�ն˵���ַ�㲥Уʱ����
    {{0x43000200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //�����豸-�豸������
    {{0x43000300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //�����豸-�汾��Ϣ
    {{0x43000400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //�����豸-��������
    {{0x43000500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //�����豸-���豸�б�
    {{0x43000600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //�����豸-֧�ֹ�Լ�б�
    {{0x43000700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                 //�����豸-��������ϱ�
    {{0x43000800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                     //�����豸-���������ϱ�
    {{0x43000900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_BOOL},                 //�����豸-��������վͨ��
    {{0x43000A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //�����豸-�ϱ�ͨ��
    {{0x43000B00}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_SERVICES_T),           E_OOP_SERVICES_T},       //�����豸-һ�������Ȩ״̬
    {{0x43000C00}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_DEVICERUNSTAT_T),      E_OOP_DEVICERUNSTAT_T},       //�����豸-�豸����ʱ��

    {{0x43070200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //ˮ��-�豸������
    {{0x43070300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //ˮ��-�汾��Ϣ
    {{0x43070400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //ˮ��-��������
    {{0x43070500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //ˮ��-���豸�б�
    {{0x43070600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //ˮ��-֧�ֹ�Լ�б�
    {{0x43070700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //ˮ��-��������ϱ�
    {{0x43070800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //ˮ��-���������ϱ�
    {{0x43070900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //ˮ��-��������վͨ��
    {{0x43070A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //ˮ��-�ϱ�ͨ��
    {{0x43080200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //����-�豸������
    {{0x43080300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //����-�汾��Ϣ
    {{0x43080400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //����-��������
    {{0x43080500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //����-���豸�б�
    {{0x43080600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //����-֧�ֹ�Լ�б�
    {{0x43080700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //����-��������ϱ�
    {{0x43080800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //����-���������ϱ�
    {{0x43080900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //����-��������վͨ��
    {{0x43080A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //����-�ϱ�ͨ��
    {{0x43090200}, 19,     0x04,   0x03, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //�ȱ�-�豸������
    {{0x43090300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //�ȱ�-�汾��Ϣ
    {{0x43090400}, 19,     0x04,   0x03, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},         //�ȱ�-��������
    {{0x43090500}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_OIS_T),                E_OOP_OIS_T},                //�ȱ�-���豸�б�
    {{0x43090600}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //�ȱ�-֧�ֹ�Լ�б�
    {{0x43090700}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //�ȱ�-��������ϱ�
    {{0x43090800}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //�ȱ�-���������ϱ�
    {{0x43090900}, 19,     0x04,   0x03, 0,      DT_BOOL,                  sizeof(uint8),                    E_UNSIGNED},                 //�ȱ�-��������վͨ��
    {{0x43090A00}, 19,     0x04,   0x03, 0,      DT_ARRAY,                 sizeof(OOP_CHANNEL_T),            E_OOP_CHANNEL_T},            //�ȱ�-�ϱ�ͨ��
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI)    
    {{0x43201300}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Line_Loss_T),          E_OOP_LINE_LOSS_T},          //����
    {{0x43201700}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //��·�迹
    {{0x43201701}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //A���·�迹 
    {{0x43201702}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //B���·�迹 
    {{0x43201703}, 19,     0x04,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_Switch_Impedance_T),   E_OOP_SWITCH_IMPEDANCE_T},   //C���·�迹 
#endif    
    {{0x44000200}, 20,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_OI_RIGHTS_T),          E_OOP_OI_RIGHTS_T},     //�����б�???û���ҵ���Ӧ�ṹ��
    {{0x44000300}, 20,     0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_ASSOCIATION_CONTEXT_T),E_OOP_ASSOCIATION_CONTEXT_T},//Ӧ���ﾳ��Ϣ
    {{0x44000400}, 20,     0x04,   0x04, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //��ǰ���ӵĿͻ�����ַ
    {{0x44000500}, 20,     0x04,   0x04, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //������֤����
    {{0x44010200}, 8,      0x04,   0x04, 0,      DT_STRUCTURE,             sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //��֤����
    {{0x45000200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T),         E_OOP_GPRSCONFIG_T},         //����ͨ��ģ�����1 - GPRSͨ������
    {{0x45000300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //����ͨ��ģ�����1 - ��վͨ�Ų�����
    {{0x45000400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T),                E_OOP_SMS_T},                //����ͨ��ģ�����1 - ����ͨ�Ų���
    {{0x45000500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //����ͨ��ģ�����1 - �汾��Ϣ
    {{0x45000600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //����ͨ��ģ�����1 - ֧�ֹ�Լ�б�
    {{0x45000700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR32_T),       E_OOP_VISIBLEVAR32_T},       //����ͨ��ģ�����1 - SIM����ICCID
    {{0x45000800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T),       E_OOP_VISIBLESTR15_T},       //����ͨ��ģ�����1 - IMSI
    {{0x45000900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG},            		  //����ͨ��ģ�����1 - �ź�ǿ��
    {{0x45000A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //����ͨ��ģ�����1 - SIM������
    {{0x45000B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //����ͨ��ģ�����1 - ����IP
    {{0x45000C00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR_T),         E_OOP_VISIBLEVAR32_T},       //�豸������     
    {{0x45000D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T),            E_OOP_VISIBLEVAR32_T},       //��Ӫ�̼�������ʽ
    {{0x45000E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),           E_OOP_VISIBLEVAR32_T},       //����������
#if (defined AREA_SHAANXI)    
	{{0x45001600}, 25,	   0x04,   0x05, 0, 	 DT_STRUCTURE,			   sizeof(OOP_4GMONITOR_INFO_T),	 E_4GMONITOR_T},			  //����ͨ��ģ�����1 - ������
	{{0x45001700}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG},            		  //����ͨ��ģ�����1 - ������
	{{0x45001800}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETSTR4_T),          E_OCTET_STR4_T},             //����ͨ��ģ�����1 - С�����
	{{0x45001900}, 25,     0x04,   0x05, 0,      DT_ENUM,                  sizeof(uint8),                    E_ENUM},       			  //����ͨ��ģ�����1 - ������ʽ
#endif 

	{{0x45010200}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_GPRSCONFIG_T),         E_OOP_GPRSCONFIG_T},         //����ͨ��ģ�����2 - GPRSͨ������
    {{0x45010300}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //����ͨ��ģ�����2 - ��վͨ�Ų�����
    {{0x45010400}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_SMS_T),                E_OOP_SMS_T},                //����ͨ��ģ�����2 - ����ͨ�Ų���
    {{0x45010500}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_VERSION_T),            E_OOP_VERSION_T},            //����ͨ��ģ�����2 - �汾��Ϣ
    {{0x45010600}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_PROTOCOL_T),           E_OOP_PROTOCOL_T},           //����ͨ��ģ�����2 - ֧�ֹ�Լ�б�
    {{0x45010700}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR20_T),       E_OOP_VISIBLESTR20_T},       //����ͨ��ģ�����2 - SIM����ICCID
    {{0x45010800}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR15_T),       E_OOP_VISIBLESTR15_T},       //����ͨ��ģ�����2 - IMSI
    {{0x45010900}, 25,     0x04,   0x05, 0,      DT_LONG,                  sizeof(int16),                    E_LONG_UNSIGNED},            //����ͨ��ģ�����2 - �ź�ǿ��
    {{0x45010A00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR16_T),       E_OOP_VISIBLESTR16_T},       //����ͨ��ģ�����2 - SIM������
    {{0x45010B00}, 25,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR16_T),         E_OOP_OCTETVAR16_T},         //����ͨ��ģ�����2 - ����IP
    {{0x45010C00}, 25,     0x04,   0x05, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR_T),         E_OOP_VISIBLEVAR32_T},       //�豸������ 
    {{0x45010D00}, 25,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_NETINFO_T),            E_OOP_VISIBLEVAR32_T},       //��Ӫ�̼�������ʽ
    {{0x45010E00}, 25,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),           E_OOP_VISIBLEVAR32_T},       //����������
    
    {{0x45100200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����1 - ��̫��ͨ������
    {{0x45100300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����1 - ��վͨ�Ų�����
    {{0x45100400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����1 - ��̫������IP����
    {{0x45100500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����1 - MAC��ַ
    {{0x45110200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����2 - ��̫��ͨ������
    {{0x45110300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����2 - ��վͨ�Ų�����
    {{0x45110400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����2 - ��̫������IP����
    {{0x45110500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����2 - MAC��ַ
    {{0x45120200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����3 - ��̫��ͨ������
    {{0x45120300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����3 - ��վͨ�Ų�����
    {{0x45120400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����3 - ��̫������IP����
    {{0x45120500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����3 - MAC��ַ
    {{0x45130200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����4 - ��̫��ͨ������
    {{0x45130300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����4 - ��վͨ�Ų�����
    {{0x45130400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����4 - ��̫������IP����
    {{0x45130500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����4 - MAC��ַ
    {{0x45140200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����5 - ��̫��ͨ������
    {{0x45140300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����5 - ��վͨ�Ų�����
    {{0x45140400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����5 - ��̫������IP����
    {{0x45140500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����5 - MAC��ַ
    {{0x45150200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����6 - ��̫��ͨ������
    {{0x45150300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����6 - ��վͨ�Ų�����
    {{0x45150400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����6 - ��̫������IP����
    {{0x45150500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����6 - MAC��ַ
    {{0x45160200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����7 - ��̫��ͨ������
    {{0x45160300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����7 - ��վͨ�Ų�����
    {{0x45160400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����7 - ��̫������IP����
    {{0x45160500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����7 - MAC��ַ
    {{0x45170200}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHCONFIG_T),          E_OOP_ETHCONFIG_T},          //��̫��ͨ��ģ�����8 - ��̫��ͨ������
    {{0x45170300}, 26,     0x04,   0x05, 0,      DT_ARRAY,                 sizeof(OOP_MASTERPARAMS_T),       E_OOP_MASTERPARAMS_T},       //��̫��ͨ��ģ�����8 - ��վͨ�Ų�����
    {{0x45170400}, 26,     0x04,   0x05, 0,      DT_STRUCTURE,             sizeof(OOP_ETHIP_T),              E_OOP_ETHIP_T},              //��̫��ͨ��ģ�����8 - ��̫������IP����
    {{0x45170500}, 26,     0x04,   0x05, 0,      DT_OCTET_STRING,          sizeof(OOP_ETHMAC_T),             E_OOP_ETHMAC_T},             //��̫��ͨ��ģ�����8 - MAC��ַ
    {{0x47010200}, 8,      0x04,   0x07, 0,      DT_STRUCTURE,             sizeof(OOP_PILE_WORK_STATUS_T),   E_PILE_WORK_STATUS_T},       //����״̬
    {{0x47020400}, 8,      0x04,   0x07, 0,      DT_STRUCTURE,             sizeof(OOP_CHARGE_SERVICE_T),     E_CHARGE_SERVICE_T},         //������
    {{0x48000400}, 8,      0x04,   0x08, 0,      DT_STRUCTURE,             sizeof(OOP_DEVFREZDATAS_T),       E_OOP_DEVFREZDATAS_T},       //�����豸�������ݼ�
    {{0x49080200}, 8,      0x04,   0x09, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //�������ɼ�����ʱ��
#if (defined AREA_FUJIAN)    
	{{0x4e010200}, 8,	   0x04,   0x0e, 0, 	 DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),       	 E_OOP_BITSTR16_T},		  	  //��ǰ��Դ״̬��
	{{0x4e020200}, 8,	   0x04,   0x0e, 0, 	 DT_STRUCTURE,			   sizeof(OOP_BATTERY_STATUS_T),	 E_BATTERY_STATUS_T},		  //ͣ�繤�����״̬
	{{0x4e030200}, 8,	   0x04,   0x0e, 0, 	 DT_STRUCTURE,			   sizeof(OOP_SINGAL_INFO_T),	     E_SINGAL_INFO_T},			  //�ֳ��ź���Ϣ
#endif 


};
const uint32 oadParaInfoListLen = sizeof(oadParaInfoList)/sizeof(oadParaInfoList[0]);

/* ������OAD�б� OIA1 = 0x05 */
const OAD_TAB_T oadFreezeInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x50000200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //˲ʱ���� - �������ݱ�
    {{0x50000300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //˲ʱ���� - �����������Ա�
    {{0x50010200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�붳�� - �������ݱ�
    {{0x50010300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�붳�� - �����������Ա�
    {{0x50020200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //���Ӷ��� - �������ݱ�
    {{0x50020300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //���Ӷ��� - �����������Ա�
    {{0x50030200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //Сʱ���� - �������ݱ�
    {{0x50030300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //Сʱ���� - �����������Ա�
    {{0x50040200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�ն��� - �������ݱ�
    {{0x50040300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�ն��� - �����������Ա�
    {{0x50050200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�����ն��� - �������ݱ�
    {{0x50050300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�����ն��� - �����������Ա�
    {{0x50060200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�¶��� - �������ݱ�
    {{0x50060300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�¶��� - �����������Ա�
    {{0x50070200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�궳�� - �������ݱ�
    {{0x50070300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�궳�� - �����������Ա�
    {{0x50080200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //ʱ�����л����� - �������ݱ�
    {{0x50080300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //ʱ�����л����� - �����������Ա�
    {{0x50090200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //��ʱ�α��л����� - �������ݱ�
    {{0x50090300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //��ʱ�α��л����� - �����������Ա�
    {{0x500A0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //���ʵ���л����� - �������ݱ�
    {{0x500A0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //���ʵ���л����� - �����������Ա�
    {{0x500B0200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //�����л����� - �������ݱ�
    {{0x500B0300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //�����л����� - �����������Ա�
    {{0x50110200}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_APNGROUP_T),    E_OOP_APNGROUP_T},           //���ݽ��㶳�� - �������ݱ�
    {{0x50110300}, 9,      0x05,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_FROZENSETS_T),  E_OOP_FROZENSETS_T},           //���ݽ��㶳�� - �����������Ա�
};
const uint32 oadFreezeInfoListLen = sizeof(oadFreezeInfoList)/sizeof(oadFreezeInfoList[0]);

/* �ɼ���OAD�б� OIA1 = 0x06 */
const OAD_TAB_T oadCollectInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x60000200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_METER_LIST},           //�ɼ��������ñ�
    {{0x60010200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_OOP_METER_T},              //�ɼ��������õ�Ԫ
    {{0x60010201}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�������
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             10,                              E_BASIC_OBJECT},             //������Ϣ
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //������Ϣ - ͨ�ŵ�ַ
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ������
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ��Լ����
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //������Ϣ - �˿�
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_PWD_COMM},                 //������Ϣ - ͨ������
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //������Ϣ - ���ʸ���
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //������Ϣ - �û�����
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ���߷�ʽ
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //������Ϣ - ��ȵ�ѹ
    {{0x60010202}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //������Ϣ - ��ȵ���
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_EXTENDED_OBJECT},          //��չ��Ϣ
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //��չ��Ϣ - �ɼ�����ַ
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_ASSETNO},                  //��չ��Ϣ - �ʲ���
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��չ��Ϣ - ��ѹ���������
    {{0x60010203}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��չ��Ϣ - �������������
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_ANNEX_OBJECT_LIST},        //������Ϣ   
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_ANNEX_OBJECT},             //������Ϣ   
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //������Ϣ - ������������
    {{0x60010204}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //������Ϣ - ����ֵ                          
    {{0x60000300}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�ɼ��������ñ� - ��ǰԪ�ظ���
    {{0x60000400}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�ɼ��������ñ� - ���Ԫ�ظ���
    {{0x60020200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_SEARCHMET_RES_LIST},   //�����ѱ���
    {{0x60030200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             7,                               E_OOP_SEARCHMET_RES_T},      //һ���ѱ���
    {{0x60030201}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //ͨ�ŵ�ַ
    {{0x60030202}, 8,      0x06,   0x00, 0,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //�����ɼ�����ַ
    {{0x60030203}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //��Լ����
    {{0x60030204}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //��λ
    {{0x60030205}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //�ź�Ʒ��
    {{0x60030206}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},         //�ѵ���ʱ��
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_ADDEDINFO_LIST},       //�ѵ��ĸ�����Ϣ
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_OOP_ADDEDINFO_T},          //�ѵ��ĸ�����Ϣ
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //�ѵ��ĸ�����Ϣ - ������������
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //�ѵ��ĸ�����Ϣ - ����ֵ 
    {{0x60020500}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_ACROSSAREA_RES_LIST},  //��̨�����
    {{0x60040200}, 11,     0x06,   0x00, 1,      DT_STRUCTURE,             3,                               E_OOP_ACROSSAREA_RES_T},     //һ����̨�����
    {{0x60040201}, 8,      0x06,   0x00, 1,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //ͨ�ŵ�ַ
    {{0x60040202}, 8,      0x06,   0x00, 1,      DT_TSA,                   sizeof(OOP_TSA_T),               E_TSA},                      //���ڵ��ַ
    {{0x60040203}, 8,      0x06,   0x00, 1,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},         //���ʱ��
    {{0x60020400}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //���Ԫ�ظ���
    {{0x60020600}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�����ѱ�����¼��
    {{0x60020700}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��̨���ѱ�����¼��
    {{0x60020800}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHPARAM_T),       E_OOP_SEARCHPARAM_T},        //�ѱ����
    {{0x60020900}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TIMINGSEARCH_T),      E_OOP_TIMINGSEARCH_T},       //��ʱ�ѱ��������
    {{0x60020A00}, 11,     0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //�ѱ�״̬
    {{0x60120200}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_TASK_LIST},            //�������ñ�
    {{0x60120300}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //��ȡ�����¼��
    {{0x60128000}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ɾ��һ������
    {{0x60128100}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //����������ñ�
    {{0x60130200}, 10,     0x06,   0x00, 1,      DT_STRUCTURE,             sizeof(OOP_TASK_T),              E_OOP_TASK_T},               //�������õ�Ԫ


    {{0x60140200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_NORMAL_T),       E_OOP_PLAN_NORMAL_T},        //��ͨ�ɼ�������
    {{0x60160200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_EVENT_T),        E_OOP_PLAN_EVENT_T},         //�¼��ɼ�������
    {{0x60180200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_TRANS_T),        E_OOP_PLAN_TRANS_T},         //͸��������
    {{0x601A0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TRANSRES_T),          E_OOP_TRANSRES_T},           //͸�����������
    {{0x601C0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_REPORT_T),       E_OOP_PLAN_REPORT_T},        //�ϱ�������
    {{0x601E0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_ENCRYPT_T),      E_NULL},                     //ת���ܷ���
    {{0x60200200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ENCRYPTRES_T),        E_NULL},                     //ת���ܽ��
    {{0x60320200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_RLUESTATE_T),     E_OOP_ACQ_RLUESTATE_T},      //�ɼ�״̬��
    {{0x60340200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T),       E_OOP_ACQ_MONITOR_T},        //�ɼ������ؼ�

    {{0x60400200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //�¼�����ʱ��
    {{0x60410200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //�¼�����ʱ��
    {{0x60420200}, 8,      0x06,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},

    {{0x60510200}, 6,      0x06,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_LNLS_STATISVAL_T),    E_OOP_LNLS_STATISVAL_T},     //�����������
    {{0x60510400}, 6,      0x06,   0x00, 0,      DT_TI,                    sizeof(OOP_LNLSPER_T),           E_OOP_LNLSPER_T},            //����ͳ������
//    {{0x6E430200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,                    sizeof(ABNORMAL_METER_INFO_T),           E_ABNORMAL_METER_INFO_T},            //�쳣�������
    {{0x60F10200}, 11,      0x06,   0x00, 0,      DT_ARRAY,                sizeof(OOP_NodeTopology_T), E_OOP_NodeTopology_T},  //�ڵ�������Ϣ
    
    {{0x6E000200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(OOP_GATHER_E),            E_NULL},                     //�ⲿ�豸���ݲɼ�ģʽ
    {{0x6E000300}, 8,      0x06,   0x0E, 1,      DT_UNSIGNED,              sizeof(uint8),                   E_NULL},                     //д����ʣ������ʱ��
    {{0x6E000400}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //�����������������ȼ�
    {{0x6E010200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //�ն˱���ɼ�ģʽ
    {{0x6E020200}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //����оƬ����
    {{0x6E030200}, 8,      0x06,   0x0E, 1,      DT_ARRAY,                 sizeof(OOP_PORTS_TI_T),          E_NULL},                     //�˿ڳ�ʱ��Ӧʱ��
    {{0x6E030300}, 8,      0x06,   0x0E, 1,      DT_ENUM,                  sizeof(uint8),                   E_NULL},                     //�˿����ݹ�����
    {{0x6E050200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //Ⱥ�鵵����
    {{0x6E050300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //Ⱥ�鵵����¼��
    {{0x6E060200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_GROUP_METER_T),       E_NULL},                     //һ��Ⱥ�鵵��
    {{0x6E080200}, 8,      0x06,   0x0E, 1,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),        E_DATETIME_S},               //��һ�ν��յ���վͨ�ű���ʱ��
    {{0x6E110200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ȫ��͸�����񷽰�
    {{0x6E110300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ȫ��͸�����񷽰�ͳ��
    {{0x6E120200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_T),        E_NULL},                     //һ��ȫ��͸�����񷽰�
    {{0x6E130200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ȫ��͸����������
    {{0x6E140200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_NODE_T),   E_NULL},                     //һ��ȫ��͸����������
    {{0x6E150200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //����͸�������������
    {{0x6E160200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_TASK_RESULT_T), E_NULL},                     //һ������͸���������
    {{0x6E170200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //�����ϱ���������������
    {{0x6E180200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_TRANS_REPORT_T),      E_NULL},                     //һ�������ϱ���������������
    {{0x6E200200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //͸�����񷽰��ɼ�״̬��
    {{0x6E210200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,         sizeof(OOP_TRANS_TASK_RLUESTATE_T),  E_NULL},                     //һ��͸�����񷽰��ɼ�״̬
    {{0x6E220200}, 8,      0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ͨ���ɼ�״̬
    {{0x6E230200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //�ɼ�״̬��������
    {{0x6E240200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,            sizeof(OOP_TRANS_ACQ_RLUESTATE_T),E_NULL},                     //һ���ɼ�״̬��������
    {{0x6E250200}, 8,      0x06,   0x0E, 1,      DT_STRUCTURE,             sizeof(OOP_STORE_SIZE_T),        E_NULL},                     //�洢�ռ�ʹ�����
    {{0x6E310200}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //����͸���¼��������
    {{0x6E310300}, 11,     0x06,   0x0E, 1,      DT_ARRAY,                 0,                               E_NULL},                     //����͸���¼���¼״̬
    {{0x6E320200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_EVENT_T),       E_NULL},                     //һ������͸���¼����
    {{0x6E330200}, 8,      0x06,   0x0E, 0,      DT_STRUCTURE,             sizeof(OOP_TRANS_EVENT_REPORT_T),E_NULL},                     //�����ϱ��¼��ϱ�����
};

const uint32 oadCollectInfoListLen = sizeof(oadCollectInfoList)/sizeof(oadCollectInfoList[0]);

/* ������OAD�б� OIA1 = 0x07 */
const OAD_TAB_T oadGatherInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x70000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ļ����� - ��ǰԪ�ظ���
    {{0x70000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ļ����� - ���Ԫ�ظ���
    {{0x70100300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ű����� - ��ǰԪ�ظ���
    {{0x70100400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ű����� - ���Ԫ�ظ���
    {{0x70120300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ű�ִ�н���� - ��ǰԪ�ظ���
    {{0x70120400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //�ű�ִ�н���� - ���Ԫ�ظ���
    {{0x71000300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //��չ�������󼯺� - ��ǰԪ�ظ���
    {{0x71000400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //��չ�������󼯺� - ���Ԫ�ظ���
    {{0x71010300}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //��չ�α������󼯺� - ��ǰԪ�ظ���
    {{0x71010400}, 11,     0x07,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},           //��չ�α������󼯺� - ���Ԫ�ظ���
    {{0x73000200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_CONTAINER_LIST},      //��������
    {{0x73010200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //Ӧ�ü���
    {{0x73010500}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //Ӧ�ü���
    {{0x73000200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_CONTAINER_LIST},      //��������
    {{0x73010200}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //Ӧ�ü���
    {{0x73010300}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //�����Ϣ
    {{0x73010500}, 11,     0x07,   0x03, 0,      DT_ARRAY,                  0,                              E_OOP_APP_INFO_LIST},       //�����Ϣ
};
const uint32 oadGatherInfoListLen = sizeof(oadGatherInfoList)/sizeof(oadGatherInfoList[0]);

/* ���ƽӿ���OAD�б� OIA1 = 0x08 */
const OAD_TAB_T oadControlInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x80000200}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_TELECON_PARAM_T),     E_OOP_TELECON_PARAM_T},      //Զ�̿��� - ���ò���

#ifdef PRODUCT_ZCU_1
    {{0x80000400}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),          E_OOP_BITSTR16_T},           //Զ�̿��� - �澯״̬
    {{0x80000500}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR16_T),          E_OOP_BITSTR16_T},           //Զ�̿��� - ����״̬
#else
    {{0x80000400}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),           E_OOP_BITSTR8_T},            //Զ�̿��� - �澯״̬
    {{0x80000500}, 8,      0x08,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),           E_OOP_BITSTR8_T},            //Զ�̿��� - ����״̬
#endif

    {{0x80010200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_UNSIGNED},                 //����״̬
    {{0x80010300}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��������վ�����ͨ��ʱ�������ӣ���0��ʾ���Զ�����
    {{0x80010400}, 8,      0x08,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�ϵ��Զ�����ʱ�������ӣ���0��ʾ�ϵ粻�Զ����硣
    {{0x80010500}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_PRTTIME_T),           E_OOP_PRTTIME_T},            //�Զ�����ʱ��
    {{0x80020200}, 8,      0x08,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_UNSIGNED},                 //�߷Ѹ澯״̬��ֻ��
    {{0x80020300}, 8,      0x08,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_CALLFEE_T),           E_OOP_CALLFEE_T},            //�߷Ѹ澯������ֻ��
    {{0x80030200}, 11,     0x08,   0x00, 0,      DT_ARRAY,                 sizeof(CHINESEINFO),             E_CHINESEINFO},              //һ��������Ϣ
    {{0x80040200}, 11,     0x08,   0x00, 0,      DT_ARRAY,                 sizeof(CHINESEINFO),             E_CHINESEINFO},              //��Ҫ������Ϣ
    {{0x81000200}, 8,      0x08,   0x01, 0,      DT_LONG64,                sizeof(int64),                   E_LONG64},                   //�ն˱�����ֵ 
    {{0x81010200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_TIME_T),       E_OOP_POWCON_TIME_T},        //�ն˹���ʱ�� 
    {{0x81020200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_POWCON_WARNING_T),    E_OOP_POWCON_WARNING_T},     //���ظ澯ʱ��
    {{0x81030200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8103_CONFIG_T),       E_OOP_8103_CONFIG_T},        //ʱ�ι���  
    {{0x81030300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81030400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81030500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬ 
    {{0x81040200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8104_CONFIG_T),       E_OOP_8104_CONFIG_T},        //���ݿ�  
    {{0x81040300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81040400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81040500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬  
    {{0x81050200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8105_CONFIG_T),       E_OOP_8105_CONFIG_T},        //Ӫҵ��ͣ�� 
    {{0x81050300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81050400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81050500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬  
    {{0x81060300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81060400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81060500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬ 
    {{0x81070200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8107_CONFIG_T),       E_OOP_8107_CONFIG_T},        //�����   
    {{0x81070300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81070400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81070500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬ 
    {{0x81080200}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_8108_CONFIG_T),       E_OOP_8108_CONFIG_T},        //�µ�� 
    {{0x81080300}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //����Ͷ��״̬
    {{0x81080400}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TURNSTATES_T),        E_OOP_TURNSTATES_T},         //�ִ�״̬
    {{0x81080500}, 13,     0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_TGSTATES_T),          E_OOP_TGSTATES_T},           //���Ƹ澯״̬ 
    {{0x810C0200}, 8,      0x08,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_CFGUNIT_810C_T),      E_OOP_GDK_PARAM_T},          //��������õ�Ԫ 
    {{0x810E0200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(uint16),                  E_NULL},           //���ƶ��� 
    {{0x810F0200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_BITSTR8_T),           E_NULL},           //��բ�ִ� 
    {{0x81100200}, 8,      0x08,   0x01, 0,      DT_ARRAY,                 sizeof(int64),                   E_NULL},           //��ض�ֵ 
};
const uint32 oadControlInfoListLen = sizeof(oadControlInfoList)/sizeof(oadControlInfoList[0]);

/* �ļ�����ӿ���OAD�б� OIA1 = 0x0F OIA2 = 0x00 */
const OAD_TAB_T oadFileInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF0000200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //�ļ���֡�������- �ļ���Ϣ
    {{0xF0000300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //�ļ���֡������� - ������
    {{0xF0000400}, 18,     0x0F,   0x00, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),       E_OOP_OCTETVAR1K_T},        //�ļ���֡������� - �ļ�����
    {{0xF0000500}, 18,     0x0F,   0x00, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                 E_DOUBLE_LONG_UNSIGNED},    //�ļ���֡������� - ��ǰָ��
    {{0xF0010200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //�ļ��ֿ鴫����� - �ļ���Ϣ
    {{0xF0010300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //�ļ��ֿ鴫����� - ������
    {{0xF0010400}, 18,     0x0F,   0x00, 0,      DT_BIT_STRING,            sizeof(OOP_BITSTR8_T),          E_OOP_BITSTR8_T},           //�ļ��ֿ鴫����� - �����״̬��
    {{0xF0020200}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FILE_T),             E_OOP_FILE_T},              //�ļ���չ������� - �ļ���Ϣ
    {{0xF0020300}, 18,     0x0F,   0x00, 0,      DT_ENUM,                  sizeof(OOP_FILEEXC_RST_E),      E_OOP_FILEEXC_RST_E},       //�ļ���չ������� - ������
    {{0xF0020400}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FTPSERVER_T),        E_OOP_FTPSERVER_T},         //�ļ���չ������� - ��������Ϣ
};
const uint32 oadFileInfoListLen = sizeof(oadFileInfoList)/sizeof(oadFileInfoList[0]);


/* ESAM�ӿ���OAD�б� OIA1 = 0x0F OIA2 = 0x01 */
const OAD_TAB_T oadEsamInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF1000200}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //ESAM���к�
    {{0xF1000300}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //ESAM�汾��
    {{0xF1000400}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //�Գ���Կ�汾
    {{0xF1000500}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //�ỰʱЧ����
    {{0xF1000600}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //�ỰʱЧʣ��ʱ��
    {{0xF1000700}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_CUR_COUNT_T),          E_OOP_CUR_COUNT_T},          //��ǰ������(û�ҵ���Ӧ�Ľṹ��)
    {{0xF1000800}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_LICENSEVER_T),         E_OOP_LICENSEVER_T},         //֤��汾
    {{0xF1000900}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //�ն�֤�����к�
    {{0xF1000A00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR2K_T),         E_OOP_OCTETVAR2K_T},         //�ն�֤��
    {{0xF1000B00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //��վ֤�����к�
    {{0xF1000C00}, 21,     0x0F,   0x01, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR2K_T),         E_OOP_OCTETVAR2K_T},         //��վ֤��
    {{0xF1000D00}, 21,     0x0F,   0x01, 0,      DT_ARRAY,                 sizeof(OOP_OADS_T),               E_OOP_OADS_T},               //ESAM ��ȫ�洢�����б�
    {{0xF1000E00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //������֤ʱЧ����
    {{0xF1000F00}, 21,     0x0F,   0x01, 0,      DT_DOUBLE_LONG_UNSIGNED,  sizeof(uint32),                   E_DOUBLE_LONG_UNSIGNED},     //������֤ʣ��ʱ��
    {{0xF1010200}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SECURITYFLAG_E),       E_ENUM},                     //��ȫģʽ���� 
    {{0xF1010300}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(OOP_SCURITYMODE_T),        E_OOP_SCURITYMODE_T},        //ȫģʽ���� 
    {{0xF1001100}, 21,     0x0F,   0x01, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},
    {{0xF1001500}, 21,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_TERMINAL_LIMIT_T),     E_OOP_TERMINAL_LIMIT_T},                   //�ն˻Ự����
    {{0xF1010400}, 8,      0x0F,   0x01, 0,      DT_ENUM,                  sizeof(uint8),                    E_UNSIGNED},                 //SAL��ȫӦ��������·����� 
    {{0xF4000200}, 8,      0x0F,   0x04, 0,      DT_ARRAY,                 sizeof(OOP_LOG_INFO_T),           E_OOP_LOG_INFO_T},           //�������
    {{0xF4060200}, 8,      0x0F,   0x04, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //��ȫ�ӹ̱�ʶ��Ϣ
};
const uint32 oadEsamInfoListLen = sizeof(oadEsamInfoList)/sizeof(oadEsamInfoList[0]);

/* ���������OAD�б� OIA1 = 0x0F OIA2 = 0x02 */
const OAD_TAB_T oadIoInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xF2000200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SERIALPORT_T),         E_OOP_SERIALPORT_T},         //RS232�б����
    {{0xF2000300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //RS232�豸��������
    {{0xF2010200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RS485_T),              E_OOP_RS485_T},              //RS485�б����
    {{0xF2010300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //RS485�豸��������
    {{0xF2020200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_IR_T),                 E_OOP_IR_T},                 //�����б����
    {{0xF2020300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�����豸��������
    {{0xF2030200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_STATE_T),     E_OOP_SWITCHIN_STATE_T},     //�ź����б����
    {{0xF2030300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�ź����豸��������
    {{0xF2030400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T),      E_OOP_SWITCHIN_FLAG_T},      //�ź�����������
    {{0xF2030600}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOP_STATUS_SIGNAL_T),      E_OOP_SWITCHIN_FLAG_T},      //�źŽڵ�
    {{0xF2031800}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHIN_FLAG_T),      E_OOP_SWITCHIN_FLAG_T},      //�ź�����������
    {{0xF2040200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_T),             E_OOP_ANALOG_T},             //ֱ��ģ����
    {{0xF2040300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //ֱ��ģ�����豸��������
    {{0xF2040400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ANALOG_CONFIG_T),      E_OOP_ANALOG_CONFIG_T},      //ֱ��ģ��������
    {{0xF2050200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_SWITCHOUT_T),          E_OOP_SWITCHOUT_T},          //�̵���
    {{0xF2050300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�̵����豸��������
    {{0xF2060200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGOUT_T),         E_OOP_WARNINGOUT_T},         //�澯���
    {{0xF2060300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�澯�豸��������
    {{0xF2060400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_WARNINGTIME_T),        E_OOP_WARNINGTIME_T},        //����澯ʱ��
    {{0xF2070200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_MUTIFUNCTERMINAL_T),   E_OOP_MUTIFUNCTERMINAL_T},   //�๦�ܶ���
    {{0xF2070300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�๦�ܶ����豸��������
    {{0xF2080200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_AC_T),                 E_OOP_AC_T},                 //���ɽӿ�
    {{0xF2080300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�����豸��������   
    {{0xF2090200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_ROUTEMODULE_T),        E_OOP_ROUTEMODULE_T},        //�ز�/΢�������߽ӿڣ�����ͨ��ģ�鵥Ԫ����
    {{0xF2090300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //�ز�/΢�������߽ӿ��豸��������
    {{0xF2090500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_CHILDNODE_T),          E_OOP_CHILDNODE_T},          //�ز�/΢�������߽ӿڣ��ӽڵ�����б�
    {{0xF2090600}, 22,     0x0F,   0x02, 0,      DT_TI,                    sizeof(OOP_TI_T),                 E_OOP_TI_T},                 //�ز�/΢�������߽ӿڣ��ӽڵ�����б��������
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
    {{0xF20A0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_PULSE_T),              E_OOP_PULSE_T},              //��������˿�����
    {{0xF20A0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //���������豸��������
    {{0xF20B0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BLUETOOTHMODS_T),      E_ARRAY},                  //����ģ��,Ŀǰû�ж�Ӧ�ṹ��
    {{0xF20B0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //����ģ���豸��������
    {{0xF20B0400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BLUETOOTHSLAVES_T),    E_ARRAY},                    //���豸�б�
    {{0xF20B0500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BTCONNECTINFO_T),      E_ARRAY},                    //������Ϣ
    {{0xF20B0600}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_BTWORKPARAMS_T),       E_ARRAY},                    //��������
    {{0xF20B0B00}, 22,     0x0F,   0x02, 0,      DT_OCTET_STRING,          sizeof(OOP_OCTETVAR1K_T),         E_OOP_OCTETVAR1K_T},         //SN
    {{0xF20C0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_T),              E_OOP_RADIO_T},              //230����ר��ģ��
    {{0xF20C0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //230����ר��ģ���豸��������
    {{0xF20C0400}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_CHS_T),          E_OOP_RADIO_CHS_T},          //Ƶ������
    {{0xF20C0500}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_RADIO_SIGS_T),         E_OOP_RADIO_SIGS_T},         //��Ч�ź�ǿ��
    {{0xF20D0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 300,                              E_NULL},                     //ģ��
    {{0xF20E0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_USB_T),                E_OOP_USB_T},                //USB�豸
    {{0xF20E0300}, 22,     0x0F,   0x02, 0,      DT_UNSIGNED,              sizeof(uint8),                    E_UNSIGNED},                 //USB�豸��������
    {{0xF20F0200}, 22,     0x0F,   0x02, 0,      DT_ARRAY,                 sizeof(OOP_REG_APP_INFOS_T),      E_NULL},                     //�������
};
const uint32 oadIoInfoListLen = sizeof(oadIoInfoList)/sizeof(oadIoInfoList[0]);

/* ��ʾ��OAD�б� OIA1 = 0x0F OIA2 = 0x03 */
const OAD_TAB_T oadGuiList[] = 
{
    {{0xf3000300},  17,  0x0F,   0x03, 0,DT_LONG_UNSIGNED,        sizeof(uint16),           E_LONG_UNSIGNED },   // �Զ�����ÿ����ʾʱ��
    {{0xf3000400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(uint16),           E_OOP_OBJ_SHOW_PARAM_T },   // ������ʾ����
    {{0xf3010200},  17,  0x0F,   0x03,0,DT_ARRAY,                   sizeof(OOP_OBJ_SHOW_T),   E_OOP_OBJ_SHOW_T },   // ��ʾ����
    {{0xf3010300},  17,  0x0F,   0x03,0,DT_LONG_UNSIGNED,            sizeof(uint16),            E_LONG_UNSIGNED },   // ��������ÿ����ʾʱ��
    {{0xf3010400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(OOP_OBJ_SHOW_PARAM_T),   E_OOP_OBJ_SHOW_PARAM_T },   // ѭ����ʾ����
};
const uint32 oadGuiListLen = sizeof(oadGuiList)/sizeof(oadGuiList[0]);


/* �Զ���OAD�б� OIA1 = 0x0F OIA2 = 0x02 */
const OAD_TAB_T oadSelfInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0xFFF00400}, 8,      0x0F,   0x0F, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLESTR4_T),        E_OOP_VISIBLESTR4_T},        //�����Զ������
    {{0xFFF00600}, 8,      0x0F,   0x0F, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //�����Զ������
    {{0xFFF20400}, 8,      0x0F,   0x0F, 0,      DT_ARRAY,                 sizeof(OOP_INNER_APP_ID_LIST_T),  E_NULL},                     //�����Զ������
};
const uint32 oadSelfInfoListLen = sizeof(oadSelfInfoList)/sizeof(oadSelfInfoList[0]);

//
//OMD�б�
//

/* ������OAD�б� OIA1 = 0x02 */
const OMD_INFO_T omdVariateInfoList[] = 
{
    {{0x24010300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24010400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24011C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)
    
    {{0x24020300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24020400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24021C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24030300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24030400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24031C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24040300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24040400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24041C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24050300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24050400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24051C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24060300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24060400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24061C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24070300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24070400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24071C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

    {{0x24080300}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_T),          E_OOP_PULSECFG_T},            //����������뵥Ԫ
    {{0x24080400}, 12,      0x02,   0x04, 0,      DT_OAD,                  sizeof(OOP_OAD_U),               E_OAD},                       //ɾ���������뵥Ԫ
    {{0x24081C00}, 12,      0x02,   0x04, 0,      DT_STRUCTURE,            sizeof(OOP_PULSECFG_B_T),        E_OOP_PULSECFG_T},            //����������뵥Ԫ(��)

};

const uint32 omdVariateInfoListLen = sizeof(omdVariateInfoList)/sizeof(omdVariateInfoList[0]);


/* �α�����OMD�б� OIA1 = 0x04 */
const OMD_INFO_T omdParaInfoList[] =
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x40007F00}, 8,      0x04,   0x00, 0,      DT_DATETIME_S,            sizeof(OOP_DATETIME_S_T),         E_DATETIME_S},               //��ʱ���ʽ ����-ʱ��
    {{0x40067F00}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},                     //���� ʱ��Դ
    {{0x40068000}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},                     //���� ʱ��Դ
	{{0x40388000}, 8,      0x04,   0x00, 0,      DT_NULL,                  0,                                E_NULL},
	{{0x43000300}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //���ݳ�ʼ��
    {{0x43000100}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //�ն˸�λ
    {{0x43000400}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //�ָ���������
    {{0x43000500}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //�¼���ʼ��
    {{0x43000600}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //������ʼ��
    {{0x43000700}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //������Ȩ����
    {{0x43007F00}, 19,     0x04,   0x03, 0,      DT_NULL,                  0,                                E_NULL},                     //�޸�Һ����������
    {{0x45000100}, 25,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //�ز�
    {{0x45010100}, 25,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //�ز�
    {{0x45100100}, 26,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //�ز�
    {{0x45110100}, 26,     0x04,   0x05, 0,      DT_NULL,                  0,                                E_NULL},                     //�ز�
};
const uint32 omdParaInfoListLen = sizeof(omdParaInfoList)/sizeof(omdParaInfoList[0]);

/* �ɼ���OMD�б� OIA1 = 0x06 */
const OMD_INFO_T omdCollectInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x60007F00}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_OOP_METER_T},              //�ɼ��������õ�Ԫ
    {{0x60007F01}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�������
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             10,                              E_BASIC_OBJECT},             //������Ϣ
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_TSA,                   0,                               E_TSA},                      //������Ϣ - ͨ�ŵ�ַ
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ������
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ��Լ����
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //������Ϣ - �˿�
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_PWD_COMM},                 //������Ϣ - ͨ������
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //������Ϣ - ���ʸ���
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_UNSIGNED,              sizeof(uint8),                   E_UNSIGNED},                 //������Ϣ - �û�����
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_ENUM,                  sizeof(uint8),                   E_ENUM},                     //������Ϣ - ���߷�ʽ
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //������Ϣ - ��ȵ�ѹ
    {{0x60007F02}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //������Ϣ - ��ȵ���
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             4,                               E_EXTENDED_OBJECT},          //��չ��Ϣ
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_TSA,                   0,                               E_TSA},                      //��չ��Ϣ - �ɼ�����ַ
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_OCTET_STRING,          0,                               E_ASSETNO},                  //��չ��Ϣ - �ʲ���
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��չ��Ϣ - ��ѹ���������
    {{0x60007F03}, 8,      0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //��չ��Ϣ - �������������
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_ANNEX_OBJECT_LIST},        //������Ϣ   
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_ANNEX_OBJECT},             //������Ϣ   
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_OAD,                   sizeof(OOP_OAD_U),               E_OAD},                      //������Ϣ - ������������
    {{0x60007F04}, 8,      0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //������Ϣ - ����ֵ
    {{0x60008000}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_METER_LIST},           //�·����������Ԫ  
    {{0x60008100}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_BASIC_OBJECT},             //���µ�����Ԫ 
    {{0x60008200}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             3,                               E_EXTENDED_OBJECT},          //���µ�����Ԫ      
    {{0x60008300}, 11,     0x06,   0x00, 0,      DT_UNSIGNED,              1,                               E_UNSIGNED},                 //ɾ������  
    {{0x60008400}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             11,                              E_EXTENDED_OBJECT},          //ɾ������
    {{0x60008500}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             2,                               E_METER_M133_T},             //ɾ������
    {{0x60008600}, 11,     0x06,   0x00, 0,      DT_UNKNOW,                0,                               E_DATA},                     //��յ���                                
    {{0x60008700}, 11,     0x06,   0x00, 0,      DT_STRUCTURE,             0,                               E_DATA},                     //��յ���                                
    {{0x60020200}, 11,     0x06,   0x00, 0,      DT_ARRAY,                 0,                               E_OOP_SEARCHMET_RES_LIST},   //�����ѱ���
    {{0x60030200}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             7,                               E_OOP_SEARCHMET_RES_T},
    {{0x60030201}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030202}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030203}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030204}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030205}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030206}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},
    {{0x60030207}, 8,      0x06,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_SEARCHMET_RES_T),     E_OOP_SEARCHMET_RES_T},

    {{0x60027F00}, 11,     0x06,   0x00, 0,      DT_LONG_UNSIGNED,         sizeof(uint16),                  E_LONG_UNSIGNED},            //�����ѱ�
    {{0x60028000}, 11,     0x06,   0x00, 0,      DT_NULL,                  sizeof(uint16),                  E_NULL},                     //����ѱ���
    {{0x60028100}, 11,     0x06,   0x00, 0,      DT_NULL,                  sizeof(uint16),                  E_NULL},                     //��տ�̨���ѱ���

    {{0x60127F00}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_TASK_LIST},            //���һ���������ñ�
    {{0x60128000}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ɾ��һ�����õ�Ԫ
    {{0x60128100}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //����������ñ�
    {{0x60128200}, 10,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //�����������ñ�
    {{0x60147F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_NORMAL_T},        //���һ��ɼ�����
    {{0x60148000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ��һ��ɼ�����
    {{0x60148100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //��ղɼ�����
    {{0x60148200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //���÷����ļ�¼��ѡ��
    {{0x60160200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_EVENT_T),        E_OOP_PLAN_EVENT_T},         //�¼��ɼ�������
    {{0x60167F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_EVENT_T},         //�¼��ɼ�������
    {{0x60168000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ���¼��ɼ�����
    {{0x60168100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ���¼��ɼ�����
    {{0x60168200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //�����¼��ɼ�����
   
    {{0x60180200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_TRANS_T),        E_OOP_PLAN_TRANS_T},         //͸��������
    {{0x60187F00}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},
    {{0x60188000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL}, 
    {{0x60188300}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL}, 
    {{0x601A0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_TRANSRES_T),          E_OOP_TRANSRES_T},           //͸�����������
    {{0x601C0200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_PLAN_REPORT_T),       E_OOP_PLAN_REPORT_T},        //�ϱ�������
    {{0x601C7F00}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_OOP_PLAN_REPORT_T},        //�����ϱ�����
    {{0x601C8000}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 0,                               E_NULL},                     //ɾ���ϱ�����
    {{0x601C8100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //����ϱ�����
    {{0x601E0200}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //�ɼ������
    {{0x601E7F00}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //��ӻ����һ��ת���ܷ�����
    {{0x601E8000}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ��һ��ת���ܷ���
    {{0x601E8100}, 11,     0x06,   0x00, 1,      DT_NULL,                  0,                               E_NULL},                     //���ת���ܷ�����    
    {{0x60320200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_RLUESTATE_T),     E_OOP_ACQ_RLUESTATE_T},      //�ɼ�״̬��
    {{0x60340200}, 11,     0x06,   0x00, 1,      DT_ARRAY,                 sizeof(OOP_ACQ_MONITOR_T),       E_OOP_ACQ_MONITOR_T},        //�ɼ������ؼ�

    {{0x6e057f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //���һ��Ⱥ�鵵��
    {{0x6e058000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��Ⱥ���ַ�;����ַɾ������
    {{0x6e058200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //���Ⱥ�鵵����
    {{0x6e058300}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ������Ⱥ�鵵���д��ڸõ�ַ���嵥
    {{0x6e087f00},  8,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��·����

    {{0x6e117f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��ӻ����һ��͸�����񷽰���Ԫ
    {{0x6e118000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //�����񷽰���ɾ��һ��͸�����񷽰���Ԫ�������������ü������ݼ�¼
    {{0x6e118100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //���͸�����񷽰����ñ������������ü������ݼ�¼��
    {{0x6e118200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //Update

    {{0x6e137f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��Ӹ���һ����һ��͸����������
    {{0x6e138000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��͸�����񷽰��ź͵�ַɾ��һ��͸����������
    {{0x6e138100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��͸�����񷽰���ɾ��һ��͸����������
    {{0x6e138200}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //���͸����������
    {{0x6e138300}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //Delete

    {{0x6e177f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //��ӻ����һ�������ϱ�������Ԫ
    {{0x6e178000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //ɾ��һ���ϱ�������Ԫ
    {{0x6e178100}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //����ϱ�������
    {{0x6e178400}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //����һ��ָ����δ�ɹ��ϱ�

    {{0x6e317f00}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //����¼���¼
    {{0x6e318000}, 11,     0x06,   0x0e, 1,      DT_NULL,                  0,                               E_NULL},                     //����һ��δ�ɹ��ϱ����¼���¼
};  
const uint32 omdCollectInfoListLen = sizeof(omdCollectInfoList)/sizeof(omdCollectInfoList[0]);

/* �α�����������OMD�б� OIA1 = 0x07 */
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


/* ���ƽӿ���OMD�б� OIA1 = 0x08 */
const OMD_INFO_T omdControlInfoList[] = 
{
    /*OAD          class   A1      A2    rsv     dataType                  len*/
    {{0x80007F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //�����澯��������
    {{0x80008000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //���������������
    {{0x80008100}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TELECON_TRIP_T),      E_NULL},                     //ң����բ
    {{0x80008200}, 8,      0x08,   0x00, 0,      DT_ARRAY,                 sizeof(OOP_TELECON_TRIP_T),      E_NULL},                     //ң�غ�բ
    {{0x80017F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //Ͷ�뱣��
    {{0x80018000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x80018100}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},    
    {{0x80027F00}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //�߷Ѹ澯Ͷ��
    {{0x80028000}, 8,      0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //�߷Ѹ澯ȡ��
    {{0x80037F00}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //���һ��������Ϣ
    {{0x80038000}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //ɾ��һ��������Ϣ
    {{0x80047F00}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //�����Ҫ������Ϣ
    {{0x80048000}, 11,     0x08,   0x00, 0,      DT_NULL,                  0,                               E_NULL},                     //ɾ����Ҫ������Ϣ
    {{0x81030300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81030400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81030500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81030600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //ʱ�ο�Ͷ��
    {{0x81030700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //ʱ�ο�ȡ��
    {{0x81037F00}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����Զ�����
    {{0x81040300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81040400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81040500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81040600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //���ݿ�Ͷ��
    {{0x81040700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //���ݿ�ȡ��
    {{0x81050300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81050400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81050500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81050600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //Ӫҵ��Ͷ��
    {{0x81050700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //Ӫҵ��ȡ��
    {{0x81060300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81060400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81060500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81060600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�¸���Ͷ��
    {{0x81060700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�¸���ȡ��
    {{0x81067f00}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�¸���Ͷ��(����)
    {{0x81070300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81070400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81070500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81070600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�����Ͷ��
    {{0x81070700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�����ȡ��
    {{0x81080300}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�������
    {{0x81080400}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //����ɾ��
    {{0x81080500}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //��������
    {{0x81080600}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�µ��Ͷ��
    {{0x81080700}, 13,     0x08,   0x01, 0,      DT_NULL,                  0,                               E_NULL},                     //�µ��ȡ��
};
const uint32 omdControlInfoListLen = sizeof(omdControlInfoList)/sizeof(omdControlInfoList[0]);

/* �ļ�����ӿ���OMD�б� OIA1 = 0x0F OIA2 = 0x00 */
const OMD_INFO_T omdFileInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF0007F00}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010400}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010500}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
    {{0xF0010700}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},                                         //��������
    {{0xF0010800}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},        //д�ļ���������
    {{0xF0010900}, 18,	   0x0F,   0x00, 0, 	 DT_STRUCTURE,			   0,			  E_NULL},		  //���ļ���������
    {{0xF0020700}, 18,     0x0F,   0x00, 0,      DT_STRUCTURE,             0,             E_NULL},
};
const uint32 omdFileInfoListLen = sizeof(omdFileInfoList)/sizeof(omdFileInfoList[0]);

/* ESAM�ӿ���OMD�б� OIA1 = 0x0F OIA2 = 0x01 */
const OMD_INFO_T omdEsamInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF1010100}, 8,     0x0F,   0x01, 0,      DT_INTEGER,               sizeof(uint8),                     E_INTEGER},                   //��λʱ���������3
    {{0xF1017F00}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},              //������ʽ��ȫģʽ������������
    {{0xF1018000}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},         //ɾ����ʽ��ȫģʽ�����������ʶ��
    {{0xF1018100}, 8,     0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_SCURITYMODE_T),         E_OOP_SCURITYMODE_T},         //��ʽ��ȫģʽ�����������ʶ��
    
    {{0xF1000700}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_7_T),            E_OOPM_ESAM_7_T},               //��Կ����
    {{0xF1000800}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_8_T),            E_OOPM_ESAM_8_T},               //��Կ����
    {{0xF1000900}, 21,    0x0F,   0x01, 0,      DT_STRUCTURE,             sizeof(OOPM_ESAM_8_T),            E_OOPM_ESAM_8_T},               //��������
    {{0xF4007F00}, 8,     0x0F,   0x04, 0,      DT_VISIBLE_STRING,        sizeof(OOP_VISIBLEVAR256_T),      E_VISIBLE_STRING},   
};
const uint32 omdEsamInfoListLen = sizeof(omdEsamInfoList)/sizeof(omdEsamInfoList[0]);

/* ���������OMD�б� OIA1 = 0x0F OIA2 = 0x02 */
const OMD_INFO_T omdIoInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xF2000300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //RS232�˿���Ȩ
    {{0xF2007F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SERIALPORT_CONFIG_T), E_OOPM_SERIALPORT_CONFIG_T},         //RS232���ò���
    {{0xF2010300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //RS485�˿���Ȩ
    {{0xF2017F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SERIALPORT_CONFIG_T), E_OOPM_SERIALPORT_CONFIG_T},         //RS485���ò���
    {{0xF2020300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //IR�˿���Ȩ
    {{0xF2027F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_IR_CONFIG_T),         E_OOPM_IR_CONFIG_T},                 //�������ò���
    {{0xF2057F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_SWITCHOUT_127_T),     E_OOPM_SWITCHOUT_127_T},             //���������ò���
    {{0xF2077F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_MULTIOUT_CONFIG_T),   E_OOPM_MULTIOUT_CONFIG_T},           //�๦�ܶ����޸Ĺ���ģʽ
    {{0xF2097F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_TRANSCMD_T),          E_OOPM_TRANSCMD_T},                  //͸��ת����������
    {{0xF2098000}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_PORTPARAM_T),         E_OOPM_PORTPARAM_T},                 //���ö˿ڲ�����������
    {{0xF20E0300}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_ADDPORT_T),           E_OOPM_ADDPORT_T},         //USB�˿���Ȩ
    {{0xF20B7F00}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHCFG_T),      E_STRUCTURE},                        //�������ö˿�
    {{0xF20B8000}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHERRCHK_T),   E_STRUCTURE},                        //�������춨
    {{0xF20B8100}, 22,     0x0F,   0x02, 0,      DT_STRUCTURE,             sizeof(OOPM_BLUETOOTHPASS_T),     E_STRUCTURE},                        //���������������
};
const uint32 omdIoInfoListLen = sizeof(omdIoInfoList)/sizeof(omdIoInfoList[0]);

/* ��ʾ��OAD�б� OIA1 = 0x0F OIA2 = 0x03 */
const OMD_INFO_T omdGuiList[] = 
{
    {{0xf3000300},  17,  0x0F,   0x03, 0,DT_LONG_UNSIGNED,        sizeof(uint16),           E_LONG_UNSIGNED },   // �Զ�����ÿ����ʾʱ��
    {{0xf3000400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(uint16),           E_OOP_OBJ_SHOW_PARAM_T },   // ������ʾ����
    {{0xf3010200},  17,  0x0F,   0x03,0,DT_ARRAY,                   sizeof(OOP_OBJ_SHOW_T),   E_OOP_OBJ_SHOW_T },   // ��ʾ����
    {{0xf3010300},  17,  0x0F,   0x03,0,DT_LONG_UNSIGNED,            sizeof(uint16),            E_LONG_UNSIGNED },   // ��������ÿ����ʾʱ��
    {{0xf3010400},  17,  0x0F,   0x03,0,DT_STRUCTURE,               sizeof(OOP_OBJ_SHOW_PARAM_T),   E_OOP_OBJ_SHOW_PARAM_T },   // ѭ����ʾ����
};
const uint32 omdGuiListLen = sizeof(omdGuiList)/sizeof(omdGuiList[0]);

/* �Զ���OMD�б� OIA1 = 0x0F OIA2 = 0x0F */
const OMD_INFO_T omdSelfInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0xFFF06600},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //�����ȫģʽ��������
    {{0xFFF0D300},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //ȫ�������ʼ����������
    {{0xFFF0D400},  8,     0x0F,   0x0F, 0,      DT_NULL,                  0,                                 E_NULL},                  //ȫ�����ݳ�ʼ����������
};
const uint32 omdSelfInfoListLen = sizeof(omdSelfInfoList)/sizeof(omdSelfInfoList[0]);

/* �ܼ�����OMD�б� OIA1 = 0x02 OIA2 = 0x03 */
const OMD_INFO_T omdTgInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x23010100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���1-����ܼ����ñ�
    {{0x23010300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���1-���һ���ܼ������õ�Ԫ
    {{0x23010400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���1-��������ܼ������õ�Ԫ
    {{0x23010500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���1-ɾ��һ���ܼ������õ�Ԫ
    {{0x23020100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���2-����ܼ����ñ�
    {{0x23020300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���2-���һ���ܼ������õ�Ԫ
    {{0x23020400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���2-��������ܼ������õ�Ԫ
    {{0x23020500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���2-ɾ��һ���ܼ������õ�Ԫ
    {{0x23030100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���3-����ܼ����ñ�
    {{0x23030300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���3-���һ���ܼ������õ�Ԫ
    {{0x23030400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���3-��������ܼ������õ�Ԫ
    {{0x23030500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���3-ɾ��һ���ܼ������õ�Ԫ
    {{0x23040100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���4-����ܼ����ñ�
    {{0x23040300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���4-���һ���ܼ������õ�Ԫ
    {{0x23040400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���4-��������ܼ������õ�Ԫ
    {{0x23040500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���4-ɾ��һ���ܼ������õ�Ԫ
    {{0x23050100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���5-����ܼ����ñ�
    {{0x23050300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���5-���һ���ܼ������õ�Ԫ
    {{0x23050400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���5-��������ܼ������õ�Ԫ
    {{0x23050500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���5-ɾ��һ���ܼ������õ�Ԫ
    {{0x23060100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���6-����ܼ����ñ�
    {{0x23060300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���6-���һ���ܼ������õ�Ԫ
    {{0x23060400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���6-��������ܼ������õ�Ԫ
    {{0x23060500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���6-ɾ��һ���ܼ������õ�Ԫ
    {{0x23070100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���7-����ܼ����ñ�
    {{0x23070300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���7-���һ���ܼ������õ�Ԫ
    {{0x23070400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���7-��������ܼ������õ�Ԫ
    {{0x23070500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���7-ɾ��һ���ܼ������õ�Ԫ
    {{0x23080100}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���8-����ܼ����ñ�
    {{0x23080300}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���8-���һ���ܼ������õ�Ԫ
    {{0x23080400}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���8-��������ܼ������õ�Ԫ
    {{0x23080500}, 23,     0x02,   0x03, 0,      DT_STRUCTURE,             sizeof(OOP_TGSETS_T),             E_OOP_TGSETS_T},            //�ܼ���8-ɾ��һ���ܼ������õ�Ԫ
};
const uint32 omdTgInfoListLen = sizeof(omdTgInfoList)/sizeof(omdTgInfoList[0]);

/* �¼���OMD�б� OIA1 = 0x03 */
const OMD_INFO_T omdEvtInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x300F0100}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ѹ�������¼�-��λ
    {{0x300F0400}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ѹ�������¼�-���һ���¼�������������
    {{0x300F0500}, 7,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ѹ�������¼�-ɾ��һ���¼�������������

    {{0x31000100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳�ʼ���¼�-��λ
    {{0x31000400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳�ʼ���¼�-���һ���¼�������������
    {{0x31000500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳�ʼ���¼�-ɾ��һ���¼�������������

    {{0x31010100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˰汾����¼�-��λ
    {{0x31010400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˰汾����¼�-���һ���¼�������������
    {{0x31010500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˰汾����¼�-ɾ��һ���¼�������������

    {{0x31040100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�״̬����λ�¼�-��λ
    {{0x31040400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�״̬����λ�¼�-���һ���¼�������������
    {{0x31040500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�״̬����λ�¼�-ɾ��һ���¼�������������

    {{0x31050100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʱ�ӳ����¼�-��λ
    {{0x31050400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʱ�ӳ����¼�-���һ���¼�������������
    {{0x31050500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʱ�ӳ����¼�-ɾ��һ���¼�������������

    {{0x31060100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�ͣ/�ϵ��¼�-��λ
    {{0x31060400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�ͣ/�ϵ��¼�-���һ���¼�������������
    {{0x31060500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն�ͣ/�ϵ��¼�-ɾ��һ���¼�������������

    {{0x310A0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�豸���ϼ�¼-��λ
    {{0x310A0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�豸���ϼ�¼-���һ���¼�������������
    {{0x310A0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�豸���ϼ�¼-ɾ��һ���¼�������������

    {{0x310B0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʾ���½��¼�-��λ
    {{0x310B0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʾ���½��¼�-���һ���¼�������������
    {{0x310B0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ʾ���½��¼�-ɾ��һ���¼�������������

    {{0x310C0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�����������¼�-��λ
    {{0x310C0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�����������¼�-���һ���¼�������������
    {{0x310C0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�����������¼�-ɾ��һ���¼�������������

    {{0x310D0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�����¼�-��λ
    {{0x310D0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�����¼�-���һ���¼�������������
    {{0x310D0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�����¼�-ɾ��һ���¼�������������

    {{0x310E0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ͣ���¼�-��λ
    {{0x310E0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ͣ���¼�-���һ���¼�������������
    {{0x310E0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ�ͣ���¼�-ɾ��һ���¼�������������

    {{0x310F0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳���ʧ���¼�-��λ
    {{0x310F0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳���ʧ���¼�-���һ���¼�������������
    {{0x310F0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˳���ʧ���¼�-ɾ��һ���¼�������������

    {{0x31100100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ͨ�����������¼�-��λ
    {{0x31100400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ͨ�����������¼�-���һ���¼�������������
    {{0x31100500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //��ͨ�����������¼�-ɾ��һ���¼�������������

    {{0x31110100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //����δ֪���ܱ��¼�-��λ
    {{0x31110400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //����δ֪���ܱ��¼�-���һ���¼�������������
    {{0x31110500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //����δ֪���ܱ��¼�-ɾ��һ���¼�������������

    {{0x31140100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˶�ʱ�¼�-��λ
    {{0x31140400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˶�ʱ�¼�-���һ���¼�������������
    {{0x31140500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˶�ʱ�¼�-ɾ��һ���¼�������������

    {{0x31180100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˱�̼�¼-��λ
    {{0x31180400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˱�̼�¼-���һ���¼�������������
    {{0x31180500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˱�̼�¼-ɾ��һ���¼�������������

    {{0x31190100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˵�����·�쳣�¼�-��λ
    {{0x31190400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˵�����·�쳣�¼�-���һ���¼�������������
    {{0x31190500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //�ն˵�����·�쳣�¼�-ɾ��һ���¼�������������

    {{0x311C0100}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ����ݱ����ؼ�¼-��λ
    {{0x311C0400}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ����ݱ����ؼ�¼-���һ���¼�������������
    {{0x311C0500}, 7,     0x03,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_EVENT_T),              E_OOP_EVENT_T},            //���ܱ����ݱ����ؼ�¼-ɾ��һ���¼�������������
};
const uint32 omdEvtInfoListLen = sizeof(omdEvtInfoList)/sizeof(omdEvtInfoList[0]);

/* ������OMD�б� OIA1 = 0x05 */
const OMD_INFO_T omdFrozenInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x50010100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�붳��-��λ
    {{0x50010400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�붳��-���һ�������������
    {{0x50010500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�붳��-ɾ��һ�������������
    {{0x50010700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�붳��-������Ӷ����������
    {{0x50010800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�붳��-�������������Ա�

    {{0x50020100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //���Ӷ���-��λ
    {{0x50020400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //���Ӷ���-���һ�������������
    {{0x50020500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //���Ӷ���-ɾ��һ�������������
    {{0x50020700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //���Ӷ���-������Ӷ����������
    {{0x50020800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //���Ӷ���-�������������Ա�

    {{0x50030100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //Сʱ����-��λ
    {{0x50030400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //Сʱ����-���һ�������������
    {{0x50030500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //Сʱ����-ɾ��һ�������������
    {{0x50030700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //Сʱ����-������Ӷ����������
    {{0x50030800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //Сʱ����-�������������Ա�

    {{0x50040100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�ն���-��λ
    {{0x50040400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�ն���-���һ�������������
    {{0x50040500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�ն���-ɾ��һ�������������
    {{0x50040700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�ն���-������Ӷ����������
    {{0x50040800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�ն���-�������������Ա�

    {{0x50060100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�¶���-��λ
    {{0x50060400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�¶���-���һ�������������
    {{0x50060500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�¶���-ɾ��һ�������������
    {{0x50060700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�¶���-������Ӷ����������
    {{0x50060800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�¶���-�������������Ա�

    {{0x50070100}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�궳��-��λ
    {{0x50070400}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�궳��-���һ�������������
    {{0x50070500}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�궳��-ɾ��һ�������������
    {{0x50070700}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�궳��-������Ӷ����������
    {{0x50070800}, 9,     0x03,   0x00, 0,      DT_STRUCTURE,             sizeof(OOP_FROZENSETS_T),     E_OOP_FROZENSETS_T},            //�궳��-�������������Ա�
};
const uint32 omdFrozenInfoListLen = sizeof(omdFrozenInfoList)/sizeof(omdFrozenInfoList[0]);

/* ͳ����OMD�б� OIA1 = 0x02 */
const OMD_INFO_T omdStatInfoList[] = 
{
    /*OMD          class   A1      A2    rsv     dataType                  len*/
    {{0x21000300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-���һ��ͳ�ƶ���
    {{0x21000400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21010300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //ʱ����ͳ��-���һ��ͳ�ƶ���
    {{0x21010400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //ʱ����ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21020300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-���һ��ͳ�ƶ���
    {{0x21020400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21030300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-���һ��ͳ�ƶ���
    {{0x21030400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21040300}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-���һ��ͳ�ƶ���
    {{0x21040400}, 14,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_REGIONS_T),      E_OOP_REGIONS_T},           //������ͳ��-ɾ��һ��ͳ�ƶ���

    {{0x21100300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-���һ��ͳ�ƶ���
    {{0x21100400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21110300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //ʱƽ��ͳ��-���һ��ͳ�ƶ���
    {{0x21110400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //ʱƽ��ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21120300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-���һ��ͳ�ƶ���
    {{0x21120400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21130300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-���һ��ͳ�ƶ���
    {{0x21130400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21140300}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-���һ��ͳ�ƶ���
    {{0x21140400}, 15,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //��ƽ��ͳ��-ɾ��һ��ͳ�ƶ���

    {{0x21200300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�ּ�ֵͳ��-���һ��ͳ�ƶ���
    {{0x21200400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�ּ�ֵͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21210300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //ʱ��ֵͳ��-���һ��ͳ�ƶ���
    {{0x21210400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //ʱ��ֵͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21220300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�ռ�ֵͳ��-���һ��ͳ�ƶ���
    {{0x21220400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�ռ�ֵͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21230300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�¼�ֵͳ��-���һ��ͳ�ƶ���
    {{0x21230400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�¼�ֵͳ��-ɾ��һ��ͳ�ƶ���
    {{0x21240300}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�꼫ֵͳ��-���һ��ͳ�ƶ���
    {{0x21240400}, 16,     0x02,   0x01, 0,      DT_STRUCTURE,             sizeof(OOP_COGNATES_T),     E_OOP_COGNATES_T},          //�꼫ֵͳ��-ɾ��һ��ͳ�ƶ���
    
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
    {127,ACCESS},//�㲥Уʱ

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//�㲥Уʱ
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
    {127,ACCESS},//����
    {128,ACCESS},//����

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//����
    {128,ACCESS},//����
};

const marAccessRight_t Class8MAR_F101[] =
{
    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//������ʽ��ȫģʽ����
    {128,ACCESS},//ɾ����ʽ��ȫģʽ����
    {129,ACCESS},//����������ʽ��ȫģʽ����

    {1,NOACCESS},
    {2,NOACCESS},
    {127,ACCESS},//������ʽ��ȫģʽ����
    {128,ACCESS},//ɾ����ʽ��ȫģʽ����
    {129,ACCESS},//����������ʽ��ȫģʽ����
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
    {1,READ},//�߼���
    {2,READ},//���ñ�
    {3,READ},//��¼��
    {4,READ},//��ǰ��¼��
    {5,READ},//����¼��

    {1,READ},//�߼���
    {2,READ},//���ñ�
    {3,READ},//��¼��
    {4,READ},//��ǰ��¼��
    {5,READ},//����¼��
};

const marAccessRight_t Class10MAR[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,NOACCESS},//��ռ�¼��

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,NOACCESS},//��ռ�¼��
};


const marAccessRight_t Class10MAR_6012[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,NOACCESS},//��ռ�¼��
    {127,ACCESS},//��ӻ����һ���������õ�Ԫ��
    {128,ACCESS},//ɾ��һ�����õ�Ԫ��
    {129,ACCESS},//����������ñ�
    {130,ACCESS},//��������״̬:

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,NOACCESS},//��ռ�¼��
    {127,ACCESS},//��ӻ����һ���������õ�Ԫ��
    {128,ACCESS},//ɾ��һ�����õ�Ԫ��
    {129,ACCESS},//����������ñ�
    {130,ACCESS},//��������״̬:
};
//Class11
const aarAccessRight_t Class11AAR[] =
{
    {1,READ},//�߼���
    {2,READWRITE},//����
    {3,READ},//��ǰԪ�ظ���
    {4,READ},//���Ԫ�ظ���

    {1,READ},//�߼���
    {2,READWRITE},//����
    {3,READ},//��ǰԪ�ظ���
    {4,READ},//���Ԫ�ظ���
};

const aarAccessRight_t Class11AAR_6002[] ={
    {1,READ},//�߼���
    {2,READ},//�����ѱ���
    {3,READ},//��ǰԪ�ظ���
    {4,READ},//���Ԫ�ظ���
    {5,READ},//��̨���ѱ���
    {6,READ},//�����ѱ�����¼��
    {7,READ},//��̨���ѱ�����¼��
    {8,READWRITE},//
    {9,READWRITE},//ÿ�������ѱ��������
    {10,READ},//

    {1,READ},//�߼���
    {2,READ},//�����ѱ���
    {3,READ},//��ǰԪ�ظ���
    {4,READ},//���Ԫ�ظ���
    {5,READ},//��̨���ѱ���
    {6,READ},//�����ѱ�����¼��
    {7,READ},//��̨���ѱ�����¼��
    {8,READWRITE},//
    {9,READWRITE},//ÿ�������ѱ��������
    {10,READ},//

};

const marAccessRight_t Class11MAR[] ={
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
};

const marAccessRight_t Class11MAR_4200[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ��·����Ϣ��Ԫ
    {128,ACCESS},//�������·����Ϣ��Ԫ��
    {129,ACCESS},//����·�ɱ��·����Ϣ��Ԫ����
    {134,ACCESS},//���·�ɱ�

    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ��·����Ϣ��Ԫ
    {128,ACCESS},//�������·����Ϣ��Ԫ��
    {129,ACCESS},//����·�ɱ��·����Ϣ��Ԫ����
    {134,ACCESS},//���·�ɱ�
};
const marAccessRight_t Class11MAR_6000[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ���ɼ��������õ�Ԫ������Ϣ�л����������� ���һ���ɼ��������õ�Ԫ������Ϣ�л����������� ���ģ���չ�͸�����Ϣ����Ϊ
    {128,ACCESS},//������Ӳɼ��������õ�Ԫ��
    {129,ACCESS},//�������õ�Ԫ�Ļ�����Ϣ����
    {130,ACCESS},//�������õ�Ԫ����չ��Ϣ�Լ�����������Ϊ �������õ�Ԫ����չ��Ϣ�Լ�����������Ϊ NULLNULLNULLNULL��ʾ������ ��ʾ������ ��
    {131,ACCESS},//ɾ�����õ�Ԫ��ͨ�����
    {132,ACCESS},//ɾ�����õ�Ԫ��ͨ��������Ϣ����
    {133,ACCESS},//ɾ�����õ�Ԫ��ͨ���ŵ�ַ���˿�
    {134,ACCESS},//��ղɼ��������ñ�

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ���ɼ��������õ�Ԫ������Ϣ�л����������� ���һ���ɼ��������õ�Ԫ������Ϣ�л����������� ���ģ���չ�͸�����Ϣ����Ϊ
    {128,ACCESS},//������Ӳɼ��������õ�Ԫ��
    {129,ACCESS},//�������õ�Ԫ�Ļ�����Ϣ����
    {130,ACCESS},//�������õ�Ԫ����չ��Ϣ�Լ�����������Ϊ �������õ�Ԫ����չ��Ϣ�Լ�����������Ϊ NULLNULLNULLNULL��ʾ������ ��ʾ������ ��
    {131,ACCESS},//ɾ�����õ�Ԫ��ͨ�����
    {132,ACCESS},//ɾ�����õ�Ԫ��ͨ��������Ϣ����
    {133,ACCESS},//ɾ�����õ�Ԫ��ͨ���ŵ�ַ���˿�
    {134,ACCESS},//��ղɼ��������ñ�
};
const marAccessRight_t Class11MAR_6002[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//
    {128,ACCESS},//
    {129,ACCESS},//
};

const marAccessRight_t Class11MAR_6014[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ����ͨ�ɼ�������
    {128,ACCESS},//ɾ��һ����ͨ�ɼ�������
    {129,ACCESS},//�����ͨ�ɼ�������
    {130,ACCESS},//���÷����ļ�¼��ѡ��
#if (HENAN_FUN==1||GANSU_FUN==1)
    {200,ACCESS},//���÷����ĵ��ܱ��ϡ�
#endif

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ����ͨ�ɼ�������
    {128,ACCESS},//ɾ��һ����ͨ�ɼ�������
    {129,ACCESS},//�����ͨ�ɼ�������
    {130,ACCESS},//���÷����ļ�¼��ѡ��
#if (HENAN_FUN==1||GANSU_FUN==1)
    {200,ACCESS},//���÷����ĵ��ܱ��ϡ�
#endif
};

const marAccessRight_t Class11MAR_6016[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ���¼��ɼ�������
    {128,ACCESS},//ɾ��һ���¼��ɼ�������
    {129,ACCESS},//����¼��ɼ���������
    {130,ACCESS},//UpdateReportFlag

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ���¼��ɼ�������
    {128,ACCESS},//ɾ��һ���¼��ɼ�������
    {129,ACCESS},//����¼��ɼ���������
    {130,ACCESS},//UpdateReportFlag
};

const marAccessRight_t Class11MAR_6018[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��Ӹ���һ��͸�������������ݣ�
    {128,ACCESS},//��һ�����ܱ�����鱨��
    {129,ACCESS},//ɾ��һ�������������ݣ�
    {130,ACCESS},//ɾ��һ��͸��������
    {131,ACCESS},//���͸����������

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��Ӹ���һ��͸�������������ݣ�
    {128,ACCESS},//��һ�����ܱ�����鱨��
    {129,ACCESS},//ɾ��һ�������������ݣ�
    {130,ACCESS},//ɾ��һ��͸��������
    {131,ACCESS},//���͸����������
};

const marAccessRight_t Class11MAR_601C[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ���ϱ�������
    {128,ACCESS},//ɾ��һ���ϱ�������
    {129,ACCESS},//����ϱ���������

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ���ϱ�������
    {128,ACCESS},//ɾ��һ���ϱ�������
    {129,ACCESS},//����ϱ���������
};

const marAccessRight_t Class11MAR_601E[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ɼ�����
    {128,ACCESS},//ɾ��һ��ɼ�����
    {129,ACCESS},//��ղɼ������

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ɼ�����
    {128,ACCESS},//ɾ��һ��ɼ�����
    {129,ACCESS},//��ղɼ������
};

const marAccessRight_t Class11MAR_6051[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����
    {130,ACCESS},//UpdateReportFlag

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����
    {130,ACCESS},//UpdateReportFlag
};
#if 1
const marAccessRight_t Class11MAR_7000[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����
};
const marAccessRight_t Class11MAR_7010[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����
    {130,ACCESS},//UpdateReportFlag


    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//��ӻ����һ��ʵʱ��زɼ�������
    {128,ACCESS},//ɾ��һ��ʵʱ��زɼ�����
    {129,ACCESS},//���ʵʱ��زɼ�����
    {130,ACCESS},//UpdateReportFlag

};
#endif
const marAccessRight_t Class11MAR_800X[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ��������Ϣ
    {128,ACCESS},//ɾ��������Ϣ

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {127,ACCESS},//���һ��������Ϣ
    {128,ACCESS},//ɾ��������Ϣ
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
    {1,READ},//�߼���
    {2,READWRITE},//���Ʒ�����
    {3,READWRITE},//����Ͷ��״̬
    {4,READ},//�̵������״̬
    {5,READ},//Խ�޸澯״̬

    {1,READ},//�߼���
    {2,READWRITE},//���Ʒ�����
    {3,READWRITE},//����Ͷ��״̬
    {4,READ},//�̵������״̬
    {5,READ},//Խ�޸澯״̬
};
const aarAccessRight_t Class13AAR_8106[] =
{
    {1,READ},//�߼���
    {2,NOACCESS},//���Ʒ�����
    {3,READWRITE},//����Ͷ��״̬
    {4,READ},//�̵������״̬
    {5,READ},//Խ�޸澯״̬

    {1,READ},//�߼���
    {2,NOACCESS},//���Ʒ�����
    {3,READWRITE},//����Ͷ��״̬
    {4,READ},//�̵������״̬
    {5,READ},//Խ�޸澯״̬
};

const marAccessRight_t Class13MAR[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,ACCESS},//��ӿ��Ƶ�Ԫ
    {4,ACCESS},//ɾ�����Ƶ�Ԫ
    {5,ACCESS},//���¿��Ƶ�Ԫ
    {6,ACCESS},//����Ͷ��
    {7,ACCESS},//���ƽ��

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,ACCESS},//��ӿ��Ƶ�Ԫ
    {4,ACCESS},//ɾ�����Ƶ�Ԫ
    {5,ACCESS},//���¿��Ƶ�Ԫ
    {6,ACCESS},//����Ͷ��
    {7,ACCESS},//���ƽ��
};
const marAccessRight_t Class13MAR_8106[] =
{
    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,ACCESS},//��ӿ��Ƶ�Ԫ
    {4,ACCESS},//ɾ�����Ƶ�Ԫ
    {5,ACCESS},//���¿��Ƶ�Ԫ
    {6,ACCESS},//����Ͷ��
    {7,ACCESS},//���ƽ��
    {127,ACCESS},//�¸���Ͷ��

    {1,NOACCESS},//��λ
    {2,NOACCESS},//ִ��
    {3,ACCESS},//��ӿ��Ƶ�Ԫ
    {4,ACCESS},//ɾ�����Ƶ�Ԫ
    {5,ACCESS},//���¿��Ƶ�Ԫ
    {6,ACCESS},//����Ͷ��
    {7,ACCESS},//���ƽ��
    {127,ACCESS},//�¸���Ͷ��
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
    {13,READ},  //����������20��������Ҫ������13��Ҫ�޸Ĳ�ѯ�����ģ����
    {14,READ},//14-19 ���Ա���δ��
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},
    {20,READ},//�����ģ
    {21,READ},//21-30���Ա���
    {22,READ},
    {23,READ},
    {24,READ},
    {25,READ},
    {26,READ},
    {27,READ},
    {28,READ},
    {29,READ},
    {30,READ},
    {31,READ},//��������ͨ��ģ������


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
    {13,READ},  //����������20��������Ҫ������13��Ҫ�޸Ĳ�ѯ�����ģ����
    {14,READ},  //14-19 ���Ա���δ��
    {15,READ},
    {16,READ},
    {17,READ},
    {18,READ},
    {19,READ},
    {20,READ},//�����ģ
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
    {31,READ},//��������ͨ��ģ������

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
/*001*/{0x0000, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// ����й�����
/*002*/{0x0001, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A������й�����
/*003*/{0x0002, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B������й�����
/*004*/{0x0003, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C������й�����
/*005*/{0x0010, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// �����й�����
/*006*/{0x0011, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A�������й�����
/*007*/{0x0012, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B�������й�����
/*008*/{0x0013, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C�������й�����
/*009*/{0x0020, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// �����й�����
/*010*/{0x0021, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A�෴���й�����
/*011*/{0x0022, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B�෴���й�����
/*012*/{0x0023, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C�෴���й�����
/*013*/{0x0030, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// ����޹�1����
/*014*/{0x0031, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A������޹�1����
/*015*/{0x0032, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B������޹�1����
/*016*/{0x0033, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C������޹�1����
/*017*/{0x0040, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// ����޹�2����
/*018*/{0x0041, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A������޹�2����
/*019*/{0x0042, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B������޹�2����
/*020*/{0x0043, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C������޹�2����
/*021*/{0x0050, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// I�����޹�����
/*022*/{0x0051, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A��I�����޹�����
/*023*/{0x0052, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B��I�����޹�����
/*024*/{0x0053, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C��I�����޹�����
/*025*/{0x0060, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// II�����޹�����
/*026*/{0x0061, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A��II�����޹�����
/*027*/{0x0062, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B��II�����޹�����
/*028*/{0x0063, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C��II�����޹�����
/*029*/{0x0070, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// III�����޹�����
/*030*/{0x0071, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A��III�����޹�����
/*031*/{0x0072, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B��III�����޹�����
/*032*/{0x0073, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C��III�����޹�����
/*033*/{0x0080, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// IV�����޹�����
/*034*/{0x0081, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A��IV�����޹�����
/*035*/{0x0082, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B��IV�����޹�����
/*036*/{0x0083, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C��IV�����޹�����
/*037*/{0x0090, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// �������ڵ���
/*038*/{0x0091, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A���������ڵ���
/*039*/{0x0092, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B���������ڵ���
/*040*/{0x0093, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C���������ڵ���
/*041*/{0x00A0, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},// �������ڵ���
/*042*/{0x00A1, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A�෴�����ڵ���
/*043*/{0x00A2, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B�෴�����ڵ���
/*044*/{0x00A3, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C�෴�����ڵ���
/*045*/{0x0110, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//��������й��ܵ���
/*046*/{0x0111, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A����������й�����
/*047*/{0x0112, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B����������й�����
/*048*/{0x0113, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C����������й�����
/*049*/{0x0120, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//��������й��ܵ���
/*050*/{0x0121, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A�෴������й�����
/*051*/{0x0122, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B�෴������й�����
/*052*/{0x0123, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C�෴������й�����
/*053*/{0x0210, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//����г���й��ܵ���
/*054*/{0x0211, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A������г���й�����
/*055*/{0x0212, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B������г���й�����
/*056*/{0x0213, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C������г���й�����
/*057*/{0x0220, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//����г���й��ܵ���
/*058*/{0x0221, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//A�෴��г���й�����
/*059*/{0x0222, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//B�෴��г���й�����
/*060*/{0x0223, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//C�෴��г���й�����
/*061*/{0x0300, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//ͭ���й��ܵ��ܲ�����
/*062*/{0x0400, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//�����й��ܵ��ܲ�����
/*063*/{0x0500, 1, CLASS_AAR_MAR(Class1AAR,      Class1MAR      )},//�����ܵ���
/*064*/{0x1000, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//����й��������
/*065*/{0x1001, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A������й��������
/*066*/{0x1002, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B������й��������
/*067*/{0x1003, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C������й��������
/*068*/{0x1010, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//�����й��������
/*069*/{0x1011, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A�������й��������
/*070*/{0x1012, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B�������й��������
/*071*/{0x1013, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C�������й��������
/*072*/{0x1020, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//�����й��������
/*073*/{0x1021, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A�෴���й��������
/*074*/{0x1022, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B�෴���й��������
/*075*/{0x1023, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C�෴���й��������
/*076*/{0x1030, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//����޹�1�������
/*077*/{0x1031, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A������޹�1�������
/*078*/{0x1032, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B������޹�1�������
/*079*/{0x1033, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C������޹�1�������
/*080*/{0x1040, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//����޹�2�������
/*081*/{0x1041, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A������޹�2�������
/*082*/{0x1042, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B������޹�2�������
/*083*/{0x1043, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C������޹�2�������
/*084*/{0x1050, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//I�����޹��������
/*085*/{0x1051, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A��I�����޹��������
/*086*/{0x1052, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B��I�����޹��������
/*087*/{0x1053, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C��I�����޹��������
/*088*/{0x1060, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//II�����޹��������
/*089*/{0x1061, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A��II�����޹��������
/*090*/{0x1062, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B��II�����޹��������
/*091*/{0x1063, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C��II�����޹��������
/*092*/{0x1070, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//III�����޹��������
/*093*/{0x1071, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A��III�����޹��������
/*094*/{0x1072, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B��III�����޹��������
/*095*/{0x1073, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C��III�����޹��������
/*096*/{0x1080, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//IV�����޹��������
/*097*/{0x1081, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A��IV�����޹��������
/*098*/{0x1082, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B��IV�����޹��������
/*099*/{0x1083, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C��IV�����޹��������
/*100*/{0x1090, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//���������������
/*101*/{0x1091, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A�����������������
/*102*/{0x1092, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B�����������������
/*103*/{0x1093, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C�����������������
/*104*/{0x10A0, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//���������������
/*105*/{0x10A1, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//A�෴�������������
/*106*/{0x10A2, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//B�෴�������������
/*107*/{0x10A3, 2, CLASS_AAR_MAR(Class2AAR,      Class2MAR      )},//C�෴�������������
/*108*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//������ѹ
/*109*/{0x2001, 3, CLASS_AAR_MAR(Class3AAR_2001, Class3MAR      )},//��������
/*110*/{0x2002, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//������ѹ���
/*111*/{0x2003, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//������ѹ�������
/*112*/{0x2004, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//�й�����
/*113*/{0x2005, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//�޹�����
/*114*/{0x2006, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//���ڹ���
/*115*/{0x2007, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//һ����ƽ���й�����
/*116*/{0x2008, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//һ����ƽ���޹�����
/*117*/{0x2009, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//һ����ƽ�����ڹ���
/*118*/{0x200A, 4, CLASS_AAR_MAR(Class4AAR,      Class4MAR      )},//��������
/*119*/{0x200B, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//��ѹ����ʧ���
/*120*/{0x200C, 3, CLASS_AAR_MAR(Class3AAR,      Class3MAR      )},//��������ʧ���
/*121*/{0x200D, 5, CLASS_AAR_MAR(Class5AAR,      Class5MAR      )},//��ѹг������
/*122*/{0x200E, 5, CLASS_AAR_MAR(Class5AAR,      Class5MAR      )},//����г������
/*123*/{0x200F, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//����Ƶ��
/*124*/{0x2010, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//�����¶�
/*125*/{0x2011, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//ʱ�ӵ��ѹ
/*126*/{0x2012, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//ͣ�糭���ѹ
/*127*/{0x2013, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//ʱ�ӵ�ع���ѹ
/*128*/{0x2014, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//���ܱ� ����״̬��
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
/*184*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ѹ�ϸ��ʲ���
#if BROADBAND_698_EXT_FUNC == 1
/*185*/{0x3030,7,  CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//ͨ��ģ�����¼�
#endif
#endif
/*001*/{0x201E, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�¼�����ʱ��
/*002*/{0x2020, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�¼�����ʱ��
/*003*/{0x2021, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//���ݶ���ʱ��
/*004*/{0x2022, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�¼���¼���
/*005*/{0x2023, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�����¼���
/*006*/{0x2024, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�¼�����Դ
/*007*/{0x2200, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//ͨ������
/*008*/{0x2203, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//����ʱ��
/*009*/{0x2204, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//��λ����
#if HUNAN_FUN == 1
/*009*/{0x2205, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//���ó����
#endif
/*010*/{0x2301,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���1
/*011*/{0x2302,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���2
/*012*/{0x2303,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���3
/*013*/{0x2304,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���4
/*014*/{0x2305,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���5
/*015*/{0x2306,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���6
/*016*/{0x2307,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���7
/*017*/{0x2308,23, CLASS_AAR_MAR(Class23AAR,     Class23MAR     )},//�ܼ���8
/*018*/{0x2401,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������1
/*019*/{0x2402,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������2
/*020*/{0x2403,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������3
/*021*/{0x2404,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������4
/*022*/{0x2405,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������5
/*023*/{0x2406,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������6
/*024*/{0x2407,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������7
/*025*/{0x2408,12, CLASS_AAR_MAR(Class12AAR,     Class12MAR     )},//���������8
/*026*/{0x3100, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˳�ʼ���¼�
/*027*/{0x3101, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˰汾����¼�
/*028*/{0x3104, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն�״̬����λ�¼�
/*029*/{0x3105, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//���ܱ�ʱ�ӳ����¼�
/*030*/{0x3106, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն�ͣ/�ϵ��¼�
/*031*/{0x3109, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//��Ϣ��֤�����¼
/*032*/{0x310A, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˹��ϼ�¼
/*033*/{0x310B, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//���ܱ�ʾ���½�
/*034*/{0x310C, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//����������
/*035*/{0x310D, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//���ܱ����
/*036*/{0x310E, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//���ܱ�ͣ��
/*037*/{0x310F, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//485����ʧ��
/*038*/{0x3110, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն�����վͨ������������
/*039*/{0x3111, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//����δ֪����¼���¼
/*040*/{0x3113, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//����ʧ���¼���¼
/*041*/{0x3114, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//��ʱ�¼���¼
/*042*/{0x3115, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//ң����բ��¼
/*043*/{0x3118, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˱�̼�¼
/*044*/{0x3119, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˵�����·�쳣�¼�
/*044*/{0x311B, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�ն˶Ե��Уʱ��¼
/*045*/{0x311C, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//���ܱ����ݱ����ؼ�¼
/*046*/{0x3200, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//������բ��¼
/*047*/{0x3201, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�����բ��¼
/*048*/{0x3202, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//����������ü�¼
/*049*/{0x3203, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//��ظ澯�¼���¼
/*050*/{0x3300, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//ͨ���ϱ�״̬
/*051*/{0x4000, 8, CLASS_AAR_MAR(Class8AAR_4000, Class8MAR_4000 )},//��ǰʱ��
/*052*/{0x4001, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//ͨ�ŵ�ַ
/*053*/{0x4003, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�ͻ����
/*054*/{0x4004, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�豸����λ��
/*055*/{0x4005, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ַ����Ҫ����ʵ��
/*056*/{0x4006, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR_4006 )},//ʱ��ԭ
/*057*/{0x4007, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//LCD ����
/*136*/{0x400A, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���׷�ʱ�����л�ʱ��
/*058*/{0x400C, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//ʱ��ʱ����
/*146*/{0x4014, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//��ǰ��ʱ����
/*147*/{0x4015, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//������ʱ����
/*059*/{0x4016, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//��ǰ����ʱ�α�
/*060*/{0x4018, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//��ǰ�׷��ʵ��
/*151*/{0x4019, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�����׷��ʵ��
/*061*/{0x4024, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�޳�
/*062*/{0x4100, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�����������
/*063*/{0x4101, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ʻ���ʱ��
/*064*/{0x4102, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//У��������
/*065*/{0x4103, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�ʲ��������
/*066*/{0x4104, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ѹ
/*067*/{0x4105, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//����� /
/*068*/{0x4106, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//������
/*069*/{0x4107, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�й�׼ȷ�ȵȼ�
/*070*/{0x4108, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�޹�׼ȷ�ȵȼ�
/*071*/{0x4109, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ܱ��й�����
/*072*/{0x410A, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ܱ��޹�����
/*073*/{0x410B, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ܱ��ͺ�
/*074*/{0x4200,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_4200)},//·����Ϣ���ñ�
/*075*/{0x4202, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//��������
/*076*/{0x4204, 8, CLASS_AAR_MAR(Class8AAR_4204, Class8MAR      )},//�ն˹㲥Уʱʱ�����
/*077*/{0x4300,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//�������豸��Ϣ
/*078*/{0x4301,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//·��ģ���豸��Ϣ
/*079*/{0x4302,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//GPRSģ���豸��Ϣ
/*080*/{0x4303,19, CLASS_AAR_MAR(Class19AAR,     Class19MAR     )},//ר���豸��Ϣ
/*081*/{0x4400,20, CLASS_AAR_MAR(Class20AAR,     Class20MAR     )},//Ӧ�����Ӷ���
/*082*/{0x4401, 8, CLASS_AAR_MAR(Class8AAR_4401, Class8MAR      )},//��������
/*083*/{0x4500,25, CLASS_AAR_MAR(Class25AAR,     Class25MAR     )},//����ͨ��ģ��1
/*084*/{0x4510,26, CLASS_AAR_MAR(Class26AAR,     Class26MAR     )},//��̫��ͨ��ģ��1
/*085*/{0x5002, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//���Ӷ���
/*085*/{0x5003, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//Сʱ����
/*086*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//�ն���
/*086*/{0x5005, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//�����ն���
/*087*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,      Class9MAR      )},//�¶���
/*088*/{0x6012,10, CLASS_AAR_MAR(Class10AAR,     Class10MAR_6012)},//�������ñ�
/*089*/{0x6000,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6000)},//�ɼ��������ñ�
/*090*/{0x6001, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�ɼ��������õ�Ԫ
/*091*/{0x6002,11, CLASS_AAR_MAR(Class11AAR_6002,Class11MAR_6002)},//�ѱ�
/*092*/{0x6003,8,  CLASS_AAR_MAR(Class11AAR,     Class11MAR_6002)},//һ���ѱ���
/*093*/{0x6014,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6014)},//��ͨ�ɼ�
/*094*/{0x6015, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//��ͨ�ɼ�������Ԫ
/*095*/{0x6016,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6016)},//�¼��ɼ�
/*096*/{0x6017, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�¼��ɼ�������Ԫ
/*097*/{0x6018,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6018)},//͸��������
/*098*/{0x6019, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//͸��������Ԫ
/*099*/{0x601A,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//͸�����������
/*100*/{0x601B, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//͸�����������Ԫ
/*101*/{0x601C,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601C)},//�ϱ�������
/*102*/{0x601D, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�ϱ�������Ԫ
/*103*/{0x601E,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601E)},//�ɼ������
/*104*/{0x6032,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//�ɼ�״̬��
/*106*/{0x6033, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�ɼ������ؼ�
/*105*/{0x6034,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR     )},//�ɼ������ؼ�
/*106*/{0x6035, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�ɼ������ؼ�
/*107*/{0x6051,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_6051)},//ʵʱ��زɼ�������
/*108*/{0x601E,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_601E)},//�ɼ������
/*109*/{0x8000, 8, CLASS_AAR_MAR(Class8AAR_8000, Class8MAR_8000 )},//ң��
/*110*/{0x8001, 8, CLASS_AAR_MAR(Class8AAR_8001, Class8MAR_8001 )},//����
/*111*/{0x8002, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR_8002 )},//�߷Ѹ澯
/*112*/{0x8003,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_800X)},//һ��������Ϣ
/*113*/{0x8004,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_800X)},//��Ҫ������Ϣ
/*114*/{0x8100, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�ն˱�����ֵ
/*115*/{0x8101, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�ն˹���ʱ��
/*116*/{0x8102, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//���ظ澯ʱ��
/*117*/{0x8103,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR_8106)},//ʱ�ι���
/*118*/{0x8104,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//���ݿ�
/*119*/{0x8105,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//Ӫҵ��ͣ��
/*120*/{0x8106,13, CLASS_AAR_MAR(Class13AAR_8106,Class13MAR_8106)},//��ǰ�����¸���
/*121*/{0x8107,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//�����
/*122*/{0x8108,13, CLASS_AAR_MAR(Class13AAR,     Class13MAR     )},//�µ��
/*123*/{0x8109, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//ʱ�ο����õ�Ԫ
/*124*/{0x810A, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//���ݿ����õ�Ԫ
/*125*/{0x810B, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//Ӫҵ��ͣ�����õ�Ԫ
/*126*/{0x810C, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//��������õ�Ԫ
/*127*/{0x810D, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//�µ�����õ�Ԫ
/*128*/{0x810E, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//���ƶ���
/*129*/{0x810F, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//��բ�ִ�
/*130*/{0x8110, 8, CLASS_AAR_MAR(Class8AAR_1,    Class8MAR_1    )},//��ض�ֵ
/*131*/{0xF000,18, CLASS_AAR_MAR(Class18AAR_F000,Class18MAR_F000)},//�ļ���֡�������
/*132*/{0xF001,18, CLASS_AAR_MAR(Class18AAR_F001,Class18MAR_F001)},//�ļ��ֿ鴫�����
/*133*/{0xF002,18, CLASS_AAR_MAR(Class18AAR_F002,Class18MAR_F002)},//�ļ���չ�������
/*134*/{0xF002,18, CLASS_AAR_MAR(Class18AAR_F002,Class18MAR_F002)},//�ļ���չ�������
/*135*/{0xF100,21, CLASS_AAR_MAR(Class21AAR,     Class21MAR     )},//ESAM
/*136*/{0xF101, 8, CLASS_AAR_MAR(Class8AAR_F101, Class8MAR_F101 )},//��ȫģʽ
/*137*/{0xF200,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//RS232
/*138*/{0xF201,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//����
/*139*/{0xF202,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//����
/*140*/{0xF203,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//����������
/*141*/{0xF204,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//ֱ��ģ����
/*142*/{0xF205,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//�̵������
/*143*/{0xF206,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//�澯���
/*144*/{0xF207,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//�๦�ܶ���
/*145*/{0xF208,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_2   )},//���ɵ�Ԫ
/*145*/{0xF209,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_2   )},//�ز�/΢��������ģ��
/*146*/{0xF20A,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR     )},//��������
/*147*/{0xF20B,22, CLASS_AAR_MAR(Class22AAR,     Class22MAR_1   )},//����
/*148*/{0xFFF0,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�豸������Ϣ
/*149*/{0xFFF1,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�����ڲ�ͨ�Ų���
/*150*/{0xFFF2,8,  CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�����ڲ�ͨ�Ų���
/*151*/{0x7000,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_7000)},//�ļ�����
/*152*/{0x7010,11, CLASS_AAR_MAR(Class11AAR,     Class11MAR_7010)},//�ű�����
/*153*/{0xF300,17, CLASS_AAR_MAR(Class17AAR,     Class17MAR     )},//�Զ�����
/*154*/{0xF301,17, CLASS_AAR_MAR(Class17AAR,     Class17MAR     )},//��������
/*155*/{0x3108, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*156*/{0x3117, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*157*/{0x311A, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*158*/{0x3107, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*159*/{0x3116, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*160*/{0x3112, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//
/*161*/{0x4520, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//����Զ��ͨ�Ŷ����㱸��ͨ��
#if HENAN_FUN == 1
/*162*/{0xFF10, 8, CLASS_AAR_MAR(Class8AAR,      Class8MAR      )},//�����ߴ�����Ϣ
/*163*/{0x3411, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//�����ߴ����¼���¼
#endif
#if SECURITY_DEFEND == 1
/*164*/{0x3140, 7, CLASS_AAR_MAR(Class7AAR,      Class7MAR      )},//��ȫ�¼������¼
#endif
#if HUNAN_FUN == 1
/*165*/{0x2212, 6, CLASS_AAR_MAR(Class6AAR,      Class6MAR      )},//��ǰ���״̬
#endif
};
#if(HARDWARE_TYPE != 9)
const ClassBaseList_t ObjectList_Logic1Device[]=
{
/*001*/{0x0000, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// ����й�����
/*002*/{0x0001, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A������й�����
/*003*/{0x0002, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B������й�����
/*004*/{0x0003, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C������й�����
/*005*/{0x0010, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// �����й�����
/*006*/{0x0011, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A�������й�����
/*007*/{0x0012, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B�������й�����
/*008*/{0x0013, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C�������й�����
/*009*/{0x0020, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// �����й�����
/*010*/{0x0021, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A�෴���й�����
/*011*/{0x0022, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B�෴���й�����
/*012*/{0x0023, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C�෴���й�����
/*013*/{0x0030, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// ����޹�1����
/*014*/{0x0031, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A������޹�1����
/*015*/{0x0032, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B������޹�1����
/*016*/{0x0033, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C������޹�1����
/*017*/{0x0040, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// ����޹�2����
/*018*/{0x0041, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A������޹�2����
/*019*/{0x0042, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B������޹�2����
/*020*/{0x0043, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C������޹�2����
/*021*/{0x0050, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// I�����޹�����
/*022*/{0x0051, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A��I�����޹�����
/*023*/{0x0052, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B��I�����޹�����
/*024*/{0x0053, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C��I�����޹�����
/*025*/{0x0060, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// II�����޹�����
/*026*/{0x0061, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A��II�����޹�����
/*027*/{0x0062, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B��II�����޹�����
/*028*/{0x0063, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C��II�����޹�����
/*029*/{0x0070, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// III�����޹�����
/*030*/{0x0071, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A��III�����޹�����
/*031*/{0x0072, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B��III�����޹�����
/*032*/{0x0073, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C��III�����޹�����
/*033*/{0x0080, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// IV�����޹�����
/*034*/{0x0081, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A��IV�����޹�����
/*035*/{0x0082, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B��IV�����޹�����
/*036*/{0x0083, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C��IV�����޹�����
/*037*/{0x0090, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// �������ڵ���
/*038*/{0x0091, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A���������ڵ���
/*039*/{0x0092, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B���������ڵ���
/*040*/{0x0093, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C���������ڵ���
/*041*/{0x00A0, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},// �������ڵ���
/*042*/{0x00A1, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A�෴�����ڵ���
/*043*/{0x00A2, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B�෴�����ڵ���
/*044*/{0x00A3, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C�෴�����ڵ���
/*045*/{0x0110, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//��������й��ܵ���
/*046*/{0x0111, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A����������й�����
/*047*/{0x0112, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B����������й�����
/*048*/{0x0113, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C����������й�����
/*049*/{0x0120, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//��������й��ܵ���
/*050*/{0x0121, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A�෴������й�����
/*051*/{0x0122, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B�෴������й�����
/*052*/{0x0123, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C�෴������й�����
/*053*/{0x0210, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//����г���й��ܵ���
/*054*/{0x0211, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A������г���й�����
/*055*/{0x0212, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B������г���й�����
/*056*/{0x0213, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C������г���й�����
/*057*/{0x0220, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//����г���й��ܵ���
/*058*/{0x0221, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//A�෴��г���й�����
/*059*/{0x0222, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//B�෴��г���й�����
/*060*/{0x0223, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//C�෴��г���й�����
/*061*/{0x0300, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//ͭ���й��ܵ��ܲ�����
/*062*/{0x0400, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//�����й��ܵ��ܲ�����
/*063*/{0x0500, 1, CLASS_AAR_MAR(Class1AAR,     Class1MAR       )},//�����ܵ���
/*064*/{0x1000, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//����й��������
/*065*/{0x1001, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A������й��������
/*066*/{0x1002, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B������й��������
/*067*/{0x1003, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C������й��������
/*068*/{0x1010, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//�����й��������
/*069*/{0x1011, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A�������й��������
/*070*/{0x1012, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B�������й��������
/*071*/{0x1013, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C�������й��������
/*072*/{0x1020, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//�����й��������
/*073*/{0x1021, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A�෴���й��������
/*074*/{0x1022, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B�෴���й��������
/*075*/{0x1023, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C�෴���й��������
/*076*/{0x1030, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//����޹�1�������
/*077*/{0x1031, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A������޹�1�������
/*078*/{0x1032, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B������޹�1�������
/*079*/{0x1033, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C������޹�1�������
/*080*/{0x1040, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//����޹�2�������
/*081*/{0x1041, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A������޹�2�������
/*082*/{0x1042, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B������޹�2�������
/*083*/{0x1043, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C������޹�2�������
/*084*/{0x1050, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//I�����޹��������
/*085*/{0x1051, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A��I�����޹��������
/*086*/{0x1052, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B��I�����޹��������
/*087*/{0x1053, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C��I�����޹��������
/*088*/{0x1060, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//II�����޹��������
/*089*/{0x1061, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A��II�����޹��������
/*090*/{0x1062, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B��II�����޹��������
/*091*/{0x1063, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C��II�����޹��������
/*092*/{0x1070, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//III�����޹��������
/*093*/{0x1071, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A��III�����޹��������
/*094*/{0x1072, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B��III�����޹��������
/*095*/{0x1073, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C��III�����޹��������
/*096*/{0x1080, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//IV�����޹��������
/*097*/{0x1081, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A��IV�����޹��������
/*098*/{0x1082, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B��IV�����޹��������
/*099*/{0x1083, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C��IV�����޹��������
/*100*/{0x1090, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//���������������
/*101*/{0x1091, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A�����������������
/*102*/{0x1092, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B�����������������
/*103*/{0x1093, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C�����������������
/*104*/{0x10A0, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//���������������
/*105*/{0x10A1, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//A�෴�������������
/*106*/{0x10A2, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//B�෴�������������
/*107*/{0x10A3, 2, CLASS_AAR_MAR(Class2AAR,     Class2MAR       )},//C�෴�������������
/*108*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//������ѹ
/*109*/{0x2001, 3, CLASS_AAR_MAR(Class3AAR_2001,Class3MAR       )},//��������
/*110*/{0x2002, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//������ѹ���
/*111*/{0x2003, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//������ѹ�������
/*112*/{0x2004, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//�й�����
/*113*/{0x2005, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//�޹�����
/*114*/{0x2006, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//���ڹ���
/*115*/{0x2007, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//һ����ƽ���й�����
/*116*/{0x2008, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//һ����ƽ���޹�����
/*117*/{0x2009, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//һ����ƽ�����ڹ���
/*118*/{0x200A, 4, CLASS_AAR_MAR(Class4AAR,     Class4MAR       )},//��������
/*119*/{0x200B, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//��ѹ����ʧ���
/*120*/{0x200C, 3, CLASS_AAR_MAR(Class3AAR,     Class3MAR       )},//��������ʧ���
/*121*/{0x200D, 5, CLASS_AAR_MAR(Class5AAR,     Class5MAR       )},//��ѹг������
/*122*/{0x200E, 5, CLASS_AAR_MAR(Class5AAR,     Class5MAR       )},//����г������
/*123*/{0x200F, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//����Ƶ��
/*124*/{0x2010, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//�����¶�
/*125*/{0x2011, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//ʱ�ӵ��ѹ
/*126*/{0x2012, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//ͣ�糭���ѹ
/*127*/{0x2013, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//ʱ�ӵ�ع���ѹ
/*128*/{0x2014, 6, CLASS_AAR_MAR(Class6AAR,     Class6MAR       )},//���ܱ� ����״̬��
/*129*///{0x2025, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//�¼���ǰֵ
/*130*///{0x202C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//����ǰ��Ǯ���ļ�
/*131*///{0x202F, 8, CLASS_AAR_MAR(Class8AAR_202F,Class8MAR        },//���ܱ������ϱ�
/*132*///{0x3300, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR        },//ͨ���ϱ�״̬
/*133*/{0x300F, 7, CLASS_AAR_MAR(Class7AAR,     Class7MAR       )},//���ɵ�ѹ������
/*134*/{0x4008, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//����ʱ�����л�ʱ��
/*135*/{0x4009, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//������ʱ���л�ʱ��
/*136*/{0x400A, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���׷�ʱ�����л�ʱ��
/*137*/{0x400B, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���׽��ݵ���л�ʱ��
/*138*/{0x400C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ʱ��ʱ����
/*139*/{0x400D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//������
/*140*/{0x400E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//г����������
/*141*/{0x400F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��Կ������
/*142*/{0x4010, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//����Ԫ����
/*143*/{0x4011, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�������ձ�
/*144*/{0x4012, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//������������
/*145*/{0x4013, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��������õ���ʱ�α��
/*146*/{0x4014, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ǰ��ʱ����
/*147*/{0x4015, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//������ʱ����
/*148*/{0x4016, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ǰ����ʱ�α�
/*149*/{0x4017, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��������ʱ�α�
/*150*/{0x4018, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ǰ�׷��ʵ��
/*151*/{0x4019, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�����׷��ʵ��
/*152*/{0x401A, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ǰ�׽��ݵ��
/*153*/{0x401B, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�����׽��ݵ��
/*154*/{0x401C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�������������
/*155*/{0x401D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ѹ���������
/*156*/{0x401E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���������ֵ
/*157*/{0x401F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���������ֵ
/*158*/{0x4022, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�忨״̬��
/*159*/{0x4023, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��֤��Чʱ��
/*160*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//��ѹ�ϸ��ʲ���
/*161*/{0x410C, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC����絼ϵ��
/*162*/{0x410D, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC����翹ϵ��
/*163*/{0x410E, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC�������ϵ��
/*164*/{0x410F, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//ABC�������ϵ��
/*165*/{0x4110, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���ܱ�����������1
/*166*/{0x4111, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//���������
/*167*/{0x4112, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�й���Ϸ�ʽ������
/*168*/{0x4113, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�޹���Ϸ�ʽ1������
/*169*/{0x4114, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�޹���Ϸ�ʽ2������
/*170*/{0x4116, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//������
/*171*/{0x4117, 8, CLASS_AAR_MAR(Class8AAR,     Class8MAR       )},//�ڼ�������������
/*172*/{0x5002, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//���Ӷ���
/*172*/{0x5003, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//Сʱ����
/*174*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//�ն���
/*175*/{0x5005, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//�����ն���
/*176*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,     Class9MAR       )},//�¶���
/*177*/{0x4300,19, CLASS_AAR_MAR(Class19AAR,   Class19MAR       )},//�������豸��Ϣ
};


#else
const ClassBaseList_t ObjectList_Logic1Device[]=
{
/*001*/{0x2000, 3, CLASS_AAR_MAR(Class3AAR,    Class3MAR            )},//������ѹ
/*002*/{0x5004, 9, CLASS_AAR_MAR(Class9AAR,    Class9MAR            )},//�ն���
/*002*/{0x5006, 9, CLASS_AAR_MAR(Class9AAR,    Class9MAR            )},//�¶���
/*003*/{0x4030, 8, CLASS_AAR_MAR(Class8AAR,    Class8MAR            )},//��ѹ�ϸ��ʲ���
};

#endif

const ObjectLit_t ObjectList_All[4] = //1�������߼��豸+3���߼��豸
{
    {ObjectList_ManageDevice,sizeof(ObjectList_ManageDevice)/sizeof(ClassBaseList_t)},//�����߼��豸
    {ObjectList_Logic1Device,sizeof(ObjectList_Logic1Device)/sizeof(ClassBaseList_t)},//�߼��豸-����
    {NULL,                   0},//δ����
    {NULL,                   0},//δ����
};
