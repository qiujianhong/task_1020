/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析中记录型数据专用存储接口，依赖libStorage.so,共用句柄
* @date： 2019.12.03
*********************************************************************
*/

#include "baseCommon.h"
#include "commClass.h"
#include "oadInfoList.h"
#include "oopservice.h"
#include "oopStorage.h"
#include "oopVarUpdate.h"
#include "area.h"

typedef enum 
{
    SPC_NO = 0,                             //其他
    SPC_OAD_20210200,                       //冻结时间
}SPC_OAD_E;

typedef enum 
{
    REC_OTHER = -1,                         //其他
    REC_EVENT_TMN = 0,                      //终端事件
    REC_EVENT_MTR = 1,                      //电表事件
    REC_FREEZE_TMN,                         //终端冻结
    REC_FREEZE_MTR,                         //电表冻结
    REC_RANGE_TMN,                          //区间统计
    REC_SUM_AVE_TMN,                        //累计平均
    REC_EXTREMUM_TMN,                       //极值统计
    REC_REAL_MTR,                           //实时数据
}RECORD_TYPE_E;

#if DESC("打印信息报文开关函数",1)
/**
*********************************************************************
* @brief：     打印信息报文开关函数
* @param[in]： 
* @param[out]：
* @return：   
*********************************************************************
*/
void diag_ycmsg_open(void)
{
    if ((getenv("DIAG_PRTL") != NULL) && (getenv("DIAG_PRTL_MSG") != NULL))
    {
        if (setenv("DIAG_YCMSG", "1", 1) < 0)
        {
            printf("diag_ycmsg_open err!\n");
        }
    }

    return;
}

/**
*********************************************************************
* @brief：     打印信息报文开关函数
* @param[in]： 
* @param[out]：
* @return：   
*********************************************************************
*/
void diag_ycmsg_close(void)
{
    if (getenv("DIAG_YCMSG") != NULL)
    {
        if (unsetenv("DIAG_YCMSG") < 0)
        {
            printf("diag_ycmsg_close err!\n");
        }
    }

    return;
}
#endif

#if DESC("时间打印函数", 1)
void PRTL_FMT_TIME(char *str, uint32 time)
{
    // 注：-8个小时(28800)转成北京时间
    time_t timeT = time - COMPENSATION_TIME;
    char strT[128] = {0};

    if (str == NULL)
    {
        PRTL_FMT_DEBUG("PRTL_FMT_TIME str == NULL!\n");
        return;
    }

    strftime(strT, sizeof(strT), "%Y-%m-%d %H:%M:%S", localtime(&timeT));
    PRTL_FMT_LOGCC("%s:%s time[s]:%d\n", str, strT, timeT);

    return;
}

