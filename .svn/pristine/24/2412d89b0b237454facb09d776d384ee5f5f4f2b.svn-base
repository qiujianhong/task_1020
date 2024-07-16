
#ifdef AREA_FUJIAN
#include "CtsAll.h"
#include "fujian_task.h"
#include "fujian_common.h"

#define ERROR_SUCCESS_CODE_ADD  100   //定义一个错误码加的值 以区分验证结果
extern uint8  gPowerOnFlag[AMR_THREAD_NUM];
extern FUJIAN_grop_meter oop_grop_meter;
extern OOP_GATHER_E gGatherMode;
FUJIAN_MULTI_645BUF_T gMulti645Buf[MAX_485_NUM]; //专门给485抄多帧645用
FUJIAN_MULTI_645_RECV_T gMulti645Recv[MAX_485_NUM];//专门给485抄多帧645用 收到的报文

char* gRecordPath = "/data/app/record";
FUJIAN_TASK_CFG_LIST_T gTransTaskCfg; //全局透明任务方案集
FUJIAN_SPECIFIC_TASK_LIST_T gSpecificTaskList; //具体任务集合
FUJIAN_TASK_COLLECTION_STATE_T gTaskCollectionState; //透明任务方案采集状态集

const PLCTASK_FUNINFO gInnerTaskList[] =
{
    {MS_RELAY_TRANS,    5,      RCS_NULL,      0,            fujian_relay_trans_deal},                  //检测透明转发

    {MS_NORMAL_AMR,     11,     RCS_NULL,   1,           taskmng_trans_task_deal},               //698规约任务抄表

    {0xFF,              0xFF,   RCS_NULL,       60,           NULL},
};


/*******************************************************************************
* 函数名称: taskmng_ms_fujian_get_by_mettype
* 函数功能: 根据一组用户类型获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_mettype(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    uint8 MtrType;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        MtrType = MeterFileAll.MeterFile[i].basic.userType;
        for(j=0;j<inMS->mst.size;j++)
        {
            if(MtrType == inMS->mst.type[j])
            {   
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
                break;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组配置序号获取对应的电表MS
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metsn(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    uint16 Sn;
    uint16 MsNum = 0;
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {   
        Sn = MeterFileAll.MeterFile[i].nIndex;
        for(j=0;j<inMS->msi.size;j++)
        {
            if(Sn == inMS->msi.idx[j])
            {   
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
                break;
            }
        }
        pMsInfo->num = MsNum;
    }
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户类型区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_mettype_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint8 start=0;
    uint8 end=0;
    uint16 MsNum = 0;
    uint8 MtrType;
    for(j=0;j<inMS->msrt.size;j++)
    {
        RegionType = inMS->msrt.region[j].nType;
        start = inMS->msrt.region[j].start;
        end = inMS->msrt.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }
        if(start>end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            MtrType = MeterFileAll.MeterFile[i].basic.userType;
            if(MtrType>=start && MtrType<=end)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户地址区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metaddr_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    OOP_TSA_T start;
    OOP_TSA_T end;
    int64         tmpaddr = 0;
    int64         startaddr64 = 0;
    int64         endaddr64 = 0;
    uint16 MsNum = 0;

    for(j=0;j<inMS->msra.size;j++)
    {
        RegionType = inMS->msra.region[j].nType;
        start = inMS->msra.region[j].start;
        end = inMS->msra.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            TsaCalcfun(&end,-1);//对地址进行减1操作
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            TsaCalcfun(&start,1);//对地址进行加1操作
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            TsaCalcfun(&start,1);//对地址进行加1操作
            TsaCalcfun(&end,-1);//对地址进行减1操作
        }

        default:
            break;
        }
        memcpy_r((uint8*)&startaddr64, start.add, start.len+1);
        memcpy_r((uint8*)&endaddr64, end.add, end.len+1);
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            memcpy_r((uint8*)&tmpaddr, MeterFileAll.MeterFile[i].basic.tsa.add, MeterFileAll.MeterFile[i].basic.tsa.len+1);
            if(tmpaddr>=startaddr64 && tmpaddr<=endaddr64)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* 函数功能: 根据一组用户序号区间获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/
uint16 taskmng_ms_fujian_get_by_metsn_region(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16 i,j;
    REGION_TYPE    RegionType;
    uint16 start=0;
    uint16 end=0;
    uint16 MsNum = 0;
    uint16 Sn;
    for(j=0;j<inMS->msri.size;j++)
    {
        RegionType = inMS->msri.region[j].nType;
        start = inMS->msri.region[j].start;
        end = inMS->msri.region[j].end;
        switch(RegionType)
        {
        case FRONTCLOSE_BACKOPEN://前闭后开
        {
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }

        case FRONTOPEN_BACKCLOSE://前开后闭
        {
            start += 1;
            break;
        }

        case FRONTCLOSE_BACKCLOSE://前闭后闭
        {
            break;
        }

        case FRONTOPEN_BACKOPEN://前开后开
        {
            start += 1;
            if(end == 0)
                return 0;
            end -= 1;
            break;
        }
        default:
            break;
        }

        if(start > end)
            return 0;
        for(i=0;i<MeterFileAll.MeterCount;i++)
        {
            Sn = MeterFileAll.MeterFile[i].nIndex;
            if(Sn>=start && Sn<=end)
            {
                pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                MsNum++;
            }
        }
    }
    pMsInfo->num = MsNum;
    return MsNum;
}

/*******************************************************************************
* 函数功能: 去除重复地址
* 输入参数:   tsa          //已存储的地址
              addr               //当前电表地址
* 输出参数: 无
* 返 回 值: true      //重复
            false      //不重复
*******************************************************************************/

BOOL taskmng_addr_is_repet(FUJIAN_ADDR_LIST_T* tsa,uint8* addr,uint16 count)
{
    if(taskmng_addr_isvalid_check(addr,6)<0)
        return TRUE;
    uint16 i ;
    if(count == 0)
        return FALSE;
    for(i=0;i<count;i++)
    {
        if(memcmp(tsa->addr[i].add,addr,6)==0)
            return TRUE;
    }
    return FALSE;
}
/*******************************************************************************
* 函数功能: 根据一组用户地址获取系列用户类型
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //重复
            1      //不重复
*******************************************************************************/

/*同时满足群组地址和标准地址*/
uint16 taskmng_FJ_ms_list_get_by_metaddr(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    int i,j,k=0;
    FUJIAN_ADDR_LIST_T      tmpmsa;   //存储所有符合要求的电表 
    uint16 count = 0;
    memset(&tmpmsa,0x00,sizeof(FUJIAN_ADDR_LIST_T));
    uint8 flag = 0;
    for(i=0;i<inMS->msa.size;i++)
    {
        if(taskmng_grop_addr_isvalid(inMS->msa.add[i].add)>0)                  //检查是否是群组地址
        {
            pthread_rwlock_rdlock(&oop_grop_meter.rwLock);
            for(j=0;j<oop_grop_meter.nNum;j++)
            {

                if(memcmp(inMS->msa.add[i].add,oop_grop_meter.grop_meter[j].groupAddr.add,6)==0)
                {
                    for(k = 0;k<oop_grop_meter.grop_meter[j].num;k++)
                    { 
                        memcpy(tmpmsa.addr[tmpmsa.num].add,oop_grop_meter.grop_meter[j].addr[k].add,6);
                        tmpmsa.addr[tmpmsa.num].len = 5;
                        tmpmsa.num ++;               
                        if(tmpmsa.num>=MAX_SPECIFIC_ADDR_NUM)
                        {
                            TASK_FMT_DEBUG(-1, TASK_LOG, "具体任务获取的档案当前数量%d 超过最大数量 %d\n",tmpmsa.num,MAX_SPECIFIC_ADDR_NUM);
                            flag = 1;
                            break;
                        }
                    }
                }
                if(flag == 1)
                    break;
            }
            pthread_rwlock_unlock(&oop_grop_meter.rwLock);
        }
        else
        {
            memcpy(&tmpmsa.addr[tmpmsa.num],&inMS->msa.add[i],sizeof(OOP_TSA_T));
            tmpmsa.num ++;
        }
        if(tmpmsa.num>=MAX_SPECIFIC_ADDR_NUM)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "具体任务获取的档案当前数量%d 超过最大数量 %d\n",tmpmsa.num,MAX_SPECIFIC_ADDR_NUM);
            break;
        }
    }
    if(tmpmsa.num!=0)
    {
        memset(pMsInfo,0x00,sizeof(FUJIAN_ADDR_LIST_T));
        for(i=0;i<tmpmsa.num;i++)
        {
              /*去除重复和无效档案*/
           if( taskmng_addr_is_repet(pMsInfo,tmpmsa.addr[i].add,count))
           {
               continue;
           }
           memcpy(&pMsInfo->addr[count],&tmpmsa.addr[i],sizeof(OOP_TSA_T));
           count ++;
        }
        pMsInfo->num = count;
    }
    /*展示地址*/
    TASK_FMT_DEBUG(-1, TASK_LOG, "读取到的档案数量%d\n",count);
 /*   for(i=0;i<pMsInfo->num;i++)
    {
       TASK_FMT_DEBUG(-1, TASK_LOG, "当前为第%d个档案\n",i+1);
       TASK_BUF_DEBUG(-1, TASK_LOG,pMsInfo->addr[i].add,6);
    }*/
    return count;
}
uint16 taskmng_FJ_ms_list_get(OOP_MSC_T *inMS, FUJIAN_ADDR_LIST_T *pMsInfo)
{
    uint16          MsNum = 0;
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);

    TASK_FMT_TRACE(-1, TASK_LOG, "taskmng_ms_list_get choice %d \n", inMS->choice);

    switch(inMS->choice)
    {
        //一组用户类型，其他暂时不支持
        case 1:                                 //全部用户地址
            {
                for(i=0;i<MeterFileAll.MeterCount;i++)
                {           
                    pMsInfo->addr[MsNum] = MeterFileAll.MeterFile[i].basic.tsa;
                    MsNum++;
                }
                pMsInfo->num = MsNum;
            }

            break;
        case 2:     //一组用户类型
            MsNum = taskmng_ms_fujian_get_by_mettype(inMS,pMsInfo);
            break;
        case 3:     //一组用户地址
            MsNum = taskmng_FJ_ms_list_get_by_metaddr(inMS,pMsInfo);
            break;
        case 4:     //一组配置序号
            MsNum = taskmng_ms_fujian_get_by_metsn(inMS,pMsInfo);
            break;
        case 5:     //一组用户类型区间
            MsNum =  taskmng_ms_fujian_get_by_mettype_region(inMS,pMsInfo);
            break;
        case 6:     //一组用户地址区间
            MsNum = taskmng_ms_fujian_get_by_metaddr_region(inMS,pMsInfo);
            break;
        case 7:     //一组配置序号区间
            MsNum = taskmng_ms_fujian_get_by_metsn_region(inMS,pMsInfo);
            break;
        default:
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return 0;

    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return MsNum;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_init_with_logicid
* 函数功能: 群组地址档案初始化 
* 输入参数: 
            pMeterFileAll   //电表档案
           logicid 逻辑设备号
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
uint16 taskmng_grop_meter_init(FUJIAN_grop_meter *pgropMeter)
{
    uint16  MeterCount=0;
    uint16   MeterNum=0;     //档案个数
    int   ret;
    uint16 i;
    OOP_GROUPMETER_NEW_T gMeterFile;     //每个电表的档案信息
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "开始群组地址初始化! \n");
    pthread_rwlock_wrlock(&pgropMeter->rwLock);

    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = 0xffff;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.optional = 0;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6e050200;

    
    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(gDBClientMain, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读记录失败 ret  %d recNum %d \n",ret,recNum);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x6E050200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_GROUPMETER_NEW_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读取的记录有问题 \n");
                continue;
            }
            memcpy(&gMeterFile,stepRecordOut.record[0].outDataBuf,sizeof(OOP_GROUPMETER_NEW_T));
            if(taskmng_grop_addr_isvalid(gMeterFile.groupAddr.add)<0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "群组地址非法 \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.groupAddr.add,6);
                continue;
            }
            TASK_FMT_DEBUG(-1,TASK_LOG, "群组地址 0X%02x%02x%02x%02x%02x%02x \n",gMeterFile.groupAddr.add[0],gMeterFile.groupAddr.add[1],gMeterFile.groupAddr.add[2],gMeterFile.groupAddr.add[3],gMeterFile.groupAddr.add[4],gMeterFile.groupAddr.add[5]);
            if(taskmng_grop_addr_repect_check(pgropMeter,&gMeterFile))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "群组地址重复 过滤 \n");
                continue;
            }
            
            MeterNum=pgropMeter->nNum;
            pgropMeter->grop_meter[MeterNum]=gMeterFile;
            pgropMeter->nNum++;
            if(pgropMeter->nNum >= GROUPMETER_NUM)
            break;               //暂时先这样
       }
    }
    db_read_record_end(gDBClientMain, handle);

    MeterCount = pgropMeter->nNum;
    TASK_FMT_DEBUG(-1,TASK_LOG, "加载完成后群组地址个数为 %d \n",MeterCount);
    pgropMeter->FileState = TASK_CFG_INITED;
    pthread_rwlock_unlock(&pgropMeter->rwLock);
    return MeterCount;    
}

/*******************************************************************************
* 函数名称: taskmng_addr_isvalid_check
* 函数功能: 检测是否有效的群组地址
* 输入参数: MtrAddr             要检测的测量点地址
*           
* 输出参数: 无
* 返 回 值:  >0                  有效
*           <0                  无效
*******************************************************************************/
int taskmng_grop_addr_isvalid(uint8 *MtrAddr)
{
    uint8   TestAddr[6];

    //判断群组地址是否是4个字节0x88
    memset(TestAddr, 0x88, 4);
    if (memcmp(MtrAddr, TestAddr, 4) == 0)
    {
        return 1;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_addr_check
* 函数功能: 判断新的群组地址是否和现有的重复
* 输入参数: 
            pMeterFileAll   //电表档案
           meterFile  待检查的档案
* 输出参数: 无
* 返 回 值:   TRUE 有重复的 FALSE 没有重复的
*******************************************************************************/
BOOL taskmng_grop_addr_repect_check(FUJIAN_grop_meter *pMeterFileAll,OOP_GROUPMETER_NEW_T *meterFile)
{
    uint16 i;
    BOOL ret = FALSE;
    for(i=0;i<pMeterFileAll->nNum;i++)
    {
        if(memcmp(meterFile->groupAddr.add,pMeterFileAll->grop_meter[i].groupAddr.add,6) == 0)
        {
            ret = TRUE;
            break;
        }
    }
    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_645_meter_data_unpack
* 函数功能: 电表规约解帧
* 输入参数: FrameLen            帧长度
*           FrameBuff           帧数据
* 输出参数: 
*           DataLen             数据域长度
*           DataBuff            数据域内容
* 返 回 值: CHECK_RESULT_SUCCEED
*           CHECK_RESULT_FAILUER
*           CHECK_RESULT_CONFIRM
*           CHECK_RESULT_DENY
*******************************************************************************/
/*int8 taskmng_645_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff,uint16 *DataLen, uint8 *DataBuff)
{
    uint16  DataIndex = 0xff;                   //指针偏移
    uint8   CS = 0;
    uint16   i;//start Common Problem

    //滑动偏移，定位帧头
    for (i = 0; i < FrameLen; i++)
    {
        if ((FrameBuff[i] == 0x68) && (FrameBuff[i+7] == 0x68))
        {
            DataIndex = i;
            break;
        }
    }

    //未定位到帧头，判为失败
    if (DataIndex == 0xff)
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新数据长度
    *DataLen = FrameBuff[DataIndex+9];

    //判断帧尾
    if (FrameBuff[(DataIndex + *DataLen + 11)] != 0x16)
    {
        return CHECK_RESULT_FAILUER;
    }

    //判断校验和
    for (i=0; i < (*DataLen + 10); i++)
    {
        CS += FrameBuff[DataIndex+i];
    }
    if (CS != FrameBuff[(DataIndex + *DataLen + 10)])
    {
        return CHECK_RESULT_FAILUER;
    }

    //刷新数据域内容，同时做余3码处理
    for (i = 0; i < *DataLen; i++)
    {
        DataBuff[i] = FrameBuff[DataIndex+10+i] - 0x33;
    }

    //判断是否是确认帧
    if (*DataLen == 0)
    {
        return CHECK_RESULT_CONFIRM;
    }
    return CHECK_RESULT_SUCCEED;
}
*/

/*******************************************************************************
* 函数名称: judge_buf_inter
* 函数功能: 比较当前报文是否在范围中
* 输入参数: 
            sectionless   范围的左值
            sectionMax    范围的右值
            buf          匹配到特征字节后加开始字节的报文
            codelen      报文截取长度
* 输出参数: 无
* 返 回 值:   TRUE 有重复的 FALSE 没有重复的
*******************************************************************************/

BOOL judge_buf_inter(uint8 pipe,OOP_OCTETVAR256_T *sectionless,OOP_OCTETVAR256_T *sectionMax,uint8 *buf,uint32 codelen)
{
    int ret1,ret2;
    BOOL backtype = FALSE;
    ret1 = memcmp(buf,sectionless->value,codelen);
    ret2 = memcmp(buf,sectionMax->value,codelen);
    if(ret1 >=0 && ret2 <=0)
    {
        backtype = TRUE;
    }
    else
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "报文验证失败\n");
        TASK_FMT_DEBUG(pipe,REC_LOG, "当前报文\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, buf,codelen);
        TASK_FMT_DEBUG(pipe,REC_LOG, "验证左值\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, sectionless->value,codelen);
        TASK_FMT_DEBUG(pipe,REC_LOG, "验证右值\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, sectionMax->value,codelen);
    }
    return backtype;
}
/*******************************************************************************
* 函数名称: normal_data_type
* 函数功能: 通用数据(正序数据排序即高字节在前)
* 输入参数: 
            datacomp   数据比对参数
            buf        本次报文
            lastbuf    上次报文
            thisindex  本次报文偏移
            lastindex  上一次报文偏移
            add        当前电表地址
* 输出参数: 无
* 返 回 值:   TRUE 验证成功 FALSE 验证失败
*******************************************************************************/

BOOL normal_data_type(uint8 pipe,OOP_DATACOMP_PARAM_T *datacomp,uint8* buf,uint8*lastbuf,uint16 thisindex,uint16 lastindex,OOP_TSA_T *add)
{

    BOOL ret = FALSE;
    OOP_OCTETVAR256_T sectionMax,sectionless;    
    uint8 i,reportnum=0,successnum=0;
    OOP_DATACOMP_SECTION_T tmpverific;
    //uint8 tmpbuf[datacomp->len] = {0};
    for(i=0;i<datacomp->num;i++)
    {
        tmpverific = datacomp->param[i];
        if(tmpverific.section1.nNum != tmpverific.section2.nNum)
            continue;
        if(tmpverific.section1.nNum != datacomp->len)
            continue;
        if(datacomp->datatype==0 || datacomp->datatype== 20 )
        {
            if(memcmp(tmpverific.section1.value,tmpverific.section2.value,datacomp->len)>= 0)  
            {
                sectionMax=tmpverific.section1;
                sectionless = tmpverific.section2;
            }
            else
            {
                sectionMax=tmpverific.section2;
                sectionless = tmpverific.section1;
            }
        }
        else
        {
            OOP_OCTETVAR256_T section1,section2;
            section1.nNum = tmpverific.section1.nNum;
            section2.nNum = tmpverific.section2.nNum;
            memcpy_r(section1.value,tmpverific.section1.value,tmpverific.section1.nNum);
            memcpy_r(section2.value,tmpverific.section2.value,tmpverific.section2.nNum);
            
            if(memcmp(section1.value,section2.value,datacomp->len)>= 0)  
            {
                sectionMax=section1;
                sectionless = section2;
            }
            else                
            {
                sectionMax=section2;
                sectionless = section1;
            }
        }
        /*比较当前报文是否在范围中*/
        ret = judge_buf_inter(pipe,&sectionless,&sectionMax,&buf[thisindex],datacomp->len);
        TASK_FMT_DEBUG(pipe,REC_LOG, "区间index %d 本次响应报文验证返回值 ret %d\n",i,ret);
        //只有获取到了上次的报文才判断
        if(ret && lastindex >0)
        {
            successnum++;
            if(FALSE == judge_buf_inter(pipe,&sectionless,&sectionMax,&lastbuf[lastindex],datacomp->len))
            {
                if(tmpverific.taskid!=0)
                {
                    reportnum++;
                    taskmng_task_trigger(pipe,tmpverific.taskid,add);
                }
            }
        }
    
    }

    TASK_FMT_DEBUG(pipe,REC_LOG, "验证区间数%d,验证成功数%d,触发条件上报数%d\n",datacomp->num,successnum,reportnum);
    return ret;
}

/*******************************************************************************
* 函数名称: normal_data_type
* 函数功能: 通用bit位判断截取长度固定为1，对应位与区间1配置进行异或为真则验证通过
* 输入参数: 
            datacomp   数据比对参数
            buf        本次报文
            lastbuf    上次报文
            thisindex  本次报文偏移
            lastindex  上一次报文偏移
            add        当前电表地址
* 输出参数: 无
* 返 回 值:   TRUE 验证成功 FALSE 验证失败
*******************************************************************************/

BOOL noramal_bit_type(uint8 pipe,OOP_DATACOMP_PARAM_T *datacomp,uint8* buf,uint8*lastbuf,uint16 thisindex,uint16 lastindex,OOP_TSA_T *add)
{
    uint8 i,reportnum=0,successnum=0;
    BOOL ret = FALSE;
    OOP_DATACOMP_SECTION_T tmpverific;
    if(datacomp->len !=1)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "normal_bit_type 截取长度不是1  \n");
        return ret;    
    }
    //num+=1;             /*固定为1*/
    for(i=0;i<datacomp->num;i++)
    {
        tmpverific = datacomp->param[i];
        if(datacomp->len != tmpverific.section1.nNum && datacomp->len != tmpverific.section1.nNum)
        {
            continue;
        }
        if((buf[thisindex]^tmpverific.section1.value[0])==1)
        {
            ret = TRUE;
            successnum++;
            if(lastindex>0 && (lastbuf[lastindex]^tmpverific.section1.value[0])==0)
            {                
                if(tmpverific.taskid!=0)
                {
                    reportnum++;
                    taskmng_task_trigger(pipe,tmpverific.taskid,add);
                }
            }
        }
    }

    TASK_FMT_DEBUG(pipe,REC_LOG, "验证类型%d验证区间数%d,验证成功数%d,触发条件上报数%d\n",datacomp->datatype,datacomp->num,successnum,reportnum);

    return ret;
}



/*******************************************************************************
* 函数名称: abnormal_data_type_645
* 函数功能: DL/T645（均按无符号数处理）
* 输入参数: 
            verific   数据比对参数
            buf        本次报文
            buflen     本次报文长度
            lastbuf    上次报文
            lastbuflen 上一次报文长度
            pcurrruntask  当前任务
            add        当前电表地址
* 输出参数: 无
* 返 回 值:   TRUE 验证成功 FALSE 验证失败
*******************************************************************************/

BOOL abnormal_data_type_645(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,uint8*lastbuf,uint16 lastbuflen,FUJIAN_SPECIFIC_TASK_T *pcurrruntask,OOP_TSA_T *add)
{
    /*不解析报文，数据域-0x33*/
    //uint8 i;
    uint8 apdu[1000];
    uint8 lastapdu[1000];
    uint16 apdulen,lastapdulen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *datacomp = &verific->dataCompare;
    //uint16 tmplen = datacomp->feature.nNum;
    if(datacomp->len == 0)
        return FALSE;
    if(datacomp->datatype==11)
    {
       if(datacomp->len != 1)
        return FALSE;
    }
    apdulen = respons_645_ctrl(pipe,pcurrruntask->resMessType,buf,buflen,apdu);
    lastapdulen = respons_645_ctrl(pipe,pcurrruntask->resMessType,lastbuf,lastbuflen,lastapdu);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "本次响应报文减0x33之后的值\n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "上次响应报文减0x33之后的值\n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, lastapdu,lastapdulen);
    if(apdulen ==0 )
    {
        return FALSE;
    }
/*判断645报文，先减0x33，然后再匹配*/
    lastindex = taskmng_data_intercept(lastapdu,lastapdulen,verific);
    thisindex = taskmng_data_intercept(apdu,apdulen,verific);
    if(thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "未找到特征字节\n");
        TASK_FMT_DEBUG(pipe,REC_LOG, "本次报文特征字 index %d 上次报文特征字index%d\n",thisindex,lastindex);
        return FALSE;
    }
    if(datacomp->datatype==10)
    {
        return  normal_data_type(pipe,datacomp,apdu,lastapdu,thisindex,lastindex,add);
    }
    else 
    {
        return noramal_bit_type(pipe,datacomp,lastapdu,lastapdu,thisindex,lastindex,add);
    }
    return FALSE;
}


uint16 respons_645_ctrl(uint8 pipe,uint8 resMessType,uint8 *buf,uint16 buflen,uint8 *outbuf)
{
    uint16 index,apdulen = 0,i;
    if(buflen ==0 )
        return 0;
    if(resMessType == 0x10)  /*完整帧*/
        index = 1+6+1+1;
    else
        index = 1;     
    apdulen = buf[index];
    memcpy(outbuf,&buf[index+1],apdulen);
    for(i=0;i<apdulen;i++)
    {
        outbuf[i] -=0x33;
    }
    return apdulen;
}

uint16 respons_698_ctrl(uint8 pipe,LinkLayerCtrl_t ctrl,uint8 *buf,uint16 buflen,uint8 resMessType,uint8 *outbuf)
{
    FRAME_CONVERT_T convert;
    uint8 apdu[1000];
    uint16 apdulen;
    uint16 i=0 ;
    if(buflen == 0)
        return 0;
    TASK_FMT_DEBUG(pipe,REC_LOG, "返回校验类型 %d\n",resMessType);
    if(resMessType==0x50 || resMessType==0x53)
    {
        memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
        convert.srcBuf = buf;
        convert.srcLen = buflen;
        convert.dstBuf = apdu;
        Convert_Response_Frame(pipe,&convert,0x51);
        apdulen = convert.dstLen;
        ctrl = convert.ctrl;
    }
    else
    {
        memcpy(apdu,buf,buflen);
        apdulen = buflen;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "控制码%d 扰码标志 %d\n",ctrl.ctrl,ctrl.ctrl_discode);
    if(ctrl.ctrl_discode==1)
    {
        for(i=0;i<apdulen;i++)
        {
            apdu[i]-=0x33;
        }
    }
    memcpy(outbuf,apdu,apdulen);
    return apdulen;
}

/*******************************************************************************
* 函数名称: abnormal_data_type_698
* 函数功能: DL/T698（均按无符号数处理）
* 输入参数: 
            verific   数据比对参数
            buf        本次报文
            buflen     本次报文长度
            lastbuf    上次报文
            lastbuflen 上一次报文长度
            pcurrruntask  当前任务
            add        当前电表地址
* 输出参数: 无
* 返 回 值:   TRUE 验证成功 FALSE 验证失败
*******************************************************************************/

BOOL abnormal_data_type_698(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,uint8*lastbuf,uint16 lastbuflen,FUJIAN_SPECIFIC_TASK_T *pcurrruntask,OOP_TSA_T *add,LinkLayerCtrl_t ctrl)
{
    uint8 apdu[1000];
    uint8 lastapdu[1000];
    uint16 apdulen;
    uint16 lastapdulen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *datacomp = &verific->dataCompare;
    //uint16 tmplen = datacomp->feature.nNum;
    if(datacomp->len == 0)
        return FALSE;
    if(datacomp->datatype==21)
    {
       if(datacomp->len != 1)
           return FALSE;
    }
    /*检查698帧格式，并存储控制码和apdu*/
  
    apdulen = respons_698_ctrl(pipe,ctrl,buf,buflen,pcurrruntask->resMessType,apdu);
    lastapdulen = respons_698_ctrl(pipe,ctrl,lastbuf,lastbuflen,pcurrruntask->resMessType,lastapdu);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "apdu报文 \n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
//    TASK_FMT_DEBUG(pipe,REC_LOG, "lastapdu报文 \n");
//    TASK_BUF_DEBUG(pipe,REC_LOG, lastapdu,lastapdulen);
    if(apdulen ==0)
    {
        return FALSE;
    }
    lastindex = taskmng_data_intercept(lastapdu,lastapdulen,verific);
    thisindex = taskmng_data_intercept(apdu,apdulen,verific);
    
    if(thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "未匹配到特征字节\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, verific->dataCompare.feature.value,verific->dataCompare.feature.nNum);
        TASK_FMT_DEBUG(pipe,REC_LOG, "本次报文\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, apdu,apdulen);
        return FALSE;
    }
    if(datacomp->datatype == 20)
    {
        return  normal_data_type(pipe,&verific->dataCompare,apdu,lastapdu,thisindex,lastindex,add);
    }
    else
    {
         return  noramal_bit_type(pipe,&verific->dataCompare,apdu,lastapdu,thisindex,lastindex,add);
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称: get_last_buf
* 函数功能: 获取上一条报文 
* 输入参数: 
           pAddTaskRunInfo   //任务指针
           nodeid               具体任务号
           add                  //表地址
* 输出参数: lastbuf          //返回上一条采集成功的报文
* 返 回 值:   0     获取上一条报文,报文标记是错误的
              1     获取上一条报文成功，报文错误类型是成功
              2     没有上一条报文(第一次采集成功，直接上报)
              3     没读取到数据中心数据
*******************************************************************************/
uint8 get_last_buf(uint8 pipe,uint8* lastbuf,uint16 *lastbuflen,FUJIAN_TASK_RUN_T * pAddTaskRunInfo,uint16 nodeid,OOP_TSA_T *add)
{
    int   ret;
    uint8 tmpbuf[1000]={0};
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_TRANS_TASK_RESULT_T tmpcode;
    //OOP_DATETIME_S_T tmpstorageTime = pAddTaskRunInfo->storageTime;
//    time_t tmpTime;
//    OOP_DATETIME_S_T dateTime;
//    DT_DateTimeShort2Time(&pAddTaskRunInfo->storageTime,&tmpTime);
//    
//    //获取上一个周期的本地址表的报文
//    tmpTime=tmpTime-(time_t)DT_TI2Seconds(pAddTaskRunInfo->transTask->cycle);
//    if(tmpTime<0)
//    {
//        TASK_FMT_DEBUG(pipe,REC_LOG,"tmpTime error\n");
//    }
//    DT_Time2DateTimeShort(tmpTime,&dateTime);
//    TASK_FMT_DEBUG(pipe,REC_LOG,"取上一次报文的存储时标\n");
//    TASK_FMT_DEBUG(pipe,REC_LOG,"%d\n",tmpTime);
//    fujian_datetime_s_log_write(1, REC_LOG, &dateTime);
//    tmpTime+= (8*60*60);
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&stepReadRecord,0,sizeof(STEP_READ_RECORD_T));
    memset(&tmpcode, 0, sizeof(OOP_TRANS_TASK_RESULT_T));

    inReadRecord.recordOAD.infoNum = pAddTaskRunInfo->taskid;
    inReadRecord.recordOAD.classtag = 5;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E150200;
    inReadRecord.recordOAD.road.oadCols.nNum = 1;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = nodeid;
    
    inReadRecord.cType = COL_TM_STORE;
    inReadRecord.cStart = 0;
    inReadRecord.cEnd = time(NULL);
#if IS_NEED_ADD_8HOUR
    inReadRecord.cEnd+=(8*60*60);

#endif
    inReadRecord.sortType = 1;

    inReadRecord.bTable = 1;
    inReadRecord.table.nNum =  6;
    memcpy(inReadRecord.table.value,add->add,6);
    stepReadRecord.MAC.nNum = 6;
    memcpy(stepReadRecord.MAC.value,add->add,6);

    ret = db_read_record_start(gAmrThreadArg[pipe].DBClientHandle, &inReadRecord, &handle, &recNum);
    if(ret!=0 )
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读上一次存储报文失败 ret  %d recNum %d \n",ret,recNum);
        db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
        return 3;
    }
    else
    {
        if(recNum==0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "读上一次存储报文个数recNum %d \n",ret,recNum);
            db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
            return 2;
        }
        
        ret = db_read_record_step(gAmrThreadArg[pipe].DBClientHandle, handle, &stepReadRecord, &stepRecordOut);
        if(ret!=0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "单步读上一次存储报文失败 ret  %d \n",ret);
            db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
            return 3;
        }
        
   /*     if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=nodeid
            ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_TRANS_TASK_RESULT_T))
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "单步读上一次存储报文有问题 \n");
            db_read_record_end(gDBClientMain, handle);
            return 3;
        }*/
        memcpy(&tmpcode,stepRecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RESULT_T));

        memcpy(tmpbuf,tmpcode.transMsg.resultData.value,tmpcode.transMsg.resultData.nNum);

    }
    db_read_record_end(gAmrThreadArg[pipe].DBClientHandle, handle);
    *lastbuflen = tmpcode.transMsg.resultData.nNum;
    memcpy(lastbuf,tmpbuf,*lastbuflen);
    
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_data_intercept
* 函数功能: 按照报文对比参数到buf中获取截取字段 
* 输入参数: buf   要对比的报文
           buflen 报文长度
           messCompare 报文对比参数
