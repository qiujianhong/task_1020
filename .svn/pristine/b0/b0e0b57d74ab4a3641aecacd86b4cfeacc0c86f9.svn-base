/****************************************************************************
Copyright (c) 2006-2009,南京新联电子股份有限责任公司
         All rights reserved.
程序名称: update_ex.h
功    能：升级程序和其他程序交互的时候使用的数据字典
开 发 人：顾君
开发时间：2009-05-26
版本历史:
        2009-05-26 ,v1.0 : New              
****************************************************************************/

#ifndef _UPDATE_EX_H
#define _UPDATE_EX_H


typedef struct  
{
	uint16 	updateex_id;    	//-升级交互的ID-
	int 	updateex_len;    	//-交互ID的长度-
}__attribute__ ((packed)) updateex_info_t;

extern updateex_info_t  updateex_info[];

//--- 数据项ID定义 ---
enum updateex_data 
{
	NEED_IUART = 0,			// 1字节接口板升级请求标志
	IUART_OK,				// 1字节接口板允许升级标志
	NEED_AUART,				// 1字节交采版升级请求标志
	AUART_OK,				// 1字节交采版允许升级标志
	Update_State,			// 1字节升级状态
	Frame_Count,			// 2字节无符号
	Cur_Frame,				// 2字节无符号
   	UPDATEEX_MAX_ID,
};
int WriteUpdateEXData(uint16 id, const char *buf);
int ReadUpdateEXData(uint16 id, char *buf);

#define updateex_id_len(index)   (updateex_info[index].updateex_len)		//取得升级交互ID的长度

/***************************************************************************************/
#endif