/**
*********************************************************************
* @brief：      获取读记录参数
* @param[in]： recordInfo     - 转换前的参数
* @param[out]: readRecord    - 转换后的参数
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_read_param_get(RECORD_INFO_T *recordInfo, READ_RECORD_T *readRecord, uint32 *stepNum)
{
    uint8 *pAddr = NULL;
    uint32 i = 0, j = 0;

    //获取终端地址
    app_get_localaddr(&pAddr);

    //MAC校验为终端地址的前6字节
    readRecord->MAC.nNum = 6;
    memcpy_s(readRecord->MAC.value, 64, pAddr, 6);

    readRecord->recordOAD.infoNum = recordInfo->append.infoNum;
    readRecord->recordOAD.logicId = recordInfo->append.logicId;
    readRecord->recordOAD.road.oadMain.value = recordInfo->oopOad.value;
    readRecord->recordOAD.road.oadCols.nNum = recordInfo->oopRcsd.nNum;

    //默认读冻结记录序号和数据冻结时间
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        if(recordInfo->oopRcsd.cols[i].oad.value == 0x20230200)
        {
            break;
        }
    }

    if(i == recordInfo->oopRcsd.nNum)
    {
        readRecord->recordOAD.road.oadCols.oad[j++].value = 0x20230200;
        readRecord->recordOAD.road.oadCols.nNum++;
    }

    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        if(recordInfo->oopRcsd.cols[i].oad.value == 0x20210200)
        {
            break;
        }
    }

    if(i == recordInfo->oopRcsd.nNum)
    {    
        readRecord->recordOAD.road.oadCols.oad[j++].value = 0x20210200;
        readRecord->recordOAD.road.oadCols.nNum++;
    }

    //关联OAD不存在记录型
    for(i = j; i <  readRecord->recordOAD.road.oadCols.nNum; i++)
    {
        readRecord->recordOAD.road.oadCols.oad[i].value = recordInfo->oopRcsd.cols[i-j].oad.value;
    }

    switch(recordInfo->oopRsd.choice)
    {
        case 0: //[0] NULL
        break;

        case 1:
        {
            if(recordInfo->oopRsd.sel1.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {

                if(recordInfo->oopRsd.sel1.value.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel1.value.type);
                    return ERR_TYPE;    
                }
                
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel1.value.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;
                readRecord->cEnd = readRecord[0].cStart+1;
            }
            else
            {
                PRTL_FMT_DEBUG("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel1.oad.value, recordInfo->oopRsd.sel1.oad.value);
                return ERR_O_RANGE;
            }

            *stepNum = 1;
        }break;

        case 2:
        {
            if(recordInfo->oopRsd.sel2.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                if(recordInfo->oopRsd.sel2.from.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.from.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;

                if(recordInfo->oopRsd.sel2.to.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.to.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.to.tmVal, (time_t*)&readRecord->cEnd);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cEnd += COMPENSATION_TIME;

                //RSD的范围是前闭后开区间，即[起始值，结束值）
                if(readRecord->cStart >= readRecord->cEnd)
                {
                    PRTL_FMT_DEBUG("not support. region[%d, %d)\n", readRecord->cStart, readRecord->cEnd);
                    return ERR_O_RANGE;
                }

                if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.span.type);
                    return ERR_TYPE;    
                }

                memcpy_s(&readRecord->ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel2.span.ti, sizeof(OOP_TI_T));

                *stepNum = 1;
            }
            else
            {
                PRTL_FMT_DEBUG("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel2.oad.value, recordInfo->oopRsd.sel2.oad.value);
                return ERR_O_RANGE;
            }
        }break;        

        case 3:
        {
 #if 0           
            for(i = 0; i < recordInfo->oopRsd.sel3.nNum; i++)
            {
                if(recordInfo->oopRsd.sel3.item[i].oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
                {
                    readRecord[i].cType = COL_TM_STORE;
                    readRecord[i].sortType = DOWN_SORT;
            
                    if(recordInfo->oopRsd.sel3.item[i].from.type != DT_DATETIME_S)
                    {
                        PRTL_FMT_DEBUG("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].from.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].from.tmVal, &readRecord[i].cStart);
            
                    if(recordInfo->oopRsd.sel3.item[i].to.type != DT_DATETIME_S)
                    {
                        PRTL_FMT_DEBUG("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].to.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].to.tmVal, &readRecord[i].cEnd);  
            
                    if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                    {
                        PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel3.item[i].span.type);
                        return ERR_TYPE;    
                    }
            
                    memcpy_s(&readRecord[i].ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel3.item[i].span.ti, sizeof(OOP_TI_T));
                }
                else
                {
                    PRTL_FMT_DEBUG("[%d] not support. oad = %lu(0x%08x)\n", i, recordInfo->oopRsd.sel3.item[i].oad.value, recordInfo->oopRsd.sel3.item[i].oad.value);
                    return ERR_O_RANGE;
                }
            }
 #endif
        }break;  
 
        case 9:
        {
            readRecord->cType = COL_TM_STORE;
            readRecord->sortType = DOWN_SORT;
            readRecord->cStart = 0;
            readRecord->cEnd = time(NULL) + 1 + COMPENSATION_TIME;
        }break;
    }

    return ERR_OK;
}

/**
*********************************************************************
* @brief：      获取读记录参数
* @param[in]： recordInfo     - 转换前的参数
* @param[out]: readRecord    - 转换后的参数
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int transparent_read_param_get(RECORD_INFO_T *recordInfo, READ_RECORD_T *readRecord, uint32 *stepNum,OOP_Transparent_InfoNum_LIST_T *Transparent_InfoNum)
{
    //uint8 *pAddr = NULL;
    uint32 i = 0, j = 0;
    uint8 num;
    OOP_PLAN_TRANS_T PLAN_TRANS_T;
    int ret;
//    OOP_TSA_T         addr;
    uint16 pn=0;
    

  
    //MAC校验为终端地址的前6字节
    readRecord->MAC.nNum = 6;
    
    
   
    
    if((recordInfo->oopRsd.choice==1)||(recordInfo->oopRsd.choice==2))
    {


    }
    PRTL_FMT_DEBUG("transparent_read_param_get\n");
    
    readRecord->recordOAD.infoNum = 0xFFFF;
    readRecord->recordOAD.logicId = recordInfo->append.logicId;
    readRecord->recordOAD.road.oadMain.value = recordInfo->oopOad.value;
    readRecord->recordOAD.road.oadCols.nNum = recordInfo->oopRcsd.nNum;
    readRecord->recordOAD.road.oadCols.nNum = 3;



//    关联OAD不存在记录型
//    for(i = j; i <  readRecord->recordOAD.road.oadCols.nNum; i++)
//    {
//        readRecord->recordOAD.road.oadCols.oad[i].value = recordInfo->oopRcsd.cols[i-j].oad.value;
//
//        readRecord->recordOAD.road.oadCols.oad[i].value = i+1;
//    }

    switch(recordInfo->oopRsd.choice)
    {
        case 0: //[0] NULL
        break;

        case 1:
        {
            if(recordInfo->oopRsd.sel1.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {

                if(recordInfo->oopRsd.sel1.value.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel1.value.type);
                    return ERR_TYPE;    
                }
                
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel1.value.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;
                readRecord->cEnd = readRecord[0].cStart;
            }
            else if(recordInfo->oopRsd.sel1.oad.value == 0x601B0201) //对于冻结来讲，筛选的OAD只支持冻结时间
            {
                if(recordInfo->oopRsd.sel1.value.type != DT_UNSIGNED)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel1.value.type);
                    return ERR_TYPE;    
                }
                readRecord->recordOAD.road.oadMain.value = 0x601A0200|recordInfo->oopRsd.sel1.value.byVal;
                
                PRTL_FMT_DEBUG("readRecord->recordOAD.road.oadMain.value is %x\n",readRecord->recordOAD.road.oadMain.value);

                PRTL_FMT_DEBUG("recordInfo->oopRsd.sel1.value.byVal is %d\n",recordInfo->oopRsd.sel1.value.byVal);
                ret=app_get_plan_transparent_list(recordInfo->oopRsd.sel1.value.byVal,&PLAN_TRANS_T);
                
                Transparent_InfoNum->PLAN_TRANS_T= PLAN_TRANS_T;
                PRTL_FMT_DEBUG("ret is %d\n",ret);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("ret = %d\n", ret);
                    return ret;
                }
                PRTL_FMT_DEBUG("PLAN_TRANS_T.value.nNum is %d\n",PLAN_TRANS_T.value.nNum);
                
                for(i=0;i<PLAN_TRANS_T.value.nNum;i++)
                {
                    pn=app_get_meter_pn_list_by_tsa(PLAN_TRANS_T.value.info[i].addr);

                    PRTL_FMT_DEBUG("pn is %d\n",pn);
                    if(pn !=0xFFFF)
                    {
                        for(j=0;j<PLAN_TRANS_T.value.nNum;j++)
                        {
                            if(pn == Transparent_InfoNum->infomun[j].pn)
                            {
                                break;
                            }
                        }
                        if(j>=PLAN_TRANS_T.value.nNum)
                        {
                            num=Transparent_InfoNum->meternum;
                            memcpy(Transparent_InfoNum->infomun[num].add,PLAN_TRANS_T.value.info[i].addr.add,6);
                            Transparent_InfoNum->infomun[num].pn= pn;
                            Transparent_InfoNum->meternum++;
                        }
                        PRTL_FMT_DEBUG("透明方案结果集22\n");
                    }
                    PRTL_FMT_DEBUG("透明方案结果集11\n");
                }
                PRTL_FMT_DEBUG("透明方案结果集\n");
                 readRecord->recordOAD.road.oadCols.nNum = PLAN_TRANS_T.value.info[0].cmd.nNum;//默认一个方案只有一个方案内容
                    //关联OAD不存在记录型
                PRTL_FMT_DEBUG("方法1 透明方案结果报文条数 readRecord->recordOAD.road.oadCols.nNum is %d\n",readRecord->recordOAD.road.oadCols.nNum);
                for(i = 0; i <  readRecord->recordOAD.road.oadCols.nNum; i++)
                {
                    readRecord->recordOAD.road.oadCols.oad[i].value = PLAN_TRANS_T.value.info[0].cmd.value[i].nIndex;
                    PRTL_FMT_DEBUG("透明方案结果子OAD readRecord->recordOAD.road.oadCols.oad[i].value is %d,i is %d\n",readRecord->recordOAD.road.oadCols.oad[i].value, i);
                }
                readRecord->cType = COL_NO_INFO;
                readRecord->sortType = UP_SORT;
                readRecord->cStart =1;
                readRecord->cEnd =1000;
            }
            else
            {
                PRTL_FMT_DEBUG("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel1.oad.value, recordInfo->oopRsd.sel1.oad.value);
                return ERR_O_RANGE;
            }
           

            *stepNum = 1;
        }break;

        case 2:
        {
            if(recordInfo->oopRsd.sel2.oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
            {
                readRecord->cType = COL_TM_STORE;
                readRecord->sortType = DOWN_SORT;

                if(recordInfo->oopRsd.sel2.from.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.from.tmVal, (time_t*)&readRecord->cStart);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cStart += COMPENSATION_TIME;

                if(recordInfo->oopRsd.sel2.to.type != DT_DATETIME_S)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.to.type);
                    return ERR_TYPE;    
                }

                DT_DateTimeShort2Time(&recordInfo->oopRsd.sel2.to.tmVal, (time_t*)&readRecord->cEnd);
                // 注：+8个小时(28800)转成格林时间
                readRecord->cEnd += COMPENSATION_TIME;

                //注意：数据中心筛选是闭区间，而RSD的范围是前闭后开区间，即[起始值，结束值）
                if(readRecord->cStart >= readRecord->cEnd)
                {
                    PRTL_FMT_DEBUG("not support. region[%d, %d)\n", readRecord->cStart, readRecord->cEnd);
                    return ERR_O_RANGE;
                }
                else
                {
                    readRecord->cEnd--;
                }

                if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.span.type);
                    return ERR_TYPE;    
                }

                memcpy_s(&readRecord->ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel2.span.ti, sizeof(OOP_TI_T));

                *stepNum = 1;
            }
            else if(recordInfo->oopRsd.sel2.oad.value == 0x601B0201) //
            {

                if(recordInfo->oopRsd.sel2.from.type != DT_UNSIGNED)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }

                if(recordInfo->oopRsd.sel2.to.type != DT_UNSIGNED)
                {
                    PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel2.from.type);
                    return ERR_TYPE;    
                }
                readRecord->recordOAD.road.oadMain.value = 0x601A0200|recordInfo->oopRsd.sel2.from.byVal;
                PRTL_FMT_DEBUG("recordInfo->oopRsd.sel2.from.byVal = %d\n", recordInfo->oopRsd.sel2.from.byVal);
                ret=app_get_plan_transparent_list(recordInfo->oopRsd.sel2.from.byVal,&PLAN_TRANS_T);

                Transparent_InfoNum->PLAN_TRANS_T= PLAN_TRANS_T;
                PRTL_FMT_DEBUG("ret2222 is %d\n",ret);
                if(ret != 0)
                {
                    PRTL_FMT_DEBUG("ret = %d\n", ret);
                    return ret;
                }
                PRTL_FMT_DEBUG("PLAN_TRANS_T.value.nNum2222 is %d\n",PLAN_TRANS_T.value.nNum);
                for(i=0;i<PLAN_TRANS_T.value.nNum;i++)
                {
                    pn=app_get_meter_pn_list_by_tsa(PLAN_TRANS_T.value.info[i].addr);
                    PRTL_FMT_DEBUG("pn is %d\n",pn);
                    if(pn !=0xFFFF)
                    {
                        for(j=0;j<PLAN_TRANS_T.value.nNum;j++)
                        {
                            if(pn == Transparent_InfoNum->infomun[j].pn)
                            {
                                break;
                            }
                        }
                        if(j>=PLAN_TRANS_T.value.nNum)
                        {
                            num=Transparent_InfoNum->meternum;
                            memcpy(Transparent_InfoNum->infomun[num].add,PLAN_TRANS_T.value.info[i].addr.add,6);
                            Transparent_InfoNum->infomun[num].pn= pn;
                            Transparent_InfoNum->meternum++;
                        }
                    }

                }
                 readRecord->recordOAD.road.oadCols.nNum = PLAN_TRANS_T.value.info[0].cmd.nNum;//默认一个方案只有一个方案内容
                    //关联OAD不存在记录型
                PRTL_FMT_DEBUG("方法2透明方案结果报文条数 readRecord->recordOAD.road.oadCols.nNum is %d\n",readRecord->recordOAD.road.oadCols.nNum);
                for(i = 0; i <  readRecord->recordOAD.road.oadCols.nNum; i++)
                {
                    readRecord->recordOAD.road.oadCols.oad[i].value = PLAN_TRANS_T.value.info[0].cmd.value[i].nIndex;
                    PRTL_FMT_DEBUG("透明方案结果子OAD readRecord->recordOAD.road.oadCols.oad[i].value is %d,i is %d\n",readRecord->recordOAD.road.oadCols.oad[i].value, i);
                }
                readRecord->cType = COL_NO_INFO;
                readRecord->sortType = UP_SORT;
                readRecord->cStart =1;
                readRecord->cEnd =1000;
            }
            else
            {
                PRTL_FMT_DEBUG("not support. oad = %lu(0x%08x)\n", recordInfo->oopRsd.sel2.oad.value, recordInfo->oopRsd.sel2.oad.value);
                return ERR_O_RANGE;
            }
            readRecord->cType = COL_NO_INFO;
            readRecord->sortType = UP_SORT;
        }break;        

        case 3:
        {
 #if 0           
            for(i = 0; i < recordInfo->oopRsd.sel3.nNum; i++)
            {
                if(recordInfo->oopRsd.sel3.item[i].oad.value == 0x20210200) //对于冻结来讲，筛选的OAD只支持冻结时间
                {
                    readRecord[i].cType = COL_TM_STORE;
                    readRecord[i].sortType = DOWN_SORT;
            
                    if(recordInfo->oopRsd.sel3.item[i].from.type != DT_DATETIME_S)
                    {
                        PRTL_FMT_DEBUG("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].from.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].from.tmVal, &readRecord[i].cStart);
            
                    if(recordInfo->oopRsd.sel3.item[i].to.type != DT_DATETIME_S)
                    {
                        PRTL_FMT_DEBUG("[%d] not support. type = %d\n", i, recordInfo->oopRsd.sel3.item[i].to.type);
                        return ERR_TYPE;    
                    }
            
                    DT_DateTimeShort2Time(&recordInfo->oopRsd.sel3.item[i].to.tmVal, &readRecord[i].cEnd);  
            
                    if((recordInfo->oopRsd.sel2.span.type != DT_TI)&&(recordInfo->oopRsd.sel2.span.type != DT_NULL))
                    {
                        PRTL_FMT_DEBUG("not support. type = %d\n", recordInfo->oopRsd.sel3.item[i].span.type);
                        return ERR_TYPE;    
                    }
            
                    memcpy_s(&readRecord[i].ti, sizeof(OOP_TI_T), &recordInfo->oopRsd.sel3.item[i].span.ti, sizeof(OOP_TI_T));
                }
                else
                {
                    PRTL_FMT_DEBUG("[%d] not support. oad = %lu(0x%08x)\n", i, recordInfo->oopRsd.sel3.item[i].oad.value, recordInfo->oopRsd.sel3.item[i].oad.value);
                    return ERR_O_RANGE;
                }
            }
 #endif
        }break;  
 
        case 9:
        {
            readRecord->cType = COL_TM_STORE;
            readRecord->sortType = DOWN_SORT;
            readRecord->cStart = 0;
            readRecord->cEnd = time(NULL) + COMPENSATION_TIME;
        }break;
    }

    return ERR_OK;
}


#endif 

#if DESC("终端事件记录读取函数", 1)
/**
*********************************************************************
* @brief：      标准事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_std_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_EVENT_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    PRTL_FMT_TRACE(" nIndex(%d)\
                    \r\n tmStart(%04d-%02d-%02d %02d:%02d:%02d)\
                    \r\n tmEnd  (%04d-%02d-%02d %02d:%02d:%02d)\n", 
                    event.nIndex, event.tmStart.year, event.tmStart.month, 
                    event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second,
                    event.tmEnd.year, event.tmEnd.month, 
                    event.tmEnd.day, event.tmEnd.hour, event.tmEnd.minute, event.tmEnd.second);

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);


    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_evt_std(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_std failed. oda(0x%08x), dar(%d)\n", pOadInfo->pTab->oad.value, nRet);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        PRTL_FMT_TRACE("[%d]rcsd.oad(0x%08x)\n",i,oopRcsd.cols[i].oad.value);

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            PRTL_FMT_DEBUG("class_oadinfo_get failed. oad(0x%08x)\n", tmpOAD.value);
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }                
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //TODO 姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.state, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      通信模块变更事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_cmc_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_MODULELOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_modulelog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_modulelog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x33120206:  //模块宿主的通信地址
            {
                if(basic_data_to_buf(E_OOP_OCTETVAR16_T, pOutBuf, tmpOutLen-offset, &offset, &event.add, sizeof(OOP_OCTETVAR16_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }      
            }break;

            case 0x33120207:  //变更前的模块描述符
            {
                if(basic_data_to_buf(E_VISIBLE_STRING, pOutBuf, tmpOutLen-offset, &offset, &event.preID, sizeof(event.preID)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;

            case 0x33120208:  //变更后的模块描述符
            {
                if(basic_data_to_buf(E_VISIBLE_STRING, pOutBuf, tmpOutLen-offset,  &offset, &event.curID, sizeof(event.curID)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
                
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }


        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：      终端停/上电事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_cuton_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_POWERLOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    OOP_DAR_E dar = DATA_SUCCESS;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_powerlog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_powerlog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, event.nIndex);
                    return dar;
                }
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }         
            }break;
            case 0x20240200:   //事件发生源
            case 0x33090204:
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
                
            case 0x33090206: //属性标志 bit-string(SIZE(8))
            {
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen-offset, &offset, &event.flag, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, event.flag.nValue);
                    return dar;
                }                
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);
                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：      发现未知电能表事件单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_smet_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_SMLOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_smlog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_smlog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x33030206:
            case 0x60020200:   //一个搜表结果
            {
                //搜表结果
                pOutBuf[offset++] = DT_ARRAY;

                //数组成员个数
                offlen = set_len_offset(event.nResNum, lenBuf);
                memcpy(&pOutBuf[offset], lenBuf, offlen);
                offset += offlen;
            
                for(j = 0; j < event.nResNum; j++)
                {
                    //一个搜表结果
                    if(data_to_buf_search_met_res(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.Res[j], sizeof(OOP_SEARCHMET_RES_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }
                }               
            }break;
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      系统监测事件,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_sysmonitor_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_MONITORLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {
        nRet = data_to_buf_sysmonitor(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_smlog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x33210206:
            if(basic_data_to_buf(E_ENUM, pOutBuf, tmpOutLen-offset, &offset, &event.warn, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            } 
            case 0x33210207:
            if(basic_data_to_buf(E_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.value, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }             
            case 0x33210208:
            if(basic_data_to_buf(E_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.rate, sizeof(uint16)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            case 0x33210209:
            if(basic_data_to_buf(E_OOP_VISIBLEVAR_T, pOutBuf, tmpOutLen-offset, &offset, &event.containerName, sizeof(event.containerName)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            case 0x3321020a:
            if(basic_data_to_buf(E_OOP_VISIBLEVAR_T, pOutBuf, tmpOutLen-offset, &offset, &event.appName, sizeof(event.appName)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            case 0x3321020b:
            if(basic_data_to_buf(E_OOP_VISIBLEVAR_T, pOutBuf, tmpOutLen-offset, &offset, &event.serviceName, sizeof(event.serviceName)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }           
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：      发现未知电能表事件单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_acrossarea_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_UNMETERLOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    uint8      offlen = 0;
    uint8      lenBuf[3] = {0};

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_acrossarea(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_smlog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x33040206:
            case 0x60020500:   //一个搜表结果
            {
                //搜表结果
                pOutBuf[offset++] = DT_ARRAY;

                //数组成员个数
                offlen = set_len_offset(event.nResNum, lenBuf);
                memcpy(&pOutBuf[offset], lenBuf, offlen);
                offset += offlen;
            
                for(j = 0; j < event.nResNum; j++)
                {
                    //一个跨台区结果
                    if(data_to_buf_acrossarea_met_res(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.Res[j], sizeof(OOP_ACROSSAREA_RES_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }
                }               
            }break;
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      电能表时钟超差记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_clockfly_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_CLOCKFLYLOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_clockflylog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_clockflylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
    
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }          
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x33130206: //电能表时钟
            {
                if(basic_data_to_buf(E_DATETIME_S,  pOutBuf, tmpOutLen-offset, &offset, &event.clkMeter, sizeof(event.clkMeter)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
            case 0x33130207://终端当前时钟
            {
                if(basic_data_to_buf(E_DATETIME_S,  pOutBuf, tmpOutLen-offset, &offset, &event.clkTerm, sizeof(event.clkTerm)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);
                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}


/**
*********************************************************************
* @brief：      电能表数据变更监控记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_metdirty_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_METERDIRTYLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    IC_OAD_INFO_T icInfo;
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    DATA_CONVERT_T convert;
    uint32 presOffset = 0;
    uint32 aftsOffset = 0; 

    MEMZERO(&convert, sizeof(convert));
    
    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_meterdirtylog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_meterdirtylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    convert.dstBuf = pOutBuf;
    convert.dstLen = DATA_LEN_MAX;
    convert.dOffset = &offset;

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }

        icInfo.choice = 1;
        icInfo.pstOdaInfo = &pstOdaInfo;
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x330F0206:   //监控数据对象
            {
                if(basic_data_to_buf(E_CSD, pOutBuf, tmpOutLen-offset, &offset, &event.di, sizeof(OOP_CSD_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            case 0x330F0207:   //变化前数据
            {
                if(event.di.choice == 0)
                {
                    icInfo.choice = 0;
                    icInfo.oad.value = event.di.oad.value;

                    if(icInfo.oad.nIndex != 0) //约定关联OAD的分量取子结构存储
                    {
                        icInfo.subParse = TRUE;
                    }
                    
                    convert.srcBuf = event.pre;
                    convert.srcLen = event.preLen;
                    convert.sOffset = &presOffset;

                    nRet = normal_data_to_fmt698(&icInfo, &convert);  
                    if(nRet != 0)
                    {
                        PRTL_FMT_DEBUG("normal_data2fmt698 failed. oda(0x%08x)", icInfo.oad.value);
                        return nRet;
                    }
                }
            }break;
                
            case 0x330F0208:   //变化后数据
            {
                if(event.di.choice == 0)
                {
                    icInfo.choice = 0;
                    icInfo.oad.value = event.di.oad.value;

                    if(icInfo.oad.nIndex != 0) //约定关联OAD的分量取子结构存储
                    {
                        icInfo.subParse = TRUE;
                    }

                    convert.srcBuf = event.aft;
                    convert.srcLen = event.aftLen;
                    convert.sOffset = &aftsOffset;

                    nRet = normal_data_to_fmt698(&icInfo, &convert); 
                    if(nRet != 0)
                    {
                        PRTL_FMT_DEBUG("normal_data2fmt698 failed. oda(0x%08x)", icInfo.oad.value);
                        return nRet;
                    }                
                }                
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      分项事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_mstd_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_MEVENT_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    //IC_OAD_INFO_T icInfo;
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_mevent(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_mevent failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }

        //icInfo.choice = 1;
        //icInfo.pstOdaInfo = pstOdaInfo;
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;         
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.state, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //后面完善这个 
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      编程事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_prog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_PROGRAMLOG_T event;    
    uint32 i = 0 ,j = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint32 tmpOffset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    PRTL_FMT_TRACE(" nIndex(%d)\
                    \r\n tmStart(%04d-%02d-%02d %02d:%02d:%02d)\
                    \r\n tmEnd  (%04d-%02d-%02d %02d:%02d:%02d)\n", 
                    event.nIndex, event.tmStart.year, event.tmStart.month, 
                    event.tmStart.day, event.tmStart.hour, event.tmStart.minute, event.tmStart.second,
                    event.tmEnd.year, event.tmEnd.month, 
                    event.tmEnd.day, event.tmEnd.hour, event.tmEnd.minute, event.tmEnd.second);

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);


    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_evt_prog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_evt_prog failed. oda(0x%08x), dar(%d)\n", pOadInfo->pTab->oad.value, nRet);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        PRTL_FMT_TRACE("[%d]rcsd.oad(0x%08x)\n",i,oopRcsd.cols[i].oad.value);

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            PRTL_FMT_DEBUG("class_oadinfo_get failed. oad(0x%08x)\n", tmpOAD.value);
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }                
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
                
            case 0x33020206:   //编程对象列表
            {
                if(data_to_buf_evt_proglist(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(OOP_PROGRAMLOG_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

        //关联对象数据
        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      安全事件变更记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_securitylog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    OOP_SECURITYLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_securitylog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_securitylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;   
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x33160206: 
            {
                //异常标志
                dar = basic_data_to_buf(E_OOP_BITSTR16_T, pOutBuf, tmpOutLen, &offset, &event.flag, sizeof(OOP_BITSTR16_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x33160207: 
            {
                //以太网远程端口开放信息
                dar = data_to_buf_evt_ethport(pOadInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(event));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;   
            case 0x33160208: 
            {
                //以太网外联信息
                dar = data_to_buf_evt_ethconnect(pOadInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(event));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;     
            case 0x33160209: 
            {                
                //通信模块端口开放信息
                dar = data_to_buf_evt_lteport(pOadInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(event));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x3316020A: 
            {                 
                //通信模块外联信息
                dar = data_to_buf_evt_lteconnect(pOadInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(event));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;   
            case 0x3316020B: 
            {                 
                //USB设备接入信息
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.usbInfo, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x3316020C: 
            {                
                //本地维护端口接入信息
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.rs232Info, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x3316020D: 
            {                 
                //关键文件目录变更信息
                dar = data_to_buf_evt_chgpath(pOadInfo, pOutBuf, tmpOutLen, &offset, &event, sizeof(event));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            case 0x3316020E: 
            {                  
                //以太网登录控制台信息
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.ethFlag, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            case 0x3316020F: 
            {                 
                //串口登录控制台信息
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.rs232Flag, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;    
            case 0x33160210: 
            {                 
                //非法操作命令信息
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.errFlag, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x33160211: 
            {                 
                //SSH异常
                dar = basic_data_to_buf(E_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.errSSH, sizeof(event.errSSH));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x33160212: 
            {                 
                //流量会话异常
                dar = data_to_buf_security_session(pOadInfo, pOutBuf, tmpOutLen, &offset, &event.errSession, sizeof(event.errSession));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            case 0x33160213: 
            {                 
                //流量带宽异常
                dar = data_to_buf_security_stream(pOadInfo, pOutBuf, tmpOutLen, &offset, &event.errStream, sizeof(event.errStream));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            case 0x33160214: 
            {                 
                //进程异常
                dar = data_to_buf_security_prog(pOadInfo, pOutBuf, tmpOutLen, &offset, &event.errProg, sizeof(event.errProg));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            case 0x33160215: 
            {                 
                //用户登录异常
                dar = data_to_buf_security_login(pOadInfo, pOutBuf, tmpOutLen, &offset, &event.errLogin, sizeof(event.errLogin));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      功控跳闸事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_pctrllog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    OOP_PCTRLLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_pctrllog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_securitylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;   
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x33050206:
            {
                //控后2分钟功率
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.power2Min, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x810E0200: 
            {
                //控制对象
                dar = basic_data_to_buf(E_OI, pOutBuf, tmpOutLen, &offset, &event.objCtrl, sizeof(OI));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x810F0200: 
            {
                //跳闸轮次 0x810F0200
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.turnState, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;              
            case 0x33050209: 
            {
                //功控定值 0x33050209
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.llSetting, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
			case 0x23012300: 
            case 0x3305020A: 
            {            
                //跳闸发生前总加组有功功率 0x3305020A
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.power, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      电控跳闸事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_ectrllog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    OOP_ECTRLLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_ectrllog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_securitylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;   
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x810E0200: 
            {
                //控制对象
                dar = basic_data_to_buf(E_OI, pOutBuf, tmpOutLen, &offset, &event.objCtrl, sizeof(OI));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x810F0200: 
            {
                //跳闸轮次 0x810F0200
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.turnState, sizeof(OOP_BITSTR8_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;              
            case 0x81100200: 
            {
                //电控定值 0x81100200
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.llSetting, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x23014900:
            case 0x33060209: 
            {            
                //跳闸发生时总加组电能量 0x33060209
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.energy, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break; 
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      电控告警事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_ewarnlog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    OOP_EWARNLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_ewarnlog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_securitylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;   
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x810E0200: 
            {
                //控制对象
                dar = basic_data_to_buf(E_OI, pOutBuf, tmpOutLen, &offset, &event.objCtrl, sizeof(OI));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            case 0x81100200: 
            {
                //电控定值 0x81100200
                dar = basic_data_to_buf(E_LONG64, pOutBuf, tmpOutLen, &offset, &event.llSetting, sizeof(int64));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
            
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      遥控跳闸事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_rctrllog_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    OOP_RCTRLLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_rctrllog(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_securitylog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;   
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
            case 0x330A0206:
            {
                //遥控命令类型
                dar = basic_data_to_buf(E_ENUM, pOutBuf, tmpOutLen, &offset, &event.cmd, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
			case 0x330A0207:
            {
                //控后2分钟总加组有功功率
                dar = data_to_buf_evt_clrttgpower(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.prePower, sizeof(event.prePower));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x330A0208: 
            {
                //控时2分钟总加组有功功率
                dar = data_to_buf_evt_clrttgpower(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.llPower, sizeof(event.llPower));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), offset(%d)\n", dar, offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;  
                
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      电流互感器异常事件记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_ct_abnormal_unit2fmt698(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_ABNORMALCTLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    OOP_DAR_E dar = DATA_SUCCESS;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_ct_abnormal(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_powerlog failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, event.nIndex);
                    return dar;
                }
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }         
            }break;
            case 0x20240200:   //事件发生源
            case 0x33180204:   //事件发生源
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
                
            case 0x33180206: //异常标志 bit-string(SIZE(8))
            {
                dar = basic_data_to_buf(E_OOP_BITSTR8_T, pOutBuf, tmpOutLen, &offset, &event.abnmlflg, sizeof(uint8));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), abnmlflg(%d)\n", dar, event.abnmlflg.nValue);
                    return dar;
                }                
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}
/**
*********************************************************************
* @brief：      终端对电表校时记录单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_p2pclock(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_P2PCLOCKLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    OOP_DAR_E dar = DATA_SUCCESS;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_p2pclock(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_p2pclock failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, event.nIndex);
                    return dar;
                }
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }         
            }break;
            case 0x20240200:   //事件发生源
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;
                
            case 0x330D0206: 
            {
                dar = basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.preTime, sizeof(OOP_DATETIME_S_T));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                    return dar;
                }                
            }break;
            case 0x330D0207: 
            {
                dar = basic_data_to_buf(E_INTEGER, pOutBuf, tmpOutLen, &offset, &event.offset, sizeof(char));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d)\n", dar);
                    return dar;
                }                
            }break;
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：      拒绝从节点事件单元,经RCSD筛选后的报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_refusenode(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_REFUSENODEACCESSLOG_T event;    
    uint32 i = 0;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    OOP_DAR_E dar = DATA_SUCCESS;

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_refusenode(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_refusenode failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen, &offset, &event.nIndex, sizeof(uint32));
                if(dar != DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, event.nIndex);
                    return dar;
                }
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }         
            }break;
            case 0x20240200:   //事件发生源
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
                {
                    return ERR_NORMAL;
                }
                
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }

   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}
int event_tmn_identify(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32 nRet = ERR_OK;
    OOP_IdentifyResult_T event;    
    uint32 i = 0 ;
    OOP_OAD_U tmpOAD;
    uint32 offset = 0;
    uint8 *pOutBuf  = NULL;
    uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
    OAD_INFO_T pstOdaInfo = {0};  
    OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
    //uint8      offlen = 0;
    //uint8      lenBuf[3] = {0};

    memcpy(&event, unit.record[0].outDataBuf, sizeof(event));

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
    memset(pOutBuf, 0, DATA_LEN_MAX);

    //NORMAL方法读，或者RECORD读出所有列
    if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
    {

        nRet = data_to_buf_identify(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
        if(nRet != 0)
        {
            PRTL_FMT_DEBUG("data_to_buf_identify failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
            return nRet;
        }

        if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
        {
            memmove(pOutBuf, pOutBuf + 2, offset - 2);
            *outLen = offset - 2;
        }
        else
        {
            *outLen = offset;
        }

        *ppOutBuf = pOutBuf;
        
        return nRet;
    }

    for(i = 0; i < oopRcsd.nNum; i++)
    {
        tmpOAD = oopRcsd.cols[i].oad;

        if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
        {
            return ERR_NORMAL;
        }
        
        switch(tmpOAD.value)
        {
            case 0x20220200:   //事件记录序号
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;
            case 0x201E0200:   //事件发生时间
            {
                if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x20200200:   //事件结束时间
            {
                if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                {
                   pOutBuf[offset++] = DT_NULL; 
                }
                else
                {
                    if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                    {
                        nRet = ERR_NORMAL;
                        return nRet;
                    }  
                }           
            }break;
            case 0x20240200:   //事件发生源
            {
                if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                } 
            }break;            
            case 0x33000200:   //事件上报状态
            {
                if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }             
            }break;

            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
        }
   }
    
    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

int event_tmn_pf_input_exit(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
     int32 nRet = ERR_OK;
     OOP_PF_INPUT_EXIT_T event;   
     uint32 i = 0,j = 0;
     OOP_OAD_U tmpOAD;
     uint32 offset = 0;
     uint32 tmpOffset = 0;
     uint8 *pOutBuf  = NULL;
     uint32 tmpOutLen = DATA_LEN_MAX; //TODO 暂时这么定
     OAD_INFO_T pstOdaInfo = {0};  
     OOP_RCSD_T oopRcsd = relyInfo->oopRcsd;
     //uint8      offlen = 0;
     //uint8      lenBuf[3] = {0};
    
     memcpy(&event, unit.record[0].outDataBuf, sizeof(event));
    
     pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//TODO 暂时这么定
     memset(pOutBuf, 0, DATA_LEN_MAX);
    
     //NORMAL方法读，或者RECORD读出所有列
     if((relyInfo->method == 1) || (oopRcsd.nNum == 0))
     {
    
         nRet = data_to_buf_pf_input_exit(pOadInfo, pOutBuf, DATA_LEN_MAX, &offset, &event, sizeof(event));
         if(nRet != 0)
         {
             PRTL_FMT_DEBUG("data_to_buf_identify failed. oda(0x%08x)\n", pOadInfo->pTab->oad.value);
             return nRet;
         }
    
         if(relyInfo->method != 1) //非NORAML方法，去掉STRUCTER类型和长度
         {
             memmove(pOutBuf, pOutBuf + 2, offset - 2);
             *outLen = offset - 2;
         }
         else
         {
             *outLen = offset;
         }
    
         *ppOutBuf = pOutBuf;
         
         return nRet;
     }
    
     for(i = 0; i < oopRcsd.nNum; i++)
     {
         tmpOAD = oopRcsd.cols[i].oad;
    
         if(!class_oadinfo_get(tmpOAD, &pstOdaInfo))
         {
             return ERR_NORMAL;
         }
         
         switch(tmpOAD.value)
         {
             case 0x20220200:   //事件记录序号
             {
                 if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.nIndex, sizeof(uint32)) < 0)
                 {
                     nRet = ERR_NORMAL;
                     return nRet;
                 } 
             }break;
             case 0x201E0200:   //事件发生时间
             {
                 if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmStart, sizeof(OOP_DATETIME_S_T)) < 0)
                 {
                     nRet = ERR_NORMAL;
                     return nRet;
                 }
             }break;
             case 0x20200200:   //事件结束时间
             {
                 if(event.tmEnd.year == 0) //姑且使用0作为不存在的判据
                 {
                    pOutBuf[offset++] = DT_NULL; 
                 }
                 else
                 {
                     if(basic_data_to_buf(E_DATETIME_S, pOutBuf, tmpOutLen-offset, &offset, &event.tmEnd, sizeof(OOP_DATETIME_S_T)) < 0)
                     {
                         nRet = ERR_NORMAL;
                         return nRet;
                     }  
                 }           
             }break;
             case 0x20240200:   //事件发生源
             {
                 if(data_to_buf_evt_source(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.source, sizeof(OOP_EVTSOURCE_T)) < 0)
                 {
                     nRet = ERR_NORMAL;
                     return nRet;
                 } 
             }break;            
             case 0x33000200:   //事件上报状态
             {
                 if(data_to_buf_evt_report(&pstOdaInfo, pOutBuf, tmpOutLen-offset, &offset, &event.evtState, sizeof(OOP_REPORTS_T)) < 0)
                 {
                     nRet = ERR_NORMAL;
                     return nRet;
                 }
             }break;
            case 0x33420600:    //动作原因
            {
                if(basic_data_to_buf(E_UNSIGNED, pOutBuf, tmpOutLen-offset, &offset, &event.actCase, sizeof(uint8)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }break;
            case 0x33420700:                //上一次投入使用的定值
            {
                if(apdu_variant_tofmt698(pOutBuf, tmpOutLen-offset, &offset, &event.constBef,sizeof(OOP_VARIANT_T))!= DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed.offset(%d)\n", offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x33420800:                //当前投入使用的定值
            {
                if(apdu_variant_tofmt698(pOutBuf, tmpOutLen-offset, &offset, &event.constCur,sizeof(OOP_VARIANT_T))!= DATA_SUCCESS)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed.offset(%d)\n", offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
            case 0x33420900:                //投退方式
            {
                if(basic_data_to_buf(E_ENUM, pOutBuf, tmpOutLen-offset, &offset, &event.prodroType, sizeof(uint8)) < 0)
                {
                    PRTL_FMT_DEBUG("basic_data_to_buf failed.offset(%d)\n", offset);
                    return DATA_TYPE_UNMATCHED;
                }
            }break;
                
            default: 
            {
                pOutBuf[offset++] = DT_NULL;
            }break;
         }
        tmpOffset = 0;
        for(j = 0; j < event.data.oadNum; j++)
        {
            if(tmpOAD.value == event.data.cols.oad[j].value)
            {
                //nRet = normal_data2fmt698(icInfo, &offset, &event.data.buf[tmpOffset], event.data.idLen[j], pOutBuf, tmpOutLen-offset);
                nRet = data_to_buf_evt_oad(tmpOAD, pOutBuf, tmpOutLen-offset, &offset, &event.data.buf[tmpOffset], event.data.idLen[j]);

                break;
            }

            tmpOffset += event.data.idLen[j];
        }
    }
     
     *ppOutBuf = pOutBuf;
     *outLen = offset;
    
     return nRet;

}

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
int event_tmn_unit2fmt698(RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    int32     nRet = ERR_NORMAL;
    OAD_INFO_T *pOadInfo = NULL;

    if ((unit.record[0].road.oadCols.nNum == 0)&&(unit.record[0].road.oadMain.value == 0x0))
    {
        return ERR_OPTION;
    }

    //TODO 若查到OAD为 DT_ARRAY 则 pOadInfo++
    pOadInfo = (OAD_INFO_T *)relyInfo->pOadInfo;
   
    //列举所有的事件结构体
    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVENT_T:        //标准事件记录单元
        {
            return event_tmn_std_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;       
        case E_OOP_MODULELOG_T:    //通信模块变更事件记录单元
        {
            return event_tmn_cmc_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;    
        case E_OOP_POWERLOG_T:     //终端停/上电事件记录单元
        {
            return event_tmn_cuton_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break; 
        case E_OOP_SMLOG_T:        //发现未知电能表事件单元
        {
            return event_tmn_smet_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break; 
        case E_OOP_CLOCKFLYLOG_T:  //电能表时钟超差记录单元
        {
            return event_tmn_clockfly_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break; 
        case E_OOP_METERDIRTYLOG_T://电能表数据变更监控记录单元
        {
            return event_tmn_metdirty_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;
        case E_OOP_PROGRAMLOG_T: //编程事件记录单元
        {
            return event_tmn_prog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;        
        case E_OOP_MEVENT_T:        //分项事件记录单元
        {
            return event_tmn_mstd_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  
        case E_OOP_SECURITYLOG_T:   //安全事件变更记录单元
        {
            return event_tmn_securitylog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;     
         case E_OOP_RCTRLLOG_T:   //遥控跳闸事件记录单元
        {
            return event_tmn_rctrllog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  
        case E_OOP_PCTRLLOG_T:   //功控跳闸事件记录单元
        {
            return event_tmn_pctrllog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  
        case E_OOP_ECTRLLOG_T:   //电控跳闸事件记录单元
        {
            return event_tmn_ectrllog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  
        case E_OOP_EWARNLOG_T:   //电控告警事件记录单元
        {
            return event_tmn_ewarnlog_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  

        case E_OOP_ABNORMALCTLOG_T:   //电流互感器异常记录单元
        {
            return event_tmn_ct_abnormal_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;     
        case E_OOP_P2PCLOCKLOG_T:   //终端对电表校时记录单元
        {
            return event_tmn_p2pclock(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;
        case E_OOP_REFUSENODEACCESSLOG_T:
        {
            return event_tmn_refusenode(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;
        case E_OOP_IdentifyResult_T:
        {
            return event_tmn_identify(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;
            
        case E_OOP_PF_INPUT_EXIT_T: //保护功能投退事件
        {
            return event_tmn_pf_input_exit(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;
        case E_OOP_UNMETERLOG_T:
        {
            return event_tmn_acrossarea_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }
        break;
        case E_OOP_SYSTEMMONITOR_T:
        {
            return event_tmn_sysmonitor_unit2fmt698(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }
#ifdef AREA_HUNAN
        case E_OOP_IMPE_OVER_T:
        {
            return event_tmn_impeover(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
        }break;  
#endif

#ifdef AREA_SHAANXI
		case E_OOP_ICCIDLOG_T:
		{
			return event_tmn_ICCID(pOadInfo, unit, relyInfo, ppOutBuf, outLen);
		}break;  
#endif

        default:
        {
            PRTL_FMT_DEBUG("event read failed data type is not support. type(%d)\n", pOadInfo->pTab->eData);
            return ERR_INVALID;   
        }break;
    }

    return nRet;
}
/**
*********************************************************************
* @brief：      读取全部记录(终端事件)
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_tmn_event_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      *stepRecordOut = NULL;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0; //输出报文的偏移
    uint32             numOffset = 0; //写记录数的偏移
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    uint32 outRecNumAll = 0;  //总的要输出的记录条数

    int32  ret    = ERR_NORMAL;
    uint32 outLen = 0;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 outRecNum = 0; //实际输出的记录条数
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    int i = 0;

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        PRTL_FMT_DEBUG("record_event_tmn_read param err!,hudp(%d), outRecordBuf(%d), recordBuf(%d)\n",
            hUdp, outRecordBuf, outRecordBuf->recordBuf);
        return ERR_PTR;
    }
    
    PRTL_FMT_TRACE("\r\ninReadRecord.recordOAD.logicId = %d, inReadRecord.recordOAD.infoNum = %d, inReadRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\ninReadRecord.recordOAD.road.oadCols.nNum = %d \n",
          inReadRecord->recordOAD.logicId, inReadRecord->recordOAD.infoNum, 
          inReadRecord->recordOAD.road.oadMain, inReadRecord->recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < inReadRecord->recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_TRACE("inReadRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, inReadRecord->recordOAD.road.oadCols.oad[i].value, inReadRecord->recordOAD.road.oadCols.oad[i].value);
    }

    for(i = 0; i < 6; i++)
    {
        PRTL_FMT_TRACE("inReadRecord.MAC[%d] = %02x\n", i, inReadRecord->MAC.value[i]);
    }

    PRTL_FMT_TRACE("inReadRecord.ti.unit = %d, inReadRecord.ti.value = %d\n", inReadRecord->ti.unit, inReadRecord->ti.value);

    PRTL_FMT_TRACE("relyInfo->method(%d), relyInfo->pOadInfo->nIndex(%d), relyInfo->oopRcsd.nNum(%d)\n", relyInfo->method, relyInfo->pOadInfo->nIndex, relyInfo->oopRcsd.nNum);
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        PRTL_FMT_TRACE("relyInfo->oopRcsd.cols[%d].oad = %lu(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value,  relyInfo->oopRcsd.cols[i].oad.value);
    }

    PRTL_FMT_TRACE("relyInfo->oopRsd.choice(%d)\n", relyInfo->oopRsd->choice);
    
    switch(relyInfo->oopRsd->choice)
    {
        case 2: PRTL_FMT_TRACE("sel2.oad = %lu(0x%08x)\n", relyInfo->oopRsd->sel2.oad.value, relyInfo->oopRsd->sel2.oad.value); 

        case 9: PRTL_FMT_TRACE("sel9.nLast = %d\n", relyInfo->oopRsd->sel9.nLast); 
        
        default:break;
    }

    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));

    //断点处理
    if(relyInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        //倒序，则截止到上次中断的时间点,前闭后开，上次结束的时间加1
        if(inReadRecord->sortType == DOWN_SORT)
        {
            inReadRecord->cEnd = relyInfo->bpBreakInfo->tmStartSeg + 1;    
        }
        else
        {
            inReadRecord->cStart = relyInfo->bpBreakInfo->tmStartSeg;
        }
    }

    PRTL_FMT_TRACE("inReadRecord.tmType = %d, inReadRecord.tmStart = %d, inReadRecord.tmEnd = %d, inReadRecord.sortType = %d\n",
          inReadRecord->cType, inReadRecord->cStart, inReadRecord->cEnd, inReadRecord->sortType);
    
    /*开始读记录*/
    ret = db_read_record_start(hUdp, inReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    //MAC校验
    memcpy(&stepReadRecord.MAC, &inReadRecord->MAC, sizeof(OOP_OCTETVAR64_T));

    //实际能输出的记录条数，取数据中心读出的数据条数与主站需要的记录条数的小者,要不要报错？  
    outRecNumAll = MIN(recNum, readRecNum); 
    PRTL_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), readRecNum(%lu)\n", outRecNumAll, recNum, readRecNum);

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //普通非分量数据需要数组类型
        if(relyInfo->method == 1)
        {
            outRecordBuf->recordBuf[offset++] = DT_ARRAY;
        }

        numOffset = offset;  //记录数或者数组成员数的位置

        /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
        outRecordBuf->recordBuf[offset++] = outRecNumAll;   //记录数先存实际条目 

        bufAllLen = offset;
    }

    //变量太大，会使栈空间溢出，破坏调用者的栈，所以改用堆分配
    stepRecordOut = (RECORD_UNIT_T*)malloc(sizeof(RECORD_UNIT_T));
    memset(stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    
    for (i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, stepRecordOut);
        if ((ret != ERR_OK) || (stepRecordOut->recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepReadRecord.MAC.value, 6, "db_read_record_step failed. ret(%d), outLen(%d), mainOAD(0x%08x)\n", 
                ret, outLen, inReadRecord->recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(relyInfo->oopRsd->choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(*inReadRecord, *stepRecordOut);
            if (ret != ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((relyInfo->oopRsd->choice == 9) && (i+1 != relyInfo->oopRsd->sel9.nLast))
        {
            continue;
        }
        
        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/
        ret = event_tmn_unit2fmt698(*stepRecordOut, relyInfo, &pBufTmp, &bufLenTmp);
        if (ret != ERR_OK)
        {
            db_read_record_end(hUdp, handle);
            FREE_S(pBufTmp);  
            FREE_S(stepRecordOut);  
            return ERR_NORMAL;
        }

        bufAllLenLast = bufAllLen; //存储上次的长度

        bufAllLen += bufLenTmp;    //更新本次的长度

        if(bufAllLen <= maxBufLen)
        {
            //在长度累积大于maxBufLen后，这个拷贝会越界
            memcpy(&outRecordBuf->recordBuf[bufAllLenLast], pBufTmp, bufLenTmp);
        }
        else //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        {
            outRecordBuf->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (inReadRecord->cType)
            {
                case COL_TM_START:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colStartTm;
                    break;
                case COL_TM_END:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colEndTm;
                    break;
                case COL_TM_STORE:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut->colStoreTm;
                    break;       
                default:
                    FREE_S(pBufTmp);
                    FREE_S(stepRecordOut);  
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            outRecordBuf->breakInfo.appSlicingFlag = 1;

            break;
        }

        //正常跳出，为已经处理的总记录数，分帧跳出，为已经处理的记录数
        outRecNum++;

        FREE_S(pBufTmp);    
    }

    FREE_S(pBufTmp); 
    FREE_S(stepRecordOut);

    /*结束读记录*/
    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        outRecordBuf->bufLen = bufAllLen;
        //outRecordBuf->recordBuf[numOffset] = outRecNumAll;
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        outRecordBuf->breakInfo.appSlicingFlag = 0x01;
        outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        
        outRecordBuf->bufLen = bufAllLenLast;
        //outRecordBuf->recordBuf[numOffset] = i; //跳出来前，已经处理的记录条数
    }

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //长度域超过后也要采用A-XDR编码
        if (outRecNum > 0x80)
        {
            offlen = set_len_offset(outRecNum, lenBuf);
            memmove(&outRecordBuf->recordBuf[numOffset+offlen], &outRecordBuf->recordBuf[numOffset+1], outRecordBuf->bufLen - numOffset - 1);
            memcpy(&outRecordBuf->recordBuf[numOffset], lenBuf, offlen);
            outRecordBuf->bufLen += (offlen - 1); //减掉原来预留的1字节长度
        }
        else
        {
            outRecordBuf->recordBuf[numOffset] = outRecNum;   //填写总记录数 
        }    
    }
    else if(outRecNum == 0) //非分量，且记录条数为0，则返回NULL
    {
        outRecordBuf->recordBuf[offset++] = DT_NULL;
        outRecordBuf->bufLen++;
    }
    
    PRTL_FMT_TRACE("record_event_tmn_read result: outRecNum(%lu), appSlicingFlag(%d), isEnd(%d)\n", outRecNum, outRecordBuf->breakInfo.appSlicingFlag, outRecordBuf->breakInfo.isEnd);

    return ERR_OK;
}

#endif

#if DESC("终端冻结记录读取函数",1)

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E sub_oad_unit2fmt698(RECORD_UNIT_T *unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint8      *pOutBuf  = NULL;
    uint32 i= 0 ,j = 0;
    DateTimeHex_t oopTime_s;

    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    MEMZERO(&oopTime_s, sizeof(DateTimeHex_t));
   
    icInfo.choice = 0;
    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//暂时这么定
    MEMZERO(pOutBuf, DATA_LEN_MAX);

    //普通方法还要返回结构成员和结构成员数
    if(relyInfo->method == 1)
    {
        pOutBuf[dOffset++] = DT_STRUCTURE;
        pOutBuf[dOffset++] = relyInfo->oopRcsd.nNum;
    }

    convert.dstBuf = pOutBuf;
    convert.dstLen = DATA_LEN_MAX;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    //每次解析其中一个OAD
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        
        for(j = 0; j < unit->recordnum; j++)
        {
            if(unit->record[j].road.oadCols.oad[0].value != relyInfo->oopRcsd.cols[i].oad.value)
            {
                continue;
            }

            icInfo.oad.value = unit->record[j].road.oadCols.oad[0].value;
            convert.srcBuf = unit->record[j].outDataBuf;
            convert.srcLen = unit->record[j].outDatalen;

            dar = normal_data_to_fmt698(&icInfo, &convert);
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("[%d] normal_data_to_fmt698 failed. oad(0x%08x)\n", i, icInfo.oad.value);
                return dar;
            }

            break;
        }

        if(j == unit->recordnum)
        {
            pOutBuf[dOffset++] = DT_NULL;
            PRTL_FMT_DEBUG("[%d] not find objs, data is null. oad(0x%08x)\n", i, icInfo.oad.value);
        }

        PRTL_BUF_TRACE(pOutBuf, dOffset, "[%d] oadCols(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value);
    }

    *ppOutBuf = pOutBuf;
    *outLen = dOffset;

    return dar;
}


