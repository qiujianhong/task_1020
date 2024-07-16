/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief：  性能测试线程
* @date：    2020-4-21
* @history：
*********************************************************************
*/

#include "xlChannel.h"
#include "baseCommon.h"
#include "db_api.h"
#include "db_struct.h"
#include "appmsg_split.h"
#include "comm_log.h"
#include "dataCommon.h"

#include "mqtt_proc.h"
#include "client_id.h"
#include "test_thread.h"

#include "db_rw_interface.h"


#include <sys/time.h>  
#include <sys/statfs.h> 
#include "framecomm.h"


// 对外消息 客户端句柄

DB_CLIENT g_dbClients[MAX_THREAD];
MSG_HEAD1_t     g_msghead1;
MSG_HEAD2_t     g_msghead2;
MSG_HEAD3_t     g_msghead3;
MSG_INFO_t      g_ramhead[4];
MSG_BIG_DATA_t  g_bigdatahead;
RW_MSG_HEAD_t   g_rw_msg_head;
uint16			g_minErrorNum;
uint16 			g_dayErrorNum;
uint16 			g_readrecordErrorNum;
DateTimeHex_t   startwriterecordtime;
DateTimeHex_t 	TestRunStartTime;
bool 			FirstRun;
void *psendMsg = NULL;
//static char array[2];
bool dayFrozen;
Readrecord 		g_outrecord;
uint32 			timelimits1;
uint32 			timelimits2;


char gStr1[128];  //每个数据占据2字节
Para  para;
MESSAGE_INFO_T *pInMsgs;
float avg = 0;

int filenum = 0;
/*
    * 函数名：         GetIniKeyString
    * 入口参数：        title
    *                      配置文件中一组数据的标识
    *                  key
    *                      这组数据中要读出的值的标识
    *                  filename
    *                      要读取的文件路径
    * 返回值：         找到需要查的值则返回正确结果 0
    *                  否则返回-1
    */
int GetIniKeyString(char *title, char *key, char *filename, char *buf)
{
    FILE *fp;
    int  flag = 0;
    char sTitle[64], *wTmp;
    char sLine[1024] = { 0 };
    sprintf(sTitle, "[%s]", title);

    if (NULL == (fp = fopen(filename, "r"))) {
        perror("fopen");
        return -1;
    }
    while (NULL != fgets(sLine, 1024, fp)) {
        // 这是注释行  
        if (0 == strncmp("//", sLine, 2)) continue;
        if ('#' == sLine[0])              continue;
        wTmp = strchr(sLine, '=');
        if ((NULL != wTmp) && (1 == flag)) {
            if (0 == strncmp(key, sLine, strlen(key))) { // 长度依文件读取的为准  
                sLine[strlen(sLine) - 1] = '\0';
                fclose(fp);
                while (*(wTmp + 1) == ' ') {
                    wTmp++;
                }
                strcpy(buf, wTmp + 1);
                return 0;
            }
        }
        else {
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) { // 长度依文件读取的为准  
                flag = 1; // 找到标题位置  
            }
        }
    }
    fclose(fp);
    return -1;
}


/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void write_normal_data(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int j = 0;
    NOMAL_OAD_T     nomalOAD;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    ret = gettimeofday(&tv1, NULL);
    for (j = 1; j <= num; j++)
    {
        nomalOAD.logicId = 0;
        nomalOAD.infoNum = j;
        nomalOAD.oad.value = oad;
        if(oad == 0x60000200)
            nomalOAD.classtag = CLASS_DATA_INIT;
        else
            nomalOAD.classtag = CLASS_DATA_NORMAL;
        memcpy(pstrbuf, &j, 4);
        ret = db_write_nomaldata( threadid, &nomalOAD, pstrbuf, buflen);
        if (ret != ERR_OK)
        {
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }
//        usleep(1000);
    }
    ret = gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 - num;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}

/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void write_normal_datas(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int i,j = 0;
    NOMAL_OAD_DATE_T    nomalOADs[20];
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    ret = gettimeofday(&tv1, NULL);
    for (j = 0; j < num; j++)
    {
        for (i = 0; i <20; i++)
        {
            nomalOADs[i].oad.value = oad+(j*20)+i;
            if(oad == 0x60000200)
                nomalOADs[i].classtag = CLASS_DATA_INIT;
            else
                nomalOADs[i].classtag = CLASS_DATA_NORMAL;
            
            nomalOADs[i].datalen = buflen;
            
        }
        ret = db_write_nomaldata_more(threadid, 0, 0, nomalOADs, 20);
        if (ret != ERR_OK)
        {
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }
//        usleep(1000);
    }
    ret = gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 - num;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}


