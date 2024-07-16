/*
*********************************************************************
* @file    app104_xml.c
* @brief： 公共内容
* @author：all
* @date：   2019.12.24
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#ifndef	__APP104_XML_H
#define __APP104_XML_H


int advPd_readXML();
int advPd_readyxfromXML(uint16 *outNo);
int advPd_readycfromXML(uint16 *outNo);
int advPd_readenfromXML(uint16 *outNo);
int advPd_readykfromXML(uint16 *outNo);
int advPd_readfaultfromXML(uint16 *outNo);
int advPd_readParafromXML(uint16 *outNo);

#endif