/**
*********************************************************************
* @brief：      读取全部记录(终端冻结)
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_tmn_frozen_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0; //输出报文的偏移
    uint32             numOffset = 0; //写记录数的偏移
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    uint32 outRecNumAll = 0;  //总的要输出的记录条数

    int32  ret    = ERR_NORMAL;
    uint32 outLen = 0;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 outRecNum = 0; //实际输出的记录条数
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    int i = 0;

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        PRTL_FMT_DEBUG("record_tmn_frozen_read param err!,hudp(%d), outRecordBuf(%d), recordBuf(%d)\n",
            hUdp, outRecordBuf, outRecordBuf->recordBuf);
        return ERR_PTR;
    }
    
    PRTL_FMT_TRACE("\r\ninReadRecord.recordOAD.logicId = %d, inReadRecord.recordOAD.infoNum = %d, inReadRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\ninReadRecord.recordOAD.road.oadCols.nNum = %d \n",
          inReadRecord->recordOAD.logicId, inReadRecord->recordOAD.infoNum, 
          inReadRecord->recordOAD.road.oadMain, inReadRecord->recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < inReadRecord->recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_TRACE("inReadRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, inReadRecord->recordOAD.road.oadCols.oad[i].value, inReadRecord->recordOAD.road.oadCols.oad[i].value);
    }

    for(i = 0; i < 6; i++)
    {
        PRTL_FMT_TRACE("inReadRecord.MAC[%d] = %02x\n", i, inReadRecord->MAC.value[i]);
    }

    PRTL_FMT_TRACE("inReadRecord.ti.unit = %d, inReadRecord.ti.value = %d\n", inReadRecord->ti.unit, inReadRecord->ti.value);

    PRTL_FMT_TRACE("relyInfo->method(%d), relyInfo->pOadInfo->nIndex(%d), relyInfo->oopRcsd.nNum(%d)\n", relyInfo->method, relyInfo->pOadInfo->nIndex, relyInfo->oopRcsd.nNum);
    for(i = 0; i < relyInfo->oopRcsd.nNum; i++)
    {
        PRTL_FMT_TRACE("relyInfo->oopRcsd.cols[%d].oad = %lu(0x%08x)\n", i, relyInfo->oopRcsd.cols[i].oad.value,  relyInfo->oopRcsd.cols[i].oad.value);
    }

    PRTL_FMT_TRACE("relyInfo->oopRsd.choice(%d)\n", relyInfo->oopRsd->choice);
    
    switch(relyInfo->oopRsd->choice)
    {
        case 2: PRTL_FMT_TRACE("sel2.oad = %lu(0x%08x)\n", relyInfo->oopRsd->sel2.oad.value, relyInfo->oopRsd->sel2.oad.value); 

        case 9: PRTL_FMT_TRACE("sel9.nLast = %d\n", relyInfo->oopRsd->sel9.nLast); 
        
        default:break;
    }

    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));

    //断点处理
    if(relyInfo->bpBreakInfo->appSlicingFlag == 1)
    {
        //倒序，则截止到上次中断的时间点,前闭后开，上次结束的时间加1
        if(inReadRecord->sortType == DOWN_SORT)
        {
            inReadRecord->cEnd = relyInfo->bpBreakInfo->tmStartSeg + 1;    
        }
        else
        {
            inReadRecord->cStart = relyInfo->bpBreakInfo->tmStartSeg;
        }
    }

    PRTL_FMT_TRACE("inReadRecord.tmType = %d, inReadRecord.tmStart = %d, inReadRecord.tmEnd = %d, inReadRecord.sortType = %d\n",
          inReadRecord->cType, inReadRecord->cStart, inReadRecord->cEnd, inReadRecord->sortType);
    
    /*开始读记录*/
    ret = db_read_record_start(hUdp, inReadRecord, &handle, &recNum);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    //MAC校验
    memcpy(&stepReadRecord.MAC, &inReadRecord->MAC, sizeof(OOP_OCTETVAR64_T));

    //实际能输出的记录条数，取数据中心读出的数据条数与主站需要的记录条数的小者,要不要报错？  
    outRecNumAll = MIN(recNum, readRecNum); 
    PRTL_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), readRecNum(%lu)\n", outRecNumAll, recNum, readRecNum);

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //普通非分量数据需要数组类型
        if(relyInfo->method == 1)
        {
            outRecordBuf->recordBuf[offset++] = DT_ARRAY;
        }

        numOffset = offset;  //记录数或者数组成员数的位置

        /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
        outRecordBuf->recordBuf[offset++] = outRecNumAll;   //记录数先存实际条目 

        bufAllLen = offset;
    }
    
    for (i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
        if ((ret < ERR_OK) || (stepRecordOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepReadRecord.MAC.value, 6, "db_read_record_step failed. ret(%d), outLen(%d), mainOAD(0x%08x)\n", 
                ret, outLen, inReadRecord->recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(relyInfo->oopRsd->choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(*inReadRecord, stepRecordOut);
            if (ret < ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((relyInfo->oopRsd->choice == 9) && (i+1 != relyInfo->oopRsd->sel9.nLast))
        {
            continue;
        }
        
        ret = sub_oad_unit2fmt698(&stepRecordOut, relyInfo, &pBufTmp, &bufLenTmp);
        if (ret != ERR_OK)
        {
            db_read_record_end(hUdp, handle);
            FREE_S(pBufTmp);             
            return ERR_NORMAL;
        }

        bufAllLenLast = bufAllLen; //存储上次的长度

        bufAllLen += bufLenTmp;    //更新本次的长度

        if(bufAllLen <= maxBufLen)
        {
            //在长度累积大于maxBufLen后，这个拷贝会越界
            memcpy(&outRecordBuf->recordBuf[bufAllLenLast], pBufTmp, bufLenTmp);
        }
        else //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        {
            outRecordBuf->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (inReadRecord->cType)
            {
                case COL_TM_START:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStartTm;
                    break;
                case COL_TM_END:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colEndTm;
                    break;
                case COL_TM_STORE:
                    outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                    break;       
                default:
                    FREE_S(pBufTmp);
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            outRecordBuf->breakInfo.appSlicingFlag = 1;

            break;
        }

        //正常跳出，为已经处理的总记录数，分帧跳出，为已经处理的记录数
        outRecNum++;

        FREE_S(pBufTmp);    
    }

    /*结束读记录*/
    db_read_record_end(hUdp, handle);

    if (!isBufFull)
    {
        outRecordBuf->bufLen = bufAllLen;
        //outRecordBuf->recordBuf[numOffset] = outRecNumAll;
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        outRecordBuf->breakInfo.appSlicingFlag = 0x01;
        outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
        
        if (relyInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            outRecordBuf->breakInfo.seq = relyInfo->bpBreakInfo->seq + 1;   //序号加1
        }
        
        outRecordBuf->bufLen = bufAllLenLast;
        //outRecordBuf->recordBuf[numOffset] = i; //跳出来前，已经处理的记录条数
    }

    //记录型，或普通数据非分量，需要返回记录条数
    if(relyInfo->pOadInfo->nIndex == 0)
    {
        //长度域超过后也要采用A-XDR编码
        if (outRecNum > 0x80)
        {
            offlen = set_len_offset(outRecNum, lenBuf);
            memmove(&outRecordBuf->recordBuf[numOffset+offlen], &outRecordBuf->recordBuf[numOffset+1], outRecordBuf->bufLen - numOffset - 1);
            memcpy(&outRecordBuf->recordBuf[numOffset], lenBuf, offlen);
            outRecordBuf->bufLen += (offlen - 1); //减掉原来预留的1字节长度
        }
        else
        {
            outRecordBuf->recordBuf[numOffset] = outRecNum;   //填写总记录数 
        }    
    }
    else if(outRecNum == 0) //非分量，且记录条数为0，则返回NULL
    {
        outRecordBuf->recordBuf[offset++] = DT_NULL;
        outRecordBuf->bufLen++;
    }    
    
    PRTL_FMT_TRACE("record_event_tmn_read result: outRecNum(%lu), appSlicingFlag(%d), isEnd(%d)\n", outRecNum, outRecordBuf->breakInfo.appSlicingFlag, outRecordBuf->breakInfo.isEnd);

    return ERR_OK;
}

#endif

#if DESC("冻结记录读取函数",1)

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E frozen_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = *outRecord->offset;
    uint32 i = 0 ,j = 0;
    
    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    MEMZERO(&icInfo, sizeof(IC_OAD_INFO_T));
    
    convert.dstBuf = outRecord->buffer;
    convert.dstLen = outRecord->bufLen;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    icInfo.choice = 0;

    //每次解析其中一个OAD
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        
        for(j = 0; j < unit->recordnum; j++)
        {
            if(unit->record[j].road.oadCols.oad[0].value != recordInfo->oopRcsd.cols[i].oad.value)
            {
                continue;
            }

            icInfo.oad.value = unit->record[j].road.oadCols.oad[0].value;
            convert.srcBuf = unit->record[j].outDataBuf;
            convert.srcLen = unit->record[j].outDatalen;

            if(icInfo.oad.nIndex != 0)
            {
                icInfo.subParse = TRUE;
            }

            dar = normal_data_to_fmt698(&icInfo, &convert);
            if(dar != DATA_SUCCESS)
            {
                PRTL_FMT_DEBUG("[%d] normal_data_to_fmt698 failed. oad(0x%08x)\n", i, icInfo.oad.value);
                return dar;
            }

            break;
        }

        if(j == unit->recordnum)
        {
            ((uint8*)(outRecord->buffer))[dOffset++] = DT_NULL;
            PRTL_FMT_DEBUG("[%d] not find oad(0x%08x), data is null.\n", i, recordInfo->oopRcsd.cols[i].oad.value);
        }
        else
        {
            PRTL_BUF_TRACE(outRecord->buffer, dOffset, "[%d] find oad(0x%08x)\n", i, recordInfo->oopRcsd.cols[i].oad.value);
        }
    }

    *outRecord->offset = dOffset;

    return dar;
}


/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E transparent_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    //IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = *outRecord->offset;
    uint32 i = 0 ,j = 0;
    uint8 mn=0;
    //uint8 planid=0;
    OOP_METER_T oopMeter;
    
    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    
    convert.dstBuf = outRecord->buffer;
    convert.dstLen = outRecord->bufLen;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    //icInfo.choice = 0;
    
 
    //每次解析其中一个OAD
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        for(j=0;j<unit->recordnum;j++)
        {
            OOP_TRANSRES_T TransResult;
            uint32 colStartTm;
            uint32 colEndTm;

            TransResult.planID = unit->record[j].road.oadMain.nIndex;
            TransResult.res.result[0].data.nNum = unit->record[j].outDatalen;
            memcpy(TransResult.res.result[0].data.value,unit->record[j].outDataBuf,unit->record[j].outDatalen);
            TransResult.res.result[0].nIndex = (unit->record[j].road.oadCols.oad[0].value);
            TransResult.res.nNum = 1;
            app_get_meter_list(unit->infoNum,&oopMeter);
            TransResult.addr = oopMeter.basic.tsa;
            colStartTm = unit->colStartTm - 8*60*60;
            DT_Time2DateTimeShort(colStartTm,&TransResult.eTime);
            colEndTm = unit->colEndTm - 8*60*60;
            DT_Time2DateTimeShort(colEndTm,&TransResult.res.result[0].aTime);

            //memcpy((uint8 *)&TransResult,unit->record[j].outDataBuf,unit->record[j].outDatalen);

            ((uint8*)(outRecord->buffer))[dOffset++]=DT_STRUCTURE;
            ((uint8*)(outRecord->buffer))[dOffset++]=0x04;

   

            if(basic_data_to_buf(E_UNSIGNED, outRecord->buffer, outRecord->bufLen-dOffset, &dOffset, &TransResult.planID, 1) < 0)
            {
                return DATA_TYPE_UNMATCHED;
            }
            if(basic_data_to_buf(E_DATETIME_S, outRecord->buffer, outRecord->bufLen-dOffset, &dOffset, &TransResult.eTime, sizeof(OOP_DATETIME_S_T)) < 0)
            {
                return DATA_TYPE_UNMATCHED;
            }
            if(basic_data_to_buf(E_TSA, outRecord->buffer, outRecord->bufLen-dOffset, &dOffset, &TransResult.addr, sizeof(OOP_TSA_T)) < 0)
            {
                return DATA_TYPE_UNMATCHED;
            }
            
            ((uint8*)(outRecord->buffer))[dOffset++]=DT_ARRAY;
            ((uint8*)(outRecord->buffer))[dOffset++]=TransResult.res.nNum;
            for(mn=0;mn<TransResult.res.nNum;mn++)
            {
                ((uint8*)(outRecord->buffer))[dOffset++]=DT_STRUCTURE;
                ((uint8*)(outRecord->buffer))[dOffset++]=0x03;
                
                if(basic_data_to_buf(E_UNSIGNED, outRecord->buffer, outRecord->bufLen-dOffset, &dOffset, &TransResult.res.result[mn].nIndex, 1) < 0)
                {
                    return DATA_TYPE_UNMATCHED;
                } 
                if(basic_data_to_buf(E_DATETIME_S, outRecord->buffer, outRecord->bufLen-dOffset, &dOffset, &TransResult.res.result[mn].aTime, sizeof(OOP_DATETIME_S_T)) < 0)
                {
                    return DATA_TYPE_UNMATCHED;
                }
                ((uint8*)(outRecord->buffer))[dOffset++]=DT_OCTET_STRING;
                ((uint8*)(outRecord->buffer))[dOffset++]=TransResult.res.result[mn].data.nNum;
                memcpy((uint8*)outRecord->buffer+dOffset,TransResult.res.result[mn].data.value,TransResult.res.result[mn].data.nNum);
                dOffset=dOffset+TransResult.res.result[mn].data.nNum;
            }

        }

        if(j == RECORD_DATA_MAX_NUM)
        {
            ((uint8*)(outRecord->buffer))[dOffset++] = DT_NULL;
        }

        PRTL_BUF_TRACE(outRecord->buffer, dOffset, "[%d] oadCols(0x%08x)\n", i, recordInfo->oopRcsd.cols[i].oad.value);
    }

    *outRecord->offset = dOffset;

    return dar;
}

