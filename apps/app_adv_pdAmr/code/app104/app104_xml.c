/*
*********************************************************************
* @file    app104_xml.c
* @brief： 公共内容
* @author：all
* @date：   2019.12.24
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include <stdlib.h>
#include "framecomm.h"
#include "mxml.h"
#include "mxml-private.h"
#include "app104_xml.h"
#include "pw_protocol.h"

#define ADV_PD_YC_XML	"yc.xml"
#define ADV_PD_YX_XML	"yx.xml"
#define ADV_PD_EN_XML	"en.xml"
#define ADV_PD_YK_XML	"yk.xml"
#define ADV_PD_FAULT_XML	"fault.xml"
#define ADV_PD_PARA_XML	"para.xml"

int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt);


extern YC_DATA_CONFIG g_aYcDataConfig[MAX_YC_NUM];
extern YX_DATA_CONFIG g_aYxDataConfig[MAX_YX_NUM];
extern EN_DATA_CONFIG g_aEnergyDataConfig[MAX_ENERGY_NUM];
extern FAULT_REPORT_CONFIG g_aFaultDataConfig[FAULT_MAX_NUM];
extern PARA_SET_VALUE_T g_aParaValueInit[PARA_MAX_NUM];
extern YK_DATA_CONFIG g_aYkDataConfig[MAX_YK_NUM];


int advPd_readXML()
{
	uint16 yxnum, ycnum, ennum, yknum, faultnum, paranum;

	advPd_readyxfromXML(&yxnum);
	advPd_readycfromXML(&ycnum);
	advPd_readenfromXML(&ennum);
	advPd_readykfromXML(&yknum);
	advPd_readfaultfromXML(&faultnum);
	advPd_readParafromXML(&paranum);
	APP104_FMT_DEBUG("ycnum = %d, yxnum = %d, ennum = %d, yknum = %d, faultnum = %d, paranum = %d\n", ycnum, yxnum, ennum, yknum, faultnum, paranum);

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readyxfromXML
* 函数功能: 加载遥信点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readyxfromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read yx.xml...\n");

	fp = fopen(ADV_PD_YX_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read yx.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Table> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	int No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}
		//printf("Item Addr = %s, name = %s, num = %s\n", mxmlElementGetAttr(node, "Addr"), mxmlElementGetAttr(node, "Name"), mxmlElementGetAttr(node, "OadColsNum"));

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aYxDataConfig[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		g_aYxDataConfig[No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		//APP104_BUF_DEBUG(mxmlElementGetAttr(node, "Name"), strlen(mxmlElementGetAttr(node, "Name")), "GetAttr:");
		//APP104_BUF_DEBUG(g_aYxDataConfig[No].szObjName, strlen(g_aYxDataConfig[No].szObjName),"szObjName:");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_SP_NA_1") == 0)
			g_aYxDataConfig[No].nSignalType = M_SP_NA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_SP_TA_1") == 0)
			g_aYxDataConfig[No].nSignalType = M_SP_TA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_DP_NA_1") == 0)
			g_aYxDataConfig[No].nSignalType = M_DP_NA_1;
		else 
			g_aYxDataConfig[No].nSignalType = M_DP_TA_1;

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYxDataConfig[No].nDbCenterOAD = MAKE_LONG(Rversedata);

		g_aYxDataConfig[No].nOadColsNum = atoi(mxmlElementGetAttr(node, "OadColsNum"));

		if(g_aYxDataConfig[No].nOadColsNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aYxDataConfig[No].nOadColsNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
			
				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "SubOAD"), 8, 0);
				memcpy_rev(Rversedata, data, 4);
				g_aYxDataConfig[No].aOadCols[i] = MAKE_LONG(Rversedata);
				}
		}
		
		g_aYxDataConfig[No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYxDataConfig[No].nDealType = MAKE_LONG(Rversedata);

		No++;
	}

	APP104_FMT_DEBUG("yx point table:\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, Name=%s, Type=%d, DbCenterOAD=%08x, index=%d, DealType=%08x \n", 
								i, g_aYxDataConfig[i].nObjAddr, g_aYxDataConfig[i].szObjName, g_aYxDataConfig[i].nSignalType, 
								g_aYxDataConfig[i].nDbCenterOAD, g_aYxDataConfig[No].nItemIndex, g_aYxDataConfig[No].nDealType);
		
	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readycfromXML
* 函数功能: 加载遥测点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readycfromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node  = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read yc.xml...\n");

	fp = fopen(ADV_PD_YC_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read yc.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Table> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	uint16 No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aYcDataConfig[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		g_aYcDataConfig[No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_NA_1") == 0) 
			g_aYcDataConfig[No].nSignalType = M_ME_NA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_NB_1") == 0)
			g_aYcDataConfig[No].nSignalType = M_ME_NB_1;
		else
			g_aYcDataConfig[No].nSignalType = M_ME_NC_1;

		g_aYcDataConfig[No].nReportRatio = atoi(mxmlElementGetAttr(node, "ReportRatio"));

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "DealZoneAddr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);
		g_aYcDataConfig[No].nDeadZoneAddr = MAKE_SHORT(Rverseaddr);

		g_aYcDataConfig[No].fDeadZoneValue = atof(mxmlElementGetAttr(node, "DealZoneValue"));
		g_aYcDataConfig[No].nLogicId = atoi(mxmlElementGetAttr(node, "LogicId"));
		g_aYcDataConfig[No].nInfoNum = atoi(mxmlElementGetAttr(node, "InfoNum"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYcDataConfig[No].nDbCenterOAD = MAKE_LONG(Rversedata);

		g_aYcDataConfig[No].nOadColsNum = atoi(mxmlElementGetAttr(node, "OadColsNum"));

		if(g_aYcDataConfig[No].nOadColsNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aYcDataConfig[No].nOadColsNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
			
				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "SubOAD"), 8, 0);
				memcpy_rev(Rversedata, data, 4);
				g_aYcDataConfig[No].aOadCols[i] = MAKE_LONG(Rversedata);
				}
		}
		
		g_aYcDataConfig[No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));
		g_aYcDataConfig[No].fFullValue = atof(mxmlElementGetAttr(node, "FullValue"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYcDataConfig[No].nDealType = MAKE_LONG(Rversedata);

		No++;
	}

	APP104_FMT_DEBUG("yc point table:\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, Name=%s, Type=%d, Rate=%d, DealZoneAddr=%04x, DeadZoneValue=%f, LogicId=%d, InfoNum=%x, DbCenterOAD=%08x, suboadnum = %d, index=%d, FullValue=%f, DealType=%08x\n", 
								i, g_aYcDataConfig[i].nObjAddr, g_aYcDataConfig[i].szObjName, g_aYcDataConfig[i].nSignalType, 
								g_aYcDataConfig[i].nReportRatio, g_aYcDataConfig[i].nDeadZoneAddr, g_aYcDataConfig[i].fDeadZoneValue,
								g_aYcDataConfig[i].nLogicId, g_aYcDataConfig[i].nInfoNum, g_aYcDataConfig[i].nDbCenterOAD, g_aYcDataConfig[i].nOadColsNum, g_aYcDataConfig[i].nItemIndex, g_aYcDataConfig[i].fFullValue, g_aYcDataConfig[No].nDealType);
		
		if(g_aYcDataConfig[i].nOadColsNum > 0 )
		{
			int j=0;
			for(j=0;j<g_aYcDataConfig[i].nOadColsNum;j++)
			{
				APP104_FMT_DEBUG("OadCol = %08x\n", g_aYcDataConfig[i].aOadCols[j]);
			}
		}

	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readenfromXML
* 函数功能: 加载电量点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readenfromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read en.xml...\n");

	fp = fopen(ADV_PD_EN_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read en.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Table> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	uint16 No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aEnergyDataConfig[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		g_aEnergyDataConfig[No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_IT_TA_1") == 0)
			g_aEnergyDataConfig[No].nSignalType = M_IT_TA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_IT_TB_1") == 0)
			g_aEnergyDataConfig[No].nSignalType = M_IT_TB_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_IT_TC_1") == 0)
			g_aEnergyDataConfig[No].nSignalType = M_IT_TC_1;
		else
			g_aEnergyDataConfig[No].nSignalType = M_IT_NA_1;

		g_aEnergyDataConfig[No].nReportRatio = atoi(mxmlElementGetAttr(node, "ReportRatio"));

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "DealZoneAddr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);
		g_aEnergyDataConfig[No].nDeadZoneAddr = MAKE_SHORT(Rverseaddr);

		g_aEnergyDataConfig[No].fDeadZoneValue = atof(mxmlElementGetAttr(node, "DealZoneValue"));
		g_aEnergyDataConfig[No].nLogicId = atoi(mxmlElementGetAttr(node, "LogicId"));
		g_aEnergyDataConfig[No].nInfoNum = atoi(mxmlElementGetAttr(node, "InfoNum"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aEnergyDataConfig[No].nDbCenterOAD = MAKE_LONG(Rversedata);

		g_aEnergyDataConfig[No].nOadColsNum = atoi(mxmlElementGetAttr(node, "OadColsNum"));

		if(g_aEnergyDataConfig[No].nOadColsNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aEnergyDataConfig[No].nOadColsNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
			
				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "SubOAD"), 8, 0);
				memcpy_rev(Rversedata, data, 4);
				g_aEnergyDataConfig[No].aOadCols[i] = MAKE_LONG(Rversedata);
				}
		}
		
		g_aEnergyDataConfig[No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aEnergyDataConfig[No].nDealType = MAKE_LONG(Rversedata);

		No++;
	}

	APP104_FMT_DEBUG("en point table\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, Name=%s, Type=%d, Rate=%d, DealZoneAddr=%04x, DeadZoneValue=%f, LogicId=%d, InfoNum=%x, DbCenterOAD=%08x, suboadnum = %d, index=%d, DealType=%08x\n", 
								i, g_aEnergyDataConfig[i].nObjAddr, g_aEnergyDataConfig[i].szObjName, g_aEnergyDataConfig[i].nSignalType, 
								g_aEnergyDataConfig[i].nReportRatio, g_aEnergyDataConfig[i].nDeadZoneAddr, g_aEnergyDataConfig[i].fDeadZoneValue,
								g_aEnergyDataConfig[i].nLogicId, g_aEnergyDataConfig[i].nInfoNum, g_aEnergyDataConfig[i].nDbCenterOAD, g_aEnergyDataConfig[i].nOadColsNum, g_aEnergyDataConfig[i].nItemIndex, g_aEnergyDataConfig[No].nDealType);
		
		if(g_aEnergyDataConfig[i].nOadColsNum > 0 )
		{
			int j=0;
			for(j=0;j<g_aEnergyDataConfig[i].nOadColsNum;j++)
			{
				APP104_FMT_DEBUG("OadCol = %08x\n", g_aEnergyDataConfig[i].aOadCols[j]);
			}
		}
	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readykfromXML
* 函数功能: 加载遥控点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readykfromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;

	printf("start read yk.xml...\n");

	fp = fopen(ADV_PD_YK_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read yk.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Item> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	uint16 No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}
		//printf("Item Addr = %s, name = %s, num = %s\n", mxmlElementGetAttr(node, "Addr"), mxmlElementGetAttr(node, "Name"), mxmlElementGetAttr(node, "OadColsNum"));

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aYkDataConfig[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		g_aYkDataConfig[No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "C_SC_NA_1") == 0)
			g_aYkDataConfig[No].nSignalType = C_SC_NA_1;
		else 
			g_aYkDataConfig[No].nSignalType = C_DC_NA_1;

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYkDataConfig[No].nDbCenterOAD = MAKE_LONG(Rversedata);
		g_aYkDataConfig[No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));
		g_aYkDataConfig[No].nSelect = atoi(mxmlElementGetAttr(node, "Select"));

		No++;
	}

	APP104_FMT_DEBUG("yk point table:\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, Name=%s, Type=%d, DbCenterOAD=%08x, Index=%d, Select=%d\n", 
								i, g_aYkDataConfig[i].nObjAddr, g_aYkDataConfig[i].szObjName, g_aYkDataConfig[i].nSignalType, g_aYkDataConfig[i].nDbCenterOAD, 
								g_aYkDataConfig[i].nItemIndex, g_aYkDataConfig[i].nSelect);
		
	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readfaultfromXML
* 函数功能: 加载故障事件点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readfaultfromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read fault.xml...\n");

	fp = fopen(ADV_PD_FAULT_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read fault.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Item> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	uint16 No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}
		//printf("Item Addr = %s, name = %s, num = %s\n", mxmlElementGetAttr(node, "Addr"), mxmlElementGetAttr(node, "Name"), mxmlElementGetAttr(node, "OadColsNum"));

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aFaultDataConfig[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		g_aFaultDataConfig[No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_FT_NA_1") == 0)
			g_aFaultDataConfig[No].nSignalType = M_FT_NA_1;
		else 
			g_aFaultDataConfig[No].nSignalType = C_DC_NA_1;

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aFaultDataConfig[No].nDbCenterOAD = MAKE_LONG(Rversedata);
		g_aFaultDataConfig[No].nReportNum = atoi(mxmlElementGetAttr(node, "ReportNum"));
		g_aFaultDataConfig[No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));
		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "FaultYxAddr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);
		g_aFaultDataConfig[No].nFaultYxAddr = MAKE_SHORT(Rverseaddr);

		if(g_aFaultDataConfig[No].nReportNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aFaultDataConfig[No].nReportNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}

				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "ReportAddr"), 4, 0);
				memcpy_rev(Rversedata, data, 2);
				g_aFaultDataConfig[No].aReportAddr[i] = MAKE_SHORT(Rversedata);
				}
			}

		No++;
	}

	APP104_FMT_DEBUG("fault point table:\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, Name=%s, Type=%d, FaultYxAddr=%04x, DbCenterOAD=%08x, Index=%d\n", 
								i, g_aFaultDataConfig[i].nObjAddr, g_aFaultDataConfig[i].szObjName, g_aFaultDataConfig[i].nSignalType, g_aFaultDataConfig[i].nFaultYxAddr, g_aFaultDataConfig[i].nDbCenterOAD, 
								g_aFaultDataConfig[i].nItemIndex);
		if(g_aFaultDataConfig[i].nReportNum > 0 )
		{
			int j=0;
			for(j=0;j<g_aFaultDataConfig[i].nReportNum;j++)
			{
				APP104_FMT_DEBUG("OadCol = %04x\n", g_aFaultDataConfig[i].aReportAddr[j]);
			}
		}
	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

/*******************************************************************************
* 函数名称: advPd_readParafromXML
* 函数功能: 加载参数点表
* 输入参数: 无
* 输出参数: 信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int advPd_readParafromXML(uint16 *outNo)
{
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;

	printf("start read para.xml...\n");

	fp = fopen(ADV_PD_PARA_XML, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
		return FALSE;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	
	if (!tree)
  	{
    	printf("Unable to read para.xml file with default callback.\n");
    	return FALSE;
  	}

	node = mxmlFindElement(tree, tree, "Table", NULL, NULL, MXML_DESCEND);
	if(node == NULL)
	{
		printf("Unable to find second <Item> element in XML tree.\n");
		return FALSE;
		}
	//printf("Name = %s\n", mxmlElementGetAttr(node, "Name"));

	uint16 No = 0;
	while(1)
	{
		/* Find the first "a" element */
		node = mxmlFindElement(node, tree, "Item", NULL, NULL, MXML_DESCEND);
		if(node == NULL)
		{
			printf("%d Unable to find second <Item> element in XML tree.\n", No);
			break;
		}
		//printf("Item Addr = %s, name = %s, num = %s\n", mxmlElementGetAttr(node, "Addr"), mxmlElementGetAttr(node, "Name"), mxmlElementGetAttr(node, "OadColsNum"));

		char addr[2] = {0};
		char Rverseaddr[2] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);

		g_aParaValueInit[No].nObjAddr = MAKE_SHORT(Rverseaddr);
		//g_aParaValueInit[No]. = mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_NONE") == 0)
			g_aParaValueInit[No].nTagType = TAG_NONE;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_BOOL") == 0)
			g_aParaValueInit[No].nTagType = TAG_BOOL;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT8") == 0)
			g_aParaValueInit[No].nTagType = TAG_INT8;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT8") == 0)
			g_aParaValueInit[No].nTagType = TAG_UINT8;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT16") == 0)
			g_aParaValueInit[No].nTagType = TAG_INT16;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT16") == 0)
			g_aParaValueInit[No].nTagType = TAG_UINT16;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT32") == 0)
			g_aParaValueInit[No].nTagType = TAG_INT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT32") == 0)
			g_aParaValueInit[No].nTagType = TAG_UINT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT64") == 0)
			g_aParaValueInit[No].nTagType = TAG_INT64;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT64") == 0)
			g_aParaValueInit[No].nTagType = TAG_UINT64;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_FLOAT32") == 0)
			g_aParaValueInit[No].nTagType = TAG_FLOAT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_FLOAT64") == 0)
			g_aParaValueInit[No].nTagType = TAG_FLOAT64;
		else
			g_aParaValueInit[No].nTagType = TAG_NONE;
		
		g_aParaValueInit[No].nDataLen = atoi(mxmlElementGetAttr(node, "DataLen"));
		//printf("Item Valid = %s\n", mxmlElementGetAttr(node, "Valid"));
		if(strcmp(mxmlElementGetAttr(node, "Valid"), "1") == 0)
			g_aParaValueInit[No].bValid = TRUE;
		else
			g_aParaValueInit[No].bValid = FALSE;
		memset(g_aParaValueInit[No].unValue.arrayV, '\0', MAX_FIXED_VALUE_LEN);
		
		g_aParaValueInit[No].fDefaultValue = atof(mxmlElementGetAttr(node, "DefaultValue"));
		g_aParaValueInit[No].fMinValue = atof(mxmlElementGetAttr(node, "MinValue"));
		g_aParaValueInit[No].fMaxValue = atof(mxmlElementGetAttr(node, "MaxValue"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aParaValueInit[No].nDealType = MAKE_LONG(Rversedata);

		No++;
	}

	APP104_FMT_DEBUG("para point table:\n");
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, Addr=%04x, TagType=%d, Valid=%d, DefaultValue=%f, MinValue=%f, MaxValue=%f, nDealType=%08x\n", 
								i, g_aParaValueInit[i].nObjAddr, g_aParaValueInit[i].nTagType, g_aParaValueInit[i].bValid, g_aParaValueInit[i].fDefaultValue, 
								g_aParaValueInit[i].fMinValue, g_aParaValueInit[i].fMaxValue, g_aParaValueInit[No].nDealType);
		
	}

	mxmlDelete(tree);

	*outNo = No;

	return TRUE;
}

