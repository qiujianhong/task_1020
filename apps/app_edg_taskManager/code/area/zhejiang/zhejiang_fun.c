
#include "../include/CtsAll.h"

#ifdef AREA_ZHEJIANG

//浙江需要判断下下发的档案是否在搜表结果中 如果在搜表结果中 则暂时不同步档案 等定时再同步
uint8 gMeterfileSyncFlag;       //默认是1 如果在搜表结果中 则置为0

//按照最新规范《浙江模组化上行补充技术协议_20200930》开发
uint16 UnpackDeviceType(uint8 pipe,uint8 prtl,uint8 *buf,uint16 len)
{
    uint16 devicetype=0;
    int ret;
    char devicestr[100]={0};
    switch(prtl)
    {
        case PRTL_07:
        {
            MeterPrtlFmt_T MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            ret = taskmng_645_meter_data_unpack(len,buf,MeterPrtlData.MtrAddr,&MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen,MeterPrtlData.DataBuff);
            if(ret ==CHECK_RESULT_SUCCEED)
            {
                strcpy(devicestr,(char*)&MeterPrtlData.DataBuff[4]);
            }
        }
            break;
        case PRTL_OOP:
        {
            AppLayerList_t *pAppLayer = NULL;
            uint8 databuf[100]={0};
            uint16 datalen;
            uint16 index=0;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive ret %d \n",ret);
            if(ret >=eOopComm_Fail)//解帧失败直接返回
                return devicetype; 
            //获取电表地址
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return devicetype;
            datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);

            //databuf从oad后面的结果类型开始
            if(databuf[index++]!=1)
                return devicetype;
            if(databuf[index++]!=DT_VISIBLE_STRING)
                return devicetype;
            uint8 strlen = databuf[index++];
            memcpy(devicestr,&databuf[index],strlen);
        }
            break;
        default:
            return devicetype;
    }
    TASK_FMT_TRACE(pipe, RELAY_LOG,"获取的设备类型字符串 %s \n",devicestr);
    //0或者未设置为：电能表
    //1：分支、表箱监测单元
    //2：智能断路器
    //98：出线监测单元
    //99：出线断路器
    //645协议的智能断路器的设备型号为： RCDXXXXXXXX，RCD三个字符固定不变，表示断路器设备，第四个字符出厂默认为0；设置为1时表示出线断路器；
    //645协议的分支、表箱监测单元的设备型号为：LTUXXXXXXX，LTU三个字符固定不变，表示分支监测设备，第四个字符出厂默认为0；设置为1时表示出线监测设备；

    if(devicestr[0]=='R'&&devicestr[1]=='C'&&devicestr[2]=='D')
    {
        if(devicestr[3]=='1')
            devicetype = 99;
        else
            devicetype = 2;
    } else if(devicestr[0]=='L'&&devicestr[1]=='T'&&devicestr[2]=='U')
    {
        if(devicestr[3]=='1')
            devicetype = 98;
        else
            devicetype = 2;
    }

    return devicetype;
}

uint16 GetDeviceType(uint8 pipe,uint8 *Addr,uint8 prtl)
{
    uint16 devicetype = 0;
    uint16 len;
    uint8 SendBuf[1000]={0};
    uint8 RecvBuf[1000]={0};
    uint16 RecvLen;
    if(prtl == PRTL_07)
    {
        uint32 DI = 0x09080A0B;  
        MeterPrtlFmt_T    Meter;
        memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
        memcpy(Meter.MtrAddr,Addr,6);
        Meter.CtrlCode = 0x11;
        Meter.DataLen = 4;

        memcpy(Meter.DataBuff,&DI,4);
        len = taskmng_meter_prtl_645_pack(SendBuf,Meter);
    }
    else if(prtl == PRTL_OOP)
    {
        OOP_OAD_U Oad = {0x43000200};
        LinkLayer_t *pLinkLayer = NULL;
        uint8 buf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        Tsa.len = 5;
        memcpy_r(Tsa.add,Addr,6);
        memcpy(buf,(uint8*)&Oad.value,4);
        index+=4;
        OopGet.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);

        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(SendBuf,pLinkLayer->sendBuff,len);
        }
    }
    else
        return devicetype;
    TASK_FMT_TRACE(pipe, RELAY_LOG, "组帧查询设备类型 \n");
    TASK_BUF_TRACE(pipe, RELAY_LOG, SendBuf,len);
    uint8 payload[200]={0};
    uint16 index = 0;
    uint16 Overtime = 30;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = prtl;
    memcpy_r(&payload[index],Addr,6);
    index+=6;
    index++;
    memcpy_r(&payload[index],(uint8*)&Overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&Overtime,2);
    index+=2;
    if(len<0x80)
    {
        payload[index++] = len;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(len>>8);
        payload[index++] = (uint8)len;
    }
    memcpy(&payload[index],SendBuf,len);
    index+=len;
    if(taskmng_msg_fun_deal(pipe,0x0003,0x0038,index,payload,&RecvLen,RecvBuf)<0)
    {
        TASK_FMT_TRACE(pipe, RELAY_LOG, "查询设备类型失败 \n");
        return devicetype;
    }
    TASK_FMT_TRACE(pipe, RELAY_LOG, "查询设备类型回码 \n");
    TASK_BUF_TRACE(pipe, RELAY_LOG, RecvBuf,RecvLen);
    devicetype = UnpackDeviceType(pipe,prtl,RecvBuf,RecvLen);
    return devicetype;
}