* 输出参数: 
* 返 回 值:  0 没匹配到
            正常返回buf中的索引
*******************************************************************************/
uint16 taskmng_data_intercept(uint8 *buf,uint16 buflen,OOP_RECV_MESS_PROC_T * verific)
{
    uint16 i;
    if(buflen==0)
        return 0;
    //只对比到特征字节长度
    switch (verific->type)
        {
            case 1:
            case 2:
            {
                if(buflen<=verific->messCompare.feature.nNum)
                break;
                for(i=0;i<buflen-verific->messCompare.feature.nNum+1;i++)
                {
                    if(memcmp(buf+i,verific->messCompare.feature.value,verific->messCompare.feature.nNum) == 0)
                    {
                        return i+verific->messCompare.feature.nNum+verific->messCompare.start-1;
                    }   
                }
            }break;
            case 3:
            {
                if(buflen<=verific->dataCompare.feature.nNum)
                break;
                for(i=0;i<buflen-verific->dataCompare.feature.nNum+1;i++)
                {
                    if(memcmp(buf+i,verific->dataCompare.feature.value,verific->dataCompare.feature.nNum) == 0)
                    {
                        return i+verific->dataCompare.feature.nNum+verific->dataCompare.start-1;
                    }   
                }
            }break;
        }
    return 0;
}

/*******************************************************************************
* 函数名称: verific_type1
* 函数功能: 报文比对参数 
* 输入参数: 
           verific 返回报文验证方式
           buf   要对比的报文
           buflen 报文长度
           pAddTaskRunInfo 任务方案参数
           nodeid          当前具体任务号
           add             tsa 电表地址

* 输出参数: 
* 返 回 值:  
            true : 验证成功
            FALSE： 验证失败
*******************************************************************************/

uint8 verific_type1(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,FUJIAN_TASK_RUN_T * pAddTaskRunInfo,uint16 nodeid, OOP_TSA_T *add)
{
    uint8 ret = 0;
    uint8 lastbuf[1000] = {0};
    uint16 lastbuflen;
    uint16 lastindex,thisindex;
    /*获取上一次采集成功的报文，开始读为0，说明本次为第一次采集，直接上报 return 1*/
    ret = get_last_buf(pipe,lastbuf,&lastbuflen,pAddTaskRunInfo,nodeid,add);
    if(ret == 3)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "读上一条报文失败\n");
        return ret;
    }
    else if(ret == 2)
    {
        //认为该条为第一条 默认正确
        return ret;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "该表上一次抄读报文\n");
    TASK_BUF_DEBUG(pipe,REC_LOG, lastbuf,lastbuflen);
    //TASK_FMT_DEBUG(pipe,REC_LOG, "该表本次抄读报文\n");
    //TASK_BUF_DEBUG(pipe,REC_LOG, buf,buflen);
    lastindex = taskmng_data_intercept(lastbuf,lastbuflen,verific);
    thisindex = taskmng_data_intercept(buf,buflen,verific);
    //没找到特征字节可能是上一轮抄失败了 这一轮认为首次抄 都上报
    if(lastindex==0 || thisindex == 0)
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "未找到特征字节\n");
        return 2;
    }

    if(memcmp(&buf[thisindex],&lastbuf[lastindex],verific->messCompare.len) == 0)
    {
        return 1;
    }
    else
    {
        TASK_FMT_DEBUG(pipe,REC_LOG, "报文验证不通过,当前报文截取的字节\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, &buf[thisindex],verific->messCompare.len);
        TASK_FMT_DEBUG(pipe,REC_LOG, "上一次报文截取的字节\n");
        TASK_BUF_DEBUG(pipe,REC_LOG, &lastbuf[lastindex],verific->messCompare.len);
        return 0;
    }

}
/*******************************************************************************
* 函数名称: verific_type3
* 函数功能: 数据判断参数 
* 输入参数: 
           verific 返回报文验证方式
           buf   要对比的报文
           buflen 报文长度
           pAddTaskRunInfo 任务方案参数
           currruntask     当前具体任务
           add             tsa 电表地址
           ctrl            控制码
* 输出参数: 
* 返 回 值:  
            true : 验证成功
            FALSE： 验证失败
*******************************************************************************/

BOOL verific_type3(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,uint8* buf,uint16 buflen,FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_SPECIFIC_TASK_T * currruntask, OOP_TSA_T *add,LinkLayerCtrl_t ctrl)
{
    uint8 ret;
    uint8 lastbuf[1000]={0};
    uint16 lastbuflen = 0;
    //uint32 tmplen;
    uint16 lastindex,thisindex;
    OOP_DATACOMP_PARAM_T *tmpdataparam = &verific->dataCompare;
    //tmplen = tmpdataparam->feature.nNum;
    if(buflen==0)
    {
        return FALSE;
    }
    ret = get_last_buf(pipe,lastbuf,&lastbuflen,pAddTaskRunInfo,currruntask->nodeid,add);

    if(ret == 3)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "上一次报文获取失败\n");
//        return FALSE;
    }
    else if(ret == 2)
    {
        //认为该条为第一条 默认正确
        TASK_FMT_DEBUG(pipe,REC_LOG, "当前为第一条数据\n");
//        if(verific->messCompare.taskid !=0)   //关联上报方案号才上报
//        {
//            taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
//        }
//        return TRUE;
    }
    TASK_FMT_DEBUG(pipe,REC_LOG, "上一次报文获取成功\n");
    TASK_BUF_DEBUG(pipe,REC_LOG, lastbuf,lastbuflen);
    if(tmpdataparam->datatype<10)              /*不需要对报文进行处理*/
    {
        lastindex = taskmng_data_intercept(lastbuf,lastbuflen,verific);
        thisindex = taskmng_data_intercept(buf,buflen,verific);
        
        if(thisindex == 0)
        {
            TASK_FMT_DEBUG(pipe,REC_LOG, "未找到特征字节\n");
            return 3;
        }

    }
    
    switch (tmpdataparam->datatype)
        {
            case 0:
            {   
                normal_data_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }   
                break;
            case 1:
            {
                normal_data_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }
                break;
            case 2:
            {
                noramal_bit_type(pipe,tmpdataparam,buf,lastbuf,thisindex,lastindex,add);
            }
                break;
            case 10:
            case 11:
            {
                abnormal_data_type_645(pipe,verific,buf,buflen,lastbuf,lastbuflen,currruntask,add);
            } break;            
            case 20:
            case 21:
            {
                abnormal_data_type_698(pipe,verific,buf,buflen,lastbuf,lastbuflen,currruntask,add,ctrl);
            }break;
                
            default:
                return FALSE;
                break;
        }
    return TRUE;
}

/*******************************************************************************
* 函数功能: 返回报文验证
* 输入参数:           inMS          //从数据中心获取的电表信息
            pMsInfo                 //抄读电表信息
* 输出参数: 无
* 返 回 值: 0      //验证不通过
            1      //验证通过
            2      //首次获得报文
*******************************************************************************/

uint8 packet_verific_Ispass(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,FRAME_CONVERT_T *convert,FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_SPECIFIC_TASK_T *currruntask, OOP_TSA_T *add)
{
    uint8 *buf = convert->dstBuf;
    uint16 buflen = convert->dstLen;
    LinkLayerCtrl_t ctrl = convert->ctrl;
    if(buflen==0)
        return FALSE;
    if(verific->type<3 || verific->type == 4)
        TASK_FMT_DEBUG(pipe,REC_LOG, "报文验证方式%d \n",verific->type);
    else
        TASK_FMT_DEBUG(pipe,REC_LOG, "报文验证方式%d,数据验证格式 %d \n",verific->type,verific->dataCompare.datatype);    
    uint8 ret;
    switch (verific->type)
        {
            case 0:
            {
                return 1;
            }
                break;
            case 1:
            {
                ret = verific_type1(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask->nodeid,add);
                if(ret == 0)
                {
                     if(verific->messCompare.taskid !=0)   //关联上报方案号才上报
                     {
                        taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
                     }
                
                     return 1;
                }
                else if(ret == 2 )
                {
                    return ret;                          //首次不触发任务,但是验证结果为正确
                }
                    
            }
                break;
            case 2:
            {
                ret = verific_type1(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask->nodeid,add);
                if(ret==1)
                {
                     if(verific->messCompare.taskid !=0)   //关联上报方案号才上报
                     {
                        taskmng_task_trigger(pipe,verific->messCompare.taskid,add);
                     }
                
                     return 1;
                }
                else if(ret == 2)
                {
                    return ret;                               // 首次不触发任务
                }
            }
                break;
            case 3:                           /*所有数据区间内验证通过都上报*/
            {
                verific_type3(pipe,verific,buf,buflen,pAddTaskRunInfo,currruntask,add,ctrl);
                return 1;
            }
                break;
            case 4:
                return 0;
                break;
            default :
                return 0;
                break;
        }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_cfg_init
* 函数功能: 任务结构体初始化
* 输入参数: 
            
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
void taskmng_trans_task_cfg_init()
{
    memset(&gTransTaskCfg, 0x00, sizeof(FUJIAN_TASK_CFG_LIST_T));
    pthread_mutex_init(&gTransTaskCfg.Lock, NULL);
    gTransTaskCfg.TaskCfgState = TASK_CFG_UNINIT;

    memset(&gSpecificTaskList, 0X00, sizeof(FUJIAN_SPECIFIC_TASK_LIST_T));

    memset(&gTaskCollectionState, 0x00, sizeof(FUJIAN_TASK_COLLECTION_STATE_T));
    pthread_mutex_init(&gTaskCollectionState.lock, NULL);
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_init
* 函数功能: 从数据中心加载全局透明任务方案
* 输入参数: 
            
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
void taskmng_trans_task_init()
{
    int             ret;
    uint32          handle = 0;
    uint32          recNum = 0;
    uint16          i;
    uint16          taskNum = 0;
    READ_RECORD_T   inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T   stepRecordOut;
    OOP_TRANS_TASK_NODE_T specificTask;

    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 65535;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E110200;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum == 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "开始读任务失败 ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        for(i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "单步读任务失败 ret %d i %d \n", ret, i);
                continue;
            }

            if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadCols.oad[0].value != 0x6E110200 ||
                 stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_T))
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "单步读取的任务记录有问题 \n");
                continue;
            }

            memcpy(&gTransTaskCfg.TaskCfg[taskNum], stepRecordOut.record[0].outDataBuf, 
                sizeof(OOP_TRANS_TASK_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "读取到全局透明任务方案 %d 端口 %08x 任务类别 %d 优先级 %d\n",
                 
                gTransTaskCfg.TaskCfg[taskNum].taskid, 
                gTransTaskCfg.TaskCfg[taskNum].oad.value, gTransTaskCfg.TaskCfg[taskNum].taskTypeData.taskType, 
                gTransTaskCfg.TaskCfg[taskNum].priority);
            taskNum++;

            if(taskNum >= MAX_TRANS_TASK_NUM)
                break;
        }
    }

    gTransTaskCfg.TaskNum = taskNum;
    TASK_FMT_DEBUG(-1, TASK_LOG, "全局透明任务方案个数 %d \n", taskNum);
    db_read_record_end(gDBClientTask, handle);

    //加载所有具体任务F102  
    /*  信息点号 透明任务方案号
        次oad为：具体任务编号
        主oad 0x6E130200
        数据类标识： 2（可初始化参数)
        读取用扩展f012读取，表地址填0
    */
    taskNum = 0;
    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    memset(&specificTask, 0x00, sizeof(OOP_TRANS_TASK_NODE_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = 65535;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E130200;
    //读所有 直接填0个次oad
    inReadRecord.recordOAD.road.oadCols.nNum = 0;
    //inReadRecord.recordOAD.road.oadCols.oad[0].value = 0;
    inReadRecord.recordOAD.classtag = CLASS_DATA_INIT;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum == 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "开始读具体任务失败 ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        for(i = 0; i < recNum; i++)
        {
            ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "单步读具体任务失败 ret %d i %d \n", ret, i);
                continue;
            }

            if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadMain.value != 0x6E130200 ||
                 stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T))
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "单步读取的具体任务记录有问题 \n");
                continue;
            }

            memcpy(&specificTask, stepRecordOut.record[0].outDataBuf, sizeof(OOP_TRANS_TASK_NODE_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "读取到具体任务 任务方案号 %d 具体任务序号 %d \n", 
                specificTask.transTaskid, specificTask.nodeid);
            gSpecificTaskList.taskList[taskNum].transTaskid = specificTask.transTaskid;
            gSpecificTaskList.taskList[taskNum].nodeid = specificTask.nodeid;

            //获取出具体的通信地址集合 最多255个
            gSpecificTaskList.taskList[taskNum].addrAll.num = taskmng_FJ_ms_list_get(&specificTask.ms,&gSpecificTaskList.taskList[taskNum].addrAll);
            gSpecificTaskList.taskList[taskNum].uartParam = specificTask.uartParam;
            gSpecificTaskList.taskList[taskNum].reqMessType = specificTask.reqMessType;
            gSpecificTaskList.taskList[taskNum].reqMess = specificTask.reqMess;
            gSpecificTaskList.taskList[taskNum].resMessType = specificTask.resMessType;
            gSpecificTaskList.taskList[taskNum].resMaxLen = specificTask.resMaxLen;
            gSpecificTaskList.taskList[taskNum].resMinLen = specificTask.resMinLen;
            gSpecificTaskList.taskList[taskNum].resMessProc = specificTask.resMessProc;
            gSpecificTaskList.taskList[taskNum].next = NULL; //各自的线程初始化的时候给赋值
            taskNum++;

            if(taskNum >= MAX_SPECIFIC_TASK_NUM)
                break;
        }
    }

    gSpecificTaskList.taskNum = taskNum;
    TASK_FMT_DEBUG(-1, TASK_LOG, "具体任务个数 %d \n", taskNum);
    db_read_record_end(gDBClientTask, handle);
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_db_update_check
* 函数功能: 检查是否收到任务相关的变更事件
* 输入参数: 

* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
void taskmng_trans_task_db_update_check(DB_UPDATE_T * dbupdate)
{
    int             Recvlen = 0;

    Recvlen = taskmng_db_update_queue_read(&pTaskDBUpdate, dbupdate);

    if(Recvlen > 0)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "收到mqtt发送的更新消息 logicid %d  infonum %d oad %08x !\n", 
            dbupdate->logicid, dbupdate->infonum, dbupdate->oad);
        gTransTaskCfg.TaskCfgState = TASK_CFG_MODIFY;
    }
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_collection_state_load
* 函数功能://从数据中心加载透明任务方案采集状态集
* 输入参数:

* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_collection_state_load()
{
    uint16          i;
    uint16          TaskId;
    int             ret;
    OOP_TRANS_TASK_RLUESTATE_T data;

    pthread_mutex_lock(&gTaskCollectionState.lock);

    gTaskCollectionState.taskNum = gTransTaskCfg.TaskNum;
    memset(gTaskCollectionState.taskState,0x00,sizeof(gTaskCollectionState.taskState));
    TASK_FMT_DEBUG(-1, TASK_LOG, "加载透明任务方案采集状态集 总任务个数 %d \n", 
        gTaskCollectionState.taskNum);
    
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E200200;

    for(i = 0; i < gTransTaskCfg.TaskNum; i++)
    {
        TaskId = gTransTaskCfg.TaskCfg[i].taskid;
        //读最近一条记录
        ReadRecord.recordOAD.infoNum = TaskId;
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = 0;
        ReadRecord.cEnd = time(NULL);
        ReadRecord.sortType = DOWN_SORT;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RLUESTATE_T))
        {
            memcpy(&data,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            TASK_FMT_TRACE(-1, TASK_LOG, "获取到采集状态集 taskid %d send %d recv %d \n",TaskId,data.sendNum,data.receNum);
            memcpy(&gTaskCollectionState.taskState[i], &data, sizeof(OOP_TRANS_TASK_RLUESTATE_T));

        }else
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "读取任务 %d 采集状态失败 ret %d \n", TaskId, ret);
            gTaskCollectionState.taskState[i].transTaskid = TaskId;
        }

    }

    pthread_mutex_unlock(&gTaskCollectionState.lock);
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_changeflag_set
* 函数功能: 收到任务变更后 针对任务号和端口 将对应线程的对应任务置位 供抄表线程调
    度时用
* 输入参数:taskid 任务方案号
            port 任务端口
            flag 要置位的标记 2表示修改 3表示删除
* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_changeflag_set(uint16 taskid, OOP_OAD_U port, uint8 flag)
{
    uint8           pipe = taskmng_mtr_pipe_get(&port);

    
    if(pipe > AMR_THREAD_NUM)
        return;

    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    uint16          i;

    taskRunList = &gAmrThreadArg[pipe].taskRunList;

    for(i = 0; i < taskRunList->taskNum; i++)
    {
        if(taskRunList->taskList[i].taskid == taskid)
        {
            taskRunList->taskList[i].changeFlag = flag;
            TASK_FMT_DEBUG(-1, TASK_LOG, "端口%08x 找到任务 %d 置位 %d\n", port.value, taskid, flag);
            return;
        }
    }

    TASK_FMT_DEBUG(-1, TASK_LOG, "端口%08x 未找到任务 %d 置位 %d\n", port.value, taskid, flag);
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_record_file_delete
* 函数功能: 针对任务号和端口 通知抄表线程删除任务记录文件
    度时用
* 输入参数:taskid 任务方案号
            port 任务端口
            flag 要置位的标记 1表示修改 2表示删除
* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_record_file_delete(uint16 taskid, OOP_OAD_U port)
{
    uint8           pipe = taskmng_mtr_pipe_get(&port);

    //目前只支持一路载波和4路485
    if(pipe > AMR_THREAD_NUM)
        return;

    int ret;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    dbupdate.infonum = taskid;
    ret = taskmng_db_update_queue_write( &pTaskRecordFileDel[pipe], &dbupdate);

    TASK_FMT_DEBUG(-1, TASK_LOG, "端口%08x 通知抄表线程删除任务 %d 记录文件 ret %d\n", port.value, taskid, ret);
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_info_read
* 函数功能: //根据透明任务方案号获取任务信息
* 输入参数: Taskid 任务号
* 输出参数: TaskInfo 获取的任务内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_trans_task_info_read(uint16 Taskid, OOP_TRANS_TASK_T * TaskInfo)
{
    int             ret;
    uint32          len = 0;
    NOMAL_OAD_T     NormalOad;

    memset(&NormalOad, 0x00, sizeof(NOMAL_OAD_T));
    NormalOad.oad.value = 0x6E110200;
    NormalOad.infoNum = Taskid;

    ret = db_read_nomal(gDBClientTask, &NormalOad, sizeof(OOP_TRANS_TASK_T), (uint8 *) TaskInfo, &len);

    if(ret != 0 || len != sizeof(OOP_TRANS_TASK_T))
        return - 1;

    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_specific_task_info_read
* 函数功能: //根据具体任务号获取任务信息
* 输入参数: Taskid 具体任务号
* 输出参数: TaskInfo 获取的任务内容
          
* 返 回 值: -1 获取失败
        0  获取成功
*******************************************************************************/
int taskmng_trans_specific_task_info_read(uint16 Taskid,OOP_OAD_U oad, OOP_TRANS_TASK_NODE_T * TaskInfo)
{
    int             ret;
    uint32          handle = 0;
    uint32          recNum = 0;
    READ_RECORD_T   inReadRecord;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T   stepRecordOut;
    memset(&inReadRecord, 0x00, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = Taskid;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = Taskid;
    inReadRecord.cEnd = Taskid+1;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0x6E130200;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = oad.value;
    inReadRecord.recordOAD.classtag = CLASS_DATA_INIT;
    inReadRecord.bTable = 1;
    inReadRecord.table.nNum = 0;
    stepReadRecord.MAC.nNum = 0;

    ret = db_read_record_start(gDBClientTask, &inReadRecord, &handle, &recNum);

    if(ret != 0 || recNum != 1)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "开始读具体任务失败 ret %d recNum %d \n", ret, recNum);
    }
    else
    {
        ret = db_read_record_step(gDBClientTask, handle, &stepReadRecord, &stepRecordOut);

        if(ret != 0)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "单步读具体任务失败 ret %d \n", ret);
            return -1;
        }

        if(stepRecordOut.recordnum != 1 || stepRecordOut.record[0].road.oadMain.value != 0x6E130200 ||
             stepRecordOut.record[0].outDatalen != sizeof(OOP_TRANS_TASK_NODE_T))
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "单步读取的具体任务记录有问题 \n");
            return -1;
        }

        memcpy(TaskInfo, stepRecordOut.record[0].outDataBuf, sizeof(OOP_TRANS_TASK_NODE_T));
        TASK_FMT_DEBUG(-1, TASK_LOG, "读取到具体任务 任务方案号 %d 具体任务序号 %d \n", 
            TaskInfo->transTaskid, TaskInfo->nodeid);
    }

   
    db_read_record_end(gDBClientTask, handle);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_change_fun 
* 函数功能:透明任务方案变更处理
* 输入参数: 
          choice 0 删除 1 变更
          TaskInfo 变更的任务信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_trans_task_change_fun(uint8 choice, OOP_TRANS_TASK_T * taskInfo)
{
    uint16          i;
    int             index = -1;

    //任务删除了
    if(choice == 0)
    {
        for(i = 0; i < gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->taskid)
            {
                index = i;
                //任务修改了就把任务记录文件删了

                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                //先给抄表线程置标记 再清空任务内容
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
                memset(&gTransTaskCfg.TaskCfg[i], 0x00, sizeof(OOP_TRANS_TASK_T));
                break;
            }
        }

        //后面的往前挪
        if(index != -1)
        {
            for(i = index; i < gTransTaskCfg.TaskNum - 1; i++)
            {
                gTransTaskCfg.TaskCfg[i] = gTransTaskCfg.TaskCfg[i + 1];
            }

            memset(&gTransTaskCfg.TaskCfg[gTransTaskCfg.TaskNum - 1], 0x00, sizeof(OOP_TRANS_TASK_T));
            gTransTaskCfg.TaskNum--;
            return TRUE;
        }
        

        return FALSE;
    }
    else
    {
        //判断是新增还是修改
        for(i = 0; i < gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->taskid)
            {
                index = i;
                //判断下修改的是不是就变了优先级为254 
                if(taskInfo->priority == 254)
                {
                    gTransTaskCfg.TaskCfg[i].priority = 254;
                    if(memcmp(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T))==0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "任务方案 %d 暂停 \n", taskInfo->taskid);
                        return FALSE;
                    }
                }
                //判断下是不是只修改了优先级
                if(gTransTaskCfg.TaskCfg[i].priority != taskInfo->priority)
                {               
                    gTransTaskCfg.TaskCfg[i].priority = taskInfo->priority;
                    if(memcmp(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T))==0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "任务方案 %d 优先级变更为 %d \n", taskInfo->taskid,taskInfo->priority);
                        fujian_task_priority_change_notify(taskInfo);
                        return FALSE;
                    }

                }

                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                //任务修改 给抄表线程置标记 
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                break;
            }
        }

        //新增的任务方案
        if(index == -1)
        {
            index = gTransTaskCfg.TaskNum;
            gTransTaskCfg.TaskNum++;
        }
        memcpy(&gTransTaskCfg.TaskCfg[index],taskInfo,sizeof(OOP_TRANS_TASK_T));
        return TRUE;
    }
}

/*******************************************************************************
* 函数名称: taskmng_trans_specific_task_change_fun 
* 函数功能:具体任务方案变更处理
* 输入参数: 
          choice 0 删除 1 变更
          TaskInfo 变更的任务信息
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_trans_specific_task_change_fun(uint8 choice, OOP_TRANS_TASK_NODE_T * taskInfo)
{
    uint16          i;
    int             index = -1;
    uint16 taskid;

    //具体任务删除了 到具体任务里找到这个 再根据任务方案号通知删除任务记录文件
    if(choice == 0)
    {
        for(i=0;i<gSpecificTaskList.taskNum;i++)
        {
            if(gSpecificTaskList.taskList[i].nodeid == taskInfo->nodeid && gSpecificTaskList.taskList[i].transTaskid == taskInfo->transTaskid)
            {
                index = i;
                taskid = gSpecificTaskList.taskList[i].transTaskid; //找到任务方案号看有没有对应的任务方案
                break;
            }
        }
        if(index!=-1)
        {
            for(i = index;i<gSpecificTaskList.taskNum-1;i++)
            {
                gSpecificTaskList.taskList[i] = gSpecificTaskList.taskList[i+1];
            }
            memset(&gSpecificTaskList.taskList[gSpecificTaskList.taskNum-1],0x00,sizeof(FUJIAN_SPECIFIC_TASK_T));
            gSpecificTaskList.taskNum--;
            
            for(i=0;i<gTransTaskCfg.TaskNum; i++)
            {
                if(gTransTaskCfg.TaskCfg[i].taskid == taskid)
                {
                    //有任务对应该删除的具体任务
                    taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                    return TRUE;
                }
            }
        }
        return FALSE;
    }
    else
    {
        //判断是新增还是修改
        for(i=0;i<gSpecificTaskList.taskNum;i++)
        {
            if(gSpecificTaskList.taskList[i].nodeid == taskInfo->nodeid && gSpecificTaskList.taskList[i].transTaskid == taskInfo->transTaskid)
            {
                index = i;
                break;
            }
        }
        if(index == -1)
        {
            index = gSpecificTaskList.taskNum;
            gSpecificTaskList.taskNum++;
        }
        
        gSpecificTaskList.taskList[index].transTaskid = taskInfo->transTaskid;
        gSpecificTaskList.taskList[index].nodeid = taskInfo->nodeid;
        
        //获取出具体的通信地址集合 最多255个
        gSpecificTaskList.taskList[index].addrAll.num = taskmng_FJ_ms_list_get(&taskInfo->ms,&gSpecificTaskList.taskList[index].addrAll);;
        gSpecificTaskList.taskList[index].uartParam =taskInfo->uartParam;
        gSpecificTaskList.taskList[index].reqMessType = taskInfo->reqMessType;
        gSpecificTaskList.taskList[index].reqMess = taskInfo->reqMess;
        gSpecificTaskList.taskList[index].resMessType = taskInfo->resMessType;
        gSpecificTaskList.taskList[index].resMaxLen = taskInfo->resMaxLen;
        gSpecificTaskList.taskList[index].resMinLen = taskInfo->resMinLen;
        gSpecificTaskList.taskList[index].resMessProc = taskInfo->resMessProc;
        gSpecificTaskList.taskList[index].next = NULL; //各自的线程初始化的时候给赋值

        //找有没有对应的任务方案
        for(i=0;i<gTransTaskCfg.TaskNum; i++)
        {
            if(gTransTaskCfg.TaskCfg[i].taskid == taskInfo->transTaskid)
            {
                //有任务对应该删除的具体任务
                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 2);
                return TRUE;
            }
        }

        return FALSE;
    }
}



/*******************************************************************************
* 函数名称: taskmng_trans_task_modify_fun 
* 函数功能:任务线程收到全局透明任务变更消息后进行处理
* 输入参数: dbupdate 变更的信息点号和数据标识
          
* 输出参数: 
* 返 回 值: TRUE 任务发生改变 FALSE 任务无需改变
*******************************************************************************/
BOOL taskmng_trans_task_modify_fun(DB_UPDATE_T dbupdate)
{
    int             ret;
    int  priority;
    uint8 pipe;
    uint16 i;
    TASK_FMT_DEBUG(-1, TASK_LOG, "透明任务变更 主oad %08x 次oad %08x infonum %d \n",dbupdate.MasterOad.value, dbupdate.oad.value, dbupdate.infonum);
    if(dbupdate.MasterOad.value == 0x6E130200)
    {
        //信息点号0xffff时删除所有具体任务 次oad为0xffff时删除一个透明任务方案下的所有具体任务
        if(dbupdate.infonum == 0xFFFF)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "所有具体任务删除 \n");
            for(i = 0; i < gTransTaskCfg.TaskNum; i++)
            {
                //通知删除任务记录文件
                taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
            }
            memset(&gSpecificTaskList, 0X00, sizeof(FUJIAN_SPECIFIC_TASK_LIST_T));
            return TRUE;
        }
        OOP_TRANS_TASK_NODE_T specificTask;
        memset(&specificTask, 0x00, sizeof(OOP_TRANS_TASK_NODE_T));
        if(dbupdate.oad.value == 0xffffffff)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "全局任务方案 %d 下属所有具体任务删除 \n",dbupdate.infonum);
            int          j;
            //先找出所有具体任务号 再删除
            for(i=0;i<gSpecificTaskList.taskNum;i++)
            {
                if(gSpecificTaskList.taskList[i].transTaskid == dbupdate.infonum)
                {
                    for(j=i;j<gSpecificTaskList.taskNum-1;j++)
                    {
                        gSpecificTaskList.taskList[j] = gSpecificTaskList.taskList[j+1];
                    }
                    memset(&gSpecificTaskList.taskList[gSpecificTaskList.taskNum-1],0x00,sizeof(FUJIAN_SPECIFIC_TASK_T));
                    gSpecificTaskList.taskNum--;
                    i--;
                }

            }
            for(i = 0; i < gTransTaskCfg.TaskNum; i++)
            {
                if(gTransTaskCfg.TaskCfg[i].taskid == dbupdate.infonum)
                {
                    //通知删除任务记录文件
                    taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    taskmng_trans_task_changeflag_set(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad, 3);
                    return TRUE;
                }
            }
            return FALSE;
        }

        ret = taskmng_trans_specific_task_info_read(dbupdate.infonum,dbupdate.oad,&specificTask);
        
        if(ret != 0)
        {
            //方案删除
            specificTask.transTaskid = dbupdate.infonum;
            specificTask.nodeid = dbupdate.oad.value;
            return taskmng_trans_specific_task_change_fun(0, &specificTask);
        }
        else
        {
            return taskmng_trans_specific_task_change_fun(1, &specificTask);
        }

    }

    switch(dbupdate.oad.value)
    {
        case 0x6E110200: //全局透明任务方案更新
            {
                //任务清空
                if(dbupdate.infonum == 0xffff)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "所有全局透明任务方案删除 \n");
                    for(i=0;i<gTransTaskCfg.TaskNum;i++)
                    {
                        taskmng_trans_task_record_file_delete(gTransTaskCfg.TaskCfg[i].taskid, gTransTaskCfg.TaskCfg[i].oad);
                    }
                    //各线程收到变更事件后查询一下任务条数 如果是0则直接自己清空
                    gTransTaskCfg.TaskNum = 0;
                    memset(&gTransTaskCfg.TaskCfg, 0x00, sizeof(gTransTaskCfg.TaskCfg));
                    return TRUE;
                }

                OOP_TRANS_TASK_T TaskInfo;

                memset(&TaskInfo, 0x00, sizeof(OOP_TRANS_TASK_T));

                ret = taskmng_trans_task_info_read(dbupdate.infonum, &TaskInfo);
                TASK_FMT_DEBUG(-1, TASK_LOG, "开始读取任务 %d  ret %d \n", dbupdate.infonum, ret);

                if(ret != 0)
                {
                    TaskInfo.taskid = dbupdate.infonum;
                    return taskmng_trans_task_change_fun(0, &TaskInfo);
                }
                else
                {
                    return taskmng_trans_task_change_fun(1, &TaskInfo);
                }

            }
            break;
        case 0x6E11FFFF: //任务优先级更新 通过消息通知对应抄表线程 不需要重新初始化任务
        {
            if(dbupdate.infonum != 0xFFFF)
            {
                priority = fujian_task_priority_update_get(gDBClientTask, dbupdate.infonum);
                TASK_FMT_DEBUG(-1, TASK_LOG, "透明任务方案 %d 优先级更新为 %d \n", dbupdate.infonum, priority);
                if(priority == 254 || priority == 255)
                {
                    for(i=0;i<gTransTaskCfg.TaskNum;i++)
                    {
                        if(dbupdate.infonum == gTransTaskCfg.TaskCfg[i].taskid)
                        {
                            pipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
                            if(pipe != 0xff)
                            {
                                OOP_TRANS_UPDATE_T data;
                                data.transTaskid = dbupdate.infonum;
                                data.priority = priority;
                                task_send_msg(gTaskSock,MSG_PRIORITY_UPDATE,(uint8*)&data,sizeof(OOP_TRANS_UPDATE_T),IPC_AMR+pipe);
                                TASK_FMT_DEBUG(-1, TASK_LOG, "通知端口 %d 任务优先级变更\n",pipe);
                            }
                            break;
                        }
                    }
                }
                return FALSE;
            }
        }
            break;
        default:
            break;
    }

    return FALSE;
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_ergodic
* 函数功能: 采集方案任务遍历
* 输入参数: 

* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
void taskmng_trans_task_ergodic()
{
    time_t          RealTime_t;
    uint8           i;
    BOOL            ischange;                       //抄表任务是否需要变更
    static BOOL     ischangelast = FALSE;

    if( ReloadTaskFlag ==1)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "档案变更后重新任务初始化 \n");
        gTransTaskCfg.TaskCfgState = TASK_CFG_UNINIT;
        ReloadTaskFlag = 0;
    }

    //根据方案参数变更状态执行相关操作
    switch(gTransTaskCfg.TaskCfgState)
    {
        case TASK_CFG_UNINIT:
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "开始任务初始化 \n");
                pthread_mutex_lock(&gTransTaskCfg.Lock);
                taskmng_trans_task_init();
                pthread_mutex_unlock(&gTransTaskCfg.Lock);
                taskmng_trans_task_collection_state_load();

                for(i = 0; i < AMR_THREAD_NUM; i++)
                {
                    gAmrThreadArg[i].TaskNeedInit = 1;
                    gAmrThreadArg[i].taskRunList.isInited = 0;
                }

                //TGThreadArg.TaskNeedInit = 1;
                gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                return;
            }

        case TASK_CFG_MODIFY:
            {
                //收到任务方案变更的消息进行处理 对应的变更任务在各线程里给置位 变更完后通知抄表线程更新这些变更的任务信息
                TASK_FMT_DEBUG(-1, TASK_LOG, "Task线程开始进行处理全局透明任务变更 \n");
                pthread_mutex_lock(&gTransTaskCfg.Lock);

                ischange = taskmng_trans_task_modify_fun(dbupdateTask);
                memset(&dbupdateTask, 0x00, sizeof(DB_UPDATE_T));
                pthread_mutex_unlock(&gTransTaskCfg.Lock);


                //如果任务改变需要通知抄表线程重新初始化 如果没改变则不需要
                if(ischangelast == FALSE)
                {
                    ischangelast = ischange;
                }

                if((ischange == FALSE) && (ischangelast == FALSE))
                {
                    gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                    return;
                }

                TASK_FMT_DEBUG(-1, TASK_LOG, "任务变更确认！ \n");
                gTransTaskCfg.ProCfgModifyDelaySec = 5; //原来30秒，现在先给缩短到5秒;
                RealTime_t = time(NULL);
                gTransTaskCfg.FunLastRunTime = RealTime_t;
                gTransTaskCfg.TaskCfgState = TASK_CFG_MODIFY_DELAY;
                return;
            }

        case TASK_CFG_MODIFY_DELAY:
            {
                RealTime_t = time(NULL);

                if(RealTime_t != gTransTaskCfg.FunLastRunTime)
                {
                    if(gTransTaskCfg.ProCfgModifyDelaySec % 10 == 0)
                    {
                        TASK_FMT_DEBUG(-1, TASK_LOG, "任务配置修改延时 : %d\n",gTransTaskCfg.ProCfgModifyDelaySec);
                    }

                    gTransTaskCfg.ProCfgModifyDelaySec--;
                    gTransTaskCfg.FunLastRunTime = RealTime_t;
                }

                if(gTransTaskCfg.ProCfgModifyDelaySec <= 0)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "任务配置修改延时已经完成 : %d\n",gTransTaskCfg.ProCfgModifyDelaySec);
                    taskmng_trans_task_collection_state_load();

                    for(i = 0; i < AMR_THREAD_NUM; i++)
                    {
                        gAmrThreadArg[i].TaskNeedInit = 1;
                    }

                    //TGThreadArg.TaskNeedInit = 1;
                    gTransTaskCfg.TaskCfgState = TASK_CFG_INITED;
                    ischangelast = FALSE;
                }
                return;
            }

        default:
            break;
    }

}

/*******************************************************************************
* 函数名称: taskmng_scheme_task_link_add
* 函数功能: 将任务按照优先级插入链表
* 输入参数: pAddTaskRunInfo 需要插入的任务
            headTaskRunInfo 当前优先级最高的链表头
* 输出参数: 
          
* 返 回 值: 优先级最高的链表头
*******************************************************************************/
FUJIAN_TASK_RUN_T *taskmng_trans_task_link_add(FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_TASK_RUN_T *headTaskRunInfo)
{
    FUJIAN_TASK_RUN_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    FUJIAN_TASK_RUN_T *ScollLink = NULL;//轮询链表时使用

    if(pAddTaskRunInfo == NULL)
        return headTaskRunInfo;

    if(headTaskRunInfo == NULL)
    {
        headTaskRunInfo = pAddTaskRunInfo;
        pAddTaskRunInfo->nextTask = NULL;
        return headTaskRunInfo; //添加成功
    }

    if(pAddTaskRunInfo->priority < headTaskRunInfo->priority)
    {
        pAddTaskRunInfo->nextTask = headTaskRunInfo;
        return pAddTaskRunInfo;
    }

    PTmpHeadTaskLink = headTaskRunInfo;
    while(PTmpHeadTaskLink != NULL)
    {
        //如果插入的节点任务优先级小于节点的任务优先级
        if(pAddTaskRunInfo->priority >= PTmpHeadTaskLink->priority)
        {
            //如果任务是独占性的 则放到相同优先级的最前面
            if(pAddTaskRunInfo->transTask->bmonopoly == 1 && pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority)
            {
                break;
            }
            //相同优先级的对比下方案号 小的在前面
            if(pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority && pAddTaskRunInfo->taskid < PTmpHeadTaskLink->taskid)
            {
                break;
            }
            //如果是空闲任务 则放到相同优先级的最后面 只有当前有空闲任务后插入的任务和空闲任务优先级一样时才需要判断
            if(PTmpHeadTaskLink->transTask->taskTypeData.taskType == 3 && pAddTaskRunInfo->priority == PTmpHeadTaskLink->priority)
            {
                break;
            }
            ScollLink = PTmpHeadTaskLink;
            PTmpHeadTaskLink = PTmpHeadTaskLink->nextTask;
        }
        else
            break;
    }
    pAddTaskRunInfo->nextTask = PTmpHeadTaskLink;
    if(NULL == ScollLink)
    {
        return pAddTaskRunInfo;
    }else
    {
        ScollLink->nextTask = pAddTaskRunInfo;
    }
    return  headTaskRunInfo;
}



/*******************************************************************************
* 函数名称: taskmng_trans_task_sort
* 函数功能: //将每个线程中的运行时任务队列按照优先级进行排序形成链表
* 输入参数: 
            
* 输出参数: 无
* 返 回 值:  
*******************************************************************************/
void taskmng_trans_task_sort(uint8 Pipe)
{
    uint16 i;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    pAmrThreadArg->taskHead = NULL;
    for(i = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        pAmrThreadArg->taskHead = taskmng_trans_task_link_add(&pAmrThreadArg->taskRunList.taskList[i],pAmrThreadArg->taskHead);
    }
    
    FUJIAN_TASK_RUN_T *PTmpHeadTaskLink = NULL;//轮询链表时使用
    PTmpHeadTaskLink = pAmrThreadArg->taskHead;
    TASK_FMT_TRACE(Pipe,REC_LOG,"抄表任务队列   : \n");

    while(PTmpHeadTaskLink != NULL)
    {
        TASK_FMT_TRACE(Pipe,REC_LOG," TaskId %d \n",PTmpHeadTaskLink->taskid);
        TASK_FMT_TRACE(Pipe,REC_LOG," Priority  %d  \n",PTmpHeadTaskLink->priority);
        PTmpHeadTaskLink = PTmpHeadTaskLink->nextTask;
    }
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_meter_info_get
* 函数功能: 遍历一个任务方案下的所有具体任务 找出所有电表个数
* 输入参数: 
* 输出参数: 无
* 返 回 值:   
*******************************************************************************/
uint16 taskmng_trans_task_meter_info_get(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    uint16 meterNum = 0;
    uint16 i;
    OOP_TSA_T meterList[MAX_METER_NUM];
    memset(meterList,0x00,sizeof(meterList));
    FUJIAN_SPECIFIC_TASK_T *specificTask = runTask->specificTask;
    memset(runTask->meterInfo,0x00,sizeof(runTask->meterInfo));

    while(specificTask!=NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "透明任务方案 %d 具体任务号 %d 需要抄读电表总数 %d \n", 
                runTask->taskid,specificTask->nodeid,specificTask->addrAll.num);
        for(i=0;i<specificTask->addrAll.num;i++)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, " %d 电表地址 %02x%02x%02x%02x%02x%02x \n",i,specificTask->addrAll.addr[i].add[0], 
                specificTask->addrAll.addr[i].add[1],specificTask->addrAll.addr[i].add[2],specificTask->addrAll.addr[i].add[3],
                specificTask->addrAll.addr[i].add[4],specificTask->addrAll.addr[i].add[5]);
            fujian_tsa_add(&specificTask->addrAll.addr[i],&meterNum,meterList);
        }
        specificTask = specificTask->next;
    }
    runTask->meterNum = meterNum;
    TASK_FMT_DEBUG(pipe, REC_LOG, "过滤后该任务需要抄读电表总数%d \n", meterNum);
    for(i=0;i<meterNum;i++)
    {
        memcpy(&runTask->meterInfo[i].meterAddr,&meterList[i],sizeof(OOP_TSA_T));
    }
    return meterNum;
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_run_change
* 函数功能: 每个抄表线程任务变更后的处理
* 输入参数: 
* 输出参数: 无
* 返 回 值:   
*******************************************************************************/
BOOL taskmng_trans_task_run_change(uint8 Pipe)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 i,j;
    int priority;
    uint8 flag = 0;
    uint8           taskPipe;
    uint16          taskNum = 0;
    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    taskRunList = &pAmrThreadArg->taskRunList;
    //FUJIAN_SPECIFIC_TASK_T *tmpSpecificTask= NULL;
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    if(gTransTaskCfg.TaskNum == 0)
    {
        TASK_FMT_DEBUG(Pipe, REC_LOG, "任务全清 \n");
        memset(&pAmrThreadArg->taskRunList, 0x00, sizeof(FUJIAN_TASK_RUN_LIST_T));
        pAmrThreadArg->taskHead = NULL;
        pthread_mutex_unlock(&gTransTaskCfg.Lock);
        return TRUE;
    }
    
    FUJIAN_TASK_RUN_LIST_T currentlist;
    memcpy(&currentlist,&pAmrThreadArg->taskRunList,sizeof(FUJIAN_TASK_RUN_LIST_T));
    
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        flag = 0;
        taskPipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
        if(taskPipe != Pipe)
            continue;
        for(j=0;j<currentlist.taskNum;j++)
        {
            if(currentlist.taskList[j].taskid == gTransTaskCfg.TaskCfg[i].taskid)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1 && currentlist.taskList[j].changeFlag == 1)
        {
            memcpy(&taskRunList->taskList[taskNum],&currentlist.taskList[j],sizeof(FUJIAN_TASK_RUN_T));
        }else
        {

            memset(&taskRunList->taskList[taskNum],0x00,sizeof(FUJIAN_TASK_RUN_T));
            taskRunList->taskList[taskNum].changeFlag = 1;//初始化后赋值
            taskRunList->taskList[taskNum].taskid = gTransTaskCfg.TaskCfg[i].taskid;
            taskRunList->taskList[taskNum].priority = gTransTaskCfg.TaskCfg[i].priority;
            taskRunList->taskList[taskNum].transTask = &gTransTaskCfg.TaskCfg[i];
        }
        taskRunList->taskList[taskNum].nextTask = NULL;
        taskRunList->taskList[taskNum].specificTask = NULL;
        taskRunList->taskList[taskNum].specificTaskNum = 0;
        //无论任务是否修改，都重新加载具体任务
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 0 || gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 1 ||
             gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            //再找到任务对应的具体任务 只有定时 广播 条件触发才有具体任务
            for(j = 0; j < gSpecificTaskList.taskNum; j++)
            {
                if(gSpecificTaskList.taskList[j].transTaskid == gTransTaskCfg.TaskCfg[i].taskid )
                {
                    //初始化的时候先把具体任务的next清一下 要不然任务更新容易出问题
                    gSpecificTaskList.taskList[j].next = NULL;
                    
                    taskRunList->taskList[taskNum].specificTask = fujian_specific_task_add(&gSpecificTaskList.taskList[j],taskRunList->taskList[taskNum].specificTask);
//                    if(taskRunList->taskList[taskNum].specificTask == NULL)
//                    {
//                        taskRunList->taskList[taskNum].specificTask = &gSpecificTaskList.taskList[j];
//                    }
//                    else
//                    {
//                        tmpSpecificTask = taskRunList->taskList[taskNum].specificTask;
//    
//                        while(tmpSpecificTask->next != NULL)
//                        {
//                            tmpSpecificTask = tmpSpecificTask->next;
//                        }
//    
//                        tmpSpecificTask->next = &gSpecificTaskList.taskList[j];
//                    }
                    taskRunList->taskList[taskNum].specificTaskNum++;
                }
            }
        }
        taskmng_trans_task_meter_info_get(Pipe,&taskRunList->taskList[taskNum]);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "任务方案 %d 需要抄读电表个数 %d 任务结果 %d\n",taskRunList->taskList[taskNum].taskid,
                taskRunList->taskList[taskNum].meterNum,taskRunList->taskList[taskNum].result);
        //读取6E11FFFF获取任务更新的优先级 如果是254 则任务暂停
        priority = fujian_task_priority_update_get(pAmrThreadArg->DBClientHandle,taskRunList->taskList[taskNum].taskid);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "获取到的任务更新优先级 %d \n", priority);
        if(priority == 254 || taskRunList->taskList[taskNum].priority == 254 )
        {
            taskRunList->taskList[taskNum].pauseFlag = 1;
        }
        taskNum++;
    }
    taskRunList->taskNum = taskNum;
    TASK_FMT_DEBUG(Pipe, REC_LOG, "任务变更流程完成 任务个数 %d \n", taskNum);
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
    pAmrThreadArg->taskHead = NULL;
    taskmng_trans_task_sort(Pipe);
    return TRUE;
}


/*******************************************************************************
* 函数名称: taskmng_trans_task_run_init
* 函数功能: 每个抄表线程初始化加载任务信息
* 输入参数: 
* 输出参数: 无
* 返 回 值:   
*******************************************************************************/
BOOL taskmng_trans_task_run_init(uint8 Pipe)
{

    uint16          i, j;
    uint8           taskPipe;
    uint16          taskNum = 0;
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    FUJIAN_TASK_RUN_LIST_T * taskRunList = NULL;
    //FUJIAN_SPECIFIC_TASK_T *tmpSpecificTask= NULL;
    int priority;
    if(gTransTaskCfg.TaskCfgState != TASK_CFG_INITED)
        return FALSE;
    
    if(pAmrThreadArg->taskRunList.isInited ==1)
    {
        //处理收到的变更
        return taskmng_trans_task_run_change(Pipe);
    }

    memset(&pAmrThreadArg->taskRunList, 0x00, sizeof(FUJIAN_TASK_RUN_LIST_T));
    pAmrThreadArg->taskHead = NULL;
    taskRunList = &pAmrThreadArg->taskRunList;

    TASK_FMT_DEBUG(Pipe, REC_LOG, "开始任务初始化 ！ \n");
    pthread_mutex_lock(&gTransTaskCfg.Lock);

    for(i = 0; i < gTransTaskCfg.TaskNum; i++)
    {
        taskPipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
    
        if(taskPipe != Pipe)
            continue;
        taskRunList->taskList[taskNum].changeFlag = 1;//初始化后赋值
        taskRunList->taskList[taskNum].taskid = gTransTaskCfg.TaskCfg[i].taskid;
        taskRunList->taskList[taskNum].priority = gTransTaskCfg.TaskCfg[i].priority;
        taskRunList->taskList[taskNum].transTask = &gTransTaskCfg.TaskCfg[i];
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 0 || gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 1 ||
             gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            //再找到任务对应的具体任务 只有定时 广播 条件触发才有具体任务
            //具体任务要按照序号大小排列 从小到大
            for(j = 0; j < gSpecificTaskList.taskNum; j++)
            {
                if(gSpecificTaskList.taskList[j].transTaskid == gTransTaskCfg.TaskCfg[i].taskid )
                {
                    //初始化的时候先把具体任务的next清一下 要不然任务更新容易出问题
                    gSpecificTaskList.taskList[j].next = NULL;

                    taskRunList->taskList[taskNum].specificTask = fujian_specific_task_add(&gSpecificTaskList.taskList[j],taskRunList->taskList[taskNum].specificTask);
//                    if(taskRunList->taskList[taskNum].specificTask == NULL)
//                    {
//                        taskRunList->taskList[taskNum].specificTask = &gSpecificTaskList.taskList[j];
//                    }
//                    else
//                    {
//                        tmpSpecificTask = taskRunList->taskList[taskNum].specificTask;
//    
//                        while(tmpSpecificTask->next != NULL)
//                        {
//                            tmpSpecificTask = tmpSpecificTask->next;
//                        }
//    
//                        tmpSpecificTask->next = &gSpecificTaskList.taskList[j];
//                    }
                    taskRunList->taskList[taskNum].specificTaskNum++;
                }
            }
        }
        taskmng_trans_task_meter_info_get(Pipe,&taskRunList->taskList[taskNum]);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "任务方案 %d 需要抄读电表个数 %d \n",taskRunList->taskList[taskNum].taskid,
                taskRunList->taskList[taskNum].meterNum);
        //读取6E11FFFF获取任务更新的优先级 如果是254 则任务暂停
        priority = fujian_task_priority_update_get(pAmrThreadArg->DBClientHandle,taskRunList->taskList[taskNum].taskid);
        TASK_FMT_DEBUG(Pipe, REC_LOG, "获取到的任务更新优先级 %d \n", priority);
        if(priority == 254 || taskRunList->taskList[taskNum].priority == 254 )
        {
            taskRunList->taskList[taskNum].pauseFlag = 1;
        }
        taskNum++;
    }
    
    
    taskRunList->taskNum = taskNum;
    TASK_FMT_DEBUG(Pipe, REC_LOG, "任务初始化完成 任务个数 %d \n", taskNum);
    pthread_mutex_unlock(&gTransTaskCfg.Lock);

    taskmng_trans_task_sort(Pipe);
    return TRUE;
}
/*******************************************************************************
* 函数名称: taskmng_trans_task_record_file_create 
* 函数功能://创建任务记录文件
* 输入参数: pipe 端口号
            TaskId 任务号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_trans_task_record_file_create(uint8 pipe,uint16 TaskId)
{

    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);
    FUJIAN_TASK_RECORD_FILE_T taskRecord;
    memset(&taskRecord,0X00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    taskRecord.taskid = TaskId;

    TASK_FMT_DEBUG(pipe, REC_LOG,"创建任务记录文件%s \n",pfile);
    write_pdata_xram(pfile,(char*)&taskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
}




/*******************************************************************************
* 函数名称: taskmng_trans_task_record_file_check 
* 函数功能://检查每个任务的私有文件是否存在 如没有则创建 私有文件存储每个任务的抄读状态
* 输入参数: pipe 端口号

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_trans_task_record_file_check(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    char      pfile[100]={0};
    uint16 i;
    uint16 taskid;

    for(i = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        taskid = pAmrThreadArg->taskRunList.taskList[i].taskid;
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,taskid);
        if(access(pfile,R_OK) == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务记录文件%s 存在\n",pfile);
            continue;
        }
        taskmng_trans_task_record_file_create(pipe,taskid);
    }

}


/*******************************************************************************
* 函数名称: taskmng_trans_task_run_reset
* 函数功能: 任务到新一轮 重新初始化
* 输入参数: runTaskInfo 任务运行时参数

* 输出参数: 无
* 返 回 值:  
*******************************************************************************/
void taskmng_trans_task_run_reset(FUJIAN_TASK_RUN_T *runTaskInfo)
{
    FUJIAN_SPECIFIC_TASK_T *tmp = runTaskInfo->specificTask;
    uint16 i;
    runTaskInfo->specificTaskIndex = 0;
    runTaskInfo->meterIndex = 0;
    runTaskInfo->noticeState = NOTICE_INIT;
    runTaskInfo->failFlag = 0;
    runTaskInfo->noticeFailCnt = 0;
    //runTaskInfo->thisTurnRetryCnt = 0;
    runTaskInfo->retryCycle = 0;
    if(runTaskInfo->storageCnt ==0)
        runTaskInfo->storageCnt = 1;
    //如果任务没执行 则不应该把存储次数加上
    if(runTaskInfo->result != SCHEME_UNDONE)
    {
        runTaskInfo->storageCnt++;
        if(runTaskInfo->storageCnt > runTaskInfo->transTask->storeNum)
            runTaskInfo->storageCnt=1;
    }
    while(tmp)
    {
        for(i=0;i<tmp->addrAll.num;i++)
        {
            tmp->addrAll.result[i] = 0;
            tmp->addrAll.retryCnt[i] = 0;
            tmp->addrAll.isReport[i] = 0;
        }
        tmp = tmp->next;
    }
    for(i=0;i<runTaskInfo->meterNum;i++)
    {
        runTaskInfo->meterInfo[i].meterResult = 0;
        runTaskInfo->meterInfo[i].specificTaskIndex = 0;
        runTaskInfo->meterInfo[i].failCnt = 0;
        runTaskInfo->meterInfo[i].multiBufIndex = 0;
    }
    runTaskInfo->meterSucNum = 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_run_retry
* 函数功能: 任务到新一轮失败重试周期的初始化
* 输入参数: runTaskInfo 任务运行时参数

* 输出参数: 无
* 返 回 值:  
*******************************************************************************/
void taskmng_trans_task_run_retry(uint8 pipe,FUJIAN_TASK_RUN_T *runTaskInfo)
{
    //如果是多帧关联任务 失败重试时从头开始
    uint8 retryCycle;
    if(runTaskInfo->transTask->bRelation == 1)
    {
        retryCycle = runTaskInfo->retryCycle;
        taskmng_trans_task_run_reset(runTaskInfo);
        //多帧关联任务重试时清空采集状态
        taskmng_trans_task_collection_state_update(pipe,runTaskInfo,TRANS_TASK_RELATION,NULL);
        runTaskInfo->retryCycle = retryCycle+1;
    }else
    {
        //清空下每块表的失败次数
        uint16 i;
        FUJIAN_SPECIFIC_TASK_T *tmp = runTaskInfo->specificTask;
        while(tmp)
        {
            for(i=0;i<tmp->addrAll.num;i++)
            {
                tmp->addrAll.retryCnt[i] = 0;
                //重试的时候不能把上报标识清空
                //tmp->addrAll.isReport[i] = 0;
            }
            tmp = tmp->next;
        }
        runTaskInfo->retryCycle++;
        for(i=0;i<runTaskInfo->meterNum;i++)
        {
            runTaskInfo->meterInfo[i].failCnt = 0;
            runTaskInfo->meterInfo[i].multiBufIndex = 0;
            if(runTaskInfo->meterInfo[i].meterResult==3)
                runTaskInfo->meterInfo[i].meterResult = 0;
        }
        //重启时从头开始搜一遍
        runTaskInfo->specificTaskIndex = 0;
        runTaskInfo->meterIndex = 0;
        runTaskInfo->meterSucNum = 0;
        runTaskInfo->failFlag = 0;
    }
    
}


/*******************************************************************************
* 函数名称: TaskRun_UpdateTimes
* 函数功能: 计算每个线程中任务执行轮次
* 输入参数: Pipe  端口号
          index  任务索引
          TaskInfo 任务运行时参数
* 输出参数: 无
* 返 回 值:  
*******************************************************************************/

BOOL task_trans_task_run_cycle_update(uint8 Pipe, FUJIAN_TASK_RUN_T * runTaskInfo)
{

    time_t          RealTime_t;                     //当前系统时间的转换的秒数
    time_t          ThisTurnStorageTime_t;
    time_t          ThisTurnBeginTime_t;
    time_t          ThisTurnEndTime_t;
    time_t          NextTurnBeginTime_t;
    time_t          tmp;
    OOP_TI_T        runTi;                          //执行周期
    OOP_TI_T        retryTi;                        //失败重试周期
    OOP_TI_T        tmpTi;
    OOP_TRANS_TASK_T * taskInfo;
    time_t          thisTurnRetryTime_t;
    OOP_DATETIME_S_T timeNow;

    taskInfo = runTaskInfo->transTask;

    //过秒计算
    if(gPastTimeFlag[Pipe].BitSect.Second != TRUE)
        return FALSE;

    memcpy(&runTi, &taskInfo->cycle, sizeof(OOP_TI_T));
    memcpy(&retryTi, &taskInfo->retryCycle, sizeof(OOP_TI_T));

    RealTime_t = time(NULL);
    DT_Time2DateTimeShort(RealTime_t, &timeNow);

    //先判断当前时间是否在配置的任务开始时间和结束时间内
    if(fujian_time_compare(RealTime_t, &taskInfo->startTime) < 0 || fujian_time_compare(RealTime_t, &taskInfo->endTime) > 0)
    {
        runTaskInfo->periodStat = AMR_AFT_PERIOD;
        return FALSE;
    }
    
    //周期为0的任务要靠触发时间来计算
    //有可能配的触发任务周期没写0
    if(runTi.value == 0 || runTaskInfo->transTask->taskTypeData.taskType == 5)
    {
        //如果触发时间为0 则表示该任务无法执行 用年来判断时间是否有效
        if(runTaskInfo->triggerTime.year == 0)
        {
            runTaskInfo->periodStat = AMR_PRE_PERIOD;
            return FALSE;
        }
        else
        {
            //有触发时间 则要计算任务开始时间和结束时间
            //判断下开始时间是否和触发时间相等 以防多次触发 以最新的一次为准
            if(runTaskInfo->thisTurnBeginTime.year != 0 && runTaskInfo->thisTurnEndTime.year != 0)
            {
                if(memcmp(&runTaskInfo->thisTurnBeginTime, &runTaskInfo->triggerTime, sizeof(OOP_DATETIME_S_T)) == 0)
                {
                
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime, &ThisTurnBeginTime_t);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime, &ThisTurnEndTime_t);
                    
                    if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t)
                    {
                        //如果当前时间还在本轮 则看下下一个失败重试开始时间是否需要更新
                        DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime, &thisTurnRetryTime_t);
                        runTaskInfo->periodStat = AMR_IN_PERIOD;
                        if(RealTime_t < thisTurnRetryTime_t)
                        {
                            return FALSE;
                        }
                        else
                        {
                            //如果任务当前状态是成功 则不需要再计算
                            if(runTaskInfo->result == SCHEME_FINISHED)
                            {
                                return FALSE;
                            }
                            //计算下一轮失败重试开始时间
                            fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,
                                 &runTaskInfo->thisTurnRetryTime);
                            DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                            DT_TimeAddInv(&tmp,retryTi);
                            DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
                            TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务%d 计算出新的失败重试时间\n",runTaskInfo->taskid);
                            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                            

                            //如果任务失败了 则可以启动下一轮
                            if(runTaskInfo->result == SCHEME_FAIL)
                            {
                                runTaskInfo->result = SCHEME_UNDONE;
                                runTaskInfo->isNewTurn = 1;
                                taskmng_trans_task_run_retry(Pipe,runTaskInfo);
                            }

                            runTaskInfo->periodStat = AMR_IN_PERIOD;
                            return TRUE;
                        }
                    }
                    else
                    {
                        //如果超过了本轮结束时间 则该触发任务不能执行 等后面再被触发 将触发时间置空
                        runTaskInfo->periodStat = AMR_AFT_PERIOD;
                        memset(&runTaskInfo->triggerTime, 0x00, sizeof(OOP_DATETIME_S_T));
                        return FALSE;
                    }
                }
                else
                {
                    //按照新的触发时间来计算任务时间
                    memcpy(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                    memcpy(&runTaskInfo->storageTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                    //结束时间为触发时刻加允许触发时间
                    DT_DateTimeShort2Time(&runTaskInfo->triggerTime,&ThisTurnEndTime_t);
                    tmpTi.value = taskInfo->taskTypeData.addi_param.condition.time;
                    tmpTi.unit = TI_MIN;
                    DT_TimeAddInv(&ThisTurnEndTime_t,tmpTi);

                    DT_Time2DateTimeShort(ThisTurnEndTime_t,&runTaskInfo->thisTurnEndTime);
                    if(retryTi.value == 0)
                    {
                        runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                    }else
                    {
                        fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,&runTaskInfo->thisTurnRetryTime);
                        DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                        DT_TimeAddInv(&tmp,retryTi);
                        DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                    }

                    TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 到新的任务开始时间\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 到新的任务结束时间\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 到新的失败重试周期\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 存储时标\n",runTaskInfo->taskid);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                    //按照新的触发时间来重新执行
                    runTaskInfo->result = SCHEME_UNDONE;
                    taskmng_trans_task_run_reset(runTaskInfo);
                    runTaskInfo->isNewTurn = 1;
                    runTaskInfo->periodStat = AMR_IN_PERIOD;
                    taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                    return TRUE;
                }
            }
            else
            {
                //计算新的任务周期
                memcpy(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                memcpy(&runTaskInfo->storageTime,&runTaskInfo->triggerTime,sizeof(OOP_DATETIME_S_T));
                //结束时间为触发时刻加允许触发时间
                DT_DateTimeShort2Time(&runTaskInfo->triggerTime,&ThisTurnEndTime_t);
                tmpTi.value = taskInfo->taskTypeData.addi_param.condition.time;
                tmpTi.unit = TI_MIN;
                DT_TimeAddInv(&ThisTurnEndTime_t,tmpTi);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 触发有效时间 %d 分钟 ThisTurnEndTime_t %ld\n",runTaskInfo->taskid,
                        taskInfo->taskTypeData.addi_param.condition.time,ThisTurnEndTime_t);
                DT_Time2DateTimeShort(ThisTurnEndTime_t,&runTaskInfo->thisTurnEndTime);
                if(retryTi.value == 0)
                    {
                    runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                }else
                {
                    fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->triggerTime,&runTaskInfo->thisTurnRetryTime);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                }

                TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 任务开始时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 任务结束时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 失败重试时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "触发任务 %d 存储时标\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                taskmng_trans_task_run_reset(runTaskInfo);
                runTaskInfo->result = SCHEME_UNDONE;
                runTaskInfo->isNewTurn = 1;
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                return TRUE;
            }
        }
    }
    else
    {
        //周期任务 计算轮次
        if(runTaskInfo->thisTurnBeginTime.year != 0 && runTaskInfo->thisTurnEndTime.year != 0)
        {
            //算过开始结束时间 计算是否需要更新
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime, &ThisTurnBeginTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime, &ThisTurnEndTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->nextTurnBeginTime, &NextTurnBeginTime_t);
            DT_DateTimeShort2Time(&runTaskInfo->storageTime, &ThisTurnStorageTime_t);
            if(RealTime_t >= ThisTurnBeginTime_t && RealTime_t < ThisTurnEndTime_t)
            {
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                //如果当前时间还在本轮 则看下下一个失败重试开始时间是否需要更新
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime, &thisTurnRetryTime_t);
                if(RealTime_t < thisTurnRetryTime_t)
                {
                    return FALSE;
                }else
                {
                    //如果任务当前状态是成功 则不需要再计算
                    if(runTaskInfo->result == SCHEME_FINISHED)
                    {
                        return FALSE;
                    }
                    //计算下一轮失败重试开始时间
                    fujian_time_calc(&timeNow, &retryTi, &runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnRetryTime);
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 计算出新的失败重试时间 result %d\n",runTaskInfo->taskid,runTaskInfo->result);
                    fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                    //如果任务失败了 则可以启动下一轮
                    if(runTaskInfo->result == SCHEME_FAIL)
                    {
                        runTaskInfo->result = SCHEME_UNDONE;
                        runTaskInfo->isNewTurn = 1;
                        taskmng_trans_task_run_retry(Pipe,runTaskInfo);
                    }
                    runTaskInfo->periodStat = AMR_IN_PERIOD;
                    return TRUE;
                }
            }
            else if(RealTime_t < ThisTurnBeginTime_t && RealTime_t > ThisTurnStorageTime_t)
            {
                //以防往前对时的时候 任务不会重新执行
                //还没到可以执行的时间
                runTaskInfo->periodStat = AMR_PRE_PERIOD;
                return FALSE;
            }
            else if(RealTime_t >= ThisTurnEndTime_t && RealTime_t < NextTurnBeginTime_t )
            {
                //超过了本轮结束时间 还没到下一轮 
                taskmng_trans_task_isreport_check(Pipe,runTaskInfo);
                runTaskInfo->periodStat = AMR_AFT_PERIOD;
                return FALSE;
            }else
            {
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 准备计算下一轮 当前存储次数 %d\n",runTaskInfo->taskid,runTaskInfo->storageCnt);
                taskmng_trans_task_isreport_check(Pipe,runTaskInfo);
                //超过下一轮开始时间 开始计算新一轮
                //计算新的任务周期
                fujian_time_calc(&timeNow, &runTi, &taskInfo->startTime,&runTaskInfo->thisTurnBeginTime);
                fujian_time_calc(&timeNow, &runTi, NULL,&runTaskInfo->storageTime);
                fujian_time_calc(&timeNow, &runTi, &taskInfo->endTime,&runTaskInfo->thisTurnEndTime);
                //按照福建的算法 如果任务开始时间和结束时间都可以被执行周期整除 则算出来的本轮开始结束时间会相等
                //此时手动给本轮结束时间增加一个周期 还会出现任务开始时间比结束时间大的情况 所以算两个时间的前后
                //if(memcmp(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime,sizeof(OOP_DATETIME_S_T))==0)
                if(fujian_datetime_s_compare(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime)>=0)
                {
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime,&tmp);
                    DT_TimeAddInv(&tmp,runTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnEndTime);
                }
                if(retryTi.value == 0|| fujian_ti_compare(&runTi,&retryTi)<0)
                {
                    runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

                }else
                {
                    fujian_time_calc(&timeNow, &retryTi, &taskInfo->startTime,&runTaskInfo->thisTurnRetryTime);
                    //失败重试时间要再加一个失败重试周期 要不然会算出来比当前时间小
                    DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                    DT_TimeAddInv(&tmp,retryTi);
                    DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);

                }

                    
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime,&NextTurnBeginTime_t);
                DT_TimeAddInv(&NextTurnBeginTime_t,runTi);
                DT_Time2DateTimeShort(NextTurnBeginTime_t,&runTaskInfo->nextTurnBeginTime);
                
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮任务开始时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮任务结束时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮失败重试时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 下轮任务开始时间\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->nextTurnBeginTime);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 存储时标\n",runTaskInfo->taskid);
                fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
                taskmng_trans_task_run_reset(runTaskInfo);
                TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 存储次数 %d\n",runTaskInfo->taskid,runTaskInfo->storageCnt);
                runTaskInfo->result = SCHEME_UNDONE;
                runTaskInfo->isNewTurn = 1;
                runTaskInfo->periodStat = AMR_IN_PERIOD;
                //存在往前对时后计算出来不再任务执行周期
                if(fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnBeginTime)<0 ||fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnEndTime)>0 )
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG, "往前对时后不在任务执行周期内\n");
                    runTaskInfo->periodStat = AMR_PRE_PERIOD;
                    return FALSE;
                }
                taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);
                return TRUE;
                
            }
            

        }else
        {
            //任务开始结束时间为空 第一次计算
            fujian_time_calc(&timeNow, &runTi, &taskInfo->startTime,&runTaskInfo->thisTurnBeginTime);
            fujian_time_calc(&timeNow, &runTi, NULL,&runTaskInfo->storageTime);
            fujian_time_calc(&timeNow, &runTi, &taskInfo->endTime,&runTaskInfo->thisTurnEndTime);
            if(fujian_datetime_s_compare(&runTaskInfo->thisTurnBeginTime,&runTaskInfo->thisTurnEndTime)>=0)
            {
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnEndTime,&tmp);
                DT_TimeAddInv(&tmp,runTi);
                DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnEndTime);
            }
            //如果失败重试周期是0 就不重试 赋个任务结束时间
            //失败重试周期比执行周期大 也不重试
            if(retryTi.value == 0 || fujian_ti_compare(&runTi,&retryTi)<0)
            {
                runTaskInfo->thisTurnRetryTime = runTaskInfo->thisTurnEndTime;

            }else
            {
                fujian_time_calc(&timeNow, &retryTi, &taskInfo->startTime,&runTaskInfo->thisTurnRetryTime);
                DT_DateTimeShort2Time(&runTaskInfo->thisTurnRetryTime,&tmp);
                DT_TimeAddInv(&tmp,retryTi);
                DT_Time2DateTimeShort(tmp,&runTaskInfo->thisTurnRetryTime);
            }
            DT_DateTimeShort2Time(&runTaskInfo->thisTurnBeginTime,&NextTurnBeginTime_t);
            DT_TimeAddInv(&NextTurnBeginTime_t,runTi);
            DT_Time2DateTimeShort(NextTurnBeginTime_t,&runTaskInfo->nextTurnBeginTime);
            
            TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮任务开始时间\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnBeginTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮任务结束时间\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnEndTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 本轮失败重试周期\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->thisTurnRetryTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 下轮任务开始时间\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->nextTurnBeginTime);
            TASK_FMT_DEBUG(Pipe, REC_LOG, "周期任务 %d 存储时标\n",runTaskInfo->taskid);
            fujian_datetime_s_log_write(Pipe, REC_LOG, &runTaskInfo->storageTime);
            taskmng_trans_task_run_reset(runTaskInfo);
            runTaskInfo->result = SCHEME_UNDONE;
            runTaskInfo->isNewTurn = 1;
            runTaskInfo->periodStat = AMR_IN_PERIOD;

            //第一次算的时候需要再判断下当前时间和开始时间 有可能还没到可以执行的时间
            if(fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnBeginTime)<0 ||fujian_time_compare(RealTime_t,&runTaskInfo->thisTurnEndTime)>0 )
            {
                TASK_FMT_DEBUG(Pipe, REC_LOG, "任务初次计算后不在任务执行周期内\n");
                runTaskInfo->periodStat = AMR_PRE_PERIOD;
                return FALSE;
            }
            //不在执行周期内的不更新采集状态
            taskmng_trans_task_collection_state_update(Pipe,runTaskInfo,TRANS_TASK_INIT,NULL);

            return TRUE;

        }
    }


}