/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void read_normal_data(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int j = 0;
    NOMAL_OAD_T     nomalOAD;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    uint32 outlen = 0;
    uint8 outbuf[MAX_MSG_BUF_LEN] = { 0 };

    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    int tmp = 0;

    gettimeofday(&tv1, NULL);
    for (j = 1; j <= num; j++)
    {
        nomalOAD.logicId = 0;
        nomalOAD.infoNum = j;
        nomalOAD.oad.value = oad;
        if(oad == 0x60000200)
            nomalOAD.classtag = CLASS_DATA_INIT;
        else
            nomalOAD.classtag = CLASS_DATA_NORMAL;
        ret = db_read_nomaldata(threadid, &nomalOAD, buflen, outbuf, &outlen);
        memcpy(&tmp, outbuf, 4);
        if (ret != ERR_OK || outlen != buflen || tmp!=j)
        {
            outmsg->err_num++;
            printf("db_read_nomaldata [%08x] err ret= %d, outlen=%d or buf[%d]!=%d\n", oad, ret, outlen, tmp, j);
//            exit(EXIT_FAILURE);
        }
        else
        {
            outmsg->secc_num++;
        }
//        usleep(1000);
    }
    gettimeofday(&tv2, NULL);
    
    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;

    outmsg->total_time = (((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000);
    outmsg->err_num = num - outmsg->secc_num;
    return;
}

/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void read_normal_data_by_oad(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int j = 0;
    NOMAL_OAD_T     nomalOAD;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    uint32 outlen = 0;
    uint8 outbuf[MAX_MSG_BUF_LEN] = { 0 };

    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    gettimeofday(&tv1, NULL);
    for (j = 0; j < num; j++)
    {
        nomalOAD.logicId = 0;
        nomalOAD.infoNum = 0;
        nomalOAD.oad.value = oad + j;
        if(oad == 0x60000200)
            nomalOAD.classtag = CLASS_DATA_INIT;
        else
            nomalOAD.classtag = CLASS_DATA_NORMAL;
        ret = db_read_nomaldata(threadid, &nomalOAD, buflen, outbuf, &outlen);
        if (ret != ERR_OK || outlen != buflen)
        {
            outmsg->err_num++;
//            exit(EXIT_FAILURE);
        }
        else
        {
            outmsg->secc_num++;
        }
//        usleep(1000);
    }
    gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 -num;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}

/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void read_normal_data_record(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int                ret = ERR_NORMAL;
    uint32             i = 0;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;

    outmsg->secc_num = 0;
    outmsg->err_num = 0;


    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = num + 1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    //不需要附加校验
    stepReadRecord.MAC.nNum = 0;
    gettimeofday(&tv1, NULL);
    ret = db_read_recorddata_start(threadid, &inReadRecord, &handle, &recNum);
    if ((ret != ERR_OK) || (recNum == 0))
    {
        outmsg->secc_num = 0;
//        exit(EXIT_FAILURE);
    }
    else
    {
        for (i = 0; i < recNum; i++)
        {
            ret = db_read_recorddata_step(threadid, handle, &stepReadRecord, &stepRecordOut);
            if (ret != ERR_OK)
            {
                outmsg->err_num++;
                printf("read_normal_data_record db_read_record_step err = %d!\n", ret);
                continue;
            }

            //检查数据有效性
            if ((stepRecordOut.recordnum != 1)
                || (stepRecordOut.record[0].road.oadCols.oad[0].value != 0x60000200)
                || (stepRecordOut.record[0].outDatalen != sizeof(OOP_METER_T)))
            {
                outmsg->err_num++;
                printf("read_normal_data_record check err!\n");
                continue;
//                exit(EXIT_FAILURE);
            }

            outmsg->secc_num++;
//            usleep(1000);

        }
    }
    db_read_recorddata_end(threadid, handle);

    gettimeofday(&tv2, NULL);
    
    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 - outmsg->secc_num;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}


void write_record_data_by_infoNum1(uint8 threadid, uint16 infoNum, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 Starttime, uint32 Endtime, int erroNo)
{
	int ret = ERR_NORMAL;
    //int j = 0;
    WRITE_RECORD_T recInData;
    uint32 stroeNo = 0;
    erroNo = 0;
	
	//for (j = 1; j <= num; j++)
	{
		recInData.recordOAD.logicId = 0;
		recInData.recordOAD.infoNum = infoNum;
		if (oadMain == 0)
		{
			recInData.recordOAD.optional = 0;
			recInData.recordOAD.road.oadMain.value = 0;
		}
		else
		{
			recInData.recordOAD.optional = 1;
			recInData.recordOAD.road.oadMain.value = oadMain;
			}
	
		recInData.recordOAD.road.oadCols.nNum = 0x01;
		recInData.recordOAD.road.oadCols.oad[0].value = oadCols;
		recInData.recordOAD.classtag = CLASS_DATA_COLLEC;
		recInData.MAC.nNum = 6;
		recInData.MAC.value[0] = 0x01;
		recInData.MAC.value[1] = 0x02;
		recInData.MAC.value[2] = 0x03;
		recInData.MAC.value[3] = 0x04;
		recInData.MAC.value[4] = 0x05;
		recInData.MAC.value[5] = 0x06;
	
		recInData.colStartTm = Starttime;
		recInData.colEndTm = Endtime;
		recInData.colStoreTm = Starttime;
	
		recInData.inDatalen = buflen;
		memcpy(recInData.inDataBuf, pstrbuf, recInData.inDatalen);
		ret = db_write_recorddata(threadid, &recInData, &stroeNo);
		if (ret != ERR_OK)
		{
			printf("record_data_write[%08x] error %d\n", oadCols, ret);
			erroNo++;
		}
		else
		{
			//tmsg->secc_num++;
			}
	//		  usleep(1000);
		}
	
	return;
}


/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void write_record_data_by_infoNum(uint8 threadid, uint16 infoNum, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    int j = 0;
    WRITE_RECORD_T recInData;
    uint32 stroeNo = 0;

    outmsg->secc_num = 0;
    outmsg->err_num = 0;


    ret = gettimeofday(&tv1, NULL);
    for (j = 1; j <= num; j++)
    {

        recInData.recordOAD.logicId = 0;
        recInData.recordOAD.infoNum = infoNum;
        if (oadMain == 0)
        {
            recInData.recordOAD.optional = 0;
			recInData.recordOAD.road.oadMain.value = 0;
        }
        else
        {
            recInData.recordOAD.optional = 1;
            recInData.recordOAD.road.oadMain.value = oadMain;
        }

        recInData.recordOAD.road.oadCols.nNum = 0x01;
        recInData.recordOAD.road.oadCols.oad[0].value = oadCols;
        recInData.recordOAD.classtag = CLASS_DATA_COLLEC;
        recInData.MAC.nNum = 6;
        recInData.MAC.value[0] = 0x01;
        recInData.MAC.value[1] = 0x02;
        recInData.MAC.value[2] = 0x03;
        recInData.MAC.value[3] = 0x04;
        recInData.MAC.value[4] = 0x05;
        recInData.MAC.value[5] = 0x06;

        if (savetime == 0)
        {
            recInData.colStartTm = 0;
            recInData.colEndTm = 0;
            recInData.colStoreTm = 0;
        }
        else
        {
            recInData.colStartTm = savetime + (j - 1) * 60;
            recInData.colEndTm = savetime + (j - 1) * 60;
            recInData.colStoreTm = savetime + (j - 1) * 60;
        }

        recInData.inDatalen = buflen;
        memcpy(pstrbuf, &recInData.colStartTm, 4);
        memcpy(recInData.inDataBuf, pstrbuf, recInData.inDatalen);
        ret = db_write_recorddata(threadid, &recInData, &stroeNo);
        if (ret != ERR_OK)
        {
            printf("record_data_write[%08x] error %d\n", oadCols, ret);
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }
//        usleep(1000);
    }
    ret = gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;

    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 - num;
    outmsg->err_num = num - outmsg->secc_num;
    return;

}


/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void write_record_data(uint8 threadid, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    write_record_data_by_infoNum(threadid, 0, oadMain, oadCols, pstrbuf, buflen, num, savetime, outmsg);
    return;
}

/**********************************************************************
* @name      : read_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void read_record_data_by_infoNum(uint8 threadid, uint16 infoNum, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    int j = 0;
    outmsg->secc_num = 0;
    outmsg->err_num = 0;
    uint32 starttime = savetime;
    uint32 tmp = 0;

    READ_RECORD_T startReadRecord;
    RECORD_UNIT_T stepRecordOut;
    STEP_READ_RECORD_T stepReadRecord;
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    if (oadMain == 0)
    {
        startReadRecord.recordOAD.optional = 0;
		startReadRecord.recordOAD.road.oadMain.value = 0;
    }
    else
    {
        startReadRecord.recordOAD.optional = 1;
        startReadRecord.recordOAD.road.oadMain.value = oadMain;
    }
    startReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    startReadRecord.recordOAD.road.oadCols.oad[0].value = oadCols;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = COL_TM_STORE;

    gettimeofday(&tv1, NULL);
    if (savetime == 0)
    {
        for (j = 1; j <= num; j++)
        {
            startReadRecord.cStart = 0;
            startReadRecord.cEnd = 1;
            startReadRecord.sortType = 0x01;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;
            ret = db_read_recorddata(threadid, &startReadRecord, buflen, &stepRecordOut);
            if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen)
            {
                //                printf("record_data_write_%d error %d\n", i, ret);
                outmsg->err_num++;
//                exit(EXIT_FAILURE);
            }
            else
            {
                outmsg->secc_num++;
            }
//            usleep(1000);
        }

    }
    else
    {
        while (starttime <= (savetime + 60 * num))
        {
            memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

            startReadRecord.cStart = starttime;
            if ((starttime + 3600 * 24) <= (savetime + 60 * num))
            {
                startReadRecord.cEnd = starttime + 3600 * 24;
            }
            else
            {
                startReadRecord.cEnd = savetime + 60 * num;
            }
            starttime += 3600 * 24;

            startReadRecord.sortType = 0x00;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;

            uint32 handle = 0;
            uint32 recNum = 0;

            ret = db_read_recorddata_start(threadid, &startReadRecord, &handle, &recNum);
            if (ret != ERR_OK || recNum == 0)
            {
                printf("db_read_record_start error, ret%d or recNum[%d]=0\n", ret, recNum);
                continue;
            }


            stepReadRecord.MAC.nNum = 6;
            stepReadRecord.MAC.value[0] = 0x01;
            stepReadRecord.MAC.value[1] = 0x02;
            stepReadRecord.MAC.value[2] = 0x03;
            stepReadRecord.MAC.value[3] = 0x04;
            stepReadRecord.MAC.value[4] = 0x05;
            stepReadRecord.MAC.value[5] = 0x06;

            for (j = 0; j < recNum; j++)
            {
                ret = db_read_recorddata_step(threadid, handle, &stepReadRecord, &stepRecordOut);
                memcpy(&tmp, stepRecordOut.record[0].outDataBuf, 4);

                if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen || tmp != stepRecordOut.colStartTm)
                {
                    printf("record_data_step_read error, ret%d or outDatalen[%d]!= buflen[%d]\n", ret, stepRecordOut.record[0].outDatalen, buflen);
                    continue;
                }
                else
                {
                    outmsg->secc_num++;
                }
//                usleep(1000);
            }

            db_read_recorddata_end(threadid, handle);

        }
    }
    gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;

    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
//    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000 - num;
    outmsg->err_num = num - outmsg->secc_num;

}

void read_record_data_by_infoNum1(uint8 threadid, uint16 infoNum, uint32 oadMain, uint32 oadCols, uint32 buflen, uint32 Starttime, uint32 Endtime, Readrecord *outreadrecord)
{
    int ret = ERR_NORMAL;
    int j = 0;
    //uint32 starttime = Starttime;
    //uint32 tmp = 0;

    READ_RECORD_T startReadRecord;
    RECORD_UNIT_T stepRecordOut;
    STEP_READ_RECORD_T stepReadRecord;
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    if (oadMain == 0)
    {
        startReadRecord.recordOAD.optional = 0;
		startReadRecord.recordOAD.road.oadMain.value = 0;
    }
    else
    {
        startReadRecord.recordOAD.optional = 1;
        startReadRecord.recordOAD.road.oadMain.value = oadMain;
    }
    startReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    startReadRecord.recordOAD.road.oadCols.oad[0].value = oadCols;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = infoNum;
    startReadRecord.cType = COL_TM_STORE;

    if (Starttime == 0)
    {
        //for (j = 1; j <= num; j++)
        {
            startReadRecord.cStart = 0;
            startReadRecord.cEnd = 1;
            startReadRecord.sortType = 0x01;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;
            ret = db_read_recorddata(threadid, &startReadRecord, buflen, &stepRecordOut);
            if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen)
            {
                //                printf("record_data_write_%d error %d\n", i, ret);
                //outmsg->err_num++;
//                exit(EXIT_FAILURE);
            }
            else
            {
                //outmsg->secc_num++;
            }
//            usleep(1000);
        }

    }
    else
    {
        //while (starttime <= (savetime + 60 * num))
        {
            memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

            startReadRecord.cStart = Starttime;
            startReadRecord.cEnd = Endtime;

            startReadRecord.sortType = 0x00;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;

            uint32 handle = 0;
            uint32 recNum = 0;

            ret = db_read_recorddata_start(threadid, &startReadRecord, &handle, &recNum);
            if (ret != ERR_OK || recNum == 0)
            {
                printf("db_read_record_start error, mainOAD = %d, subOAD = %d, ret%d or recNum[%d]=0\n", startReadRecord.recordOAD.road.oadMain.value, startReadRecord.recordOAD.road.oadCols.oad[0].value, ret, recNum);
                return;
            }


            stepReadRecord.MAC.nNum = 6;
            stepReadRecord.MAC.value[0] = 0x01;
            stepReadRecord.MAC.value[1] = 0x02;
            stepReadRecord.MAC.value[2] = 0x03;
            stepReadRecord.MAC.value[3] = 0x04;
            stepReadRecord.MAC.value[4] = 0x05;
            stepReadRecord.MAC.value[5] = 0x06;

			//memset(&outreadrecord, 0, sizeof(Readrecord));
			
            for (j = 0; j < recNum; j++)
            {
                ret = db_read_recorddata_step(threadid, handle, &stepReadRecord, &stepRecordOut);
                //memcpy(&tmp, stepRecordOut.record[0].outDataBuf, 4);

                if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen)
                {
                    printf("record_data_step_read error, ret%d or outDatalen[%d]!= buflen[%d]\n", ret, stepRecordOut.record[0].outDatalen, buflen);
					g_readrecordErrorNum++;
                    continue;
                }
                else
                {
                    //outmsg->secc_num++;
                    memcpy(&outreadrecord->record[j], stepRecordOut.record[0].outDataBuf, stepRecordOut.record[0].outDatalen);
					outreadrecord->savetime[j] = stepRecordOut.colStoreTm;
					outreadrecord->len = stepRecordOut.record[0].outDatalen;
                }
//                usleep(1000);
            }
			outreadrecord->Num = recNum;

            db_read_recorddata_end(threadid, handle);

        }
    }
}



/**********************************************************************
* @name      : read_record_data
* @brief     ：读记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void read_record_data(uint8 threadid, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    read_record_data_by_infoNum(threadid, 0, oadMain, oadCols, pstrbuf, buflen,num, savetime, outmsg);
    return;
}

#if DESC("读写分离测试",1)
uint8 run_normal_reading = 0;
uint8 run_normal_writing = 0;
uint8 run_record_reading = 0;
uint8 run_record_writing = 0;

void writing_normal_data(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int j = 1;
    NOMAL_OAD_T     nomalOAD;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    int tmp = 0;

    ret = gettimeofday(&tv1, NULL);
    while (run_normal_writing==1)
    {
        nomalOAD.logicId = 0;
        nomalOAD.infoNum = j%num;
        nomalOAD.oad.value = oad;
        nomalOAD.classtag = CLASS_DATA_NORMAL;
        tmp = j%num;
        memcpy(pstrbuf, &tmp, 4);
        ret = db_write_nomaldata( threadid, &nomalOAD, pstrbuf, buflen);
        if (ret != ERR_OK)
        {
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }

        j++;
        usleep(1000);
    }
    ret = gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}

/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void reading_normal_data(uint8 threadid, uint32 oad, uint8 *pstrbuf, uint32 buflen, uint32 num, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    int j = 1;
    NOMAL_OAD_T     nomalOAD;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    uint32 outlen = 0;
    uint8 outbuf[8000] = { 0 };

    outmsg->secc_num = 0;
    outmsg->err_num = 0;

    gettimeofday(&tv1, NULL);
    while (run_normal_reading ==1)
    {
        nomalOAD.logicId = 0;
        nomalOAD.infoNum = j%num;
        nomalOAD.oad.value = oad;
        nomalOAD.classtag = CLASS_DATA_NORMAL;
        ret = db_read_nomaldata(threadid, &nomalOAD, buflen, outbuf, &outlen);
        if (ret != ERR_OK || outlen != buflen)
        {
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }

        j++;
        usleep(1000);
    }
    gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;

    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
    outmsg->err_num = num - outmsg->secc_num;
    return;
}


/**********************************************************************
* @name      : write_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void writing_record_data(uint8 threadid, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    int j = 1;
    WRITE_RECORD_T recInData;
    uint32 stroeNo = 0;

    outmsg->secc_num = 0;
    outmsg->err_num = 0;


    ret = gettimeofday(&tv1, NULL);
    while (run_record_writing == 1)
    {

        recInData.recordOAD.logicId = 0;
        recInData.recordOAD.infoNum = 0;
        if (oadMain == 0)
        {
            recInData.recordOAD.optional = 0;
        }
        else
        {
            recInData.recordOAD.optional = 1;
            recInData.recordOAD.road.oadMain.value = oadMain;
        }

        recInData.recordOAD.road.oadCols.nNum = 0x01;
        recInData.recordOAD.road.oadCols.oad[0].value = oadCols;
        recInData.recordOAD.classtag = CLASS_DATA_COLLEC;
        recInData.MAC.nNum = 6;
        recInData.MAC.value[0] = 0x01;
        recInData.MAC.value[1] = 0x02;
        recInData.MAC.value[2] = 0x03;
        recInData.MAC.value[3] = 0x04;
        recInData.MAC.value[4] = 0x05;
        recInData.MAC.value[5] = 0x06;

        if (savetime == 0)
        {
            recInData.colStartTm = 0;
            recInData.colEndTm = 0;
            recInData.colStoreTm = 0;
        }
        else
        {
            recInData.colStartTm = savetime + (j%num - 1) * 60;
            recInData.colEndTm = savetime + (j%num - 1) * 60;
            recInData.colStoreTm = savetime + (j%num - 1) * 60;
        }

        recInData.inDatalen = buflen;
        memcpy(pstrbuf, &recInData.colStartTm, 4);
        memcpy(recInData.inDataBuf, pstrbuf, recInData.inDatalen);
        ret = db_write_recorddata(threadid, &recInData, &stroeNo);
        if (ret != ERR_OK)
        {
            printf("writing_record_data[%08x] error %d\n", oadCols, ret);
            outmsg->err_num++;
        }
        else
        {
            outmsg->secc_num++;
        }

        j++;
        usleep(1000);
    }
    ret = gettimeofday(&tv2, NULL);

    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;
    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
    outmsg->err_num = num - outmsg->secc_num;
    return;

}

/**********************************************************************
* @name      : read_record_data
* @brief     ：写记录型数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void reading_record_data(uint8 threadid, uint32 oadMain, uint32 oadCols, uint8 *pstrbuf, uint32 buflen, uint32 num, uint32 savetime, MSG_INFO_t *outmsg)
{
    int ret = ERR_NORMAL;
    struct timeval tv1, tv2;
    uint64 tv1_ll = 0, tv2_ll = 0;
    int j = 1;
    outmsg->secc_num = 0;
    outmsg->err_num = 0;
    uint32 starttime = savetime;

    READ_RECORD_T startReadRecord;
    RECORD_UNIT_T stepRecordOut;
    STEP_READ_RECORD_T stepReadRecord;
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    if (oadMain == 0)
    {
        startReadRecord.recordOAD.optional = 0;
    }
    else
    {
        startReadRecord.recordOAD.optional = 1;
        startReadRecord.recordOAD.road.oadMain.value = oadMain;
    }
    startReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    startReadRecord.recordOAD.road.oadCols.oad[0].value = oadCols;
    startReadRecord.recordOAD.logicId = 0;
    startReadRecord.recordOAD.infoNum = 0;
    startReadRecord.cType = COL_TM_STORE;

    gettimeofday(&tv1, NULL);
    if (savetime == 0)
    {
        while (run_record_reading == 1)
        {
            startReadRecord.cStart = 0;
            startReadRecord.cEnd = 1;
            startReadRecord.sortType = 0x01;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;
            ret = db_read_recorddata(threadid, &startReadRecord, buflen, &stepRecordOut);
            if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen)
            {
                //                printf("record_data_write_%d error %d\n", i, ret);
                outmsg->err_num++;
            }
            else
            {
                outmsg->secc_num++;
            }
            usleep(1000);
        }

    }
    else
    {
        while (run_record_reading == 1)
        {
            memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

            startReadRecord.cStart = starttime;
            if ((starttime + 3600 * 24) <= (savetime + 60 * num))
            {
                startReadRecord.cEnd = starttime + 3600 * 24;
                starttime += 3600 * 24;
            }
            else
            {
                startReadRecord.cEnd = savetime + 60 * num;
                starttime = savetime;
            }

            startReadRecord.sortType = 0x00;
            startReadRecord.MAC.nNum = 6;
            startReadRecord.MAC.value[0] = 0x01;
            startReadRecord.MAC.value[1] = 0x02;
            startReadRecord.MAC.value[2] = 0x03;
            startReadRecord.MAC.value[3] = 0x04;
            startReadRecord.MAC.value[4] = 0x05;
            startReadRecord.MAC.value[5] = 0x06;

            uint32 handle = 0;
            uint32 recNum = 0;

            ret = db_read_recorddata_start(threadid, &startReadRecord, &handle, &recNum);
            if (ret != ERR_OK || recNum == 0)
            {
                printf("reading_record_data error, ret%d or recNum[%d]=0\n", ret, recNum);
                continue;
            }


            stepReadRecord.MAC.nNum = 6;
            stepReadRecord.MAC.value[0] = 0x01;
            stepReadRecord.MAC.value[1] = 0x02;
            stepReadRecord.MAC.value[2] = 0x03;
            stepReadRecord.MAC.value[3] = 0x04;
            stepReadRecord.MAC.value[4] = 0x05;
            stepReadRecord.MAC.value[5] = 0x06;

            for (j = 0; j < recNum; j++)
            {
                ret = db_read_recorddata_step(threadid, handle, &stepReadRecord, &stepRecordOut);

                if (ret != 0 || stepRecordOut.record[0].outDatalen != buflen)
                {
                    printf("reading_record_data error, ret%d or outDatalen[%d]!= buflen[%d]\n", ret, stepRecordOut.record[0].outDatalen, buflen);
                    continue;
                }
                else
                {
                    outmsg->secc_num++;
                }
            }

            db_read_recorddata_end(threadid, handle);

        }
    }
    gettimeofday(&tv2, NULL);

    outmsg->err_num = num - outmsg->secc_num;
    tv2_ll = tv2.tv_sec;
    tv1_ll = tv1.tv_sec;

    outmsg->total_time = ((tv2_ll * 1000000 + tv2.tv_usec) - (tv1_ll * 1000000 + tv1.tv_usec)) / 1000;
    outmsg->err_num = num - outmsg->secc_num;

}

/**********************************************************************
* @name      : test_function
* @brief     ：功能测试
* @param[in] ：int id 线程号
               uint8 testitem 测试项 0 所有测试项都运行
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void test_read_write_function(int id)
{
    MSG_INFO_t msgs;

    uint8 pdata[9000] = { 0 };
    
    if(id == 1)
    {
        reading_normal_data(id, 0x44020200, pdata, 1000, SEND_RECORD_NUM, &msgs);
    }

    if(id == 2)
    {
        reading_record_data(id, 0x50042200, 0x20000200, pdata, 1000, SEND_RECORD_NUM, 1586966400, &msgs);
    }

    if(id == 3 )
    {
        writing_normal_data(id, 0x44020200, pdata, 1000, SEND_RECORD_NUM, &msgs);
    }

    if(id == 4 )
    {
        writing_record_data(id, 0x50042200, 0x20000200, pdata, 1000, SEND_RECORD_NUM, 1586966400, &msgs);
    }

}


#endif

//是否正在检测
uint8 isrun[10] = { 0 };
uint8 isruns[10] = { 0 };
uint8 ISOK = 0;
uint8 StabuilityTestRun[3] = {0};

struct timeval tm1, tm2;

/**********************************************************************
* @name      : test_ram_func
* @brief     ：内存库功能测试
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void test_ram_func()
{
    OOP_WORD3V_T      val;
    OOP_DATETIME_S_T   tms;

    printf("test_ram_func strat!\n");

    write_normal_data(0, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RAM_NUM, g_ramhead);
    printf("write_normal_data 0x20000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
        , g_ramhead[0].secc_num, g_ramhead[0].err_num, g_ramhead[0].total_time);
    
    read_normal_data(0, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RAM_NUM, g_ramhead + 1);
    printf("read_normal_data 0x20000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
        , g_ramhead[1].secc_num, g_ramhead[1].err_num, g_ramhead[1].total_time);

    write_normal_data(0, 0x40000200, (uint8*)&tms, sizeof(OOP_DATETIME_S_T), SEND_RAM_NUM, g_ramhead + 2);
    printf("write_normal_data 0x40000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_DATETIME_S_T)
        , g_ramhead[2].secc_num, g_ramhead[2].err_num, g_ramhead[2].total_time);
    
    read_normal_data(0, 0x40000200, (uint8*)&tms, sizeof(OOP_DATETIME_S_T), SEND_RAM_NUM, g_ramhead + 3);
    printf("read_normal_data 0x40000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_DATETIME_S_T)
        , g_ramhead[3].secc_num, g_ramhead[3].err_num, g_ramhead[3].total_time);


    printf("test_ram_func end!\n");

}

/**********************************************************************
* @name      : test_biggata_func
* @brief     ：大数据功能测试
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-5-14
* @Update    :
**********************************************************************/
void test_bigdata_func()
{
    int j = 0;
    MSG_INFO_t msghead;

    uint8 pdata[9000] = { 0 };
    
    printf("test_bigdata_func start!\n");

    for(j=1; j<=10; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00100200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[0][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=11; j<=20; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00200200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00200200 end!\n");

    for(j=21; j<=30; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00300200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00300200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[1][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=31; j<=40; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00400200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00400200 end!\n");

    for(j=41; j<=50; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00500200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00500200 end!\n");

    for(j=51; j<=60; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00600200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00600200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[2][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=61; j<=70; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00700200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00700200 end!\n");

    for(j=71; j<=80; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x00800200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x00800200 end!\n");

    for(j=1; j<=10; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20000200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20000200 end!\n");

    for(j=11; j<=20; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20010200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20010200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[3][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=21; j<=30; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20020200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20020200 end!\n");

    for(j=31; j<=40; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20030200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20030200 end!\n");

    for(j=41; j<=50; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20040200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20040200 end!\n");

    for(j=51; j<=60; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20050200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20050200 end!\n");

    for(j=61; j<=70; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20060200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20060200 end!\n");

    for(j=71; j<=80; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020200, 0x20070200, pdata, 256, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50020200, 0x20070200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[4][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=1; j<=10; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00100200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00100200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50040200, 0x00100200, pdata, 1024, 1, 946663200, &g_bigdatahead.msgrecordes[5][j-1]);
    }
    printf("read_record_data 0x50040200, 0x00100200 end!\n");

    for(j=11; j<=20; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00200200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00200200 end!\n");

    for(j=21; j<=30; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50020400, 0x00300200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00300200 end!\n");

    for(j=31; j<=40; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00400200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00400200 end!\n");

    for(j=41; j<=50; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00500200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00500200 end!\n");

    for(j=51; j<=60; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00600200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00600200 end!\n");

    for(j=61; j<=70; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00700200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00700200 end!\n");

    for(j=71; j<=80; j++)
    {
        write_record_data_by_infoNum(0, j, 0x50040200, 0x00800200, pdata, 1024, SEND_BIGDATA_NUM, 946656000, &msghead);
    }
    printf("write_record_data 0x50040200, 0x00800200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50040200, 0x00100200, pdata, 1024, 1, 946663200, &g_bigdatahead.msgrecordes[6][j-1]);
    }
    printf("read_record_data 0x50040200, 0x00100200 end!\n");
   
    write_normal_datas(0, 0x90000000, pdata, 128, 10000, &msghead);
    printf("write_normal_datas 0x90000000 len 128 succnum[%u] errnum[%u] tm[%u]ms\n"
        , msghead.secc_num, msghead.err_num, msghead.total_time);

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50020200, 0x00100200, pdata, 1024, 1, 946659600, &g_bigdatahead.msgrecordes[7][j-1]);
    }
    printf("read_record_data 0x50020200, 0x00100200 end!\n");

    for(j=1; j<=10; j++)
    {
        read_record_data_by_infoNum(0, j, 0x50040200, 0x00100200, pdata, 1024, 1, 946663200, &g_bigdatahead.msgrecordes[8][j-1]);
    }
    printf("read_record_data 0x50040200, 0x00100200 end!\n");

    read_normal_data_by_oad(0, 0x90000000, (uint8*)&pdata, 128, 1000, &g_bigdatahead.msgnormal);
    printf("read_normal_data 0x90000000 len 128 succnum[%u] errnum[%u] tm[%u]ms\n"
        , g_bigdatahead.msgnormal.secc_num, g_bigdatahead.msgnormal.err_num, g_bigdatahead.msgnormal.total_time);

    printf("test_bigdata_func end\n");

}


/**********************************************************************
* @name      : test_function
* @brief     ：功能测试
* @param[in] ：int id 线程号
               uint8 testitem 测试项 0 所有测试项都运行
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void test_function(int id, uint8 testitem)
{
    OOP_METER_T        oopMeter;
    OOP_WORD3V_T      val;

    MSG_HEAD1_t msghead1;
    memset(&msghead1, 0, sizeof(MSG_HEAD1_t));

    uint8 pdata[9000] = { 0 };
    
    printf("test_function id[%d] testitem[%d] start!\n", id, testitem);

    if(testitem == 0 || testitem == 1)
    {
        write_normal_data(id, 0x60000200, (uint8*)&oopMeter, sizeof(OOP_METER_T), SEND_PARAM_NUM, msghead1.msgs);
        printf("write_normal_data 0x60000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_METER_T)
            , msghead1.msgs[0].secc_num, msghead1.msgs[0].err_num, msghead1.msgs[0].total_time);
    }

    if(testitem == 0 || testitem == 2)
    {
        read_normal_data(id, 0x60000200, (uint8*)&oopMeter, sizeof(OOP_METER_T), SEND_PARAM_NUM, msghead1.msgs + 1);
        printf("read_normal_data 0x60000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_METER_T)
            , msghead1.msgs[1].secc_num, msghead1.msgs[1].err_num, msghead1.msgs[1].total_time);
    }

    if(testitem == 0 || testitem == 3)
    {
        read_normal_data_record(id, 0x60000200, (uint8*)&oopMeter, sizeof(OOP_METER_T), SEND_PARAM_NUM, msghead1.msgs + 2);
        printf("read_normal_data_record 0x60000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_METER_T)
            , msghead1.msgs[2].secc_num, msghead1.msgs[2].err_num, msghead1.msgs[2].total_time);
    }

    if(testitem == 0 || testitem == 4)
    {
        write_normal_data(id, 0x40020200, pdata, 1000, SEND_RECORD_NUM, msghead1.msgs + 3);
        printf("write_normal_data 0x40020200 len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , msghead1.msgs[3].secc_num, msghead1.msgs[3].err_num, msghead1.msgs[3].total_time);
    }

    if(testitem == 0 || testitem == 5)
    {
        read_normal_data(id, 0x40020200, pdata, 1000, SEND_RECORD_NUM, msghead1.msgs + 4);
        printf("read_normal_data 0x40020200 len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , msghead1.msgs[4].secc_num, msghead1.msgs[4].err_num, msghead1.msgs[4].total_time);
    }

    if(testitem == 0 || testitem == 6)
    {
        write_record_data(id, 0, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RECORD_NUM, 0, msghead1.msgs + 5);
        printf("write_record_data 0x20000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
            , msghead1.msgs[5].secc_num, msghead1.msgs[5].err_num, msghead1.msgs[5].total_time);
    }

    if(testitem == 0 || testitem == 7)
    {
        read_record_data(id, 0, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RECORD_NUM, 0, msghead1.msgs + 6);
        printf("read_record_data 0x20000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
            , msghead1.msgs[6].secc_num, msghead1.msgs[6].err_num, msghead1.msgs[6].total_time);
    }

    if(testitem == 0 || testitem == 8)
    {
        write_record_data(id, 0x50040200, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RECORD_NUM, 1586966400, msghead1.msgs + 7);
        printf("write_record_data 0x50040200, 0x20000200, len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
            , msghead1.msgs[7].secc_num, msghead1.msgs[7].err_num, msghead1.msgs[7].total_time);
    }

    if(testitem == 0 || testitem == 9)
    {
        read_record_data(id, 0x50040200, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), SEND_RECORD_NUM, 1586966400, msghead1.msgs + 8);
        printf("read_record_data 0x50040200, 0x20000200 len %d succnum[%u] errnum[%u] tm[%u]ms\n", sizeof(OOP_WORD3V_T)
            , msghead1.msgs[8].secc_num, msghead1.msgs[8].err_num, msghead1.msgs[8].total_time);
    }

    if(testitem == 0)
    {
        run_normal_reading = 1;
        write_normal_data(id, 0x44020200, pdata, 1000, SEND_RECORD_NUM, g_rw_msg_head.rwmsg);
        printf("write_normal_data 0x44020200 len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , g_rw_msg_head.rwmsg[0].secc_num, g_rw_msg_head.rwmsg[0].err_num, g_rw_msg_head.rwmsg[0].total_time);
        run_normal_reading = 0;
    }

    if(testitem == 0)
    {
        run_record_reading = 1;
        write_record_data(id, 0x50042200, 0x20000200, pdata, 1000, SEND_RECORD_NUM, 1586966400, g_rw_msg_head.rwmsg + 1);
        printf("write_record_data 0x50042200, 0x20000200, len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , g_rw_msg_head.rwmsg[1].secc_num, g_rw_msg_head.rwmsg[1].err_num, g_rw_msg_head.rwmsg[1].total_time);
        run_record_reading = 0;
    }

    if(testitem == 0 )
    {
        run_normal_writing = 1;
        read_normal_data(id, 0x44020200, pdata, 1000, SEND_RECORD_NUM, g_rw_msg_head.rwmsg + 2);
        printf("read_normal_data 0x44020200 len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , g_rw_msg_head.rwmsg[2].secc_num, g_rw_msg_head.rwmsg[2].err_num, g_rw_msg_head.rwmsg[2].total_time);
        run_normal_writing = 0;
    }

    if(testitem == 0 )
    {   
        run_record_writing = 1;
        read_record_data(id, 0x50042200, 0x20000200, pdata, 1000, SEND_RECORD_NUM, 1586966400, g_rw_msg_head.rwmsg + 3);
        printf("read_record_data 0x50042200, 0x20000200 len 1000 succnum[%u] errnum[%u] tm[%u]ms\n"
            , g_rw_msg_head.rwmsg[3].secc_num, g_rw_msg_head.rwmsg[3].err_num, g_rw_msg_head.rwmsg[3].total_time);
        run_record_writing = 0;
    }


    if (isrun[0] == 1 && id == 0)
    {
        g_rw_msg_head.msgs = msghead1;
    }

    if (isrun[2] > 0)
    {
        if (id == 0)
        {
            //            g_msghead3.msg[0] = msghead1;
        }
        else if (id == 11)
        {
            g_msghead3.msg2 = msghead1;
        }
        else
        {
            g_msghead3.msg1.msgs[testitem-1].secc_num += msghead1.msgs[testitem-1].secc_num;
            g_msghead3.msg1.msgs[testitem-1].err_num += msghead1.msgs[testitem-1].err_num;

        }
    }

    printf("test_function end\n");

}

/**********************************************************************
* @name      : thread_function
* @brief     ：功能测试线程
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void test_function1()
{

    uint8 pdata[9000] = { 0 };
    uint32 datalen = 0;
    memset(&g_msghead2, 0, sizeof(MSG_HEAD2_t));

    printf("test_function1 start!\n");

    datalen = 10;
    g_msghead2.data_len[0] = datalen;
    write_normal_data(0, 0x66000200, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg1);
    read_normal_data(0, 0x66000200, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg2);
    write_record_data(0, 0x50040200, 0x26000200, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg3);
    read_record_data(0, 0x50040200, 0x26000200, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg4);

    datalen = 500;
    g_msghead2.data_len[1] = datalen;
    write_normal_data(0, 0x66000201, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg1 + 1);
    read_normal_data(0, 0x66000201, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg2 + 1);
    write_record_data(0, 0x50040201, 0x26000201, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg3 + 1);
    read_record_data(0, 0x50040201, 0x26000201, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg4 + 1);

    datalen = 1000;
    g_msghead2.data_len[2] = datalen;
    write_normal_data(0, 0x66000202, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg1 + 2);
    read_normal_data(0, 0x66000202, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg2 + 2);
    write_record_data(0, 0x50040202, 0x26000202, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg3 + 2);
    read_record_data(0, 0x50040202, 0x26000202, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg4 + 2);

    datalen = 4000;
    g_msghead2.data_len[3] = datalen;
    write_normal_data(0, 0x66000203, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg1+3);
    read_normal_data(0, 0x66000203, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg2+3);
    write_record_data(0, 0x50040203, 0x26000200, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg3+3);
    read_record_data(0, 0x50040203, 0x26000200, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg4+3);

    datalen = 7800;
    g_msghead2.data_len[4] = datalen;
    write_normal_data(0, 0x66000204, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg1 + 4);
    read_normal_data(0, 0x66000204, pdata, datalen, SEND_NORMAL_NUM, g_msghead2.msg2 + 4);
    write_record_data(0, 0x50040204, 0x26000201, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg3 + 4);
    read_record_data(0, 0x50040204, 0x26000201, pdata, datalen, SEND_NORMAL_NUM, 1586966400, g_msghead2.msg4 + 4);

    printf("test_function1 end\n");
}

int threadrun = 0;
uint8 runitem = 1;
void *thread_function(void *arg)
{
    int i = *(int *)(arg);
    uint64 tv1_ll = 0, tv2_ll = 0;

    uint8 testitem = 1;
    while (1)
    {
        if (isrun[0] == 1 )
        {
            if (i == 0)
            {
                test_function(0, 0);
                isrun[0] = 0;
            }
            else if (0 < i && i < 5)
            {
                test_read_write_function(i);
            }
            
        }

        if (isrun[1] == 1 && i == 0)
        {
            test_function1();
            isrun[1] = 0;
        }

        if (isrun[3] == 1 && i == 0)
        {
            test_bigdata_func();
            isrun[3] = 0;
        }

        if (isrun[4] == 1 && i == 0)
        {
            test_ram_func();
            isrun[4] = 0;
        }

        if (isrun[2] == 1 && 0 < i && i < 11)
        {
            if (threadrun == 0)
            {
                gettimeofday(&tm1, NULL);
            }
                
            if (threadrun < 10 && runitem == testitem)
            {
                printf("now isrun[thread%d] testitem=%d \n", i, testitem);
                test_function(i, testitem);
                testitem++;
                threadrun += 1;
            }
            
            if (threadrun == 10) //10线程完成 进入下个测试项
            {
                gettimeofday(&tm2, NULL);
                tv2_ll = tm2.tv_sec;
                tv1_ll = tm1.tv_sec;
                g_msghead3.msg1.msgs[runitem-1].total_time = ((tv2_ll * 1000000 + tm2.tv_usec) - (tv1_ll * 1000000 + tm1.tv_usec)) / 1000;
                threadrun = 0;
                runitem++;
            }

            //所有测试项完成
            if (runitem == 10)
            {
                isrun[2] = 2;
                testitem = 1;
                runitem = 1;
                threadrun = 0;
            }

        }
        else if (isrun[2] == 2 && i == 11)
        {
            test_function(11, 0);
            isrun[2] = 0;
        }

        usleep(10000);
    }
    pthread_exit("Thank you for your CPU time!");
}
void TimeSet(int year, int month, int day, int hour, int min, int sec)
{
    struct tm tptr;
    struct timeval tv;

    tptr.tm_year = year - 1900;
    tptr.tm_mon = month - 1;
    tptr.tm_mday = day;
    tptr.tm_hour = hour;
    tptr.tm_min = min;
    tptr.tm_sec = sec;

    tv.tv_sec = mktime(&tptr);
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

}
//判断是否是闰年
int fun(int n)
{
    if ((n % 4 == 0 && n % 100 != 0) || (n % 400 == 0))
        return 1;
    else return 0;
}

BOOL LinuxTm2DateTimeShort(time_t linuxTime, struct tm* systime)
{
    struct  timeval tv;
    struct  timezone tz;
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = linuxTime;
    FastSecondToDate(&tv, &tz, systime);

    return TRUE;
}

BOOL DT_DateTimeHex2TimeShort(DateTimeHex_t *dateTimeHex, struct tm *ptm)
{
    //struct tm ptm;
    uint16 year;
    if(dateTimeHex == NULL)
        return FALSE;
    if(FALSE == DT_DateTimeHexIsValid(dateTimeHex))
    {
		printf("DT_DateTimeHex2Time Error\n");
		return FALSE;
    	}
    year = (uint16)dateTimeHex->year_h<<8;
    year += dateTimeHex->year_l;
    ptm->tm_year = year-1900;
    ptm->tm_mon = dateTimeHex->month-1;
    ptm->tm_mday = dateTimeHex->day;
    ptm->tm_hour = dateTimeHex->hour;
    ptm->tm_min = dateTimeHex->min;
    ptm->tm_sec = dateTimeHex->sec;

	printf("year:%d,month:%d,day:%d,hour:%d,min:%d,sec=%d\n",ptm->tm_year, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    //*linuxTim = FastDateToSecond(&ptm);

    return TRUE;
}

/**********************************************************************
* @name      : set_local_time
* @brief     ：设置时钟
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int set_local_time()
{
    /* struct timeval stime;

     gettimeofday(&stime, NULL);
     printf("现在的时间秒数是：%ld,毫秒数是：%ld\n现在的时间是:", stime.tv_sec, stime.tv_usec);
     fflush(stdout);
     system("date");

     stime.tv_sec += 86399;
     settimeofday(&stime, NULL);
     printf("现在的时间秒数是：%ld,毫秒数是：%ld\n现在的时间是:", stime.tv_sec, stime.tv_usec);
     fflush(stdout);
     system("date");



     return 0;*/
    int year = 0;
    int mon = 0;
    int day = 0;
    int h = 23;
    int min = 59;
    int s = 50;

    struct tm *tm_ptr;
    time_t the_time;

    (void)time(&the_time);
    tm_ptr = localtime(&the_time);

    printf("Date: %04d/%02d/%02d\n",
        1900 + tm_ptr->tm_year, tm_ptr->tm_mon + 1, tm_ptr->tm_mday);
    printf("Time: %02d:%02d:%02d\n",
        tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

    year = 1900 + tm_ptr->tm_year;//系统当前年
    mon = tm_ptr->tm_mon + 1;//系统当前月
    day = tm_ptr->tm_mday;//系统当前天数
    int ref = fun(year);//判断是否是闰年

    switch (mon)
    {
    case 1:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 2;
        }
        else
        {
            day += 1;
        }
        break;
    case 2:
        if (ref == 1)//是闰年
        {

            if (day + 1 > 29)//判断是否当月最后一天
            {
                day = 1;
                mon = 3;
            }
            else
            {
                day += 1;
            }
        }
        else
        {
            if (day + 1 > 28)//判断是否当月最后一天
            {
                day = 1;
                mon = 3;
            }
            else
            {
                day += 1;
            }
        }
        break;
    case 3:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 4;
        }
        else
        {
            day += 1;
        }
        break;
    case 4:
        if (day + 1 > 30)//判断是否当月最后一天
        {
            day = 1;
            mon = 5;
        }
        else
        {
            day += 1;
        }
        break;
    case 5:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 6;
        }
        else
        {
            day += 1;
        }
        break;
    case 6:
        if (day + 1 > 30)//判断是否当月最后一天
        {
            day = 1;
            mon = 7;
        }
        else
        {
            day += 1;
        }
        break;
    case 7:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 8;
        }
        else
        {
            day += 1;
        }
        break;
    case 8:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 9;
        }
        else
        {
            day += 1;
        }
        break;
    case 9:
        if (day + 1 > 30)//判断是否当月最后一天
        {
            day = 1;
            mon = 10;
        }
        else
        {
            day += 1;
        }
        break;
    case 10:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 11;
        }
        else
        {
            day += 1;
        }
        break;
    case 11:
        if (day + 1 > 30)//判断是否当月最后一天
        {
            day = 1;
            mon = 12;
        }
        else
        {
            day += 1;
        }
        break;
    case 12:
        if (day + 1 > 31)//判断是否当月最后一天
        {
            day = 1;
            mon = 1;
            year += 1;
        }
        else
        {
            day += 1;
        }
        break;
    default:
        break;
    }
    printf("before time set");
    fflush(stdout);
    system("date");
    system("hwclock");

    TimeSet(year, mon, day, h, min, s);
    system("hwclock -w");


    printf("after time set");
    fflush(stdout);
    system("date");
    system("hwclock");
    return 0;
}
//自动删除
void test_functions(Para para, int j) {


    uint8 tag = 0;
    int ret = 0;
    uint32 depthNum = 0;//存储深度
    uint32 recNum = 0;//存储个数
    uint32 stroeNo = 0;//存储序号
    int Index = 0;//递增存储值
    //写记录型数据
    WRITE_RECORD_T recInData;
    memset(&recInData, 0, sizeof(WRITE_RECORD_T));
    
    recInData.recordOAD.logicId = 0;
    recInData.recordOAD.infoNum = 0;
    if (para.MainOAD == 0)
    {
        recInData.recordOAD.optional = 0;
    }
    else {
        recInData.recordOAD.optional = 1;
        recInData.recordOAD.road.oadMain.value = para.MainOAD;
    }
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0].value = para.ColOAD;
    recInData.recordOAD.classtag = para._CLASS_DATA_E;
    recInData.MAC.nNum = 6;
    recInData.MAC.value[0] = 0x01;
    recInData.MAC.value[1] = 0x02;
    recInData.MAC.value[2] = 0x03;
    recInData.MAC.value[3] = 0x04;
    recInData.MAC.value[4] = 0x05;
    recInData.MAC.value[5] = 0x06;

//    db_get_recorddata_depth(10, &recInData.recordOAD, &depthNum);//获取存储深度
	db_get_record_depth(g_dbClients[0], &recInData.recordOAD, &depthNum);//获取存储深度

    printf("depthNum0=%d\n", depthNum);
    
    while (recNum < depthNum * (1.5))
    {
        recInData.colStartTm = (Index);
        recInData.colEndTm = (Index);
        recInData.colStoreTm = (Index);

        recInData.inDatalen = sizeof(Index);
        memcpy(recInData.inDataBuf, &Index, recInData.inDatalen);
        ret = db_write_recorddata(0, &recInData, &stroeNo);
        if (ret == 0)
        {
            Index++;
        }
        recNum = Index;

    }
    printf("recNum=%d\n", recNum);

    system("appm -S -c baseContainer -n dbCenter");


    system("appm -s -c baseContainer -n dbCenter");

    sleep(60);

    printf("set_local_time OK\n");

    GET_RECORD_NUM_T pRecord;
    memset(&pRecord, 0, sizeof(GET_RECORD_NUM_T));
    uint32 recNums = 0;//存储个数

    pRecord.filter = GET_ALL_RECORD;    
    pRecord.recordOAD = recInData.recordOAD;
	db_get_record_num(g_dbClients[0], &pRecord, &recNums);//获取记录条数
//    db_get_recorddata_num(0, &pRecord, &recNums);//获取记录条数
    printf("db_get_record_num=%d\n", recNums);
    
    //读记录型数据
    READ_RECORD_T startReadRecord;
    RECORD_UNIT_T stepRecordOut;
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&startReadRecord, 0, sizeof(READ_RECORD_T));

    startReadRecord.recordOAD  = recInData.recordOAD;
    startReadRecord.cType = COL_TM_STORE;
    startReadRecord.cStart = 0;
    startReadRecord.cEnd = 0;
    startReadRecord.sortType = 0x00;
    startReadRecord.MAC = recInData.MAC;
    db_read_recorddata(0, &startReadRecord, sizeof(OOP_WORD3V_T), &stepRecordOut);
    printf(" stepRecordOut.recordnum=%d\n", stepRecordOut.recordnum);

    if (stepRecordOut.recordnum==0)//第一条数据如果不存在
    {
        printf("first data delete success!\n");
        
        startReadRecord.cStart = Index-1;
        startReadRecord.cEnd = Index;
        db_read_recorddata(0, &startReadRecord, sizeof(Index), &stepRecordOut);
        printf("stepRecordOut.recordnum=%d\n", (stepRecordOut.recordnum));
        if (stepRecordOut.recordnum != 0)//最后一条数据如果存在
        {
            tag = 6;
            printf(" tag1=%d\n", tag);
            psendMsg = &tag;
            isruns[j] = 0;
        }
        else
        {
            tag = 3;
            printf(" tag1=%d\n", tag);
            psendMsg = &tag;
            isruns[j] = 0;

        }
    }
    else
    {
        tag = 3;
        printf(" tag2=%d\n", tag);
        psendMsg = &tag;
        isruns[j] = 0;

    }
    //return;//当满足条件或者不满足条件跳出循环
}


//自救删除
void test_functionsSY(Para para, int j) 
{
	uint8 tmp = 0;
    char buff[64] = "/data/app/";
	char fileName[128] = {0};
    int Index = 0;
    int ret;
	int n = 0;
    
	//写记录型数据
    WRITE_RECORD_T recInData;
    memset(&recInData, 0, sizeof(WRITE_RECORD_T));
    uint32 stroeNo = 0;

	recInData.recordOAD.logicId = 0;
    recInData.recordOAD.infoNum = 0;

    recInData.recordOAD.optional = 1;

	recInData.recordOAD.road.oadMain.value = 0x50050200;
    recInData.recordOAD.road.oadCols.nNum = 0x01;
    recInData.recordOAD.road.oadCols.oad[0].value = 0x20020200;
    recInData.recordOAD.classtag = CLASS_DATA_COLLEC;
    recInData.MAC.nNum = 0;

	recInData.inDatalen = 1000;
	
    for (Index=0; Index<10000; Index++)
    {
        recInData.colStartTm = (Index+1) * 60;
        recInData.colEndTm = (Index+1) * 60;
        recInData.colStoreTm = (Index+1) * 60;

        ret = db_write_recorddata(1, &recInData, &stroeNo);
        if(ret != 0)	
		{
			COMM_FMT_DEBUG("db_write_record  Index %d error ret=%d\n", Index, ret);
		}
        	
	}
	
	uint32 recNum1 , recNum2 = 0;
    GET_RECORD_NUM_T pRecord;
    memset(&pRecord, 0, sizeof(GET_RECORD_NUM_T));


    pRecord.filter = GET_ALL_RECORD;

    pRecord.recordOAD.optional = 1;
    pRecord.recordOAD.road.oadMain.value = 0x50050200;
    pRecord.recordOAD.infoNum = 0;
    pRecord.recordOAD.logicId = 0;
    pRecord.recordOAD.road.oadCols.nNum = 1;
    pRecord.recordOAD.road.oadCols.oad[0].value = 0x20020200;
    ret = db_get_record_num(g_dbClients[0], &pRecord, &recNum1);//获取记录条数
    if(ret != 0)	
	{
		COMM_FMT_DEBUG("db_get_record_num   error ret=%d\n", ret);
		goto end;
	}
    COMM_FMT_DEBUG("recNum1=%d\n", recNum1);


	char cmd[128];
	
	uint64 totalSize;
    size_t mbTotalsize;
    uint64 freeDisk;
    size_t mbFreedisk;
    uint64 totalBlocks;
	struct statfs diskInfos;
	float rate = 0.0;

	statfs(buff, &diskInfos);
	totalBlocks = diskInfos.f_bsize;
 	totalSize = totalBlocks * diskInfos.f_blocks;
    mbTotalsize = totalSize >> 20;
    freeDisk    = diskInfos.f_bavail * totalBlocks;
    mbFreedisk  = freeDisk >> 20;
    rate = (float)mbFreedisk / (float)mbTotalsize;
    printf("availableDisk / totalsize =%f\n", rate);
	
    if (rate >= 0.2)
    {
		FILE *lb;  
		char buf[10240];
        sprintf(fileName,"%stestMax", buff);
		
        //!<打开文件    
        if (NULL == (lb = fopen(fileName, "a+")))
        {
            COMM_FMT_DEBUG("Fail to open the file");
			goto end;
        }

		for(n=0; n<1024*10; n++)
		{
			fwrite(buf, sizeof(buf), 1, lb);
			if(n == 1024)
			{
				sprintf(cmd, "cp  %stestMax %stestMIn", buff, buff);
				system(cmd);
			}
		}

        fclose(lb);

    }
	n = 0;

	while (1)
    {
        
        statfs(buff, &diskInfos);
		totalBlocks = diskInfos.f_bsize;
     	totalSize = totalBlocks * diskInfos.f_blocks;
	    mbTotalsize = totalSize >> 20;
	    freeDisk    = diskInfos.f_bavail * totalBlocks;
	    mbFreedisk  = freeDisk >> 20;
	    rate = (float)mbFreedisk / (float)mbTotalsize;
//        printf("availableDisk / totalsize =%f\n", rate);
		
        if (rate >= 0.15)
        {

			sprintf(cmd, "cp  %stestMax %stestMax%d", buff, buff, n);
			system(cmd);

        }
        else if (rate >= 0.1)
        {
		
            sprintf(cmd, "cp  %stestMIn %stestMIn%d", buff, buff, n);
			system(cmd);
//			sprintf(fileName,"%stest%d", buff, n);
//            n ++;
//            //!<打开文件    
//            int fd  = -1;
//		    fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0664);
//		    if(fd < 0)
//		    {
//		        COMM_FMT_DEBUG("Fail to open the file");
//		        return ;
//		    }
//		    
//		    lseek(fd, 10*1024LL*1024LL-1LL, SEEK_SET);
//		    write(fd,"",1);
//			close(fd);
        }
		else
        {
		
            break;
        }
		n++;
    }

	system("sync");

	system("appm -S -c baseContainer -n dbCenter");
	COMM_FMT_DEBUG(" stop dbCenter\n");

	system("appm -s -c baseContainer -n dbCenter");
	COMM_FMT_DEBUG(" start dbCenter\n");
	sleep(60);

	sprintf(cmd, "rm -rf %stest*", buff);
	
	ret = db_get_record_num(g_dbClients[0], &pRecord, &recNum2);//获取记录条数
	COMM_FMT_DEBUG("recNum2=%d\n", recNum2);
	if(ret != 0)	
	{
		COMM_FMT_DEBUG("db_get_record_num   error ret=%d\n", ret);
		goto end;
	}
	else if ((recNum1 / 2) >= recNum2)
	{
		tmp = 6;//成功
		psendMsg = &tmp;
		printf("tmp=%d\n", tmp);
		ISOK = 0;
		system(cmd);
		return ;
	}
end :
	{
		tmp = 3;//失败
		psendMsg = &tmp;
		printf("tmp=%d\n", tmp);
		ISOK = 0;
		system(cmd);
	}

	
}
//用于自动删除
void *thread_functions(void *arg)
{

    //uint8 runnum = 0;
    while (1)
    {
        if (isruns[0] == 1)
        {

            printf("isruns[0]=%d\n", isruns[0]);
            //统计类自动删除
            memset(&para, 0, sizeof(Para));

            para.pInMsg = pInMsgs;
            para.MainOAD = 0x21000200;
            para.ColOAD = 0x22000200;
            para.logicId = 0;
            para._CLASS_DATA_E = 5;
            test_functions(para, 0);
            isruns[0] = 0;
        }

        if (isruns[1] == 1)
        {
            printf("isruns[1]=%d\n", isruns[1]);
            //事件类
            memset(&para, 0, sizeof(Para));
            para.MainOAD = 0x00;
            para.ColOAD = 0x31140200;
            para._CLASS_DATA_E = 4;
            para.logicId = 1;
            test_functions(para, 1);
            isruns[1] = 0;
        }


        if (isruns[2] == 1)
        {
            printf("isruns[2]=%d\n", isruns[2]);
            //冻结类
            memset(&para, 0, sizeof(Para));
            para.MainOAD = 0x50080200;
            para.ColOAD = 0x20020200;
            para.logicId = 2;
            para._CLASS_DATA_E = 5;
            test_functions(para, 2);
            isruns[2] = 0;
        }


        if (isruns[3] == 1)//启动采集自动删除
        {
            printf("isruns[3]=%d\n", isruns[3]);

            //采集类
            memset(&para, 0, sizeof(Para));
            para.MainOAD = 0;
            para.ColOAD = 0x20000200;
            para.logicId = 3;
            para._CLASS_DATA_E = 5;
            test_functions(para, 3);
            isruns[3] = 0;
        }

        sleep(3);

    }
    pthread_exit("Thank you for your CPU time!");

}


//用于自救删除
void *thread_functionsSY(void *arg)
{
    int index = 0;

    while (1)
    {
        if (ISOK == 1)
        {

            printf("ISOK=%d\n", ISOK);
            //统计类自动删除
            memset(&para, 0, sizeof(Para));

            para.pInMsg = pInMsgs;

            test_functionsSY(para, index);
            ISOK = 0;
        }

        sleep(3);
    }
    pthread_exit("Thank you for your CPU time!");
}

void *thread_StabilityTest(void *pArg)
{
	struct tm tm_starttime;

	// 稳定性测试
	while(1)
	{
		//分钟数据
		if(StabuilityTestRun[0] == 1)
		{
			//printf("%x %x %x %x %x %x %x", startwriterecordtime.year_h, startwriterecordtime.year_l, startwriterecordtime.month, startwriterecordtime.day, startwriterecordtime.hour, startwriterecordtime.min, startwriterecordtime.sec);
			DT_DateTimeHex2TimeShort(&startwriterecordtime, &tm_starttime);
			//printf("year:%d,month:%d,day:%d,hour:%d,min:%d,sec=%d\n",tm_starttime.tm_year, tm_starttime.tm_mon, tm_starttime.tm_mday, tm_starttime.tm_hour, tm_starttime.tm_min, tm_starttime.tm_sec);
			StabuilityTest(0, &tm_starttime);
			StabuilityTestRun[0] = 0;
			}

		//日冻结
		if(StabuilityTestRun[1] == 1)
		{
			dayFrozen = true;
			DT_DateTimeHex2TimeShort(&startwriterecordtime, &tm_starttime);
			StabuilityTest(1, &tm_starttime);
			StabuilityTestRun[1] = 0;
			}

		//读取数据，对比数据
		if(StabuilityTestRun[2] == 1)
		{
			DT_DateTimeHex2TimeShort(&startwriterecordtime, &tm_starttime);
			printf("year:%d,month:%d,day:%d,hour:%d,min:%d,sec=%d\n",tm_starttime.tm_year, tm_starttime.tm_mon, tm_starttime.tm_mday, tm_starttime.tm_hour, tm_starttime.tm_min, tm_starttime.tm_sec);
			StabuilityTest(2, &tm_starttime);
			StabuilityTestRun[2] = 0;
			}

		usleep(10000);
	}

	pthread_exit("Thank you for your CPU time!");
}

void *thread_OadTest(void *pArg)
{
    OOP_WORD3V_T      val;

    MSG_HEAD1_t msghead1;
    memset(&msghead1, 0, sizeof(MSG_HEAD1_t));

    int i =0;
    

	// 稳定性测试
	while(1)
	{
		for(i=1; i<=500; i++)
        {
            write_record_data(0, i, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), 10, 0, msghead1.msgs);
            sleep(1);

        }

        for(i=1; i<=500; i++)
        {
            read_record_data(0, i, 0x20000200, (uint8*)&val, sizeof(OOP_WORD3V_T), 10, 0, msghead1.msgs);
            sleep(1);
        }

		sleep(10);
	}

	pthread_exit("Thank you for your CPU time!");
}

void *thread_OadTest1(void *pArg)
{
    OOP_WORD3V_T      val;

    MSG_HEAD1_t msghead1;
    memset(&msghead1, 0, sizeof(MSG_HEAD1_t));

    int i =0;
	int j = 0;
    

	// 稳定性测试
	while(1)
	{
		for(i=1; i<=10; i++)
        {
        	for(j=1; j<=20; j++)
	        {
	            write_record_data(0, i, 0x20000200+j, (uint8*)&val, sizeof(OOP_WORD3V_T), 1, 0, msghead1.msgs);
	            sleep(1);

	        }
        }

	}

	pthread_exit("Thank you for your CPU time!");
}

/**********************************************************************
* @name      : app_heart_bear
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_test_msg(MESSAGE_INFO_T *pInMsg)
{
    void *psendMsg = NULL;
    uint8 tag;

    switch (pInMsg->IOP)
    {
    case 1:
        if (isrun[0] == 0)//未执行或已执行
        {
            tag = 1;
            isrun[0] = 1;
        }
        else
        {
            tag = 0;

        }
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        break;
    case 2:
        if (isrun[0] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(g_rw_msg_head);
            psendMsg = &g_rw_msg_head;
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        break;
    case 3:
        if (isrun[1] == 0)//未执行或已执行
        {
            tag = 1;
            isrun[1] = 1;
        }
        else
        {
            tag = 0;

        }
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        break;
    case 4:
        if (isrun[1] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(g_msghead2);
            psendMsg = &g_msghead2;
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        break;
    case 5:
        if (isrun[2] == 0)//未执行或已执行
        {
            tag = 1;
            isrun[2] = 1;
            memset(&g_msghead3, 0, sizeof(MSG_HEAD3_t));
        }
        else
        {
            tag = 0;

        }
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        break;
    case 6:
        if (isrun[2] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(g_msghead3);
            psendMsg = &g_msghead3;
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;
        }
        break;
    case 7:
        if (isrun[3] == 0)//未执行或已执行
        {
            tag = 1;
            isrun[3] = 1;
            memset(&g_bigdatahead, 0, sizeof(MSG_BIG_DATA_t));
        }
        else
        {
            tag = 0;

        }
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        break;
    case 8:
        if (isrun[3] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(g_bigdatahead);
            psendMsg = &g_bigdatahead;
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;
        }
        break;
    case 9:
        if (isrun[4] == 0)//未执行或已执行
        {
            tag = 1;
            isrun[4] = 1;
            memset(&g_ramhead, 0, sizeof(MSG_INFO_t)*4);
        }
        else
        {
            tag = 0;

        }
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        break;
    case 10:
        if (isrun[4] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(g_ramhead);
            psendMsg = &g_ramhead;
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;
        }
        break;
    default:
        tag = 0xff;
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
    }

    app_send_msg(pInMsg, psendMsg);

    return 0;
}


void Writefile(char *data, int len)
{
	FILE *fp;
	fp = fopen("/mnt/test/bin/data","ab+");
	if(fp == NULL)
	{
		printf("file open error!");
		return;
	}

	fwrite(data, len, 1, fp);
	fclose(fp);
}

int CompareData(char *str1, char *str2, int bytes)
{
	int i;

	for(i=0;i<bytes;i++)
	{
		if(str1[i]!=str2[i]) return 1;
		}

	return 0;
	}


int my_getline(char* line, int max_size) 
{
	int c; 
	int len = 0; 
	while( (c = getchar()) != EOF && len < max_size )
	{ 
		line[len++] = c; 
		if(0x0D == c)
			break; 
	}
	
	line[len] = '\0';
	return len;
}

void StabuilityTest(int select, struct tm *time)
{
	int errorNo = 0;
    uint8 pdata[9000] = { 0 };

	OOP_WORD3V_T tVoltage;
	OOP_INT3V_T tCurrent;
	OOP_INT4V_T activepower;
	OOP_INT4V_T Reactivepower;
	OOP_INT4V_T Apparentpower;
	OOP_LONG4V_T tFactor;
	OOP_ENERGYS_T tEnergy;

	char temp[1100];
	if(select == 0)
	{
		printf("50020200\n");
		
		//曲线数据
		int i;
		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tVoltage.nNum = 3;
			tVoltage.rsv = i;
			tVoltage.nValue[0] = 2200 + time->tm_mday;
			tVoltage.nValue[1] = 2100 + time->tm_hour;
			tVoltage.nValue[2] = 2000 + time->tm_min;
			memcpy(pdata, &tVoltage, sizeof(OOP_WORD3V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x20000200, pdata, sizeof(OOP_WORD3V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020020000200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_DWORD3V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//printf("50020200:%s\n", temp);
			//Writefile(temp, sizeof(OOP_DWORD3V_T) * 2 + 2 + 21);
			}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tCurrent.nNum = 3;
			tCurrent.rsv = i;
			tCurrent.nValue[0] = 10000 + time->tm_mday;
			tCurrent.nValue[1] = 11000 + time->tm_hour;
			tCurrent.nValue[2] = 22000 + time->tm_min;
			memcpy(pdata, &tCurrent, sizeof(OOP_INT3V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x20010200, pdata, sizeof(OOP_INT3V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020020010200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			activepower.nNum = 4;
			activepower.rsv = i;
			activepower.nValue[0] = 100000 + time->tm_mday;
			activepower.nValue[1] = 110000 + time->tm_hour;
			activepower.nValue[2] = 220000 + time->tm_min;
			activepower.nValue[3] = 220000 + time->tm_sec;
			memcpy(pdata, &activepower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x20040200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020020040200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			Reactivepower.nNum = 4;
			Reactivepower.rsv = i;
			Reactivepower.nValue[0] = 200000 + time->tm_mday;
			Reactivepower.nValue[1] = 210000 + time->tm_hour;
			Reactivepower.nValue[2] = 320000 + time->tm_min;
			Reactivepower.nValue[3] = 320000 + time->tm_sec;
			memcpy(pdata, &Reactivepower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x20050200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020020050200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			Apparentpower.nNum = 4;
			Apparentpower.rsv = i;
			Apparentpower.nValue[0] = 300000 + time->tm_mday;
			Apparentpower.nValue[1] = 310000 + time->tm_hour;
			Apparentpower.nValue[2] = 420000 + time->tm_min;
			Apparentpower.nValue[3] = 420000 + time->tm_sec;
			memcpy(pdata, &Apparentpower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x20060200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020020060200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tFactor.nNum = 4;
			tFactor.rsv = i;
			tFactor.nValue[0] = 99000 + time->tm_mday;
			tFactor.nValue[1] = 99900 + time->tm_hour;
			tFactor.nValue[2] = 100000 + time->tm_min;
			tFactor.nValue[3] = 105000 + time->tm_sec;
			memcpy(pdata, &tFactor, sizeof(OOP_LONG4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x200A0200, pdata, sizeof(OOP_LONG4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "50020200200A0200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_LONG4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_LONG4V_T) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 55000 + time->tm_mon;
			tEnergy.nValue[1] = 22000 + time->tm_mday;
			tEnergy.nValue[2] = 33000 + time->tm_hour;
			tEnergy.nValue[3] = 44000 + time->tm_min;
			tEnergy.nValue[4] = 11000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x00100200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020000100200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 66000 + time->tm_mon;
			tEnergy.nValue[1] = 33000 + time->tm_mday;
			tEnergy.nValue[2] = 44000 + time->tm_hour;
			tEnergy.nValue[3] = 55000 + time->tm_min;
			tEnergy.nValue[4] = 22000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x00200200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020000200200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 77000 + time->tm_mon;
			tEnergy.nValue[1] = 44000 + time->tm_mday;
			tEnergy.nValue[2] = 55000 + time->tm_hour;
			tEnergy.nValue[3] = 66000 + time->tm_min;
			tEnergy.nValue[4] = 33000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x00300200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020000300200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 99000 + time->tm_mon;
			tEnergy.nValue[1] = 55000 + time->tm_mday;
			tEnergy.nValue[2] = 66000 + time->tm_hour;
			tEnergy.nValue[3] = 77000 + time->tm_min;
			tEnergy.nValue[4] = 44000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50020200, 0x00400200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5002020000400200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		char temp1[2150]={0};
		char temp[15];
		int len = 0;
		for(i = 0; i < 500; i++)
		{
			sprintf(temp, "%04d", i);
			temp[4] = time->tm_mon;
			temp[5] = time->tm_mday;
			temp[6] = time->tm_hour;
			temp[7] = time->tm_min;
			temp[8] = time->tm_sec;
			temp[9] = 0;
			//DPRINT_MSG(temp, strlen(temp), "temp:");
			
			strcpy((char *)&temp1, "01000000E407040E08211400E407040E0821140001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000020000000000000000220040008200400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008080000000000000000000000000000000000000000000000000000000000000000000010E407040E08210B00E407040E0821140000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022");

			len = strlen((const char*)&temp1);
			commfun_ASC2BCD((char *)&pdata, temp1, len, 0);
			//DPRINT_MSG(pdata, len / 2, "temp1:");
			memcpy((char *)&pdata[len / 2], temp, 9);
			//DPRINT_MSG(pdata, len / 2 + 9, "temp2:");
			
			write_record_data_by_infoNum1(0, i + 1, 0, 0x31050200, pdata, len / 2 + 9, mktime(time), 0, errorNo);
			//strcpy(temp1, "0000000031050200,");
			//sprintf(&temp1[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp1[21], (char *)&pdata, (len / 2 + 9) * 2);
			
			//temp1[(len / 2 + 9) * 2 + 21] = 0x0D;
			//temp1[(len / 2 + 9) * 2 + 1 + 21] = 0x0A;
			///Writefile(temp1, (len / 2 + 9) * 2 + 2 + 21);
		}
		g_minErrorNum += errorNo;

		memset(temp, 0, 100);
		for(i = 0; i < 500; i++)
		{
			sprintf(temp, "%04d", i);
			temp[4] = 0;
			strcpy((char *)&temp1, "00001000");
			strcat((char *)&temp1, temp);
			strcpy(temp, "2000");
			strcat((char *)&temp1, temp);
			len = strlen((const char*)&temp1);
			commfun_ASC2BCD((char *)&pdata, temp1, len, 0);
			
			write_record_data_by_infoNum1(0, i + 1, 0x21020200, 0x22000200, pdata, len / 2, mktime(time), mktime(time), errorNo);
			//strcpy(temp, "2102020022000200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, len);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, len + 2 + 21);
		}
		g_minErrorNum += errorNo;

		printf("Error = %d\n", g_minErrorNum);
		}
	else if(select == 1)
	{
		printf("50040200\n");

		//dayFrozen = true;
		//曲线数据
		int i;
		for(i = 0; i < 500; i++)
		{
			tVoltage.nNum = 3;
			tVoltage.rsv = i;
			tVoltage.nValue[0] = 2200 + time->tm_mday;
			tVoltage.nValue[1] = 2100 + time->tm_hour;
			tVoltage.nValue[2] = 2000 + time->tm_min;
			memcpy(pdata, &tVoltage, sizeof(OOP_WORD3V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x20000200, pdata, sizeof(OOP_WORD3V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020020000200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_DWORD3V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_DWORD3V_T) * 2 + 2 + 21);
			}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			tCurrent.nNum = 3;
			tCurrent.rsv = i;
			tCurrent.nValue[0] = 10000 + time->tm_mday;
			tCurrent.nValue[1] = 11000 + time->tm_hour;
			tCurrent.nValue[2] = 22000 + time->tm_min;
			memcpy(pdata, &tCurrent, sizeof(OOP_INT3V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x20010200, pdata, sizeof(OOP_INT3V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020020010200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			activepower.nNum = 4;
			activepower.rsv = i;
			activepower.nValue[0] = 100000 + time->tm_mday;
			activepower.nValue[1] = 110000 + time->tm_hour;
			activepower.nValue[2] = 220000 + time->tm_min;
			activepower.nValue[3] = 220000 + time->tm_sec;
			memcpy(pdata, &activepower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x20040200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020020040200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			activepower.nNum = 4;
			activepower.rsv = i;
			activepower.nValue[0] = 200000 + time->tm_mday;
			activepower.nValue[1] = 210000 + time->tm_hour;
			activepower.nValue[2] = 320000 + time->tm_min;
			activepower.nValue[3] = 320000 + time->tm_sec;
			memcpy(pdata, &Reactivepower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x20050200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020020050200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			Apparentpower.nNum = 4;
			Apparentpower.rsv = i;
			Apparentpower.nValue[0] = 300000 + time->tm_mday;
			Apparentpower.nValue[1] = 310000 + time->tm_hour;
			Apparentpower.nValue[2] = 420000 + time->tm_min;
			Apparentpower.nValue[3] = 420000 + time->tm_sec;
			memcpy(pdata, &Apparentpower, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x20060200, pdata, sizeof(OOP_INT4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020020060200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_INT4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_INT4V_T) * 2 + 2 + 21);
			}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			tFactor.nNum = 4;
			tFactor.rsv = i;
			tFactor.nValue[0] = 99000 + time->tm_mday;
			tFactor.nValue[1] = 99900 + time->tm_hour;
			tFactor.nValue[2] = 100000 + time->tm_min;
			tFactor.nValue[3] = 105000 + time->tm_sec;
			memcpy(pdata, &tFactor, sizeof(OOP_INT4V_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x200A0200, pdata, sizeof(OOP_LONG4V_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "50040200200A0200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_LONG4V_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_LONG4V_T) * 2 + 2 + 21);
		}
		g_dayErrorNum += errorNo;

		
		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 55000 + time->tm_mon;
			tEnergy.nValue[1] = 22000 + time->tm_mday;
			tEnergy.nValue[2] = 33000 + time->tm_hour;
			tEnergy.nValue[3] = 44000 + time->tm_min;
			tEnergy.nValue[4] = 11000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x00100200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020000100200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_dayErrorNum += errorNo;

		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 66000 + time->tm_mon;
			tEnergy.nValue[1] = 33000 + time->tm_mday;
			tEnergy.nValue[2] = 44000 + time->tm_hour;
			tEnergy.nValue[3] = 55000 + time->tm_min;
			tEnergy.nValue[4] = 22000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x00200200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020000200200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_dayErrorNum += errorNo;

		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 77000 + time->tm_mon;
			tEnergy.nValue[1] = 44000 + time->tm_mday;
			tEnergy.nValue[2] = 55000 + time->tm_hour;
			tEnergy.nValue[3] = 66000 + time->tm_min;
			tEnergy.nValue[4] = 33000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x00300200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020000300200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_dayErrorNum += errorNo;

		for(i = 0; i < 500; i++)
		{
			tEnergy.nNum = 5;
			tEnergy.rsv = i;
			tEnergy.nValue[0] = 99000 + time->tm_mon;
			tEnergy.nValue[1] = 55000 + time->tm_mday;
			tEnergy.nValue[2] = 66000 + time->tm_hour;
			tEnergy.nValue[3] = 77000 + time->tm_min;
			tEnergy.nValue[4] = 44000 + time->tm_sec;
			memcpy(pdata, &tEnergy, sizeof(OOP_ENERGYS_T));
			
			write_record_data_by_infoNum1(0, i + 1, 0x50040200, 0x00400200, pdata, sizeof(OOP_ENERGYS_T), mktime(time), mktime(time), errorNo);
			//strcpy(temp, "5004020000400200,");
			//sprintf(&temp[17], "%03d,", i+1);
			//commfun_BCD2ASC(&temp[21], (char *)&pdata, sizeof(OOP_ENERGYS_T) * 2);
			//temp[strlen(temp)] = 0x0D;
			//temp[strlen(temp)] = 0x0A;
			//Writefile(temp, sizeof(OOP_ENERGYS_T) * 2 + 2 + 21);
		}
		g_dayErrorNum += errorNo;

		printf("Error = %d\n", g_dayErrorNum);
	}
	else //读取比对
	{
		int i, j;
		int No;
		struct tm tm_starttime;
		struct tm tm_endtime;
		struct tm tm_time;

		DT_DateTimeHex2TimeShort(&TestRunStartTime, &tm_starttime);
		printf("year = %d, mon = %d, day = %d, hour = %d, min = %d, sec = %d\n", tm_starttime.tm_year, tm_starttime.tm_mon, tm_starttime.tm_mday, tm_starttime.tm_hour, tm_starttime.tm_min, tm_starttime.tm_sec);

		tm_endtime.tm_year = tm_starttime.tm_year;
		tm_endtime.tm_mon = tm_starttime.tm_mon;
		tm_endtime.tm_mday = tm_starttime.tm_mday + 1;
		tm_endtime.tm_hour = 0;
		tm_endtime.tm_min = 0;
		tm_endtime.tm_sec = 0;
		printf("year = %d, mon = %d, day = %d, hour = %d, min = %d, sec = %d\n", tm_endtime.tm_year, tm_endtime.tm_mon, tm_endtime.tm_mday, tm_endtime.tm_hour, tm_endtime.tm_min, tm_endtime.tm_sec);
		printf("timelimits1 = %d, timelimits2 = %d\n", timelimits1, timelimits2);

		No = ((23 - tm_starttime.tm_hour) * 60 + (60 - tm_starttime.tm_min)) / 5;
		int len;
		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x20000200, sizeof(OOP_WORD3V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);

			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}

				tVoltage.nNum = 3;
				tVoltage.rsv = i;
				
				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tVoltage.nValue[0] = 2200 + tm_time.tm_mday;
				tVoltage.nValue[1] = 2100 + tm_time.tm_hour;
				tVoltage.nValue[2] = 2000 + tm_time.tm_min;
				memcpy(temp, &tVoltage, sizeof(OOP_WORD3V_T));

				//DPRINT_MSG(temp, sizeof(OOP_DWORD3V_T), "temp:  ");
				//DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_DWORD3V_T), "record:");
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_WORD3V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_WORD3V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_WORD3V_T), "record:");
					g_readrecordErrorNum++;
				}

				//j++;
			}
		}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x20010200, sizeof(OOP_INT3V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);

			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tCurrent.nNum = 3;
				tCurrent.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tCurrent.nValue[0] = 10000 + tm_time.tm_mday;
				tCurrent.nValue[1] = 11000 + tm_time.tm_hour;
				tCurrent.nValue[2] = 22000 + tm_time.tm_min;
				memcpy(temp, &tCurrent, sizeof(OOP_INT4V_T));

				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x20040200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				activepower.nNum = 4;
				activepower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				activepower.nValue[0] = 100000 + tm_time.tm_mday;
				activepower.nValue[1] = 110000 + tm_time.tm_hour;
				activepower.nValue[2] = 220000 + tm_time.tm_min;
				activepower.nValue[3] = 220000 + tm_time.tm_sec;
				memcpy(temp, &activepower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x20050200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				Reactivepower.nNum = 4;
				Reactivepower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				Reactivepower.nValue[0] = 200000 + tm_time.tm_mday;
				Reactivepower.nValue[1] = 210000 + tm_time.tm_hour;
				Reactivepower.nValue[2] = 320000 + tm_time.tm_min;
				Reactivepower.nValue[3] = 320000 + tm_time.tm_sec;
				memcpy(temp, &Reactivepower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x20060200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				Apparentpower.nNum = 4;
				Apparentpower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				Apparentpower.nValue[0] = 300000 + tm_time.tm_mday;
				Apparentpower.nValue[1] = 310000 + tm_time.tm_hour;
				Apparentpower.nValue[2] = 420000 + tm_time.tm_min;
				Apparentpower.nValue[3] = 420000 + tm_time.tm_sec;
				memcpy(temp, &Apparentpower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x200A0200, sizeof(OOP_LONG4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tFactor.nNum = 4;
				tFactor.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tFactor.nValue[0] = 99000 + tm_time.tm_mday;
				tFactor.nValue[1] = 99900 + tm_time.tm_hour;
				tFactor.nValue[2] = 100000 + tm_time.tm_min;
				tFactor.nValue[3] = 105000 + tm_time.tm_sec;
				memcpy(temp, &tFactor, sizeof(OOP_LONG4V_T));
				
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_LONG4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_LONG4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_LONG4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x00100200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 55000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 22000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 33000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 44000 + tm_time.tm_min;
				tEnergy.nValue[4] = 11000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x00200200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 66000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 33000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 44000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 55000 + tm_time.tm_min;
				tEnergy.nValue[4] = 22000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));

				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x00300200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 77000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 44000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 55000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 66000 + tm_time.tm_min;
				tEnergy.nValue[4] = 33000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50020200, 0x00400200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}
				
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 99000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 55000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 66000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 77000 + tm_time.tm_min;
				tEnergy.nValue[4] = 44000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		char temp1[15];
		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0, 0x31050200, 533, mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], timelimits1, timelimits2);
					continue;
					}

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				
				sprintf(temp1, "%04d", i);
				temp1[4] = tm_time.tm_mon;
				temp1[5] = tm_time.tm_mday;
				temp1[6] = tm_time.tm_hour;
				temp1[7] = tm_time.tm_min;
				temp1[8] = tm_time.tm_sec;
				temp1[9] = 0;
			
				strcpy((char *)&temp, "01000000E407040E08211400E407040E0821140001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000020000000000000000220040008200400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008080000000000000000000000000000000000000000000000000000000000000000000010E407040E08210B00E407040E0821140000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022");

				len = strlen((const char*)&temp);
				commfun_ASC2BCD((char *)&pdata, temp, len, 0);
				//DPRINT_MSG(pdata, len / 2, "temp1:");
				memcpy((char *)&pdata[len / 2], temp1, 9);
				//DPRINT_MSG(pdata, len / 2 + 9, "temp2:");
				
				//DPRINT_MSG(pdata, len / 2 + 9, "temp:  ");
				//DPRINT_MSG(g_outrecord.record[j], len / 2 + 9, "record:");
				if(CompareData((char *)&pdata, g_outrecord.record[j], len / 2 + 9) != 0)
				{
					DPRINT_MSG(pdata, len / 2 + 9, "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], len / 2 + 9, "Err record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x21020200, 0x22000200, 8, mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < g_outrecord.Num; j++)
			{
				if(g_outrecord.savetime[j] >= timelimits1 && g_outrecord.savetime[j] <= timelimits2)
				{
					printf("savetime = %d, timelimits1 = %d, timelimits2 = %d\n", g_outrecord.savetime[j], (int)timelimits1, (int)timelimits2);
					continue;
					}
				
				sprintf(temp, "%04d", i);
				temp[4] = 0;
				strcpy((char *)&temp1, "00001000");
				strcat((char *)&temp1, temp);
				strcpy(temp, "2000");
				strcat((char *)&temp1, temp);
				len = strlen((const char*)&temp1);
				commfun_ASC2BCD((char *)&pdata, temp1, len, 0);
				
				if(CompareData((char *)&pdata, g_outrecord.record[j], len / 2) != 0)
				{
					DPRINT_MSG(pdata, len / 2, "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], len / 2, "record:");
					g_readrecordErrorNum++;
				}
			}
		}

		if(dayFrozen == true)
		{
			for(i = 0; i < 500; i++)
			{
				read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x20000200, sizeof(OOP_WORD3V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);

				for(j = 0; j < No; j++)
				{
					//if()

					tVoltage.nNum = 3;
					tVoltage.rsv = i;
					
					LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
					tVoltage.nValue[0] = 2200 + tm_time.tm_mday;
					tVoltage.nValue[1] = 2100 + tm_time.tm_hour;
					tVoltage.nValue[2] = 2000 + tm_time.tm_min;
					memcpy(temp, &tVoltage, sizeof(OOP_WORD3V_T));
					
					if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_WORD3V_T)) != 0)
					{
						DPRINT_MSG(temp, sizeof(OOP_WORD3V_T), "temp:  ");
						DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_WORD3V_T), "record:");
						g_readrecordErrorNum++;
					}

					j++;
				}
			}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x20010200, sizeof(OOP_INT3V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);

			for(j = 0; j < No; j++)
			{
				tCurrent.nNum = 3;
				tCurrent.rsv = i;
				
				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tCurrent.nValue[0] = 10000 + tm_time.tm_mday;
				tCurrent.nValue[1] = 11000 + tm_time.tm_hour;
				tCurrent.nValue[2] = 22000 + tm_time.tm_min;
				memcpy(temp, &tCurrent, sizeof(OOP_INT3V_T));

				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT3V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT3V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT3V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x20040200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				activepower.nNum = 4;
				activepower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				activepower.nValue[0] = 100000 + tm_time.tm_mday;
				activepower.nValue[1] = 110000 + tm_time.tm_hour;
				activepower.nValue[2] = 220000 + tm_time.tm_min;
				activepower.nValue[3] = 220000 + tm_time.tm_sec;
				memcpy(temp, &activepower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}

		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x20050200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				Reactivepower.nNum = 4;
				Reactivepower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				Reactivepower.nValue[0] = 200000 + tm_time.tm_mday;
				Reactivepower.nValue[1] = 210000 + tm_time.tm_hour;
				Reactivepower.nValue[2] = 320000 + tm_time.tm_min;
				Reactivepower.nValue[3] = 320000 + tm_time.tm_sec;
				memcpy(temp, &Reactivepower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x20060200, sizeof(OOP_INT4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				Apparentpower.nNum = 4;
				Apparentpower.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				Apparentpower.nValue[0] = 300000 + tm_time.tm_mday;
				Apparentpower.nValue[1] = 310000 + tm_time.tm_hour;
				Apparentpower.nValue[2] = 420000 + tm_time.tm_min;
				Apparentpower.nValue[3] = 420000 + tm_time.tm_sec;
				memcpy(temp, &Apparentpower, sizeof(OOP_INT4V_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_INT4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_INT4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_INT4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x200A0200, sizeof(OOP_LONG4V_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				tFactor.nNum = 4;
				tFactor.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tFactor.nValue[0] = 99000 + tm_time.tm_mday;
				tFactor.nValue[1] = 99900 + tm_time.tm_hour;
				tFactor.nValue[2] = 100000 + tm_time.tm_min;
				tFactor.nValue[3] = 105000 + tm_time.tm_sec;
				memcpy(temp, &tFactor, sizeof(OOP_LONG4V_T));
				
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_LONG4V_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_LONG4V_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_LONG4V_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x00100200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 55000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 22000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 33000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 44000 + tm_time.tm_min;
				tEnergy.nValue[4] = 11000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x00200200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 66000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 33000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 44000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 55000 + tm_time.tm_min;
				tEnergy.nValue[4] = 22000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));

				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x00300200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 77000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 44000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 55000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 66000 + tm_time.tm_min;
				tEnergy.nValue[4] = 33000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);

		for(i = 0; i < 500; i++)
		{
			read_record_data_by_infoNum1(0, i+1, 0x50040200, 0x00400200, sizeof(OOP_ENERGYS_T), mktime(&tm_starttime), mktime(&tm_endtime), &g_outrecord);
			
			for(j = 0; j < No; j++)
			{
				tEnergy.nNum = 5;
				tEnergy.rsv = i;

				LinuxTm2DateTimeShort(g_outrecord.savetime[j], &tm_time);
				tEnergy.nValue[0] = 99000 + tm_time.tm_mon;
				tEnergy.nValue[1] = 55000 + tm_time.tm_mday;
				tEnergy.nValue[2] = 66000 + tm_time.tm_hour;
				tEnergy.nValue[3] = 77000 + tm_time.tm_min;
				tEnergy.nValue[4] = 44000 + tm_time.tm_sec;
				memcpy(temp, &tEnergy, sizeof(OOP_ENERGYS_T));
			
				if(CompareData(temp, g_outrecord.record[j], sizeof(OOP_ENERGYS_T)) != 0)
				{
					DPRINT_MSG(temp, sizeof(OOP_ENERGYS_T), "temp:  ");
					DPRINT_MSG(g_outrecord.record[j], sizeof(OOP_ENERGYS_T), "record:");
					g_readrecordErrorNum++;
				}
			}
		}
		if(g_readrecordErrorNum > 0)
			printf("read Error = %d\n", g_readrecordErrorNum);
			
		}

		printf("read Error = %d\n", g_readrecordErrorNum);
	}
}

//int commfun_BcdToHex(char compress_bcd)
//{
//    int rv;
//
//    rv = ((compress_bcd)>>4)*10+(compress_bcd&0x0f);
//    return rv;
//}




int app_deal_StabilityTest_msg(MESSAGE_INFO_T *pInMsg)
{
	void *psendMsg = NULL;
	uint8 tag;

	printf("IOP = %x\n", pInMsg->IOP);
	switch (pInMsg->IOP)
	{
		case 0x200:
			if (StabuilityTestRun[0] == 0)//未执行或已执行
	        {
	        	g_minErrorNum = 0;
	            tag = 1;
	            StabuilityTestRun[0] = 1;
				memcpy((char *)&startwriterecordtime, pInMsg->playload, sizeof(DateTimeBcd_t));
				/*if(FirstRun[0] == true)
				{
					TestRunStartTime = startwriterecordtime;
					FirstRun[0] = false;
					}*/

				/*if(startwriterecordtime.hour == 0 && startwriterecordtime.min == 0)
				{
					TestRunStartTime = startwriterecordtime;
					}*/
	        }
	        else
	        {
	            tag = 0;
	        }
	        pInMsg->msglen = sizeof(uint8);
        	psendMsg = &tag;
			break;
		case 0x201:
			if (StabuilityTestRun[0] == 0)//未执行或已执行
	        {
	            pInMsg->msglen = sizeof(uint16);
	            psendMsg = &g_minErrorNum;
	        }
	        else
	        {
	            tag = 0;
	            pInMsg->msglen = sizeof(uint8);
	            psendMsg = &tag;
	        }
			break;
		case 0x202:
			if (StabuilityTestRun[1] == 0)//未执行或已执行
	        {
	        	g_dayErrorNum = 0;
	            tag = 1;
	            StabuilityTestRun[1] = 1;
				memcpy((char *)&startwriterecordtime, pInMsg->playload, sizeof(DateTimeBcd_t));
	        }
	        else
	        {
	            tag = 0;
	        }
	        pInMsg->msglen = sizeof(uint8);
        	psendMsg = &tag;
			break;
		case 0x203:
			if (StabuilityTestRun[1] == 0)//未执行或已执行
	        {
	            pInMsg->msglen = sizeof(uint16);
	            psendMsg = &g_dayErrorNum;
	        }
	        else
	        {
	            tag = 0;
	            pInMsg->msglen = sizeof(uint8);
	            psendMsg = &tag;
	        }
			break;
		case 0x300:
			if (StabuilityTestRun[2] == 0)//未执行或已执行
	        {
	        	int offset = 0;
	        	g_readrecordErrorNum = 0;
	            tag = 1;
	            StabuilityTestRun[2] = 1;
				memcpy((char *)&startwriterecordtime, pInMsg->playload, sizeof(DateTimeBcd_t));
				offset += sizeof(DateTimeBcd_t);
				if(pInMsg->msglen > sizeof(DateTimeBcd_t) + 1)
				{
					DateTimeBcd_t time;
					memcpy((char *)&time, &pInMsg->playload[offset], sizeof(DateTimeBcd_t));
					offset += sizeof(DateTimeBcd_t);
					DT_DateTimeBCD2Time(&time, (time_t *)&timelimits1);
					memcpy((char *)&time, &pInMsg->playload[offset], sizeof(DateTimeBcd_t));
					DT_DateTimeBCD2Time(&time, (time_t *)&timelimits2);
				}
				else
				{
					timelimits1 = timelimits2 = 0;
				}
				
				TestRunStartTime = startwriterecordtime;
	        }
	        else
	        {
	            tag = 0;
	        }
	        pInMsg->msglen = sizeof(uint8);
        	psendMsg = &tag;
			break;
		case 0x301:
			if (StabuilityTestRun[2] == 0)//未执行或已执行
	        {
	            pInMsg->msglen = sizeof(uint16);
	            psendMsg = &g_readrecordErrorNum;

				remove("/mnt/test/bin/data");
	        }
	        else
	        {
	            tag = 0;
	            pInMsg->msglen = sizeof(uint8);
	            psendMsg = &tag;
	        }
			break;
		default:
	       tag = 0xff;
    	   pInMsg->msglen = sizeof(uint8);
		   psendMsg = &tag;
		   break;
	}

	app_send_msg(pInMsg, psendMsg);

	return 0;
}


/**********************************************************************
* @name      : app_heart_bear
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_send_msg(MESSAGE_INFO_T *pInMsg, void *sendMsg)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint32    len = 0;
    char      uTopic[128] = { 0 };

    len = sizeof(MESSAGE_INFO_T) + pInMsg->msglen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if (NULL == pMsgInfo)
    {
        printf("Heart bear malloc failed %d\n", len);
        return -1;
    }
    memset(pMsgInfo, 0, len);

    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = pInMsg->IID;
    pMsgInfo->IOP = pInMsg->IOP;

    pMsgInfo->sourlen = pInMsg->destlen;
    memcpy(pMsgInfo->souraddr, pInMsg->destaddr, pMsgInfo->sourlen);
    pMsgInfo->destlen = pInMsg->sourlen;
    memcpy(pMsgInfo->destaddr, pInMsg->souraddr, pMsgInfo->destlen);

    pMsgInfo->msglen = pInMsg->msglen;
    memcpy(pMsgInfo->playload, sendMsg, pInMsg->msglen);

    sprintf(uTopic, "%s/%s", pMsgInfo->souraddr, pMsgInfo->destaddr);
    ret = app_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
   return ret;
}

/**********************************************************************
* @name      : set_next_two_time
* @brief     ：设置时钟到次日的23:59:50
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
void set_next_two_time(struct tm *tm_tmp)
{
    //将struct tm结构时间转换成GMT时间time_t
    tm_tmp->tm_sec = 50;
    tm_tmp->tm_min = 59;
    tm_tmp->tm_hour = 23;
    if (tm_tmp->tm_mday > 27)
    {
        tm_tmp->tm_mday = 1;
        if (tm_tmp->tm_mon == 12)
        {
            tm_tmp->tm_mon = 1;
            tm_tmp->tm_year += 1;
        }
        else
        {
            tm_tmp->tm_mon += 1;
        }

    }
    else
    {
        tm_tmp->tm_mday += 1;
    }


    return;
}



/**********************************************************************
* @name      : app_deal_auto_del_msg
* @brief     ：统计类自动删除
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_deal_auto_del_msg(MESSAGE_INFO_T *pInMsg)
{

    int tag;
    pInMsgs = pInMsg;
    switch (pInMsg->IOP)
    {
        printf(" isruns[0]=%d", isruns[0]);
    case 1:

        //if (isruns[0] == 0)//未执行或已执行
        //{

        //  tag = 1;
        //  
        //}
        //else
        //{
        tag = 0;
        isruns[0] = 1;
        /*}*/
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        app_send_msg(pInMsg, psendMsg);

        break;
    case 2:


        if (isruns[0] == 0)//未执行或已执行
        {
            if (psendMsg != NULL)
            {
                pInMsg->msglen = sizeof(uint8);
            }
            else
            {
                tag = 0;
                pInMsg->msglen = sizeof(uint8);
                psendMsg = &tag;
            }

        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }

        app_send_msg(pInMsg, psendMsg);
        break;
    }
    return 0;
}



/**********************************************************************
* @name      : app_deal_auto_del_msg
* @brief     ：事件类自动删除
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_deal_auto_del_msg1(MESSAGE_INFO_T *pInMsg)
{
    int tag;
    pInMsgs = pInMsg;
    switch (pInMsg->IOP)
    {
    case 1:

        //if (isruns[1] == 0)//未执行或已执行
        //{
        //  tag = 1;
        //  
        //}
        //else
        //{
        tag = 0;
        isruns[1] = 1;
        /*}*/
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        app_send_msg(pInMsg, psendMsg);


        break;
    case 2:
        if (isruns[1] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(uint8);

        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        app_send_msg(pInMsg, psendMsg);
        break;
    }
    return 0;
}


/**********************************************************************
* @name      : app_deal_auto_del_msg
* @brief     ：采集类自动删除
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_deal_auto_del_msg2(MESSAGE_INFO_T *pInMsg)
{
    int tag;
    pInMsgs = pInMsg;
    switch (pInMsg->IOP)
    {
    case 1:

        //if (isruns[3] == 0)//未执行或已执行
        //{
        //  tag = 1;
        //  
        //}
        //else
        //{
        tag = 0;
        isruns[3] = 1;
        /*}*/
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        app_send_msg(pInMsg, psendMsg);


        break;
    case 2:
        printf(" isruns[3]=%d", isruns[3]);
        if (isruns[3] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(uint8);
            
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        app_send_msg(pInMsg, psendMsg);
        break;
    }
    return 0;
}


/**********************************************************************
* @name      : app_deal_auto_del_msg
* @brief     ：冻结类自动删除
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_deal_auto_del_msg3(MESSAGE_INFO_T *pInMsg)
{
    int tag;
    pInMsgs = pInMsg;
    switch (pInMsg->IOP)
    {
    case 1:

        //if (isruns[2] == 0)//未执行或已执行
        //{
        //  tag = 1;
        //
        //}
        //else
        //{
        tag = 0;
        isruns[2] = 1;

        /*}*/
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        app_send_msg(pInMsg, psendMsg);


        break;
    case 2:
        if (isruns[2] == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(uint8);

        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        app_send_msg(pInMsg, psendMsg);
        break;
    }
    return 0;
}


/**********************************************************************
* @name      : app_deal_selfhelp_del_msg
* @brief     ：自救删除
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    :
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int app_deal_selfhelp_del_msg(MESSAGE_INFO_T *pInMsg)
{
    int tag;
    //uint32 tmp = 0;
    switch (pInMsg->IOP)
    {
    case 1:
        ISOK = 1;
        tag = 0;
        pInMsg->msglen = sizeof(uint8);
        psendMsg = &tag;
        app_send_msg(pInMsg, psendMsg);
        break;

    case 2:
        printf(" ISOK=%d\n", ISOK);
        if (ISOK == 0)//未执行或已执行
        {
            pInMsg->msglen = sizeof(uint8);
        }
        else
        {
            tag = 0;
            pInMsg->msglen = sizeof(uint8);
            psendMsg = &tag;

        }
        app_send_msg(pInMsg, psendMsg);
        break;
    }
    return 0;
}

