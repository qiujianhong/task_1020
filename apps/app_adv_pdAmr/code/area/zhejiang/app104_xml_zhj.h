/*
*********************************************************************
* @file    app104_xml.c
* @brief�� ��������
* @author��all
* @date��   2019.12.24
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#ifndef	__APP104_XML_ZHJ_H
#define __APP104_XML_ZHJ_H

extern int pw_readyxfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo);
extern int pw_readycfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo);
extern int pw_readParafromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo);
extern int pw_readfaultfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo);

#endif