/*******************************************************************************
* 函数名称: taskmng_task_run_ergodic
* 函数功能: 每个线程内更新任务运行时参数
* 输入参数: 
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_run_ergodic(uint8 Pipe)
{
    uint16 i;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    if(pAmrThreadArg->TaskNeedInit == 1)
    {
        pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
        pAmrThreadArg->TaskNeedInit =0;
    }
    switch(pAmrThreadArg->TaskModifyFlag)
    {

    case TASK_CFG_UNINIT:
    {
        while(1)
        {
            if(fujian_task_record_file_del_check(Pipe)==0)
                break;
        }
        if(taskmng_trans_task_run_init(Pipe)==TRUE)
        {
            taskmng_trans_task_record_file_check(Pipe);
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_INITED;
            pAmrThreadArg->taskRunList.isInited = 1;
            //任务初始化完之后判断下是否有上电触发任务
//            if(pAmrThreadArg->taskPowerOnFlag ==1)
//            {
//                taskmng_trigger_task_poweron_check(Pipe);
//                pAmrThreadArg->taskPowerOnFlag = 0 ;
//            }

        }
        break;
    }
    default:
        break;
    }
    if(pAmrThreadArg->taskPowerOnFlag ==1)
    {
        taskmng_trigger_task_poweron_check(Pipe);
        pAmrThreadArg->taskPowerOnFlag = 0 ;
    }

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        task_trans_task_run_cycle_update(Pipe,&pAmrThreadArg->taskRunList.taskList[i]);
    }

}

/*******************************************************************************
* 函数名称:         taskmng_fujian_task_queue_add
* 函数功能:         任务链表添加
* 输入参数:         PLCTASK_QUEUE *PlcTaskQueue//任务队列
                    PlcTask                    //当前任务
* 输出参数:         int
* 返 回 值:         > 0  添加成功
                    < 0  添加失败

*******************************************************************************/
int taskmng_fujian_task_queue_add ( uint8 CommPipe,MAIN_CYC_STATE_T TaskName )
{
    int             i = 0;
    PLCTASK_QUEUE*  PlcAddTask = NULL;
    PLCTASK_QUEUE*  PlcTaskQueue = NULL;

    PlcTaskQueue = gAmrThreadArg[CommPipe].PlcTaskQueue;

    for ( i = 0; i < PLCTASK_MAXNUM; i++ )
    {
        if ( PlcTaskQueue[i].TaskStat == 0 )
        {
            PlcAddTask = &PlcTaskQueue[i];
            break;
        }
    }

    //TASK_FMT_TRACE ( CommPipe,COMM_LOG, "查找到相应的任务队列 %d \n",i );
    //任务队列满
    if ( PlcAddTask == NULL )
    {
        return -1;
    }

    i = 0;
    while ( gInnerTaskList[i].TaskName != 0xFF )
    {
        if ( gInnerTaskList[i].TaskName == TaskName )
        {
            PlcAddTask->TaskStat    = 1;
            PlcAddTask->TaskName    = TaskName;
            PlcAddTask->Priority    = gInnerTaskList[i].Priority;
            PlcAddTask->DelaySec    = gInnerTaskList[i].DelaySec;
            PlcAddTask->RoutrtCtrl  = gInnerTaskList[i].RoutrtCtrl;
            PlcAddTask->pPlcTaskFun = gInnerTaskList[i].pPlcTaskFun;      //任务的处理函数
            PlcAddTask->PlcTaskDeal_Stat = TASK_INIT;                      //任务赋值为初始状态
            break;
        }
        i++;
    }

    //任务队列的插入
    InsertPlcTaskQueue ( CommPipe,PlcAddTask );
    //TASK_FMT_TRACE ( CommPipe,COMM_LOG,"新增任务 %d\n",TaskName);

    PLCTASK_QUEUE*   pTmpLink = NULL;

    pTmpLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;
    TASK_FMT_TRACE ( CommPipe,COMM_LOG,"新增任务后 刷新任务队列   : \n" );
    while ( pTmpLink != NULL )
    {
        TASK_FMT_TRACE ( CommPipe,COMM_LOG,"TaskName  %d Priority %d  \n",pTmpLink->TaskName,pTmpLink->Priority);
        pTmpLink = pTmpLink->next;
    }

    return 1;

}



/*******************************************************************************
* 函数名称: taskmng_fujian_task_creat
* 函数功能: 任务队列产生判断
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_fujian_task_creat(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    //上电任务
    if(gPowerOnFlag[Pipe]==TRUE)
    {
        gPowerOnFlag[Pipe]=FALSE;
        //上电产生档案比对任务
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "上电产生抄表任务 ！\n");
        taskmng_fujian_task_queue_add(Pipe,MS_NORMAL_AMR); 

        Init_ReadMtr(Pipe);
        
        //taskmng_trigger_task_poweron_check(Pipe);
    }
    
    //载波特有的任务
    if(Pipe == 0)
    {
        if(gPastTimeFlag[Pipe].BitSect.Second == TRUE)
        {
            if(pAmrThreadArg->IsReportHPLCInfo == 0 && time(NULL) -pAmrThreadArg->HPLCInfoSendTime > 60)
            {
                //taskmng_HPLC_info_get(Pipe);
    
            }
        }
    
        if(IsSetAddr[Pipe] == 0 && taskmng_addr_isvalid_check(LocalAddr,6) > 0)
        {
            TASK_FMT_DEBUG(Pipe, COMM_LOG, "设置模块地址 ！\n");
            fujian_local_addr_set(Pipe);
        }

        if(IsSetGatherMode[Pipe] == 0 )
        {
            if(fujian_gather_mode_get_from_ccoRouter(Pipe) == gGatherMode+1)
            {
                IsSetGatherMode[Pipe] = 1; 
            }else
            {
                TASK_FMT_DEBUG(Pipe, COMM_LOG, "设置通信协议模式 ！\n");
                fujian_gather_mode_set(Pipe);
            }
        }
        if(gIsGetCCOInfo[Pipe] == 0)
        {
            fujian_cco_info_get(Pipe);
        }
    }
    
    //穿透任务的产生
    if(pAmrThreadArg->RelayTrans.isValid ==0)
    {
        while(1)
        {
            ret = taskmng_relay_exist_check(Pipe);
            if(ret !=-1)
            {
                break;
            }
        }
       if(ret == 1)
       {
           TASK_FMT_DEBUG( Pipe, RELAY_LOG, "产生穿透任务 ！\n");
           taskmng_fujian_task_queue_add(Pipe,MS_RELAY_TRANS);
       }
    }

}

/*******************************************************************************
* 函数名称: taskmng_plc_task_run
* 函数功能: 任务执行处理
* 输入参数: 
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_fujian_task_run ( uint8 CommPipe )
{

    int              Ret;
    PLCTASK_QUEUE*   pTmpLink = NULL;
    PLCTASK_QUEUE*   PlcTask_RunLink = NULL;
    static uint8     gDelaySec[AMR_THREAD_NUM];

    PlcTask_RunLink = gAmrThreadArg[CommPipe].PlcTask_RunLink;

    //当前任务是否有效,如果无效表明已经执行完成,执行下一个任务队列
    if ( PlcTask_RunLink == NULL || PlcTask_RunLink->TaskStat == 0 )
    {
        //TASK_FMT_TRACE(CommPipe,COMM_LOG, "当前没有任务需要执行 \n" );
        return;//任务队列为空执行返回不需要执行
    }

    //当前任务的执行处理

    switch ( PlcTask_RunLink->PlcTaskDeal_Stat )
    {
        case TASK_INIT:                         //任务的初始化操作
            TASK_FMT_TRACE(CommPipe,COMM_LOG, "---->    新任务 : %d 执行 开始   ------   \n",PlcTask_RunLink->TaskName );
            if ( PlcTask_RunLink->RoutrtCtrl != RCS_NULL )
            {
                TASK_FMT_TRACE(CommPipe,COMM_LOG, " 路由控制 : %d  \n",PlcTask_RunLink->RoutrtCtrl );
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERCTRL_DEAL;
            }
            else
            {
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;
            }
            break;

        case ROUTERCTRL_DEAL:                   //路由的操作
            Ret = taskmng_router_ctrl_set ( CommPipe,PlcTask_RunLink->RoutrtCtrl );
            if ( Ret > 0 )
            {
                if ( PlcTask_RunLink->DelaySec <= 5 )
                {
                    gDelaySec[CommPipe] = PlcTask_RunLink->DelaySec;
                }
                else
                {
                    gDelaySec[CommPipe] = GetEnvValue ( "PLC_DELAY",PlcTask_RunLink->DelaySec );
                }
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERCTRL_DELAY;
            }
            break;

        case ROUTERCTRL_DELAY:                  //延时操作
            Ret = taskmng_delay_deal ( CommPipe,&gDelaySec[CommPipe] );
            if ( Ret > 0 )
            {
                PlcTask_RunLink->PlcTaskDeal_Stat = FUN_DEAL;
            }
            break;

        case FUN_DEAL:                          //当前任务处理

            Ret = PlcTask_RunLink->pPlcTaskFun ( CommPipe );
            if ( Ret > 0 )
            {
                TASK_FMT_TRACE(CommPipe,COMM_LOG, "---->  任务 : %d 执行 结束  ------    \n",PlcTask_RunLink->TaskName );

                pTmpLink = PlcTask_RunLink->next;
                PlcTask_RunLink->TaskStat = 0;
                PlcTask_RunLink = pTmpLink;
                
            }
            else
                return;  //抄表过程中添加的事件上报任务如果不return到下面会被删除
            
            break;
        default:
            break;
 
    }
    gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
}

/*******************************************************************************
* 函数名称: taskmng_task_runlist_check
* 函数功能: 检查是否有需要运行的抄表任务
* 输入参数: 端口
* 输出参数: TaskInfo  待运行的任务 如没有则为空
* 返 回 值: >0                  有任务
*           <0               抄表任务全部完成
*******************************************************************************/
int taskmng_trans_task_run_check(uint8 Pipe, FUJIAN_TASK_RUN_T **TaskInfo)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    FUJIAN_TASK_RUN_T * tmpTask = pAmrThreadArg->taskHead;

    while(1)
    {
        if(tmpTask != NULL)
        {
            //如果任务是独占的 不往下找了
            if(tmpTask->periodStat == AMR_IN_PERIOD && tmpTask->transTask->bmonopoly == 1)
            {
                *TaskInfo = tmpTask;
                return 1;
            }
            else
            {   
                //没有配具体任务的方案也不能执行 不能更新执行时间
                if(tmpTask->periodStat != AMR_IN_PERIOD || tmpTask->result == SCHEME_FINISHED ||
                     tmpTask->result == SCHEME_FAIL || tmpTask->pauseFlag == 1 || 
                     ((tmpTask->transTask->taskTypeData.taskType == 0 ||tmpTask->transTask->taskTypeData.taskType == 1 ||
                     tmpTask->transTask->taskTypeData.taskType == 5)&& tmpTask->specificTask==NULL))
                {
                    tmpTask = tmpTask->nextTask;
                }
                else
                {
                    *TaskInfo = tmpTask;
                    return 1;
                }
            }
        }
        else
            return - 1;
    }
}

/*******************************************************************************
* 函数名称: taskmng_task_record_file_load 
* 函数功能://任务开始抄读时检查任务记录文件 当前轮次的任务是否抄过 如果抄过则将部分结果加载
* 输入参数: pipe 端口号
            TaskInfo 任务信息
            RunTask 任务运行时信息
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_trans_task_record_file_load(uint8 pipe,FUJIAN_TASK_RUN_T* RunTask)
{

    uint16 i,j;

    FUJIAN_TASK_RECORD_FILE_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    uint16 TaskId = RunTask->taskid;
    uint8 flag = 0;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);

    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(FUJIAN_TASK_RECORD_FILE_T));

    //如果任务记录文件中记录的信息和任务当前轮次相同则加载记录中的抄表结果
    if(memcmp(&TaskRecord.thisTurnBeginTime,&RunTask->thisTurnBeginTime,sizeof(OOP_DATETIME_S_T))==0)
    {
        //暂不取这两个 根据表成功标记来判断
        //RunTask->specificTaskIndex = TaskRecord.specificTaskIndex;
        RunTask->storageCnt = TaskRecord.storageCnt;
        //RunTask->meterIndex = TaskRecord.meterIndex;
        RunTask->result  = TaskRecord.result;
        //先给成功表个数置0
        RunTask->meterSucNum = 0;
        //如果不是成功状态 一律再抄一遍
        if(RunTask->result != SCHEME_FINISHED)
            RunTask->result = SCHEME_UNDONE;
        if(RunTask->meterNum!=TaskRecord.meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"任务记录文件内电表个数和当前任务不一致RunTask->meterNum %d TaskRecord.meterNum %d \n",
                RunTask->meterNum,TaskRecord.meterNum);
        }
        for(i=0;i<RunTask->meterNum;i++)
        {
            flag = 0;
            for(j=0;j<TaskRecord.meterNum;j++)
            {
                if(memcmp(&RunTask->meterInfo[i].meterAddr,&TaskRecord.meterInfo[j].meterAddr,sizeof(OOP_TSA_T))==0)
                {
                    flag = 1;
                    RunTask->meterInfo[i].meterResult = TaskRecord.meterInfo[j].meterResult;
                    //没抄成功的表赋初值
                    if(RunTask->meterInfo[i].meterResult == 2)
                    {
                        RunTask->meterSucNum++;
                    }
                    else
                    {
                        RunTask->meterInfo[i].meterResult = 0;
                    }
                    
                    break;
                }
            }
            if(flag == 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"有表还未抄成功 \n");
                RunTask->result = SCHEME_UNDONE;
            }
        }
        
    }
    else  //将任务记录文件中信息初始化为当前轮次
    {
        memset(&TaskRecord,0X00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
        memcpy(&TaskRecord.thisTurnBeginTime,&RunTask->thisTurnBeginTime,sizeof(OOP_DATETIME_S_T));
        TaskRecord.taskid = RunTask->taskid;
        TaskRecord.meterNum = RunTask->meterNum;
        memcpy(TaskRecord.meterInfo,RunTask->meterInfo,sizeof(FUJIAN_METER_INFO_T)*MAX_METER_NUM);
        TASK_FMT_DEBUG(pipe, REC_LOG,"任务记录文件更新至当前轮次 \n");
        write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    }
    TASK_FMT_DEBUG( pipe, REC_LOG, "LOAD 任务记录文件后 specificTaskIndex %d meterIndex %d meterSucNum %d 存储序号 %d\n",
        RunTask->specificTaskIndex,RunTask->meterIndex,RunTask->meterSucNum,RunTask->storageCnt);
    if(RunTask->result == SCHEME_FINISHED)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"任务 %d 当前轮次已经执行成功 \n",RunTask->taskid);
    }
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_record_file_write 
* 函数功能:写任务记录文件
* 输入参数: pipe 端口号
            TaskInfo 任务信息
            RunTask 任务运行时信息
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_trans_task_record_file_write(uint8 pipe,FUJIAN_TASK_RUN_T* RunTask)
{
    FUJIAN_TASK_RECORD_FILE_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(FUJIAN_TASK_RECORD_FILE_T));
    uint16 TaskId = RunTask->taskid;

    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%d",PrivateDir,TASK_RECORD_FILE,TaskId);

    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(FUJIAN_TASK_RECORD_FILE_T));
    
    TaskRecord.specificTaskIndex = RunTask->specificTaskIndex;
    TaskRecord.storageCnt = RunTask->storageCnt;
    TaskRecord.meterIndex = RunTask->meterIndex;
    TaskRecord.result = RunTask->result;
    TaskRecord.meterNum = RunTask->meterNum;
    memcpy(TaskRecord.meterInfo,RunTask->meterInfo,sizeof(FUJIAN_METER_INFO_T)*MAX_METER_NUM);
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(FUJIAN_TASK_RECORD_FILE_T));
}


/*******************************************************************************
* 函数名称: taskmng_specific_task_get_from_index
* 函数功能: 根据具体任务索引获取当前需要执行的具体任务指针
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_specific_task_get_from_index(FUJIAN_TASK_RUN_T * runTask,uint16 index,FUJIAN_SPECIFIC_TASK_T **specificTask)
{
    uint16 i;
    FUJIAN_SPECIFIC_TASK_T *tmp=runTask->specificTask;
    for(i =0;i<index;i++)
    {
        if(tmp == NULL)
            return -1;
        tmp = tmp->next;
    }
    if(NULL == tmp)
        return -1;
    *specificTask = tmp;
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trigger_flag_get
* 函数功能: 获取触发标记
* 输入参数: pipi 通道号
            runTask 任务方案
            
* 输出参数: 无
* 返 回 值: 0   
*******************************************************************************/
uint8 taskmng_trigger_flag_get(FUJIAN_TASK_RUN_T * runTask,OOP_TSA_T *tsa)
{
    uint16 i;
    uint8 flag = 0;
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(tsa,&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T))==0)
        {
            flag = runTask->meterInfo[i].triggerFlag;
            break;
        }

    }
    return flag;
}

/*******************************************************************************
* 函数名称: taskmng_fully_next_specific_get
* 函数功能: 判断这个表是否还有后续任务
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 
            0          有后续任务
*           <0         没有后续任务
*******************************************************************************/
int taskmng_fully_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **outSpecificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    TASK_FMT_DEBUG(pipe, REC_LOG,"specificTaskIndex %d \n",runTask->specificTaskIndex);
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&specificTask);
    if(ret<0)
    {
        return -1;
    }
    while(specificTask)
    {
        for(i =0;i<specificTask->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&specificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
        {
            *outSpecificTask = specificTask;
            return 0;
        }
        specificTask = specificTask->next;
        runTask->specificTaskIndex++;
    }
    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_fully_trans_task_next_meter_get
* 函数功能: 全透明转发 获取下一个需要抄表的具体任务和表 全透明任务改为按照表来抄 把一个表所有的具体任务抄完再
            下一个表
* 输入参数: pipi 通道号
            runTask 任务方案
            
* 输出参数: 无
* 返 回 值: 0   找到下一个要抄的表
*           1                全部抄完了
           
*******************************************************************************/

int taskmng_fully_trans_task_next_meter_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,OOP_TSA_T *outTsa,FUJIAN_SPECIFIC_TASK_T **SpecificTask)
{
    uint16 meterIndex = runTask->meterIndex;
    OOP_TSA_T tsa;
    int ret;
    //int index = -1;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    FUJIAN_SPECIFIC_TASK_T *tmp; 
    uint16 i;
    //根据当前任务消息获取发送的报文
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask);
    if(ret<0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"获取第%d个具体任务失败 \n",runTask->specificTaskIndex);
        runTask->result = SCHEME_FAIL;
        return 1;
    }
    while(1)
    {
        if(meterIndex >= runTask->meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"任务方案 %d 执行完毕 \n",runTask->taskid);
            if(runTask->failFlag == 1)
            {
                runTask->result = SCHEME_FAIL;
                TASK_FMT_DEBUG(pipe, REC_LOG,"任务状态失败 等待重启 \n");
            }else
            {
                runTask->result = SCHEME_FINISHED;
                TASK_FMT_DEBUG(pipe, REC_LOG,"任务全部执行成功 \n");
            }
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 1;
        }
        if(runTask->meterInfo[meterIndex].meterResult ==2 )
        {
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }
        memcpy(&tsa,&runTask->meterInfo[meterIndex].meterAddr,sizeof(OOP_TSA_T));
        //判断下哪些表被触发了
        if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
        {
            if(taskmng_trigger_flag_get(runTask,&tsa) == 0)
            {
                meterIndex++;
                runTask->meterIndex = meterIndex;
                runTask->specificTaskIndex = 0;
                continue;
            }
        }

        if(runTask->meterInfo[meterIndex].failCnt >runTask->transTask->turnRetryNum  )
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"本轮失败次数过多 %d 切表 表地址 %02x%02x%02x%02x%02x%02x\n",runTask->meterInfo[meterIndex].failCnt,
                tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
            runTask->failFlag = 1;
            //写一个失败的结果 成功后会更新结果
            //该表失败 记录下来 后面的具体任务也不抄了
            runTask->meterInfo[meterIndex].meterResult = 3;
            runTask->meterInfo[meterIndex].multiBufIndex = 0;
            runTask->meterInfo[meterIndex].execTime = time(NULL);
            //如果只失败了一次 就直接上报结果 如果直接读存储结果会读到上一次失败的结果 任务执行时间会不对

            ret = taskmng_trans_task_error_report(pipe, &tsa,runTask->meterInfo[meterIndex].execTime, runTask, currentSpecificTask);
            if(ret < 0)
            {
                taskmng_trans_task_result_save(pipe, 1, &tsa, runTask->meterInfo[meterIndex].execTime, runTask,
                     currentSpecificTask, OOP_FJ_ERR_RESQ_TIME_OUT, NULL, 0);
            }
            
            
           //把后面的具体任务里这块表的结果写未执行
            tmp = currentSpecificTask->next;
            while(tmp)
            {
                for(i=0;i<tmp->addrAll.num;i++)
                {
                    if(memcmp(&runTask->meterInfo[meterIndex].meterAddr,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                    {
                        taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        break;
                    }
                }
                tmp = tmp->next;
            }
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }

        //找到当前表要抄的具体任务
        ret = taskmng_fully_next_specific_get(pipe,runTask,&currentSpecificTask,meterIndex);
        if(ret < 0 )
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 抄读完毕 \n",tsa.add[0],tsa.add[1],
                tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);

            runTask->meterInfo[meterIndex].meterResult = 2;
            meterIndex++;
            runTask->meterIndex = meterIndex;
            runTask->specificTaskIndex = 0;
            continue;
        }else
        {
            //找到可以运行的表
            memcpy(outTsa,&tsa,sizeof(OOP_TSA_T));
            *SpecificTask = currentSpecificTask;
            
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 0;
        }

    }
}