/**
*********************************************************************
* @brief：     根据rsd判断读的结果是不是唯一
* @param[in]： oopRsd   - 行刷选
* @param[out]：
* @return：    TRUE  唯一
               FALSE 非唯一
*********************************************************************
*/
BOOL is_result_only(OOP_PRSD_T oopRsd)
{
    BOOL ret = FALSE;
    time_t timeFrom = 0;
    time_t timeTo = 0;

    switch (oopRsd.choice)
    {
    case 1:
        {
            if (oopRsd.sel1.oad.value == 0x20210200)
            {
                if (oopRsd.sel1.value.type == DT_DATETIME_S)
                {
                    ret = TRUE;
                }
            }
        }
    case 2:
        {
            if (oopRsd.sel2.oad.value == 0x20210200)
            {
                if ((oopRsd.sel2.from.type == DT_DATETIME_S) && 
                    (oopRsd.sel2.to.type == DT_DATETIME_S))
                {
                    if ((oopRsd.sel2.span.type == DT_TI) &&
                        (oopRsd.sel2.span.ti.unit == TI_DAY) &&
                        (oopRsd.sel2.span.ti.value == 1))
                    {
                        DT_DateTimeShort2Time(&oopRsd.sel2.from.tmVal, &timeFrom);
                        DT_DateTimeShort2Time(&oopRsd.sel2.to.tmVal, &timeTo);

                        if ((timeTo - timeFrom)/(24*60*60) == 1)
                        {
                            ret = TRUE;
                        }
                    }
                    else if (oopRsd.sel2.span.type == DT_NULL)
                    {
                        ret = TRUE;
                    } 
                }
            }
        }
        break;
    case 3:
        {
            int i =0 ;
            
            for (i = 0; i < oopRsd.sel3.nNum; i++)
            {
                if (oopRsd.sel3.item[i].oad.value == 0x20210200)
                {
                    if ((oopRsd.sel3.item[i].from.type == DT_DATETIME_S) && 
                        (oopRsd.sel3.item[i].to.type == DT_DATETIME_S))
                    {
                        if ((oopRsd.sel3.item[i].span.type == DT_TI) &&
                            (oopRsd.sel3.item[i].span.ti.unit == TI_DAY) &&
                            (oopRsd.sel3.item[i].span.ti.value == 1))
                        {
                            DT_DateTimeShort2Time(&oopRsd.sel3.item[i].from.tmVal, &timeFrom);
                            DT_DateTimeShort2Time(&oopRsd.sel3.item[i].to.tmVal, &timeTo);
                
                            if ((timeTo - timeFrom)/(24*60*60) == 1)
                            {
                                ret = TRUE;
                                break;
                            }
                        }
                        else if (oopRsd.sel3.item[i].span.type == DT_NULL)
                        {
                            ret = TRUE;
                            break;
                        } 
                    }
                }
            }
        }
        break;
    case 9:
        {
            ret = TRUE;
        }
        break;
    default:
        break;
    }

    return ret;
}

