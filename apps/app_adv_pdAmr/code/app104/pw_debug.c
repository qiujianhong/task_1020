/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/

#include "pw_debug.h"

//打印buff
void buffer_print(uint8 *buf, int len)
{
    int i;

    for (i=0; i < len; i++)
    {
        printx("%02x ", buf[i]);
    }
    
    if (len > 0)
    {
        printx("\n");
    }
}

//打印buff（数据中心专用）
void buffer_print_db(uint8 *buf, int len)
{
    int i;

    for (i=0; i < len; i++)
    {
        printdb("%02x ", buf[i]);
    }
    
    if (len > 0)
    {
        printdb("\n");
    }
}


//报文buff转换为string打印，没有空格
void buff_no_space_to_str(char *buf, int len, int maxlen, char *comm_log)
{
  	int i;
  	char *p;
  
  	p = comm_log;
  
  	if (len < maxlen)
  	{
    	for (i=0; i<len; i++)
      	{	
      		sprintf(p+2*i, "%02x", buf[i]);   
    	}
  	}
  	else
  	{
    	for (i=0; i<maxlen; i++)
      	{	
      		sprintf(p+2*i, "%02x", buf[i]);
    	}
  	}
}

//报文buff转换为string打印，添加空格
void buff_space_to_str(char *buf, int len, int maxlen, char *comm_log)
{
  	int i;
  	char *p;
  
  	p = comm_log;
	
  	if(len <= 0)
  	{
		return;
	}
        
  	if (len < maxlen)
  	{
    	for (i=0; i<len; i++)
      	{	
      		sprintf(p+3*i, "%02x ", buf[i]);
    	}
  	}
  	else
  	{
    	for (i=0; i<maxlen; i++)
      	{	
      		sprintf(p+3*i, "%02x ", buf[i]);
    	}
  	}
}

//将buff保存至日志
void log_buff_detail_save(uint8 *buf, int len, char *path)
{
	int  nMaxLen = 2000;
	char aCommLog[65535];

	memset(aCommLog, 0, sizeof(aCommLog));
	buff_space_to_str((char *)buf, len, nMaxLen, aCommLog);
	writelog(path, " len:%d, buf:%s \n\n", len, aCommLog);
}


/*******************************************************************************
* 函数名称:	bcd_time_print
* 函数功能:	打印bcdtime
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void bcd_time_print(DATE_TIME_T stCurBcdTime)
{
	FRAME104_FMT_TRACE("******当前时间g_stCurBcdTime YY-MM-DD hh:nn:ss = %02x-%02x-%02x  %02x:%02x:%02x****** \n", 
		stCurBcdTime.Year, stCurBcdTime.Month, stCurBcdTime.Day, 
		stCurBcdTime.Hour, stCurBcdTime.Minute, stCurBcdTime.Second);
}


/*******************************************************************************
* 函数名称:	bcd_time_print_db
* 函数功能:	打印bcdtime（数据中心专用）
* 输入参数:	无
* 输出参数:	无
* 返 回 值:	无
*******************************************************************************/
void bcd_time_print_db(DATE_TIME_T stCurBcdTime)
{
	DB104_FMT_TRACE("******当前时间g_stCurBcdTime YY-MM-DD hh:nn:ss = %02x-%02x-%02x  %02x:%02x:%02x****** \n", 
		stCurBcdTime.Year, stCurBcdTime.Month, stCurBcdTime.Day, 
		stCurBcdTime.Hour, stCurBcdTime.Minute, stCurBcdTime.Second);
}