/*******************************************************************************
* 函数名称: taskmng_fully_trans_task_send
* 函数功能: 全透明转发 发送
* 输入参数: pipi 通道号
            runTask 任务方案
            type 1表示376.2国网协议透明转发模式 2表示376.2福建增补协议V1.2透明转发模式
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_fully_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8 type)
{
    uint8 sendbuf[2000]={0};
    //uint8 recvbuf[2000]={0};
    uint16 sendlen;
    //uint16 recvlen = 0;
    uint8 dstbuf[2000]={0};
    int ret;
    //uint8 errorCode= 0;
    //uint8 result =0;
    OOP_TSA_T tsa;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    time_t timenow = time(NULL);
    uint16 meterindex ;
    uint8 i;
    uint16 index=0;
    FRAME_CONVERT_T convert;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    memset(&tsa,0x00,sizeof(OOP_TSA_T));


    ret = taskmng_fully_trans_task_next_meter_get(pipe,runTask,&tsa,&currentSpecificTask);
    if(ret == 1)
    {
        return 1;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG," 抄表地址 %02x%02x%02x%02x%02x%02x 具体任务号 %d\n",
        tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5],currentSpecificTask->nodeid);

    //终端本体端口的任务 如果是给交采的 逻辑地址填1 全部发送给puamr处理
    if(pipe == PLC_NUM + MAX_485_NUM + COMM_MODULE_NUM)
    {
        //地址用终端地址 以防具体任务里地址不对
        memcpy_r(tsa.add, LocalAddr, tsa.len + 1);
        if(runTask->transTask->commType == OOP_COMM_TYPE_AC)
        {
            tsa.oldvxd = 1;
        }
    }
    if(pipe >= PLC_NUM && pipe < PLC_NUM+MAX_485_NUM)
    {
        if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"串口初始化失败 \n");
            return -1;
        }
        //多帧645需要一个一个的发送
        meterindex = runTask->meterIndex;
        if(((currentSpecificTask->reqMessType>>4) & 0x0f) == 1  )
        {
            if(runTask->meterInfo[meterindex].multiBufIndex ==0)
            {
                convert.srcBuf = currentSpecificTask->reqMess.value;
                convert.srcLen = currentSpecificTask->reqMess.nNum;
                convert.dstBuf = dstbuf;
                Convert_Request_Frame(pipe,&convert,currentSpecificTask->reqMessType,&tsa);
                
                TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文转换前内容 \n");
                TASK_BUF_DEBUG(pipe,REC_LOG,currentSpecificTask->reqMess.value,currentSpecificTask->reqMess.nNum);
                TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 0x%02x 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文 645报文个数\n",currentSpecificTask->reqMessType,
                    tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5],convert.num645);
                TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);
                memset(&gMulti645Buf[pipe-PLC_NUM],0x00,sizeof(FUJIAN_MULTI_645BUF_T));
                gMulti645Buf[pipe-PLC_NUM].num = convert.num645;
                for(i=0;i<convert.num645;i++)
                {
                    gMulti645Buf[pipe-PLC_NUM].databuf[i].buflen = convert.datalen645[i];
                    memcpy(gMulti645Buf[pipe-PLC_NUM].databuf[i].buf,dstbuf+index,convert.datalen645[i]);
                    index += convert.datalen645[i];
                }
            }
        }
        else
        {
            convert.srcBuf = currentSpecificTask->reqMess.value;
            convert.srcLen = currentSpecificTask->reqMess.nNum;
            convert.dstBuf = dstbuf;
            Convert_Request_Frame(pipe, &convert, currentSpecificTask->reqMessType, &tsa);
            
            TASK_FMT_DEBUG(pipe, REC_LOG, "请求报文转换前内容 \n");
            TASK_BUF_DEBUG(pipe, REC_LOG, currentSpecificTask->reqMess.value, currentSpecificTask->reqMess.nNum);
            TASK_FMT_DEBUG(pipe, REC_LOG, "请求报文格式字 0x%02x 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文\n",
                 currentSpecificTask->reqMessType, 
                tsa.add[0], tsa.add[1], tsa.add[2], tsa.add[3], tsa.add[4], tsa.add[5]);
            TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);

        }
    }else
    {
    
        convert.srcBuf = currentSpecificTask->reqMess.value;
        convert.srcLen = currentSpecificTask->reqMess.nNum;
        convert.dstBuf = dstbuf;
        Convert_Request_Frame(pipe, &convert, currentSpecificTask->reqMessType, &tsa);
    
        TASK_FMT_DEBUG(pipe, REC_LOG, "请求报文转换前内容 \n");
        TASK_BUF_DEBUG(pipe, REC_LOG, currentSpecificTask->reqMess.value, currentSpecificTask->reqMess.nNum);
        TASK_FMT_DEBUG(pipe, REC_LOG, "请求报文格式字 0x%02x 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文\n",
             currentSpecificTask->reqMessType, 
            tsa.add[0], tsa.add[1], tsa.add[2], tsa.add[3], tsa.add[4], tsa.add[5]);
        TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);
    }
    //载波需要组一下消息
    if(pipe<PLC_NUM)
    {

        if(type == 1)
        {
            sendlen = fujian_send_payload_pack_0100(pipe, convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe, 0x0100, sendbuf, sendlen, &runTask->sendMsgIndex);
        }else
            {
            sendlen = fujian_send_payload_pack_0101(pipe, &tsa,convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe, 0x0101, sendbuf, sendlen, &runTask->sendMsgIndex);
        }
        
    }else if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
     {
        if(((currentSpecificTask->reqMessType>>4) & 0x0f) == 1  )
        {
            
            uint8 bufindex = runTask->meterInfo[meterindex].multiBufIndex;
            ret = fujian_trans_task_buf_send(pipe,0,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buf ,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buflen,&runTask->sendMsgIndex);
            TASK_FMT_DEBUG(pipe, REC_LOG, "多帧645抄读 发送 index %d 总共 %d \n",bufindex,gMulti645Buf[pipe-PLC_NUM].num);
            TASK_BUF_DEBUG(pipe, REC_LOG, gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buf ,gMulti645Buf[pipe-PLC_NUM].databuf[bufindex].buflen);
        }else
        {
            ret = fujian_trans_task_buf_send(pipe,0,convert.dstBuf,convert.dstLen,&runTask->sendMsgIndex);
        }
    }
    else if(pipe ==PLC_NUM+MAX_485_NUM )
    {       
        sendlen = fujian_send_payload_pack_0012(pipe, convert.dstBuf, convert.dstLen, sendbuf);
        ret = fujian_trans_task_buf_send(pipe,0x0012,sendbuf,sendlen,&runTask->sendMsgIndex);
    }
    else if(pipe ==PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM)
    {
        
        //最新 全部发送给puamr处理
        //感觉通信对象类型区分下发给终端本体的
        //if(runTask->transTask->commType == 0)
        //{
            //通过通用透传消息发送给puamr
            sendlen = fujian_send_payload_pack_0012(pipe, convert.dstBuf, convert.dstLen, sendbuf);
            ret = fujian_trans_task_buf_send(pipe,0x0012,sendbuf,sendlen,&runTask->sendMsgIndex);
       // }
//        else
//        {
//        
//            ret = taskManager_acMeter_revsnd(convert.dstBuf, convert.dstLen, recvbuf, &recvlen);
//        
//            if(ret != ERR_OK)
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG, "taskManager 获取交采数据失败 \n");
//                runTask->result = SCHEME_FAIL;
//                taskmng_trans_task_record_file_write(pipe, runTask);
//                return 1;
//            }
//            else
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG, "taskManager 获取交采数据成功 \n");
//        
//                //处理收到的报文 按照响应报文格式字解析
//                convert.srcBuf = recvbuf;
//                convert.srcLen = recvlen;
//                convert.dstBuf = dstbuf;
//                Convert_Response_Frame(pipe, &convert, currentSpecificTask->resMessType);
//        
//                TASK_FMT_DEBUG(pipe, REC_LOG, "接收报文转换前内容 \n");
//                TASK_BUF_DEBUG(pipe, REC_LOG, recvbuf, recvlen);
//                TASK_FMT_DEBUG(pipe, REC_LOG, "接收报文格式字 %d 转换后报文\n", currentSpecificTask->reqMessType);
//                TASK_BUF_DEBUG(pipe, REC_LOG, convert.dstBuf, convert.dstLen);
//        
//                /*获取本地地址为交采地址*/
//                tsa.len = 5;
//                memcpy(tsa.add, LocalAddr, tsa.len + 1);
//        
//                //报文验证
//                result = packet_verific_Ispass(pipe, &currentSpecificTask->resMessProc, &convert, runTask,
//                     currentSpecificTask, & tsa);
//                TASK_FMT_DEBUG(pipe, REC_LOG, "报文验证结果 %d \n", result);
//                errorCode = result + ERROR_SUCCESS_CODE_ADD;
//        
//                taskmng_trans_task_result_save(pipe, 1, &tsa, timenow, runTask, currentSpecificTask, errorCode,
//                     convert.dstBuf, convert.dstLen);
//        
//                taskmng_trans_task_collection_state_update(pipe, runTask, TRANS_TASK_SUCC, (void *) &timenow);
//        
//                //还有具体任务就继续跑 没有就任务结束
//                if(currentSpecificTask->next != NULL)
//                {
//                    runTask->specificTaskIndex++;
//                    taskmng_trans_task_record_file_write(pipe, runTask);
//                    return - 1;
//                }
//                else
//                {
//                    TASK_FMT_DEBUG(pipe, REC_LOG, "交采任务执行完毕\n");
//                    runTask->result = SCHEME_FINISHED;
//                    taskmng_trans_task_record_file_write(pipe, runTask);
//        
//                    return 1;
//        
//                }
//        
//            }
//        }
        
    }
    if(ret <0 )
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"报文发送失败 \n");
        return -1;
    }
    runTask->lastSendTime = timenow;
    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_notice_object_get
* 函数功能: 获取预告抄读对象
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 0 有预告抄读对象发送 
            -1 全部预告完了
*******************************************************************************/
int taskmng_notice_object_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 num = 0;
    OOP_TSA_T tsaList[255];
    uint16 i,j;
    int ret;
    memset(tsaList,0x00,sizeof(tsaList));
    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;

    if(runTask->noticeFailCnt > runTask->transTask->turnRetryNum)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"预告抄表失败次数过多 本轮不再尝试 \n");
        runTask->failFlag = 1;
        return -1;
    }

    for(i=0;i<runTask->meterNum;i++)
    {
        if(runTask->meterInfo[i].meterResult ==0 )
        {
            //如果是触发任务 再判断下
            if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
            {
                if(runTask->meterInfo[i].triggerFlag == 0)
                {
                    runTask->meterInfo[i].meterResult = 2;
                    continue;
                }
            }
            if(runTask->meterInfo[i].failCnt > runTask->transTask->turnRetryNum)
            {   
                OOP_TSA_T tsa;
                memcpy(&tsa,&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T));
                TASK_FMT_DEBUG(pipe, REC_LOG,"本轮失败次数过多 %d 不再进行预告 specificTaskIndex %d 电表地址 %02x%02x%02x%02x%02x%02x\n",runTask->meterInfo[i].failCnt,
                    runTask->meterInfo[i].specificTaskIndex,tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
                runTask->meterInfo[i].meterResult = 3;
                runTask->failFlag = 1;
                //给后面的任务写上任务未执行

                taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp);
                ret = taskmng_trans_task_error_report(pipe,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime,runTask,tmp);
                if(ret<0)
                {
                    taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
                }
                tmp = tmp->next;
                while(tmp)
                {
                    for(j=0;j<tmp->addrAll.num;j++)
                    {
                        if(memcmp(&runTask->meterInfo[i].meterAddr,&tmp->addrAll.addr[j],sizeof(OOP_TSA_T)) == 0)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"具体任务号 %d 未执行 \n",tmp->nodeid);
                            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                            break;
                        }
                    }
                    tmp = tmp->next;
                }
                continue;
            }
            runTask->meterInfo[i].meterResult = 1;
            //找到这个表第一个要发送的具体任务索引
            if(runTask->meterInfo[i].specificTaskIndex == 0)
            {
                tmp = runTask->specificTask;
                uint8 flag = 0;
                while(tmp)
                {
                    for(j=0;j<tmp->addrAll.num;j++)
                    {
                        if(memcmp(&runTask->meterInfo[i].meterAddr,&tmp->addrAll.addr[j],sizeof(OOP_TSA_T)) == 0)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 1)
                        break;
                    tmp = tmp->next;
                    runTask->meterInfo[i].specificTaskIndex++;
                }
            }

            
            memcpy(&tsaList[num],&runTask->meterInfo[i].meterAddr,sizeof(OOP_TSA_T));
            num++;
            if(num >= pAmrThreadArg->maxNoticeNum)
                break;
        }
    }
    if(num == 0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"所有对象全部预告完毕 \n");
        return -1;
    }

    TASK_FMT_DEBUG(pipe, REC_LOG,"本次预告对象个数 %d \n",num);
    //组预告抄读对象报文 0095
    uint8 payload[3000]={0};
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    uint16 index = 0;
    uint16 seq;

    uint8 lenoffset;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],&pipeoad.value,4);
    index+=4;
    memcpy_r(&payload[index],&num,2);
    index+=2;
    payload[index++] = 1;   //默认全部进行通信延时修正 等具体抄读的时候再看要不要加上延时
    lenoffset = set_len_offset(num,&payload[index]);
    index+=lenoffset;
    for(i=0;i<num;i++)
    {
        seq = i+1;
        memcpy_r(&payload[index],&seq,2);
        index+=2;
        payload[index++] = 3;       //电能表
        memcpy(&payload[index],tsaList[i].add,6);
        index+=6;
        TASK_FMT_DEBUG(pipe, REC_LOG,"i %d 预告的电表地址 0x%02x%02x%02x%02x%02x%02x\n",i,
            tsaList[i].add[0],tsaList[i].add[1],tsaList[i].add[2],tsaList[i].add[3],tsaList[i].add[4],tsaList[i].add[5]);
    }

    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0095,index,payload,&recvlen,recvbuf);
    if(ret <0 || recvbuf[0]==0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"预告抄读对象失败\n");
    }
    return 0;
    
}

/*******************************************************************************
* 函数名称: fujian_notice_task_start 
* 函数功能: 启动预告任务执行 0094
* 输入参数: pipe 端口号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint8 fujian_notice_task_start(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 payload[20]={0};
    uint8 len = 0;

    int ret;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = pAmrThreadArg->noticeMode;

    uint16 recvlen;
    uint8 recvbuf[100]={0};
    ret =taskmng_msg_fun_deal(pipe,0x0003,0x0094,len,payload,&recvlen,recvbuf);
    if(ret>0)
    {
        if(recvlen!=1)
            return 0;
        return recvbuf[0];
    }
    return 0;

}

/*******************************************************************************
* 函数名称: taskmng_notice_task_amr_info_ask
* 函数功能: 预告抄表 收到抄读内容请求后的处理
* 输入参数: pipi 通道号
            runTask 任务方案
            payload 消息内容
            payloadLen 消息长度
* 输出参数: 无
* 返 回 值: 
            0          发送内容
*           <0         失败
*******************************************************************************/
int taskmng_notice_task_amr_info_ask(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T * msgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 index = 0;
    uint8 addr[6];      //地址 大端
    uint16 delay;
    int meterIndex = -1;
    uint16 i;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    
    index+=4;
    uint8 commType = msgInfo->playload[index++];
    memcpy(addr,&msgInfo->playload[index],6);
    index+=6;
    memcpy_r(&delay,&msgInfo->playload[index],2);
    index+=2;
    TASK_FMT_TRACE( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 请求抄读内容 信道延时 %d \n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5],delay);
    //找表
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(addr,runTask->meterInfo[i].meterAddr.add,6) == 0)
        {
            meterIndex = i;
            break;
        }
    }
    if(meterIndex == -1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"未在任务中找到该表\n");
        return -1;
    }
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"获取具体任务失败 specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
        return -1;
    }
    convert.srcBuf = specificTask->reqMess.value;
    convert.srcLen = specificTask->reqMess.nNum;
    convert.pipedelay = delay;
    convert.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&runTask->meterInfo[meterIndex].meterAddr);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文转换前内容 具体任务号 %d\n",specificTask->nodeid);
    TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 0x%02x 转换后报文\n",specificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

    //组发送报文
    uint8 sendbuf[2000]={0};
    uint8 lenoffset;
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    memcpy_r(&sendbuf[index],&specificTask->transTaskid,2);
    index+=2;
    memcpy_r(&sendbuf[index],&specificTask->nodeid,2);
    index+=2;
    sendbuf[index++] = commType;
    memcpy(&sendbuf[index],addr,6);
    index+=6;
    sendbuf[index++] = convert.prtl;
    sendbuf[index++] = pAmrThreadArg->noticeMode;
    lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
    index+=lenoffset;
    memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
    index+=convert.dstLen;
    TASK_FMT_DEBUG(pipe, REC_LOG,"发送抄读内容 \n");
    fujian_trans_task_buf_ack(pipe,0x0104,sendbuf,index,msgInfo->index,msgInfo->label);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_notice_next_specific_get
* 函数功能: 判断这个表是否还有后续任务
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 
            0          有后续任务
*           <0         没有后续任务
*******************************************************************************/
int taskmng_notice_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *specificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    TASK_FMT_DEBUG(pipe, REC_LOG,"meterIndex %d specificTaskIndex %d \n",meterIndex,runTask->meterInfo[meterIndex].specificTaskIndex);
    FUJIAN_SPECIFIC_TASK_T *tmp = specificTask->next;
    runTask->meterInfo[meterIndex].specificTaskIndex++;
    while(tmp)
    {
        for(i =0;i<tmp->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
            break;
        tmp = tmp->next;
        runTask->meterInfo[meterIndex].specificTaskIndex++;
    }
    if(flag == 1)
        return 0;
    else
        return -1;
}

/*******************************************************************************
* 函数名称: taskmng_notice_task_data_recv
* 函数功能: 预告抄表 收到上报的通信对象数据后的处理
* 输入参数: pipi 通道号
            runTask 任务方案
            payload 消息内容
            payloadLen 消息长度
* 输出参数: 无
* 返 回 值: 
            0          发送内容
*           <0         失败
*******************************************************************************/
int taskmng_notice_task_data_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T* msgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 transTaskId,specificTaskId,datalen;
    uint8 commType,lenoffset;
    uint8 addr[6]={0};
    uint8 databuf[2000]={0};
    int meterIndex = -1;
    uint16 i;
    int ret;
    uint8 result = 0;
    uint8 errorCode = 0;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    index+=4;
    memcpy_r(&transTaskId,&msgInfo->playload[index],2);
    index+=2;
    memcpy_r(&specificTaskId,&msgInfo->playload[index],2);
    index+=2;
    commType = msgInfo->playload[index++];
    memcpy(addr,&msgInfo->playload[index],6);
    index+=6;
    datalen = get_len_offset(&msgInfo->playload[index], &lenoffset);
    index+=lenoffset;
    memcpy(databuf,&msgInfo->playload[index],datalen);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 上报数据 任务方案号%d 具体任务号 %d\n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5],transTaskId,specificTaskId);
    TASK_BUF_TRACE( pipe, REC_LOG,databuf,datalen);

    //找表
    for(i=0;i<runTask->meterNum;i++)
    {
        if(memcmp(addr,runTask->meterInfo[i].meterAddr.add,6) == 0)
        {
            meterIndex = i;
            break;
        }
    }
    if(meterIndex == -1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"未在任务中找到该表\n");
        return 0;
    }
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
        return 0;
    //会出现上报的任务号和当前抄的不一样的 这个时候返回0 会继续等待 要不然就会重新预告 然后导致任务异常结束
    if(specificTask->transTaskid!=transTaskId || specificTask->nodeid!=specificTaskId)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"收发的任务号不同\n");
        return 0;
    }
    uint8 state = 2;
    time_t timenow = time(NULL);
    if(datalen < specificTask->resMinLen )
    {
        //回复的报文小于最短有效长度认为抄表失败 再次预告
        //测试的时候故意把最短有效长度改为很长 这个时候错误码回报文校验错误
        state = 1;
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        runTask->meterInfo[meterIndex].meterResult = 0; //重新预告
        runTask->meterInfo[meterIndex].failCnt++;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,1);
        taskmng_trans_task_record_file_write(pipe,runTask);
        TASK_FMT_DEBUG(pipe, REC_LOG,"回码长度小于最短有效长度 %d 失败次数 %d\n",specificTask->resMinLen,runTask->meterInfo[meterIndex].failCnt);
    }else
    {
        //处理收到的报文 按照响应报文格式字解析
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,specificTask->resMessType);
        

        TASK_FMT_DEBUG(pipe, REC_LOG,"接收报文格式字 0x%02x 转换后报文\n",specificTask->resMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

        //报文验证
        result = packet_verific_Ispass(pipe,&specificTask->resMessProc ,&convert, runTask,specificTask,&runTask->meterInfo[meterIndex].meterAddr);
        TASK_FMT_DEBUG(pipe, REC_LOG,"报文验证结果 %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;

        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,0);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        ret = taskmng_notice_next_specific_get(pipe,runTask,specificTask,meterIndex);
        if(ret <0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"该表全部抄完 没有后续任务\n");
            state = 1;
            runTask->meterInfo[meterIndex].meterResult = 2;
            taskmng_trans_task_record_file_write(pipe,runTask);
        }else
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"该表还有后续任务 specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
        }
    }
    runTask->meterInfo[meterIndex].execTime = timenow;

    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime,runTask,specificTask,errorCode,convert.dstBuf,convert.dstLen);
    
    //组回码 判断还有没有下个具体任务需要抄 如果没有 则置该表成功
    

    uint8 sendbuf[2000]={0};
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    memcpy_r(&sendbuf[index],&specificTask->transTaskid,2);
    index+=2;
    memcpy_r(&sendbuf[index],&specificTask->nodeid,2);
    index+=2;
    sendbuf[index++] = commType;
    memcpy(&sendbuf[index],addr,6);
    index+=6;
    sendbuf[index++] = pAmrThreadArg->noticeMode;
    sendbuf[index++] = state;
    sendbuf[index++] = 0;
    TASK_FMT_DEBUG(pipe, REC_LOG,"上报数据确认回复 \n");
    fujian_trans_task_buf_ack(pipe,0x0105,sendbuf,index,msgInfo->index,msgInfo->label);
    //检查下当前预告的表是否都抄完了
    uint8 flag = 0;
    for(i = 0;i<runTask->meterNum;i++)
    {
        if(runTask->meterInfo[i].meterResult == 1)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"当前预告的对象全部抄完\n");
        return -1;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_notice_trans_task_recv
* 函数功能: 预告抄表 接收处理
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: >0                  继续在接受态
            0          正常切换到发送
*           <0               接收超时
*******************************************************************************/
int taskmng_notice_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    int ret;
    uint16 i;
    switch(runTask->noticeState)
    {
        //等待上报抄读内容请求 或者 上报预告的通信对象数据
        case NOTICE_WAIT:
            {
                recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
                if(recvlen>0)
                {
                    MsgInfo = (MESSAGE_INFO_T*)recvbuf;
                    TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
                    TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
                    runTask->lastNoticeRecvTime = time(NULL);
                    //抄读内容请求
                    if(MsgInfo->IOP == 0x0104)
                    {
                        taskmng_notice_task_amr_info_ask(pipe,runTask,MsgInfo);

                    }else if(MsgInfo->IOP == 0x0105)
                    {
                        ret = taskmng_notice_task_data_recv(pipe,runTask,MsgInfo);
                        if(ret <0 )
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"判断是否还有未预告的对象 \n");
                            runTask->noticeState = NOTICE_OBJECT;
                            return 0;
                        }
                    }
                }else
                {
                    //判断是否超时 如果启动预告后什么都没收到 则重新预告 
                    if(runTask->lastNoticeRecvTime==0)
                    {
                        if(time(NULL) - runTask->lastSendTime > 300)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"启动预告后超时未收到抄读内容请求 \n");
                            runTask->noticeFailCnt++;
                            runTask->noticeState = NOTICE_OBJECT;
                            for(i=0;i<runTask->meterNum;i++)
                            {
                                if(runTask->meterInfo[i].meterResult == 1)
                                {
                                    runTask->meterInfo[i].meterResult = 0;
                                    runTask->meterInfo[i].execTime = time(NULL);
                                    fujian_col_state_update(pipe,&runTask->meterInfo[i].meterAddr,1);
                                    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
                                    taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp);
                                    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_MODULE_EXCEPTION, NULL,0);
                                }
                            }
                            taskmng_trans_task_record_file_write(pipe,runTask);
                            return 0;
                        }
                    }else
                    {
                        if(time(NULL) - runTask->lastNoticeRecvTime > 120)
                        {
                            TASK_FMT_DEBUG(pipe, REC_LOG,"超时未收到任何上报数据 \n");
                            runTask->noticeFailCnt++;
                            //失败的再预告一次
                            for(i=0;i<runTask->meterNum;i++)
                            {
                                if(runTask->meterInfo[i].meterResult == 1)
                                {
                                    fujian_col_state_update(pipe,&runTask->meterInfo[i].meterAddr,1);
                                    runTask->meterInfo[i].meterResult = 0;
                                    runTask->meterInfo[i].execTime = time(NULL);
                                    FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
                                    if(taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[i].specificTaskIndex,&tmp)<0)
                                    {
                                        TASK_FMT_DEBUG(pipe, REC_LOG,"获取具体任务失败 i %d specificTaskIndex %d \n",i,runTask->meterInfo[i].specificTaskIndex);
                                    }else
                                    {
                                        taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[i].meterAddr,runTask->meterInfo[i].execTime, runTask,tmp,OOP_FJ_ERR_MODULE_EXCEPTION, NULL,0);
                                    }
                                }

                            }
                            runTask->noticeState = NOTICE_OBJECT;
                            taskmng_trans_task_record_file_write(pipe,runTask);
                            return 0;
                        }
                    }
                }
            }
            break;
    
        default:
            return 0;
            break;
    }
    
    return 1;
    
}


/*******************************************************************************
* 函数名称: taskmng_notice_trans_task_send
* 函数功能: 预告抄表 发送
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 1 任务执行完成
            0 有数据发送
*           <0                  不切换收发状态
*******************************************************************************/
int taskmng_notice_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    int ret;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 objectNum;

    switch(runTask->noticeState)
    {
        //查询本地模块参数 获取一次预告最多的对象数量 查询模块串口参数 判断新旧抄读模式
        case NOTICE_INIT:
            {
                objectNum = fujian_module_info_get(pipe);
                if(objectNum>0)
                {
                    pAmrThreadArg->maxNoticeNum = objectNum;
                    pAmrThreadArg->noticeMode = fujian_amr_mode_get(pipe);
                    TASK_FMT_DEBUG(pipe, REC_LOG,"最大允许的预告对象数量 %d 抄读模式 %d \n",objectNum,pAmrThreadArg->noticeMode );
                    //结束当前任务
                    //fujian_current_task_end(pipe);
                    runTask->noticeState = NOTICE_OBJECT;
                    return -1;
                }else
                    return -1;
            }
            break;
    
        case NOTICE_OBJECT:
            {
                //预告抄读对象
                //预告前结束下当前任务
                fujian_current_task_end(pipe);
                ret = taskmng_notice_object_get(pipe,runTask);
                if(ret<0)
                {
                    
                    if(runTask->failFlag == 1)
                    {
                        runTask->result = SCHEME_FAIL;
                        TASK_FMT_DEBUG(pipe, REC_LOG,"任务执行完毕 有失败 等重启\n");
                    }
                    else
                    {
                        runTask->result = SCHEME_FINISHED;
                        TASK_FMT_DEBUG(pipe, REC_LOG,"任务执行完毕 全部成功\n");
                    }
                    runTask->noticeState  = NOTICE_INIT;
                    taskmng_trans_task_record_file_write(pipe,runTask);
                    return 1;
                }else
                {
                    //启动预告任务执行
                    TASK_FMT_DEBUG(pipe, REC_LOG,"启动预告任务执行\n");
                    fujian_notice_task_start(pipe);
                    time_t timenow = time(NULL);
                    runTask->lastSendTime = timenow;
                    runTask->noticeState = NOTICE_WAIT;
                    runTask->lastNoticeRecvTime = 0;
                    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
                    return 0;
                }
            }
            break;

        default:
            runTask->noticeState = NOTICE_INIT;
            break;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_parallel_next_specific_get
* 函数功能: 判断这个表是否还有后续任务
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 
            0          有后续任务
*           <0         没有后续任务
*******************************************************************************/
int taskmng_parallel_next_specific_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **outSpecificTask,uint16 meterIndex)
{
    uint8 flag = 0;
    uint16 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    TASK_FMT_DEBUG(pipe, REC_LOG,"specificTaskIndex %d \n",runTask->meterInfo[meterIndex].specificTaskIndex);
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
    {
        return -1;
    }
    while(specificTask)
    {
        for(i =0;i<specificTask->addrAll.num;i++)
        {
            if(memcmp(&runTask->meterInfo[meterIndex].meterAddr ,&specificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 1)
        {
            *outSpecificTask = specificTask;
            return 0;
        }
        specificTask = specificTask->next;
        runTask->meterInfo[meterIndex].specificTaskIndex++;
    }
    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_parallel_next_meter_get
* 函数功能:  国网并发抄表 获取下一个发送的表和具体任务号
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 1 全部抄完
            0 有数据发送
*           <0    没数据发送 继续等待接受
*******************************************************************************/

int taskmng_parallel_next_meter_get(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T **specificTask,uint16 *index)
{
    int meterIndex = -1;
    int ret;
    uint16 i;
    while(meterIndex < runTask->meterNum-1)
    {
        meterIndex++;
        if(runTask->meterSucNum>=runTask->meterNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"并发抄表所有表全部抄读完毕\n");
            if(runTask->failFlag == 1)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"有失败表 等重启\n");
                runTask->result = SCHEME_FAIL;
            }else
            {
                runTask->result = SCHEME_FINISHED;
            }
            taskmng_trans_task_record_file_write(pipe,runTask);
            return 1;
        }
        //TASK_FMT_DEBUG(pipe, REC_LOG,"runTask->meterInfo[%d].meterResult %d\n",meterIndex,runTask->meterInfo[meterIndex].meterResult);
        if(runTask->meterInfo[meterIndex].meterResult!=0)
        {
            continue;
        }
        if(runTask->meterInfo[meterIndex].failCnt > runTask->transTask->turnRetryNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"表失败次数过多 %d \n",runTask->meterInfo[meterIndex].failCnt);
            FUJIAN_SPECIFIC_TASK_T * tmp = NULL;
            taskmng_specific_task_get_from_index(runTask, runTask->meterInfo[meterIndex].specificTaskIndex,&tmp);
            runTask->meterInfo[meterIndex].execTime = time(NULL);
            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
            runTask->meterInfo[meterIndex].meterResult = 3;
            runTask->meterSucNum++;
            runTask->failFlag = 1;
            taskmng_trans_task_record_file_write(pipe,runTask);
            //把后面的具体任务里这块表的结果写未执行
            tmp = tmp->next;
            while(tmp)
            {
                for(i=0;i<tmp->addrAll.num;i++)
                {
                    if(memcmp(&runTask->meterInfo[meterIndex].meterAddr,&tmp->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                    {
                        taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[meterIndex].meterAddr,runTask->meterInfo[meterIndex].execTime, runTask,tmp,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        break;
                    }
                }
                tmp = tmp->next;
            }
            continue;
        }
        if(runTask->transTask->taskTypeData.taskType == 5 && runTask->transTask->taskTypeData.addi_param.condition.object ==1)
        {
            if(runTask->meterInfo[meterIndex].triggerFlag ==0)
            {
                runTask->meterInfo[meterIndex].meterResult = 2;
                runTask->meterSucNum++;
                continue;
            }
        }
        //找到表后再找具体任务有没有这块表
        ret = taskmng_parallel_next_specific_get(pipe,runTask,specificTask,meterIndex);
        if(ret <0 )
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"该表所有具体任务全部抄读完毕\n");
            runTask->meterInfo[meterIndex].meterResult = 2;
            runTask->meterSucNum++;
            taskmng_trans_task_record_file_write(pipe,runTask);
            continue;
        }
        //找到要抄的表和具体任务
        runTask->meterInfo[meterIndex].meterResult = 1;
        *index = meterIndex;
        TASK_FMT_DEBUG(pipe, REC_LOG,"找到需要抄表的电表meterIndex %d specificTaskIndex %d 具体任务号 %d failCnt %d\n",meterIndex,
                    runTask->meterInfo[meterIndex].specificTaskIndex,(*specificTask)->nodeid,runTask->meterInfo[meterIndex].failCnt);
        return 0;
    }
    //没找到要发送的数据 等回码
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_parallel_trans_task_send
* 函数功能:  国网并发抄表 发送 记录下每个电表在每个具体任务里的成功状态
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: 1 任务执行完成
            0 有数据发送