/**
*********************************************************************
* @brief：      读取冻结记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_frozen_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead;
    RECORD_UNIT_T      stepOut;
//    READ_RECORD_T readRecord[MAX_SELECTOR2_NUM];
//    uint32 stepHandle[MAX_SELECTOR2_NUM] = 0;  //单步读句柄，最多4轮  
//    uint32 recNum[MAX_SELECTOR2_NUM]     = 0;  //记录条数   
    READ_RECORD_T readRecord;
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffffffff;
    uint32 stepNum = 0; //单步读轮数    
    uint32 offset     = *outRecord->offset;  //输出报文的偏移
    uint32 numOffset  = 0;  //写记录数的偏移    
    uint32 i          = 0;
    uint32 outRecNum  = 0; //实际输出的记录条数
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOLEAN               isBufFull = FALSE;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint8 *outBuffer = (uint8*)outRecord->buffer;
    BUFFER_INFO_T toutBuffer;
    uint32 tOffset = 0;
    uint8 tmpBuf[DATA_LEN_MAX] = {0};

    /*入参检查*/
    if((recordInfo == NULL) || (outRecord == NULL))
    {
        PRTL_FMT_DEBUG("record_stat_read param check failed. recordInfo(%d), outBuffer(%d)\n", recordInfo, outRecord);
        return ERR_PTR;
    }

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&stepRead, sizeof(stepRead));
    MEMZERO(&stepOut, sizeof(stepOut));
    MEMZERO(&toutBuffer, sizeof(toutBuffer));

    //获取数据中心读取参数
    ret = record_read_param_get(recordInfo, &readRecord, &stepNum);
    if(ret == ERR_O_RANGE) //超出范围的OAD
    {
        ctrlRecNum = 0;
    }
    else if(ret != ERR_OK)
    {
        return DATA_TYPE_UNMATCHED;
    }

    PRTL_FMT_TRACE("\r\nreadRecord.recordOAD.logicId = %d, readRecord.recordOAD.infoNum = %d, readRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\nreadRecord.recordOAD.road.oadCols.nNum = %d \n",
                    readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, 
                    readRecord.recordOAD.road.oadMain, readRecord.recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_TRACE("readRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, readRecord.recordOAD.road.oadCols.oad[i].value, readRecord.recordOAD.road.oadCols.oad[i].value);
    }

    //如果有断点，由于是倒序，则截止到上次中断的时间点，前闭后开，上次结束的时间的加1
    if(recordInfo->append.bpInfo->appSlicingFlag == 1)
    {
        readRecord.cEnd = recordInfo->append.bpInfo->tmStartSeg + 1;    
    }

    PRTL_FMT_TRACE("readRecord.ti.unit = %d, readRecord.ti.value = %d, readRecord.cType = %d, readRecord.tmStart = %d, readRecord.tmEnd = %d, readRecord.sortType = %d\n", 
        readRecord.ti.unit, readRecord.ti.value,
        readRecord.cType, readRecord.cStart, readRecord.cEnd, readRecord.sortType);

    /*开始读记录*/
    ret = db_read_record_start(clientfd, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_record_start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    outRecNumAll = MIN(recNum, ctrlRecNum); 
    PRTL_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), ctrlRecNum(%lu)\n", outRecNumAll, recNum, ctrlRecNum);

    //MAC校验
    memcpy(&stepRead.MAC, &readRecord.MAC, sizeof(OOP_OCTETVAR64_T));

    for(i = 0; i < 6; i++)
    {
        PRTL_FMT_TRACE("stepRead.MAC[%d] = %02x\n", i, stepRead.MAC.value[i]);
    }

    numOffset = offset;  //记录数或者数组成员数的位置
    
    /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
    outBuffer[offset++] = outRecNumAll;

    // 如果outRecNumAll为0，且结果具有唯一性，每个CSD对应填NULL
    if ((outRecNumAll == 0) && (is_result_only(recordInfo->oopRsd)))
    {
        PRTL_FMT_TRACE("outRecNumAll(%lu), and the result is only\n", outRecNumAll);
        outRecNum = 1;
        for (i = 0; i < recordInfo->oopRcsd.nNum; i ++)
        {
            outBuffer[offset++] = 0;
        }
    }
    
    bufAllLen = *outRecord->offset = offset;

    //每读一条存到一个临时buffer,便于分帧
    toutBuffer.buffer = tmpBuf;
    toutBuffer.offset = &tOffset;
    toutBuffer.bufLen = DATA_LEN_MAX;

    //由于统计的特殊性，由于1条记录，所以只支持方法9，上第1条
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(clientfd, stepHandle, &stepRead, &stepOut);
        if ((ret != ERR_OK) || (stepOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(recordInfo->oopRsd.choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(readRecord, stepOut);
            if (ret != ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((recordInfo->oopRsd.choice == 9) && (i+1 != recordInfo->oopRsd.sel9.nLast))
        {
            continue;
        }

        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/        

        bufAllLenLast = bufAllLen; //存储上次的长度
        tOffset = 0;
        ret = frozen_unit2fmt698(recordInfo, &stepOut, &toutBuffer);
        if(ret != ERR_OK)
        {
            db_read_record_end(clientfd, stepHandle);          
            return ret;
        }
        
        bufAllLen = bufAllLenLast + tOffset;

        //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        if(bufAllLen <= outRecord->bufLen)
        {
            memcpy(&outBuffer[bufAllLenLast], toutBuffer.buffer, tOffset);
        }
        else
        {
            recordInfo->append.bpInfo->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (readRecord.cType)
            {
                case COL_TM_START:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStartTm;
                    break;
                case COL_TM_END:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colEndTm;
                    break;
                case COL_TM_STORE:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStoreTm;
                    break;       
                default:
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            recordInfo->append.bpInfo->appSlicingFlag = 1;

            break;
        }

        outRecNum++;
    }

    /*结束读记录*/
    db_read_record_end(clientfd, stepHandle);

    if (!isBufFull)
    {
        *outRecord->offset = bufAllLen;
        
        if (recordInfo->append.bpInfo->appSlicingFlag == 0x01)
        {
            recordInfo->append.bpInfo->appSlicingFlag = 0x01;
            recordInfo->append.bpInfo->isEnd = 0x01;                          //末尾帧
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(recordInfo->append.bpInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        *outRecord->offset = bufAllLenLast;
            
        if(recordInfo->append.bpInfo->appSlicingFlag == 0x00)
        {
            recordInfo->append.bpInfo->seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        
        recordInfo->append.bpInfo->appSlicingFlag = 0x01;
        recordInfo->append.bpInfo->isEnd = 0x00;                              //不是末尾帧
    }

    //长度域超过后也要采用A-XDR编码
    if (outRecNum > 0x80)
    {
        offlen = set_len_offset(outRecNum, lenBuf);
        memmove(&((uint8*)outRecord->buffer)[numOffset+offlen], &((uint8*)outRecord->buffer)[numOffset+1], *outRecord->offset - numOffset - 1);
        memcpy(&((uint8*)outRecord->buffer)[numOffset], lenBuf, offlen);
        *outRecord->offset += (offlen - 1); //减掉原来预留的1字节长度
    }
    else
    {
        //填写总记录数 
        ((uint8*)outRecord->buffer)[numOffset] = outRecNum;   
    }      

    return ERR_OK;
}

#endif


#if DESC("统计记录读取函数",1)

/**
*********************************************************************
* @brief：      终端事件转换成698格式报文
* @param[in]： unit           - 单个数据记录单元
* @param[in]： oopRcsd        - 记录列选择描述符
* @param[out]：ppOutBuf       - 698格式报文
               outLen        - 报文长度
* @return：    =0             - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E stat_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    OAD_INFO_T *mainOadInfo = NULL;
    IC_OAD_INFO_T icInfo;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = *outRecord->offset;
    uint32 i       = 0;
    uint32 recIndex = 0;

    mainOadInfo = (OAD_INFO_T *)recordInfo->append.pOadInfo;

    //统计的特殊性，每个记录数据的结构是一样的，不随关联OAD变化
    icInfo.choice = 1;
    icInfo.pstOdaInfo = mainOadInfo;

    MEMZERO(&convert, sizeof(DATA_CONVERT_T));
    convert.dstBuf = outRecord->buffer;
    convert.dstLen = outRecord->bufLen;
    convert.sOffset = &sOffset;   
    convert.dOffset = &dOffset;

    //每次解析其中一个OAD
    for(i = 0; i < recordInfo->oopRcsd.nNum; i++)
    {
        sOffset = 0;
        
        if(unit->record[recIndex].road.oadCols.oad[0].value != recordInfo->oopRcsd.cols[i].oad.value)
        {
            ((uint8*)(outRecord->buffer))[dOffset++] = DT_NULL;
            
            PRTL_BUF_TRACE(outRecord->buffer, dOffset, "[%d] oadCols(0x%08x)\n", i, recordInfo->oopRcsd.cols[i].oad.value);
            
            continue;
        }
        
        convert.srcBuf = unit->record[i].outDataBuf;
        convert.srcLen = unit->record[i].outDatalen;
        
        dar = normal_data_to_fmt698(&icInfo, &convert);
        if(dar != DATA_SUCCESS)
        {
            PRTL_FMT_DEBUG("[%d] normal_data_to_fmt698 failed. oad(0x%08x)\n", i, icInfo.pstOdaInfo->pTab->oad.value);
            return dar;
        }

        recIndex++;
        PRTL_BUF_TRACE(outRecord->buffer, dOffset, "[%d] oadCols(0x%08x)\n", i, recordInfo->oopRcsd.cols[i].oad.value);
    }

    *outRecord->offset = dOffset;

    return dar;
}
#if 0
/**
*********************************************************************
* @brief：      读取统计记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_loginfo_read(RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret;
    OOP_DAR_E dar = DATA_SUCCESS;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    DATA_CONVERT_T convert;
    uint32 sOffset = 0;
    uint32 dOffset = 0,bufAllLen=0;
    BP_INFO_T breakInfo;
    MEMZERO(&breakInfo, sizeof(BP_INFO_T));
    uint32    maxBufLen = 0;
    uint32 offset  = 0;    
    uint8 sendbuf[100] = {0};
    uint16 recvlen = 0,count = 0,i,lognumoffset = 0;
    uint8 recvbuf[1024] = {0};
    MEMZERO(&convert, sizeof(convert)); 
    BOOL      isBufFull = FALSE;
    uint8 lognum = 0;
    maxBufLen = pApduInfo->connectionInfo.ApplSplitLen - cal_excludeLen(pApduInfo->connectionInfo);

    OOP_LOG_INFO_T loginfo[OOP_MAX_LOGS];
    memset(&loginfo,0,sizeof(OOP_LOG_INFO_T)*OOP_MAX_LOGS);
    uint8 option = 0;
    sendbuf[0] = 1; //日志类型     [1] LOGTYPE，
    sendbuf[1] = 0; //全部日志（0），
    ret = appmain_send_syncmsg_time(MSG_APPAMIN_LOGINFO_GET,sendbuf,2,recvbuf,sizeof(recvbuf),&recvlen, 60);
    if(ret!=ERR_OK)
    {
        PRTL_FMT_DEBUG("获取全部日志信息失败 ret %d\n",ret);
        return 0;
    }
    count = recvbuf[offset++];
    for(i=0;i<count;i++)
    {
        loginfo[i].logpathname.nNum = recvbuf[offset++];
        memcpy(loginfo[i].logpathname.value,&recvbuf[offset],loginfo[i].logpathname.nNum);
        offset+=loginfo[i].logpathname.nNum;
        memcpy_r(&loginfo[i].logsize,&recvbuf[offset],sizeof(uint32));
        offset+=sizeof(uint32);
        if(recvbuf[offset++]==1)
        {
            loginfo[i].logmd5.nNum = recvbuf[offset++];
            memcpy(loginfo[i].logmd5.value,&recvbuf[offset],loginfo[i].logmd5.nNum);
            offset+=loginfo[i].logmd5.nNum;   
        }
        memcpy_r(&loginfo[i].creattime.year,&recvbuf[offset],sizeof(uint16));
        offset+=sizeof(uint16);
        loginfo[i].creattime.month = recvbuf[offset++];
        loginfo[i].creattime.day = recvbuf[offset++];
        loginfo[i].creattime.hour = recvbuf[offset++];
        loginfo[i].creattime.minute = recvbuf[offset++];
        loginfo[i].creattime.second = recvbuf[offset++];
        loginfo[i].logtype = recvbuf[offset++];
        option = recvbuf[offset++];
        if(option != 0)
        {
         loginfo[i].conname.nNum = recvbuf[offset++];
         memcpy(loginfo[i].conname.value,&recvbuf[offset],loginfo[i].conname.nNum);
         offset+=loginfo[i].conname.nNum;
        }
        option = recvbuf[offset++];
        if(option != 0)
        {
         loginfo[i].appname.nNum = recvbuf[offset++];
         memcpy(loginfo[i].appname.value,&recvbuf[offset],loginfo[i].appname.nNum);
         offset+=loginfo[i].appname.nNum;
        }  
    }
    offset=0;
    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));   //拷贝OAD
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;                             //结果
    outData->pResultData[offset ++] = DT_ARRAY;                         //类型
    lognumoffset = offset;                                        
    outData->pResultData[offset ++] = 0x00;                             //数量，先填0
    bufAllLen = offset;
    for(i=0;i<count;i++)
    {
        if (slicing_is_comtinue_oop_meter(pApduInfo->bpBreakInfo, i))
        {
            continue;
        }   
        //转换成报文数据
        convert.srcBuf = &loginfo[i];
        convert.srcLen = sizeof(OOP_CONTAINER_T);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outData->pResultData[offset];
        convert.dstLen = outDataMax - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_log_info(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outData->pResultData[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outData->resultDataLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个日志信息报文:", i);
                lognum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("class11_invoke_get_container isBufFull == TRUE \n");
            }
        }
        else
        {
            continue;
        }
        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }
    if (!isBufFull)
    {
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = pApduInfo->reqType;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (pApduInfo->bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = pApduInfo->bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(pApduInfo->bpBreakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_DEBUG(outData->pResultData, offset, "整个日志信息报文:");
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", pApduInfo->bpBreakInfo->appSlicingFlag);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->index = %d\n", pApduInfo->bpBreakInfo->index);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->isEnd = %d\n", pApduInfo->bpBreakInfo->isEnd);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->objPre = %d\n", pApduInfo->bpBreakInfo->objPre);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->seq = %d\n", pApduInfo->bpBreakInfo->seq);
    PRTL_FMT_DEBUG("记下断点信息 bpBreakInfo->reqType = %d\n", pApduInfo->bpBreakInfo->reqType);
    if (lognum > 0)
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[lognumoffset] = lognum;   
        outData->resultDataLen = offset;
    }
    else
    {
        nRet = DATA_SUCCESS;
        outData->pResultData[lognumoffset] = 0;  
        outData->resultDataLen = offset;
    }
    return nRet;
}

    
    /* 数据和报文转换 */
    dar = data_to_buf_class8(pOadInfo, &convert);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_class8 failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    /* 填写报文开头处的OAD和数据结果0x01*/
    memcpy(outData->pResultData, inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    outData->pResultData[offset ++] = 0x01;
    outData->resultDataLen += offset;

    return dar;
}
#endif

/**
*********************************************************************
* @brief：      读取统计记录
* @param[in]： clientfd       - 数据中心句柄
               recordInfo    - 读记录信息
* @param[out]：outRecord      - 输出记录信息
* @return：    =0             - 成功
              <0             - 错误码
*********************************************************************
*/
int record_stat_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord)
{
    int ret = 0;
    STEP_READ_RECORD_T stepRead;
    RECORD_UNIT_T      stepOut;
    READ_RECORD_T readRecord;
    uint32 stepHandle = 0;  //单步读句柄，最多4轮  
    uint32 recNum     = 0;  //记录条数   
    uint32 outRecNumAll = 0;  //总的要输出的记录条数
    uint32 ctrlRecNum = 0xffffffff;
    //uint32 stepNum = 0; //单步读轮数    
    uint32 offset     = *outRecord->offset;  //输出报文的偏移
    uint32 numOffset  = 0;  //写记录数的偏移    
    uint32 i          = 0;
    uint32 outRecNum  = 0; //实际输出的记录条数
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOLEAN               isBufFull = FALSE;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint8 *outBuffer = (uint8*)outRecord->buffer;
    uint8 *pAddr = NULL;

    /*入参检查*/
    if((recordInfo == NULL) || (outRecord == NULL))
    {
        PRTL_FMT_DEBUG("record_stat_read param check failed. recordInfo(%d), outBuffer(%d)\n", recordInfo, outRecord);
        return ERR_PTR;
    }

    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&stepRead, sizeof(stepRead));
    MEMZERO(&stepOut, sizeof(stepOut));

    readRecord.recordOAD.infoNum = recordInfo->append.infoNum;
    readRecord.recordOAD.logicId = recordInfo->append.logicId;
    readRecord.recordOAD.road.oadMain.value = recordInfo->oopOad.value;
    readRecord.recordOAD.road.oadCols.nNum = recordInfo->oopRcsd.nNum;
    readRecord.cType = COL_TM_STORE;

    //统计的关联OAD不存在记录型
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        if(recordInfo->oopRcsd.cols[i].oad.value == 0x200A0200) //兼容适配，功率因数只算总
        {
            recordInfo->oopRcsd.cols[i].oad.value = 0x200A0201;
        }

        readRecord.recordOAD.road.oadCols.oad[i].value = recordInfo->oopRcsd.cols[i].oad.value;
    }
    
    memcpy(&readRecord.MAC, &recordInfo->append.MAC, sizeof(OOP_OCTETVAR64_T));
    readRecord.sortType = DOWN_SORT;
    readRecord.cStart = 0;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME + 1;      // 注：+8个小时(28800)转成格林时间

    PRTL_FMT_TRACE("\r\nreadRecord.recordOAD.logicId = %d, readRecord.recordOAD.infoNum = %d, readRecord.recordOAD.road.oadMain = 0x%08x\
                    \r\nreadRecord.recordOAD.road.oadCols.nNum = %d \n",
                    readRecord.recordOAD.logicId, readRecord.recordOAD.infoNum, 
                    readRecord.recordOAD.road.oadMain, readRecord.recordOAD.road.oadCols.nNum);
    
    for(i = 0; i < readRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_TRACE("readRecord.recordOAD.road.oadCols.oad[%d] = %lu(0x%08x)\n", i, readRecord.recordOAD.road.oadCols.oad[i].value, readRecord.recordOAD.road.oadCols.oad[i].value);
    }

    //如果有断点，由于是倒序，则截止到上次中断的时间点，前闭后开，上次结束的时间的加1
    if(recordInfo->append.bpInfo->appSlicingFlag == 1)
    {
        readRecord.cEnd = recordInfo->append.bpInfo->tmStartSeg + 1;    
    }

    PRTL_FMT_TRACE("readRecord.ti.unit = %d, readRecord.ti.value = %d, readRecord.cType = %d, readRecord.tmStart = %d, readRecord.tmEnd = %d, readRecord.sortType = %d\n", 
        readRecord.ti.unit, readRecord.ti.value,
        readRecord.cType, readRecord.cStart, readRecord.cEnd, readRecord.sortType);

    /*开始读记录*/
    ret = db_read_record_start(clientfd, &readRecord, &stepHandle, &recNum);
    if (ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("stat read record start failed. ret(%d), reNum(%d)\n", ret, recNum);
        return ERR_START_REC;
    }

    outRecNumAll = MIN(recNum, ctrlRecNum); 
    PRTL_FMT_TRACE("outRecNumAll(%lu), MIN(recNum(%lu), ctrlRecNum(%lu)\n", outRecNumAll, recNum, ctrlRecNum);

    //MAC校验
    app_get_localaddr(&pAddr);

    //MAC校验为终端地址的前6字节
    stepRead.MAC.nNum = 6;
    memcpy_s(&stepRead.MAC.value, 64, pAddr, 6);

    for(i = 0; i < 6; i++)
    {
        PRTL_FMT_TRACE("stepRead.MAC[%d] = %02x\n", i, stepRead.MAC.value[i]);
    }

    //记录型，或普通数据非分量，需要返回记录条数
    if(recordInfo->append.pOadInfo->nIndex == 0)
    {
        //普通非分量数据需要数组类型
        if(recordInfo->append.bNormal == TRUE)
        {
            outBuffer[offset++] = DT_ARRAY;
        }

        numOffset = offset;  //记录数或者数组成员数的位置

        /*M 条记录 [1] SEQUENCE OF A-RecordRow*/
        outBuffer[offset++] = outRecNumAll;   //记录数先存实际条目 

        bufAllLen = *outRecord->offset = offset;
    }

    //由于统计的特殊性，由于1条记录，所以只支持方法9，上第1条
    for(i = 0; i < outRecNumAll; i++)
    {
        ret = db_read_record_step(clientfd, stepHandle, &stepRead, &stepOut);
        if ((ret != ERR_OK) || (stepOut.recordnum == 0))
        {
            PRTL_BUF_DEBUG(stepRead.MAC.value, 6, "db_read_record_step failed. ret(%d), recordnum(%d), mainOAD(0x%08x)\n", 
                ret, stepOut.recordnum, readRecord.recordOAD.road.oadMain.value);
            continue;
        }

        //方法2：判断范围正确性
        if(recordInfo->oopRsd.choice == 2)
        {
            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(readRecord, stepOut);
            if (ret < ERR_OK)
            {
                continue;
            }  
        }

        //方法9：读取上第n次记录
        if((recordInfo->oopRsd.choice == 9) && (i+1 != recordInfo->oopRsd.sel9.nLast))
        {
            continue;
        }

        /* 
        A-RecordRow∷=SEQUENCE
        {
         第 1 列数据 Data，
         第 2 列数据 Data，
         …
         第 N 列数据 Data
        }*/        

        bufAllLenLast = *outRecord->offset; //存储上次的长度

        ret = stat_unit2fmt698(recordInfo, &stepOut, outRecord);
        if((ret != ERR_OK) && (ret != DATA_SCOPE_OF_ACCESS_VIOLATED))
        {
            db_read_record_end(clientfd, stepHandle);          
            return ret;
        } 
        
        bufAllLen = *outRecord->offset;

        //超长，需要分帧，记下尚未处理的记录序号和尚未处理的时间，如果第一个就超长？
        if((bufAllLen > outRecord->bufLen) || (ret == DATA_SCOPE_OF_ACCESS_VIOLATED)) 
        {
            recordInfo->append.bpInfo->recNumSeg = outRecNum; //未处理的记录序号，同时表示已处理的记录数
            //方法4 5 6 7 8
            switch (readRecord.cType)
            {
                case COL_TM_START:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStartTm;
                    break;
                case COL_TM_END:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colEndTm;
                    break;
                case COL_TM_STORE:
                    recordInfo->append.bpInfo->tmStartSeg = stepOut.colStoreTm;
                    break;       
                default:
                    return ERR_PTR;
                break;
            }
            
            isBufFull = TRUE;
            recordInfo->append.bpInfo->appSlicingFlag = 1;

            break;
        }

        outRecNum++;
    }

    /*结束读记录*/
    db_read_record_end(clientfd, stepHandle);

    if (!isBufFull)
    {
        *outRecord->offset = bufAllLen;
        
        if (recordInfo->append.bpInfo->appSlicingFlag == 0x01)
        {
            recordInfo->append.bpInfo->appSlicingFlag = 0x01;
            recordInfo->append.bpInfo->isEnd = 0x01;                          //末尾帧
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        else
        {
            /*清除断点信息*/
            memset(recordInfo->append.bpInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        *outRecord->offset = bufAllLenLast;
            
        if(recordInfo->append.bpInfo->appSlicingFlag == 0x00)
        {
            recordInfo->append.bpInfo->seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            recordInfo->append.bpInfo->seq++;   //序号加1
        }
        
        recordInfo->append.bpInfo->appSlicingFlag = 0x01;
        recordInfo->append.bpInfo->isEnd = 0x00;                              //不是末尾帧
    }

    //长度域超过后也要采用A-XDR编码
    if (outRecNum > 0x80)
    {
        offlen = set_len_offset(outRecNum, lenBuf);
        memmove(&((uint8*)outRecord->buffer)[numOffset+offlen], &((uint8*)outRecord->buffer)[numOffset+1], *outRecord->offset - numOffset - 1);
        memcpy(&((uint8*)outRecord->buffer)[numOffset], lenBuf, offlen);
        *outRecord->offset += (offlen - 1); //减掉原来预留的1字节长度
    }
    else
    {
        //填写总记录数 
        ((uint8*)outRecord->buffer)[numOffset] = outRecNum;   
    }      

    return ERR_OK;
}

#endif

/**
*********************************************************************
* @brief：     根据rcsd判断读的数据类型（实时数据或者冻结数据）
* @param[in]： oopRcsd       - 列刷选
* @param[out]：
* @return：    数据类型 0 - 不带主OAD数据 1 - 其他数据（带主OAD）
*********************************************************************
*/
int32 task_table_data_type(OOP_RCSD_T oopRcsd)
{
    uint8      i = 0;
    BOOL       isSigleOAD = FALSE;
    BOOL       isMainOAD = FALSE;
    int32      attr = -1;
    OAD_INFO_T pOadInfo = {0};

    for (i = 0; i < oopRcsd.nNum; i++)
    {
        if (oopRcsd.cols[i].choice == 0)
        {
            if ((oopRcsd.cols[i].oad.value != 0x60400200) && 
                (oopRcsd.cols[i].oad.value != 0x60410200) &&
                (oopRcsd.cols[i].oad.value != 0x60420200) &&
                (oopRcsd.cols[i].oad.value != 0x20210200) &&
                (oopRcsd.cols[i].oad.value != 0x40010200) &&
                (oopRcsd.cols[i].oad.value != 0x202A0200) &&
                (class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo)))
            {
                PRTL_FMT_TRACE("find singal oad[0x%8x]\n", oopRcsd.cols[i].oad.value);
                isSigleOAD = TRUE;
            }
        }
        else if (oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_TRACE("find main oad[0x%8x]\n", oopRcsd.cols[i].road.oadMain.value);
            isMainOAD = TRUE;
        }
        else
        {
            PRTL_FMT_DEBUG("oopRcsd.cols[%d].choice[%d] err\n", i, oopRcsd.cols[i].choice);
            return -1;
        }  
    }

    if ((isSigleOAD && isMainOAD) || (!isSigleOAD && !isMainOAD))
    {
        PRTL_FMT_DEBUG("isSigleOAD[%d] and isMainOAD[%d] err\n", isSigleOAD, isMainOAD);
        return -1;
    }

    if (isSigleOAD)
    {
        attr = 0;
    }  
    else if (isMainOAD)
    {
        attr = 1;
    }

    return attr;
}

/**
*********************************************************************
* @brief：     电表实时数据转换成698格式报文
               默认每次单步出来的数据是个完整的记录（同一个测量点，相同时间），不需要再拼接
               组织698报文严格按照rcsd顺序
* @param[in]： unit          - 单个数据记录单元
               oopRcsd       - 列刷选
* @param[out]：ppOutBuf      - 698格式报文
               outLen        - 报文长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int sigle_oad_task_table_data_to_698buf_malloc(RECORD_UNIT_T unit, OOP_RCSD_T oopRcsd, MS_T *ms, SPC_OAD_E *spcOad, uint8 **ppOutBuf, uint32 *outLen)
{
    OOP_OAD_U      oad;
    int32          nRet = ERR_NORMAL;
    OAD_INFO_T     pOadInfo = {0};
    uint8          *pOutBuf = NULL;
    IC_OAD_INFO_T  icInfo = {0};
    DATA_CONVERT_T convert = {0};
    uint32         dOffset = 0;
    uint32         sOffset = 0;
    uint32         offset  = 0;
    uint32         i = 0;
    uint32         j = 0;
    DateTimeHex_t  oopTime_s;
    uint32         tmpTime = 0;

    /* 入参检查 */
    if ((unit.recordnum == 0) || (ms == NULL) || (spcOad == NULL) || (outLen == NULL))
    {
        PRTL_FMT_DEBUG("param err. unit.recordnum[%d] ms[%p], spcOad[%d], outLen[%d]\n", unit.recordnum, ms, spcOad, outLen);
        return ERR_OPTION;
    }

    /* 此接口只做做单OAD数据 */
    for (i = 0; i < oopRcsd.nNum; i++)
    {
        if (oopRcsd.cols[i].choice == 1)
        {
            PRTL_FMT_DEBUG("oopRcsd.cols[%d].choice[%d] err\n", i, oopRcsd.cols[i].choice);
            return ERR_OPTION;
        }
    }

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//暂时这么定

    PRTL_FMT_LOGCC("****************打印数据中心数据*********************\n");
    PRTL_FMT_LOGCC("unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("unit.record[%d].road.oadCols.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j,  unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("****************打印数据中心数据结束*****************\n");

    for (i = 0; i < oopRcsd.nNum; i++)
    {
        memset((uint8*)&pOadInfo,0x00,sizeof(OAD_INFO_T));
        memset((uint8*)&icInfo, 0x00, sizeof(IC_OAD_INFO_T));
        if (oopRcsd.cols[i].oad.value == 0x60400200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colStartTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colStartTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        else if (oopRcsd.cols[i].oad.value == 0x60410200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colEndTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colEndTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        else if (oopRcsd.cols[i].oad.value == 0x60420200 || oopRcsd.cols[i].oad.value == 0x20210200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colStoreTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colStoreTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        //else if (oopRcsd.cols[i].oad.value == 0x40010200 || oopRcsd.cols[i].oad.value == 0x202A0200)
        else if (oopRcsd.cols[i].oad.value == 0x202A0200)
        {
            pOutBuf[offset++] = DT_TSA;
            pOutBuf[offset++] = ms->oopMeter.basic.tsa.len+2;
            pOutBuf[offset++] = ms->oopMeter.basic.tsa.af;
			if (ms->oopMeter.basic.tsa.flag == 0x01)
		    {
		        pOutBuf[offset ++] = ms->oopMeter.basic.tsa.vxd;
		        memcpy(&pOutBuf[offset], ms->oopMeter.basic.tsa.add, ms->oopMeter.basic.tsa.len); 
		        offset += ms->oopMeter.basic.tsa.len;
		    }
		    else
		    {
		        memcpy(&pOutBuf[offset], ms->oopMeter.basic.tsa.add, ms->oopMeter.basic.tsa.len + 1); 
		        offset += ms->oopMeter.basic.tsa.len + 1;
		    }
            continue;
        }
        else if (!class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo))
        {
            PRTL_FMT_DEBUG("oopRcsdtmp have no object oad oad = [0x%08x]\n", oopRcsd.cols[i].oad.value);
            pOutBuf[offset++] = DT_NULL;
            continue;
        }
        //PRTL_FMT_DEBUG("oopRcsdtmp have no object oad oad = [0x%08x]\n", oopRcsd.cols[i].oad.value);
        //PRTL_FMT_DEBUG("pOadInfo->nIndex is %d,i is %d\n",pOadInfo.nIndex,i);
        for (j = 0; j < unit.recordnum; j++)
        {
            //参数检查
            if ((unit.record[j].road.oadCols.nNum == 0) || (unit.record[j].road.oadMain.value != 0x0))
            {
                PRTL_FMT_DEBUG("unit.record[%d] param err\n", j);
                free(pOutBuf);
                pOutBuf = NULL;
                return ERR_OPTION;
            }

            if ((oopRcsd.cols[i].oad.value & 0xffffff00) == unit.record[j].road.oadCols.oad[0].value || 
                oopRcsd.cols[i].oad.value == unit.record[j].road.oadCols.oad[0].value)
            {
                break;
            }
        }

        if (j == unit.recordnum)
        {
            PRTL_FMT_DEBUG("sigle_oad_task_table_data_to_698buf_malloc: find no oad[0x%08x] err!\n", oopRcsd.cols[i].oad.value);
            pOutBuf[offset++] = DT_NULL;
            continue;
        }

        oad = oopRcsd.cols[i].oad;
        PRTL_FMT_TRACE("sigle_oad_task_table_data_to_698buf_malloc: find oad[0x%08x]\n", oad.value);

        icInfo.choice = 1;
        icInfo.pstOdaInfo = &pOadInfo;

        convert.dstBuf = &(pOutBuf[offset]);
        convert.dstLen = DATA_LEN_MAX - offset;
        convert.dOffset = &dOffset;
        convert.srcBuf = unit.record[j].outDataBuf;
        convert.srcLen = unit.record[j].outDatalen;
        convert.sOffset = &sOffset;

        //组织698格式报文
        if (normal_data_to_fmt698(&icInfo, &convert) != DATA_SUCCESS)
        {
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_INVALID;
        }
        else
        {
            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "convert.dstBuf:");
            offset += dOffset;
            PRTL_FMT_LOGCC("offset:%d\n", offset);
            sOffset = 0;
            dOffset = 0;
            nRet = ERR_OK;
        }

    }

    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：     电表实时数据转换成698格式报文
               默认每次单步出来的数据是个完整的记录（同一个测量点，相同时间），不需要再拼接
               组织698报文严格按照rcsd顺序
* @param[in]： unit          - 单个数据记录单元
               oopRcsd       - 列刷选
* @param[out]：ppOutBuf      - 698格式报文
               outLen        - 报文长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int main_oad_task_table_data_to_698buf_malloc(RECORD_UNIT_T unit, OOP_RCSD_T oopRcsd, MS_T *ms, SPC_OAD_E *spcOad, uint8 **ppOutBuf, uint32 *outLen)
{
    OOP_OAD_U      oad;
    int32          nRet = ERR_NORMAL;
    OAD_INFO_T     pOadInfo = {0};
    uint8          *pOutBuf = NULL;
    IC_OAD_INFO_T  icInfo = {0};
    DATA_CONVERT_T convert = {0};
    uint32         dOffset = 0;
    uint32         sOffset = 0;
    uint32         offset  = 0;
    uint32         i = 0;
    uint32         j = 0;
    uint32         k = 0;
    DateTimeHex_t  oopTime_s;
    uint32         tmpTime = 0;

    /* 入参检查 */
    if ((unit.recordnum == 0) || (ms == NULL) || (spcOad == NULL) || (outLen == NULL))
    {
        PRTL_FMT_DEBUG("param err. unit.recordnum[%d] ms[%p], spcOad[%d], outLen[%d]\n", unit.recordnum, ms, spcOad, outLen);
        return ERR_OPTION;
    }

    /* 此接口只做带主OAD的数据 */
    for (i = 0; i < oopRcsd.nNum; i++)
    {
        if (oopRcsd.cols[i].choice == 0)
        {
            if ((oopRcsd.cols[i].oad.value != 0x60400200) && 
                (oopRcsd.cols[i].oad.value != 0x60410200) &&
                (oopRcsd.cols[i].oad.value != 0x60420200) &&
                (oopRcsd.cols[i].oad.value != 0x20210200) &&
                (oopRcsd.cols[i].oad.value != 0x40010200) &&
                (oopRcsd.cols[i].oad.value != 0x202A0200) &&
                (class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo)))
            {
                PRTL_FMT_DEBUG("oopRcsd.cols[%d].choice[%d] err\n", i, oopRcsd.cols[i].choice);
                return ERR_OPTION;
            }
        }
    }

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//暂时这么定

    PRTL_FMT_LOGCC("****************打印数据中心数据*********************\n");
    PRTL_FMT_LOGCC("unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("unit.record[%d].road.oadCols.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j,  unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("****************打印数据中心数据结束*****************\n");

    for (i = 0; i < oopRcsd.nNum; i++)
    {
        if (oopRcsd.cols[i].oad.value == 0x60400200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colStartTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colStartTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        else if (oopRcsd.cols[i].oad.value == 0x60410200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colEndTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colEndTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        else if (oopRcsd.cols[i].oad.value == 0x60420200 || oopRcsd.cols[i].oad.value == 0x20210200)
        {
            PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", unit.colStoreTm);
            // 注：-8个小时(28800)转成北京时间
            tmpTime = unit.colStoreTm - COMPENSATION_TIME;
            if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
            {
                PRTL_FMT_DEBUG("follow the 0x60400200 time scale of the data err!\n");
            }
            pOutBuf[offset++] = DT_DATETIME_S;
            memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
            offset += sizeof(DateTimeHex_t);
            continue;
        }
        else if (oopRcsd.cols[i].oad.value == 0x40010200 || oopRcsd.cols[i].oad.value == 0x202A0200)
        {
            pOutBuf[offset++] = DT_TSA;
            pOutBuf[offset++] = ms->oopMeter.basic.tsa.len+2;
            pOutBuf[offset++] = ms->oopMeter.basic.tsa.len;
            memcpy(&pOutBuf[offset], &ms->oopMeter.basic.tsa.add[0], ms->oopMeter.basic.tsa.len+1);
            offset += ms->oopMeter.basic.tsa.len+1;
            continue;
        }
        else if (!class_oadinfo_get(oopRcsd.cols[i].oad, &pOadInfo))
        {
            PRTL_FMT_DEBUG("oopRcsdtmp have no object oad oad = [0x%08x]\n", oopRcsd.cols[i].oad.value);
            pOutBuf[offset++] = DT_NULL;
            continue;
        }

        if (oopRcsd.cols[i].choice != 1)
        {
            PRTL_FMT_DEBUG("oopRcsd.cols[%d].choice err\n", i);
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_OPTION;
        }

        pOutBuf[offset++] = 0x01;
        pOutBuf[offset++] = oopRcsd.cols[i].road.oadCols.nNum;
        
        for (j = 0; j < oopRcsd.cols[i].road.oadCols.nNum; j++)
        {
            for (k = 0; k < unit.recordnum; k++)
            {
                //参数检查
                if ((unit.record[k].road.oadCols.nNum == 0) || (unit.record[k].road.oadMain.value == 0x0))
                {
                    PRTL_FMT_DEBUG("unit.record[%d] param err\n", j);
                    free(pOutBuf);
                    pOutBuf = NULL;
                    return ERR_OPTION;
                }

                if ((oopRcsd.cols[i].road.oadCols.oad[j].value  & 0xffffff00) == unit.record[k].road.oadCols.oad[0].value)
                {
                    break;
                }
            }

            if (k == unit.recordnum)
            {
                if (oopRcsd.cols[i].road.oadCols.oad[j].value == 0x20210200)
                {
                    *spcOad = SPC_OAD_20210200;
                    PRTL_FMT_TIME("follow the 0x20210200 time scale of the data", unit.colStoreTm);
                    // 注：-8个小时(28800)转成北京时间
                    tmpTime = unit.colStoreTm - COMPENSATION_TIME;
                    if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                    {
                        PRTL_FMT_DEBUG("follow the 0x20210200 time scale of the data err!\n");
                    }
                    pOutBuf[offset++] = DT_DATETIME_S;
                    memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
                    offset += sizeof(DateTimeHex_t);
                }
                else
                {
                    PRTL_FMT_DEBUG("main_oad_task_table_data_to_698buf_malloc: find no oad err. i[%d] oad[0x%08x]\n", i, oopRcsd.cols[i].road.oadCols.oad[j].value);
                    pOutBuf[offset++] = DT_NULL;
                }
                continue;
            }
			if ((oopRcsd.cols[i].road.oadCols.oad[j].value == 0x20210200)&&(unit.record[k].outDatalen==0x00))
			{
			    *spcOad = SPC_OAD_20210200;
                PRTL_FMT_TIME("follow the 0x20210200 time scale of the data", unit.colStoreTm);
                // 注：-8个小时(28800)转成北京时间
                tmpTime = unit.colStoreTm - COMPENSATION_TIME;
                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                {
                    PRTL_FMT_DEBUG("follow the 0x20210200 time scale of the data err!\n");
                }
                pOutBuf[offset++] = DT_DATETIME_S;
                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
                offset += sizeof(DateTimeHex_t);
				continue;
			}
            oad = oopRcsd.cols[i].road.oadCols.oad[j];
            PRTL_FMT_LOGCC("main_oad_task_table_data_to_698buf_malloc: find oad[0x%08x]\n", oad.value);

            //判断此OAD是否存在oad表中
            MEMZERO(&pOadInfo, sizeof(OAD_INFO_T));
            MEMZERO(&icInfo, sizeof(IC_OAD_INFO_T));
            if (!class_oadinfo_get(oad, &pOadInfo))
            {
                PRTL_FMT_DEBUG("unit have no object oad oad = [0x%08x]\n", oad.value);
                pOutBuf[offset++] = DT_NULL;
                continue;
            }

            icInfo.choice = 1;
            icInfo.pstOdaInfo = &pOadInfo;

            convert.dstBuf = &(pOutBuf[offset]);
            convert.dstLen = DATA_LEN_MAX - offset;
            convert.dOffset = &dOffset;
            convert.srcBuf = unit.record[k].outDataBuf;
            convert.srcLen = unit.record[k].outDatalen;
            convert.sOffset = &sOffset;

            //组织698格式报文
            if (normal_data_to_fmt698(&icInfo, &convert) != DATA_SUCCESS)
            {
                free(pOutBuf);
                pOutBuf = NULL;
                PRTL_FMT_DEBUG("normal_data_to_fmt698 err\n");
                return ERR_INVALID;
            }
            else
            {
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "convert.dstBuf:");
                offset += dOffset;
                PRTL_FMT_LOGCC("offset:%d\n", offset);
                sOffset = 0;
                dOffset = 0;
                nRet = ERR_OK;
            }
        }

    }

    *ppOutBuf = pOutBuf;
    *outLen = offset;

    return nRet;
}

/**
*********************************************************************
* @brief：     电表数据转换成698格式报文
               默认每次单步出来的数据是个完整的记录（同一个测量点，相同时间），不需要再拼接
* @param[in]： unit          - 单个数据记录单元
               oopRcsd       - 列刷选
* @param[out]：ppOutBuf      - 698格式报文
               outLen        - 报文长度
               pOadNum       - OAD数量
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int task_table_data_to_698buf_malloc(RECORD_UNIT_T unit, OOP_RCSD_T oopRcsd, MS_T *ms, SPC_OAD_E *spcOad, uint8 **ppOutBuf, uint32 *outLen)
{
    int32          attr = -1;

    /* 入参检查 */
    if ((unit.recordnum == 0) || (ms == NULL))
    {
        PRTL_FMT_DEBUG("param err. unit.recordnum[%d] ms[%p]\n", unit.recordnum, ms);
        return ERR_OPTION;
    }

    /* 先判断实时数据还是冻结数据 默认不支持同时召测实时数据和冻结数据 */
    attr = task_table_data_type(oopRcsd);
    if (attr < 0)
    {
        PRTL_FMT_DEBUG("task_table_data_type err[%d]\n", attr);
        return ERR_ITEM;
    }

    switch (attr)
    {
    case 0:
        return sigle_oad_task_table_data_to_698buf_malloc(unit, oopRcsd, ms, spcOad, ppOutBuf, outLen);
        break;
    case 1:
        return main_oad_task_table_data_to_698buf_malloc(unit, oopRcsd, ms, spcOad, ppOutBuf, outLen);
        break;
    default:
        break;
    }

    return ERR_NORMAL;
}

/**
*********************************************************************
* @brief：     电表事件转换成698格式报文
* @param[in]： unit          - 单个数据记录单元
               oopRcsd       - 列刷选
* @param[out]：ppOutBuf        - 698格式报文
               outLen        - 报文长度
               pOadNum       - 如果有特殊OAD，会自加
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int data_transparent_task_table_to_698buf_malloc(RECORD_UNIT_T unit, int32 *pOadNum, OOP_RCSD_T oopRcsd, SPC_OAD_E *spcOad, uint8 **ppOutBuf, uint32 *outLen)
{
    OOP_OAD_U      oad;
    int32          nRet = ERR_NORMAL;
    OAD_INFO_T     pOadInfo = {0};
    uint8          *pOutBuf = NULL;
    IC_OAD_INFO_T  icInfo = {0};
    DATA_CONVERT_T convert = {0};
    uint32         dOffset = 0;
    uint32         sOffset = 0;
    uint32         offset  = 0;
    uint32         i = 0;
    uint32         j = 0;
    int32          oadNum = 0;
    int32          oad20210200offset = -1;
    uint32         oadColsNum = 0;
//    DateTimeHex_t oopTime_s;

    /* 入参检查 */
    if ((unit.recordnum == 0) || (pOadNum == NULL))
    {
        return ERR_OPTION;
    }

    pOutBuf = (uint8 *)malloc(DATA_LEN_MAX);//暂时这么定
    oadNum  = *pOadNum;

    /* 寻找是否有需要特殊处理的OAD */
//    for (i = 0; i < oopRcsd.nNum; i++)
//    {
//        if (oopRcsd.cols[i].choice == 1)
//        {
//            oadColsNum = oopRcsd.cols[i].road.oadCols.nNum;
//            for (j = 0; j < oopRcsd.cols[i].road.oadCols.nNum; j++)
//            {
//                if (oopRcsd.cols[i].road.oadCols.oad[j].value == 0x20210200)
//                {
//                    oad20210200offset = j;
//                    *spcOad = SPC_OAD_20200200;
//                }
//            }
//        }
//    }

    PRTL_FMT_LOGCC("****************************************************\n");
    PRTL_FMT_LOGCC("oad20210200offset = %d\n", oad20210200offset);
    PRTL_FMT_LOGCC("oadColsNum = %d\n", oadColsNum);
    PRTL_FMT_LOGCC("unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("unit.record[%d].road.oadMain.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j,  unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("****************************************************\n");

    for (i = 0; i < unit.recordnum; i++)
    {
        //参数检查
        if ((unit.record[i].road.oadCols.nNum == 0)&&(unit.record[i].road.oadMain.value == 0x0))
        {
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_OPTION;
        }

        //此时说明特殊OAD还没有处理，在此处组织报文
//        if (oadNum <= oad20210200offset)
//        {
//            //如果0x2021022OAD排在最后之前
//            if ((oadNum + i) == oad20210200offset)
//            {
//                uint32 len = sizeof(DateTimeHex_t) + 1;
//                PRTL_FMT_TIME("secondary freezing data time", unit.colStoreTm);
//                if (!DT_Time2DateTimeHex(unit.colStoreTm, &oopTime_s))
//                {
//                    PRTL_FMT_DEBUG("secondary freezing data time err!\n");
//                    //是否需要其他处理？？
//                }
//                pOutBuf[offset++] = DT_DATETIME_S;
//                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
//                offset += sizeof(DateTimeHex_t);
//                PRTL_BUF_LOGCC(&pOutBuf[offset - len], len, "convert.dstBuf:");
//                PRTL_FMT_LOGCC("offset:%d\n", offset);
//
//                oad20210200offset = -1;
//            }
//        }

        //找到要组数据内容的OAD
        if ((unit.record[i].road.oadCols.nNum == 0)&&(unit.record[i].road.oadMain.value != 0x0))
        {
            oad = unit.record[i].road.oadMain;
            PRTL_FMT_LOGCC("data_task_table_to_698buf_malloc: oad[0x%08x] err!\n", oad.value);
        }
        else if ((unit.record[i].road.oadCols.nNum > 0)&&(unit.record[i].road.oadMain.value == 0x0))
        {
            oad = unit.record[i].road.oadCols.oad[0];
            PRTL_FMT_LOGCC("data_task_table_to_698buf_malloc: find oad[0x%08x]\n", oad.value);
        }
        else if ((unit.record[i].road.oadCols.nNum > 0)&&(unit.record[i].road.oadCols.oad[0].value != 0x0))
        {
            oad = unit.record[i].road.oadCols.oad[0];
            PRTL_FMT_LOGCC("data_task_table_to_698buf_malloc: find oad[0x%08x]\n", oad.value);
        }
        else
        {
            PRTL_FMT_DEBUG("data_task_table_to_698buf_malloc: find no oad err!\n");
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_INVALID;
        }

//        if (!class_oadinfo_get(oad, &pOadInfo))
//        {
//            PRTL_FMT_DEBUG("data_task_table_to_698buf_malloc class_oadinfo_get err\n");
//            free(pOutBuf);
//            pOutBuf = NULL;
//            return ERR_INVALID;
//        }

        icInfo.choice = 1;
        icInfo.pstOdaInfo = &pOadInfo;

        convert.dstBuf = &(pOutBuf[offset]);
        convert.dstLen = DATA_LEN_MAX - offset;
        convert.dOffset = &dOffset;
        convert.srcBuf = unit.record[i].outDataBuf;
        convert.srcLen = unit.record[i].outDatalen;
        convert.sOffset = &sOffset;

        //组织698格式报文
        if (transparent_data_to_fmt698(&icInfo, &convert) != DATA_SUCCESS)
        {
            free(pOutBuf);
            pOutBuf = NULL;
            return ERR_INVALID;
        }
        else
        {
            PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "convert.dstBuf:");
            offset += dOffset;
            PRTL_FMT_LOGCC("offset:%d\n", offset);
            sOffset = 0;
            dOffset = 0;
            nRet = ERR_OK;
        }

        //此时说明特殊OAD还没有处理，在此处组织报文
//        if ((oadNum <= oad20210200offset))
//        {
//            //如果0x2021022OAD排在最后
//            if (((oadNum + oad20210200offset) == oadColsNum - 1) && (i == (unit.recordnum - 1)))
//            {
//                uint32 len = sizeof(DateTimeHex_t) + 1;
//                PRTL_FMT_TIME("secondary freezing data time", unit.colStoreTm);
//                if (!DT_Time2DateTimeHex(unit.colStoreTm, &oopTime_s))
//                {
//                    PRTL_FMT_DEBUG("secondary freezing data time err!\n");
//                    //是否需要其他处理？？
//                }
//                pOutBuf[offset++] = DT_DATETIME_S;
//                memcpy(&pOutBuf[offset], &oopTime_s, sizeof(DateTimeHex_t));
//                offset += sizeof(DateTimeHex_t);
//                PRTL_BUF_LOGCC(&pOutBuf[offset - len], len, "convert.dstBuf:");
//                PRTL_FMT_LOGCC("offset:%d\n", offset);
//
//                oad20210200offset = -1;
//            }
//        }
    }

    *ppOutBuf = pOutBuf;
    *outLen = offset;
    *pOadNum = oadNum;

    return nRet;
}


/**
*********************************************************************
* @brief：     该记是否是需要的
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_data_is_valid(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit)
{
    OOP_TI_T  ti = {0};
    uint32    div = 0;
    uint8     tmType;
    int32     ret = ERR_NORMAL;
    OOP_OAD_U oad;
    uint8     OIA;
    uint8     OIA1;

    ti = inReadRecord.ti;
    tmType = inReadRecord.cType;

    oad  = inReadRecord.recordOAD.road.oadMain;
    OIA  = (uint8)(oad.value >> 24); 
    OIA1 = (uint8)((OIA>>0x04)&0x0f);

    PRTL_FMT_LOGCC("**************打印调整前的数据中心数据******************\n");
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadMain = 0x%08x\n", inReadRecord.recordOAD.road.oadMain.value);
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadCols.nNum = %d\n", inReadRecord.recordOAD.road.oadCols.nNum);
    uint8 i = 0;
    uint8 j = 0;
    for (i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
    {
        PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.recordOAD.road.oadCols.oad[%d] = 0x%08x\n", i, inReadRecord.recordOAD.road.oadCols.oad[i].value);
    }
    switch (inReadRecord.cType)
    {
        case COL_TM_START: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_START");     break; }
        case COL_TM_END:   {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_END");       break; }
        case COL_TM_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_TM_STORE");     break; }
        case COL_NO_STORE: {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_STORE");     break; }
        case COL_NO_INFO:  {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NO_INFO");      break; }
        case COL_NOT:      {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "COL_NOT");          break; }
        default:           {PRTL_FMT_LOGCC("CHOOSE_TYPE_E:%s\n", "NO CHOOSE_TYPE_E"); break; }
    }
    PRTL_FMT_TIME("record_data_is_valid unit.colStartTm", unit.colStartTm);
    PRTL_FMT_TIME("record_data_is_valid unit.colStoreTm", unit.colStoreTm);
    PRTL_FMT_TIME("record_data_is_valid unit.colEndTm", unit.colEndTm);
    PRTL_FMT_LOGCC("record_data_is_valid unit.recordnum = %d\n", unit.recordnum);
    for (i = 0; i < unit.recordnum; i++)
    {
        PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadMain.value = 0x%08x\n", i, unit.record[i].road.oadMain.value);
        PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadMain.nNum = %d\n", i, unit.record[i].road.oadCols.nNum);
        for (j = 0; j < unit.record[i].road.oadCols.nNum; j++)
        {
            PRTL_FMT_LOGCC("record_data_is_valid unit.record[%d].road.oadCols.oad[%d].value = 0x%08x\n", i, j,  unit.record[i].road.oadCols.oad[j].value);
        }
        PRTL_BUF_LOGCC(unit.record[i].outDataBuf, unit.record[i].outDatalen, "record_data_is_valid unit.record[%d].outDataBuf:", i);
    }
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.ti.unit = %d\n", inReadRecord.ti.unit);
    PRTL_FMT_LOGCC("record_data_is_valid inReadRecord.ti.value = %d\n", inReadRecord.ti.value);
    PRTL_FMT_LOGCC("**************打印调整前的数据中心数据结束**************\n");

    if (OIA1 == 0x03) //事件类
    {
        /*检测读出来的事件是否在刷选时间范围内*/
        switch (inReadRecord.cType)
        {
        case COL_TM_START:
            if ((unit.colStartTm >= inReadRecord.cStart) && (unit.colStartTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;
        case COL_TM_END:
            if ((unit.colEndTm >= inReadRecord.cStart) && (unit.colEndTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;
        case COL_TM_STORE:
            if ((unit.colStartTm >= inReadRecord.cStart) && (unit.colStoreTm <= inReadRecord.cEnd))
            {
                ret = ERR_OK;
            }
            else
            {
                ret = ERR_O_RANGE;
            } 
            break;        
        default:
            ret = ERR_O_RANGE;
            break;
        }
    }
    else if (OIA1 == 0x05)
    {
        /*检测读出来的数据是否在刷选时间点上*/
        switch (ti.unit)
        {
        case 0:
            div = ti.value;
            break;
        case 1:
            div = ti.value * 60;
            break;
        case 2:
            div = ti.value * 3600;
            break;
        case 3:
            div = ti.value * 24 * 3600;
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            break;
        }

        if (div == 0)
        {
            return ERR_OK;
        }

        switch (tmType)
        {
        case COL_TM_START:
            // if (unit.colStartTm%div == 0)
            // {
            //     ret = ERR_OK;
            // }
            // break;
        case COL_TM_END:
            // if (unit.colEndTm%div == 0)
            // {
            //     ret = ERR_OK;
            // }
            // break;
        case COL_TM_STORE:
            if (ti.unit == 3)
            {
                //相对上日23点59分和相对当日0点0分 
                if ((unit.colStoreTm%div == 0) || (unit.colStoreTm%div == 86340))
                {
                    ret = ERR_OK;
                }
            }
            else if (unit.colStoreTm%div == 0)
            {
                ret = ERR_OK;
            }
            break;    
        default:
            break;
        }
    }
    else
    {
        ret = ERR_OK;
    }
    
    return ret;
}

/**
*********************************************************************
* @brief：     是否是电表事件中的元素
* @param[in]： oad           - OAD
               inReadRecord  - 筛选需要的数据
* @return：    =1            - 是
               =0            - 否
*********************************************************************
*/
BOOL record_data_is_event_element(READ_RECORD_T inReadRecord, OOP_OAD_U oad)
{
    if ((oad.nObjID == inReadRecord.recordOAD.road.oadCols.oad[0].nObjID) || 
        (oad.nObjID == inReadRecord.recordOAD.road.oadCols.oad[1].nObjID) ||
        (oad.nObjID == inReadRecord.recordOAD.road.oadCols.oad[2].nObjID))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
*********************************************************************
* @brief：     单步出来的数据有时需要调整
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_data_adjust_event_element(READ_RECORD_T inReadRecord, RECORD_UNIT_T *pUnit)
{
    int32          ret = ERR_OK;
    RECORD_UNIT_T *pUnitTmp;
    RECORD_UNIT_T unit;
    uint32 i = 0;

    memcpy(&unit, pUnit, sizeof(unit));
    pUnitTmp = pUnit;

    if (pUnit == NULL)
    {
        return ERR_PTR;
    }

    if (pUnitTmp->recordnum != 3)
    {
        return ERR_NORMAL;
    }

    //检查是否需要调整
    if ((pUnitTmp->record[0].road.oadCols.oad[0].nObjID == inReadRecord.recordOAD.road.oadCols.oad[0].nObjID) || 
        (pUnitTmp->record[1].road.oadCols.oad[0].nObjID == inReadRecord.recordOAD.road.oadCols.oad[1].nObjID) ||
        (pUnitTmp->record[2].road.oadCols.oad[0].nObjID == inReadRecord.recordOAD.road.oadCols.oad[2].nObjID))
    {
        PRTL_FMT_LOGCC("record_data_adjust_event_element no need to adjust\n");
        return ERR_OK;
    }

    //读电表记录时，数据中心回复的顺序是事件开始时间、事件结束事件和事件记录号
    //应该调整为事件记录号、事件开始时间和事件结束事件
    for (i = 0; i < 3; i++)
    {
        //寻找事件记录号
        if (pUnitTmp->record[i].road.oadCols.oad[0].nObjID == inReadRecord.recordOAD.road.oadCols.oad[0].nObjID)
        {
            memcpy(&unit.record[0], &pUnitTmp->record[i], sizeof(RECORD_DATA_T));
            break;
        }
    }
    if (i == 3)
    {
        PRTL_FMT_DEBUG("record_data_adjust_event_element err:no 0x%04x\n", inReadRecord.recordOAD.road.oadCols.oad[0].nObjID);
        return ERR_NORMAL;
    }

    for (i = 0; i < 3; i++)
    {
        //寻找事件开始时间
        if (pUnitTmp->record[i].road.oadCols.oad[0].nObjID == 0x201e)
        {
            memcpy(&unit.record[1], &pUnitTmp->record[i], sizeof(RECORD_DATA_T));
            break;
        }
    }
    if (i == 3)
    {
        PRTL_FMT_DEBUG("record_data_adjust_event_element err:no 0x%04x\n", inReadRecord.recordOAD.road.oadCols.oad[1].nObjID);
        return ERR_NORMAL;
    }

    for (i = 0; i < 3; i++)
    {
        //寻找事件结束时间
        if (pUnitTmp->record[i].road.oadCols.oad[0].nObjID == 0x2020)
        {
            memcpy(&unit.record[2], &pUnitTmp->record[i], sizeof(RECORD_DATA_T));
            break;
        }
    }
    if (i == 3)
    {
        PRTL_FMT_DEBUG("record_data_adjust_event_element err:no 0x%04x\n", inReadRecord.recordOAD.road.oadCols.oad[2].nObjID);
        return ERR_NORMAL;
    }

    //调整后的单步记录赋值过去
    PRTL_FMT_LOGCC("record_data_adjust_event_element need to adjust\n");
    memcpy(pUnit, &unit, sizeof(unit));

    return ret;
}

/**
*********************************************************************
* @brief：     单步出来的数据有时需要调整
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_data_adjust_other_element(READ_RECORD_T inReadRecord, RECORD_UNIT_T *pUnit)
{
    int32          ret = ERR_OK;
    RECORD_UNIT_T *pUnitTmp;
    RECORD_UNIT_T unit;
    uint32 i = 0;
    uint32 j = 0;
    BOOL   isAdjust = FALSE;

    memcpy(&unit, pUnit, sizeof(unit));
    pUnitTmp = pUnit;

    if (pUnit == NULL)
    {
        PRTL_FMT_DEBUG("record_data_adjust_other_element ERR_PTR\n");
        return ERR_PTR;
    }

    //检查是否需要调整
    if (inReadRecord.recordOAD.road.oadCols.nNum != pUnitTmp->recordnum)
    {
        isAdjust = TRUE;
    }
    else
    {
        for (i = 0; i < pUnitTmp->recordnum; i++)
        {
            if (inReadRecord.recordOAD.road.oadCols.oad[i].value != pUnitTmp->record[i].road.oadCols.oad[0].value)
            {
                isAdjust = TRUE;
                break;
            }
        }
    }

    if (isAdjust)
    {
        unit.recordnum = inReadRecord.recordOAD.road.oadCols.nNum;
        for (i = 0; i < inReadRecord.recordOAD.road.oadCols.nNum; i++)
        {
            for (j = 0; j < pUnitTmp->recordnum; j++)
            {
                if (inReadRecord.recordOAD.road.oadCols.oad[i].value == pUnitTmp->record[j].road.oadCols.oad[0].value)
                {
                    memcpy(&unit.record[i], &pUnitTmp->record[j], sizeof(RECORD_DATA_T));
                    break;
                }
            }

            //说明该数据的值为NULL，数据中心并没有给出该数值
            if (j == pUnitTmp->recordnum)
            {
                memset(&unit.record[i], 0, sizeof(RECORD_DATA_T));
                unit.record[i].optional = inReadRecord.recordOAD.optional;
                unit.record[i].road.oadMain = inReadRecord.recordOAD.road.oadMain;
                unit.record[i].road.oadCols.nNum = 1;
                unit.record[i].road.oadCols.oad[0].value = inReadRecord.recordOAD.road.oadCols.oad[i].value;
                unit.record[i].outDatalen = 0;
            }
        }
    }
    else
    {
        PRTL_FMT_LOGCC("record_data_adjust_other_element NO need to adjust\n");
        return ERR_OK;
    }

    //调整后的单步记录赋值过去
    PRTL_FMT_LOGCC("record_data_adjust_other_element need to adjust\n");
    memcpy(pUnit, &unit, sizeof(unit));

    return ret;
}

/**
*********************************************************************
* @brief：     单步出来的数据有时需要调整
* @param[in]： inReadRecord  - 筛选需要的数据
               unit          - 单个数据记录单元
* @return：    =0            - 该记录是所需
               <0            - 该记录不是所需
*********************************************************************
*/
int record_data_adjust(READ_RECORD_T inReadRecord, RECORD_UNIT_T *pUnit)
{
    int32          ret = ERR_OK;
    RECORD_UNIT_T *pUnitTmp;
    RECORD_UNIT_T unit;
    uint16        mainOI = 0;

    MEMZERO(&unit, sizeof(unit));
    pUnitTmp = pUnit;

    if (pUnit == NULL)
    {
        return ERR_PTR;
    }

    //读电表记录时，数据中心回复的顺序是事件开始时间、事件结束事件和事件记录号
    //应该调整为事件记录号、事件开始时间和事件结束事件
    mainOI = inReadRecord.recordOAD.road.oadMain.nObjID;
    if ((pUnitTmp->recordnum == 3) && (mainOI >= 0x3000) && (mainOI <= 0x302F))
    {
        if ((record_data_is_event_element(inReadRecord, pUnitTmp->record[0].road.oadCols.oad[0])) &&
            (record_data_is_event_element(inReadRecord, pUnitTmp->record[1].road.oadCols.oad[0])) &&
            (record_data_is_event_element(inReadRecord, pUnitTmp->record[2].road.oadCols.oad[0])))
        {
            ret = record_data_adjust_event_element(inReadRecord, pUnit);
        }
    }

    // inReadRecord中的数据个数与pUnit中不一致，需调整，理论上这个调整是包含上面的事件内容调整的，事件内容调整可删除
    ret = record_data_adjust_other_element(inReadRecord, pUnit);
    if (ret != 0)
    {
        PRTL_FMT_DEBUG("record_data_adjust_other_element err ret[%d]\n", ret);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      通用参数获取
* @param[in]： pRSD          -  行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：     =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 general_param_get(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_METER_T *pInReadMeter, RELY_INFO_T *pRelyInfo)
{
    int32 nRet = ERR_NORMAL;
    READ_METER_T   inReadMeter = {0};
    RELY_INFO_T    relyInfo = {0};

    switch (pOopRsd->choice)
    {
    case 0://方法0
    case 1://方法1
    case 2://方法2
    case 3://方法3
        {
            if (pOopRsd->choice == 0)
            {
                inReadMeter.startInfoNum = 1;
                inReadMeter.endInfoNum = NO_VIR_MTR_MAX_NUM;
            }       
            else if(pOopRsd->choice == 1)
            {
                if (pOopRsd->sel1.value.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.startInfoNum = pOopRsd->sel1.value.wVal;
                    inReadMeter.endInfoNum = pOopRsd->sel1.value.wVal;
                }
                else if (pOopRsd->sel1.value.type == DT_OAD)
                {
                    inReadMeter.startInfoNum = pOopRsd->sel1.value.oad.value;
                    inReadMeter.endInfoNum = pOopRsd->sel1.value.oad.value;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type = [%d]\n", pOopRsd->sel1.value.type);
                }
            }
            else if(pOopRsd->choice == 2)//前闭后开
            {
                if (pOopRsd->sel2.from.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.startInfoNum = pOopRsd->sel2.from.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                }

                if (pOopRsd->sel2.to.type == DT_LONG_UNSIGNED)
                {
                    inReadMeter.endInfoNum = pOopRsd->sel2.to.wVal;
                }
                else
                {
                    PRTL_FMT_LOGCC("pOopRsd->sel2.from.type = [%d]\n", pOopRsd->sel2.from.type);
                }
            }
            else if(pOopRsd->choice == 3)
            {
                uint32 i     = 0;
                uint32 j     = 0;
                uint32 start = 0;
                uint32 end   = 0;
                inReadMeter.startInfoNum = 1;
                inReadMeter.endInfoNum = NO_VIR_MTR_MAX_NUM;
                for (i = 0; i < NO_VIR_MTR_MAX_NUM + 1; i++)
                {
                    inReadMeter.isValid[i] = 1;
                }

                for (i = 0; i < pOopRsd->sel3.nNum; i++)
                {
                    if (pOopRsd->sel3.item[i].from.type == DT_LONG_UNSIGNED)
                    {
                        start = pOopRsd->sel3.item[i].from.wVal;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                    }

                    if (pOopRsd->sel3.item[i].to.type == DT_LONG_UNSIGNED)
                    {
                        end = pOopRsd->sel3.item[i].to.wVal - 1;
                    }
                    else
                    {
                        PRTL_FMT_DEBUG("pOopRsd->sel1.value.type != DT_LONG_UNSIGNED\n");
                    }
                    for (j = start; j <= end; j++)
                    {
                        inReadMeter.isValid[j] = 0;
                    }
                }
            }
            else
            {
                nRet =  ERR_NORMAL;
            }
            nRet = ERR_OK;
        }
        break;

    default:
        nRet = ERR_NORMAL;
        break;
    }

    if (nRet == ERR_OK)
    {
        relyInfo.oopRsd  = pOopRsd;
        relyInfo.oopRcsd = *pOopRcsd;
        relyInfo.bpBreakInfo = pApduInfo->bpBreakInfo;
        *pInReadMeter = inReadMeter;
        *pRelyInfo     = relyInfo;
    }

    return nRet;
}

/**
*********************************************************************
* @brief：     读记录型档案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_meter_read(DB_CLIENT hUdp, const READ_METER_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60000200:
        {
            DATA_CONVERT_T convert;
            OOP_METER_T oopMeter = {0};                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if METER_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定档案\n");

            oopMeterNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadMeter.startInfoNum; i <= inReadMeter.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_oop_meter(relyInfo.bpBreakInfo, i) || inReadMeter.isValid[i] == 1)
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if METER_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_METER_T), (uint8*)&oopMeter, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_METER_T)))
                #else
                ret = app_get_meter_list(i, &oopMeter);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到档案 序号为%d\n", oopMeter.nIndex);

                //转换成报文数据
                convert.srcBuf = &oopMeter;
                convert.srcLen = sizeof(oopMeter);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                #if AREA_ZHEJIANG
                nRet = area_data_to_buf_oop_meter(pOadInfo, &convert); 
                #else
                nRet = data_to_buf_oop_meter(pOadInfo, &convert);  //入参
                #endif
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个档案报文:", i);
                        oopMeterNum ++;
                        if (bufAllLen + dOffset >= maxBufLen) //虽然本次没有超最大长度，但下次会
                        {
                            isBufFull = TRUE;
                            PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                        }
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("class11_invoke_get_oop_meter isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个档案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型档案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_searchmeter_res_all_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60020200:
        {
            DATA_CONVERT_T convert;
            OOP_SEARCHMET_RES_T oopSearchMer;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopSearchMerNum = 0;
            uint32 oopSearchMerNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&oopSearchMer, sizeof(oopSearchMer));

            #if SEEK_METER_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定档案\n");

            oopSearchMerNumOffset = offset;                                     //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_searchmeter_res_all(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all after i = %d\n", i);

                #if SEEK_METER_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_SEARCHMET_RES_T), (uint8*)&oopSearchMer, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_SEARCHMET_RES_T)))
                #else
                ret = app_get_seek_meter_list(i, &oopSearchMer);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到找到搜表结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &oopSearchMer;
                convert.srcLen = sizeof(oopSearchMer);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class8_searchmeter_res_all(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个搜表报文:", i);
                        oopSearchMerNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_searchmeter_res_all_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个搜表报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopSearchMerNumOffset] = oopSearchMerNum;   //重新记数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型采集状态集
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E record_meter_acq_state(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60320200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQ_RLUESTATE_T acqstate;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 acqstateNum = 0;
            uint32 acqstateNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&acqstate, sizeof(acqstate));
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;

            PRTL_FMT_LOGCC("获取指定采集状态\n");

            acqstateNumOffset = offset;                                         //数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                if (slicing_is_comtinue_oop_meter(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }

                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_ACQ_RLUESTATE_T), (uint8*)&acqstate, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQ_RLUESTATE_T)))
                {
                    continue;
                }

                //转换成报文数据
                convert.srcBuf = &acqstate;
                convert.srcLen = sizeof(acqstate);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_acq_state(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集状态报文:", i);
                        acqstateNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("获取采集状态信息长度[%d]超最大长度[%d]\n", bufAllLen, maxBufLen);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个档案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            outRecordBuf->recordBuf[acqstateNumOffset] = acqstateNum;   //重新记数量
            outRecordBuf->bufLen = offset;

            result = DATA_SUCCESS;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型任务
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_task_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60120200:
        {
            DATA_CONVERT_T convert;
            OOP_TASK_T oopTask = {0};                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopTaskNum = 0;
            uint32 oopTaskNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if TASK_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定档案\n");

            oopTaskNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_task(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if TASK_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_TASK_T), (uint8*)&oopTask, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_TASK_T)))
                #else
                ret = app_get_task_list(i, &oopTask);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到任务 序号为%d\n", oopTask.id);

                //转换成报文数据
                convert.srcBuf = &oopTask;
                convert.srcLen = sizeof(oopTask);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_task_config_uint(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个任务报文:", i);
                        oopTaskNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_task_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个任务报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopTaskNumOffset] = oopTaskNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型普通方案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_normal_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60140200:
        {
            DATA_CONVERT_T convert;
            OOP_PLAN_NORMAL_T oopPlanNpor = {0};                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopPlanNorNum = 0;
            uint32 oopPlanNorNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if NOR_PLAN_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定普通方案\n");

            oopPlanNorNumOffset = offset;                                       //方案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_normal_scheme(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme after i = %d\n", i);

                #if NOR_PLAN_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_PLAN_NORMAL_T), (uint8*)&oopPlanNpor, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_PLAN_NORMAL_T)))
                #else
                //ret = app_get_meter_list(i, &oopPlanNpor);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到普通方案 序号为%d\n", oopPlanNpor.planID);

                //转换成报文数据
                convert.srcBuf = &oopPlanNpor;
                convert.srcLen = sizeof(oopPlanNpor);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_normal_scheme(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个普通方案报文:", i);
                        oopPlanNorNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_normal_scheme_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个普通方案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopPlanNorNumOffset] = oopPlanNorNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}


/**
*********************************************************************
* @brief：     读记录型透明方案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_trans_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60180200:
        {
            DATA_CONVERT_T convert;
            OOP_PLAN_TRANS_T oopPlanNtrans = {0};                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopPlanTransNum = 0;
            uint32 oopPlanTransNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if NOR_PLAN_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定普通方案\n");

            oopPlanTransNumOffset = offset;                                       //方案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_normal_scheme(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_normal_scheme after i = %d\n", i);

                #if NOR_PLAN_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_PLAN_TRANS_T), (uint8*)&oopPlanNtrans, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_PLAN_TRANS_T)))
                #else
                //ret = app_get_meter_list(i, &oopPlanNpor);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到普通方案 序号为%d\n", oopPlanNtrans.planID);

                //转换成报文数据
                convert.srcBuf = &oopPlanNtrans;
                convert.srcLen = sizeof(oopPlanNtrans);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_transparent_scheme(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个普通方案报文:", i);
                        oopPlanTransNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_normal_scheme_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个普通方案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopPlanTransNumOffset] = oopPlanTransNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型事件方案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_event_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60160200:
        {
            DATA_CONVERT_T convert;
            OOP_PLAN_EVENT_T oopPlanEvent = {0};                              //每个事件方案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopPlanEventNum = 0;
            uint32 oopPlanEventNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if EVENT_PLAN_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定事件方案\n");

            oopPlanEventNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_event_scheme(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if EVENT_PLAN_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_PLAN_EVENT_T), (uint8*)&oopPlanEvent, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_PLAN_EVENT_T)))
                #else
                ret = app_get_plan_event_list(i, &oopPlanEvent);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到事件方案 序号为%d\n", oopPlanEvent.planID);

                //转换成报文数据
                convert.srcBuf = &oopPlanEvent;
                convert.srcLen = sizeof(oopPlanEvent);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_event_scheme(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个事件方案报文:", i);
                        oopPlanEventNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_event_scheme_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个事件方案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopPlanEventNumOffset] = oopPlanEventNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型上报方案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_report_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x601C0200:
        {
            DATA_CONVERT_T convert;
            OOP_PLAN_REPORT_T oopPlanReport = {0};                              //每个事件方案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopPlanReportNum = 0;
            uint32 oopPlanReportNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));

            #if EVENT_PLAN_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定上报方案\n");

            oopPlanReportNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_report_scheme(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if REPORT_PLAN_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(oopPlanReport), (uint8*)&oopPlanReport, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_PLAN_REPORT_T)))
                #else
                ret = app_get_plan_report_list(i, &oopPlanReport);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到上报方案 序号为%d\n", oopPlanReport.planID);

                //转换成报文数据
                convert.srcBuf = &oopPlanReport;
                convert.srcLen = sizeof(oopPlanReport);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_report_plan(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个上报方案报文:", i);
                        oopPlanReportNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_event_scheme_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个上报方案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopPlanReportNumOffset] = oopPlanReportNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;
        case 0x601E0200:
        {
            DATA_CONVERT_T convert;
            OOP_PLAN_ENCRYPT_T oopPlanEncrypt = {0};                              //每个事件方案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopPlanEncryptNum = 0;
            uint32 oopPlanEncryptNumOffset = 0;
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;

            MEMZERO(&convert, sizeof(convert));

            PRTL_FMT_LOGCC("获取指定转加密方案\n");

            oopPlanEncryptNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_report_scheme(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(oopPlanEncrypt), (uint8*)&oopPlanEncrypt, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_PLAN_ENCRYPT_T)))
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到转加密方案 序号为%d\n", oopPlanEncrypt.planID);

                //转换成报文数据
                convert.srcBuf = &oopPlanEncrypt;
                convert.srcLen = sizeof(oopPlanEncrypt);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_encryption_plan(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个转加密方案报文:", i);
                        oopPlanEncryptNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_encryption_scheme_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个转加密方案报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopPlanEncryptNumOffset] = oopPlanEncryptNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型档案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_acq_monitor_read(DB_CLIENT hUdp, const READ_NOR_BP_T readNorBp, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60340200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQ_MONITOR_T acqMonitor;                                   //每个任务监控信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&acqMonitor, sizeof(acqMonitor));

            PRTL_FMT_LOGCC("获取指定采集任务状态\n");

            #if ACQ_MONITOR_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            oopMeterNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = readNorBp.startInfoNum; i <= readNorBp.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_acq_monitor(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if ACQ_MONITOR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_ACQ_MONITOR_T), (uint8*)&acqMonitor, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQ_MONITOR_T)))
                #else
                //ret = app_get_acq_monitor_list(i, &acqMonitor);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到采集任务状态 序号为%d\n", acqMonitor.id);

                //转换成报文数据
                convert.srcBuf = &acqMonitor;
                convert.srcLen = sizeof(acqMonitor);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class8_acq_monitor(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集任务状态报文:", i);
                        oopMeterNum ++;
                        dOffset = 0;
                        dOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        dOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_acq_monitor_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个采集任务状态报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读记录型档案
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_acq_rule_read(DB_CLIENT hUdp, const READ_NOR_BP_T readNorBp, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x601E0200:
        {
            DATA_CONVERT_T convert;
            OOP_ACQRULE_T acqRule;                                           //每个任务监控信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 oopMeterNum = 0;
            uint32 oopMeterNumOffset = 0;
            MEMZERO(&acqRule, sizeof(acqRule));

            PRTL_FMT_LOGCC("获取指定采集任务状态\n");

            #if ACQ_MONITOR_READ_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            oopMeterNumOffset = offset;                                         //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = readNorBp.startInfoNum; i <= readNorBp.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue_acq_monitor(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_oop_meter after i = %d\n", i);

                #if ACQ_MONITOR_READ_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_ACQRULE_T), (uint8*)&acqRule, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ACQRULE_T)))
                #else
                //ret = app_get_acq_monitor_list(i, &acqMonitor);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到采集规则 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &acqRule;
                convert.srcLen = sizeof(acqRule);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class8_acq_monitor(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个采集规则报文:", i);
                        oopMeterNum ++;
                        dOffset = 0;
                        dOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        dOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("record_acq_rule_read isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个采集规则报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[oopMeterNumOffset] = oopMeterNum;   //重新记档案数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

/**
*********************************************************************
* @brief：     读任务记录表
* @param[in]： hUdp          - 数据中心句柄
               inReadRecord  - 筛选需要的数据
               relyInfo      - 依赖数据
               readRecNum    - 读记录数量，：0：读全部记录 n：读最近的n条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_task_table_read(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    int32              oadNum = 0;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0;
    uint8              *recordBufTmp;
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    OOP_RCSD_T         oopRcsdtmp;
    MS_T               *ms;
    OOP_COAD_T         road;

    int32  ret    = ERR_NORMAL;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 oopRecNumPn  = 0;       //单个记录表中记录条数
    uint32 oopRecNumAll = 0;       //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint32 index = 0;              //比如用采集成功时间筛选时，用于记录到哪个点

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        PRTL_FMT_DEBUG("record_task_table_read param err!\n");
        return ERR_PTR;
    }
    else
    {
        PRTL_FMT_LOGCC("record_task_table_read param ok! maxBufLen = %d\n", maxBufLen);
    }

    memset(&road, 0, sizeof(road));

    /*如果只读最近n条记录*/
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;
    if (readRecNum > 0)
    {
        inReadRecordTmp.sortType = 0x01;
    }
    else
    {
        inReadRecordTmp.sortType = 0x00;
    }

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    ms = relyInfo.ms;

    /*申请一份内存，注意释放*/
    recordBufTmp = (uint8 *)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    offset = 0;
    recordBufTmp[offset++] = 0x00;   //记录数先存0   
    while (ms)
    {
        /*判断如果是应该分帧，跳过已经处理过的测量点，填入新的起始时间*/
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;
        oopRecNumPn = 0;
        if (slicing_is_comtinue_ms(relyInfo.bpBreakInfo, ms->MtrNo, &inReadRecordTmp, &oopRecNumPn, &index))
        {
            ms = ms->next;
            continue;
        }

        /*开始读记录*/
        inReadRecordTmp.recordOAD.infoNum = ms->MtrNo;
        diag_ycmsg_open();
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        diag_ycmsg_close();
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_DEBUG("record_task_table_read db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
            ms = ms->next;
            if (handle != 0)
            {
                /*结束读记录*/
                db_read_record_end(hUdp, handle);
                handle = 0;
            }
            continue;
        }
        else
        {
            PRTL_FMT_LOGCC("record_task_table_read db_read_record_start ok infoNum = %d recNum = %d\n", 
                inReadRecordTmp.recordOAD.infoNum, recNum);
        }

        /*循环单步读操作*/
        int i = 0;
        int k = 0;
        SPC_OAD_E spcOad = SPC_NO;
        stepReadRecord.MAC.nNum = 0x06;
        memcpy(&stepReadRecord.MAC.value[0], &ms->oopMeter.basic.tsa.add[0], 6);
        PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "MAC address for single-step filtering: ");
        for (i = 0; i < recNum; i++)
        {
            diag_ycmsg_open();
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            diag_ycmsg_close();
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("record_task_table_read db_read_record_step err = %d!\n", ret);
                continue;
            }

            /* 跳过断点前已经处理过的单步信息 */
            if (i < index)
            {
                PRTL_FMT_LOGCC("record_task_table_read db_read_record_step i < index index = %d!\n", index);
                continue;
            }

            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(inReadRecordTmp, stepRecordOut);
            if (ret < ERR_OK)
            {
                oadNum = 0;
                continue;
            }

            /* 单步出来的数据有时需要调整 */
            ret = record_data_adjust(inReadRecordTmp, &stepRecordOut);
            if (ret < ERR_OK)
            {
                oadNum = 0;
                continue;
            }

            ret = task_table_data_to_698buf_malloc(stepRecordOut, oopRcsdtmp, ms, &spcOad, &pBufTmp, &bufLenTmp);
            if (ret < ERR_OK)
            {
                db_read_record_end(hUdp, handle);
                PRTL_FMT_DEBUG("record_task_table_read data_task_table_to_698buf_malloc err[%d]!\n", ret);
                free(recordBufTmp);
                recordBufTmp = NULL;
                return ERR_NORMAL;
            }
            else
            {
                /* 打印单步读信息 */
                PRTL_FMT_LOGCC("*****************打印单步读信息**********************\n");
                PRTL_FMT_LOGCC("stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                for (k = 0; k < stepRecordOut.recordnum; k++)
                {
                    PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadMain.value = 0x%08x\n", k, stepRecordOut.record[k].road.oadMain.value);
                    PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadCols.nNum = %d\n", k, stepRecordOut.record[k].road.oadCols.nNum);
                    if (stepRecordOut.record[k].road.oadCols.nNum != 0)
                    {
                        PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadCols.oad[0].value = 0x%08x\n", k, stepRecordOut.record[k].road.oadCols.oad[0].value);
                    }
                }
                PRTL_FMT_LOGCC("oadNum = %d\n", oadNum);
                PRTL_BUF_LOGCC(pBufTmp, bufLenTmp, "pBufTmp:");
                PRTL_FMT_LOGCC("*****************打印单步读信息结束********************\n");

                /* 拷贝数据域内容到recordBufTmp */
                memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                offset += bufLenTmp; 
                oadNum += stepRecordOut.recordnum;

                /* 如果oadNum达到road.oadCols.nNum，如果拼到完整格式记录 */
                if ((oadNum >= road.oadCols.nNum) || ((spcOad != SPC_NO) && (oadNum >= road.oadCols.nNum - 1)))
                {
                    oadNum = 0;

                    //方法10
                    if ((readRecNum > 0) && (oopRecNumPn == readRecNum))
                    {
                        oopRecNumPn = 0;
                        offset = 0;
                        break;
                    }

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    //方法4 5 6 7 8实时记下断点信息
                    switch (inReadRecord.cType)
                    {
                    case COL_TM_START:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStartTm;
                        outRecordBuf->breakInfo.index = i;
                        break;
                    case COL_TM_END:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colEndTm;
                        outRecordBuf->breakInfo.index = i;
                        break;
                    case COL_TM_STORE:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                        break;       
                    default:
                        PRTL_FMT_DEBUG("record_task_table_read inReadRecord.tmType err!\n");
                        db_read_record_end(hUdp, handle);
                        free(pBufTmp);
                        pBufTmp = NULL;
                        free(recordBufTmp);
                        recordBufTmp = NULL;
                        return ERR_PTR;
                        break;
                    }

                    // 方法10实时记下断点信息
                    if (readRecNum > 0)
                    {
                        outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;
                    }
                    else
                    {
                        outRecordBuf->breakInfo.recNumSeg = 0;
                    }
                    
                    
                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll ++;
                        offset = 0;
                    }
                    else
                    {
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        offset = 0;
                        isBufFull = TRUE;

                        PRTL_FMT_LOGCC("isBufFull == TRUE isBufFull = %d\n", isBufFull);

                    }

                    //打印完整记录报文信息
                    PRTL_FMT_LOGCC("*****************打印完整记录信息*********************\n");
                    PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                    PRTL_FMT_TIME("stepRecordOut.colStartTm", stepRecordOut.colStartTm);
                    PRTL_FMT_TIME("stepRecordOut.colEndTm", stepRecordOut.colEndTm);
                    PRTL_FMT_TIME("stepRecordOut.colStoreTm", stepRecordOut.colStoreTm);
                    PRTL_FMT_LOGCC("index = %d\n", index);
                    PRTL_FMT_LOGCC("outRecordBuf->breakInfo.index = %d\n", outRecordBuf->breakInfo.index);
                    PRTL_FMT_LOGCC("oopRecNumPn = %d\n", oopRecNumPn);
                    PRTL_FMT_LOGCC("oopRecNumAll = %d\n", oopRecNumAll);
                    PRTL_FMT_LOGCC("bufAllLenLast = %d\n", bufAllLenLast);
                    PRTL_FMT_LOGCC("bufAllLen = %d\n", bufAllLen);
                    PRTL_FMT_LOGCC("len = %d\n", len);
                    PRTL_BUF_LOGCC(&outRecordBuf->recordBuf[bufAllLenLast], len, "outRecordBuf->recordBuf[%d]:", bufAllLenLast);
                    PRTL_FMT_LOGCC("*****************打印完整记录信息结束********************\n");
                }

                free(pBufTmp);
                pBufTmp = NULL;
            }

            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }
        }

        if (!isBufFull)
        {
            outRecordBuf->bufLen = bufAllLen;

            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
            {
                outRecordBuf->breakInfo.reqType = 0x03;
                outRecordBuf->breakInfo.appSlicingFlag = 0x01;
                outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
                outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
            }
            else
            {
                /*清除断点信息*/
                memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
            }
        }
        else
        {
            outRecordBuf->breakInfo.reqType = 0x03;
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
            {
                outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
            }
            else
            {
                outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
            }
            outRecordBuf->breakInfo.metsn = ms->MtrNo;
            memcpy(&outRecordBuf->breakInfo.MAC, &ms->oopMeter.basic.tsa.add[0], sizeof(OOP_OCTETSTR6_T));
            outRecordBuf->bufLen = bufAllLenLast;                               //填写返回报文长度
            break;
        }

        /*结束读记录*/
        db_read_record_end(hUdp, handle);
        ms = ms->next;
    }

    /* 没有找到有效数据 满足协议一致性要求*/
    if (oopRecNumAll == 0)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            outRecordBuf->breakInfo.reqType = 0x03;
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
            outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
        }
    
        outRecordBuf->bufLen = 1;
        PRTL_FMT_DEBUG("no valid data was found, oopRecNumAll == 0\n");
    }

    if (oopRecNumAll > 0x80)
    {
        offlen = set_len_offset(oopRecNumAll, lenBuf);
        memmove(&outRecordBuf->recordBuf[offlen], &outRecordBuf->recordBuf[1], outRecordBuf->bufLen - 1);
        memcpy(&outRecordBuf->recordBuf[0], lenBuf, offlen);
        outRecordBuf->bufLen += (offlen - 1);
    }
    else
    {
        outRecordBuf->recordBuf[0] = oopRecNumAll;   //填写总记录数 
    }

    free(recordBufTmp);
    recordBufTmp = NULL;
    return ERR_OK;
}



/**
*********************************************************************
* @brief：     读任务记录表
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_transparent_task_table_read(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    READ_RECORD_T      inReadRecordTmp;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    int32              oadNum = 0;
    uint8              *pBufTmp = NULL;
    uint32             bufLenTmp = 0;
    uint32             offset = 0;
    uint8              *recordBufTmp;
    uint32             bufAllLen = 0;
    uint32             bufAllLenLast = 0;
    BOOL               isBufFull = FALSE;
    OOP_RCSD_T         oopRcsdtmp;
    MS_T               *ms;
    OOP_COAD_T         road;

    int32  ret    = ERR_NORMAL;
    uint32 handle = 0;
    uint32 recNum = 0;
    uint32 oopRecNumPn  = 0;       //单个记录表中记录条数
    uint32 oopRecNumAll = 0;       //总记录表中记录条数
    uint32 len = 0;
    uint8  offlen = 0;
    uint8  lenBuf[3] = {0};
    uint32 index = 0;              //比如用采集成功时间筛选时，用于记录到哪个点
    uint32 tmpTime = 0;

    /*入参检查*/
    if ((hUdp <= 0) || (outRecordBuf == NULL) || (outRecordBuf->recordBuf == NULL))
    {
        PRTL_FMT_DEBUG("record_task_table_read param err!\n");
        return ERR_PTR;
    }
    else
    {
        PRTL_FMT_LOGCC("record_task_table_read param ok! maxBufLen = %d\n", maxBufLen);
    }

    memset(&road, 0, sizeof(road));

    /*如果只读最近n条记录*/
    inReadRecordTmp = inReadRecord;
    oopRcsdtmp = relyInfo.oopRcsd;
    if (readRecNum > 0)
    {
        inReadRecordTmp.sortType = 0x01;
    }
    else
    {
        inReadRecordTmp.sortType = 0x00;
    }

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));

    ms = relyInfo.ms;

    /*申请一份内存，注意释放*/
    recordBufTmp = (uint8 *)malloc(RECORD_LEN_MAX);
    memset(recordBufTmp, 0, RECORD_LEN_MAX);

    offset = 0;
    recordBufTmp[offset++] = 0x00;   //记录数先存0   
    while (ms)
    {
        /*判断如果是应该分帧，跳过已经处理过的测量点，填入新的起始时间*/
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms before relyInfo.bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo->appSlicingFlag);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms beforerelyInfo.bpBreakInfo->metsn = %d\n", relyInfo.bpBreakInfo->metsn);
        // PRTL_FMT_TIME("slicing_is_comtinue_ms beforerelyInfo.bpBreakInfo->tmStartSeg", relyInfo.bpBreakInfo->tmStartSeg);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms beforerelyInfo.bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo->seq);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms beforerelyInfo.bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo->isEnd);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms beforerelyInfo.bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo->index);
        inReadRecordTmp.cStart = inReadRecord.cStart;
        inReadRecordTmp.cEnd = inReadRecord.cEnd;
        oopRecNumPn = 0;
        if (slicing_is_comtinue_ms(relyInfo.bpBreakInfo, ms->MtrNo, &inReadRecordTmp, &oopRecNumPn, &index))
        {
            ms = ms->next;
            continue;
        }
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.cType = %d\n", inReadRecordTmp.cType);
        // PRTL_FMT_TIME("slicing_is_comtinue_ms after inReadRecordTmp.cStart", inReadRecordTmp.cStart);
        // PRTL_FMT_TIME("slicing_is_comtinue_ms after inReadRecordTmp.cEnd", inReadRecordTmp.cEnd);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.sortType = %d\n", inReadRecordTmp.sortType);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.logicId = %d\n", inReadRecordTmp.recordOAD.logicId);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.infoNum = %d\n", inReadRecordTmp.recordOAD.infoNum);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.oadMain = 0x%08x\n", inReadRecordTmp.recordOAD.road.oadMain.value);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after inReadRecordTmp.oadMain nNum = 0x%08x\n", inReadRecordTmp.recordOAD.road.oadCols.nNum);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_ms after index = %d\n", index);

        /*开始读记录*/
        inReadRecordTmp.recordOAD.infoNum = ms->MtrNo;
        diag_ycmsg_open();
        ret = db_read_record_start(hUdp, &inReadRecordTmp, &handle, &recNum);
        diag_ycmsg_close();
        if ((ret != ERR_OK) || (recNum == 0))
        {
            PRTL_FMT_LOGCC("record_task_table_read db_read_record_start err! ret = %d, infoNum = %d recNum = %d\n", 
                ret, inReadRecordTmp.recordOAD.infoNum, recNum);
            ms = ms->next;
            if (handle != 0)
            {
                /*结束读记录*/
                db_read_record_end(hUdp, handle);
                handle = 0;
            }
            continue;
        }
        else
        {
            PRTL_FMT_LOGCC("record_task_table_read db_read_record_start ok infoNum = %d recNum = %d\n", 
                inReadRecordTmp.recordOAD.infoNum, recNum);
        }
        

        /*循环单步读操作*/
        int i = 0;
        int j = 0;
        int k = 0;
        SPC_OAD_E spcOad = SPC_NO;
        stepReadRecord.MAC.nNum = 0x06;
        memcpy(&stepReadRecord.MAC.value[0], &ms->oopMeter.basic.tsa.add[0], 6);
        PRTL_BUF_LOGCC(&stepReadRecord.MAC.value[0], 6, "MAC address for single-step filtering: ");
        for (i = 0; i < recNum; i++)
        {
            diag_ycmsg_open();
            ret = db_read_record_step(hUdp, handle, &stepReadRecord, &stepRecordOut);
            diag_ycmsg_close();
            if (ret != ERR_OK)
            {
                PRTL_FMT_DEBUG("record_task_table_read db_read_record_step err = %d!\n", ret);
                continue;
            }

            /* 跳过断点前已经处理过的单步信息 */
            if (i < index)
            {
                PRTL_FMT_DEBUG("record_task_table_read db_read_record_step i < index index = %d!\n", index);
                continue;
            }

            /* 判断是否是所需记录单元 */
            ret = record_data_is_valid(inReadRecordTmp, stepRecordOut);
            if (ret < ERR_OK)
            {
                oadNum = 0;
                continue;
            }

            /* 单步出来的数据有时需要调整 */
            ret = record_data_adjust(inReadRecordTmp, &stepRecordOut);
            if (ret < ERR_OK)
            {
                oadNum = 0;
                continue;
            }

            ret = data_transparent_task_table_to_698buf_malloc(stepRecordOut, &oadNum, oopRcsdtmp, &spcOad, &pBufTmp, &bufLenTmp);
            if (ret < ERR_OK)
            {
                db_read_record_end(hUdp, handle);
                PRTL_FMT_DEBUG("record_task_table_read data_task_table_to_698buf_malloc err!\n");
                free(recordBufTmp);
                recordBufTmp = NULL;
                return ERR_NORMAL;
            }
            else
            {
                /* 打印单步读信息 */
                PRTL_FMT_LOGCC("*****************打印单步读信息**********************\n");
                PRTL_FMT_LOGCC("stepRecordOut.recordnum = %d\n", stepRecordOut.recordnum);
                for (k = 0; k < stepRecordOut.recordnum; k++)
                {
                    PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadMain.value = 0x%08x\n", k, stepRecordOut.record[k].road.oadMain.value);
                    PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadCols.nNum = %d\n", k, stepRecordOut.record[k].road.oadCols.nNum);
                    if (stepRecordOut.record[k].road.oadCols.nNum != 0)
                    {
                        PRTL_FMT_LOGCC("stepRecordOut.record[%d].road.oadCols.oad[0].value = 0x%08x\n", k, stepRecordOut.record[k].road.oadCols.oad[0].value);
                    }
                }
                PRTL_FMT_LOGCC("oadNum = %d\n", oadNum);
                PRTL_BUF_LOGCC(pBufTmp, bufLenTmp, "pBufTmp:");

                /*因为每次从接口中能读出若干个次OAD，但需要考虑拼成一条记录报文*/
                DateTimeHex_t oopTime_s;
                if (oadNum == 0)
                {
                    for (j = 0; j < oopRcsdtmp.nNum; j++)
                    {
                        if (oopRcsdtmp.cols[j].choice == 0x00)
                        {
                            if ((oopRcsdtmp.cols[j].oad.nObjID == 0x4001) || (oopRcsdtmp.cols[j].oad.nObjID == 0x202A)) //通讯地址
                            {
                                recordBufTmp[offset++] = DT_TSA;
                                recordBufTmp[offset++] = 0x07;
                                recordBufTmp[offset++] = 0x05;
                                memcpy(&recordBufTmp[offset], &ms->oopMeter.basic.tsa.add[0], sizeof(OOP_OCTETSTR6_T));
                                offset += sizeof(OOP_OCTETSTR6_T);
                            }
                            if(oopRcsdtmp.cols[j].oad.nObjID == 0x6040)
                            {
                                PRTL_FMT_TIME("follow the 0x60400200 time scale of the data", stepRecordOut.colStartTm);
                                // 注：-8个小时(28800)转成北京时间
                                tmpTime = stepRecordOut.colStartTm - COMPENSATION_TIME;
                                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                                {
                                    PRTL_FMT_LOGCC("follow the 0x60400200 time scale of the data err!\n");
                                    //是否需要其他处理？？
                                }
                                recordBufTmp[offset++] = DT_DATETIME_S;
                                memcpy(&recordBufTmp[offset], &oopTime_s, sizeof(DateTimeHex_t));
                                offset += sizeof(DateTimeHex_t);
                            }
                            if(oopRcsdtmp.cols[j].oad.nObjID == 0x6041)
                            {
                                PRTL_FMT_TIME("follow the 0x60410200 time scale of the data", stepRecordOut.colEndTm);
                                // 注：-8个小时(28800)转成北京时间
                                tmpTime = stepRecordOut.colEndTm - COMPENSATION_TIME;
                                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                                {
                                    PRTL_FMT_LOGCC("follow the 0x60410200 time scale of the data err!\n");
                                    //是否需要其他处理？？
                                }
                                recordBufTmp[offset++] = DT_DATETIME_S;
                                memcpy(&recordBufTmp[offset], &oopTime_s, sizeof(DateTimeHex_t));
                                offset += sizeof(DateTimeHex_t);
                            }
                            if(oopRcsdtmp.cols[j].oad.nObjID == 0x6042)
                            {
                                PRTL_FMT_TIME("follow the 0x60420200 time scale of the data", stepRecordOut.colStoreTm);
                                // 注：-8个小时(28800)转成北京时间
                                tmpTime = stepRecordOut.colStoreTm - COMPENSATION_TIME;
                                if (!DT_Time2DateTimeHex(tmpTime, &oopTime_s))
                                {
                                    PRTL_FMT_LOGCC("follow the 0x60420200 time scale of the data err!\n");
                                    //是否需要其他处理？？
                                }
                                recordBufTmp[offset++] = DT_DATETIME_S;
                                memcpy(&recordBufTmp[offset], &oopTime_s, sizeof(DateTimeHex_t));
                                offset += sizeof(DateTimeHex_t);
                            }
                        }
                        else
                        {
                            road = oopRcsdtmp.cols[j].road;
                        }
                        
                    }

                    if (road.oadMain.value != 0)
                    {
                        recordBufTmp[offset++] = 0x01;
                        recordBufTmp[offset++] = road.oadCols.nNum;  
                    }                  
                }
                PRTL_FMT_LOGCC("*****************打印单步读信息结束********************\n");

                /* 拷贝数据域内容到recordBufTmp */
                memcpy(&recordBufTmp[offset], pBufTmp, bufLenTmp);
                offset += bufLenTmp; 
                oadNum += stepRecordOut.recordnum;

                /* 如果oadNum达到road.oadCols.nNum，如果拼到完整格式记录 */
                if ((oadNum >= road.oadCols.nNum) || ((spcOad != SPC_NO) && (oadNum >= road.oadCols.nNum - 1)))
                {
                    oadNum = 0;

                    //方法10
                    if ((readRecNum > 0) && (oopRecNumPn == readRecNum))
                    {
                        oopRecNumPn = 0;
                        offset = 0;
                        break;
                    }

                    bufAllLenLast = bufAllLen;
                    bufAllLen += offset;

                    //方法4 5 6 7 8实时记下断点信息
                    switch (inReadRecord.cType)
                    {
                    case COL_TM_START:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStartTm;
                        outRecordBuf->breakInfo.index = i;
                        break;
                    case COL_TM_END:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colEndTm;
                        outRecordBuf->breakInfo.index = i;
                        break;
                    case COL_TM_STORE:
                        outRecordBuf->breakInfo.tmStartSeg = stepRecordOut.colStoreTm;
                        break;       
                    default:
                        PRTL_FMT_DEBUG("record_task_table_read inReadRecord.tmType err!\n");
                        db_read_record_end(hUdp, handle);
                        free(pBufTmp);
                        pBufTmp = NULL;
                        free(recordBufTmp);
                        recordBufTmp = NULL;
                        return ERR_PTR;
                        break;
                    }

                    // 方法10实时记下断点信息
                    if (readRecNum > 0)
                    {
                        outRecordBuf->breakInfo.recNumSeg = oopRecNumPn;
                    }
                    else
                    {
                        outRecordBuf->breakInfo.recNumSeg = 0;
                    }
                    
                    
                    len = offset;

                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[bufAllLenLast], recordBufTmp, offset);
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        oopRecNumPn ++;
                        oopRecNumAll ++;
                        offset = 0;
                    }
                    else
                    {
                        memset(&recordBufTmp[0], 0x00, RECORD_LEN_MAX);
                        offset = 0;
                        isBufFull = TRUE;

                        PRTL_FMT_LOGCC("isBufFull == TRUE isBufFull = %d\n", isBufFull);

                    }

                    //打印完整记录报文信息
                    PRTL_FMT_LOGCC("*****************打印完整记录信息*********************\n");
                    PRTL_FMT_LOGCC("stepRecordOut.infoNum = %d\n", stepRecordOut.infoNum);
                    PRTL_FMT_TIME("stepRecordOut.colStartTm", stepRecordOut.colStartTm);
                    PRTL_FMT_TIME("stepRecordOut.colEndTm", stepRecordOut.colEndTm);
                    PRTL_FMT_TIME("stepRecordOut.colStoreTm", stepRecordOut.colStoreTm);
                    PRTL_FMT_LOGCC("index = %d\n", index);
                    PRTL_FMT_LOGCC("outRecordBuf->breakInfo.index = %d\n", outRecordBuf->breakInfo.index);
                    PRTL_FMT_LOGCC("oopRecNumPn = %d\n", oopRecNumPn);
                    PRTL_FMT_LOGCC("oopRecNumAll = %d\n", oopRecNumAll);
                    PRTL_FMT_LOGCC("bufAllLenLast = %d\n", bufAllLenLast);
                    PRTL_FMT_LOGCC("bufAllLen = %d\n", bufAllLen);
                    PRTL_FMT_LOGCC("len = %d\n", len);
                    PRTL_BUF_LOGCC(&outRecordBuf->recordBuf[bufAllLenLast], len, "outRecordBuf->recordBuf[%d]:", bufAllLenLast);
                    PRTL_FMT_LOGCC("*****************打印完整记录信息结束********************\n");
                }

                free(pBufTmp);
                pBufTmp = NULL;
            }

            if ((isBufFull) && (bufAllLenLast > 0))
            {
                break;
            }
        }

        if (!isBufFull)
        {
            outRecordBuf->bufLen = bufAllLen;

            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
            {
                outRecordBuf->breakInfo.appSlicingFlag = 0x01;
                outRecordBuf->breakInfo.isEnd = 0x01;                          //末尾帧
                outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
            }
            else
            {
                /*清除断点信息*/
                memset(&outRecordBuf->breakInfo, 0, sizeof(BP_INFO_T));
            }
        }
        else
        {
            outRecordBuf->breakInfo.appSlicingFlag = 0x01;
            outRecordBuf->breakInfo.isEnd = 0x00;                              //不是末尾帧
            if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
            {
                outRecordBuf->breakInfo.seq = BRK_SEQ_START;                   //首次序号
            }
            else
            {
                outRecordBuf->breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;   //序号加1
            }
            outRecordBuf->breakInfo.metsn = ms->MtrNo;
            memcpy(&outRecordBuf->breakInfo.MAC, &ms->oopMeter.basic.tsa.add[0], sizeof(OOP_OCTETSTR6_T));
            outRecordBuf->bufLen = bufAllLenLast;                               //填写返回报文长度
            break;
        }

        /*结束读记录*/
        db_read_record_end(hUdp, handle);
        ms = ms->next;
    }

    /* 没有找到有效数据 */
    if (oopRecNumAll == 0)
    {
        free(recordBufTmp);
        recordBufTmp = NULL;
        PRTL_FMT_DEBUG("no valid data was found, oopRecNumAll == 0, err = %d\n", ERR_ITEM);
        return ERR_ITEM;
    }

    if (oopRecNumAll > 0x80)
    {
        offlen = set_len_offset(oopRecNumAll, lenBuf);
        memmove(&outRecordBuf->recordBuf[offlen], &outRecordBuf->recordBuf[1], outRecordBuf->bufLen - 1);
        memcpy(&outRecordBuf->recordBuf[0], lenBuf, offlen);
        outRecordBuf->bufLen += (offlen - 1);
    }
    else
    {
        outRecordBuf->recordBuf[0] = oopRecNumAll;   //填写总记录数 
    }

    free(recordBufTmp);
    recordBufTmp = NULL;
    return ERR_OK;
}