/*******************************************************************************
* 函数名称: taskmng_meter_search_result_write_area 
* 函数功能: //将搜表结果写入数据中心
* 输入参数: pipe 端口号
        buf 上报的搜表数据
        len  长度
* 输出参数:  
* 返 回 值:
*******************************************************************************/
void taskmng_meter_search_result_write_area(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MeterNum;
    
    uint16 index=0;
    uint8 prtl;
    int ret;
    uint8 optional;
    OOP_SEARCHMET_RES_T  SearchResult;
    
    memset(&SearchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    uint8 colAddr[6]={0};
    uint8 colflag = 0;
    uint8 Addr[6]={0};
    time_t timenow = time(NULL);
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020200;
    NormalOad.classtag = 3;
    uint8 i;

    index+=4;
    optional = buf[index++];
    
    if(optional== 0x01 )
    {
        index+=8 ;      //8个字节附加信息
    }
    MeterNum = buf[index++];
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "搜表上报结果 档案个数 %d \n",MeterNum);
    //浙江特殊需求 第一块表规约类型填0XCC 表示采集器地址
    SearchResult.serchTime = Time_tToDataTimeS(timenow);
    for(i=0;i<MeterNum;i++)
    {

        index+=2;
        memcpy_r(Addr,&buf[index],6);
        index+=6;
        index+=6;
        prtl = buf[index++];
        //index++;
        if(i==0 && prtl == 0xcc)
        {
            memcpy(colAddr,Addr,6);
            colflag =1;
            continue;
        }
        if(colflag==1)
        {
            SearchResult.collAdd.len = 5;
            memcpy_r(SearchResult.collAdd.add,colAddr,6);
        }
        if(taskmng_addr_isvalid_check(Addr,6)<0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址非法 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
            continue;
        }
        SearchResult.commAdd.len = 5;
        memcpy_r(SearchResult.commAdd.add,Addr,6);
        SearchResult.proto = prtl;
        SearchResult.nAdded = 2;
        SearchResult.info[0].oad.value = 0x60060200;    //端口
        SearchResult.info[0].value.type = DT_OAD;
        SearchResult.info[0].value.oad = taskmng_pipe_oad_get( pipe);
        SearchResult.info[1].oad.value = 0x60070200;        //波特率
        SearchResult.info[1].value.type = DT_ENUM;
        SearchResult.info[1].value.byVal = 255;     //波特率自适应
        if(CheckMeterSearchResult(pipe,SearchResult,&NormalOad.infoNum)==FALSE)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址重复直接过滤 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
            continue;
        }
        //浙江需要获取设备类型写入搜表结果中
        SearchResult.nAdded = 3;
        SearchResult.info[2].oad.value = 0x6E010200;    //端口
        SearchResult.info[2].value.type = DT_LONG_UNSIGNED;
        SearchResult.info[2].value.wVal =GetDeviceType(pipe,Addr,prtl);

        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&SearchResult,sizeof(OOP_SEARCHMET_RES_T));

        if(ret ==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表结果写入成功 index %d prtl %d  \n",NormalOad.infoNum,prtl);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        }
    }
    
}


/*******************************************************************************
* 函数功能: 检查下发的档案是否在搜表结果中
* 输入参数:           
* 输出参数: 无
* 返 回 值: 
            
*******************************************************************************/

BOOL taskmng_meter_file_in_search_result_check(OOP_METER_T *meter)
{
    uint16 i;
    BOOL result = FALSE;
    pthread_mutex_lock(&MeterSearchResult.lock);
    for(i=0;i<MeterSearchResult.count;i++)
    {
        if(meter->basic.protocol ==MeterSearchResult.result[i].prtl && 
            memcmp(meter->basic.tsa.add,MeterSearchResult.result[i].Addr,6)==0)
        {
            result = TRUE;
            break;
        }

    }
    pthread_mutex_unlock(&MeterSearchResult.lock);
    return result;
}

/*******************************************************************************
* 函数功能: 过分执行该任务 浙江定时产生档案同步任务 6:10 15:10 21:10
* 输入参数:           
* 输出参数: 无
* 返 回 值: 
            
*******************************************************************************/

BOOL taskmng_meter_file_cmp_check(uint8 pipe)
{
    DateTime_T *timenow = &gCurBinTime[pipe];
    if(timenow->Hour == 6 && timenow->Minute == 10)
        return TRUE;
    if(timenow->Hour == 15 && timenow->Minute == 10)
        return TRUE;
    if(timenow->Hour == 21 && timenow->Minute == 10)
        return TRUE;
    return FALSE;
}

#endif