*           <0                  不切换收发状态
*******************************************************************************/
int taskmng_parallel_trans_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    uint16 meterIndex;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000];
    OOP_TSA_T tsa;
    uint16 index;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe); 
    uint16 msgIndex;
    //7个并发
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            ret = taskmng_parallel_next_meter_get(pipe,runTask,&specificTask,&meterIndex);
            if(ret>0)
            {
                return 1;
            }else if(ret <0)
            {
                return 0;
            }else
            {
                memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
                memset(&dstbuf,0x00,sizeof(dstbuf));
                memcpy(&tsa,&runTask->meterInfo[meterIndex].meterAddr,sizeof(OOP_TSA_T));
                convert.srcBuf = specificTask->reqMess.value;
                convert.srcLen = specificTask->reqMess.nNum;
                convert.dstBuf = dstbuf;
                Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&runTask->meterInfo[meterIndex].meterAddr);
                TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文转换前内容 \n");
                TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
                TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 0x%02x 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文\n",specificTask->reqMessType,
                    tsa.add[0],tsa.add[1],tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
                TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

                //组发送报文
                uint8 sendbuf[2000]={0};
                uint8 lenoffset;
                index = 0;
                memcpy_r(&sendbuf[index],&pipeOAD.value,4);
                index+=4;
                index+=2;

                memcpy(&sendbuf[index],tsa.add,6);
                index+=6;
                lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
                index+=lenoffset;
                memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
                index+=convert.dstLen;
                fujian_trans_task_buf_send(pipe,0x003E,sendbuf,index,&msgIndex);
                TASK_FMT_DEBUG(pipe, REC_LOG,"并发抄表发送数据 消息索引 msgIndex %d \n",msgIndex);
                
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag=1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = meterIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,tsa.add,6);
            }
        }


    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_seq_is_valid
* 函数功能: 判断上行帧是否有效帧
* 输入参数: InSeq     需判断的电表号
* 输出参数: OutSeq    输出对应的帧索引
* 返 回 值: 1         有效
*           0         无效
*******************************************************************************/
uint8 taskmng_seq_is_valid_fujian(uint8 Pipe,uint8* OutSeq, uint8 *addr)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0; i < PARALLEL_QUEUE_NUM; i++)
    {
        //上行帧序号在所发有效帧之内

        if(( memcmp(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,addr,6)==0)
                && (pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1))
        {
            *OutSeq = i;
            return 1;
        }
    }

    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_parallel_task_data_recv
* 函数功能: 并发抄表收到数据后的处理
* 输入参数: pipi 通道号
            runTask 任务方案
            payload 消息内容
            payloadLen 消息长度
* 输出参数: 无
* 返 回 值: 
            0          数据处理成功 进入接受态
*           <0         失败
*******************************************************************************/
int taskmng_parallel_task_data_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8* payload,uint16 payloadLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 datalen;
    uint8 lenoffset;
    uint8 addr[6]={0};
    uint8 databuf[2000]={0};
    int meterIndex = -1;

    int ret;
    uint8 result = 0;
    uint8 errorCode = 0;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    uint8 seq;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    
    index+=4;
    memcpy(addr,&payload[index],6);
    index+=6;
    index++;    //协议类型
    datalen = get_len_offset(&payload[index], &lenoffset);
    index+=lenoffset;
    memcpy(databuf,&payload[index],datalen);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 抄表数据 \n",addr[0],addr[1],addr[2],addr[3],addr[4],
        addr[5]);
    TASK_BUF_TRACE( pipe, REC_LOG,databuf,datalen);

    if(taskmng_seq_is_valid_fujian(pipe,&seq,addr)==0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"未获取到发送队列索引 \n");
        return -1;
    }
    meterIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[seq].MsIndex;
    //获取抄读的具体任务
    ret = taskmng_specific_task_get_from_index(runTask,runTask->meterInfo[meterIndex].specificTaskIndex,&specificTask);
    if(ret<0)
        return -1;
    time_t timenow = time(NULL);
    if(datalen < specificTask->resMinLen )
    {
        //回复的报文小于最短有效长度认为抄表失败 再次预告
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);

        taskmng_parallel_task_meter_fail( pipe, seq,runTask);
        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,1);
        taskmng_trans_task_record_file_write(pipe,runTask);
        TASK_FMT_DEBUG(pipe, REC_LOG,"回码长度小于最短有效长度%d 失败次数 %d\n",specificTask->resMinLen,runTask->meterInfo[meterIndex].failCnt);
    }else
    {

        //处理收到的报文 按照响应报文格式字解析
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,specificTask->resMessType);
        

        TASK_FMT_DEBUG(pipe, REC_LOG,"接收报文格式字 0x%02x 转换后报文\n",specificTask->reqMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);

        //报文验证
        result = packet_verific_Ispass(pipe,&specificTask->resMessProc ,&convert, runTask,specificTask,&runTask->meterInfo[meterIndex].meterAddr);
        TASK_FMT_DEBUG(pipe, REC_LOG,"报文验证结果 %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;

        fujian_col_state_update(pipe,&runTask->meterInfo[meterIndex].meterAddr,0);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        runTask->meterInfo[meterIndex].specificTaskIndex++;
        runTask->meterInfo[meterIndex].meterResult = 0;
        pAmrThreadArg->AmrSendFrameInfo.Queue[seq].ValidFlag=0;
        if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
            pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
    }
    //存储数据 存储次数为0时不存储

    taskmng_trans_task_result_save(pipe,0,&runTask->meterInfo[meterIndex].meterAddr,timenow,runTask,specificTask,errorCode,convert.dstBuf,convert.dstLen);
    


    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_pn_state_refresh
* 函数功能: 刷新Pn的失败数据列表
* 输入参数: SeqIndex  帧序号
            MeterPrtlData 上报的645帧信息
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_parallel_task_meter_fail(uint8 Pipe,uint8 SeqIndex,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    uint16 MtrIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].MsIndex;

     pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].ValidFlag = 0;
    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;

    runTask->meterInfo[MtrIndex].failCnt++;
    runTask->meterInfo[MtrIndex].meterResult = 0;

    return;
}


/*******************************************************************************
* 函数名称: taskmng_parallel_trans_task_recv
* 函数功能: 并发抄表 接收处理
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: >0                  继续在接受态
            0          正常切换到发送
*           <0               接收超时
*******************************************************************************/

int taskmng_parallel_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    int ret;
    uint16 i;

    recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
    if(recvlen>0)
    {
        MsgInfo = (MESSAGE_INFO_T*)recvbuf;
        TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
        TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);

        //收到上报数据
        if(MsgInfo->IOP == 0x0031)
        {
            ret = taskmng_parallel_task_data_recv(pipe,runTask,MsgInfo->playload, MsgInfo->msglen);
            if(ret ==0 )
                return 0;
            else 
                return 1;
        }
    }
    else
    {
        uint8 flag = 0;
        for(i=0;i<PARALLEL_QUEUE_NUM;i++)
        {
            if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
            {
                flag =1;
                if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>90)
                {
                    TASK_FMT_DEBUG( pipe, REC_LOG, "Queue index %d 超时 meterindex %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex);
                    taskmng_parallel_task_meter_fail( pipe, i,runTask);
                    fujian_col_state_update(pipe,&runTask->meterInfo[pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex].meterAddr,0);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                }
            }
        }
        if(flag==0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "发送队列无效 切换到发送 \n");
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }

    }
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_fully_trans_task_recv
* 函数功能: 全透明转发 接受
* 输入参数: pipi 通道号
            runTask 任务方案
            type 1表示376.2国网协议透明转发模式 2表示376.2福建增补协议V1.2透明转发模式
* 输出参数: 无
* 返 回 值: >0                  继续在接受态
            0          正常切换到发送
*           <0               接收超时
*******************************************************************************/

int taskmng_fully_trans_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,uint8 type)
{
    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    uint16 overtime = 90;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8 errorCode= 0;
    uint8 databuf[5000]={0};
    uint16 datalen = 0;
    uint8 dstbuf[5000]={0};
    int ret;
    uint8 result;
    OOP_TSA_T tsa;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    FRAME_CONVERT_T convert;
    memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
        
    uint16 meterindex = runTask->meterIndex;
    //根据当前任务消息获取发送的报文
    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask);
    if(ret<0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"获取第%d个具体任务失败 \n",runTask->specificTaskIndex);
        runTask->result = SCHEME_FAIL;
        return -1;
    }
    //memcpy(&tsa,&currentSpecificTask->addrAll.addr[meterindex],sizeof(OOP_TSA_T));
    memcpy(&tsa,&runTask->meterInfo[meterindex].meterAddr,sizeof(OOP_TSA_T));
    if(pipe<PLC_NUM)
    {
        recvlen = taskmng_plc_queue_read(pAmrRecvDown[pipe], recvbuf);
        if(recvlen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)recvbuf;
            TASK_FMT_TRACE( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
            TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            if(MsgInfo->msglen != runTask->sendMsgIndex)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "消息序号不对 发送的是 %d \n",runTask->sendMsgIndex);
                return 1;
            }
            if(type == 1)
            {
                datalen = fujian_recv_payload_unpack_0100(MsgInfo->playload,MsgInfo->msglen,databuf);
            }else
                {
                datalen = fujian_recv_payload_unpack_0101(MsgInfo->playload,MsgInfo->msglen,databuf);
            }
            if(datalen==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "收到报文为空 \n");
                fujian_col_state_update(pipe,&tsa,1);
                //runTask->thisTurnRetryCnt++;
                runTask->meterInfo[meterindex].failCnt++;
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                return 0;
            }
            
        }else
        {
            if(time(NULL) - runTask->lastSendTime > overtime)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"任务 %d 具体任务%d 超时\n",runTask->taskid,currentSpecificTask->nodeid);
                fujian_col_state_update(pipe,&tsa,1);
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return -1;
            }
            return 1;
        }
    }else if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        datalen = fujian_rs485_recv(pipe,3,&currentSpecificTask->uartParam,databuf,sizeof(databuf));
        if(datalen == 0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "485超时 \n");
            fujian_col_state_update(pipe,&tsa,1);
            //runTask->thisTurnRetryCnt++;
            //currentSpecificTask->addrAll.retryCnt[meterindex]++;
            runTask->meterInfo[meterindex].failCnt++;
            return 0;
        }
    }else                                                               // 通信模块
    {
        recvlen = taskmng_plc_queue_read(pAmrRecvDown[pipe], recvbuf);
        if(recvlen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)recvbuf;
            TASK_FMT_DEBUG( pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
            TASK_BUF_DEBUG( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            if(MsgInfo->index != runTask->sendMsgIndex)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "消息序号不对 发送的是 %d \n",runTask->sendMsgIndex);
                return 1;
            }
            datalen=fujian_recv_payload_unpack_0012(MsgInfo->playload,MsgInfo->msglen,databuf);
            if(datalen==0)
            {
                TASK_FMT_DEBUG( pipe, REC_LOG, "收到报文为空 \n");
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return 0;
            }
            
        }else
        {
            if(time(NULL) - runTask->lastSendTime > 10)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"任务 %d 具体任务%d 超时\n",runTask->taskid,currentSpecificTask->nodeid);
                //currentSpecificTask->addrAll.retryCnt[meterindex]++;
                runTask->meterInfo[meterindex].failCnt++;
                return -1;
            }
            return 1;
        }
    }
    //645多帧需要特殊处理
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM && ((currentSpecificTask->reqMessType>>4) & 0x0f) == 1)
    {
        if(runTask->meterInfo[meterindex].multiBufIndex == 0)
        {
            memset(&gMulti645Recv[pipe-PLC_NUM],0x00,sizeof(FUJIAN_MULTI_645_RECV_T));
        }
        TASK_FMT_DEBUG(pipe, REC_LOG,"多帧645收到报文 multiBufIndex %d \n",runTask->meterInfo[meterindex].multiBufIndex);
        TASK_BUF_DEBUG(pipe,REC_LOG,databuf,datalen);
        
        memcpy(gMulti645Recv[pipe-PLC_NUM].recvbuf+gMulti645Recv[pipe-PLC_NUM].len,databuf,datalen);
        gMulti645Recv[pipe-PLC_NUM].len += datalen;

        if(runTask->meterInfo[meterindex].multiBufIndex >= gMulti645Buf[pipe-PLC_NUM].num-1)
        {
            //一个具体任务的所有多帧645全部抄完了 继续走下去
            memcpy(databuf,gMulti645Recv[pipe-PLC_NUM].recvbuf,gMulti645Recv[pipe-PLC_NUM].len );
            datalen = gMulti645Recv[pipe-PLC_NUM].len ;
        }else
        {
            runTask->meterInfo[meterindex].multiBufIndex++;
            return 0;
        }
    }
    
    time_t timenow = time(NULL);
    if(datalen < currentSpecificTask->resMinLen  )  
    {
        TASK_FMT_DEBUG(pipe, REC_LOG,"回码长度小于最短有效长度%d\n",currentSpecificTask->resMinLen);
        //errorCode = OOP_FJ_ERR_MODULE_EXCEPTION;
        if(datalen == 1)
        {
            errorCode = OOP_FJ_ERR_RESQ_TIME_OUT;
        }else
        {
            errorCode = OOP_FJ_ERR_MESS_CHECK;
        }
        convert.dstBuf = dstbuf;
        memcpy(convert.dstBuf,databuf,datalen);
        convert.dstLen = datalen;
        //currentSpecificTask->addrAll.retryCnt[meterindex]++;
        runTask->meterInfo[meterindex].failCnt++;
        fujian_col_state_update(pipe,&tsa,1);
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    } 
    else
    {

        TASK_FMT_DEBUG(pipe, REC_LOG,"接收报文转换前内容 \n");
        TASK_BUF_DEBUG(pipe,REC_LOG,databuf,datalen);
        //处理收到的报文 按照响应报文格式字解析
        convert.srcBuf = databuf;
        convert.srcLen = datalen;
        convert.dstBuf = dstbuf;
        Convert_Response_Frame(pipe,&convert,currentSpecificTask->resMessType);
        
        TASK_FMT_DEBUG(pipe, REC_LOG,"接收报文格式字 0x%02x 转换后报文\n",currentSpecificTask->resMessType);
        TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);


        //报文验证
        result = packet_verific_Ispass(pipe,&currentSpecificTask->resMessProc ,&convert, runTask,currentSpecificTask,&tsa);

        TASK_FMT_DEBUG(pipe, REC_LOG,"报文验证结果 %d \n",result);
        errorCode = result+ERROR_SUCCESS_CODE_ADD;
        
        fujian_col_state_update(pipe,&tsa,0);
        //currentSpecificTask->addrAll.result[meterindex] = 1;
        if(currentSpecificTask->next != NULL)
        {
            runTask->specificTaskIndex++;
            runTask->meterInfo[meterindex].multiBufIndex = 0;
        }else
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "电表 %02x%02x%02x%02x%02x%02x 抄读完毕 \n",tsa.add[0],tsa.add[1],
                tsa.add[2],tsa.add[3],tsa.add[4],tsa.add[5]);
            runTask->meterInfo[meterindex].meterResult = 2;
            runTask->meterInfo[meterindex].multiBufIndex = 0;
            runTask->meterIndex++;
            runTask->specificTaskIndex = 0;
        }
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,(void*)&timenow);
        
        taskmng_trans_task_record_file_write(pipe,runTask);
    }

    taskmng_trans_task_result_save(pipe,0,&tsa,timenow,runTask,currentSpecificTask,errorCode,convert.dstBuf,convert.dstLen);

    
    return 0;

}


/*******************************************************************************
* 函数名称: taskmng_trans_task_send
* 函数功能: 定时任务数据发送
* 输入参数: pipi 通道号
            runTask 任务方案
* 输出参数: 无
* 返 回 值: 1 任务执行完成
            0 有数据发送
*           <0                  失败
*******************************************************************************/
int taskmng_scheduled_task_send(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    
    OOP_TRANS_TASK_T * transTask = runTask->transTask;
    //485只支持 全透明转发和透明信道搜表
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        if(transTask->packType!=0 && transTask->packType!=9)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"当前端口不支持的协议封拆包方式 %d \n",transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    if(pipe==PLC_NUM+MAX_485_NUM )                   //和无线拨号app交互的端口 3
    {
        if(transTask->packType!=0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"当前端口%d 不支持的协议封拆包方式 %d \n",pipe,transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    if(pipe==PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM)                //交采端口
    {
        if(transTask->packType!=0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG,"当前端口交采 不支持的协议封拆包方式 %d \n",transTask->packType);
            runTask->result = SCHEME_FAIL;
            return -1;
        }
    }
    
    
    switch(transTask->packType)
    {
        case 0: //全透明转发
        case 1: //376.2国网协议透明转发模式 AFN=02H_F1H
            return taskmng_fully_trans_task_send(pipe, runTask, 1);
    
        case 2: //376.2福建增补协议V1.2透明转发模式（AFN=52H_F1H）
            return taskmng_fully_trans_task_send(pipe, runTask, 2);
    
        case 3: //376.2福建增补协议V1.2任务预告抄表(广播)方式
            return taskmng_notice_trans_task_send(pipe, runTask);

        case 4: //376.2 国网并发抄表方式
            return taskmng_parallel_trans_task_send(pipe, runTask);
        default:
            break;
    }
    
    
    return - 1;
    
}

/*******************************************************************************
* 函数名称: taskmng_scheduled_task_recv
* 函数功能: 定时任务数据接受处理
* 输入参数: pipi 通道号
            runTask 任务方案
* 输出参数: 无
* 返 回 值:  >0                  继续在接受态
            0          正常切换到发送
*           <0               接收超时
*******************************************************************************/
int taskmng_scheduled_task_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{

    OOP_TRANS_TASK_T * transTask = runTask->transTask;

    switch(transTask->packType)
    {
        case 0: //全透明转发
        case 1: //376.2国网协议透明转发模式 AFN=02H_F1H
            return taskmng_fully_trans_task_recv(pipe, runTask, 1);
    
        case 2: //376.2福建增补协议V1.2透明转发模式（AFN=52H_F1H）
            return taskmng_fully_trans_task_recv(pipe, runTask, 2);
    
        case 3: //376.2福建增补协议V1.2任务预告抄表(广播)方式
            return taskmng_notice_trans_task_recv(pipe, runTask);

        case 4://376.2 国网并发抄表方式
            return taskmng_parallel_trans_task_recv(pipe,runTask);
        default:
            break;
    }
    
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_scheduled_task_deal
* 函数功能: 定时任务的执行逻辑
* 输入参数: pipi 通道号
            runTask 任务方案
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_scheduled_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    FUJIAN_SPECIFIC_TASK_T *currentSpecificTask = NULL;
    uint16 i;
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
        case RTS_IDLE:
            {
                taskmng_pipe_collection_state_write(pipe,runTask->transTask->taskTypeData.taskType,runTask->taskid,runTask->priority);
                taskmng_trans_task_record_file_load(pipe,runTask);
                taskmng_comm_recv_buff_clear(pipe);
                if(pipe<PLC_NUM)
                {
                    fujian_current_task_end(pipe);
                    memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
                    if(runTask->transTask->taskTypeData.taskType == 3)
                    {
                        runTask->noticeState = NOTICE_INIT;
                        //预告任务重新执行时把每个表的状态都清一下 以防任务被打断重新执行后有问题
                        for(i=0;i<runTask->meterNum;i++)
                        {
                            if(runTask->meterInfo[i].meterResult == 1)
                            {
                                runTask->meterInfo[i].meterResult = 0;
                            }
                        }
                    }
                }
                if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_485_SET;
                }
                else
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                }
            }
            break;
        case RTS_485_SET:
            {
                ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&currentSpecificTask);
                if(ret<0)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"获取第%d个具体任务失败 \n",runTask->specificTaskIndex);
                    runTask->result = SCHEME_FAIL;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                    return -1;
                }
                if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"串口初始化失败 \n");
                    return -1;
                }
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
            break;
        case RTS_SEND:
            {
                ret = taskmng_scheduled_task_send(pipe,runTask);
                if(ret == 1)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"全局透明任务方案 %d 执行完成 \n",runTask->taskid);
                    fujian_task_fail_addr_write(pipe,runTask);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                    return -1;
                }else if(ret == 0)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
                }else if(ret == -2)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_485_SET;
                }else
                {
                    //返回-1时不切换收发态
                    return -1;
                }
            }
            break;
    
        case RTS_RECV:
            {
                ret = taskmng_scheduled_task_recv(pipe,runTask);
                if(ret<=0)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

                }
            }
            break;
    
        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            break;
    }
    
    return - 1;
    
}

/*******************************************************************************
* 函数名称: get_channel_delay_revise
* 函数功能: 根据请求报文格式字判断是否需要修正信道时间
* 输入参数: 
                resMess        具体任务的报文格式字
* 输出参数: 无
* 返 回 值: 1                    存在
*           0                  不存在
*******************************************************************************/

BOOL get_channel_delay_revise(uint8 reqMessType,OOP_OCTETVAR2K_T* resMess)
{
    uint8 framelen,tmpresType;
    uint16 delayrevise = 0,apdulen = 0;
    tmpresType = reqMessType;
    switch((tmpresType>>4)&0x0f)
    {
        case 0:
        break;
        case 1:
        {
            if(((tmpresType)&(0x0F))==1 || ((tmpresType)&(0x0F))==2)
            {
                framelen = resMess->value[0];
                if(resMess->nNum != framelen + 3)
                {
                    return FALSE;
                }
                else
                {
                    memcpy_r(&delayrevise,&resMess->value[1+framelen],2);
                }
            }
            else if(((tmpresType)&(0x0F))==4 || ((tmpresType)&(0x0F))==5)
            {
                framelen = resMess->value[1]; 
                if(resMess->nNum != framelen + 4)
                {
                    return FALSE;
                }
                else 
                {
                    memcpy_r(&delayrevise,&resMess->value[2+framelen],2);
                }
            }
        }break;
            /*类型2 不支持3761 类型 3 4没有修正值*/
        case 5:
        {
            if(((tmpresType)&(0x0F))==2)
            {
                memcpy_r(&apdulen,&resMess->value[2],2);
                if(resMess->nNum != apdulen + 6)
                {
                    return FALSE;
                } 
                else
                {
                    memcpy_r(&delayrevise,&resMess->value[4+apdulen],2);
                }
            }
            else if(((tmpresType)&(0x0F))==3)
            {
                    OOP_TSA_T tsa;
                    tsa.af = resMess->value[1];
                  
                    memcpy_r(&apdulen,&resMess->value[tsa.len+3],2);
                    if(resMess->nNum != apdulen + 7 +  tsa.len)
                    {
                        return FALSE;
                    }
                    else
                    {
                        memcpy_r(&delayrevise,&resMess->value[5+apdulen+tsa.len],2);
                    }
            }
        }break;
        default:
        break;
    }
    
    return delayrevise & 0x4000;
}
void task_revise_mess_send(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    FRAME_CONVERT_T *frame = NULL;
    uint8 index = 0;
    uint32 tmplen = frame->dstLen;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    if(tmplen<0x80)
    {
        SendBuffer[index++] = tmplen;
    }
    else
    {
        SendBuffer[index++] = 0x82;
        SendBuffer[index++] = (uint8)(tmplen>>8);
        SendBuffer[index++] = (uint8)tmplen;
    }
    memcpy(&SendBuffer[index],frame->dstBuf,tmplen);
    index+=tmplen;
    SendLen = index;
    taskmng_msg_data_pack(pipe,0x0003,0x0106,SendLen,SendBuffer,&runTask->sendMsgIndex);
}

void task_broad_finish_reply(uint8 pipe,FUJIAN_TASK_RUN_T *runTask)
{
    uint8 index = 0;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    SendLen = index;
    taskmng_msg_data_pack(pipe,0x0003,0x0107,SendLen,SendBuffer,&runTask->sendMsgIndex);;
}

/*需要修正的报文下发*/
void Broad_Timing_Send_abnomal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T  *currentSpecificTask)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    switch(pAmrThreadArg->BroadTask.state)
    {
        case BROAD_TASK_REVISE_MESS:
        {
            task_revise_mess_send(pipe,runTask);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RECV;
        }
        break;
        case BROAD_TASK_REPORT_FINISH:
        {
            task_broad_finish_reply(pipe,runTask);
//            /*存储一条成功的透明纪录*/
//            if(runTask->storageCnt>0)
//            {
//                taskmng_trans_task_result_save(pipe,runTask,currentSpecificTask,OOP_FJ_ERR_SUCCESS, NULL,0);
//            }
            runTask->specificTaskIndex++;   /*当前任务指针++*/
        }
        break;
        default:
        break;
    }

}


uint8 Broad_Timing_Send_nomal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask)
{
    uint8 ret;
    FRAME_CONVERT_T frame;
    uint8 dstbuf[1000] = {0};
    uint8 index = 0;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0,RecvLen = 0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    OOP_TSA_T tsa;
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy_r(SendBuffer,&pipe_oad.value,4);
    index+=4;
    SendBuffer[index++] = runTask->transTask->commType;
    SendBuffer[index++] = runTask->transTask->taskTypeData.addi_param.broadcast/60;        //分钟    
    memset(&frame,0x00,sizeof(FRAME_CONVERT_T));
    ret = fujian_broadcast_tsa_get(currentSpecificTask->reqMessType,&tsa);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"广播任务不支持的请求格式字 %d ",currentSpecificTask->reqMessType);
        return -1;
    }
    frame.srcLen = currentSpecificTask->reqMess.nNum;
    frame.srcBuf = currentSpecificTask->reqMess.value;
    frame.pipedelay = 0;
    frame.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&frame,currentSpecificTask->reqMessType,&tsa);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文转换前内容 \n");
    TASK_BUF_DEBUG(pipe,REC_LOG,currentSpecificTask->reqMess.value,currentSpecificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 0x%02x 转换后报文\n",currentSpecificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,frame.dstBuf,frame.dstLen);
    if(pipe>=PLC_NUM)
    {
        //串口发送
        TASK_FMT_DEBUG(pipe, REC_LOG,"485广播报文发送 \n");
        fujian_rs485_send(pipe,3,frame.dstBuf,frame.dstLen);
        return 0;
    }
    if (frame.dstLen < 0x80)
    {
        SendBuffer[index++] = frame.dstLen;
    }
    else
    {
        SendBuffer[index++] = 0x82;
        SendBuffer[index++] = (uint8)(frame.dstLen >> 8);
        SendBuffer[index++] = (uint8)frame.dstLen;
    }
    memcpy(&SendBuffer[index],frame.dstBuf,frame.dstLen);
    index+=frame.dstLen;
    SendLen = index;
    ret = taskmng_msg_fun_deal(pipe,0x0003,0x0091, SendLen, SendBuffer, &RecvLen, RecvBuffer);
    if(ret > 0)
    {
         if(RecvLen==1&&RecvBuffer[0]==1)
         {
            return 1;
         }
    }
    return -1;
 
}

void Broad_Timing_Send(uint8 pipe,FRAME_CONVERT_T *frame,uint16 MsgIndex,uint16 IOP)
{
    uint8 payload[200]={0};
    uint16 index = 0;
    uint8 tmplen = frame->dstLen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    if(tmplen<0x80)
    {
        payload[index++] = tmplen;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(tmplen>>8);
        payload[index++] = (uint8)tmplen;
    }
    memcpy(&payload[index],frame->dstBuf,tmplen);
    index+=tmplen;
    taskmng_msg_data_pack(pipe,0x0003,IOP,index,payload,&MsgIndex);
}

void Broad_Timing_Send_NoDelay(uint8 pipe,FRAME_CONVERT_T *frame,uint16 MsgIndex,uint16 IOP,uint8 commtype,uint16 runtime )
{
    uint8 payload[200]={0};
    uint16 index = 0;
    uint8 tmplen = frame->dstLen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = commtype;
    payload[index++] = runtime%60;
    if(tmplen<0x80)
    {
        payload[index++] = tmplen; 
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(tmplen>>8);
        payload[index++] = (uint8)tmplen;
    }
    memcpy(&payload[index],frame->dstBuf,tmplen);
    index+=tmplen;
    taskmng_msg_data_pack(pipe,0x0003,IOP,index,payload,&MsgIndex);
}

uint8 get_broads_delay(uint8 pipe,uint8 commtype,uint8 runtime)
{
    uint16 IID = 0x0003;
    uint16 IOP; 
    uint8 index = 0;
    IOP = 0x0090;
    OOP_OAD_U pipe_oad;
    uint16 SendLen=0,RecvLen = 0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    memset(SendBuffer,0x00,MAX_MSG_LEN);
    pipe_oad = taskmng_pipe_oad_get(pipe);
    memcpy_r(SendBuffer,&pipe_oad.value,SendLen);
    index+=4;
    SendBuffer[index++] = commtype;
    SendBuffer[index++] = runtime/60;        //分钟
    SendLen = index;
    return taskmng_msg_fun_deal(pipe,IID,IOP, SendLen, SendBuffer, &RecvLen, RecvBuffer);
}

