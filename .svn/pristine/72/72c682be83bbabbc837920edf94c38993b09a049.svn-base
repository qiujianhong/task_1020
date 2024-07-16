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
#include "app104_xml_zhj.h"
#include "pw_protocol_zhj.h"

int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt);

/*******************************************************************************
* 函数名称: pw_readyxfromXML_zhj
* 函数功能: 加载遥信点表
* 输入参数: uint8 equipType    设备类型
            uint8 equipGroup   设备组别
            uint16 equipNo     组内序号
* 输出参数: uint16 *outNo          信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int pw_readyxfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo)
{
    if((equipType < 1) || (equipNo < 1))
    {
        APP104_FMT_DEBUG("pw_readyxfromXML_zhj() 参数非法! equipType=%d, equipNo=%d \n", equipType, equipNo);
    }
    
    //点表文件名
    char filename[50] = {0};
    sprintf(filename, "zhj_%d_%d_%d_yx.xml", equipType, equipGroup, equipNo);
    APP104_FMT_DEBUG("\n\n遥信点表开始加载...设备类型:%d, 组别:%d, 编号:%d, 文件名:%s \n", equipType, equipGroup, equipNo, filename);
    
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read yx.xml...\n");

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
        APP104_FMT_DEBUG("xml file 打开失败! \n");
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

    //取出当前设备第1个遥信信号的索引(索引从0开始)
    uint32 FirstIndex = g_aYxDataIndexZhj[equipType-1][equipGroup][equipNo-1];
    APP104_FMT_DEBUG("当前信号索引 FirstIndex=%u \n", FirstIndex);

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

        g_aYxDataConfigZhj[FirstIndex + No].nEqType = equipType;
        g_aYxDataConfigZhj[FirstIndex + No].nEqGroup = equipGroup;
        g_aYxDataConfigZhj[FirstIndex + No].nEquipNo = equipNo;
        
		char addr[4] = {0};
		char Rverseaddr[4] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

        commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 8, 0);
		memcpy_rev(Rverseaddr, addr, 4);
		g_aYxDataConfigZhj[FirstIndex + No].nObjAddrU32 = MAKE_LONG(Rverseaddr);

		g_aYxDataConfigZhj[FirstIndex + No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		//APP104_BUF_DEBUG(mxmlElementGetAttr(node, "Name"), strlen(mxmlElementGetAttr(node, "Name")), "GetAttr:");
		//APP104_BUF_DEBUG(g_aYxDataConfig[No].szObjName, strlen(g_aYxDataConfig[No].szObjName),"szObjName:");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_SP_NA_1") == 0)
			g_aYxDataConfigZhj[FirstIndex + No].nSignalType = M_SP_NA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_SP_TA_1") == 0)
			g_aYxDataConfigZhj[FirstIndex + No].nSignalType = M_SP_TA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_DP_NA_1") == 0)
			g_aYxDataConfigZhj[FirstIndex + No].nSignalType = M_DP_NA_1;
		else 
			g_aYxDataConfigZhj[FirstIndex + No].nSignalType = M_DP_TA_1;

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYxDataConfigZhj[FirstIndex + No].nDbCenterOAD = MAKE_LONG(Rversedata);

		g_aYxDataConfigZhj[FirstIndex + No].nOadColsNum = atoi(mxmlElementGetAttr(node, "OadColsNum"));

		if(g_aYxDataConfigZhj[FirstIndex + No].nOadColsNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aYxDataConfigZhj[FirstIndex + No].nOadColsNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
			
				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "SubOAD"), 8, 0);
				memcpy_rev(Rversedata, data, 4);
				g_aYxDataConfigZhj[FirstIndex + No].aOadCols[i] = MAKE_LONG(Rversedata);
			}
		}
		
		g_aYxDataConfigZhj[FirstIndex + No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		g_aYxDataConfigZhj[FirstIndex + No].nDealType = MAKE_LONG(Rversedata);

		No++;
	}

	APP104_FMT_DEBUG("yx point table: No=%d \n", No);
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, arrindex=%u, Addr=%08x, Type=%d, DbCenterOAD=%08x, index=%d, DealType=%08x \n", 
								i, FirstIndex+i, g_aYxDataConfigZhj[FirstIndex+i].nObjAddrU32, g_aYxDataConfigZhj[FirstIndex+i].nSignalType, 
								g_aYxDataConfigZhj[FirstIndex+i].nDbCenterOAD, g_aYxDataConfigZhj[FirstIndex + No].nItemIndex, g_aYxDataConfigZhj[FirstIndex + No].nDealType);
		
	}

	mxmlDelete(tree);

	*outNo = No;

    //更新下一个设备第1个遥信信号的索引(索引从0开始)
    uint16 nextTyIndex = 0;
    uint16 nextGrIndex = 0;
    uint16 nextSeqIndex = 0;
    int getRet = get_next_equip_attr_zhj(equipType-1, equipGroup, equipNo-1, &nextTyIndex, &nextGrIndex, &nextSeqIndex);
    if(getRet > 0)
    {
        g_aYxDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex] = FirstIndex + No;
        APP104_FMT_DEBUG("下一个设备 nextTyIndex=%d, nextGrIndex=%d, nextSeqIndex=%d, 信号索引=%u \n", 
            nextTyIndex, nextGrIndex, nextSeqIndex, g_aYxDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex]);
    }
    else
    {
        APP104_FMT_DEBUG("get_next_equip_attr_zhj() 获取失败! \n");
    }

    //更新累计数量
    g_nTotalYxNumZhj += No;
    APP104_FMT_DEBUG("目前累计 g_nTotalYxNumZhj=%u \n", g_nTotalYxNumZhj);

	return TRUE;
}

/*******************************************************************************
* 函数名称: pw_readycfromXML_zhj
* 函数功能: 加载遥测点表
* 输入参数: uint8 equipType    设备类型
            uint8 equipGroup   设备组别
            uint16 equipNo     组内序号
* 输出参数: uint16 *outNo          信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int pw_readycfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo)
{
    if((equipType < 1) || (equipNo < 1))
    {
        APP104_FMT_DEBUG("pw_readycfromXML_zhj() 参数非法! equipType=%d, equipNo=%d \n", equipType, equipNo);
    }
    
    //点表文件名
    char filename[50] = {0};
    sprintf(filename, "zhj_%d_%d_%d_yc.xml", equipType, equipGroup, equipNo);
    APP104_FMT_DEBUG("\n\n遥测点表开始加载...设备类型:%d, 组别:%d, 编号:%d, 文件名:%s \n", equipType, equipGroup, equipNo, filename);
    
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node  = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read yc.xml...\n");

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
        APP104_FMT_DEBUG("xml file 打开失败! \n");
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

    //取出当前设备第1个遥测信号的索引(索引从0开始)
    uint32 FirstIndex = g_aYcDataIndexZhj[equipType-1][equipGroup][equipNo-1];
    APP104_FMT_DEBUG("当前信号索引 FirstIndex=%u \n", FirstIndex);

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

		g_aYcDataConfigZhj[FirstIndex + No].nEqType = equipType;
        g_aYcDataConfigZhj[FirstIndex + No].nEqGroup = equipGroup;
        g_aYcDataConfigZhj[FirstIndex + No].nEquipNo = equipNo;
        
		char addr[4] = {0};
		char Rverseaddr[4] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

        commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 8, 0);
		memcpy_rev(Rverseaddr, addr, 4);
		g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 = MAKE_LONG(Rverseaddr);
        
        if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008800)  //电压统计
            g_nYcStatVoltDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008860) //谐波统计
            g_nYcStatHarmonyDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008920) //电压不平衡度统计
            g_nYcStatVoltUnbalanceDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00000080)   //电压不平衡度实时值
            g_nYcVoltUnbalance = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008940) //电压不平衡度统计
            g_nYcStatCurrUnbalanceDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008960) //频率统计
            g_nYcStatFrequencyDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00018030) //极值统计
            g_nYcStatExtremeDay = No;
        else if (g_aYcDataConfigZhj[FirstIndex + No].nObjAddrU32 == 0x00008900) //功率因数统计
            g_nYcStatPwrFactDay = No;

		g_aYcDataConfigZhj[FirstIndex + No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_NA_1") == 0) 
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_ME_NA_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_NB_1") == 0)
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_ME_NB_1;
		else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_NC_1") == 0)
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_ME_NC_1;
        else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_VBO_TB_1") == 0)
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_VBO_TB_1;
        else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_ME_TF_1") == 0)
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_ME_TF_1;
        else if(strcmp(mxmlElementGetAttr(node, "Type"), "M_IT_NB_1") == 0)
			g_aYcDataConfigZhj[FirstIndex + No].nSignalType = M_IT_NB_1;

		g_aYcDataConfigZhj[FirstIndex + No].nReportRatio = atoi(mxmlElementGetAttr(node, "ReportRatio"));

		commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "DealZoneAddr"), 4, 0);
		memcpy_rev(Rverseaddr, addr, 2);
		g_aYcDataConfigZhj[FirstIndex + No].nDeadZoneAddr = MAKE_SHORT(Rverseaddr);

		g_aYcDataConfigZhj[FirstIndex + No].fDeadZoneValue = atof(mxmlElementGetAttr(node, "DealZoneValue"));
		g_aYcDataConfigZhj[FirstIndex + No].nLogicId = atoi(mxmlElementGetAttr(node, "LogicId"));
		g_aYcDataConfigZhj[FirstIndex + No].nInfoNum = atoi(mxmlElementGetAttr(node, "InfoNum"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYcDataConfigZhj[FirstIndex + No].nDbCenterOAD = MAKE_LONG(Rversedata);

		g_aYcDataConfigZhj[FirstIndex + No].nOadColsNum = atoi(mxmlElementGetAttr(node, "OadColsNum"));

		if(g_aYcDataConfigZhj[FirstIndex + No].nOadColsNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

			int i=0;
			for(i=0;i<g_aYcDataConfigZhj[FirstIndex + No].nOadColsNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
			
				commfun_ASC2BCD(data, mxmlElementGetAttr(subnode, "SubOAD"), 8, 0);
				memcpy_rev(Rversedata, data, 4);
				g_aYcDataConfigZhj[FirstIndex + No].aOadCols[i] = MAKE_LONG(Rversedata);
				}
		}
		
		g_aYcDataConfigZhj[FirstIndex + No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));
		g_aYcDataConfigZhj[FirstIndex + No].fFullValue = atof(mxmlElementGetAttr(node, "FullValue"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		
		g_aYcDataConfigZhj[FirstIndex + No].nDealType = MAKE_LONG(Rversedata);
        g_aYcDataConfigZhj[FirstIndex + No].nArrLen = atoi(mxmlElementGetAttr(node, "ArrLen"));

        No++;
	}

	APP104_FMT_DEBUG("yc point table: No=%d \n", No);
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, arrindex=%u, Addr=%08x, Type=%d, Rate=%d, DealZoneAddr=%04x, DeadZoneValue=%f, LogicId=%d, InfoNum=%x, DbCenterOAD=%08x, suboadnum = %d, index=%d, FullValue=%f, DealType=%08x, nArrLen=%d\n", 
								i, FirstIndex+i, g_aYcDataConfigZhj[FirstIndex+i].nObjAddrU32, g_aYcDataConfigZhj[FirstIndex+i].nSignalType, 
								g_aYcDataConfigZhj[FirstIndex+i].nReportRatio, g_aYcDataConfigZhj[FirstIndex+i].nDeadZoneAddr, g_aYcDataConfigZhj[FirstIndex+i].fDeadZoneValue,
								g_aYcDataConfigZhj[FirstIndex+i].nLogicId, g_aYcDataConfigZhj[FirstIndex+i].nInfoNum, g_aYcDataConfigZhj[FirstIndex+i].nDbCenterOAD, 
								g_aYcDataConfigZhj[FirstIndex+i].nOadColsNum, g_aYcDataConfigZhj[FirstIndex+i].nItemIndex, g_aYcDataConfigZhj[FirstIndex+i].fFullValue, 
								g_aYcDataConfigZhj[FirstIndex + No].nDealType, g_aYcDataConfigZhj[FirstIndex + No].nArrLen);
		
		if(g_aYcDataConfigZhj[FirstIndex+i].nOadColsNum > 0 )
		{
			int j=0;
			for(j=0;j<g_aYcDataConfigZhj[FirstIndex+i].nOadColsNum;j++)
			{
				APP104_FMT_DEBUG("OadCol = %08x\n", g_aYcDataConfigZhj[FirstIndex+i].aOadCols[j]);
			}
		}

	}

	mxmlDelete(tree);

	*outNo = No;

    //更新下一个设备第1个遥测信号的索引(索引从0开始)
    uint16 nextTyIndex = 0;
    uint16 nextGrIndex = 0;
    uint16 nextSeqIndex = 0;
    int getRet = get_next_equip_attr_zhj(equipType-1, equipGroup, equipNo-1, &nextTyIndex, &nextGrIndex, &nextSeqIndex);
    if(getRet > 0)
    {
        g_aYcDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex] = FirstIndex + No;
        APP104_FMT_DEBUG("下一个设备 nextTyIndex=%d, nextGrIndex=%d, nextSeqIndex=%d, 信号索引=%u \n", 
            nextTyIndex, nextGrIndex, nextSeqIndex, g_aYcDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex]);
    }
    else
    {
        APP104_FMT_DEBUG("get_next_equip_attr_zhj() 获取失败! \n");
    }

    //更新累计数量
    g_nTotalYcNumZhj += No;
    APP104_FMT_DEBUG("目前累计 g_nTotalYcNumZhj=%u \n", g_nTotalYcNumZhj);

	return TRUE;
}

/*******************************************************************************
* 函数名称: pw_readParafromXML_zhj
* 函数功能: 加载参数点表
* 输入参数: uint8 equipType    设备类型
            uint8 equipGroup   设备组别
            uint16 equipNo     组内序号
* 输出参数: uint16 *outNo          信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int pw_readParafromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo)
{
    if((equipType < 1) || (equipNo < 1))
    {
        APP104_FMT_DEBUG("pw_readParafromXML_zhj() 参数非法! equipType=%d, equipNo=%d \n", equipType, equipNo);
    }
    
    //点表文件名
    char filename[50] = {0};
    sprintf(filename, "zhj_%d_%d_%d_para.xml", equipType, equipGroup, equipNo);
    APP104_FMT_DEBUG("\n\n定值参数点表开始加载...设备类型:%d, 组别:%d, 编号:%d, 文件名:%s \n", equipType, equipGroup, equipNo, filename);
    
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;

	printf("start read para.xml...\n");

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
        APP104_FMT_DEBUG("xml file 打开失败! \n");
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

    //取出当前设备第1个定值参数的索引(索引从0开始)
    uint32 FirstIndex = g_aParaValueIndexZhj[equipType-1][equipGroup][equipNo-1];
    APP104_FMT_DEBUG("当前信号索引 FirstIndex=%u \n", FirstIndex);

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

		g_aParaValueInitZhj[FirstIndex + No].nEqType = equipType;
        g_aParaValueInitZhj[FirstIndex + No].nEqGroup = equipGroup;
        g_aParaValueInitZhj[FirstIndex + No].nEquipNo = equipNo;
        
		char addr[4] = {0};
		char Rverseaddr[4] = {0};
		char data[4] = {0};
		char Rversedata[4] = {0};

        commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 8, 0);
		memcpy_rev(Rverseaddr, addr, 4);
		g_aParaValueInitZhj[FirstIndex + No].nObjAddrU32 = MAKE_LONG(Rverseaddr);

		//g_aParaValueInit[No]. = mxmlElementGetAttr(node, "Name");
		if(strcmp(mxmlElementGetAttr(node, "Writable"), "1") == 0)
			g_aParaValueInitZhj[FirstIndex + No].bModifiable = TRUE;
		else
			g_aParaValueInitZhj[FirstIndex + No].bModifiable = FALSE;
        
		if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_NONE") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_NONE;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_BOOL") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_BOOL;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT8") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_INT8;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT8") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_UINT8;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT16") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_INT16;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT16") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_UINT16;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT32") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_INT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT32") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_UINT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_INT64") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_INT64;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_UINT64") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_UINT64;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_FLOAT32") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_FLOAT32;
		else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_FLOAT64") == 0)
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_FLOAT64;
        else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_STRING") == 0)
        {
            //字符串, 字符串和八位位串虽然标记TAG都是4, 但是点表中处理方式不同
            g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_STRING;
            g_aParaValueInitZhj[FirstIndex + No].nArrLen = atoi(mxmlElementGetAttr(node, "ArrLen"));
            //g_aParaValueInitZhj[FirstIndex + No].nArrLen = strlen(mxmlElementGetAttr(node, "ArrValue"));
            if(g_aParaValueInitZhj[FirstIndex + No].nArrLen > 0)
            {
                if(g_aParaValueInitZhj[FirstIndex + No].nArrLen > MAX_ARR_VALUE_LEN)
                    g_aParaValueInitZhj[FirstIndex + No].nArrLen = MAX_ARR_VALUE_LEN;

                memcpy((char *)&g_aParaValueInitZhj[FirstIndex + No].aArrValue[0], mxmlElementGetAttr(node, "ArrValue"), 
                    g_aParaValueInitZhj[FirstIndex + No].nArrLen);

                APP104_FMT_DEBUG("No=%d, 字符串内容=%s \n", No, (char *)&g_aParaValueInitZhj[FirstIndex + No].aArrValue[0]);
            }
        }	
        else if(strcmp(mxmlElementGetAttr(node, "TagType"), "TAG_OCTSTRING") == 0)
        {
            //八位位串, 字符串和八位位串虽然标记TAG都是4, 但是点表中处理方式不同
            g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_OCTSTRING_ZHJ;
            g_aParaValueInitZhj[FirstIndex + No].nArrLen = atoi(mxmlElementGetAttr(node, "ArrLen"));
            //g_aParaValueInitZhj[FirstIndex + No].nArrLen = strlen(mxmlElementGetAttr(node, "ArrValue"))/2;
            if(g_aParaValueInitZhj[FirstIndex + No].nArrLen > 0)
            {
                if(g_aParaValueInitZhj[FirstIndex + No].nArrLen > MAX_ARR_VALUE_LEN)
                    g_aParaValueInitZhj[FirstIndex + No].nArrLen = MAX_ARR_VALUE_LEN;

                commfun_ASC2BCD((char *)&g_aParaValueInitZhj[FirstIndex + No].aArrValue[0], mxmlElementGetAttr(node, "ArrValue"), 
                    2*g_aParaValueInitZhj[FirstIndex + No].nArrLen, 0);
            }
        }
		else
			g_aParaValueInitZhj[FirstIndex + No].nTagType = TAG_NONE;
		
		g_aParaValueInitZhj[FirstIndex + No].nDataLen = atoi(mxmlElementGetAttr(node, "DataLen"));
		//printf("Item Valid = %s\n", mxmlElementGetAttr(node, "Valid"));
		if(strcmp(mxmlElementGetAttr(node, "Valid"), "1") == 0)
			g_aParaValueInitZhj[FirstIndex + No].bValid = TRUE;
		else
			g_aParaValueInitZhj[FirstIndex + No].bValid = FALSE;
		memset(g_aParaValueInitZhj[FirstIndex + No].unValue.arrayV, '\0', MAX_FIXED_VALUE_LEN);

        if(TAG_NONE == g_aParaValueInitZhj[FirstIndex + No].nTagType ||
            TAG_FLOAT32 == g_aParaValueInitZhj[FirstIndex + No].nTagType ||
            TAG_FLOAT64 == g_aParaValueInitZhj[FirstIndex + No].nTagType)
        {
            g_aParaValueInitZhj[FirstIndex + No].fDefaultValue = atof(mxmlElementGetAttr(node, "DefaultValue"));
            memcpy(g_aParaValueInitZhj[FirstIndex + No].unValue.arrayV, &g_aParaValueInitZhj[FirstIndex + No].fDefaultValue, 
                    g_aParaValueInitZhj[FirstIndex + No].nDataLen);
        }
        else
        {
            g_aParaValueInitZhj[FirstIndex + No].nDefaultValueI64 = atoi(mxmlElementGetAttr(node, "DefaultValue"));
            memcpy(g_aParaValueInitZhj[FirstIndex + No].unValue.arrayV, &g_aParaValueInitZhj[FirstIndex + No].nDefaultValueI64, 
                    g_aParaValueInitZhj[FirstIndex + No].nDataLen);
        }
		g_aParaValueInitZhj[FirstIndex + No].fMinValue = atof(mxmlElementGetAttr(node, "MinValue"));
		g_aParaValueInitZhj[FirstIndex + No].fMaxValue = atof(mxmlElementGetAttr(node, "MaxValue"));

		commfun_ASC2BCD(data, mxmlElementGetAttr(node, "DealType"), 8, 0);
		memcpy_rev(Rversedata, data, 4);
		g_aParaValueInitZhj[FirstIndex + No].nDealType = MAKE_LONG(Rversedata);

        g_aParaValueInitZhj[FirstIndex + No].nUpdateFlag = atoi(mxmlElementGetAttr(node, "UpdateFlag"));
        g_aParaValueInitZhj[FirstIndex + No].nUpdateArea = atoi(mxmlElementGetAttr(node, "UpdateArea"));
        
		No++;
	}

	APP104_FMT_DEBUG("para point table: No=%d \n", No);
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, arrindex=%u, Addr=%08x, Writable=%d, TagType=%d, DataLen=%d, Valid=%d, DefaultValue=%f, MinValue=%f, MaxValue=%f, nDealType=%08x, nUpdateFlag=%d, nUpdateArea=%d, nArrLen=%d \n", 
								i, FirstIndex+i, g_aParaValueInitZhj[FirstIndex+i].nObjAddrU32, g_aParaValueInitZhj[FirstIndex+i].bModifiable, 
								g_aParaValueInitZhj[FirstIndex+i].nTagType, g_aParaValueInitZhj[FirstIndex+i].nDataLen, 
								g_aParaValueInitZhj[FirstIndex+i].bValid, g_aParaValueInitZhj[FirstIndex+i].fDefaultValue, 
								g_aParaValueInitZhj[FirstIndex+i].fMinValue, g_aParaValueInitZhj[FirstIndex+i].fMaxValue, 
								g_aParaValueInitZhj[FirstIndex+i].nDealType, 
								g_aParaValueInitZhj[FirstIndex+i].nUpdateFlag, g_aParaValueInitZhj[FirstIndex+i].nUpdateArea, 
								g_aParaValueInitZhj[FirstIndex+i].nArrLen);
        if(g_aParaValueInitZhj[FirstIndex+i].nArrLen > 0)
        {
            APP104_BUF_DEBUG(g_aParaValueInitZhj[FirstIndex+i].aArrValue, g_aParaValueInitZhj[FirstIndex+i].nArrLen, "aArrValue: ");
        }
		
	}

	mxmlDelete(tree);

	*outNo = No;

    //更新下一个设备第1个定值参数的索引(索引从0开始)
    uint16 nextTyIndex = 0;
    uint16 nextGrIndex = 0;
    uint16 nextSeqIndex = 0;
    int getRet = get_next_equip_attr_zhj(equipType-1, equipGroup, equipNo-1, &nextTyIndex, &nextGrIndex, &nextSeqIndex);
    if(getRet > 0)
    {
        g_aParaValueIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex] = FirstIndex + No;
        APP104_FMT_DEBUG("下一个设备 nextTyIndex=%d, nextGrIndex=%d, nextSeqIndex=%d, 信号索引=%u \n", 
            nextTyIndex, nextGrIndex, nextSeqIndex, g_aParaValueIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex]);
    }
    else
    {
        APP104_FMT_DEBUG("get_next_equip_attr_zhj() 获取失败! \n");
    }

    //更新累计数量
    g_nTotalParaNumZhj += No;
    APP104_FMT_DEBUG("目前累计 g_nTotalParaNumZhj=%u \n", g_nTotalParaNumZhj);

	return TRUE;
}

/*******************************************************************************
* 函数名称: pw_readfaultfromXML_zhj
* 函数功能: 加载故障事件点表
* 输入参数: uint8 equipType    设备类型
            uint8 equipGroup   设备组别
            uint16 equipNo     组内序号
* 输出参数: uint16 *outNo          信号个数
* 返 回 值: 0:失败； 1:成功
*******************************************************************************/
int pw_readfaultfromXML_zhj(uint8 equipType, uint8 equipGroup, uint16 equipNo, uint16 *outNo)
{
    if((equipType < 1) || (equipNo < 1))
    {
        APP104_FMT_DEBUG("pw_readfaultfromXML_zhj() 参数非法! equipType=%d, equipNo=%d \n", equipType, equipNo);
    }
    
    //点表文件名
    char filename[50] = {0};
    sprintf(filename, "zhj_%d_%d_%d_fault.xml", equipType, equipGroup, equipNo);
    APP104_FMT_DEBUG("\n\n故障事件点表开始加载...设备类型:%d, 组别:%d, 编号:%d, 文件名:%s \n", equipType, equipGroup, equipNo, filename);
    
	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	mxml_node_t *subnode = NULL;

	printf("start read fault.xml...\n");

	fp = fopen(filename, "r");
	if(fp == NULL)
	{
		printf("xml file not open");
        APP104_FMT_DEBUG("xml file 打开失败! \n");
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

    //取出当前设备第1个故障事件的索引(索引从0开始)
    uint32 FirstIndex = g_aFaultDataIndexZhj[equipType-1][equipGroup][equipNo-1];
    APP104_FMT_DEBUG("当前信号索引 FirstIndex=%u \n", FirstIndex);

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

		g_aFaultDataConfigZhj[FirstIndex + No].nEqType = equipType;
        g_aFaultDataConfigZhj[FirstIndex + No].nEqGroup = equipGroup;
        g_aFaultDataConfigZhj[FirstIndex + No].nEquipNo = equipNo;
        
		char addr[4] = {0};
        char Rverseaddr[4] = {0};
        char data[4] = {0};
        char Rversedata[4] = {0};

        commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "Addr"), 8, 0);
        memcpy_rev(Rverseaddr, addr, 4);
        g_aFaultDataConfigZhj[FirstIndex + No].nObjAddrU32 = MAKE_LONG(Rverseaddr);

        //g_aFaultDataConfigZhj[FirstIndex + No].szObjName = (char *)mxmlElementGetAttr(node, "Name");
		g_aFaultDataConfigZhj[FirstIndex + No].nEuipType = atoi(mxmlElementGetAttr(node, "EquipType"));

        if(strcmp(mxmlElementGetAttr(node, "Type"), "M_FT_NA_ZHJ_1") == 0)
			g_aFaultDataConfigZhj[FirstIndex + No].nSignalType = M_FT_NA_ZHJ_1;
		else 
			g_aFaultDataConfigZhj[FirstIndex + No].nSignalType = M_FT_NA_ZHJ_1;

        commfun_ASC2BCD(data, mxmlElementGetAttr(node, "MainOAD"), 8, 0);
		memcpy_rev(Rversedata, data, 4);

        g_aFaultDataConfigZhj[FirstIndex + No].nDbCenterOAD = MAKE_LONG(Rversedata);
		g_aFaultDataConfigZhj[FirstIndex + No].nReportNum = atoi(mxmlElementGetAttr(node, "ReportNum"));
		g_aFaultDataConfigZhj[FirstIndex + No].nItemIndex = atoi(mxmlElementGetAttr(node, "Index"));
		g_aFaultDataConfigZhj[FirstIndex + No].nRptDataSrc = atoi(mxmlElementGetAttr(node, "DataSrc"));

        commfun_ASC2BCD(addr, mxmlElementGetAttr(node, "FaultYxAddr"), 8, 0);
        memcpy_rev(Rverseaddr, addr, 4);
        g_aFaultDataConfigZhj[FirstIndex + No].nFaultYxAddr = MAKE_LONG(Rverseaddr);

        if(g_aFaultDataConfigZhj[FirstIndex + No].nReportNum > MAX_FAULT_SIGNAL_NUM)
        {
            g_aFaultDataConfigZhj[FirstIndex + No].nReportNum = MAX_FAULT_SIGNAL_NUM;
        }
        g_aFaultDataConfigZhj[FirstIndex + No].nReportYxNum = atoi(mxmlElementGetAttr(node, "YxNum"));
        g_aFaultDataConfigZhj[FirstIndex + No].nReportYcNum = atoi(mxmlElementGetAttr(node, "YcNum"));
        g_aFaultDataConfigZhj[FirstIndex + No].nReportYcTiNum = atoi(mxmlElementGetAttr(node, "YcTiNum"));
        g_aFaultDataConfigZhj[FirstIndex + No].nReportEnNum = atoi(mxmlElementGetAttr(node, "EnNum"));
        g_aFaultDataConfigZhj[FirstIndex + No].nReportStrNum = atoi(mxmlElementGetAttr(node, "StrNum"));
        g_aFaultDataConfigZhj[FirstIndex + No].nReportParaNum = atoi(mxmlElementGetAttr(node, "ParaNum"));
        
        if(g_aFaultDataConfigZhj[FirstIndex + No].nReportNum > 0)
		{
			subnode = mxmlFindElement(node, tree, "SubItem", NULL, NULL, MXML_DESCEND);
			if(subnode == NULL)
			{
				printf("Unable to find second <SubItem> element in XML tree.\n");
				return FALSE;
			}

            int i=0;
			for(i=0;i<g_aFaultDataConfigZhj[FirstIndex + No].nReportNum;i++)
			{
				subnode = mxmlFindElement(subnode, tree, "SubOAD", NULL, NULL, MXML_DESCEND);
				if(subnode == NULL)
				{
					printf("Unable to find second <SubOAD> element in XML tree.\n");
					return FALSE;
				}
                
                commfun_ASC2BCD(addr, mxmlElementGetAttr(subnode, "ReportAddr"), 8, 0);
                memcpy_rev(Rverseaddr, addr, 4);
                g_aFaultDataConfigZhj[FirstIndex + No].aReportAddrU32[i] = MAKE_LONG(Rverseaddr);
			}
		}

		No++;
	}

	APP104_FMT_DEBUG("fault point table: No=%d \n", No);
	int i=0;
	for(i=0;i<No;i++)
	{
		APP104_FMT_DEBUG("i=%d, arrindex=%u, Addr=%08x, Type=%d, EquipType=%d, FaultYxAddr=%08x, ReportNum=%d, YxNum=%d, YcNum=%d, YcTiNum=%d, EnNum=%d, StrNum=%d, ParaNum=%d, DbCenterOAD=%08x, Index=%d, DataSrc=%d\n", 
								i, FirstIndex+i, g_aFaultDataConfigZhj[FirstIndex+i].nObjAddrU32, g_aFaultDataConfigZhj[FirstIndex+i].nSignalType, 
								g_aFaultDataConfigZhj[FirstIndex+i].nEuipType, 
								g_aFaultDataConfigZhj[FirstIndex+i].nFaultYxAddr, g_aFaultDataConfigZhj[FirstIndex+i].nReportNum,
								g_aFaultDataConfigZhj[FirstIndex+i].nReportYxNum, g_aFaultDataConfigZhj[FirstIndex+i].nReportYcNum,
								g_aFaultDataConfigZhj[FirstIndex+i].nReportYcTiNum, g_aFaultDataConfigZhj[FirstIndex+i].nReportEnNum,
								g_aFaultDataConfigZhj[FirstIndex+i].nReportStrNum, g_aFaultDataConfigZhj[FirstIndex+i].nReportParaNum,
								g_aFaultDataConfigZhj[FirstIndex+i].nDbCenterOAD, 
								g_aFaultDataConfigZhj[FirstIndex+i].nItemIndex, g_aFaultDataConfigZhj[FirstIndex+i].nRptDataSrc);
		if(g_aFaultDataConfigZhj[FirstIndex+i].nReportNum > 0 )
		{
			int j=0;
			for(j=0;j<g_aFaultDataConfigZhj[FirstIndex+i].nReportNum;j++)
			{
				APP104_FMT_DEBUG("OadCol = %08x\n", g_aFaultDataConfigZhj[FirstIndex+i].aReportAddrU32[j]);
			}
		}
	}

	mxmlDelete(tree);

	*outNo = No;

    //更新下一个设备第1个故障事件的索引(索引从0开始)
    uint16 nextTyIndex = 0;
    uint16 nextGrIndex = 0;
    uint16 nextSeqIndex = 0;
    int getRet = get_next_equip_attr_zhj(equipType-1, equipGroup, equipNo-1, &nextTyIndex, &nextGrIndex, &nextSeqIndex);
    if(getRet > 0)
    {
        g_aFaultDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex] = FirstIndex + No;
        APP104_FMT_DEBUG("下一个设备 nextTyIndex=%d, nextGrIndex=%d, nextSeqIndex=%d, 信号索引=%u \n", 
            nextTyIndex, nextGrIndex, nextSeqIndex, g_aFaultDataIndexZhj[nextTyIndex][nextGrIndex][nextSeqIndex]);
    }
    else
    {
        APP104_FMT_DEBUG("get_next_equip_attr_zhj() 获取失败! \n");
    }

    //更新累计数量
    g_nTotalFaultNumZhj += No;
    APP104_FMT_DEBUG("目前累计 g_nTotalFaultNumZhj=%u \n", g_nTotalFaultNumZhj);

	return TRUE;
}