#if DESC("读记录型转加密", 1)
/**
*********************************************************************
* @brief：     读记录型转加密
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_encryption_res_all_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                                //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));
   
    switch (pOadInfo->pTab->oad.value)
    {
        case 0x60200200:
        {
            DATA_CONVERT_T convert;
            OOP_ENCRYPTRES_T enCryption;                                        //每个电表的档案信息
            uint32 sOffset = 0;
            uint32 dOffset = 0;
            uint32 enCryptionNum = 0;
            uint32 enCryptionNumOffset = 0;

            MEMZERO(&convert, sizeof(convert));
            MEMZERO(&enCryption, sizeof(enCryption));

            #if ENCRYPTION_MOD == 1
            uint32      len = 0;
            NOMAL_OAD_T nomalOad;
            MEMZERO(&nomalOad, sizeof(nomalOad));
            nomalOad.logicId = 0x00;
            nomalOad.oad.value = pOadInfo->pTab->oad.value;
            #endif

            PRTL_FMT_LOGCC("获取指定转加密\n");

            enCryptionNumOffset = offset;                                     //档案数量偏移
            outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
            bufAllLen = offset;
            for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
            {
                //有断点时，找到上次执行到的i
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
                // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
                if (slicing_is_comtinue(relyInfo.bpBreakInfo, i))
                {
                    continue;
                }
                //PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all after i = %d\n", i);

                #if ENCRYPTION_MOD == 1
                nomalOad.infoNum = i;
                ret = db_read_nomal(hUdp, &nomalOad, sizeof(OOP_ENCRYPTRES_T), (uint8*)&enCryption, &len);
                if ((ret < ERR_OK) || (len != sizeof(OOP_ENCRYPTRES_T)))
                #else
                ret = app_get_encryption_res_list(i, &enCryption);
                if (ret < ERR_OK)
                #endif
                {
                    continue;
                }

                PRTL_FMT_LOGCC("找到转加密结果 序号为%d\n", i);

                //转换成报文数据
                convert.srcBuf = &enCryption;
                convert.srcLen = sizeof(enCryption);
                convert.sOffset = &sOffset;

                convert.dstBuf = &outRecordBuf->recordBuf[offset];
                convert.dstLen = maxBufLen - offset;
                convert.dOffset = &dOffset;

                //记录断点信息
                breakInfo.index = i;

                nRet = data_to_buf_class8_encryption_res_all(pOadInfo, &convert);  //入参
                if (nRet == DATA_SUCCESS)
                {
                    bufAllLen += dOffset;
                    if (bufAllLen <= maxBufLen)
                    {
                        memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                        offset += dOffset;
                        outRecordBuf->bufLen = offset;
                        PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个转加密报文:", i);
                        enCryptionNum ++;
                        dOffset = 0;
                        sOffset = 0;
                    }
                    else
                    {
                        dOffset = 0;
                        sOffset = 0;
                        isBufFull = TRUE;
                        PRTL_FMT_LOGCC("data_to_buf_class8_encryption_res_all isBufFull == TRUE isBufFull = %d\n", isBufFull);
                    }
                }
                else
                {
                    continue;
                }

                if ((isBufFull) && (offset > 0))
                {
                    break;
                }
            }

            if (!isBufFull)
            {
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
                {
                    breakInfo.appSlicingFlag = 0x01;
                    breakInfo.isEnd = 0x01;                           //末尾帧
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
                    breakInfo.objPre = 0x01;                          //对象属性
                }
                else
                {
                    /*清除断点信息*/
                    memset(&breakInfo, 0, sizeof(BP_INFO_T));
                }
            }
            else
            {
                breakInfo.reqType = 0x03;
                breakInfo.appSlicingFlag = 0x01;
                breakInfo.isEnd = 0x00;                              //不是末尾帧
                breakInfo.objPre = 0x01;                             //对象属性
                if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
                {
                    breakInfo.seq = BRK_SEQ_START;                   //首次序号
                }
                else
                {
                    breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
                }
            }

            //刷新断点信息
            memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

            PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个转加密报文:");
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
            PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

            result = DATA_SUCCESS;
            outRecordBuf->recordBuf[enCryptionNumOffset] = enCryptionNum;   //重新记数量
            outRecordBuf->bufLen = offset;
        }
        break;

        default:
        {
            return DATA_READ_WRITE_DENIED;
        }
        break;
    }

    return result;
}