/*******************************************************************************
* 函数名称: taskmng_broadcast_delay_report_deal
* 函数功能: 收到上报广播延时报文后的处理
* 输入参数: pipi 通道号
            runTask 任务方案
            payload 消息内容
            payloadLen 消息长度
* 输出参数: 无
* 返 回 值: 
            0          发送内容
*           <0         失败
*******************************************************************************/
int taskmng_broadcast_delay_report_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T* msgInfo)
{

    uint8 lenoffset;
    int ret;
    uint16 index;
    FUJIAN_SPECIFIC_TASK_T *specificTask = NULL;
    FRAME_CONVERT_T convert;
    uint8 dstbuf[2000]={0};
    uint16 delay = 0;
    OOP_TSA_T tsa;
    memcpy_r(&delay,&msgInfo->playload[4],2);
    TASK_FMT_TRACE( pipe, REC_LOG,"上报的广播延时为%d (单位10ms)\n",delay);

    //组回码

    ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&specificTask);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"未找到具体任务 specificTaskIndex %d ",runTask->specificTaskIndex);
        return -1;
    }
    ret = fujian_broadcast_tsa_get(specificTask->reqMessType,&tsa);
    if(ret<0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"广播任务不支持的请求格式字 %d ",specificTask->reqMessType);
            return -1;

    }
    convert.srcBuf = specificTask->reqMess.value;
    convert.srcLen = specificTask->reqMess.nNum;
    convert.pipedelay = delay;
    convert.dstBuf = dstbuf;
    Convert_Request_Frame(pipe,&convert,specificTask->reqMessType,&tsa);
    
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文转换前内容 \n");
    TASK_BUF_DEBUG(pipe,REC_LOG,specificTask->reqMess.value,specificTask->reqMess.nNum);
    TASK_FMT_DEBUG(pipe, REC_LOG,"请求报文格式字 0x%02x 转换后报文\n",specificTask->reqMessType);
    TASK_BUF_DEBUG(pipe,REC_LOG,convert.dstBuf,convert.dstLen);
    
    //组回码 判断还有没有下个具体任务需要抄 如果没有 则置该表成功
    uint8 sendbuf[2000]={0};
    index = 0;
    memcpy(&sendbuf[index],msgInfo->playload,4);
    index+=4;
    lenoffset = set_len_offset(convert.dstLen, &sendbuf[index]);
    index+=lenoffset;
    memcpy(&sendbuf[index],convert.dstBuf,convert.dstLen);
    index+=convert.dstLen;

    TASK_FMT_DEBUG(pipe, REC_LOG,"上报广播延时回复 \n");
    fujian_trans_task_buf_ack(pipe,0x0106,sendbuf,index,msgInfo->index,msgInfo->label);
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_broadcast_task_deal
* 函数功能: 广播任务的执行逻辑
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_broadcast_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    if(pipe>PLC_NUM+MAX_485_NUM)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播任务只支持载波和485端口\n");
        runTask->result = SCHEME_FINISHED;
        return -1;
    }
    /*pipe问题先不管,先载波*/
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    uint16 RecvLen = 0;
    uint8 RecvBuffer[MAX_MSG_LEN]={0};
    OOP_OAD_U pipe_oad;
    memset(&pipe_oad,0x00,sizeof(OOP_OAD_U));
    MESSAGE_INFO_T* MsgInfo =NULL; 
    FUJIAN_SPECIFIC_TASK_T * currentSpecificTask = NULL;

    if(pAmrThreadArg->TotalState.NormalAmr == RTS_IDLE)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播任务 %d 开始执行\n",runTask->taskid);
        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        memset(&pAmrThreadArg->BroadTask,0x00,sizeof(BROAD_TASK_T));
    }
    if(pAmrThreadArg->BroadTask.FailCnt > runTask->transTask->turnRetryNum)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "广播任务 %d 失败次数过多 本轮不再执行\n",runTask->taskid);
        runTask->result = SCHEME_FAIL;
        return -1;
    }
    switch(pAmrThreadArg->TotalState.SyncTimeState)
    {
        case INNER_RTS_PIPE_INIT:
        {
            taskmng_pipe_collection_state_write(pipe,TASK_BROADCAST,runTask->taskid,runTask->priority);
            if(pipe < PLC_NUM)
            {
                if(!fujian_current_task_end(pipe))
                {
                    break;
                }
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
            }else
            {
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_485_INIT;
            }
            
            
        }break;
        case INNER_RTS_485_INIT:
        {
            ret = taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex,&currentSpecificTask);
            if(ret<0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"广播任务执行完毕 \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FINISHED;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                time_t timenow = time(NULL);
                taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                return -1;
            }
            if(fujian_rs485_init(pipe,&currentSpecificTask->uartParam)<0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"串口初始化失败 \n");
                return -1;
            }
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
        }
            break;
        case INNER_RTS_BROASTDELAY:
        {
            /*获取当前的具体任务指针*/
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask) == 0)
            {
                if(get_channel_delay_revise(currentSpecificTask->reqMessType,&currentSpecificTask->reqMess))
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"获取广播延时 具体任务 %d \n",currentSpecificTask->nodeid);
                    pAmrThreadArg->BroadTask.GET_DELAY = TRUE;
                    ret = get_broads_delay(pipe,runTask->transTask->commType,runTask->transTask->taskTypeData.addi_param.broadcast);
                    if(ret <0)
                    {
                        runTask->result = SCHEME_FAIL; 
                        TASK_FMT_DEBUG(pipe, REC_LOG,"获取广播延时失败 \n");
                        break;
                    }
                    else if(ret >0)
                    {
                        pAmrThreadArg->BroadTask.sendTime = time(NULL);
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_DELAY_WAIT;
                    }
                }
                else
                {
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
                    pAmrThreadArg->BroadTask.GET_DELAY = FALSE;
                }
            }
            else
            {
                 TASK_FMT_DEBUG(pipe, REC_LOG,"广播任务执行完毕 \n");
                 pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                 runTask->result = SCHEME_FINISHED;
                 pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                 time_t timenow = time(NULL);
                 taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                 return -1;
            }

            /*广播任务执行时间设置为任务类别广播里的每帧间隔时间*/
        }break;
        case INNER_RTS_DELAY_WAIT:
        {
            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuffer );
            if(RecvLen>0)
            {
                MsgInfo = (MESSAGE_INFO_T*)RecvBuffer;
                TASK_FMT_DEBUG( pipe, REC_LOG, "广播收到消息 IID %04X IOP %04X payload \n",MsgInfo->IID,MsgInfo->IOP);
                TASK_BUF_DEBUG( pipe, REC_LOG,MsgInfo->playload,MsgInfo->msglen);
                if(MsgInfo->IOP == 0x0106)
                {
                    ret = taskmng_broadcast_delay_report_deal(pipe,runTask,MsgInfo);
                    if(ret<0)
                    {
                        pAmrThreadArg->BroadTask.FailCnt++;
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                    }else
                    {
                        pAmrThreadArg->BroadTask.sendTime = time(NULL);
                        pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_WAIT_NEXT;
                    }
                }
            }else
            {
                if(time(NULL) - pAmrThreadArg->BroadTask.sendTime > 90)
                {
                    TASK_FMT_DEBUG(pipe, REC_LOG,"超时未收到上报的广播延时 \n");
                    pAmrThreadArg->BroadTask.FailCnt++;
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                }
            }
        }
            break;
        case INNER_RTS_WAIT_NEXT:
        {
            if(time(NULL) - pAmrThreadArg->BroadTask.sendTime > runTask->transTask->taskTypeData.addi_param.broadcast)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"当前具体任务执行完毕 广播时长 %d\n",runTask->transTask->taskTypeData.addi_param.broadcast);
                runTask->specificTaskIndex++;
                if(pipe<PLC_NUM)
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
                else
                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_485_INIT;
            }
        }
            break;
        
        case INNER_RTS_SEND:
        {
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex, &currentSpecificTask) < 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"获取具体任务失败 specificTaskIndex %d \n",runTask->specificTaskIndex);
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FAIL;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                return -1;
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"发送广播 无需通信延时 具体任务号 %d \n",currentSpecificTask->nodeid);
            Broad_Timing_Send_nomal(pipe,runTask,currentSpecificTask);
            /*发送完先判断有没有下一个具体任务，没有直接完成*/
            if(taskmng_specific_task_get_from_index(runTask,runTask->specificTaskIndex+1, &currentSpecificTask) < 0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"没有找到下一个具体任务，广播任务执行完毕，更新采集状态集 \n");
                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_PIPE_INIT;
                runTask->result = SCHEME_FINISHED;
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                time_t timenow = time(NULL);
                taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_SUCC,&timenow);
                return -1;
            }
            pAmrThreadArg->BroadTask.sendTime = time(NULL);
            pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_WAIT_NEXT;
        }break;
//        case INNER_RTS_RECV:
//        {
//            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuffer );
//            if(RecvLen>0)
//            {
//                MsgInfo = (MSG_INFO_T*)RecvBuffer;
//                TASK_FMT_DEBUG( pipe, RELAY_LOG, "广播收到消息 IID %04X IOP %04X MsgLen %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->MsgLen);
//                if(MsgInfo->MsgLen ==0)
//                {
//                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "MsgInfo->MsgLen is %d\n",MsgInfo->MsgLen);
//                    break;
//                }
//                else if(MsgInfo->IOP == 0x0106)
//                {
//                    memcpy_r(&pAmrThreadArg->BroadTask.BroadDelay,&MsgInfo->MsgData[4],2);
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
//                    pAmrThreadArg->BroadTask.state = BROAD_TASK_REVISE_MESS;
//                    pAmrThreadArg->BroadTask.sendTime = time(NULL);
//                }
//                else if(MsgInfo->IOP == 0x0107)
//                {
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_SEND;
//                    pAmrThreadArg->BroadTask.state = BROAD_TASK_REPORT_FINISH;
//                }
//                else
//                    pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
//            }
//            else
//            {
//                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_RETRY;
//            }
//        }break;
//        case INNER_RTS_RETRY:
//        {
//            time_t tmpnum = runTask->transTask->taskTypeData.addi_param.broadcast;
//            if(time(NULL) - pAmrThreadArg->BroadTask.retrytime > tmpnum)
//            {
//                /*下一个具体任务*/
//                runTask->specificTaskIndex++;
//                pAmrThreadArg->TotalState.SyncTimeState = INNER_RTS_BROASTDELAY;
//                pAmrThreadArg->BroadTask.state = BROAD_TASK_GET_DELAY;
////                if(runTask->storageCnt >0)
////                {
////                    taskmng_trans_task_result_save(pipe, runTask, currentSpecificTask,OOP_FJ_ERR_RESQ_TIME_OUT, NULL,0);
////                }
//            }
//
//        }break;
        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        break;
    }
    return -1;

}

/*******************************************************************************
* 函数名称: taskmng_metersearch_task_deal_485
* 函数功能: 搜表任务的执行逻辑 485
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

//int taskmng_metersearch_task_deal_485(uint8 pipe, FUJIAN_TASK_RUN_T * runTask)
//{
//    return SearchRs485Meter(pipe);
//}



/*******************************************************************************
* 函数名称: taskmng_metersearch_save
* 函数功能: 存储搜表数据
* 输入参数: pipi 通道号
            runTask 任务方案
            payload 消息内容
            payloadLen 消息长度
* 输出参数: 无
* 返 回 值: 

*******************************************************************************/
int taskmng_metersearch_save(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,MESSAGE_INFO_T *MsgInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index = 0;
    uint16 num,i;
    uint8 lenoffset;
    uint8 addr[6];
    int ret;
    OOP_SEARCHMET_RES_T  searchResult;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020200;
    NormalOad.classtag = 3;
    memset(&searchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    
    index+=4;
    num = get_len_offset(&MsgInfo->playload[index], &lenoffset);
    index+=lenoffset;
    TASK_FMT_TRACE( pipe, REC_LOG, "上报从节点信息 个数 %d\n",num);
    DT_Time2DateTimeShort(time(NULL),&searchResult.serchTime);
    
    for(i=0;i<num;i++)
    {
        memcpy(addr,&MsgInfo->playload[index],6);
        index+=6;
        if(taskmng_addr_isvalid_check(addr,6)<0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "搜表上报地址非法 0x%02x%02x%02x%02x%02x%02x \n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
            continue;
        }
        searchResult.commAdd.len = 5;
        memcpy(searchResult.commAdd.add,addr,6);
        if(fujian_metersearch_result_check(pipe,searchResult,&NormalOad.infoNum)==FALSE)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "搜表地址重复直接过滤 0x%02x%02x%02x%02x%02x%02x \n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
            continue;
        }
        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&searchResult,sizeof(OOP_SEARCHMET_RES_T));
        if(ret==0)
        {
            TASK_FMT_DEBUG( pipe, REC_LOG, "搜表地址写入成功infonum %d 0x%02x%02x%02x%02x%02x%02x \n",NormalOad.infoNum,addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
        }
    }
    //再回个确认
    uint8 sendbuf[10]={0};
    index = 0;
    sendbuf[index++] = 1;
    fujian_trans_task_buf_ack(pipe,0x0102,sendbuf,index,MsgInfo->index,MsgInfo->label);
    return  0;
}

/*******************************************************************************
* 函数名称: taskmng_metersearch_recv
* 函数功能: 搜表 接收处理 本轮结束时间之前结束
* 输入参数: pipi 通道号
            runTask 任务方案

* 输出参数: 无
* 返 回 值: >0                  继续在接受态
            0          正常切换到发送
*           <0               接收超时
*******************************************************************************/

int taskmng_metersearch_recv(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{

    uint8 recvbuf[2000]={0};
    uint16 recvlen;
    MESSAGE_INFO_T      *MsgInfo = NULL;


    recvlen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], recvbuf);
    if(recvlen>0)
    {
        MsgInfo = (MESSAGE_INFO_T*)recvbuf;
        TASK_FMT_TRACE( pipe, REC_LOG, "搜表 recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);
        TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);

        //收到上报数据
        if(MsgInfo->IOP == 0x0102)
        {
            taskmng_metersearch_save(pipe,runTask,MsgInfo);
        }
    }
    else
    {
        //判断是否到任务结束时间
        time_t endTime;
        DT_DateTimeShort2Time(&runTask->thisTurnEndTime,&endTime);
        if(endTime - time(NULL) < 5 )
        {
            return 0;
        }
    }
    return 1;
}


/*******************************************************************************
* 函数名称: taskmng_metersearch_task_deal
* 函数功能: 搜表任务的执行逻辑
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_metersearch_task_deal(uint8 pipe, FUJIAN_TASK_RUN_T * runTask)
{
    if(pipe>=PLC_NUM && pipe<PLC_NUM+MAX_485_NUM)
    {
        return taskmng_metersearch_task_deal_485(pipe,runTask);
    }

    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 allowTime;
    int ret;
    time_t endtime;
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
        case RTS_IDLE:
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "开始执行搜表任务\n");
                taskmng_pipe_collection_state_write(pipe,TASK_METERSEARCH,runTask->taskid,runTask->priority);
                //先结束当前任务
                fujian_current_task_end(pipe);
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
            break;

        case RTS_SEND:
            {
                //启动搜表
                DT_DateTimeShort2Time(&runTask->thisTurnEndTime,&endtime);
                allowTime = (endtime-time(NULL))/60;
                fujian_meter_search_start(pipe,allowTime);
                pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
                TASK_FMT_TRACE( pipe, REC_LOG, "等待上报搜表结果\n");
            }
            break;

        case RTS_RECV:
            {
                ret = taskmng_metersearch_recv(pipe,runTask);
                if(ret<=0)
                {
                    TASK_FMT_TRACE( pipe, REC_LOG, "搜表任务结束 \n");
                    fujian_current_task_end(pipe);
                    runTask->result = SCHEME_FINISHED;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                }
            }
            break;

        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            break;
    }

    return - 1;

}

/*******************************************************************************
* 函数名称: taskmng_idle_task_deal
* 函数功能: 空闲任务的执行逻辑
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_idle_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    time_t timenow = time(NULL);
    taskmng_pipe_collection_state_write(pipe,TASK_IDLE,runTask->taskid,runTask->priority);
    //随时更新最后执行时间
    taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_EXEC,(void*)&timenow);
    sleep(1);
    return -1;

}

/*******************************************************************************
* 函数名称: taskmng_commlog_task_deal
* 函数功能: 通信日志任务的执行逻辑 根据任务时间 到/mnt/data/app/record目录下找所有的日志文件打包
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_commlog_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    taskmng_trans_task_record_file_load(pipe,runTask);    
    if(runTask->result == SCHEME_FINISHED)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "通信日志任务本轮已完成\n");
        return -1;
    }
    //获取上轮开始时间 作为筛选日志文件的开始时间
    taskmng_pipe_collection_state_write(pipe,runTask->transTask->taskTypeData.taskType,runTask->taskid,runTask->priority);
    time_t tmp;
    char saveDir[1000]={0};
    char dirName[100]={0};
    DIR           *dirp;
    struct dirent *entp;
    char cmd[1000]={0};
    char *str1,*str2;
    char  datestr[10]={0};
    int year,month,day,hour;
    OOP_DATETIME_S_T fileDate;
    time_t fileTime;
    memset(&fileDate,0x00,sizeof(OOP_DATETIME_S_T));
    DT_DateTimeShort2Time(&runTask->storageTime, &tmp);
    //用存储时标减一个周期来作为筛选的开始时间
    DT_TimeSubInv(&tmp,runTask->transTask->cycle);
    sprintf(dirName,"commlog_%02d%02d%02d%02d",runTask->storageTime.year -2000,runTask->storageTime.month,
            runTask->storageTime.day,runTask->storageTime.hour);
    sprintf(saveDir,"%s/%s",gRecordPath,dirName);
    if(!path_exist(saveDir))
    {
        path_dir_make(saveDir);
    }
    if ((dirp = opendir(gRecordPath)) == NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "打开目录%s 失败\n",gRecordPath);
        return -1;
    }
    while ((entp = readdir(dirp)) != NULL)
    {
        if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
        {
            continue;
        }
        str1 = strstr(entp->d_name,".log.");
        str2 = strstr(entp->d_name,".record");
        //找日志文件类似于 ctsrec00.log.22033118.record
        if(str1!=NULL && str2!=NULL && (str2-str1 == 13))
        {
            memset(datestr,0x00,sizeof(datestr));
            memcpy(datestr,str1+5,8);
            sscanf(datestr,"%02d%02d%02d%02d",&year,&month,&day,&hour);
            fileDate.year = year+2000;
            fileDate.month = month;
            fileDate.day = day;
            fileDate.hour = hour;
            if(DT_DateTimeShort2Time(&fileDate,&fileTime))
            {
                if(fileTime>=tmp && fileTime< time(NULL))
                {
                    memset(cmd,0x00,sizeof(cmd));
                    sprintf(cmd,"mv -f %s/%s %s",gRecordPath,entp->d_name,saveDir);
                    TASK_FMT_DEBUG(pipe, REC_LOG, "备份文件 命令为 %s \n",cmd);
                    system(cmd);
                }
            }
        }
    }
    //再把文件夹打一个压缩包
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"cd %s ;tar -czvf %s.tar.gz %s",gRecordPath,dirName,dirName);
    TASK_FMT_DEBUG(pipe, REC_LOG, "打包所有文件 命令为 %s \n",cmd);
    system(cmd);
    //打包完后删除
    memset(cmd,0x00,sizeof(cmd));
    sprintf(cmd,"rm -rf  %s",saveDir);
    TASK_FMT_DEBUG(pipe, REC_LOG, "删除命令为 %s \n",cmd);
    system(cmd);
    char logfile[200]={0};
    sprintf(logfile,"%s/%s.tar.gz",gRecordPath,dirName);
    fujian_task_log_event_report( pipe,logfile,runTask->taskid,&runTask->storageTime);
    runTask->result = SCHEME_FINISHED;
    taskmng_trans_task_record_file_write( pipe, runTask);
    return -1;

}

/*******************************************************************************
* 函数名称: taskmng_trigger_task_deal
* 函数功能: 条件触发任务的执行逻辑
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_trigger_task_deal(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    //任务触发后和定时任务一个逻辑
    return taskmng_scheduled_task_deal(pipe,runTask);
   // return -1;

}

/*******************************************************************************
* 函数名称: taskmng_task_trigger
* 函数功能: 条件触发任务启动 找到给触发的任务置个触发时间即可
* 输入参数: 触发的任务方案号
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_task_trigger(uint8 pipe,uint16 taskId,OOP_TSA_T *tsa)
{
    uint16 i,j;
    uint8 taskpipe = 0xff;
    //根据任务方案号找到任务
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        if(taskId == gTransTaskCfg.TaskCfg[i].taskid && gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5)
        {
            taskpipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
            break;
        }
    }
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
    if(taskpipe == 0xff)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "未找到触发的任务方案号 %d\n",taskId);
        return 0;
    }
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[taskpipe];
    FUJIAN_TASK_RUN_T * runTask = NULL;
    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].taskid == taskId)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
            TASK_FMT_DEBUG(taskpipe, REC_LOG, "任务方案号 %d 被触发\n",taskId);
            //如果是触发后执行的本方案相同地址对象的具体任务 则给这个地址置个标记
            if(runTask->transTask->taskTypeData.addi_param.condition.object ==1 && tsa!=NULL)
            {
                for(j=0;j<runTask->meterNum;j++)
                {
                    if(memcmp(tsa,&runTask->meterInfo[j].meterAddr,sizeof(OOP_TSA_T)) == 0)
                    {
                        runTask->meterInfo[j].triggerFlag = 1;
                        TASK_FMT_DEBUG(taskpipe, REC_LOG, "地址为 %02x%02x%02x%02x%02x%02x\n",tsa->add[0],tsa->add[1],tsa->add[2],
                            tsa->add[3],tsa->add[4],tsa->add[5]);
                        break;
                    }
                }

            }

            break;
        }
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_deal
* 函数功能: 全局透明任务执行 抄表任务一直运行 不结束 
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_trans_task_deal(uint8 pipe)
{
    AmrThreadArg_T * pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[pipe];
    int             ret;
    static uint8 flag[AMR_THREAD_NUM] = {0};
    FUJIAN_TASK_RUN_T * runTask = NULL;
    //任务没初始化好时不执行
    if(pAmrThreadArg->TaskModifyFlag != TASK_CFG_INITED)
        return -1;
    //找到当前可以执行的任务
    ret = taskmng_trans_task_run_check(pipe, &runTask);

    if(ret < 0)
    {
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        taskmng_pipe_collection_state_write(pipe,TASK_NULL,0,0);
        return - 1;
    }

    if(runTask->changeFlag != 1)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "任务 %d 有修改 不运行\n", runTask->taskid);
        return - 1;
    }

    if(runTask->taskid != pAmrThreadArg->currentTaskId)
    {
        flag[pipe]=0;
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        pAmrThreadArg->currentTaskId = runTask->taskid;
        TASK_FMT_DEBUG(pipe, REC_LOG, "任务切换 当前运行任务 %d \n", runTask->taskid);
    }
    if(runTask->isNewTurn == 1)
    {
        flag[pipe]=0;
        runTask->isNewTurn = 0;
        taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_INIT,NULL);
        runTask->result = SCHEME_DOING;
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        TASK_FMT_DEBUG(pipe, REC_LOG, "当前运行任务 %d 到新一轮 retryCycle %d 任务独占性 %d 多帧关联 %d\n", runTask->taskid,runTask->retryCycle,
            runTask->transTask->bmonopoly,runTask->transTask->bRelation);
        if(runTask->retryCycle == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "写任务执行开始时间\n");
            taskmng_trans_task_collection_state_update(pipe,runTask,TRANS_TASK_START,NULL);
        }
        if(runTask->retryCycle > runTask->transTask->turnNum)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务 %d 失败轮次 %d 超过 %d 不允许重试\n", runTask->taskid,runTask->retryCycle,runTask->transTask->turnNum);
            runTask->result = SCHEME_FINISHED;
            return -1;
        }
    }
    //如果是独占任务且任务已经执行完毕 就不要往下走了
    //如果独占任务往下执行 初始化的时候会把任务结果置空 可能导致到重试周期的时候不重试
    if(runTask->transTask->bmonopoly == 1 && (runTask->result ==SCHEME_FINISHED || runTask->result == SCHEME_FAIL))
    {
        if(flag[pipe]==0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务 %d 独占任务 本轮已执行完毕\n", runTask->taskid);
            flag[pipe]=1;
        }
        return -1;
    }

    switch(runTask->transTask->taskTypeData.taskType)
    {
        case 0: //定时任务
            {
                return taskmng_scheduled_task_deal(pipe,runTask);
            }
            break;

        case 1: //广播任务
            {
                return taskmng_broadcast_task_deal(pipe,runTask);
            }
            break;

        case 2: //搜表
            {
                return taskmng_metersearch_task_deal(pipe,runTask);

            }
            break;

        case 3: //空闲
            {
                return taskmng_idle_task_deal(pipe,runTask);
            }
            break;

        case 4: //通信日志
            {
               return taskmng_commlog_task_deal(pipe,runTask);
            }
            break;

        case 5: //条件触发
            {
                return taskmng_trigger_task_deal(pipe,runTask);
            }
            break;

        default:
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务%d 不支持的任务类别 %d \n", runTask->taskid,
                 runTask->transTask->taskTypeData.taskType);
            break;
    }

    return - 1;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_result_save
* 函数功能: 全局透明方案结果集存储
* 输入参数:
* 输出参数: 无
* 返 回 值: 0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_trans_task_result_save(uint8 pipe,uint8 isreport,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask,uint8 errorCode,uint8 *databuf,uint16 datalen)
{
    uint8 result = 0;
    OOP_TRANS_TASK_RESULT_T transResult;
    memset(&transResult,0x00,sizeof(OOP_TRANS_TASK_RESULT_T));
    time_t timenow = time(NULL);
    int ret;
    uint32 storeNo=0;
    transResult.transTaskid = runTask->taskid;
    transResult.nodeid = currentSpecificTask->nodeid;
    memcpy(&transResult.groupAdd,tsa,sizeof(OOP_TSA_T));
    transResult.commType = runTask->transTask->commType;
    transResult.storeTime = runTask->storageTime;
    //终端抄表时间直接取当前时间
    DT_Time2DateTimeShort(exectime,&transResult.readTime);
    transResult.storeNO = runTask->storageCnt;
    
    //此处重试轮次应值抄表次数 任务未执行的填0 其他从1开始
    if(errorCode == OOP_FJ_ERR_TASK_NO_EXECUTE)
        transResult.retryTurn = 0;
    else
        transResult.retryTurn = runTask->retryCycle + 1;
    if(errorCode >= ERROR_SUCCESS_CODE_ADD)
    {
        result = errorCode-ERROR_SUCCESS_CODE_ADD;
        transResult.result = 1;
        transResult.transMsg.errCode = 0;
    }else
    {
        transResult.result = (errorCode==0?1:0);
        transResult.transMsg.errCode = errorCode;
    }
    
    transResult.resultMessType = currentSpecificTask->resMessType;


    if(datalen != 0)
    {
        if(datalen > currentSpecificTask->resMaxLen)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "报文超过最长长度 %d \n",currentSpecificTask->resMaxLen);
            datalen = currentSpecificTask->resMaxLen;
            transResult.transMsg.errCode = OOP_FJ_ERR_MESS_CHECK;
        }
        transResult.transMsg.resultData.nNum = datalen;
        memcpy(transResult.transMsg.resultData.value, databuf, datalen);
    }

    //存储序号为0时不用存储 但是还需要上报
    if(runTask->transTask->storeNum != 0)
    {
        WRITE_RECORD_T  inData;
    
        memset(&inData, 0x00, sizeof(WRITE_RECORD_T));
        inData.recordOAD.infoNum = runTask->taskid;
        inData.recordOAD.classtag = 5;
        inData.recordOAD.road.oadMain.value = 0x6E150200;
        inData.recordOAD.road.oadCols.nNum = 1;
        inData.recordOAD.road.oadCols.oad[0].value = currentSpecificTask->nodeid;
        inData.MAC.nNum = 6;
        memcpy(inData.MAC.value, transResult.groupAdd.add, 6);
        DT_DateTimeShort2Time(&runTask->thisTurnBeginTime, (time_t *) &inData.colStartTm);
        DT_DateTimeShort2Time(&runTask->storageTime, (time_t *) &inData.colStoreTm);
        inData.colEndTm = timenow;
        inData.inDatalen = sizeof(OOP_TRANS_TASK_RESULT_T);
        memcpy(inData.inDataBuf, &transResult, sizeof(OOP_TRANS_TASK_RESULT_T));
    
    #if IS_NEED_ADD_8HOUR
    
        //三个时标都要加上8小时
        inData.colStartTm += (8 * 3600);
        inData.colStoreTm += (8 * 3600);
        inData.colEndTm += (8 * 3600);
    #endif
    
        //先查询下数据中心有没有历史数据 如果有则更新
        RECORD_UNIT_T   RecordOut;
    
        memset(&RecordOut, 0x00, sizeof(RECORD_UNIT_T));
        READ_RECORD_T   ReadRecord;
    
        memset(&ReadRecord, 0x00, sizeof(READ_RECORD_T));
        memcpy(&ReadRecord.recordOAD, &inData.recordOAD, sizeof(RECORD_OAD_T));
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = inData.colStoreTm;
        ReadRecord.cEnd = inData.colStoreTm;
        ReadRecord.sortType = 0;
        memcpy(&ReadRecord.MAC, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
        ReadRecord.bTable = 1;
        memcpy(&ReadRecord.table, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
        ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle, &ReadRecord, DATA_LEN_MAX, &RecordOut);
    
        if(ret == 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "需要更新数据\n");
            UPDATE_RECORD_T update;
    
            memset(&update, 0x00, sizeof(UPDATE_RECORD_T));
            memcpy(&update.recordOAD, &inData.recordOAD, sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = inData.colStoreTm;
            memcpy(&update.MAC, &inData.MAC, sizeof(OOP_OCTETVAR64_T));
            update.inDatalen = sizeof(OOP_TRANS_TASK_RESULT_T);
            memcpy(&update.inDataBuf, &transResult, sizeof(OOP_TRANS_TASK_RESULT_T));
            ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle, &update);
    
            if(ret != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "透明结果集更新失败 ret is %d\n", ret);
            }
        }
        else
        {
    
            ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
    
            if(ret != 0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "透明结果集存储失败 ret is %d\n", ret);
            }
    
        }
    }
    
    
    //上报透明任务结果事件 超时没抄到的不上报
    //这儿只上报成功的 其他有问题的上报最后一次
    uint16 i;
    TASK_FMT_TRACE(pipe, REC_LOG, "上报结果 存储序号 %d\n",transResult.storeNO);
    if(runTask->transTask->storeNum ==0)
        transResult.storeNO = 1;
    if( isreport == 1 || transResult.result == 1)
    {
        fujian_task_data_event_report(pipe, &transResult,result);
        for(i=0;i<currentSpecificTask->addrAll.num;i++)
        {
            if(memcmp(tsa,&currentSpecificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
            {
                currentSpecificTask->addrAll.isReport[i] = 1;
                break;
            }
        }
    }
    
    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_trigger_task_poweron_check
* 函数功能: 检查终端上电的触发任务
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 
*******************************************************************************/
void taskmng_trigger_task_poweron_check(uint8 pipe)
{
    uint16 i;
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    FUJIAN_TASK_RUN_T *runTask;
    for(i  = 0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        if(pAmrThreadArg->taskRunList.taskList[i].transTask->taskTypeData.taskType == 5 && 
                pAmrThreadArg->taskRunList.taskList[i].transTask->taskTypeData.addi_param.condition.type == 0)
        {
            runTask = &pAmrThreadArg->taskRunList.taskList[i];
            DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务方案号 %d 终端上电被触发\n",runTask->taskid);
        }

    }
}
/*******************************************************************************
* 函数名称: taskmng_trigger_task_yxchange_check 
* 函数功能:收到遥信变位后检查是否触发任务
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_trigger_task_yxchange_check(uint8* buf,uint16 len)
{
    uint16 index = 0;
    uint16 i,j;
    uint8  yxchange=0;
    uint8  taskYxState;
    uint8 taskpipe=0xff;
    AmrThreadArg_T * pAmrThreadArg;
    FUJIAN_TASK_RUN_T * runTask = NULL;
    index++;    //设备号
    index+=2;   //遥信状态 一个字节长度加一个字节状态 不关心
    index++;
    yxchange = buf[index++];    //获取到遥信变位状态 后面的不关心
        
    //到任务里找对应任务
    pthread_mutex_lock(&gTransTaskCfg.Lock);
    for(i=0;i<gTransTaskCfg.TaskNum;i++)
    {
        if(gTransTaskCfg.TaskCfg[i].taskTypeData.taskType == 5 && gTransTaskCfg.TaskCfg[i].taskTypeData.addi_param.condition.type == 1)
        {
            taskYxState = gTransTaskCfg.TaskCfg[i].taskTypeData.addi_param.condition.data.yx.nValue;
            membitrev(&taskYxState,8);
            //只要任务里的遥信变位有效位和遥信变位信息某位全是1 则触发任务
            if((yxchange & taskYxState) >0)
            {
                taskpipe = taskmng_mtr_pipe_get(&gTransTaskCfg.TaskCfg[i].oad);
                if(taskpipe == 0xff)
                {
                    TASK_FMT_DEBUG(-1, TASK_LOG, "遥信变位触发的任务方案号 %d 没有通道\n",gTransTaskCfg.TaskCfg[i].taskid);
                    continue;
                }
                TASK_FMT_DEBUG(-1, TASK_LOG, "遥信变位触发的任务方案号 %d 通道号 %d \n",gTransTaskCfg.TaskCfg[i].taskid,taskpipe);
                pAmrThreadArg = &gAmrThreadArg[taskpipe];
                for(j=0;j<pAmrThreadArg->taskRunList.taskNum;j++)
                {
                    if(pAmrThreadArg->taskRunList.taskList[j].taskid == gTransTaskCfg.TaskCfg[i].taskid)
                    {
                        runTask = &pAmrThreadArg->taskRunList.taskList[j];
                        DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
                        TASK_FMT_DEBUG(taskpipe, REC_LOG, "任务方案号 %d 被遥信变位触发\n",runTask->taskid);
                        break;
                    }

                }
            }
        }
    }
    pthread_mutex_unlock(&gTransTaskCfg.Lock);
}

/*******************************************************************************
* 函数名称: fujian_meter_event_report_check 
* 函数功能://收到电表事件上报后判断是否能触发
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数: 
* 返 回 值: 
*******************************************************************************/

BOOL fujian_meter_event_report_check(uint8 *buf,uint16 len,OOP_TRIGGER_T *trigger)
{
    if(trigger->type!=4)
        return FALSE;
    uint16            start;              /**< 截取开始       */
    uint16            end;                /**< 截取结束       */
    OOP_OCTETVAR256_T features;           /**< 特征字节       */
    start = trigger->data.features.start;
    end = trigger->data.features.end;
    memcpy(&features,&trigger->data.features.features,sizeof(OOP_OCTETVAR256_T));
    //按特征字节匹配
    if(end>len || end<=start)
    {
        TASK_FMT_DEBUG( 0, RELAY_LOG, "电表事件上报后特征匹配参数不对 start %d end %d len %d \n",start,end,len);
        return FALSE;
    }
    //start可能从1开始
    if(memcmp(features.value,buf+start-1,features.nNum) == 0)
    {
        TASK_FMT_DEBUG( 0, RELAY_LOG, "电表事件上报后特征匹配参数成功 特征字节如下\n");
        TASK_BUF_DEBUG( 0, RELAY_LOG,features.value,features.nNum);
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称: fujian_event_report_mqtt 
* 函数功能://mqtt线程中收到上报事件后的处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_event_report_mqtt(uint8* buf,uint16 len)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[0];
    uint16 i,j;
    uint16 index=0;
    uint8 DataBuf[2000]={0};
    uint16 DataLen;

    uint8 lenoffset;
    uint8 addr[6];
    FUJIAN_TASK_RUN_T * runTask = NULL;
    index+=4;       //端口
    index++;        //通信类型
    memcpy(addr,&buf[index],6);
    index+=6;
    //用数据帧长度来区分376.2事件标识置1 和 电能表等设备有事件主动上报
    DataLen = XADR_GetEleNum(&buf[index],&lenoffset);
    index+=lenoffset;
    TASK_FMT_DEBUG( 0, RELAY_LOG, "收到电表上报的事件 长度 %d \n",DataLen);
    if(DataLen!=0)
    {
        memcpy(DataBuf,&buf[index],DataLen);
        index+=DataLen;
        TASK_BUF_DEBUG( 0, RELAY_LOG,DataBuf,DataLen);
    }
    

    for(i=0;i<pAmrThreadArg->taskRunList.taskNum;i++)
    {
        runTask  = &pAmrThreadArg->taskRunList.taskList[i];
        if(runTask->transTask->taskTypeData.taskType == 5)
        {
            if((DataLen == 0 && runTask->transTask->taskTypeData.addi_param.condition.type == 2) ||
                (DataLen > 0 && fujian_meter_event_report_check(DataBuf,DataLen,&runTask->transTask->taskTypeData.addi_param.condition)==TRUE))
            {
                DT_Time2DateTimeShort(time(NULL),&runTask->triggerTime);
                TASK_FMT_DEBUG(0, RELAY_LOG, "任务方案号 %d 被事件上报触发\n",runTask->taskid);
                //再判断下触发后执行的具体任务对象
                if(runTask->transTask->taskTypeData.addi_param.condition.object ==1)
                {
                    for(j=0;j<runTask->meterNum;j++)
                    {
                        if(memcmp(addr,runTask->meterInfo[j].meterAddr.add,6) == 0)
                        {
                            runTask->meterInfo[j].triggerFlag = 1;
                            TASK_FMT_DEBUG(0, RELAY_LOG, "触发地址为 %02x%02x%02x%02x%02x%02x\n",addr[0],addr[1],addr[2],
                                addr[3],addr[4],addr[5]);
                            break;
                        }
                    }

                }
            }

        }
    }
                
}
/*******************************************************************************
* 函数名称: taskmng_trans_task_meter_num_get
* 函数功能://获取配置总数量
* 输入参数:

* 输出参数: 
* 返 回 值:
*******************************************************************************/

uint16 taskmng_trans_task_meter_num_get(FUJIAN_TASK_RUN_T * runTask)
{
    uint16 num = 0;
    FUJIAN_SPECIFIC_TASK_T *tmp = runTask->specificTask;
    while(tmp)
    {
        num+=tmp->addrAll.num;
        tmp = tmp->next;
    }
    return num;
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_collection_state_update
* 函数功能://透明任务方案采集状态集 更新
* 输入参数:

* 输出参数: 
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_collection_state_update(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_TASK_STATE_CMD_E cmd,void* inbuf)
{
    uint16          i;
    int             ret;
    time_t exectime = 0;
    OOP_TRANS_TASK_RLUESTATE_T *data = NULL;

    pthread_mutex_lock(&gTaskCollectionState.lock);
    for(i = 0;i<gTaskCollectionState.taskNum;i++)
    {
        if(runTask->taskid == gTaskCollectionState.taskState[i].transTaskid)
        {
            data = &gTaskCollectionState.taskState[i];
            break;
        }
    }
    if(data  == NULL)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "未找到任务方案号采集状态 %d\n",runTask->taskid);
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return ;
    }
    //如果没有初始化过 其他命令都不响应
    if(cmd!=TRANS_TASK_INIT)
    {
        if(data->storeTime.year == 0)
        {
            TASK_FMT_DEBUG(pipe, REC_LOG, "任务方案 %d 采集状态未初始化\n",runTask->taskid);
            pthread_mutex_unlock(&gTaskCollectionState.lock);
            return;
        }
    }
    
    switch(cmd)
    {
        case TRANS_TASK_CLEAR: //清空所有
        {
            //只清本轮次的 以防往前对时后再数据初始化
            if(memcmp(&data->storeTime,&runTask->storageTime,sizeof(OOP_DATETIME_S_T))!=0)
            {
                TASK_FMT_TRACE(pipe, REC_LOG, "往前对时后不更新采集状态集\n");
                pthread_mutex_unlock(&gTaskCollectionState.lock);
                return;
            }
            memset(data,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            data->transTaskid = runTask->taskid;
            data->storeTime = runTask->storageTime;
            //存储次数为0时上报的要填1
            if(runTask->transTask->storeNum == 0)
                data->storenum = 1;
            else
                data->storenum = runTask->storageCnt;
            //data->firststartTime = runTask->thisTurnBeginTime;
            data->paramSum = taskmng_trans_task_meter_num_get(runTask);
        }
        break;
        case TRANS_TASK_INIT: //初始化
            {
                //先下方案再下具体任务时会出现总个数为0的情况
                TASK_FMT_DEBUG(pipe, REC_LOG, "初始化 paramSum %d \n",data->paramSum);
                fujian_datetime_s_log_write(pipe, REC_LOG, &data->firststartTime);
                fujian_datetime_s_log_write(pipe, REC_LOG, &data->storeTime);
                if(memcmp(&data->storeTime,&runTask->storageTime,sizeof(OOP_DATETIME_S_T))==0
                        && data->paramSum == taskmng_trans_task_meter_num_get(runTask))
                    break;
                memset(data,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
                data->transTaskid = runTask->taskid;
                data->storeTime = runTask->storageTime;
                if(runTask->transTask->storeNum == 0)
                    data->storenum = 1;
                else
                    data->storenum = runTask->storageCnt;
                //data->firststartTime = runTask->thisTurnBeginTime;
                data->paramSum = taskmng_trans_task_meter_num_get(runTask);
                //给任务执行开始时间和最后执行时间赋个2099年12月31日23:59:59
                data->firststartTime.year = 2099;
                data->firststartTime.month = 12;
                data->firststartTime.day = 31;
                data->firststartTime.hour = 23;
                data->firststartTime.minute = 59;
                data->firststartTime.second = 59;
                data->laststartTime = data->firststartTime;
            }
            break;
        case TRANS_TASK_START: //更新开始时间
        {
            //如果程序重启 会更新开始时间 修复该问题
            if(data->firststartTime.year != 0 && data->firststartTime.year !=2099)
                    break;
            time_t timenow = time(NULL);
            DT_Time2DateTimeShort(timenow,&data->firststartTime);
            DT_Time2DateTimeShort(timenow,&data->laststartTime);
        }
            break;
        case TRANS_TASK_EXEC: //更新最后执行时间
        {
            if(inbuf != NULL)
            {
                memcpy(&exectime , (time_t*)inbuf,sizeof(time_t));
                DT_Time2DateTimeShort(exectime,&data->laststartTime);
            }
        }
            break;
        case TRANS_TASK_SUCC: //成功的时候也更新下最后执行时间
            {
                if(inbuf != NULL)
                {
                    memcpy(&exectime , (time_t*)inbuf,sizeof(time_t));
                    DT_Time2DateTimeShort(exectime,&data->laststartTime);
                }
                if(data->success>=data->paramSum)
                    break;
                data->success++;
                data->machingSum++;

            }
            break;
        case TRANS_TASK_RELATION:
        {
                data->success = 0;
                data->machingSum = 0;
        }
        break;
        default:
            break;
    }
    TASK_FMT_DEBUG(pipe, REC_LOG, "任务方案 %d all %d 存储次数 %d succ %d 最后执行时间 %d:%d:%d cmd %d\n",runTask->taskid,data->paramSum,
            data->storenum,data->success,data->laststartTime.hour,data->laststartTime.minute,data->laststartTime.second,cmd);
    //如果任务是暂停状态 则只计算 不写数据中心
    if(runTask->pauseFlag == 1)
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "任务方案暂停 不更新采集状态集\n");
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return;

    }

    //现在状态集写记录型数据 写之前还需要读出来更新
    uint32 storeNo;
    WRITE_RECORD_T inData;
    memset(&inData,0x00,sizeof(WRITE_RECORD_T));
    inData.recordOAD.infoNum = runTask->taskid;
    inData.recordOAD.classtag = 5;
    inData.recordOAD.road.oadMain.value = 0;
    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0].value = 0x6E200200;
    DT_DateTimeShort2Time(&runTask->storageTime,(time_t*)&inData.colStoreTm);
    inData.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
    memcpy(inData.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));

    //先查询下数据中心有没有历史数据 如果有则更新
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    memcpy(&ReadRecord.recordOAD,&inData.recordOAD,sizeof(RECORD_OAD_T));
    ReadRecord.cType = COL_TM_STORE;
    ReadRecord.cStart = inData.colStoreTm;
    ReadRecord.cEnd = inData.colStoreTm;
    ReadRecord.sortType = 0;

    ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
    if(ret == 0)
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "采集状态集需要更新数据\n");
        UPDATE_RECORD_T update;
        memset(&update,0x00,sizeof(UPDATE_RECORD_T));
        memcpy(&update.recordOAD,&inData.recordOAD,sizeof(RECORD_OAD_T));
        update.filter.choice = 1;
        update.filter.storeTm = inData.colStoreTm;

        update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
        memcpy(&update.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
        ret = db_update_record(gAmrThreadArg[pipe].DBClientHandle,&update);
        if(ret != 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "采集状态集更新失败 ret is %d\n", ret);
        }
    }
    else
    {
        ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);
        if(ret != 0)
        {
            TASK_FMT_TRACE(pipe, REC_LOG, "采集状态集存储失败 ret is %d\n", ret);
        }
        
    }

    pthread_mutex_unlock(&gTaskCollectionState.lock);
}

/*******************************************************************************
* 函数名称: taskmng_pipe_collection_state_write
* 函数功能: 写通道采集状态
* 输入参数:

* 输出参数: 
* 返 回 值:
*******************************************************************************/

void taskmng_pipe_collection_state_write(uint8 pipe,FUJIAN_TASK_TYPE_E type,uint16 taskid,uint8 priority)
{
    AmrThreadArg_T * pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int ret;
    OOP_CHAN_RLUESTATE_T state;
    memset(&state,0x00,sizeof(OOP_CHAN_RLUESTATE_T));
    state.oad = taskmng_pipe_oad_get(pipe);
    state.taskType = type;
    state.transTaskid = taskid;
    state.priority = priority;
    if(memcmp(&state,&pAmrThreadArg->collectionState,sizeof(OOP_CHAN_RLUESTATE_T)) == 0)
    {
        return;
    }
    memcpy(&pAmrThreadArg->collectionState,&state,sizeof(OOP_CHAN_RLUESTATE_T));
    NOMAL_OAD_T     NormalOad;
    memset(&NormalOad,0x00,sizeof(NOMAL_OAD_T));
    NormalOad.oad.value = 0x6E220200;
    NormalOad.infoNum = pipe;
    NormalOad.classtag = 3;
    ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle,&NormalOad,(uint8*)&state,sizeof(OOP_CHAN_RLUESTATE_T));
    if(ret!=0)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "通道采集状态写失败 ret %d \n",ret);
    }
    TASK_FMT_DEBUG(pipe, REC_LOG, "通道采集状态写成功 type %d \n",type);
}

/*******************************************************************************
* 函数名称: taskmng_grop_meter_file_change_fun 
* 函数功能:群组档案变更的处理
* 输入参数: pMeterFileAll 电表档案
          choice 1 表示档案变更 新增或者改变
          choice 0 表示档案删除
          gMeterFile中nindex为删除的序号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_grop_meter_file_change_fun(FUJIAN_grop_meter *pMeterFileAll,uint8 choice,OOP_GROUPMETER_NEW_T gMeterFile)
{
    uint16 i,j;
    int index =-1;
    
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    TASK_FMT_DEBUG(-1,TASK_LOG, "进入档案修改流程 choice %d \n",choice);
    if(choice==0)
    {
        for(i=0;i<pMeterFileAll->nNum;i++)
        {
            if(memcmp(pMeterFileAll->grop_meter[i].groupAddr.add,gMeterFile.groupAddr.add,6)==0)                
            {
                index = i;
                memset(&pMeterFileAll->grop_meter[i],0x00,sizeof(OOP_GROUPMETER_NEW_T));               
                break; 
            }
        }
        if(index!=-1)
        {
            for(i=index;i<pMeterFileAll->nNum-1;i++)
            {
                pMeterFileAll->grop_meter[i] = pMeterFileAll->grop_meter[i+1];
            }
            memset(&(pMeterFileAll->grop_meter[pMeterFileAll->nNum-1]),0x00,sizeof(OOP_METER_T));
            pMeterFileAll->nNum--;
        }
    }
    else
    {
        for(i=0;i<pMeterFileAll->nNum;i++)
        {
            //如果当前档案中找到相同的 则直接替换
            if(memcmp(pMeterFileAll->grop_meter[i].groupAddr.add,gMeterFile.groupAddr.add,6)==0)
            {
                index = i;
                memcpy(&pMeterFileAll->grop_meter[i],&gMeterFile,sizeof(OOP_GROUPMETER_NEW_T));
                break; 
            }
        }
        //如果没有找到 则新增
        if(index==-1)
        {
            pMeterFileAll->grop_meter[pMeterFileAll->nNum] = gMeterFile;
            pMeterFileAll->nNum++;
        }
    }
    TASK_FMT_DEBUG(-1,TASK_LOG, "群组档案个数  %d \n",pMeterFileAll->nNum);
    for(i=0;i<pMeterFileAll->nNum;i++)
    {
            TASK_FMT_DEBUG(-1,TASK_LOG, "群组档案地址\n");
            TASK_BUF_DEBUG(-1,TASK_LOG,pMeterFileAll->grop_meter[i].groupAddr.add,6);
        for(j=0;j<pMeterFileAll->grop_meter[i].num;j++)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "档案%d 地址\n",j+1);
            TASK_BUF_DEBUG(-1,TASK_LOG,&pMeterFileAll->grop_meter[i].addr[j],6);
        }
        
    }
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}


/*******************************************************************************
* 函数名称: taskmng_grop_meter_file_update_check 
* 函数功能: 检查是否收到群组档案更新的消息 
* 输入参数: 
* 输出参数: 档案的更新消息
* 返 回 值: 
*******************************************************************************/
void taskmng_grop_meter_file_update_check(DB_UPDATE_T *dbupdate)
{
    int Recvlen = 0;

    Recvlen = taskmng_db_update_queue_read(&pGropMeterFileUpdate, dbupdate);
    if(Recvlen>0)
    {

        TASK_FMT_DEBUG(-1,TASK_LOG, "收到mqtt发送的更新群组档案消息 infonum %d oad %08x !\n",dbupdate->infonum,dbupdate->oad);
        oop_grop_meter.FileState = TASK_CFG_MODIFY;
    }
}
/*******************************************************************************
* 函数名称: taskmng_grop_meter_file_read_from_pn
* 函数功能: 根据信息点号从数据中心读取群组档案
* 输入参数: 
            infonum     信息点号
           
* 输出参数: gMeterFile  群组档案信息
* 返 回 值:  0 成功

*******************************************************************************/

uint8 taskmng_grop_meter_file_read_from_pn(uint16 infonum,OOP_GROUPMETER_NEW_T *gMeterFile)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x6E050200;
    NormalOad.infoNum = infonum;
    
#if DBCENTER_MQTT
    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_GROUPMETER_NEW_T),(uint8*)gMeterFile,&len);
#else
    ret = nomal_data_read(MainHandle, NormalOad, sizeof(OOP_GROUPMETER_NEW_T),(uint8*)gMeterFile, &len);
#endif
    if(ret !=0 ||len!=sizeof(OOP_GROUPMETER_NEW_T))
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "读取群组地址失败 ret %d len %d  nomal len %d \n",ret,len,sizeof(OOP_GROUPMETER_NEW_T));
         return -1 ;
    }
       
    
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_grop_meter_file_delete
* 函数功能: 删除全部电表档案
* 输入参数: 
            pMeterFileAll   //电表档案
           
* 输出参数: 无
* 返 回 值:   档案中剩余的电表个数
*******************************************************************************/
uint16 taskmng_grop_meter_file_delete(FUJIAN_grop_meter *pMeterFileAll)
{
    uint16 leftcount = 0;
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    pMeterFileAll->nNum = leftcount;
    memset(pMeterFileAll->grop_meter,0x00,sizeof(OOP_GROUPMETER_NEW_T)*255);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return leftcount;
}

/*******************************************************************************
* 函数名称: taskmng_grop_meter_file_ergodic
* 函数功能: 添加群组档案
* 输入参数: 
            oop_grop_meter   //群组档案
           
* 输出参数: 无
* 返 回 值:  
*******************************************************************************/

void taskmng_grop_meter_file_ergodic(FUJIAN_grop_meter *oop_grop_meter)
{
    time_t RealTime_t;
    int ret;
    OOP_GROUPMETER_NEW_T gMeterFile;
    uint16 leftcount=0;
    //OOP_OAD_U oad = {0xF1010200};
    switch(oop_grop_meter->FileState)
    {
    case TASK_CFG_MODIFY:
        //收到档案变更消息后进行处理
        
        //0xff表示所有档案删除
        if(dbupdateMeter.infonum == 0xffff)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "infonum %d 群组档案清空 !\n",dbupdateMeter.infonum);
            
            leftcount = taskmng_grop_meter_file_delete(oop_grop_meter);
            TASK_FMT_DEBUG(-1,TASK_LOG, "剩余群组档案个数 %d \n",leftcount);
        }else
        {
            memset(&gMeterFile,0x00,sizeof(OOP_GROUPMETER_NEW_T));
            ret = taskmng_grop_meter_file_read_from_pn(dbupdateMeter.infonum,&gMeterFile);
            TASK_FMT_DEBUG(-1,TASK_LOG, "读取档案%d logicid %d ret %d !\n",dbupdateMeter.infonum,dbupdateMeter.logicid,ret);
            if(ret==0)
            {
                //测点删除时同时删除数据中心中安全模式
                //taskmng_normal_data_delete(dbupdateMeter.infonum,oad);
                //读取到档案信息 表示档案变更或者新增
                TASK_FMT_DEBUG(-1,TASK_LOG, "群组档案地址 \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.groupAddr.add,6);
                if(taskmng_grop_addr_isvalid(gMeterFile.groupAddr.add)>0)
                {
                    taskmng_grop_meter_file_change_fun(oop_grop_meter,1,gMeterFile);
                }
                 
                else
                {
                    TASK_FMT_DEBUG(-1,TASK_LOG, "群组档案地址不合法 \n");
                }
            }
            else
            {
                //未读取到档案信息，表示档案删除
               memset(gMeterFile.groupAddr.add,0x88,4);
               gMeterFile.groupAddr.add[4] = dbupdateMeter.infonum/100;
               gMeterFile.groupAddr.add[5] = dbupdateMeter.infonum%100;
               gMeterFile.groupAddr.add[4] = commfun_HexToBcd(gMeterFile.groupAddr.add[4]);
               gMeterFile.groupAddr.add[5] = commfun_HexToBcd(gMeterFile.groupAddr.add[5]);
               taskmng_grop_meter_file_change_fun(oop_grop_meter,0,gMeterFile);
            }
        }
        //进入30秒延迟 预防多次修改档案
        oop_grop_meter->ModifyDelaySec = 5; //原来30秒，现在先给缩短到5秒
        RealTime_t = time(NULL);
        oop_grop_meter->FunLastRunTime = RealTime_t;
        oop_grop_meter->FileState = TASK_CFG_MODIFY_DELAY;
        break;
    case TASK_CFG_MODIFY_DELAY:

        RealTime_t = time(NULL);
        if(RealTime_t!=oop_grop_meter->FunLastRunTime)
        {   
            oop_grop_meter->ModifyDelaySec--;
            oop_grop_meter->FunLastRunTime=RealTime_t;
            TASK_FMT_DEBUG(-1,TASK_LOG, "延时%d \n",oop_grop_meter->ModifyDelaySec);
        }
        //延时过后 通知各抄表线程档案变更
        if(oop_grop_meter->ModifyDelaySec<=0)
        {   
            oop_grop_meter->FileState = TASK_CFG_INITED;
            ReloadTaskFlag = 1;
        }
        break;
    default:
        break;   
    }
}

/*******************************************************************************
* 函数名称: fujian_collection_update_deal 
* 函数功能:处理采集状态中的已发送和已确认条数
* 输入参数:

* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void fujian_collection_update_deal(uint8* buf,uint16 len)
{
    uint16          i;
    int             ret;
    OOP_TRANS_TASK_RLUESTATE_T *data = NULL;
    uint16 index = 0;
    uint16 taskid;
    uint8 cmd ;
    uint16 num;
    OOP_DATETIME_S_T storgeTime;
    memset(&storgeTime,0x00,sizeof(OOP_DATETIME_S_T));
    memcpy_r(&taskid,&buf[index],2);
    index+=2;
    memcpy_r(&storgeTime.year,&buf[index],2);
    index+=2;
    storgeTime.month = buf[index++];
    storgeTime.day = buf[index++];
    storgeTime.hour = buf[index++];
    storgeTime.minute = buf[index++];
    storgeTime.second = buf[index++];
    cmd = buf[index++];
    memcpy_r(&num,&buf[index],2);
    index+=2;
    
    pthread_mutex_lock(&gTaskCollectionState.lock);
    for(i = 0;i<gTaskCollectionState.taskNum;i++)
    {
        if(taskid == gTaskCollectionState.taskState[i].transTaskid)
        {
            data = &gTaskCollectionState.taskState[i];
            break;
        }
    }
    if(data  == NULL)
    {
        TASK_FMT_DEBUG(-1, TASK_LOG, "未找到任务方案号采集状态 %d\n",taskid);
        pthread_mutex_unlock(&gTaskCollectionState.lock);
        return ;
    }

    uint32 tmptime;
    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    ReadRecord.recordOAD.infoNum = taskid;
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = 0x6E200200;
    UPDATE_RECORD_T update;
    memset(&update,0x00,sizeof(UPDATE_RECORD_T));

    if(storgeTime.year == data->storeTime.year && storgeTime.month == data->storeTime.month &&
        storgeTime.day == data->storeTime.day && storgeTime.hour == data->storeTime.hour && 
        storgeTime.minute == data->storeTime.minute && storgeTime.second == data->storeTime.second)
    {
        if(cmd == 0)
        {
            if(data->sendNum+num > data->paramSum)
            {
                data->sendNum = data->paramSum;
            }else
            {
                data->sendNum+=num;
            }
        }else
        {
            if(data->receNum+num > data->paramSum)
            {
                data->receNum = data->paramSum;
            }else
            {
                data->receNum+=num;
            }
        }

        DT_DateTimeShort2Time(&data->storeTime,(time_t*)&tmptime);

        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = tmptime;
        ReadRecord.cEnd = tmptime;
        ReadRecord.sortType = 0;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0)
        {
            TASK_FMT_DEBUG(-1, TASK_LOG, "采集状态集更新数据 taskid %d send %d recv %d \n",taskid,data->sendNum,data->receNum);

            memcpy(&update.recordOAD,&ReadRecord.recordOAD,sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = tmptime;
        
            update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
            memcpy(&update.inDataBuf,data,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            ret = db_update_record(gDBClientTask,&update);
            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "采集状态集更新失败 ret is %d\n", ret);
            }
        } 

    }
    else
    {
        //如果不是当前轮次的 需要从数据中心读出来更新
        DT_DateTimeShort2Time(&storgeTime,(time_t*)&tmptime);
        ReadRecord.cType = COL_TM_STORE;
        ReadRecord.cStart = tmptime;
        ReadRecord.cEnd = tmptime;
        ReadRecord.sortType = 0;
        
        ret = db_read_record(gDBClientTask,&ReadRecord,DATA_LEN_MAX,&RecordOut);
        if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RLUESTATE_T))
        {
            OOP_TRANS_TASK_RLUESTATE_T readdata;
            memset(&readdata,0x00,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            memcpy(&readdata,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            TASK_FMT_DEBUG(-1, TASK_LOG, "数据中心读出来存储时标 %d月%d日 %d:%d:%d taskid %d send %d recv %d paramSum %d\n",
                readdata.storeTime.month,readdata.storeTime.day,readdata.storeTime.hour,readdata.storeTime.minute,
                    readdata.storeTime.second ,taskid,readdata.sendNum,readdata.receNum,readdata.paramSum);
            if(cmd == 0)
            {
                if(readdata.sendNum+num > readdata.paramSum)
                {
                    readdata.sendNum = readdata.paramSum;
                }else
                {
                    readdata.sendNum+=num;
                }
            }else
            {
                if(readdata.receNum+num > readdata.paramSum)
                {
                    readdata.receNum = readdata.paramSum;
                }else
                {
                    readdata.receNum+=num;
                }
            }

            memcpy(&update.recordOAD,&ReadRecord.recordOAD,sizeof(RECORD_OAD_T));
            update.filter.choice = 1;
            update.filter.storeTm = tmptime;
        
            update.inDatalen = sizeof(OOP_TRANS_TASK_RLUESTATE_T);
            memcpy(&update.inDataBuf,&readdata,sizeof(OOP_TRANS_TASK_RLUESTATE_T));
            
            TASK_FMT_DEBUG(-1, TASK_LOG, "采集状态集更新前面轮次数据 存储时标 %d月%d日 %d:%d:%d taskid %d send %d recv %d \n",
                readdata.storeTime.month,readdata.storeTime.day,readdata.storeTime.hour,readdata.storeTime.minute,
                    readdata.storeTime.second ,taskid,readdata.sendNum,readdata.receNum);
            ret = db_update_record(gDBClientTask,&update);
            if(ret != 0)
            {
                TASK_FMT_DEBUG(-1, TASK_LOG, "采集状态集更新失败 ret is %d\n", ret);
            }
        }

    }
    pthread_mutex_unlock(&gTaskCollectionState.lock);
}

/*******************************************************************************
* 函数名称: taskmng_trans_task_error_report
* 函数功能: 抄读失败的表上报结果 从数据中心读最后一条记录上报
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0                  成功
*           <0                  失败
*******************************************************************************/

int taskmng_trans_task_error_report(uint8 pipe,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask)
{
    uint16 i;
    OOP_TRANS_TASK_RESULT_T transResult;
    memset(&transResult,0x00,sizeof(OOP_TRANS_TASK_RESULT_T));
    int ret;
    time_t storetime,readtime;
    DT_DateTimeShort2Time(&runTask->storageTime,(time_t*)&storetime);

    RECORD_UNIT_T RecordOut;
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    READ_RECORD_T ReadRecord;
    memset(&ReadRecord,0x00,sizeof(READ_RECORD_T));
    ReadRecord.recordOAD.infoNum = runTask->taskid;
    ReadRecord.recordOAD.classtag = 5;
    ReadRecord.recordOAD.road.oadMain.value = 0x6E150200;
    ReadRecord.recordOAD.road.oadCols.nNum = 1;
    ReadRecord.recordOAD.road.oadCols.oad[0].value = currentSpecificTask->nodeid;
    ReadRecord.MAC.nNum = 6;
    memcpy(ReadRecord.MAC.value,tsa->add,6);
    ReadRecord.cType = COL_TM_STORE;
    ReadRecord.cStart = storetime;
    ReadRecord.cEnd = storetime;
    ReadRecord.sortType = 0;

    ReadRecord.bTable = 1;     
    memcpy(&ReadRecord.table,&ReadRecord.MAC,sizeof(OOP_OCTETVAR64_T));
    ret = db_read_record(gAmrThreadArg[pipe].DBClientHandle,&ReadRecord,DATA_LEN_MAX,&RecordOut);
    if(ret == 0 && RecordOut.record[0].outDatalen == sizeof(OOP_TRANS_TASK_RESULT_T))
    {
        TASK_FMT_TRACE(pipe, REC_LOG, "获取到最后一条记录上报\n");
        memcpy(&transResult,RecordOut.record[0].outDataBuf,sizeof(OOP_TRANS_TASK_RESULT_T));
        //对比下获取到的记录的存储时标和当前一不一样
        if(memcmp(&transResult.storeTime, &runTask->storageTime, sizeof(OOP_DATETIME_S_T)) == 0)
        {
            //如果从数据中心的读的抄表时间还是老的 则更新下
            DT_DateTimeShort2Time(&transResult.readTime ,&readtime);
            if(readtime < exectime)
            {
                DT_Time2DateTimeShort(exectime,&transResult.readTime);
            }
            transResult.retryTurn = runTask->retryCycle + 1;
            fujian_task_data_event_report(pipe, &transResult, 0);
            for(i=0;i<currentSpecificTask->addrAll.num;i++)
            {
                if(memcmp(tsa,&currentSpecificTask->addrAll.addr[i],sizeof(OOP_TSA_T)) == 0)
                {
                    currentSpecificTask->addrAll.isReport[i] = 1;
                    break;
                }
            }
            return 0;
        }
    }

    return -1;


}

/*******************************************************************************
* 函数名称: taskmng_trans_task_isreport_check
* 函数功能: 任务一轮结束时检查下有没有没上报的数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 0                  成功
*           <0                  失败
*******************************************************************************/

void taskmng_trans_task_isreport_check(uint8 pipe,FUJIAN_TASK_RUN_T * runTask)
{
    uint16 i,j;
    uint8 flag = 0;
    int ret;
    FUJIAN_SPECIFIC_TASK_T *specificTask =NULL;
    //任务暂停时不上报结果
    if(runTask->pauseFlag == 1 || runTask->result == SCHEME_FINISHED)
        return;
    for(j=0;j<runTask->meterNum;j++)
    {
        if(runTask->meterInfo[j].meterResult == 2)
            continue;
        specificTask = runTask->specificTask;
        flag = 0;
        ret =-1;
        while(specificTask)
        {
            for(i=0;i<specificTask->addrAll.num;i++)
            {
                if(specificTask->addrAll.isReport[i] == 0 && 
                    memcmp(&specificTask->addrAll.addr[i],&runTask->meterInfo[j].meterAddr,sizeof(OOP_TSA_T))==0)
                {
                    TASK_FMT_TRACE(pipe, REC_LOG, "方案号 %d 具体任务号 %d 电表 %02x%02x%02x%02x%02x%02x 还未上报结果 flag %d\n",runTask->taskid,
                            specificTask->nodeid,specificTask->addrAll.addr[i].add[0],specificTask->addrAll.addr[i].add[1],specificTask->addrAll.addr[i].add[2],
                            specificTask->addrAll.addr[i].add[3],specificTask->addrAll.addr[i].add[4],specificTask->addrAll.addr[i].add[5],flag);
                    if(flag == 0)
                    {
                        ret = taskmng_trans_task_error_report(pipe,&specificTask->addrAll.addr[i],runTask->meterInfo[j].execTime,runTask,specificTask);
                        if(ret < 0)
                        {
                            TASK_FMT_TRACE(pipe, REC_LOG,"未获取到相关数据\n");
                        }
                        specificTask->addrAll.isReport[i] = 1;
                        flag = 1;
                    }else
                    {
                        if(ret == 0)
                        {
                            taskmng_trans_task_result_save(pipe,1,&runTask->meterInfo[j].meterAddr,runTask->meterInfo[j].execTime, runTask,specificTask,OOP_FJ_ERR_TASK_NO_EXECUTE, NULL,0);
                        }
                        specificTask->addrAll.isReport[i] = 1;
                    }
                }
            }
            specificTask = specificTask->next;
        }

    }

}

#endif