#endif

#if DESC("读记录型载波子节点", 1)
/**
*********************************************************************
* @brief：      读记录型转加密
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_chip_node_rec_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, 
    RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                           //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    DATA_CONVERT_T convert;
    OOP_CHILDNODE_T chipnode;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 chipnodeNum = 0;
    uint32 chipnodeNumOffset = 0;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&chipnode, sizeof(chipnode));

    #if CHILDNODE_READ_MOD == 1
    uint32      len = 0;
    NOMAL_OAD_T nomalOad;
    MEMZERO(&nomalOad, sizeof(nomalOad));
    nomalOad.logicId = 0x00;
    nomalOad.oad.value = pOadInfo->pTab->oad.value;
    #endif
    
    /* 类型不存在 */
    if (pOadInfo->pTab->oad.value != 0xf2090500)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    PRTL_FMT_LOGCC("获取指定载波子节点信息\n");

    chipnodeNumOffset = offset;                                     //档案数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;
    for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
    {
        //有断点时，找到上次执行到的i
        // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->appSlicingFlag = %d\n", relyInfo.bpBreakInfo.appSlicingFlag);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->index = %d\n", relyInfo.bpBreakInfo.index);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->isEnd = %d\n", relyInfo.bpBreakInfo.isEnd);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->objPre = %d\n", relyInfo.bpBreakInfo.objPre);
        // PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all before bpBreakInfo->seq = %d\n", relyInfo.bpBreakInfo.seq);
        if (slicing_is_comtinue(relyInfo.bpBreakInfo, i))
        {
            continue;
        }
        //PRTL_FMT_LOGCC("slicing_is_comtinue_searchmeter_res_all after i = %d\n", i);

        #if CHILDNODE_READ_MOD == 1
        nomalOad.infoNum = i;
        ret = db_read_nomal(hUdp, &nomalOad, sizeof(chipnode), (uint8*)&chipnode, &len);
        if ((ret < ERR_OK) || (len != sizeof(chipnode)))
        #else
        ret = app_get_childnode_list(i, &chipnode);
        if (ret < ERR_OK)
        #endif
        {
            continue;
        }

        PRTL_FMT_LOGCC("找到载波子节点信息 序号为%d\n", i);

        //转换成报文数据
        convert.srcBuf = &chipnode;
        convert.srcLen = sizeof(chipnode);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_chip_node(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个载波子节点信息:", i);
                chipnodeNum ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("获取载波子节点信息长度[%d]超最大长度[%d]\n", bufAllLen, maxBufLen);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个载波子节点信息:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[chipnodeNumOffset] = chipnodeNum;   //重新记数量
    outRecordBuf->bufLen = offset;

    return result;
}

#endif

#if DESC("读记录型购电参数", 1)
/**
*********************************************************************
* @brief：      读记录型转加密
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_cfg_8107_rec_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, 
    RECORD_BUF_T *outRecordBuf)
{
    int       result = DATA_OTHER_REASON;
    uint32    offset = 0;
    uint32    i = 0;
    uint32    j = 0;
    int32     ret = ERR_NORMAL;
    OOP_DAR_E nRet = DATA_OTHER_REASON;
    uint32    bufAllLen = 0;
    BP_INFO_T breakInfo = {0};                                           //断点信息
    BOOL      isBufFull = FALSE;
    OAD_INFO_T *pOadInfo = relyInfo.pOadInfo;

    DATA_CONVERT_T convert;
    OOP_8107_CONFIG_T cfg8107;
    uint32 sOffset = 0;
    uint32 dOffset = 0;
    uint32 cfg8107Num = 0;
    uint32 cfg8107NumOffset = 0;

    /*清除断点信息, 需要注意应用分帧的未帧*/
    memset(&breakInfo, 0, sizeof(BP_INFO_T));

    MEMZERO(&convert, sizeof(convert));
    MEMZERO(&cfg8107, sizeof(cfg8107));

    uint32      len = 0;
    NOMAL_OAD_T nomalOad;
    MEMZERO(&nomalOad, sizeof(nomalOad));
    nomalOad.logicId = 0x00;
    nomalOad.oad.value = pOadInfo->pTab->oad.value;
    
    /* 类型不存在 */
    if (pOadInfo->pTab->oad.value != 0x81070200)
    {
        return DATA_OBJECT_UNAVAILABLE;
    }

    ret = db_read_nomal(hUdp, &nomalOad, sizeof(cfg8107), (uint8*)&cfg8107, &len);
    if ((ret < ERR_OK) || (len != sizeof(cfg8107)))
    {
        PRTL_FMT_DEBUG("获取指定购电参数信息失败 ret[%d]\n", ret);
    }
    else
    {
        PRTL_FMT_LOGCC("获取指定购电参数信息\n");
    }

    cfg8107NumOffset = offset;                                          //数量偏移
    outRecordBuf->recordBuf[offset ++] = 0x00;                          //数量，先填0
    bufAllLen = offset;
    for (i = inReadNor.startInfoNum; i <= inReadNor.endInfoNum; i++)
    {
        //有断点时，找到上次执行到的i
        if (slicing_is_comtinue(relyInfo.bpBreakInfo, i))
        {
            continue;
        }

        for (j = 0; j < cfg8107.nNum; j++)
        {
            if (cfg8107.item[j].object == i)
            {
                break;
            }
        }
        if (j == cfg8107.nNum)
        {
            break;
        }
        PRTL_FMT_LOGCC("找到指定购电参数信息 序号为0x%04X\n", i);

        //转换成报文数据
        convert.srcBuf = &cfg8107.item[j];
        convert.srcLen = sizeof(cfg8107.item[j]);
        convert.sOffset = &sOffset;

        convert.dstBuf = &outRecordBuf->recordBuf[offset];
        convert.dstLen = maxBufLen - offset;
        convert.dOffset = &dOffset;

        //记录断点信息
        breakInfo.index = i;

        nRet = data_to_buf_cfg_810C(pOadInfo, &convert);  //入参
        if (nRet == DATA_SUCCESS)
        {
            bufAllLen += dOffset;
            if (bufAllLen <= maxBufLen)
            {
                memcpy(&outRecordBuf->recordBuf[offset], convert.dstBuf, dOffset);
                offset += dOffset;
                outRecordBuf->bufLen = offset;
                PRTL_BUF_LOGCC(convert.dstBuf, dOffset, "第%d个购电参数信息:", i);
                cfg8107Num ++;
                dOffset = 0;
                sOffset = 0;
            }
            else
            {
                dOffset = 0;
                sOffset = 0;
                isBufFull = TRUE;
                PRTL_FMT_LOGCC("获取购电参数信息长度[%d]超最大长度[%d]\n", bufAllLen, maxBufLen);
            }
        }
        else
        {
            continue;
        }

        if ((isBufFull) && (offset > 0))
        {
            break;
        }
    }

    if (!isBufFull)
    {
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x01)
        {
            breakInfo.appSlicingFlag = 0x01;
            breakInfo.isEnd = 0x01;                           //末尾帧
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;    //序号加1
            breakInfo.objPre = 0x01;                          //对象属性
        }
        else
        {
            /*清除断点信息*/
            memset(&breakInfo, 0, sizeof(BP_INFO_T));
        }
    }
    else
    {
        breakInfo.reqType = 0x03;
        breakInfo.appSlicingFlag = 0x01;
        breakInfo.isEnd = 0x00;                              //不是末尾帧
        breakInfo.objPre = 0x01;                             //对象属性
        if (relyInfo.bpBreakInfo->appSlicingFlag == 0x00)
        {
            breakInfo.seq = BRK_SEQ_START;                   //首次序号
        }
        else
        {
            breakInfo.seq = relyInfo.bpBreakInfo->seq + 1;  //序号加1
        }
    }

    //刷新断点信息
    memcpy(&outRecordBuf->breakInfo, &breakInfo, sizeof(BP_INFO_T));

    PRTL_BUF_LOGCC(outRecordBuf->recordBuf, offset, "整个购电参数信息:");
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->appSlicingFlag = %d\n", outRecordBuf->breakInfo.appSlicingFlag);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->index = %d\n", outRecordBuf->breakInfo.index);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->isEnd = %d\n", outRecordBuf->breakInfo.isEnd);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->objPre = %d\n", outRecordBuf->breakInfo.objPre);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->seq = %d\n", outRecordBuf->breakInfo.seq);
    PRTL_FMT_LOGCC("记下断点信息 bpBreakInfo->reqType = %d\n", outRecordBuf->breakInfo.reqType);

    result = DATA_SUCCESS;
    outRecordBuf->recordBuf[cfg8107NumOffset] = cfg8107Num;   //重新记数量
    outRecordBuf->bufLen = offset;

    return result;
}

#endif

