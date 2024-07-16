
#include "CtsAll.h"
#include "CtsPlc.h"
#include "CtsACMeter.h"
#include "area.h"
#include "CtsBt.h"

#include "min_gather.h"
#ifdef AREA_FUJIAN
extern OOP_GATHER_E gGatherMode;
#endif

#define GET_MAX_METER_FAIL_COUNT(roundFailCnt) (roundFailCnt >= ROUND_FAIL_CNT_LIMIT ? 1 : MAX_METER_FAILURE_COUNT)

#define MAX_NODE_VERSION_INFO_GET_NUM  15   //查询 HPLC 载波模块节点信息 每次查询的最多数量
 //当前是否处于抄表空闲时段，即没有抄表任务，0表示不空闲 1表示空闲，可以重新执行本轮分钟曲线任务
uint8  gFreeFlag[AMR_THREAD_NUM] = {0};
uint8  gPowerOnFlag[AMR_THREAD_NUM]={FALSE};                 //上电标志
//档案变更使用的全局变量
MODEFY_TYPE_T              gMeterFileModifyType[AMR_THREAD_NUM] = {MODEFY_CLEAR};      //档案修改方式
uint8                      gMeterFileCmpFlag[AMR_THREAD_NUM]={FALSE};
uint8                      gMeterFileModifyFlag[AMR_THREAD_NUM]={FALSE};

uint8 BroadAddr645[6]={0x99,0x99,0x99,0x99,0x99,0x99};//645广播帧地址域
//#define IS_NEED_GET_ROUTER_STATE  1     //控制是否需要抄表前查询路由学习状态

#define IS_NEED_CLEAR_645_FLAG   0     //控制是否需要在中午12点清空645负荷曲线抄读标志 目前默认关闭

#define PATCH_FUN       0       //补抄开关 测试时关闭
extern uint8 gRouterStudy;
uint8 nodecollect = 0xAA;//默认所有的采集器都支持分钟冻结;
extern uint16 nodecount;
extern uint8 gHPLCSearchState[PLC_NUM];  //搜表完成标识 0表示未完成 不能抄表 1表示搜表完成，用于江苏
OOP_COLLECTOR_T Collector;
uint16 metercount;
extern uint8 gReponsePIIDCAD;
extern uint8 minutecollect;
uint8   gLastFrameFlag            = 0;           //最后一帧，无须回复
extern uint8 routecollectstate;
extern uint8 nodecollectstate;
extern uint16 nodecollectcount;
extern uint16 cajpn;
uint8 finishstate=0;


extern AmrThreadArg_T gCajThreadArg[2];
extern uint8 gMinuteCollect;
extern const MapOopTo645_NEW MapOopTo645_Min[];
extern time_t GetCurveTaskBeginTime(time_t startTime, SCHEME_TASK_RUN_INFO * pCbTask);
extern time_t GetDayTaskBeginTime(uint8 pipe,time_t startTime, SCHEME_TASK_RUN_INFO * pCbTask);

uint8 total_col_flg[AMR_THREAD_NUM] = {0};   //打高分全量数据采集时使用，0代表不需要重置重试次数，1代表需要重置重试次数为1
uint8 patch_policy_flg[AMR_THREAD_NUM] = {0};   //打高分专三补抄策略时使用，0代表不需要重置重试次数，1代表需要重置重试次数为0

extern uint8 gdelay;

const PLCTASK_FUNINFO PlcTask_FunList[] =
{
    //档案同步加上5秒延时 以防台体测试时清空档案后立刻档案同步 ccoRouter不会和下面再档案同步
   {MS_FILE_CMP,       3,      RCS_PAUSE,      5,            taskmng_meter_file_cmp_deal},                //电表档案比对
    {MS_FILE_MODIFY,    4,      RCS_PAUSE,      1,           taskmng_meter_file_modify_deal},             //电表档案修改处理
    {MS_RELAY_TRANS,    5,      RCS_PAUSE,      0,            taskmng_relay_trans_deal},                  //检测透明转发
    {MS_FILE_TRANSFER,  6,      RCS_PAUSE,      0,            taskmng_file_transfer_deal},                  //检测透明转发
    {MS_EVENT_REPORT,   7,      RCS_PAUSE,       3,          EventReportDeal},               //电表事件上报任务
    {MS_NODE_VERSION_GET,8,      RCS_NULL,       5,          taskmng_node_version_info_get},               //节点模块版本信息查询
#ifdef AREA_HUNAN
    {MS_STA_CURVE_STATUS_GET,9,      RCS_NULL,       5,      taskmng_sta_curve_state_get}, //湖南查询STA模块配置状态
#endif
    {MS_RENETWORKING,   10,     RCS_NULL,    5,           ReNetworking},               //重组网任务
    {MS_METER_TIME_MANAGER,   11,      RCS_NULL,  1,          taskmng_meter_time_manager},       //电表上电后的时间管理
    {MS_METER_TIME_DELAY,   12,      RCS_NULL,  1,          taskmng_meter_time_delay},       //电表避开整点5分钟后的校时
    //全能测试要在抄表过程中校时，所以把对时的优先级放高点
    {MS_BROAD_TIMING,   13,      RCS_NULL,  5,              BroadTiming},       //广播对时
    {MS_NORMAL_AMR,     14,     RCS_NULL,   1,           taskmng_data_reading_normal},               //698规约任务抄表
    {MS_PATCH_AMR,     15,     RCS_NULL,    5,           PatchDataReading},         //补抄功能

    {0xFF,              0xFF,   RCS_NULL,       60,           NULL},
};
/*******************************************************************************
* 函数名称: taskmng_task_record_file_del_que_clear
* 函数功能: 清空存放需要删除的任务记录文件的队列
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_record_file_del_que_clear(uint8 pipe)
{
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    taskmng_db_update_queue_read(&pTaskRecordFileDel[pipe], &dbupdate);

}

 OOP_NODEPHASEINFO_T gettmpphase(uint16 Pn)
 {
//    uint8 outdata=0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[0];
    uint32 len;
    uint8 ret;
    OOP_NODEPHASEINFO_T PnPhase;
    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = Pn;
    NormalOad.oad.value = 0xF2090A00;
    memset(&PnPhase,0x0,sizeof(OOP_NODEPHASEINFO_T));
    ret=db_read_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,sizeof(OOP_NODEPHASEINFO_T),(uint8*)&PnPhase,&len);
    if(ret<0)
    {
      PDS_FMT_DEBUG("Pn %d 读相位信息出错,按照默认解析 \n", Pn);
      return PnPhase;
    }
    return PnPhase;
 }

int GetEverPnPhase(uint16 pn)
{
    OOP_NODEPHASEINFO_T Pn_Phase;
    uint16 tmp;
    Pn_Phase = gettmpphase(pn);
//    if(sizeof(Pn_Phase)==0)       //读节点信息失败，直接返回
//      return 0;
    tmp = Pn_Phase.info&0x07;
   // PDS_FMT_DEBUG("表节点 Pn %d 相位tmp %d\n",pn,tmp);
    if(tmp==1)
        return 1;
    if(tmp == 2)
        return 2;
    if(tmp == 4)
        return 3;
    else
        return 0;
}

int From_adr_get_phase(uint8 *addr)
{
    OOP_TSA_T tsa;
    uint16 Pn;
    memset(&tsa,0,sizeof(OOP_TSA_T));
    memcpy_r(&tsa.add,addr,6);
    Pn=taskmng_pn_get_from_tsa(tsa,0);
    return GetEverPnPhase(Pn);

}

/*******************************************************************************
* 函数名称: taskmng_meter_search_result_write
* 函数功能: //将搜表结果写入数据中心
* 输入参数: pipe 端口号
        buf 上报的搜表数据
        len  长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_meter_search_result_write(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MeterNum;

    uint16 index=0;
    uint8 prtl;
    int ret;
    uint8 optional;
    OOP_SEARCHMET_RES_T  SearchResult;
    uint8 collAdd[6]={0};

    memset(&SearchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    uint8 Addr[6]={0};

    time_t timenow = time(NULL);
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0x60020200;
    NormalOad.classtag = 3;
    uint8 i,j;
    uint8 k;

    index+=4;
    optional = buf[index++];

    if(optional== 0x01 )
    {
        index+=8 ;      //8个字节附加信息
    }
    MeterNum = buf[index++];
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "搜表上报结果 档案个数 %d \n",MeterNum);

    SearchResult.serchTime = Time_tToDataTimeS(timenow);
    for(i=0;i<MeterNum;i++)
    {

        memcpy_r(Addr,&buf[index],6);
        index+=6;
        memcpy(collAdd,&buf[index],6);
        index+=6;
        prtl = buf[index++];
        //index++;
        if(taskmng_addr_isvalid_check(Addr,6)<0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址非法 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
            continue;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "采集器地址 0x%02x%02x%02x%02x%02x%02x \n",collAdd[5],collAdd[4],collAdd[3],collAdd[2],collAdd[1],collAdd[0]);
        if(taskmng_addr_isvalid_check(collAdd,6)==1)
        {
            for(j =0;j<Collector.count;j++)
            {
                if(memcmp(Collector.collectaddr[j].addr,collAdd,6)==0)
                {
                    for(k=0;k<Collector.collectaddr[j].meterinfo.num;k++)
                    {
                        if(memcmp((uint8*)&Collector.collectaddr[j].meterinfo.meteraddr[k][0],Addr,6)==0)
                        {
                            break;
                        }
                    }
                    if(k>=Collector.collectaddr[j].meterinfo.num)
                    {
                        memcpy((uint8*)&Collector.collectaddr[j].meterinfo.meteraddr[k][0],Addr,6);
                        Collector.collectaddr[j].meterinfo.num++;
                    }
                    break;
                }
            }
            if(j>=Collector.count)
            {
                Collector.count++;

                TASK_FMT_DEBUG( pipe, RELAY_LOG, "Collector.count is %d\n",Collector.count);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "新采集器地址 0x%02x%02x%02x%02x%02x%02x \n",collAdd[5],collAdd[4],collAdd[3],collAdd[2],collAdd[1],collAdd[0]);
                memcpy(Collector.collectaddr[j].addr,collAdd,6);
                Collector.collectaddr[j].prtl = 3;
                Collector.collectaddr[j].pn = cajpn++;
                memcpy((uint8*)&Collector.collectaddr[j].meterinfo.meteraddr[0][0],Addr,6);
                Collector.collectaddr[j].meterinfo.num++;
            }
        }
        SearchResult.commAdd.len = 5;
        memcpy_r(SearchResult.commAdd.add,Addr,6);

        SearchResult.collAdd.len = 5;
        memcpy(SearchResult.collAdd.add,collAdd,6);

        SearchResult.proto = prtl;
        SearchResult.nAdded = 2;
        SearchResult.phase = From_adr_get_phase(Addr);
        SearchResult.info[0].oad.value = 0x60060200;    //端口
        SearchResult.info[0].value.type = DT_OAD;
        SearchResult.info[0].value.oad = taskmng_pipe_oad_get( pipe);
        SearchResult.info[1].oad.value = 0x60070200;        //波特率
        SearchResult.info[1].value.type = DT_ENUM;
        SearchResult.info[1].value.byVal = 255;     //波特率自适应
        //#if(!defined AREA_JIANGSU)
        if(CheckMeterSearchResult(pipe,SearchResult,&NormalOad.infoNum)==FALSE)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址重复直接过滤 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
            continue;
        }


        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&SearchResult,sizeof(OOP_SEARCHMET_RES_T));

        if(ret ==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表结果写入成功 index %d prtl %d  \n",NormalOad.infoNum,prtl);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表地址 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_event_report_response
* 函数功能:组上报的时间确认帧
* 输入参数:pipe 端口号


* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_event_report_response(uint8 pipe,OOP_TSA_T *tsa,uint8 choice,uint8 oadnum, OOP_OAD_U *oad)
{
    uint8 indata[100] = {0};
    uint16 inlen = 0;
    LinkLayer_t *pLinkLayer = NULL;
    uint8 i;
    indata[inlen++] = oadnum;
    for(i=0;i<oadnum;i++)
    {
        memcpy_r(&indata[inlen],&oad[i].value,4);
        inlen+=4;
    }
    OopReport.response(pipe,tsa,choice,indata,inlen);
    pLinkLayer = FindLinkLayer(pipe, NULL);

    if(pLinkLayer->sendLen>MAX_MSG_LEN)
        return ;

    uint8 payload[1000]={0};
    uint16 index = 0;
    uint16 overtime = 0xffff; //帧超时时间改为0XFFFF ccoRouter会特殊处理 用AFNF1F02来下发
    uint16 msgindex;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    payload[index++] = PRTL_OOP;
    memcpy(&payload[index],tsa->add,6);
    index+=6;
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    if(pLinkLayer->sendLen<0x80)
    {
        payload[index++] = pLinkLayer->sendLen;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(pLinkLayer->sendLen>>8);
        payload[index++] = (uint8)pLinkLayer->sendLen;
    }
    memcpy(&payload[index],pLinkLayer->sendBuff,pLinkLayer->sendLen);
    index+=pLinkLayer->sendLen;

    taskmng_msg_data_pack(pipe,0x0003,0x0038,index,payload,&msgindex);
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"电表事件上报发送确认报文\n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,payload, index);

}

/*******************************************************************************
* 函数名称: taskmng_event_report_3320_check
* 函数功能://检测上报的事件是否为698的3320对象
* 输入参数:pipe 端口号
        buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_event_report_3320_check(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U RecvOad;
    uint16 SubOadcount;
    OOP_OAD_U SubOad[MAX_OAD_NUM];
    uint8 i,j,k;
    int ret;
    uint8 RecvBuf[1000]={0};
    uint16 RecvLen=0;
    uint16 Pn,newpn;
    OOP_OAD_U EventOadList[MAX_OAD_NUM]={0};
    uint16 EventOadNum=0;
    uint8 prtl = PRTL_OOP;
    AppLayerList_t *pAppLayer = NULL;
    OOP_TSA_T tsa;
    uint8 reportState[4]={0}; //记录下上报的状态字 复位时用
    OOP_OAD_U oadlist[10]={0};
    ret = LinkDataReceive(pipe, buf, len,NULL);
    if(ret >=eOopComm_Fail)//解帧失败直接返回
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"事件上报报文按照698协议解帧失败 ret %d \n",ret);
        return;
    }

    pAppLayer = FindAppLayer(pipe, NULL);
    if(pAppLayer == NULL)
    {
        return;
    }
    memset(RecvBuf,0x00,sizeof(RecvBuf));

    RecvLen = pAppLayer->recvApduList->apduLen;
    memcpy(RecvBuf, pAppLayer->recvApduList->apdu, RecvLen);

    TASK_FMT_DEBUG( pipe, RELAY_LOG,"收到上报的事件报文消息 len %d RecvBuf \n",RecvLen);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
    memcpy(&tsa,&pAppLayer->sa,sizeof(OOP_TSA_T));
    Pn = taskmng_pn_get_from_tsa(tsa,LOGIC_ID_YC);
    if(Pn == 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"上报的电表地址没有对应的档案 \n");
        return;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"上报的电表pn %d \n",Pn);
    uint16 index = 0;
    uint8 serID = RecvBuf[index++];
    uint8  choice;

    if((serID!=0x85)&&(serID!=0x88))
        return;
    choice = RecvBuf[index++];
    index++;  //PIID

    //处理读取响应
    if(serID == 0x85)
    {
        switch(choice)
        {
        case 0x01:
        {
            memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
            index+=4;
            if(RecvOad.value!= 0x33200200)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"上报的不是3320200 \n");
                return;
            }
            if(RecvBuf[index++]==0) //1 DAR 1 DATA
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"上报的3320200无数据\n");
                return;
            }
            if(RecvBuf[index++]==1)
            {

                EventOadNum = RecvBuf[index++] ;
                for(j = 0;j<EventOadNum;j++)
                {
                    index++;//oad类型81
                    memcpy_r((uint8*)&EventOadList[j].value,&RecvBuf[index],4);
                    index+=4;
                }
            }
        }
            break;
        case 0x02:
        {
            uint8 normalcount = RecvBuf[index++];
            for(i=0;i<normalcount;i++)
            {
                memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
                index+=4;
                if(RecvOad.value== 0x33200200)
                {
                    if(RecvBuf[index++]==1)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"解析出3320200\n");
                        if(RecvBuf[index++]==1)
                        {
                            EventOadNum = RecvBuf[index++] ;
                            for(j = 0;j<EventOadNum;j++)
                            {
                                index++;//oad类型81
                                memcpy_r((uint8*)&EventOadList[j].value,&RecvBuf[index],4);
                                index+=4;
                            }
                        }
                        break;
                    }else
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320200无数据\n");
                        return;
                    }
                }else
                {
                    if(RecvBuf[index++]==0)
                        continue;
                    OadInfoMap pOadInfo;
                    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
                    ret = taskmng_oad_info_get( RecvOad,&pOadInfo);
                    if(ret == FALSE)
                    {
                        return;
                    }
                    //其他oad的暂不处理
                    index+=pOadInfo.dataLen;
                }
            }


        }
            break;
        default:
            return;
        }
    }

    //处理上报通知
    if(serID == 0x88)
    {
        switch(choice)
        {
        case 0x01:
        {
            uint8 normalcount = RecvBuf[index++];
            for(i=0;i<normalcount;i++)
            {
                memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
                index+=4;
                if(RecvOad.value== 0x33200200)
                {
                    if(RecvBuf[index++]==1)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"解析出3320200\n");
                        //把上报的事件都解出来
                        if(RecvBuf[index++] !=1 )//1代表array
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG,"数据类型错误\n");
                            return;
                        }
                        EventOadNum = RecvBuf[index++] ;
                        for(j = 0;j<EventOadNum;j++)
                        {
                            index++;//oad类型81
                            memcpy_r((uint8*)&EventOadList[j].value,&RecvBuf[index],4);
                            index+=4;
                        }
                        oadlist[0] = RecvOad;
                        taskmng_event_report_response( pipe,&tsa,1,1,oadlist);
                        break;
                    }else
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320200无数据\n");
                        return;
                    }
                }
                    else if(RecvOad.value== 0x20150200)
                {
                    if(RecvBuf[index++]==1)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"解析出20150200\n");
                        if(RecvBuf[index++] !=04 ) //bitstring
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG,"数据类型错误\n");
                            return;
                        }
                        if(RecvBuf[index++] != 32 ) //长度32
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG,"bitstring长度不对\n");
                            return;
                        }
                        //根据模式字获取事件oad
                        memcpy(reportState,&RecvBuf[index],4);
                        EventOadNum = taskmng_2015_event_oad_get(&RecvBuf[index],EventOadList);
                        index+=4;
                        prtl = PRTL_20_IOT;
                        oadlist[0] = RecvOad;
                        taskmng_event_report_response( pipe,&tsa,1,1,oadlist);
                        tsa.oldvxd  = 1;
                        newpn = taskmng_pn_get_from_tsa(tsa,LOGIC_ID_YC);
                        if(newpn == 0)
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG,"未找到逻辑地址为1的对应档案\n");
                        }else
                        {
                            Pn = newpn;
                        }
                        break;
                    }else
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"20150200无数据\n");
                        return;
                    }
                }
                    else
                {
                    if(RecvBuf[index++]==0)
                        continue;
                    OadInfoMap pOadInfo;
                    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
                    ret = taskmng_oad_info_get( RecvOad,&pOadInfo);
                    if(ret == FALSE)
                    {
                        return;
                    }
                    //其他oad的暂不处理
                    index+=pOadInfo.dataLen;
                }
            }
        }
            break;
        case 0x02:
        {
            //上报记录型对象 直接组织成6012格式存储到数据中心
            uint8 num = RecvBuf[index++];
            uint16 offset = 0;
            for(i=0;i<num;i++)
            {
                memcpy_r(&oadlist[i].value,&RecvBuf[index],4);
                offset = event_report_data_save(pipe,RecvBuf+index,RecvLen-index,Pn);
                index +=offset;
            }
            taskmng_event_report_response( pipe,&tsa,2,num,oadlist);
            return;
        }
            break;
        default:
            return;
        }
    }

    //uint8 OadCount = RecvBuf[index++] ;
    uint8 eventindex=0;
    if(EventOadNum == 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"上报的事件无法找到对应oad\n");
        return ;
    }
    for(i=0;i<MAX_EVENT_MTR;i++)
    {
        if(pAmrThreadArg->EventReport.MtrEvent[i].Pn == 0)
        {
            memset(&pAmrThreadArg->EventReport.MtrEvent[i],0x00,sizeof(MTR_EVENT_REPORT_T));
            pAmrThreadArg->EventReport.MtrEvent[i].Pn =Pn;
            pAmrThreadArg->EventReport.MtrEvent[i].prtl =prtl;
            pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
            if(prtl == PRTL_20_IOT)
            {
                memcpy(pAmrThreadArg->EventReport.MtrEvent[i].ReportState,reportState,4);
            }
            for(j=0;j<EventOadNum;j++)
            {
                //index++;//oad类型81
               // memcpy_r((uint8*)&RecvOad.value,&RecvBuf[index],4);
                //index+=4;
                memset(SubOad,0x00,sizeof(OOP_OAD_U)*MAX_OAD_NUM);
                SubOadcount = GetEventSubOad(Pn,EventOadList[j],SubOad,&newpn);
                if(SubOadcount==0)
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件 oad %08x 未找到对应的方案,检查是否存在默认方案\n",EventOadList[j].value);
                    //if(EventOadList[j].value == 0x30150200 || EventOadList[j].value == 0x30200200 || EventOadList[j].value == 0x305a0200  ||
                    //    EventOadList[j].value == 0x305b0200 || EventOadList[j].value == 0x301B0200)
                    {
                        //先改为全部采用默认抄读
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"事件 oad %08x 存在默认方案采用方案抄读\n",EventOadList[j].value);
                        pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].EventDI = EventOadList[j];
                        pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].SubOadNum = 4;
                        memcpy(pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].SubOad,g_report_301f_obj,sizeof(g_report_301f_obj));
                        eventindex++;
                        //pAmrThreadArg->EventReport.MtrEvent[MtrIndex].EventCount = eventindex;
                        //pAmrThreadArg->EventReport.MtrEvent[MtrIndex].state = EVENT_GET_EVENT;
                        break;
                    }
                    //continue;
                }
                pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].EventDI = EventOadList[j];
                pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].SubOadNum = SubOadcount;
                memcpy(pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[eventindex].SubOad,SubOad,sizeof(OOP_OAD_U)*SubOadcount);
                eventindex++;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"3320事件列表 count %d i %d mainoad %08x \n",EventOadNum,i,EventOadList[j].value);
                for(k=0;k<SubOadcount;k++)
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG,"suboad index %d  %08x \n",k,SubOad[k].value);
                }
            }
            pAmrThreadArg->EventReport.MtrEvent[i].Pn =newpn;

            pAmrThreadArg->EventReport.MtrEvent[i].EventCount = eventindex;
            pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "添加事件上报任务 i %d newpn %d \n",i,newpn);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            if(eventindex==0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"没有需要抄读的事件 \n");
                pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_ONEMTR_END;
            }
            break;
        }
    }
}


/*******************************************************************************
* 函数名称: taskmng_event_report_amr_fun
* 函数功能://抄表线程处理收到的上报状态字
* 输入参数:pipe 端口号
        buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_event_report_amr_fun(uint8 pipe,uint8* buf,uint16 len)
{
    int ret;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8  EventStateDI[4] = {0x34, 0x48, 0x33, 0x37}; //主动上报状态字的ID  0x04001501
    uint16 i;
    uint8 Addr[6]={0};

    uint8 data[300] = {0};  //从状态字开始的数据域
    uint8 datalen;         //数据域长度
    uint16 StateLen;            //完整的上报状态字 645报文长度
    OOP_OAD_U pipeOad;
    uint16 Pn;
    uint8 prtl;
    uint16 reallen=0;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "收到上报的状态字 len %d  data \n",len);

    TASK_BUF_DEBUG( pipe, RELAY_LOG, buf, len);
    pipeOad = taskmng_pipe_oad_get( pipe);
    uint8 *prt =(uint8*)commfun_645_frame_get((char*)buf,0x68,len,&reallen);
    if(prt == NULL)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "不是645报文  \n");
        //检查是否698的3320
        taskmng_event_report_3320_check(pipe,buf,len);
        return ;
    }
    memcpy(Addr,prt+1,6);
    if(taskmng_addr_isvalid_check(Addr,6)<0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "上报的地址非法！  \n");
        return;
    }
    if(prt[8]==0x9E && prt[9]==0x07 && prt[10]==0x36)
    {
        //非本台区电表信息上报
        time_t timenow = time(NULL);
        uint16 infonum = 0;
        OOP_ACROSSAREA_RES_T    AcrossArea;
        memset(&AcrossArea,0x00,sizeof(OOP_ACROSSAREA_RES_T));
        AcrossArea.commAdd.len = 5;
        memcpy_r(AcrossArea.commAdd.add,Addr,6);
        memcpy(data,prt+11,6);
        datalen = 6;
        Pro_ReduceMeterGap(data,datalen,0x33);
        AcrossArea.netAdd.len = 5;
        memcpy_r(AcrossArea.netAdd.add,data,6);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "收到非本台区电表信息上报 通信地址 以及主节点地址 \n");
        TASK_BUF_DEBUG( pipe, RELAY_LOG,Addr,6);
        TASK_BUF_DEBUG( pipe, RELAY_LOG,data,6);
        DT_Time2DateTimeShort(timenow,&AcrossArea.change);
        if(CheckAcrossAreaResult(Addr,&infonum)==FALSE)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "该地址已上报过，直接过滤！ \n");
            return ;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "写入非本台区电表上报记录 infonum %d \n",infonum);
        taskmng_normal_data_write_pipe(pipe,(uint8*)&AcrossArea,sizeof(OOP_ACROSSAREA_RES_T),AcrossAreaOad,infonum,0);
        return;
    }
    Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC) ;
    taskmng_prtl_get_from_addr(Addr,LOGIC_ID_YC,&prtl);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 pn %d prtl %d \n",Pn,prtl);
    if(Pn== 0)
    {
        return;
    }
    if(prtl!=PRTL_OOP && prtl!=PRTL_07)
        return;
    if(memcmp(&prt[10],EventStateDI,4)!=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "不是04001501  \n");
        return ;
    }
    if(prt[9]<4+12) //di4 状态字12 两个AAH分隔符 状态字全0的时候可能没有AA
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据域长度 %d  \n",prt[9]);
        return;
    }
    StateLen = 10+prt[9]+2; //68 地址 68 控制域 长度域 数据域 CS 16
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "报文长度 %d \n",StateLen);
    for(i=0;i<MAX_EVENT_MTR;i++)
    {
        //如果当前事件结构中有这个电表 则说明不是主动上报的 是主动抄的
        //抄读过程中再收到主动上报状态字不做处理 抄完后还会再读一遍状态字
        if(pAmrThreadArg->EventReport.MtrEvent[i].Pn == Pn )
        {
//            if(prtl == PRTL_OOP)
//                return;
//            TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前电表Pn %d 收到抄的状态字 \n",Pn);
//            datalen = prt[9] -4; //减去4个字节DI
//            memcpy(data,prt+14,datalen);
//            Pro_ReduceMeterGap(data,datalen,0x33);
//            ReportEventStateFun( pipe,data,datalen,i);
//            pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
//
//            //第一次抄的主动上报状态字要写入总数据中 如果事件次数无效就存 如果有具体的事件就不存
//            //测试时有主动上报两个事件的 一个事件需要抄一个事件不需要 这种情况下需要存主动上报状态字
//            if(pAmrThreadArg->EventReport.MtrEvent[i].DataLen ==0 )
//            {
//                memcpy_r(&pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[0],(uint8*)&pipeOad.value,4);
//                pAmrThreadArg->EventReport.MtrEvent[i].DataLen = 4;
//                //if(pAmrThreadArg->EventReport.MtrEvent[i].EventCount==0)
//                {
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataLen +=(1+1+StateLen);
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[4] = 1;
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[5] = StateLen; //主动上报状态字最多不会超过127 所以长度占一个字节
//                    memcpy(&pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[6],prt,StateLen);
//                }
//            }else
//            {
//                //复位后的主动上报状态字不存
//                //测试时复位状态字后继续抄的状态字还会有事件 此时也在该逻辑中
//                if(pAmrThreadArg->EventReport.MtrEvent[i].EventCount!=0 || datalen>(12+2) )
//                {
//                    uint16 currentlen = pAmrThreadArg->EventReport.MtrEvent[i].DataLen;
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[currentlen] = StateLen;
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataLen++;
//                    memcpy(&pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[currentlen+1],prt,StateLen);
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataLen+=StateLen;
//                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[4]++;
//                }
//            }
//            TASK_FMT_DEBUG( pipe, RELAY_LOG,"645总数据 len %d Buf \n",pAmrThreadArg->EventReport.MtrEvent[i].DataLen);
//            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->EventReport.MtrEvent[i].DataBuf, pAmrThreadArg->EventReport.MtrEvent[i].DataLen);

            return;
        }
    }
    for(i=0;i<MAX_EVENT_MTR;i++)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, " i %d pn %d \n",i,pAmrThreadArg->EventReport.MtrEvent[i].Pn);
        if(pAmrThreadArg->EventReport.MtrEvent[i].Pn == 0)
        {
            //拓扑识别测试时发现会有断路器上报空的状态字 ，此时再进入事件上报逻辑会有问题
            if(prtl == PRTL_07 && prt[9] == 12+4)
                return;
            memset(&pAmrThreadArg->EventReport.MtrEvent[i],0x00,sizeof(MTR_EVENT_REPORT_T));
            pAmrThreadArg->EventReport.MtrEvent[i].Pn =Pn;
            pAmrThreadArg->EventReport.MtrEvent[i].prtl =prtl;
            if(prtl == PRTL_07)
            {
                //645收到上报状态字后进行处理 根据上报的状态字计算出需要抄读的事件
                datalen = prt[9] -4; //减去4个字节DI
                memcpy(data,prt+14,datalen);
                Pro_ReduceMeterGap(data,datalen,0x33);
                ret = ReportEventStateFun( pipe,data,datalen,i);
                //根据状态字判断逻辑

                if(ret == 1)
                {
                    pAmrThreadArg->EventReport.MtrEvent[i].state =EVENT_RESET_STATE;
                }else if(ret == 2)
                {
                    pAmrThreadArg->EventReport.MtrEvent[i].state =EVENT_GET_EVENT;
                }else
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "状态字全空\n");
#ifdef AREA_SHANDONG
                    /* 对于量测开关，收到的上报状态字为空，需要抄读运行状态字2 */
                    uint8       useType;
                    useType = taskmng_usertype_get_from_no(Pn, LOGIC_ID_YC);
                    if (useType == USERTYPE_BREAKER_SD)
                    {
                        pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
                        pAmrThreadArg->EventReport.MtrEvent[i].EventCount = 1;
                        pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[0].count = 1;
                        pAmrThreadArg->EventReport.MtrEvent[i].EventInfo[0].AllDI[0] = 0x04000502;
                    }
                    else
                    {
                        pAmrThreadArg->EventReport.MtrEvent[i].Pn =0;
                        return;
                    }
#else
                    pAmrThreadArg->EventReport.MtrEvent[i].Pn =0;
                    return;
#endif

                }
                //记录上报的12个字节状态字 供复位时使用
                memcpy(pAmrThreadArg->EventReport.MtrEvent[i].ReportState,data,12);
                //pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENT;
                memcpy_r(&pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[0],(uint8*)&pipeOad.value,4);
                pAmrThreadArg->EventReport.MtrEvent[i].DataLen = 4;
                //if(pAmrThreadArg->EventReport.MtrEvent[i].EventCount==0)
                {
                    pAmrThreadArg->EventReport.MtrEvent[i].DataLen+=(1+1+StateLen);
                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[4] = 1;
                    pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[5] = StateLen; //主动上报状态字最多不会超过127 所以长度占一个字节
                    memcpy(&pAmrThreadArg->EventReport.MtrEvent[i].DataBuf[6],prt,StateLen);
                }
            }else if(prtl == PRTL_OOP)
            {
                //698表收到上报状态字后 直接采集3320
                pAmrThreadArg->EventReport.MtrEvent[i].state = EVENT_GET_EVENTLIST;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "添加事件上报任务 i %d  \n",i);
            taskmng_plc_task_queue_add(pipe,MS_EVENT_REPORT);
            pAmrThreadArg->EventReport.AllCount++;
            break;
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice1
* 函数功能://处理上报的HPLC信息 1 网络拓扑信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice1(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_NETTOPOLOGYINFO_T NetTopo;

    uint8 i;
    uint16 index = 0;
    uint16 count;
    uint8 Addr[6]={0};
    OOP_OAD_U OAD = {0XF2090700};
    count = buf[index++];
    uint16 Pn;
    if(index+count*11!=len)
        return;

    for(i=0;i<count;i++)
    {
        memset(&NetTopo,0x00,sizeof(OOP_NETTOPOLOGYINFO_T));
        memcpy_r(Addr,&buf[index],6);
        memcpy(NetTopo.addr.nValue,&buf[index],6);
        index+=6;
        if(memcmp(LocalAddr,Addr,6)==0)
        {
            NetTopo.type = 0;    //主节点
        }
        else
        {
            NetTopo.type = 1;       //从节点
        }

        Pn = taskmng_net_topology_seq_get();

        memcpy_r((uint8*)&NetTopo.nid ,&buf[index],2);
        index+=2;
        memcpy_r((uint8*)&NetTopo.proxynid,&buf[index],2);
        index+=2;
        NetTopo.info = buf[index++];
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "上报网络拓扑信息总个数 %d i %d Pn %d  \n",count,i,Pn);
        taskmng_normal_data_write_pipe(pipe,(uint8*)&NetTopo,sizeof(OOP_NETTOPOLOGYINFO_T),OAD,Pn,0);
    }

}

/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice2
* 函数功能://处理上报的HPLC信息 2宽带载波芯片信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice2(uint8 pipe,uint8* buf,uint16 len )
{
    uint8 i;
    uint16 index = 0;
    uint16 count;
    uint16 seq;
    uint8 Addr[6]={0};
    uint8 devicetype ;
    uint8 ICID[30]={0};
    uint8 ICVersion[4]={0};
    int ret;
    uint16 DataLen;
    OOP_OAD_U OAD = {0XF2090500};
    count = buf[index++];
    for(i=0;i<count;i++)
    {
        memcpy_r((uint8*)&seq,&buf[index],2);
        index+=2;
        memcpy_r(Addr,&buf[index],6);
        index+=6;
        devicetype = buf[index++];
        memcpy(ICID,&buf[index],24);
        index+=24;
        memcpy_r(ICVersion,&buf[index],2);
        index+=2;
        if(seq == 1)    //表示主节点 先读F2090200看是否在 如果有则更新 没有就新建
        {
            OAD.value = 0xF2090200;
            ROUTEMODULE RouterModule;
            memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
            DESCRIPTOR_T Descriptor;
            memset(&Descriptor,0x00,sizeof(DESCRIPTOR_T));
            char tmp[300]={0};
            uint16 infonum = pipe+1 ;
            ret = taskmng_normal_data_read_pipe( pipe,OAD, infonum, 0, (uint8 *) &RouterModule, &DataLen);
            //数据中心还没写主节点信息 写进去
            if(ret!=0 || DataLen!=sizeof(ROUTEMODULE))
            {
                memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
            }else
            {
                Descriptor = UnpackDescriptor(0,RouterModule.name.value);

            }
            Descriptor.idformat = 2;
            memcpy(Descriptor.id,ICID,24);
            //全叫PLC
//            if(ICID[6]==1)
//            {
//                strcpy(Descriptor.type,"PLC");
//            }else
//            {
//                strcpy(Descriptor.type,"HPLC");
//            }
#ifdef AREA_HUBEI
            strcpy(Descriptor.type,"HPLC");
#else
            strcpy(Descriptor.type,"PLC");
#endif
            if(ICID[7]==0||ICID[7]==0XFF)
            {
                Descriptor.mfrs[0] = 0x2d;
            }else
            {
                Descriptor.mfrs[0] = ICID[7];
            }
            if(ICID[8]==0||ICID[8]==0XFF)
            {
                Descriptor.mfrs[1] = 0x2d;
            }else
            {
                Descriptor.mfrs[1] = ICID[8];
            }
            if(Descriptor.mmfrs[0]==0)
            {
                Descriptor.mmfrs[0] = 0x2d;
            }
            if(Descriptor.mmfrs[1]==0)
            {
                Descriptor.mmfrs[1] = 0x2d;
            }
            MakeDescriptor( 0,Descriptor, tmp);
            RouterModule.name.nNum = strlen(tmp);
            memcpy(RouterModule.name.value,tmp,strlen(tmp));
            //给通讯参数赋个默认值
            taskmng_comdcb_default(&RouterModule.dcb);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "端口描述符 %s   \n",tmp);
            taskmng_normal_data_write_pipe( pipe, (uint8*)&RouterModule, sizeof(ROUTEMODULE), OAD, infonum, 0);
        }
        else
        {
            OAD.value = 0xF2090500;
            OOP_CHILDNODE_T ChildNode;
            memset(&ChildNode,0x00,sizeof(OOP_CHILDNODE_T));
            DESCRIPTOR_T Descriptor;
            memset(&Descriptor,0x00,sizeof(DESCRIPTOR_T));
            char tmp[300]={0};
            uint16 Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
            if(Pn == 0)
                continue;
            ret = taskmng_normal_data_read_pipe( pipe,OAD, Pn, 0, (uint8 *) &ChildNode, &DataLen);
            if(ret!=0||DataLen!=sizeof(OOP_CHILDNODE_T))
            {
                memset(&ChildNode,0x00,sizeof(OOP_CHILDNODE_T));
                ChildNode.index = Pn;
                memcpy_r(ChildNode.add.add,Addr,6);
                ChildNode.add.len = 6;

            }else
            {
                Descriptor = UnpackDescriptor(1,ChildNode.id.value);

            }
            Descriptor.type[0] = devicetype;
            Descriptor.idformat = 2;
            memcpy(Descriptor.id,ICID,24);
            if(ICID[7]==0||ICID[7]==0XFF)
            {
                Descriptor.mfrs[0] = 0x2d;
            }else
            {
                Descriptor.mfrs[0] = ICID[7];
            }
            if(ICID[8]==0||ICID[8]==0XFF)
            {
                Descriptor.mfrs[1] = 0x2d;
            }else
            {
                Descriptor.mfrs[1] = ICID[8];
            }
            if(Descriptor.mmfrs[0]==0)
            {
                Descriptor.mmfrs[0] = 0x2d;
            }
            if(Descriptor.mmfrs[1]==0)
            {
                Descriptor.mmfrs[1] = 0x2d;
            }
            MakeDescriptor(1, Descriptor, tmp);
            ChildNode.id.nNum = strlen(tmp);
            memcpy(ChildNode.id.value,tmp,strlen(tmp));
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "从节点描述符 %s   \n",tmp);
            taskmng_normal_data_write_pipe( pipe, (uint8*)&ChildNode, sizeof(OOP_CHILDNODE_T), OAD, Pn, 0);
        }
    }
}



/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice3
* 函数功能://处理上报的HPLC信息 3 多网络信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice3(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0xF2090800};
    uint8 i;
    OOP_NETSINFO_T NetsInfo;
    memset(&NetsInfo,0x00,sizeof(OOP_NETSINFO_T));
    uint16 index=0;
    uint8 count =0;

    NetsInfo.nNum = buf[index++];   //邻居节点个数
    memcpy_r(NetsInfo.nodenid.nValue,&buf[index],3);
    index+=3;
    memcpy(NetsInfo.addr.nValue,&buf[index],6);
    index+=6;
    count = buf[index++];
    for(i=0;i<count;i++)
    {
        memcpy_r(NetsInfo.neighbour[i].nValue,&buf[index],3);
        index+=3;
    }
    uint16 infonum = pipe+1 ;
    taskmng_normal_data_write_pipe(pipe,(uint8*)&NetsInfo,sizeof(OOP_NETSINFO_T),OAD,infonum,0);
}

/*******************************************************************************
* 函数名称: taskmng_phase_info_unpack
* 函数功能://解析相位信息 相位信息：D0-D2；置“1”依次表示第1、2、3相，置“1”电表接入对应相位，置“0”表示未接入该相位或该相位断相；三相均为0，表示不支持或者正在识别中。
                        //D7～D5：三相位相序类型，
* 输入参数:info 节点相位信息
        phase  相位信息
        phaseseq 相序
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_phase_info_unpack(uint16 info ,uint8* phase,uint8* phaseseq)
{
    uint8 tmp;
    tmp = info&0x07; //取低三位
    if(tmp == 1)
        *phase = 1;
    else if(tmp==2)
        *phase = 2;
    else if(tmp==4)
        *phase = 3;
    else if(tmp == 0)
        *phase = 0;
    else
        *phase = 4;//增加4 三相
    tmp = (info&0xE0)>>5;
    if(tmp == 0)
        *phaseseq = 0;
    else if(tmp ==6 ) //零火反接
        *phaseseq = 1;
    else
        *phaseseq = 2;
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice4
* 函数功能://处理上报的HPLC信息 4 节点相线信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice4(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    int16 MtrIndex;
    OOP_OAD_U OAD = {0xF2090A00};
    OOP_NODEPHASEINFO_T NodePhaseInfo;
    memset(&NodePhaseInfo,0x00,sizeof(OOP_NODEPHASEINFO_T));
    uint16 count=0 ;
    uint16 i;
    uint16 index=0;
    uint8 Addr[6]={0};
    uint16 Pn;
    uint16 info;
    uint16 seq;
    count = buf[index++];
    for(i=0;i<count;i++)
    {
        memcpy_r(&seq,&buf[index],2);
        index+=2;   //不用报上来的序号
        memcpy_r(Addr,&buf[index],6);
        index+=6;
        memcpy_r((uint8*)&info,&buf[index],2);
        index+=2;
        //现在不过滤主节点
        //有交采档案的情况下主节点会查到到交采档案对应的pn
        //台体测试发现终端地址设置为100000000001 会和电表地址一样 导致写结果的时候少一个
        if(seq == 1)
        {
            Pn = 0;
        }else
        {
            Pn = taskmng_pn_get_from_addr( Addr,LOGIC_ID_YC);
            if(Pn==0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "seq %d 上报的地址找不到对应序号 \n",seq);
                TASK_BUF_DEBUG(pipe, RELAY_LOG, Addr,6);

            }else
            {

                MtrIndex = taskmng_ms_index_get(pipe, Pn,LOGIC_ID_YC);
                if(MtrIndex!=-1)
                {
                    taskmng_phase_info_unpack(info,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].phase,&pAmrThreadArg->pMtrAmrInfo[MtrIndex].phaseseq);
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "pn %d 相位 %d 相序 %d\n",Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].phase,
                                pAmrThreadArg->pMtrAmrInfo[MtrIndex].phaseseq);

                }

            }
        }

        NodePhaseInfo.index = Pn;
        NodePhaseInfo.addr.nNum = 6;
        memcpy_r(NodePhaseInfo.addr.value,Addr,6);
        NodePhaseInfo.info = info;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "写节点相线信息 pn %d info %d 0x%02x%02x%02x%02x%02x%02x\n",Pn,info,
            Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        taskmng_normal_data_write_pipe(pipe,(uint8*)&NodePhaseInfo,sizeof(OOP_NODEPHASEINFO_T),OAD,Pn,0);
    }
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice5
* 函数功能://处理上报的HPLC信息 5 网络规模
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice5(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0xF2091400};
    uint16 NetScale;
    memcpy_r((uint8*)&NetScale,buf,2);
#ifdef AREA_HUBEI
    //湖北的网络规模要加上主节点
    NetScale++;
#endif
    uint16 infonum = pipe+1 ;
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "网络规模 %d \n",NetScale);

    taskmng_normal_data_write_pipe(pipe,(uint8*)&NetScale,2,OAD,infonum,0);
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice6
* 函数功能://处理上报的HPLC信息 6 本地主节点模块ID信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice6(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0xF2090200};
    uint16 index=0;
    uint8 ModuleCode[2]={0};
    //uint8 ModuleLen;
    uint8 ModuleType;
    uint16 ModuleIDLen;
    uint8 lenoffset;
    uint8 ModuleID[300]={0};
    int ret;
    uint8 i;
    uint16 DataLen;
    //ECU的ccoRouter台体测试的时候就回个00 此处做下容错 最少5个字节
    if(len < 5)
        return;
    memcpy(ModuleCode,&buf[index],2);
    index+=2;
    //ModuleLen = buf[index++];
    index++;
    ModuleType = buf[index++];
    ModuleIDLen = XADR_GetEleNum(&buf[index],&lenoffset);
    //模块id最多50个字节
    if(ModuleIDLen>50)
        ModuleIDLen = 50;
    index+=lenoffset;
    memcpy_r(ModuleID,&buf[index],ModuleIDLen);

    ROUTEMODULE RouterModule;
    memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
    DESCRIPTOR_T Descriptor;
    memset(&Descriptor,0x00,sizeof(DESCRIPTOR_T));
    char tmp[300]={0};
    uint16 infonum = pipe+1 ;
    ret = taskmng_normal_data_read_pipe( pipe,OAD,infonum, 0, (uint8 *) &RouterModule, &DataLen);
    //数据中心还没写主节点信息 写进去
    if(ret!=0 || DataLen!=sizeof(ROUTEMODULE))
    {
        memset(&RouterModule,0x00,sizeof(ROUTEMODULE));

    }else
    {
        Descriptor = UnpackDescriptor(0,RouterModule.name.value);
    }

    if(Descriptor.mfrs[0]==0)
    {
        Descriptor.mfrs[0] = 0x2d;
    }
    if(Descriptor.mfrs[1]==0)
    {
        Descriptor.mfrs[1] = 0x2d;
    }
    memcpy(Descriptor.mmfrs,ModuleCode,2);
    if(Descriptor.mmfrs[0]==0)
    {
        Descriptor.mmfrs[0] = 0x2d;
    }
    if(Descriptor.mmfrs[1]==0)
    {
        Descriptor.mmfrs[1] = 0x2d;
    }

    Descriptor.midformat = ModuleType;
    for(i=0;i<ModuleIDLen;i++)
    {
        sprintf(&Descriptor.mid[i*2],"%02x",ModuleID[i]);
    }

    MakeDescriptor(0, Descriptor, tmp);
    RouterModule.name.nNum = strlen(tmp);
    strcpy(RouterModule.name.value,tmp);
    taskmng_comdcb_default(&RouterModule.dcb);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "端口描述符 %s\n",tmp);
    taskmng_normal_data_write_pipe( pipe, (uint8*)&RouterModule, sizeof(ROUTEMODULE), OAD, infonum, 0);
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice7
* 函数功能://处理上报的HPLC信息 7 从节点ID信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice7(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0XF2090500};
    uint16 index=0;
    uint8 i,j;
    uint8 count ;
    uint8 Addr[6]={0};
    //uint8 type ;
    uint8 ModuleCode[2]={0};
    //uint8 ModuleLen;
    uint8 ModuleType;
    uint8 ModuleID[300]={0};
    uint16 ModuleIDLen;
    uint8 lenoffset;
    int ret;
    uint16 DataLen;
    count = buf[index++];
    for(i=0;i<count;i++)
    {
        index+=2;
        memcpy_r(Addr,&buf[index],6);
        index+=6;
        TASK_BUF_DEBUG(pipe, COMM_LOG, Addr,6);
        if(taskmng_check_caq_from_addr(Addr,0)==TRUE)
        {
            nodecount++;
            TASK_FMT_DEBUG(pipe, COMM_LOG, "nodecount is %d \n",nodecount);
            if((buf[index]&0x10)==0x00)
            {
                nodecollect = 0x55;//表示整个台区并不都是支持分钟采集
                TASK_FMT_DEBUG(pipe, COMM_LOG, "nodecollect %d \n",nodecollect);
            }
        }
        index++;
        //type = buf[index++];
        memcpy(ModuleCode,&buf[index],2);
        index+=2;
        //ModuleLen = buf[index++];
        index++;
        ModuleType = buf[index++];
        ModuleIDLen = XADR_GetEleNum(&buf[index],&lenoffset);
        //模块id最多50个字节
        if(ModuleIDLen>50)
            ModuleIDLen = 50;
        index+=lenoffset;
        memcpy_r(ModuleID,&buf[index],ModuleIDLen);
        index+=ModuleIDLen;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "从节点模块id len %d  lenoffset %d %x \n",ModuleIDLen,lenoffset,ModuleID[0]);
        TASK_BUF_DEBUG(pipe, RELAY_LOG,ModuleID,10);
        OOP_CHILDNODE_T ChildNode;
        memset(&ChildNode,0x00,sizeof(OOP_CHILDNODE_T));
        DESCRIPTOR_T Descriptor;
        memset(&Descriptor,0x00,sizeof(DESCRIPTOR_T));
        char tmp[300]={0};
        uint16 Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
        if(Pn == 0)
            continue;
        ret = taskmng_normal_data_read_pipe( pipe,OAD, Pn, 0, (uint8 *) &ChildNode, &DataLen);
        if(ret!=0||DataLen!=sizeof(OOP_CHILDNODE_T))
        {
            memset(&ChildNode,0x00,sizeof(OOP_CHILDNODE_T));
            ChildNode.index = Pn;
            memcpy_r(ChildNode.add.add,Addr,6);
            ChildNode.add.len = 6;
        }else
        {
            Descriptor = UnpackDescriptor(1,ChildNode.id.value);
        }

        //Descriptor.type[0] = type;
        if(Descriptor.mfrs[0]==0)
        {
            Descriptor.mfrs[0] = 0x2d;
        }
        if(Descriptor.mfrs[1]==0)
        {
            Descriptor.mfrs[1] = 0x2d;
        }
        memcpy(Descriptor.mmfrs,ModuleCode,2);
        if(Descriptor.mmfrs[0]==0)
        {
        Descriptor.mmfrs[0] = 0x2d;
        }
        if(Descriptor.mmfrs[1]==0)
        {
        Descriptor.mmfrs[1] = 0x2d;
        }
        Descriptor.midformat = ModuleType;
        for(j=0;j<ModuleIDLen;j++)
        {
            sprintf(&Descriptor.mid[j*2],"%02x",ModuleID[j]);
        }

        TASK_FMT_DEBUG(pipe, RELAY_LOG, "从节点模块id %s \n",Descriptor.mid);
        MakeDescriptor(1, Descriptor, tmp);
        ChildNode.id.nNum = strlen(tmp);
        strcpy(ChildNode.id.value,tmp);
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "从节点描述符 PN %d %s \n",Pn,tmp);
        taskmng_normal_data_write_pipe( pipe, (uint8*)&ChildNode, sizeof(OOP_CHILDNODE_T), OAD, Pn, 0);
        //nodecount++;
    }
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice8
* 函数功能://处理上报的HPLC信息8 网络基本信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice8(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0XF2090200};
    uint16 index=0;
    int ret;
    uint16 DataLen;
    ROUTEMODULE RouterModule;
    memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
    uint16 infonum = pipe+1 ;
    ret = taskmng_normal_data_read_pipe( pipe,OAD, infonum, 0, (uint8 *) &RouterModule, &DataLen);
    //数据中心还没写主节点信息 写进去
    if(ret!=0 || DataLen!=sizeof(ROUTEMODULE))
    {
        memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
    }

    RouterModule.verInfo.factory.nNum = 2;
    memcpy(RouterModule.verInfo.factory.value,&buf[index],2);
    index+=2;
    RouterModule.verInfo.ic.nNum = 2;
    memcpy(RouterModule.verInfo.ic.value,&buf[index],2);
    index+=2;
    memcpy_r(&RouterModule.verInfo.dtVer.year,&buf[index],2);
    index+=2;
    //模块回的年只有20这种 得再加2000
    if(RouterModule.verInfo.dtVer.year < 2000)
        RouterModule.verInfo.dtVer.year+=2000;
    RouterModule.verInfo.dtVer.month = buf[index++];
    RouterModule.verInfo.dtVer.mday = buf[index++];
    RouterModule.verInfo.dtVer.wday = buf[index++];
    memcpy_r(&RouterModule.verInfo.wVer ,&buf[index],2);
    index+=2;

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取到模块厂商代码 %s 芯片代码 %s 版本日期 %d-%d-%d 软件版本 %d\n",RouterModule.verInfo.factory.value,RouterModule.verInfo.ic.value,
        RouterModule.verInfo.dtVer.year,RouterModule.verInfo.dtVer.month,RouterModule.verInfo.dtVer.mday,RouterModule.verInfo.wVer);
    taskmng_normal_data_write_pipe( pipe, (uint8*)&RouterModule, sizeof(ROUTEMODULE), OAD, infonum, 0);

    //在F2090C00里也写一遍
    OOP_NODEVERSIONINFO_T  nodeversioninfo;
    memset(&nodeversioninfo,0x00,sizeof(OOP_NODEVERSIONINFO_T));
    OAD.value = 0XF2090C00;
    memcpy_r(nodeversioninfo.addr.nValue,LocalAddr,6);
    nodeversioninfo.type = 0;
    nodeversioninfo.wVer = RouterModule.verInfo.wVer;
    nodeversioninfo.dtVer = RouterModule.verInfo.dtVer;
    nodeversioninfo.factory = RouterModule.verInfo.factory;
    nodeversioninfo.ic = RouterModule.verInfo.ic;

    taskmng_normal_data_write_pipe( pipe, (uint8*)&nodeversioninfo, sizeof(nodeversioninfo), OAD, 0, 0);
}


/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save_choice9
* 函数功能://处理上报的HPLC信息9     网络节点版本信息
* 输入参数:pipe 端口号
        buf 数据内容
        len 数据长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save_choice9(uint8 pipe,uint8* buf,uint16 len)
{
    OOP_OAD_U OAD = {0XF2090C00};
    uint16 index=0;
    uint16 i;
    uint16 count;
    uint8 lenoffset;
    uint8 addr[6]={0};
    uint16 pn;
    OOP_NODEVERSIONINFO_T  nodeversioninfo;
    memset(&nodeversioninfo,0x00,sizeof(OOP_NODEVERSIONINFO_T));

    count=get_len_offset(&buf[index],&lenoffset);
    index +=lenoffset;
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "上报网络节点版本信息个数 %d \n",count);

    for(i=0;i<count;i++)
    {
        memcpy_r(addr,&buf[index],6);
        memcpy(nodeversioninfo.addr.nValue,&buf[index],6);
        index+=6;
        memcpy_r(&nodeversioninfo.wVer,&buf[index],2);
        index+=2;
        memcpy_r(&nodeversioninfo.dtVer.year ,&buf[index],2);
        index+=2;
        nodeversioninfo.dtVer.month = buf[index++];
        nodeversioninfo.dtVer.mday = buf[index++];
        nodeversioninfo.dtVer.wday = buf[index++];
        nodeversioninfo.factory.nNum = 2;
        memcpy(nodeversioninfo.factory.value,&buf[index],2);
        index+=2;
        nodeversioninfo.ic.nNum = 2;
        memcpy(nodeversioninfo.ic.value,&buf[index],2);
        index+=2;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, " %d 地址 %02x%02x%02x%02x%02x%02x 软件版本 %d 版本日期 %d-%d-%d 厂商代码 %s 芯片代码 %s \n",i,addr[5],addr[4],addr[3],addr[2],addr[1],addr[0],
            nodeversioninfo.wVer,nodeversioninfo.dtVer.year,nodeversioninfo.dtVer.month,nodeversioninfo.dtVer.mday,
            nodeversioninfo.factory.value,nodeversioninfo.ic.value);
        if(memcmp(addr,LocalAddr,6)==0)
        {
            pn = 0;
            nodeversioninfo.type = 0;   //0主节点 1从节点
        }else
        {
            pn = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
            if(pn ==0)
                continue;
            nodeversioninfo.type = 1;
        }
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "信息点号 %d \n",pn);
        taskmng_normal_data_write_pipe( pipe, (uint8*)&nodeversioninfo, sizeof(OOP_NODEVERSIONINFO_T), OAD, pn, 0);
    }

}

/*******************************************************************************
* 函数名称: taskmng_HPLC_info_save
* 函数功能:抄表线程收到上报的HPLC信息进行处理
* 输入参数:pipe 端口号
        buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_info_save(uint8 pipe,uint8* buf,uint16 len)
{

    uint16 beginseq = 0;
    uint8 choice;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    pAmrThreadArg->IsReportHPLCInfo = 1;
    uint16 index = 0;
    index+=4;   //端口号
    if(buf[index++]!=0)
    {
        memcpy_r((uint8*)&beginseq,&buf[index],2);
        index+=2;
    }
    choice = buf[index++];

    TASK_FMT_DEBUG(pipe, RELAY_LOG, "HPLC查询信息上报choice %d \n",choice);

    TASK_BUF_DEBUG(pipe, RELAY_LOG,buf, len);
    switch(choice)
    {
    case 1:         //网络拓扑信息
    {
        taskmng_HPLC_info_save_choice1(pipe,&buf[index],len-index);
    }
        break;
    case 2:
    {
        taskmng_HPLC_info_save_choice2(pipe,&buf[index],len-index );
    }
        break;
    case 3:
    {
        taskmng_HPLC_info_save_choice3(pipe,&buf[index],len-index );
    }
        break;
    case 4:
    {
        taskmng_HPLC_info_save_choice4(pipe,&buf[index],len-index );
    }
        break;
    case 5:
    {
        taskmng_HPLC_info_save_choice5(pipe,&buf[index],len-index );
    }
        break;
    case 6:
    {
        taskmng_HPLC_info_save_choice6(pipe,&buf[index],len-index );
    }
        break;
    case 7:
    {
        taskmng_HPLC_info_save_choice7(pipe,&buf[index],len-index );
    }
        break;
    case 8:
    {
        taskmng_HPLC_info_save_choice8(pipe,&buf[index],len-index );
    }
        break;
    case 9:
    {
        taskmng_HPLC_info_save_choice9(pipe,&buf[index],len-index );
    }
        break;
    default:
        break;
    }

}

/*******************************************************************************
* 函数名称: taskmng_node_reject_info_save
* 函数功能:写节点拒绝信息上报
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_node_reject_info_save(uint8 pipe,uint8* buf,uint16 len)
{
    uint8 i,count;
    uint16 index=0;
    uint16 infonum;
    OOP_REFUSESLAVE_T Refuseslave;
    memset(&Refuseslave,0x00,sizeof(OOP_REFUSESLAVE_T));
    TASK_BUF_TRACE(pipe, RELAY_LOG, buf,len);
    count = buf[index++];
    if(len<1+count*7)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "报文长度不对 不处理 \n");
        return;
    }
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "节点拒绝信息上报个数 %d  \n",count);
    Refuseslave.nNum = count;
    for(i=0;i<count;i++)
    {
        memcpy_r(Refuseslave.sa[i].nValue,&buf[index],6);
        index+=7;
    }
    infonum = taskmng_event_report_num_get(NodeRejectOad);
    taskmng_normal_data_write_pipe(pipe,(uint8*)&Refuseslave,sizeof(OOP_REFUSESLAVE_T),NodeRejectOad,infonum,0);

}

/*******************************************************************************
* 函数名称: taskmng_power_on_off_deal_col
* 函数功能://采集器停上电事件处理
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_power_on_off_deal_col(uint8 pipe,uint8* buf,uint16 len)
{
    uint16 index=0;
    uint8 Addr[6]={0};
    uint8 EventType;    //事件类型 1停电 2复电
    uint8 PowerState;
    uint8 databuf[2048]={0};
    uint16 datalen = 0;
    TMN_POWER_STATE_E powerState;
    EventType = buf[index++];
    if(EventType!=1 && EventType !=2)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停上电事件类型 %d 不对！  \n",EventType);
        return;
    }
    if(EventType == 1)
    {
        powerState = POWER_OFF;
    }else
    {
        powerState = POWER_ON;
    }
    while(index+7<=len)
    {
        memcpy(Addr,&buf[index],6);
        index+=6;
        PowerState = buf[index++];  //带电状态：0代表停电，1代表未停电
        if(PowerState == EventType-1)
        {
            /* SaveEventPowerOnOff( pipe,Addr,EventType); */
            datalen = taskmng_meter_power_deal(pipe,Addr,EventType,databuf,sizeof(databuf));
            if(datalen > 0)
            {
                /*将一个表的停电事件缓存住  */
                taskmng_meter_power_buf_add(pipe,Addr,powerState,databuf,datalen);
            }
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_power_on_off_deal_met
* 函数功能://电表停上电事件处理
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_power_on_off_deal_met(uint8 pipe,uint8* buf,uint16 len)
{
    uint16 index=0;
    uint8 Addr[6]={0};
    uint8 EventType;    //事件类型 1停电 2复电
    //提高上报效率 10块表组一帧报文上报
    uint8 databuf[2048]={0};
    uint16 datalen = 0;
    TMN_POWER_STATE_E powerState;
    EventType = buf[index++];
    if(EventType!=1 &&EventType !=2)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停上电事件类型 %d 不对！  \n",EventType);
        return;
    }
    if(EventType == 1)
    {
        powerState = POWER_OFF;
    }else
    {
        powerState = POWER_ON;
    }
    /* 打高分跑电表停上电下300个档案 一次性上报20个表的停电 加上判断 收到100个之后才依次写数据中心 */
    uint8 flag = 0;
    if(gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt == 300 )
    {
        flag = 1;
    }
    while(index+6 <= len)
    {
        memcpy(Addr,&buf[index],6);
        index+=6;
        //SaveEventPowerOnOff( pipe,Addr,EventType);
        datalen = taskmng_meter_power_deal(pipe,Addr,EventType,databuf,sizeof(databuf));
        if(datalen > 0)
        {
            /*将一个表的停电事件缓存住  */
            taskmng_meter_power_buf_add(pipe,Addr,powerState,databuf,datalen);
        }
    }
    if(flag == 1)
    {
        /*台体打高分的情况下要特殊处理 其他情况下还是放外面上报  */
        taskmng_meter_power_special_deal(pipe);
    }

}

/*******************************************************************************
* 函数名称: taskmng_HPLC_across_area_flag_set
* 函数功能: //给本地通信APP设置HPLC台区识别开关
* 输入参数: pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_across_area_flag_set(uint8 pipe,uint8 flag)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = (flag&0x01);
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0054,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"设置HPLC台区识别开关成功 \n");
    }

}

/*******************************************************************************
* 函数名称: taskmng_HPLC_off_grid_set
* 函数功能: //给本地通信APP设置HPLC离网感知
* 输入参数: pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_off_grid_set(uint8 pipe,uint8 flag)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = ((flag>>2)&0x01);
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0055,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"设置HPLC离网感知开关成功 \n");
    }

}
/*******************************************************************************
* 函数名称: taskmng_HPLC_node_reject_set
* 函数功能: //给本地通信APP设置HPLC拒绝节点上报开关
* 输入参数: pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_HPLC_node_reject_set(uint8 pipe,uint8 flag)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    payload[len++] = flag;
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x0056,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"设置HPLC拒绝节点上报开关成功 \n");
    }

}
/*******************************************************************************
* 函数名称: taskmng_cco_meter_file_clear
* 函数功能: 0022 清除表档案
* 输入参数: pipe
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_cco_meter_file_clear(uint8 pipe)
{
    uint16 IID = 0x0003;
    uint16 IOP = 0x0022;
    uint16 len=4;
    uint8 Buffer[10]={0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    int ret;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    memcpy_r(Buffer,(uint8*)&pipeOAD.value,4);

    ret = taskmng_msg_fun_deal(pipe,IID,IOP,len,Buffer,&RecvLen,RecvBuf);
    if(ret ==1 && RecvBuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "清除ccoRouter表档案成功 \n");
    }
}
/*******************************************************************************
* 函数名称: taskmng_cco_sta_info_fun
* 函数功能:MQTT线程收到上报的HPLC信息进行处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_cco_sta_info_fun(uint8* buf,uint16 len)
{
    uint16 index=0;
    uint8 pipe;

    OOP_OAD_U pipeOAD ;
    memset(&pipeOAD,0x00,sizeof(OOP_OAD_U));
    memcpy_r((uint8*)&pipeOAD.value,&buf[index],4);
    index+=4;
    pipe = taskmng_mtr_pipe_get(&pipeOAD);

    if(pipe>1)
        return ;
    task_send_msg(gMqttSock,MSG_CCO_STA_INFO,buf,len,IPC_AMR+pipe);

}
/*******************************************************************************
* 函数名称: taskmng_cco_sta_result_check
* 函数功能://检查搜表结果是否重复 如果重复则不写入数据中心
* 输入参数:pipe 端口号
        searchresult 搜表结果
* 输出参数: infonum 写入数据中心用的信息点号
* 返 回 值: TRUE 没有重复的 FALSE有重复的
*******************************************************************************/
void taskmng_cco_sta_result_check(uint8 pipe,OOP_NODEVERSIONINFO_T nodeversioninfo,uint16 *infonum)
{

    uint16 i;
    int index = -1;
    uint8 tmpaddr[6];
    pthread_mutex_lock(&ccostainfo.lock);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_cco_sta_result_check infonum %d count %d \n",ccostainfo.infonum,ccostainfo.count);
    for(i=0;i<ccostainfo.count;i++)
    {
        memcpy(tmpaddr,nodeversioninfo.addr.nValue,6);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccosta11地址 0x%02x%02x%02x%02x%02x%02x \n",tmpaddr[5],tmpaddr[4],tmpaddr[3],tmpaddr[2],tmpaddr[1],tmpaddr[0]);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccosta22地址 0x%02x%02x%02x%02x%02x%02x \n",ccostainfo.result[i].Addr[5],ccostainfo.result[i].Addr[4],ccostainfo.result[i].Addr[3],ccostainfo.result[i].Addr[2],ccostainfo.result[i].Addr[1],ccostainfo.result[i].Addr[0]);
        if(memcmp(tmpaddr,ccostainfo.result[i].Addr,6)==0)
        {
            if(pipe == ccostainfo.result[i].pipe)
            {
                pthread_mutex_unlock(&ccostainfo.lock);
                *infonum = ccostainfo.result[i].infonum;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccostainfo.result[i].infonum %d i %d \n",ccostainfo.result[i].infonum,i);
                return ;
            }

        }
    }
    if(index==-1)
    {
        ccostainfo.infonum++;
        ccostainfo.result[ccostainfo.count].pipe = pipe;
        ccostainfo.result[ccostainfo.count].infonum = ccostainfo.infonum;
        memcpy(ccostainfo.result[ccostainfo.count].Addr,nodeversioninfo.addr.nValue,6);
        ccostainfo.count++;

        *infonum = ccostainfo.infonum;
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_cco_sta_result_check 添加记录 infonum  %d count %d \n",ccostainfo.infonum,ccostainfo.count);

    pthread_mutex_unlock(&ccostainfo.lock);
    return ;
}

/*******************************************************************************
* 函数名称: taskmng_cco_sta_info_write
* 函数功能: //将CCOSTAinfo写入数据中心
* 输入参数: pipe 端口号
        buf 上报的搜表数据
        len  长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_cco_sta_info_write(uint8 pipe,uint8* buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 MeterNum;
    YYMMDDWWhhnnss_TYPE date;
    uint16 index=0;
    int ret;
    uint8 bytes;
    OOP_NODEVERSIONINFO_T  nodeversioninfo;

    memset(&nodeversioninfo,0x00,sizeof(OOP_NODEVERSIONINFO_T));
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = 0;
    NormalOad.oad.value = 0xF2090C00;
    NormalOad.classtag = 3;
    uint8 i;

    index+=4;
    MeterNum=get_len_offset(&buf[index],&bytes);
    index = index+bytes;
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "上报宽带模块载波节点信息个数 %d \n",MeterNum);

    for(i=0;i<MeterNum;i++)
    {

        //从节点地址
        memcpy(nodeversioninfo.addr.nValue,&buf[index],6);
        index+=6;
        //版本号
        nodeversioninfo.wVer = commfun_BcdToHex(buf[index])*100+commfun_BcdToHex(buf[index+1]);
        //memcpy_r((uint8*)&nodeversioninfo.wVer,&buf[index],2);
        index+=2;
        //版本日期(日)
        date.DD = buf[index];
        nodeversioninfo.dtVer.mday = Bcd2Hex(buf[index++]);
        //版本日期(月)
        date.MM = buf[index];
        nodeversioninfo.dtVer.month = Bcd2Hex(buf[index++]);
        //版本日期(年)
        date.YY = buf[index];
        nodeversioninfo.dtVer.year = 2000+Bcd2Hex(buf[index++]);
        //版本日期(周)
        CaculateWeekDay(&date);
        nodeversioninfo.dtVer.wday = date.WW;
        //模块厂商代码
        nodeversioninfo.factory.nNum =2;
        memcpy(nodeversioninfo.factory.value,&buf[index],2);
        index+=2;
        nodeversioninfo.ic.nNum =2;
        memcpy(nodeversioninfo.ic.value,&buf[index],2);
        index+=2;
        //节点类型
        nodeversioninfo.type = buf[index++];
        //判断从节点的写入测量点号
        taskmng_cco_sta_result_check(pipe,nodeversioninfo,&NormalOad.infoNum);


        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&nodeversioninfo,sizeof(OOP_NODEVERSIONINFO_T));

        if(ret ==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccosta结果写入成功 index %d\n",NormalOad.infoNum);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccosta地址 0x%02x%02x%02x%02x%02x%02x \n",nodeversioninfo.addr.nValue[5],nodeversioninfo.addr.nValue[4],nodeversioninfo.addr.nValue[3],nodeversioninfo.addr.nValue[2],nodeversioninfo.addr.nValue[1],nodeversioninfo.addr.nValue[0]);
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_msg_0061_unpack
* 函数功能:解析抄表模块信息，存储F2090200一些信息
模块版本 厂商代码、芯片代码-年月日-版本
消息报文示例：
10 54 58 30 33 2d 31 39 30 39 32 34 2d 30 30 30 35  --模块版本 TX03-190924-0005

* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_msg_0061_unpack(uint8 pipe,uint8*buf,uint16 len)
{
    int ret;
    uint16 DataLen = 0;
    OOP_OAD_U OAD = {0xF2090200};
    if(len!=0x2E)
        return;
    uint8 index = 29;//前面的消息不需要，只需要模块版本
    if(buf[index++]!=0x10)
        return;
    uint8 tmp[20]={0};
    memcpy(tmp,&buf[index],16);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "获取到模块版本 %s \n",tmp);
    ROUTEMODULE RouterModule;
    memset(&RouterModule,0x00,sizeof(ROUTEMODULE));

    uint16 infonum = pipe+1 ;
    ret = taskmng_normal_data_read_pipe( pipe,OAD, infonum, 0, (uint8 *) &RouterModule, &DataLen);
    //数据中心还没写主节点信息 写进去
    if(ret!=0 || DataLen!=sizeof(ROUTEMODULE))
    {
        memset(&RouterModule,0x00,sizeof(ROUTEMODULE));
    }
    RouterModule.verInfo.factory.nNum = 2;
    memcpy(RouterModule.verInfo.factory.value,&buf[index],2);
    index+=2;
    RouterModule.verInfo.ic.nNum = 2;
    memcpy(RouterModule.verInfo.ic.value,&buf[index],2);
    index+=2;
    index++; //'-'

    RouterModule.verInfo.dtVer.year = 2000+CharToNum(buf[index])*10 +CharToNum(buf[index+1]);
    index+=2;

    RouterModule.verInfo.dtVer.month = CharToNum(buf[index])*10 +CharToNum(buf[index+1]);
    index+=2;

    RouterModule.verInfo.dtVer.mday = CharToNum(buf[index])*10 +CharToNum(buf[index+1]);
    index+=2;
    index++;//'-'

    //RouterModule.verInfo.wVer = (CharToNum(buf[index])<<4|CharToNum(buf[index+1]))*10+(CharToNum(buf[index+2])<<4|CharToNum(buf[index+3]));
    //软件版本显示
    RouterModule.verInfo.wVer = CharToNum(buf[index])*1000+CharToNum(buf[index+1])*100+CharToNum(buf[index+2])*10 +CharToNum(buf[index+3]);
    taskmng_normal_data_write_pipe( pipe, (uint8*)&RouterModule, sizeof(ROUTEMODULE), OAD, infonum, 0);
}

/*******************************************************************************
* 函数名称: taskmng_hplc_update_check
* 函数功能://mqtt线程中收到0020设备文件代发后检查是否是给模块的升级文件
        确认升级文件名中是否包含“router”和“slaver”关键字，若有匹配字符串，则调用“启动升级”给ccoRouter
* 输入参数:filepath 文件路径
        len 消息长度

* 输出参数: fileflag 文件标识：0—主节点升级文件；1—主节点和子节点升级文件；2—子节点升级文件。
* 返 回 值: -1 电表升级文件 0 给载波模块的升级文件
*******************************************************************************/

int taskmng_hplc_update_check(char* filepath,uint8* fileflag)
{
    char *p = NULL;
    p = strstr(filepath,"router");
    if(NULL != p)
    {
        *fileflag = 0;
        return 0;
    }
    p = strstr(filepath,"slaver");
    if(NULL != p)
    {
        *fileflag =2;
        return 0;
    }
    return -1;
}



/*******************************************************************************
* 函数名称: taskmng_file_transfer_msg_fun
* 函数功能://mqtt线程中收到0020设备文件代发的处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值: 0 可以正常启动 -1 当前正在升级中 不响应
*******************************************************************************/
int taskmng_file_transfer_msg_fun(uint8* buf,uint16 len)
{
    TASK_FMT_DEBUG(-1,UP_LOG,"收到0020设备文件代发\n");

    UPDATE_INFO_T updateinfo;
    memset(&updateinfo,0x00,sizeof(UPDATE_INFO_T));
    uint8 pipelist[AMR_THREAD_NUM]={0};
    uint8 pipenum = 0;
    uint8 flag;
    int ret;
    uint16 num,i,j;
    uint16 index = 0;
    uint8 pipe;
    uint8 lenoffset;
    int fileflag = -1; //0—主节点升级文件；1—主节点和子节点升级文件；2—子节点升级文件。
    uint8 type;
    OOP_OAD_U oad ,suboad;
    //按照最新消息结构进行解析
    type = buf[index++];

    if(type == 81)
    {
        //oad 给主节点升级
        memcpy_r(&oad.value,&buf[index],4);
        index+=4;
        if(oad.nObjID == 0XF209 && oad.attID == 2)
        {
            TASK_FMT_DEBUG(-1,UP_LOG,"收到载波主节点升级命令 \n");
            fileflag = 0;
        }
        if(fileflag == -1)
            return -1;
    }
    else if(type == 82)
    {
        //road 给子节点升级
        memcpy_r(&oad.value,&buf[index],4);
        index+=4;
        num = buf[index++];
        for(i=0;i<num;i++)
        {
            memcpy_r(&suboad.value,&buf[index],4);
            index+=4;
            if(oad.nObjID == 0XF209 && oad.attID == 2 && suboad.value == 0xF2100200)
            {
                TASK_FMT_DEBUG(-1,UP_LOG,"收到载波子节点升级命令 \n");
                fileflag = 2;
            }
        }
        if(fileflag == -1)
            return -1;
    }
    else if(type == 92)
    {
        //ms 电表升级
        num = buf[index++];
        if(num>MAX_UPDATE_METER_NUM)
        {
            TASK_FMT_DEBUG(-1,UP_LOG,"设备地址过多暂不支持  %d \n",num);
            return -1;
        }
        updateinfo.MeterNum = num;
        for(i=0;i<num;i++)
        {
            updateinfo.MeterInfo[i].AddrFlag = buf[index++];
            index++;
            memcpy_r(updateinfo.MeterInfo[i].Addr,&buf[index],6);
            index+=6;
            TASK_FMT_DEBUG(-1,UP_LOG,"%d 要升级的地址 AddrFlag %d \n",i,updateinfo.MeterInfo[i].AddrFlag,
                updateinfo.MeterInfo[i].Addr[5],updateinfo.MeterInfo[i].Addr[4],updateinfo.MeterInfo[i].Addr[3],updateinfo.MeterInfo[i].Addr[2],
                updateinfo.MeterInfo[i].Addr[1],updateinfo.MeterInfo[i].Addr[0]);
        }
    }
    else
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"不支持的目标类型 %d \n",type);
        return -1;
    }
    //源文件
    updateinfo.FileInfo.file.srceFile.nNum = XADR_GetEleNum(&buf[index], &lenoffset);
    index+=lenoffset;
    if(updateinfo.FileInfo.file.srceFile.nNum > 64)
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"源文件路径超长       %d \n",updateinfo.FileInfo.file.srceFile.nNum);
        return -1;
    }
    memcpy(updateinfo.FileInfo.file.srceFile.value,&buf[index],updateinfo.FileInfo.file.srceFile.nNum);
    index+=updateinfo.FileInfo.file.srceFile.nNum;
    //模块升级只需要判断源文件
    if(fileflag != -1)
    {
        TASK_FMT_DEBUG(0,RELAY_LOG," 文件路径 %s 文件标识 %d \n",updateinfo.FileInfo.file.srceFile.value,fileflag);
        HPLC_UPDATE_T update;
        memset(&update,0x00,sizeof(HPLC_UPDATE_T));
        update.filepath.nNum = updateinfo.FileInfo.file.srceFile.nNum;
        memcpy(update.filepath.value,updateinfo.FileInfo.file.srceFile.value,update.filepath.nNum);
        update.fileflag = fileflag;
        ret = task_send_msg(gMqttSock,MSG_HPLC_UPDATE,(uint8*)&update,sizeof(HPLC_UPDATE_T),IPC_AMR);
        if(ret ==0)
        {
            return ret;
        }else
        {
            TASK_FMT_DEBUG(0,RELAY_LOG,"MSG_HPLC_UPDATE send error %d \n",ret);
            return -1;
        }
    }
    updateinfo.FileInfo.file.destFile.nNum = XADR_GetEleNum(&buf[index], &lenoffset);
    index+=lenoffset;
    if(updateinfo.FileInfo.file.destFile.nNum > 64)
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"目标文件路径超长       %d \n",updateinfo.FileInfo.file.destFile.nNum);
        return -1;
    }
    memcpy(updateinfo.FileInfo.file.destFile.value,&buf[index],updateinfo.FileInfo.file.destFile.nNum);
    index+=updateinfo.FileInfo.file.destFile.nNum;
    memcpy_r(&updateinfo.FileInfo.file.nSize,&buf[index],4);
    index+=4;
    //bitstring 有个3的长度
    index++;
    updateinfo.FileInfo.file.property.nValue = buf[index++];
    updateinfo.FileInfo.file.version.nNum = XADR_GetEleNum(&buf[index], &lenoffset);
    index+=lenoffset;
    if(updateinfo.FileInfo.file.version.nNum > 64)
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"文件版本超长       %d \n",updateinfo.FileInfo.file.version.nNum);
        return -1;
    }
    memcpy(updateinfo.FileInfo.file.version.value,&buf[index],updateinfo.FileInfo.file.version.nNum);
    index+=updateinfo.FileInfo.file.version.nNum;
    updateinfo.FileInfo.file.filetype = buf[index++];
    TASK_FMT_DEBUG(-1,UP_LOG,"源文件 %s 目标文件 %s 文件大小 %d 文件属性 0x%02x 文件版本 %s 文件类别 %d \n",
        updateinfo.FileInfo.file.srceFile.value,updateinfo.FileInfo.file.destFile.value,updateinfo.FileInfo.file.nSize,
        updateinfo.FileInfo.file.property.nValue,updateinfo.FileInfo.file.version.value,updateinfo.FileInfo.file.filetype);

    //传输块大小
    memcpy_r(&updateinfo.FileInfo.nBlockSize,&buf[index],2);
    index+=2;
    //校验
    updateinfo.FileInfo.crc.type = buf[index++];
    updateinfo.FileInfo.crc.content.nNum = XADR_GetEleNum(&buf[index], &lenoffset);
    index+=lenoffset;
    if(updateinfo.FileInfo.crc.content.nNum > 16)
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"文件校验超长   %d \n",updateinfo.FileInfo.crc.content.nNum);
        return -1;
    }
    memcpy(updateinfo.FileInfo.crc.content.value,&buf[index],updateinfo.FileInfo.crc.content.nNum);
    index+=updateinfo.FileInfo.crc.content.nNum;
    TASK_FMT_DEBUG(-1,UP_LOG,"传输块大小 %d \n",updateinfo.FileInfo.nBlockSize);
    //兼容软件版本号
    updateinfo.FileInfo.swlist.nNum = buf[index++];
    for(i=0;i<updateinfo.FileInfo.swlist.nNum;i++)
    {
        updateinfo.FileInfo.swlist.value[i].nNum = buf[index++];
        memcpy(updateinfo.FileInfo.swlist.value[i].value,&buf[index],updateinfo.FileInfo.swlist.value[i].nNum);
        index+=updateinfo.FileInfo.swlist.value[i].nNum;
        TASK_FMT_DEBUG(-1,UP_LOG,"兼容的软件版本号index %d %s\n",i,updateinfo.FileInfo.swlist.value[i].value);
    }
    //硬件版本号
    updateinfo.FileInfo.hwlist.nNum = buf[index++];
    for(i=0;i<updateinfo.FileInfo.hwlist.nNum;i++)
    {
        updateinfo.FileInfo.hwlist.value[i].nNum = buf[index++];
        memcpy(updateinfo.FileInfo.hwlist.value[i].value,&buf[index],updateinfo.FileInfo.hwlist.value[i].nNum);
        index+=updateinfo.FileInfo.hwlist.value[i].nNum;
        TASK_FMT_DEBUG(-1,UP_LOG,"兼容的硬件版本号index %d %s\n",i,updateinfo.FileInfo.hwlist.value[i].value);
    }
    //下载方标识
    updateinfo.FileInfo.identy.nNum = buf[index++];
    memcpy(updateinfo.FileInfo.identy.value,&buf[index],updateinfo.FileInfo.identy.nNum);
    index+=updateinfo.FileInfo.identy.nNum;
    //安全传输
    updateinfo.bEncode = buf[index++];
    TASK_FMT_DEBUG(-1,UP_LOG,"下载方标识 %s 安全传输标识 %d \n",updateinfo.FileInfo.identy.value,updateinfo.bEncode );


    for(i=0;i<updateinfo.MeterNum;i++)
    {
        if(updateinfo.MeterInfo[i].AddrFlag == ADDR_SINGLE)
        {
            pipe = taskmng_pipe_get_from_addr(updateinfo.MeterInfo[i].Addr,LOGIC_ID_YC);
            TASK_FMT_DEBUG(-1,UP_LOG,"电表索引 %d pipe %d \n",i,pipe);
            updateinfo.MeterInfo[i].pipe = pipe;
            //就载波和485口支持物联表升级
            if(pipe ==1 || pipe > AMR_THREAD_NUM-3)
                continue;
            //看下是不是新的端口
            flag = 0;
            for(j=0;j<pipenum;j++)
            {
                if(pipelist[j] == pipe)
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
            {
                pipelist[pipenum] = pipe;
                pipenum++;
            }
        }
    }

    for(j=0;j<pipenum;j++)
    {
        TASK_FMT_DEBUG(-1,UP_LOG,"电表升级总端口数 %d 当前 %d \n",pipenum,pipelist[j]);
        if(gAmrThreadArg[pipe].IsUpdating == 1)
        {
            TASK_FMT_DEBUG(-1,UP_LOG,"当前正在文件传输中\n");
            continue;
        }
        ret = task_send_msg(gMqttSock,MSG_FILE_TRANSFER,(uint8*)&updateinfo,sizeof(updateinfo),IPC_AMR+pipelist[j]);
        if(ret !=0)
        {
            TASK_FMT_DEBUG(-1,UP_LOG,"MSG_FILE_TRANSFER send error %d \n",ret);
        }
    }
    return 0;

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_msg_amr
* 函数功能:抄表线程收到文件代发后进行处理
* 输入参数:
        pipe 端口号
        buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/

void taskmng_file_transfer_msg_amr(uint8 pipe,uint8*buf,uint16 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    UPDATE_INFO_T *updateinfo = &pAmrThreadArg->UpdateInfo;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "收到文件代发命令 \n");
    pAmrThreadArg->IsUpdating = 1;
    memset(updateinfo,0x00,sizeof(UPDATE_INFO_T));

    uint16 i;
    UPDATE_INFO_T info;
    memcpy(&info,buf,len);
    memcpy(updateinfo,&info,sizeof(UPDATE_INFO_T));
    updateinfo->MeterNum = 0;
    memset(updateinfo->MeterInfo,0x00,sizeof(updateinfo->MeterIndex));

    //筛选一下要升级的电表
    for(i=0;i<info.MeterNum;i++)
    {
        if(i==0 && info.MeterInfo[i].AddrFlag == ADDR_GROUP)
        {
            updateinfo->MeterInfo[0] = info.MeterInfo[0];
            updateinfo->MeterNum = 1;
            continue;
        }
        if(info.MeterInfo[i].AddrFlag == ADDR_SINGLE && pipe == info.MeterInfo[i].pipe)
        {
            updateinfo->MeterInfo[updateinfo->MeterNum] = info.MeterInfo[i];
            updateinfo->MeterNum++;
        }
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件路径 %s 要升级的表地址如下 总个数 %d \n",updateinfo->FileInfo.file.srceFile.value,updateinfo->MeterNum);
    uint8 tmpaddr[6];
    for(i=0;i<updateinfo->MeterNum;i++)
    {
        memcpy(tmpaddr,updateinfo->MeterInfo[i].Addr,6);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "%d 地址类型 %d 地址 %02x%02x%02x%02x%02x%02x \n",i,updateinfo->MeterInfo[i].AddrFlag,
            tmpaddr[5],tmpaddr[4],tmpaddr[3],tmpaddr[2],tmpaddr[1],tmpaddr[0]);
    }
    //485口的直接判IsUpdating标记
    if(pipe == 0)
        taskmng_plc_task_queue_add(pipe,MS_FILE_TRANSFER);
}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_deal
* 函数功能:文件代发处理
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
*******************************************************************************/
int taskmng_file_transfer_deal(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    UPDATE_INFO_T *updateinfo = &pAmrThreadArg->UpdateInfo;
    FILE_UPDATE_ENCRYPT_INFO_T *encryptinfo = &pAmrThreadArg->UpdateEncryptInfo;
    int ret;
    static time_t  BeginTime[PLC_NUM];
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    int RecvLen;
    MESSAGE_INFO_T *MsgInfo = NULL;
    switch(pAmrThreadArg->TotalState.FileTransfer)
    {
    case INNER_RTS_IDLE:
    {
        memset(encryptinfo,0x00,sizeof(FILE_UPDATE_ENCRYPT_INFO_T));
        //如果第一个地址是组地址 则要用广播发 每一帧限定长度
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            if(updateinfo->FileInfo.nBlockSize > 170)
                updateinfo->FileInfo.nBlockSize = 170;
        }
        //计算需要发送多少帧
        uint16 tmp = updateinfo->FileInfo.file.nSize%updateinfo->FileInfo.nBlockSize;
        if(tmp>0)
        {
            updateinfo->FrameCount = updateinfo->FileInfo.file.nSize/updateinfo->FileInfo.nBlockSize+1;
        }else
        {
            updateinfo->FrameCount = updateinfo->FileInfo.file.nSize/updateinfo->FileInfo.nBlockSize;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件大小 %d 每帧 %d 总帧数%d \n",updateinfo->FileInfo.file.nSize,updateinfo->FileInfo.nBlockSize,updateinfo->FrameCount);
        pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
        updateinfo->FileState = FILE_INIT;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret = taskmng_file_transfer_send(pipe);
        if(ret == 1)
        {
            BeginTime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RECV;

        }else if(ret == 0)
        {
            pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_IDLE;
            pAmrThreadArg->IsUpdating = 0;
            return 1;
        }
    }
        break;
    case INNER_RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            if(MsgInfo->index!=updateinfo->SendMsgIndex)
            {
                break;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件代发收到消息 IID %04X IOP %04X  \n",MsgInfo->IID,MsgInfo->IOP);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
            ret = taskmng_file_transfer_recv(pipe,MsgInfo->playload, MsgInfo->msglen);
            if(ret == 0)
                pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
            else
                pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RETRY;
            if(updateinfo->isBroad == 1)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "广播后延时10秒\n");
                sleep(10);
            }
        }else
        {
            if(time(NULL)-BeginTime[pipe] >FILE_TRANSFER_OVERTIME )
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表索引 %d 发送超时 \n",updateinfo->MeterIndex);

                pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_RETRY;
            }
        }

    }
        break;
    case INNER_RTS_RETRY:
    {
        if(updateinfo->FileState >= FILE_ENCRYPT_INIT )
        {
            encryptinfo->failcnt ++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "转加密文件升级 失败次数 %d \n",encryptinfo->failcnt);
        }else
        {
            updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt++;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表索引 %d    失败次数 %d \n",updateinfo->MeterIndex,updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt);
        }
        pAmrThreadArg->TotalState.FileTransfer = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_read_pack
* 函数功能: 组织40270200查询报文
* 输入参数:pipe 端口号
        tsa 地址
* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_read_pack(uint8 pipe,OOP_TSA_T tsa,uint8* outbuf)
{

    OOP_OAD_U oad = {0x40270200};
    LinkLayer_t *pLinkLayer = NULL;
    uint16 len = 0;

    OopGet.requst(pipe,&tsa,1,(uint8*)&oad.value,4);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        len = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,len);
    }
    return len;
}


/*******************************************************************************
* 函数名称: taskmng_file_transfer_send
* 函数功能:文件代发发送处理
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1表示有报文发送 2表示不切换状态 0表示任务结束
*******************************************************************************/

int taskmng_file_transfer_send(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    UPDATE_INFO_T *updateinfo = &pAmrThreadArg->UpdateInfo;
    FILE_UPDATE_ENCRYPT_INFO_T *encryptinfo = &pAmrThreadArg->UpdateEncryptInfo;
    static int fd[AMR_THREAD_NUM];
    uint8 filebuf[2000]={0};
    uint8 payload[MAX_MSG_LEN]={0};
    uint8 framebuf[MAX_MSG_LEN] = {0};
    uint16 framelen = 0;
    uint16 index = 0;
    int ret;
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    uint16 Overtime = FILE_TRANSFER_OVERTIME;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    uint8 isbroad =0 ;      //标记是否需要广播发送
    uint16 i=0;
    updateinfo->isBroad = 0;
    if(encryptinfo->failcnt >=3)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "转加密升级电表 %d 失败超过3次 \n",gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn);
        encryptinfo->msindex++;
        encryptinfo->failcnt=0;
        updateinfo->FileState = FILE_ENCRYPT_INIT;
    }
    for(i = updateinfo->MeterIndex;i<updateinfo->MeterNum;i++)
    {
        if(updateinfo->MeterInfo[i].FainCnt>=3)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表索引 %d 失败超过3次         不再抄读 \n",updateinfo->MeterIndex);
            updateinfo->MeterInfo[i].UpdateResult = UPDATE_FAIL_OTHER_REASON;
            updateinfo->MeterInfo[i].CheckResult =1;
            continue;
        }
        updateinfo->MeterIndex = i;
        break;
    }
    updateinfo->MeterIndex = i;
    uint16  tmpindex = 0;
    uint8 tmpresult= 0;
    switch(updateinfo->FileState)
    {
    case FILE_INIT:
    {
        fd[pipe] = open(updateinfo->FileInfo.file.srceFile.value ,O_RDONLY);
        if(fd[pipe]<0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件%s 打开失败 \n",updateinfo->FileInfo.file.srceFile.value);
            return 0;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件%s 打开成功 \n",updateinfo->FileInfo.file.srceFile.value);
        updateinfo->startcnt = 0;
        updateinfo->FileState = FILE_START;
        //先写一下升级结果
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
            tmpindex = 1;
        if(pipe == 0)
            tmpresult = 6;
        else
            tmpresult = 3;
        for(;tmpindex<updateinfo->MeterNum;tmpindex++)
        {
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[tmpindex].Addr,6);
            taskmng_file_transfer_update_result_save(pipe,tmpresult,&tsa);
        }
        return 2;
    }
        break;
    case FILE_START:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "启动文件传输\n");
        //单地址需要一个个表的发送
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
        {
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[updateinfo->MeterIndex].Addr,6);
            updateinfo->MeterInfo[updateinfo->MeterIndex].UpdateResult = UPDATE_DOWNLOADING;
        }else if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            //第一个地址为组地址 后面的为组中的设备具体地址 在查询文件传输状态时使用
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.type = 2;
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[0].Addr,6);
            for(i=1;i<updateinfo->MeterNum;i++)
            {
                updateinfo->MeterInfo[i].UpdateResult = UPDATE_DOWNLOADING;
            }
            isbroad = 1;//需要广播发送
        }else
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "暂不支持广播地址 \n");
            close(fd[pipe]);
            return 0;
        }

        framelen = taskmng_file_transfer_start_pack(pipe,tsa,&updateinfo->FileInfo,updateinfo->bEncode,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_start_pack error !\n");
            return 2;
        }
    }
        break;
    case FILE_SEND:
    {

        if(updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex>=updateinfo->FrameCount)
        {
            if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "一个表发完 切下一个!\n");
                updateinfo->MeterIndex++;
                updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex = 0;
                updateinfo->FileState = FILE_START;
                return 2;
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "组地址文件发送完毕，检查\n");
                updateinfo->FileState = FILE_CHECK;
                updateinfo->MeterIndex = 1;  //组地址从第二个单地址开始查询
                return 2;
            }
        }
        if(updateinfo->MeterIndex>=updateinfo->MeterNum)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "单地址文件发送完毕，检查\n");
            updateinfo->FileState = FILE_CHECK;
            updateinfo->MeterIndex = 0; //复用 作为检查文件传输状态的索引
            return 2;
        }
        //根据索引获取文件内容
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "fd %d当前发送表 %d/%d 文件 %d/%d\n",fd[pipe],
            updateinfo->MeterIndex,updateinfo->MeterNum,updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex,updateinfo->FrameCount);
        uint16 sendindex = updateinfo->FileInfo.nBlockSize*updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex;
        lseek(fd[pipe],sendindex,SEEK_SET);
        memset(filebuf,0x00,sizeof(filebuf));
        ret = read(fd[pipe],filebuf,updateinfo->FileInfo.nBlockSize);
        if(ret<0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "read error !\n");
            close(fd[pipe]);
            return 0;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "read ret %d\n",ret);
        //TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件开始索引 %d \n",sendindex);
        //TASK_BUF_DEBUG( pipe, RELAY_LOG, filebuf,ret);
        //单地址需要一个个表的发送
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
        {
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[updateinfo->MeterIndex].Addr,6);
            updateinfo->MeterInfo[updateinfo->MeterIndex].UpdateResult = UPDATE_DOWNLOADING;
        }else if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            //第一个地址为组地址 后面的为组中的设备具体地址 在查询文件传输状态时使用
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.type = 2;
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[0].Addr,6);
            for(i=1;i<updateinfo->MeterNum;i++)
            {
                updateinfo->MeterInfo[i].UpdateResult = UPDATE_DOWNLOADING;
            }
            isbroad = 1;//需要广播发送
        }else
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "暂不支持广播地址 \n");
            close(fd[pipe]);
            return 0;

        }
        framelen = taskmng_file_transfer_sendbuf_pack(pipe,tsa,updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex,filebuf,ret,updateinfo->bEncode,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_sendbuf_pack error !\n");
            return 2;
        }

    }
        break;
    case FILE_CHECK:
    {
        //依次查询每块表的文件传输状态 05 01 20 f0 01 04 00 00
        //如果该表全部传输成功 则跳过
        while(updateinfo->MeterIndex < updateinfo->MeterNum)
         {
            if(updateinfo->MeterInfo[updateinfo->MeterIndex].CheckResult ==1)
            {
                updateinfo->MeterIndex++;
                continue;
            }
            if(updateinfo->MeterInfo[updateinfo->MeterIndex].CheckCnt>=3)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表已重发超过3次 \n");
                updateinfo->MeterInfo[updateinfo->MeterIndex].UpdateResult = UPDATE_FAIL_OTHER_REASON;
                updateinfo->MeterInfo[updateinfo->MeterIndex].CheckResult =1;
                updateinfo->MeterIndex++;
                continue;
            }
            break;
        }

        if(updateinfo->MeterIndex>=updateinfo->MeterNum)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "全部检查完毕 \n");
            //检查是否所有表全部传输成功
            uint8 flag = 0;
            if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
            {
                i = 0;
            }else
                i = 1;
            for(;i<updateinfo->MeterNum;i++)
            {
                if(updateinfo->MeterInfo[i].CheckResult == 0)
                {
                    flag = 1;
                    break;
                }
            }
            //所有表全部传输成功
            if(flag == 0)
            {
                //传输完成后读取升级结果40270200，并作为采集数据存储
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "文件全部传输完成 读取升级结果 ！\n");
                updateinfo->FileState = FILE_READ;
                if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
                {
                    updateinfo->MeterIndex = 0;
                }else
                    updateinfo->MeterIndex = 1;
                close(fd[pipe]);
                return 2;
            }
            updateinfo->FileState = FILE_RESEND;
            if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
            {
                updateinfo->MeterIndex = 0;
            }else
            {
                updateinfo->MeterIndex = 1;
                for(i=0;i<updateinfo->MeterInfo[0].FrameResendCount;i++)
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "i %d 需要广播补发的索引号 %d\n",i,updateinfo->MeterInfo[0].FrameResendSeq[i]);
                }
            }
            return 2;
        }
        updateinfo->MeterInfo[updateinfo->MeterIndex].CheckCnt++;
        memset(&tsa,0x00,sizeof(OOP_TSA_T));
        tsa.len = 5;
        memcpy_r(tsa.add,updateinfo->MeterInfo[updateinfo->MeterIndex].Addr,6);
        framelen = taskmng_file_transfer_check_pack(pipe,tsa,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_check_pack error !\n");
            return 2;
        }
    }
        break;
    case FILE_RESEND:
    {
        uint8 flag = 0;

        //如果组播 还用广播发送

        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            if(updateinfo->MeterInfo[0].FrameResendIndex>=updateinfo->MeterInfo[0].FrameResendCount)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "组播重发送全部完毕 再次检查发送状态\n");
                updateinfo->FileState = FILE_CHECK;
                updateinfo->MeterIndex = 1;
                return 2;
            }
            updateinfo->MeterInfo[0].FrameIndex = updateinfo->MeterInfo[0].FrameResendSeq[updateinfo->MeterInfo[0].FrameResendIndex];
            updateinfo->MeterIndex = 0;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa.type = 2;
            tsa.len = 5;
            memcpy_r(tsa.add,updateinfo->MeterInfo[0].Addr,6);
            isbroad = 1;
        }else
        {
            for(i = updateinfo->MeterIndex;i<updateinfo->MeterNum;i++)
            {
                if(updateinfo->MeterInfo[i].CheckResult == 1)
                    continue;
                if(updateinfo->MeterInfo[i].FrameResendIndex>=updateinfo->MeterInfo[i].FrameResendCount)
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表全部发送完 切换下个表\n");
                    continue;
                }
                updateinfo->MeterInfo[i].FrameIndex = updateinfo->MeterInfo[i].FrameResendSeq[updateinfo->MeterInfo[i].FrameResendIndex];
                memset(&tsa,0x00,sizeof(OOP_TSA_T));
                tsa.len = 5;
                memcpy_r(tsa.add,updateinfo->MeterInfo[i].Addr,6);
                updateinfo->MeterIndex = i;
                flag = 1;
                break;
            }
            //全部重发完毕 继续检查状态
            if(flag == 0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "单地址全部重发完毕 再次检查\n");
                updateinfo->FileState = FILE_CHECK;
                updateinfo->MeterIndex = 0;
                return 2;
            }
        }
        //找到需要发送的包
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前重发送表 %d/%d 文件索引号 %d 重发进度 %d/%d\n",updateinfo->MeterIndex,updateinfo->MeterNum,
            updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex,updateinfo->MeterInfo[updateinfo->MeterIndex].FrameResendIndex,updateinfo->MeterInfo[updateinfo->MeterIndex].FrameResendCount);
        lseek(fd[pipe],updateinfo->FileInfo.nBlockSize*updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex,SEEK_SET);
        memset(filebuf,0x00,sizeof(filebuf));
        ret = read(fd[pipe],filebuf,updateinfo->FileInfo.nBlockSize);
        if(ret<0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "read error !\n");
            close(fd[pipe]);
            return 0;
        }
        framelen = taskmng_file_transfer_sendbuf_pack(pipe,tsa,updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex,filebuf,ret,updateinfo->bEncode,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_sendbuf_pack error !\n");
            return 2;
        }

    }
        break;
    case FILE_READ:
    {
        if(updateinfo->MeterIndex>=updateinfo->MeterNum)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "全部电能表升级结果读取完毕  ！\n");
            if(TRUE == taskmng_file_encrypt_update_check(pipe))
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "开始进行转加密升级  ！\n");
                updateinfo->FileState = FILE_ENCRYPT_INIT;
                encryptinfo->msindex = 0;
                encryptinfo->updateflag = 0;
                return 2;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "升级结束  ！\n");
            updateinfo->FileState = FILE_INIT;
            return 0;
        }
        memset(&tsa,0x00,sizeof(OOP_TSA_T));
        tsa.len = 5;
        memcpy_r(tsa.add,updateinfo->MeterInfo[updateinfo->MeterIndex].Addr,6);
        framelen = taskmng_file_transfer_read_pack(pipe,tsa,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_read_pack error !\n");
            return 2;
        }
    }
        break;
    case FILE_ENCRYPT_INIT:
    {
        if(gEncryptPipe != -1 && gEncryptPipe != pipe)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "当前端口 %d 在执行转加密任务\n",gEncryptPipe);
            return 2;
        }else
        {
            gEncryptPipe = pipe;
        }
        uint8 flag = 0;
        for(i = encryptinfo->msindex;i < gFileUpdateEncrypt.msnum;i++)
        {
            if(pipe == gFileUpdateEncrypt.msinfo[i].Pipe)
            {
                flag = 1;
                encryptinfo->msindex = i;
                break;
            }
        }
        if(flag == 0)
        {

            encryptinfo->updateflag = 1;
            encryptinfo->delay = 30;        //30秒后查询
            encryptinfo->sendtime = time(NULL);
            updateinfo->FileState = FILE_DELAY;
            if(gEncryptPipe == pipe)
            {
                gEncryptPipe = -1;
            }

            TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表转加密升级完成 延时 %d 秒后读取升级状态\n",encryptinfo->delay);
            return 2;
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 %d 转加密升级初始化\n",gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn);

        framelen = taskmng_file_transfer_encrypt_init_pack(pipe,gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_encrypt_init_pack error !\n");
            return 2;
        }
        tsa = taskmng_tsa_get(gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,0);
    }
        break;
    case FILE_ENCRYPT_COUNTER:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 %d 转加密升级获取计数器\n",gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn);

        framelen = taskmng_file_transfer_encrypt_couter_pack(pipe,gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_encrypt_couter_pack error !\n");
            return 2;
        }
        tsa = taskmng_tsa_get(gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,0);

    }
        break;
    case FILE_ENCRYPT_CONNECT:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 %d 转加密升级建立应用连接\n",gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn);

        framelen = taskmng_file_transfer_encrypt_connect_pack(pipe,encryptinfo->msindex,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_encrypt_connect_pack error !\n");
            return 2;
        }
        tsa = taskmng_tsa_get(gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,0);
    }
        break;
    case FILE_ENCRYPT_UPDATE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "电表 %d 转加密升级启动升级\n",gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn);

        framelen = taskmng_file_transfer_encrypt_update_pack(pipe,encryptinfo->msindex,framebuf);
        if(framebuf==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_file_transfer_encrypt_connect_pack error !\n");
            return 2;
        }
        tsa = taskmng_tsa_get(gFileUpdateEncrypt.msinfo[encryptinfo->msindex].Sn,0);
    }
        break;
    case FILE_DELAY:
    {
        if ( time(NULL) - encryptinfo->sendtime >= encryptinfo->delay)
        {
            if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
            {
                updateinfo->MeterIndex = 0;
            }else
                updateinfo->MeterIndex = 1;
            updateinfo->FileState = FILE_READ;
            encryptinfo->delay = 0;
        }else
        {
            sleep(1);
        }
        return 2;
    }
        break;
    default:
        break;
    }
    if(pipe == 0)
    {
        if(isbroad == 0)
        {
            memcpy_r(&payload[index], (uint8 *) &pipeOAD.value, 4);
            index += 4;
            payload[index++] = PRTL_OOP;
            memcpy(&payload[index], tsa.add, 6);
            index += 6;
            index += 2;
            memcpy_r(&payload[index], (uint8 *) &Overtime, 2);
            index += 2;
            memcpy_r(&payload[index], (uint8 *) &Overtime, 2);
            index += 2;

            if(framelen < 0x80)
            {
                payload[index++] = framelen;
            }
            else
            {
                payload[index++] = 0x82;
                payload[index++] = (uint8) (framelen >> 8);
                payload[index++] = (uint8)
                framelen;
            }

            memcpy(&payload[index], framebuf, framelen);
            index += framelen;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "taskmng_file_transfer_send \n");
            TASK_BUF_DEBUG(pipe, RELAY_LOG, payload, index);
            taskmng_msg_data_pack(pipe, 0x0003, 0x0038, index, payload, &updateinfo->SendMsgIndex);
        }
        else
        {
            //广播发送
            memcpy_r(&payload[index], (uint8 *) &pipeOAD.value, 4);
            index += 4;
            payload[index++] = PRTL_OOP;
            index += 2;

            if(framelen < 0x80)
            {
                payload[index++] = framelen;
            }
            else
            {
                payload[index++] = 0x82;
                payload[index++] = (uint8) (framelen >> 8);
                payload[index++] = (uint8)
                framelen;
            }

            memcpy(&payload[index], framebuf, framelen);
            index += framelen;
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "taskmng_file_transfer_send broad \n");
            TASK_BUF_DEBUG(pipe, RELAY_LOG, payload, index);
            taskmng_msg_data_pack(pipe, 0x0003, 0x003A, index, payload, &updateinfo->SendMsgIndex);
        }
        updateinfo->isBroad = isbroad;
    }else
    {
        //485发送
        uint8 speed;
        if(isbroad==1)
        {
            for(i=0;i<gSpeedList.num;i++)
            {
                speed = gSpeedList.speed[i];
                rs485_RS232_send( pipe, MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,framebuf, framelen);
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "taskmng_file_transfer_send speed %d \n",speed);
                TASK_BUF_DEBUG(pipe, RELAY_LOG, framebuf, framelen);
                sleep(3);
            }
        }
        else
        {
            OOP_METER_BASIC_T MeterBasicInfo;
            memset(&MeterBasicInfo,0x00,sizeof(MeterBasicInfo));
            MeterBasicInfo.baud = 10;   //给个默认值
            taskmng_mtr_basic_info_get_by_tsa(&tsa,&MeterBasicInfo);
            speed = MeterBasicInfo.baud;
            //转加密相关的超时时间给15秒
            if(updateinfo->FileState >= FILE_ENCRYPT_INIT)
                rs485_RS232_send( pipe, 15,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,framebuf, framelen);
            else
                rs485_RS232_send( pipe, MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,framebuf, framelen);
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "taskmng_file_transfer_send speed %d \n",speed);
            TASK_BUF_DEBUG(pipe, RELAY_LOG, framebuf, framelen);
        }
        updateinfo->isBroad = isbroad;

    }


    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_sendbuf_pack
* 函数功能: 组织文件传输发送报文
* 输入参数:pipe 端口号
        filebuf 发送内容
        size 长度
* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_sendbuf_pack(uint8 pipe,OOP_TSA_T tsa,uint16 fileindex,uint8 *filebuf,uint16 size,BOOL bEncode,uint8* outbuf)
{
    //AmrThreadArg_T* pAmrThreadArg;
    //pAmrThreadArg = &gAmrThreadArg[pipe];
    //UPDATE_INFO_T *updateinfo = &pAmrThreadArg->UpdateInfo;
    uint8 buf[MAX_MSG_LEN]={0};
    uint16 index = 0;
    OOP_OMD_U omd = {0xF0010800};
    uint8 offset=0;
    LinkLayer_t *pLinkLayer = NULL;
    uint16 len = 0;

    //组0701后面数据
    memcpy_r(buf,&omd,4);
    index+=4;
    buf[index++] = DT_STRUCTURE;
    buf[index++] = 2;
    buf[index++] = DT_LONG_UNSIGNED;
    memcpy_r(&buf[index],&fileindex,2);
    index+=2;
    buf[index++] = DT_OCTET_STRING;
    offset = set_len_offset(size, &buf[index]);
    index+=offset;
    memcpy(&buf[index],filebuf,size);
    index+=size;
    if(tsa.type == 2 && bEncode == 1)
    {
        OopAction.requst(pipe,&tsa,4,buf,index);
    }else
    {
        OopAction.requst(pipe,&tsa,1,buf,index);
    }

    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        len = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,len);
    }

    return len;
}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_update_result_save
* 函数功能:存储电表升级结果
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_file_transfer_update_result_save(uint8 pipe,uint8 result,OOP_TSA_T *tsa)
{
    WRITE_RECORD_T inData;
    memset(&inData,0x00,sizeof(WRITE_RECORD_T));
    uint16 pn;
    uint32 timenow,storeNo;
    int ret;

    pn = taskmng_pn_get_from_tsa_new(tsa);
    if(pn == 0)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"表地址找不到对应信息点号 %02x%02x%02x%02x%02x%02x \n",tsa->add[0],
                    tsa->add[1],tsa->add[2],tsa->add[3],tsa->add[4],tsa->add[5]);
    }
    TASK_FMT_DEBUG(pipe, RELAY_LOG,"pn %d 写结果 表地址 %02x%02x%02x%02x%02x%02x \n",pn,tsa->add[0],
                    tsa->add[1],tsa->add[2],tsa->add[3],tsa->add[4],tsa->add[5]);
    inData.recordOAD.infoNum = pn;
    inData.recordOAD.classtag = 5;
    inData.recordOAD.road.oadMain.value = 0;
    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0].value = 0x40270200;
    inData.MAC.nNum = 6;
    memcpy(inData.MAC.value,tsa->add,6);
    timenow = time(NULL);
    inData.colStartTm=timenow;
    inData.colStoreTm=timenow;
    inData.colEndTm=timenow;
    inData.inDatalen = 1;
    inData.inDataBuf[0] = result;
    ret = db_write_record(gAmrThreadArg[pipe].DBClientHandle, &inData, &storeNo);

    if(ret != 0)
    {
        TASK_FMT_TRACE ( pipe,RELAY_LOG, "升级结果写失败 ret is %d\n",ret);
        return;
    }
    TASK_FMT_DEBUG(pipe, RELAY_LOG,"pn %d 升级结果 %d 写成功 \n",pn,result);

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_recv
* 函数功能://文件代发收到回码后的处理
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 0 收到正确回码 -1 回码错误
*******************************************************************************/
int taskmng_file_transfer_recv(uint8 pipe,uint8* buf,uint16 len)
{
    UPDATE_INFO_T *updateinfo = &gAmrThreadArg[pipe].UpdateInfo;
    FILE_UPDATE_ENCRYPT_INFO_T *encryptinfo = &gAmrThreadArg[pipe].UpdateEncryptInfo;
    int ret;
    AppLayerList_t* pAppLayer = NULL;

    uint16 index = 0;
    uint8 lenoffset;
    uint16 framelen;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint16 RecvLen;
    //广播收到的是bool 从节点监控收到的是octet-string按698解帧 apdu
    if(len >1)
    {
        //framelen= XADR_GetEleNum(buf, &lenoffset);
        //485也走这 所以直接从第一个字节开始
        //ret = LinkDataReceive(pipe, &buf[lenoffset], framelen,NULL);
        ret = LinkDataReceive(pipe, buf, len,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"taskmng_file_transfer_recv  698解帧返回值 %d \n",ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
            return -1 ;

        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            return -1;
        memset(RecvBuf,0x00,sizeof(RecvBuf));
        RecvLen = pAppLayer->recvApduList->apduLen;
        memcpy(RecvBuf, pAppLayer->recvApduList->apdu, RecvLen);
    }

    switch(updateinfo->FileState)
    {
    case FILE_START:
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动文件传输收到回码 \n");
        updateinfo->startcnt++;
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
        {
            //apdu 87 01 09 f0 01 0E 00 00 00 00 00
            //判断是否回复成功
            uint8 result = RecvBuf[7];
            if(result == 0)
            {
                updateinfo->FileState = FILE_SEND;
            }else
                return -1;
        }else if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            //广播是否收到确认帧
            if(len == 1 && buf[0]!=0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动文件传输广播成功 启动次数 %d \n",updateinfo->startcnt);
                if(updateinfo->startcnt >=3 )
                    updateinfo->FileState = FILE_SEND;
            }else
                return -1;
        }
    }
        break;
    case FILE_SEND:
    {
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
        {
            //apdu 87 01 09 f0 01 08 00 00 00 00 00
            //判断是否回复成功
            uint8 result = RecvBuf[7];
            if(result == 0)
            {
                //切换下一个表
                updateinfo->MeterInfo[updateinfo->MeterIndex].FrameIndex++;
                updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt = 0;
            }else
                return -1;
        }else if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
        {
            //广播是否收到确认帧
            if(len == 1 && buf[0]!=0)
            {
                updateinfo->MeterInfo[0].FrameIndex++;
            }else
                return -1;
        }
    }
        break;
    case FILE_CHECK:
    {
        //85 01 20 f0 01 04 00 01 04 81 15 ff ff f8 00 00
        index = 3;
        OOP_OAD_U oad;
        uint8 state[500]={0};
        uint16 statelen;
        memcpy_r(&oad, &RecvBuf[index],4);
        index+=4;
        if(oad.value!=0xF0010400)
            return -1;
        if(RecvBuf[index++]!=1)
        {
            return -1;
        }
        if(RecvBuf[index++]!=DT_BIT_STRING)
        {
            return -1;
        }
        framelen= XADR_GetEleNum(&RecvBuf[index], &lenoffset);
        index+=lenoffset;
        statelen = framelen/8;
        if(framelen%8 >0)
        {
            statelen++;
        }
        memcpy(state,&RecvBuf[index],statelen);
        taskmng_file_transfer_state_unpack(pipe,state,statelen);
        updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt = 0;
        updateinfo->MeterIndex++;
    }
        break;
    case FILE_RESEND:
    {
        if(updateinfo->MeterInfo[0].AddrFlag == ADDR_SINGLE)
        {
            uint8 result = RecvBuf[7];
            if(result == 0)
            {
                //切换下一个表
                updateinfo->MeterInfo[updateinfo->MeterIndex].FrameResendIndex++;
                updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt = 0;
            }else
                return -1;

        }else
        {
            if(len == 1 && buf[0]!=0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG,"重发送广播成功 \n");
                updateinfo->MeterInfo[updateinfo->MeterIndex].FrameResendIndex++;
                updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt = 0;
            }else
                return -1;
        }

    }
        break;
    case FILE_READ:
    {

        index = 3;
        uint8 updateResult = 0;
        OOP_OAD_U oad;
        OOP_TSA_T tsa ;
        memset(&tsa,0x00,sizeof(OOP_TSA_T));
        memcpy_r(&oad, &RecvBuf[index],4);
        index+=4;
        if(oad.value!=0x40270200)
            return -1;
        if(RecvBuf[index++]!=1)
        {
            return -1;
        }
        if(RecvBuf[index++]!=DT_ENUM)
        {
            return -1;
        }
        updateResult = RecvBuf[index++];
        //把结果存起来
        tsa.len = 5;
        memcpy_r(tsa.add,updateinfo->MeterInfo[updateinfo->MeterIndex].Addr,6);

        taskmng_file_transfer_update_result_save( pipe, updateResult, &tsa);

        updateinfo->MeterInfo[updateinfo->MeterIndex].FainCnt = 0;
        updateinfo->MeterIndex++;
    }
        break;
    case FILE_ENCRYPT_INIT:
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"文件升级转加密初始化收到apdu \n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,RecvBuf,RecvLen);

        if( RecvBuf[10]!=04)
            return -1;
        index = 11;  //从表号开始
        if(RecvBuf[index]!=9 && RecvBuf[index+1]!=8)
            return -1;
        //获取到表号 下面的暂时不需要
        memcpy(encryptinfo->encryptMetNo,&RecvBuf[index+2],8);
        updateinfo->FileState = FILE_ENCRYPT_COUNTER;
    }
        break;
    case FILE_ENCRYPT_COUNTER:
    {
        //从oad后面的结果类型 开始
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"文件升级转加密获取计数器收到apdu\n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,RecvBuf,RecvLen);


        if(RecvBuf[7]!=1 && RecvBuf[8]!=DT_DOUBLE_LONG_UNSIGNED)
            return -1;

        memcpy_r(&encryptinfo->encryptCounter,&RecvBuf[9],4);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"获取到计数器 %d \n",encryptinfo->encryptCounter);
        updateinfo->FileState = FILE_ENCRYPT_CONNECT;   //下一步建立应用连接
    }
        break;
    case FILE_ENCRYPT_CONNECT:
    {
        uint8 databuf[2000]={0};
        ret = security_sessonkey_get(encryptinfo->key.value,encryptinfo->key.nNum,pAppLayer->serverConnectInfo.rand,
                    pAppLayer->serverConnectInfo.randLen,pAppLayer->serverConnectInfo.sign,pAppLayer->serverConnectInfo.signLen,databuf);
        if(ret==0)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"security_sessonkey_get error \n");
            return -1;
        }
        encryptinfo->sessionKey.nNum = ret;
        memcpy(encryptinfo->sessionKey.value,databuf,ret);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"获取到会话密钥包 \n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,databuf,ret);
        updateinfo->FileState = FILE_ENCRYPT_UPDATE;   //下一步启动升级

    }
        break;
    case FILE_ENCRYPT_UPDATE:
    {
        uint8 secretdata[1000]={0};
        uint16 secretlen;
        uint8 mac[4]={0};
        uint16 index = 0;
        uint8 databuf[2000]={0};

        index = 2;
        secretlen = RecvBuf[index++];
        memcpy(secretdata,RecvBuf+3,secretlen);
        index+=secretlen;
        if(RecvBuf[index++]==1 && RecvBuf[index++]==0 && RecvBuf[index++] == 4)
        {
            memcpy(mac,pAppLayer->recvApduList->apdu+index,4);
        }
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"收到的回码密文\n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,secretdata,secretlen);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"收到的回码mac\n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,mac,4);

        //发给esam解密
        ret = security_decrypt_sec_mac(encryptinfo->sessionKey.value,encryptinfo->sessionKey.nNum,secretdata,secretlen,mac,databuf);
        if(ret==0)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"security_decrypt_sec_mac error \n");
            return -1;
        }

        TASK_FMT_DEBUG(pipe, RELAY_LOG,"解密后明文apdu \n");
        TASK_BUF_DEBUG(pipe, RELAY_LOG,databuf,ret);
        //87 01 00 f0 01 0d 00 00 00 00 01 07 e6 08 03 0a 21 0d 05 00 05
        OOP_OMD_U omd = {0};
        memcpy_r(&omd.value,databuf+3,4);
        if(omd.value==0xF0010D00 && databuf[7]==0)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动升级成功 \n");
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动升级失败 \n");
        }
        encryptinfo->msindex++;
        updateinfo->FileState = FILE_ENCRYPT_INIT;
    }
        break;
    default:
        break;
    }
    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_file_transfer_state_unpack
* 函数功能: 解析回复的文件传输块状态字
* 输入参数:pipe 端口号
        buf 状态字

* 输出参数:
* 返 回 值:
*******************************************************************************/

void taskmng_file_transfer_state_unpack(uint8 pipe,uint8 *buf,uint16 len)
{
    UPDATE_INFO_T *updateinfo = &gAmrThreadArg[pipe].UpdateInfo;
    UPDATE_METER_INFO_T *meterinfo =  &updateinfo->MeterInfo[updateinfo->MeterIndex];
    uint16 num = 0;
    uint16 i,j;
    uint16 index,bitindex;
    uint8 flag;
    TASK_FMT_DEBUG(pipe, RELAY_LOG,"获取到的状态字       \n");
    TASK_BUF_DEBUG(pipe, RELAY_LOG,buf,len);
    //bitstring 为大端
    for(i=0;i<updateinfo->FrameCount;i++)
    {
        index = i/8;
        bitindex =  7 - i%8;
        if(((buf[index]>>bitindex) & 1) == 0)
        {
            meterinfo->FrameResendSeq[num] = i;
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"num %d 缺的包索引号 %d       \n",num,i);
            num++;
            if(num>=100)
                break;
        }
    }
    meterinfo->FrameResendCount = num;
    meterinfo->FrameResendIndex = 0;
    //为台体做点特殊处理 如果485第一次查询没缺 也固定补索引5
    if(pipe == 2 && meterinfo->CheckCnt == 1 && num ==0 && updateinfo->bEncode == 1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"485 特殊处理 补发索引5 \n");
        meterinfo->FrameResendCount = 1;
        meterinfo->FrameResendSeq[0] = 5;
        num = 1;
    }
    if(num == 0)
    {
        meterinfo->UpdateResult = UPDATE_DOWNLOAD_SUCC;
        meterinfo->CheckResult = 1;
    }
    //如果是组地址   要把没块表缺的索引求并集写到组地址里面去 然后再通过广播补发
    if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP)
    {
        for(i=0;i<num;i++)
        {
            flag = 0;
            for(j=0;j<updateinfo->MeterInfo[0].FrameResendCount;j++)
            {
                if(meterinfo->FrameResendSeq[i] == updateinfo->MeterInfo[0].FrameResendSeq[j])
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 1)
                continue;
            updateinfo->MeterInfo[0].FrameResendSeq[updateinfo->MeterInfo[0].FrameResendCount] = meterinfo->FrameResendSeq[i];
            updateinfo->MeterInfo[0].FrameResendCount++;

            if(updateinfo->MeterInfo[0].FrameResendCount>=100)
                break;
        }
        updateinfo->MeterInfo[0].FrameResendIndex = 0;
        if(updateinfo->MeterInfo[0].FrameResendCount == 0)
        {
            updateinfo->MeterInfo[0].UpdateResult = UPDATE_DOWNLOAD_SUCC;
            updateinfo->MeterInfo[0].CheckResult = 1;
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_check_pack
* 函数功能: 组织F0010400查询报文
* 输入参数:pipe 端口号
        tsa 地址
* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_check_pack(uint8 pipe,OOP_TSA_T tsa,uint8* outbuf)
{

    OOP_OAD_U oad = {0xF0010400};
    LinkLayer_t *pLinkLayer = NULL;
    uint16 len = 0;

    OopGet.requst(pipe,&tsa,1,(uint8*)&oad.value,4);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        len = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,len);
    }
    return len;
}
/*******************************************************************************
* 函数名称: taskmng_file_transfer_result_check
* 函数功能://mqtt线程中收到0021设备升级结果处理
* 输入参数:buf 消息内容
        len 消息长度

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_file_transfer_result_check(MESSAGE_INFO_T* MsgInfo )
{
    uint8 index = 0;
    uint8 payload[200]={0};
    uint8 pipe;
    uint16 i;
    uint8 flag = 0;
    UPDATE_INFO_T *updateinfo = NULL;
    if(MsgInfo->msglen!=7)
        return;
    ADDR_FLAG addrflag = MsgInfo->playload[index++];
    uint8 addr[6]={0};
    memcpy_r(addr,&MsgInfo->playload[index],6);

    index = 0;
    if(addrflag == ADDR_SINGLE)
    {
        pipe = taskmng_pipe_get_from_addr(addr,LOGIC_ID_YC);
        if(pipe>1)
        {
            TASK_FMT_DEBUG(-1,MQTT_LOG,"未在档案中找到该设备地址\n");
            return ;
        }
        payload[index++] = 1;
        index++;
        memcpy_r(&payload[index],addr,6);
        index +=6;

        updateinfo = &gAmrThreadArg[pipe].UpdateInfo;
        for(i=0;i<updateinfo->MeterNum;i++)
        {
            if(memcmp(addr,updateinfo->MeterInfo[i].Addr,6)==0)
            {
                payload[index++] = updateinfo->MeterInfo[i].UpdateResult;
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            TASK_FMT_DEBUG(-1,MQTT_LOG,"未在设备地址集合中找到该设备地址\n");
            return;
        }
    }else if(addrflag == ADDR_GROUP)
    {
        for(i = 0;i<PLC_NUM;i++)
        {
            updateinfo = &gAmrThreadArg[i].UpdateInfo;
            if(updateinfo->MeterInfo[0].AddrFlag == ADDR_GROUP && memcmp(updateinfo->MeterInfo[0].Addr,addr,6)==0)
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            TASK_FMT_DEBUG(-1,MQTT_LOG,"未在设备地址集合中找到该组地址\n");
            return;
        }
        payload[index++] = updateinfo->MeterNum-1;
        for(i=1;i<updateinfo->MeterNum;i++)
        {
            memcpy_r(&payload[index],updateinfo->MeterInfo[i].Addr,6);
            index+=6;
            payload[index++] = updateinfo->MeterInfo[i].UpdateResult;
        }
    }
    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 0;
    MsgSend->index = MsgInfo->index;
    MsgSend->priority = MsgInfo->priority;
    MsgSend->label = MsgInfo->label;
    MsgSend->sourlen = strlen(APPNAME);
    memcpy(MsgSend->souraddr,APPNAME,MsgSend->sourlen);
    MsgSend->destlen = MsgInfo->sourlen;
    memcpy(MsgSend->destaddr,MsgInfo->souraddr,MsgSend->destlen );
    MsgSend->IID = MsgInfo->IID;
    MsgSend->IOP = MsgInfo->IOP;
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);

    taskmng_plc_queue_write ( APPArg.SendBuf, (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index );
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: taskmng_data_init_check
* 函数功能:抄表线程将记录的事件上报序号清零
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_data_init_check(uint8 pipe)
{
    if(DataInitFlag[pipe]==0)
        return;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //CTS_RUN_TASK_T           *pTaskHead;
    uint16 i;
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        pAmrThreadArg->pMtrAmrInfo[i].powereventseq = 0;
    }
    //删除任务记录文件
    char pfile[200];
    int ret;
    uint8 DayCnt ;
    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        //taskmng_task_monitor_init(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid);
        memset(pfile,0x00,sizeof(pfile));
        sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,pAmrThreadArg->pTaskRunInfo[i].TaskId,pipe,LOGIC_ID_YC);
        if(access(pfile,R_OK)==0)
        {
            ret=remove(pfile);
            TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务记录文件%s ret is %d\n",pfile,ret);
        }
        pAmrThreadArg->pTaskRunInfo[i].RunResult = SCHEME_UNDONE;
        memset(pAmrThreadArg->pTaskRunInfo[i].ThisTurnBeginTime,0x00,sizeof(char)*7);
        memset(pAmrThreadArg->pTaskRunInfo[i].ThisTurnEndTime,0x00,sizeof(char)*7);
        memset(pAmrThreadArg->pTaskRunInfo[i].NextTurnBeginTime,0x00,sizeof(char)*7);
        memset(pAmrThreadArg->pTaskRunInfo[i].LastTurnBeginTime,0x00,sizeof(char)*7);
        //删除补抄文件

        for(DayCnt =0 ;DayCnt<PATCH_MAX_DAY;DayCnt++)
        {
            sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,pAmrThreadArg->pTaskRunInfo[i].TaskId,DayCnt,pipe);
            if(access(pfile,R_OK)==0)
            {
                ret=remove(pfile);
                TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务补抄文件%s ret is %d\n",pfile,ret);
            }
        }
    }
    //初始化的时候也把事件上报的清掉
    memset(&pAmrThreadArg->EventReport, 0x00, sizeof(EVENT_REPORT_T));
    pAmrThreadArg->TotalState.EventReportState = INNER_RTS_IDLE;

    if(pAmrThreadArg->pTaskHead != NULL)
    {
        pAmrThreadArg->pTaskHead->RunResult = SCHEME_UNDONE;
    }
    pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
    DataInitFlag[pipe] = 0;

}
void JudgeNeedDeleteTaskRecord(DateTime_T curtime,DateTime_T passtime,uint8 pipe)
{
    DateTimeHex_t nowtime;
    DateTimeHex_t lasttime;
    time_t cursecond;
    time_t passsecond;
    uint16 i;
    uint16 year=0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    BOOL rmv=FALSE;

    year = 2000+curtime.Year;
    nowtime.year_h = year>>8;
    nowtime.year_l = year&0xFF;
    nowtime.month  = curtime.Month;
    nowtime.day    = curtime.Day;
    nowtime.hour = curtime.Hour;
    nowtime.min = curtime.Minute;
    nowtime.sec = curtime.Second;

    year = 2000+passtime.Year;
    lasttime.year_h = year>>8;
    lasttime.year_l = year&0xFF;;
    lasttime.month  = passtime.Month;
    lasttime.day    = passtime.Day;
    lasttime.hour = passtime.Hour;
    lasttime.min = passtime.Minute;
    lasttime.sec = passtime.Second;

    DT_DateTimeHex2Time(&nowtime,&cursecond);
    DT_DateTimeHex2Time(&lasttime,&passsecond);
    if(cursecond < passsecond)
    {
            //删除任务记录文件
        char pfile[200];
        int ret;

        for(i=0;i<pAmrThreadArg->TaskNum;i++)
        {

            SCHEME_TASK_RUN_INFO TaskInfo;
            memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
            taskmng_task_info_get(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid, &TaskInfo);
            if(TaskInfo.CsdTask.RoadTask[0].MainOad.value != 0x50060200)
            {
                memset(pfile,0x00,sizeof(pfile));
                sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,pAmrThreadArg->pTaskRunInfo[i].TaskId,pipe,LOGIC_ID_YC);
                if(access(pfile,R_OK)==0)
                {
                    ret=remove(pfile);
                    TASK_FMT_DEBUG(pipe,REC_LOG, "往前对时过日删除任务记录文件%s ret is %d\n",pfile,ret);
                    rmv = TRUE;
                }
                memset(pAmrThreadArg->pTaskRunInfo[i].ThisTurnBeginTime,0x00,7);
                memset(pAmrThreadArg->pTaskRunInfo[i].ThisTurnEndTime,0x00,7);
            }
        }
        if(rmv == TRUE)
        {
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
        }
    }

}
/*******************************************************************************
* 函数名称: taskmng_time_manage
* 函数功能: 时间管理函数
* 输入参数: 端口号
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_time_manage ( uint8 CommPipe )
{
    gPastTimeFlag[CommPipe].Word = 0;          //清除时间标志

    taskmng_cur_bin_time_get ( &gCurBinTime[CommPipe] );  //刷新当前HEX码时间
    taskmng_cur_bcd_time_get ( &gCurBcdTime[CommPipe] );  //刷新当前BCD码时间

    //过月
    if ( gCurBinTime[CommPipe].Month != gPreBinTime[CommPipe].Month )
    {
        //同时产生过月、过日、过时、过分、过秒标志
        gPastTimeFlag[CommPipe].BitSect.Month   = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Day     = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过日
    if ( gCurBinTime[CommPipe].Day != gPreBinTime[CommPipe].Day )
    {
        //同时产生过日、过时、过分标志
        if(CommPipe >= PLC_NUM)
        {
            JudgeNeedDeleteTaskRecord(gCurBinTime[CommPipe],gPreBinTime[CommPipe],CommPipe);
        }
        gPastTimeFlag[CommPipe].BitSect.Day     = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过时
    else if ( gCurBinTime[CommPipe].Hour != gPreBinTime[CommPipe].Hour )
    {
        //同时产生过时、过分标志
        gPastTimeFlag[CommPipe].BitSect.Hour    = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Minute  = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second  = TRUE;
    }
    //过分
    else if ( gCurBinTime[CommPipe].Minute != gPreBinTime[CommPipe].Minute )
    {
        //产生过分标志
        gPastTimeFlag[CommPipe].BitSect.Minute = TRUE;
        gPastTimeFlag[CommPipe].BitSect.Second = TRUE;
        #ifdef AREA_JIANGSU
        TASK_FMT_DEBUG(CommPipe,REC_LOG, "gCurBinTime[CommPipe].Minute is %d\n",gCurBinTime[CommPipe].Minute);
        if((gCurBinTime[CommPipe].Minute%10==0)||(gCurBinTime[CommPipe].Minute==1))
        {
            TASK_FMT_DEBUG(CommPipe,REC_LOG, "准备开启采集器点名上报\n");
            nodecollect =0xAA;
            nodecount = 0;
            taskmng_HPLC_node_info_get(CommPipe);
            minutecollect =1;
            TASK_FMT_DEBUG(CommPipe,REC_LOG, "minutecollect11 is %d\n",minutecollect);
            taskmng_router_version_query(CommPipe);
            //taskmng_node_minute_collect_check(CommPipe);
        }
        #endif
//        if((access(DELAY_FILE_PATH,F_OK)==0)&&(CommPipe == 2))
//        {
//            if(gdelay == 0)
//            {
//                gdelay = 1;
//                TASK_FMT_DEBUG(CommPipe,REC_LOG, "检测到开始减压\n");
//            }
//        }
//        else
//        {
//            if((gdelay != 0)&&(CommPipe == 2))
//            {
//                gdelay = 0;
//            }
//        }
        if(CommPipe == 2)
        {
            if((access(DELAY_FILE_PATH,F_OK)==0))
            {
                if(gdelay == 0)
                {
                    gdelay = 1;
                    gdelaytime = DELAYTIME;
                    TASK_FMT_DEBUG(CommPipe,REC_LOG,"检测到开始减压\n");
                }
            }else if((access(DELAY_FILE_PATH2,F_OK)==0))
            {
                if(gdelay != 2)
                {
                    gdelay = 2;
                    gdelaytime = DELAYTIME_MORE;
                    TASK_FMT_DEBUG(CommPipe,REC_LOG,"进一步减压\n");

                }
            }
            else
            {
                if(gdelay != 0)
                {
                    gdelay = 0;
                    gdelaytime = 0;
                }
            }
        }
    }

    //过秒
    else if ( gCurBinTime[CommPipe].Second != gPreBinTime[CommPipe].Second )
    {
        gPastTimeFlag[CommPipe].BitSect.Second = TRUE;
    }
    else
    {
        //usleep ( 10000 );
    }

    //过月处理
    if ( gPastTimeFlag[CommPipe].BitSect.Month == TRUE )
    {
        ;
    }

    //过日处理
    if ( gPastTimeFlag[CommPipe].BitSect.Day == TRUE )
    {
        if(CommPipe<1)
        {
            ReOrderPatchFile ( CommPipe );//过日时更新补抄文件中时标
            CreatPatchFile(CommPipe, 0);//过日创建当天的补抄文件
            gAmrThreadArg[CommPipe].IsResetCCO = 0;
        }

    }

    //过时处理
    if ( gPastTimeFlag[CommPipe].BitSect.Hour == TRUE )
    {
        //taskmng_HPLC_info_get(CommPipe);
    }

    //过分处理
    if ( gPastTimeFlag[CommPipe].BitSect.Minute == TRUE )
    {
        if(CommPipe<1)
        {
            taskmng_update_cycle_check(CommPipe);
        }
        gAmrThreadArg[CommPipe].Isadjusttime = 0;
    }

    if ( gPastTimeFlag[CommPipe].BitSect.Second == TRUE )
    {
        //taskmng_task_run_ergodic(CommPipe);
        if(CommPipe<1)
        {
            SearchMeterCheck(CommPipe);

        }
        SyncTimingCheck(CommPipe);
    }

    memcpy ( &gPreBinTime[CommPipe], &gCurBinTime[CommPipe], sizeof ( DateTime_T ) );

}

/*******************************************************************************
* 函数名称: taskmng_task_run_init
* 函数功能: 每个线程运行时任务参数初始化
* 输入参数:
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
BOOL taskmng_task_run_init(uint8 Pipe)
{
    uint8 i;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    memset(pAmrThreadArg->pTaskRunInfo,0x00,sizeof(CTS_RUN_TASK_T)*SCHEME_MAX_NUM);
    pAmrThreadArg->pTaskHead = NULL;
    pAmrThreadArg->TaskNum = 0;
      //如果任务队列还未初始化好 则不运行

    if(SchemeTaskCfgList.TaskCfgState!=TASK_CFG_INITED)
    {
        return FALSE;
    }
    TASK_FMT_DEBUG( Pipe, REC_LOG, "开始任务初始化 ！ \n");
    pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);
    total_col_flg[Pipe] = 0;
    patch_policy_flg[Pipe] = 0;
    for(i=0;i<SchemeTaskCfgList.TaskNum;i++)
    {
        pAmrThreadArg->pTaskRunInfo[i].logicid = SchemeTaskCfgList.OopTaskCfg[i].logicid;
        pAmrThreadArg->pTaskRunInfo[i].TaskId = SchemeTaskCfgList.OopTaskCfg[i].TaskId;
        pAmrThreadArg->pTaskRunInfo[i].Priority = SchemeTaskCfgList.OopTaskCfg[i].Priority;
        pAmrThreadArg->pTaskRunInfo[i].TaskState = SchemeTaskCfgList.OopTaskCfg[i].TaskState;
        pAmrThreadArg->pTaskRunInfo[i].MainOad = taskmng_task_mainoad_get(i);
        pAmrThreadArg->pTaskRunInfo[i].TotalMtrNum = taskmng_task_need_mtr_num_get(i);
        pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum = GetNeedMtrNum(Pipe,i);
        pAmrThreadArg->pTaskRunInfo[i].RunResult = SCHEME_UNDONE;
        memset(pAmrThreadArg->pTaskRunInfo[i].MtrResult,0x00,128);
        TASK_FMT_DEBUG( Pipe, REC_LOG, "TaskId %d mainoad %08x TotalMtrNum %d  NeedMtrNum %d ！\n",pAmrThreadArg->pTaskRunInfo[i].TaskId,
            pAmrThreadArg->pTaskRunInfo[i].MainOad,pAmrThreadArg->pTaskRunInfo[i].TotalMtrNum,pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum);

        if(SchemeTaskRunQueue[i].AcqInfo.Acqtype == ACQTYP_BC && SchemeTaskRunQueue[i].AcqInfo.LastN == 62)
            total_col_flg[Pipe] = 1;
#ifndef PRODUCT_CCU
        if(pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum == 200)
            patch_policy_flg[Pipe] = 1;
        if(pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum == 300 && SchemeTaskRunQueue[i].AcqInfo.Acqtype == ACQTYP_NULL)
            patch_policy_flg[Pipe] = 2;
#else
        if(pAmrThreadArg->pTaskRunInfo[i].TotalMtrNum == 300 && pAmrThreadArg->pTaskRunInfo[i].TaskId == 1)
            patch_policy_flg[Pipe] = 1;
        if(pAmrThreadArg->pTaskRunInfo[i].TotalMtrNum == 1000)
            patch_policy_flg[Pipe] = 2;
#endif
    }
    pAmrThreadArg->TaskNum = SchemeTaskCfgList.TaskNum;
    TASK_FMT_DEBUG( Pipe, REC_LOG, "任务初始化完成 任务个数 %d, total_col_flg:%d, patch_policy_flg:%d！\n",pAmrThreadArg->TaskNum,total_col_flg[Pipe],patch_policy_flg[Pipe]);
    pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);
    nodecollectcount =taskmng_get_tsa_count();
    TaskRunSort(Pipe);
    return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_task_run_ergodic
* 函数功能: 每个线程内更新任务运行时参数
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_run_ergodic(uint8 Pipe)
{
    uint8 i;
    BOOL          IsNewTurn = FALSE;
    AmrThreadArg_T* pAmrThreadArg;
    time_t  RealTime_t ;
    //static uint8 lasthour[AMR_THREAD_NUM] ;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    if(pAmrThreadArg->TaskNeedInit == 1)
    {
        pAmrThreadArg->TaskModifyFlag = TASK_CFG_MODIFY;
        pAmrThreadArg->TaskNeedInit =0;
    }
    switch(pAmrThreadArg->TaskModifyFlag)
    {
    case TASK_CFG_MODIFY:
    {
        pAmrThreadArg->ProCfgModifyDelaySec = 2;
        pAmrThreadArg->FunLastRunTime = time(NULL);
        pAmrThreadArg->TaskModifyFlag = TASK_CFG_MODIFY_DELAY;
    }
        break;
    case TASK_CFG_MODIFY_DELAY:
    {
        RealTime_t = time(NULL);
        if(RealTime_t != pAmrThreadArg->FunLastRunTime)
        {
            pAmrThreadArg->ProCfgModifyDelaySec--;
            pAmrThreadArg->FunLastRunTime = RealTime_t;
        }
        if(pAmrThreadArg->ProCfgModifyDelaySec <= 0)
        {
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
        }
    }
        break;
    case TASK_CFG_UNINIT:
    {
        while(1)
        {
            if(taskmng_task_record_file_del_check(Pipe)==0)
                break;
        }
        if(taskmng_task_run_init(Pipe)==TRUE)
        {
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_PATCH;
            //只有和本线程号相同的才修改
            if(gEncryptPipe == Pipe)
            {
                TASK_FMT_TRACE(Pipe, REC_LOG, "gEncryptPipe %d 置为-1\n",gEncryptPipe);
                gEncryptPipe = -1;
            }
            //TASK_FMT_TRACE(Pipe, REC_LOG, "任务初始化完后的状态 pAmrThreadArg->TaskModifyFlag is %d\n",pAmrThreadArg->TaskModifyFlag);
        }
        break;
    }
    case TASK_CFG_PATCH:
    {
        //检查是否存在任务记录文件
        CheckTaskRecordFile(Pipe);
        //任务初始化完成后才检查补抄文件
        ReOrderPatchFile (Pipe ); //上电更新补抄文件时标
        CheckPatchFile(Pipe);//检查前三天补抄文件是否存在
        pAmrThreadArg->TaskModifyFlag = TASK_CFG_INITED;
        pAmrThreadArg->TotalState.PatchAmr = RTS_IDLE;
        break;
    }
    default:
        break;
    }

    //更新每个任务的运行时参数
    //taskmng_task_info_get里面用到另一个锁 和任务管理线程会出现死锁
    //pthread_rwlock_rdlock(&SchemeTaskCfgList.rwLock);


    for(i=0;i<pAmrThreadArg->TaskNum;i++)
    {
        SCHEME_TASK_RUN_INFO TaskInfo;
        memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
        if(pAmrThreadArg->pTaskRunInfo[i].TaskState!=1||pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum==0)
        {
            //TASK_FMT_TRACE( Pipe, REC_LOG,"任务 %d 已停用 无需更新轮次 \n",pAmrThreadArg->pTaskRunInfo[i].TaskId);
            continue;
        }
        taskmng_task_info_get(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid, &TaskInfo);

        IsNewTurn = TaskRun_UpdateTimes(Pipe,i,&pAmrThreadArg->pTaskRunInfo[i]);
        //每个小时运行一次日冻结任务 为应付测试 如果空闲时且少于5块表则会重启日冻结任务
        //if(gCurBinTime[Pipe].Hour!=lasthour[Pipe] ||(gFreeFlag[Pipe]==1 &&pAmrThreadArg->pTaskRunInfo[i].NeedMtrNum<5 ) )
        //为测试《505-补抄策略》，修改下逻辑，载波空闲的时候日冻结如果有失败的就一直执行，参考分钟曲线的逻辑，目前是一个小时检查一次日冻结是否有失败的。档案如果太多的情况下可适当缩小载波的超时时间。
        if(gFreeFlag[Pipe]==1)
        {
#ifdef PRODUCT_CCU
            //系统当前时间
            RealTime_t = time(NULL);
            OOP_DATETIME_S_T    curTime = {0};

            DT_Time2DateTimeShort(RealTime_t, &curTime);
            if(patch_policy_flg[Pipe] == 1)
            {
                if(curTime.minute == 22)
                {
                    IsNewTurn = TRUE;
                    TASK_FMT_TRACE( Pipe, REC_LOG,"日冻结任务重启 \n");
                }
                else if(curTime.minute <= 15 || curTime.minute > 22)
                {
                    IsNewTurn = TRUE;
                    TASK_FMT_TRACE( Pipe, REC_LOG,"日冻结任务重启 \n");
                }
                else
                    TASK_FMT_TRACE( Pipe, REC_LOG,"未到日冻结任务重启时间22min50s \n");
            }
            else
            {
                if((TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50040200||TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50050200)&&pAmrThreadArg->pTaskRunInfo[i].DayFrozenFailCnt>0)
                {
                    IsNewTurn = TRUE;
                    TASK_FMT_TRACE( Pipe, REC_LOG,"日冻结任务重启 \n");
                }
            }
#else
            if((TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50040200||TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50050200)&&pAmrThreadArg->pTaskRunInfo[i].DayFrozenFailCnt>0)
            {
                IsNewTurn = TRUE;
                TASK_FMT_TRACE( Pipe, REC_LOG,"日冻结任务重启 \n");
            }
#endif
            if(TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50060200&&pAmrThreadArg->pTaskRunInfo[i].MonFrozenFailCnt>0)
            {
                IsNewTurn = TRUE;
                TASK_FMT_TRACE( Pipe, REC_LOG,"月冻结任务重启 \n");
            }
        }
        if(gFreeFlag[Pipe]==1)
        {
//            SCHEME_TASK_RUN_INFO TaskInfo;
//            memset(&TaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
            //taskmng_task_info_get(pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid, &TaskInfo);
            if(TaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50020200&&pAmrThreadArg->pTaskRunInfo[i].MinFrozenFailCnt>0

                &&pAmrThreadArg->pTaskRunInfo[i].MinFrozenRebootCnt<3)
            {
                pAmrThreadArg->pTaskRunInfo[i].MinFrozenRebootCnt++;
                TASK_FMT_TRACE( Pipe, REC_LOG," 当前空闲 任务%d失败表数%d 重启次数%d \n",TaskInfo.TaskId,pAmrThreadArg->pTaskRunInfo[i].MinFrozenFailCnt,
                    pAmrThreadArg->pTaskRunInfo[i].MinFrozenRebootCnt);
                IsNewTurn = TRUE;
                //判断需要重启后就直接加入 以防补抄任务的执行会导致存储时标错误
                taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR);
            }
        }
        if(IsNewTurn)
        {
            TASK_FMT_TRACE( Pipe, REC_LOG,"任务 %d IsNewTurn ThisTurnBeginTime \n",pAmrThreadArg->pTaskRunInfo[i].TaskId);
            WriteTimeLog(Pipe,pAmrThreadArg->pTaskRunInfo[i].ThisTurnBeginTime);
            TASK_FMT_TRACE( Pipe, REC_LOG," ThisTurnEndTime LastTurnBeginTime \n");
            WriteTimeLog(Pipe,pAmrThreadArg->pTaskRunInfo[i].ThisTurnEndTime);
            WriteTimeLog(Pipe,pAmrThreadArg->pTaskRunInfo[i].LastTurnBeginTime);
            if(Pipe == PLC_NUM)
            {
                taskmng_task_run_info_reset_485(&pAmrThreadArg->pTaskRunInfo[i],&TaskInfo);

            }
                else
            {
                taskmng_task_run_info_reset(&pAmrThreadArg->pTaskRunInfo[i]);
            }


            taskmng_task_monitor_update(Pipe,pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid,UDTASK_START,&pAmrThreadArg->pTaskRunInfo[i]);
            GetTaskRealBegiantime(Pipe,pAmrThreadArg->pTaskRunInfo[i].TaskId,pAmrThreadArg->pTaskRunInfo[i].logicid,&pAmrThreadArg->pTaskRunInfo[i]);

            TASK_FMT_TRACE( Pipe, REC_LOG," ThisTurnEndTime ThisTurnRealBeginTime \n");
            WriteTimeLog(Pipe,pAmrThreadArg->pTaskRunInfo[i].ThisTurnRealBeginTime);
            //如果到新一轮的任务 直接创建抄表任务
            taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR);
        }
        //更新任务执行状态
        TaskRun_UpdatePeriodStat(i,&pAmrThreadArg->pTaskRunInfo[i]);
    }
    //lasthour[Pipe] = gCurBinTime[Pipe].Hour;
    gFreeFlag[Pipe]=0;
    //pthread_rwlock_unlock(&SchemeTaskCfgList.rwLock);

}

void taskmng_record_monitor_delete_pipe(uint8 pipe,uint16 TaskId,uint8 logicid)
{
    CLEAR_DATA_T RecordDel;
    int ret=0;
    MEMZERO(&RecordDel, sizeof(RecordDel));
    RecordDel.recordOAD.logicId = 0;
    RecordDel.recordOAD.infoNum = TaskId;
    RecordDel.recordOAD.classtag = CLASS_DATA_INIT;
    RecordDel.recordOAD.road.oadMain.value = 0;
    RecordDel.recordOAD.road.oadCols.nNum = 1;
    RecordDel.recordOAD.road.oadCols.oad[0].value = 0x60340200;
    RecordDel.bClassTag = FALSE;
    ret = db_clear_oad_data(gAmrThreadArg[pipe].DBClientHandle,&RecordDel);
    if(ret==0)
    {
        TASK_FMT_DEBUG( pipe, REC_LOG,"采集监控集删除成功！oad %04x 任务 %d \n",RecordDel.recordOAD.road.oadCols.oad[0].value,TaskId);
    }else
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"集监控集删除成功！oad %04x 任务 %d \n",RecordDel.recordOAD.road.oadCols.oad[0].value,TaskId);
    }
    //taskmng_rs485_patch_file_delete(pipe,TaskId);
}

/*******************************************************************************
* 函数名称: taskmng_task_record_file_del_check
* 函数功能: 检查是否有需要删除的任务记录文件
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_task_record_file_del_check(uint8 pipe)
{
    int Recvlen = 0;
    DB_UPDATE_T dbupdate;
    memset(&dbupdate,0x00,sizeof(DB_UPDATE_T));
    Recvlen = taskmng_db_update_queue_read(&pTaskRecordFileDel[pipe], &dbupdate);
    if(Recvlen>0)
    {

        TASK_FMT_DEBUG(pipe, REC_LOG, "收到logicid %d 任务 %d 的变更消息 删除任务记录文件\n",dbupdate.logicid,dbupdate.infonum);
        if(pipe<AMR_THREAD_NUM)
        {
            taskmng_record_file_delete_pipe(pipe,dbupdate.infonum,dbupdate.logicid);
            taskmng_record_monitor_delete_pipe(pipe,dbupdate.infonum,dbupdate.logicid);
        }
        else
            taskmng_record_file_delete_tg(dbupdate.infonum);
        return 1;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_record_file_delete_pipe
* 函数功能:收到任务管理线程发送的任务变更消息，删除任务记录文件
* 输入参数:pipe  端口
        TaskId 任务id
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_record_file_delete_pipe(uint8 pipe,uint16 TaskId,uint8 logicid)
{
    CTS_RUN_TASK_T *tmpTask = NULL;
    char  pfile[100];
    int ret;
    taskmng_run_task_get( pipe,TaskId,logicid,&tmpTask);
    if(tmpTask != NULL)
        tmpTask->TaskState = 2;     //收到任务变更的消息后暂时将该任务停用 延时过后会重新加载任务
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);
    if(access(pfile,R_OK)==0)
    {
        ret=remove(pfile);
        TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务记录文件%s ret is %d\n",pfile,ret);
    }
    //删除补抄文件
    uint8 DayCnt ;
    for(DayCnt =0 ;DayCnt<PATCH_MAX_DAY;DayCnt++)
    {
        sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,TaskId,DayCnt,pipe);
        if(access(pfile,R_OK)==0)
        {
            ret=remove(pfile);
            TASK_FMT_DEBUG(pipe,REC_LOG, "删除任务补抄文件%s ret is %d\n",pfile,ret);
        }
    }
    //taskmng_rs485_patch_file_delete(pipe,TaskId);
}


/*******************************************************************************
* 函数名称: taskmng_run_task_get
* 函数功能: 根据任务id获取线程内任务运行参数
* 输入参数: Pipe 端口号
            TaskId  任务id
* 输出参数: RunTask 任务对应的运行时参数
* 返 回 值:
*******************************************************************************/
void taskmng_run_task_get(uint8 Pipe,uint8 TaskId,uint8 logicid,CTS_RUN_TASK_T **RunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8 TaskNum;
    TaskNum = pAmrThreadArg->TaskNum;
    uint8 i;
    for(i=0;i<TaskNum;i++)
    {
        if(pAmrThreadArg->pTaskRunInfo[i].TaskId == TaskId &&pAmrThreadArg->pTaskRunInfo[i].logicid ==logicid )
        {
            *RunTask = &pAmrThreadArg->pTaskRunInfo[i];
            return;
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_router_state_frame_unpack
* 函数功能: 解析查询路由状态回码
* 输入参数:buf 报文
    len 报文长度
* 输出参数:
* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
*******************************************************************************/
int taskmng_router_state_frame_unpack(uint8 *buf,uint16 len)
{
    int ret=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
    uint8 routestate;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x10&&appdata.Fn == 0x04)
        {
            routestate = appdata.DataUnitBuff[0]&0x01;
            if(routestate == 0)
            {
                return -1;
            }
        }
    }else
    {
        return -1;
    }
    return 0;
}

int taskmng_router_version_query_frame_unpack(uint8 *buf,uint16 len)
{
    int ret=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
    uint8 mincollect;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x03&&appdata.Fn == 0x0A)
        {
            mincollect = appdata.DataUnitBuff[3]&0x10;
            if(mincollect == 0x10)
            {
                return 0;
            }
        }
    }else
    {
        return -1;
    }
    return 1;
}

int taskmng_node_minute_collect_query_frame_unpack(uint8 *buf,uint16 len)
{
    int ret=0;
    uint8 i=0;
    uint8 count=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
//    uint8 mincollect;
    uint16 offset=0;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x10&&appdata.Fn == 0x07)
        {
            offset =offset+2;
            count = appdata.DataUnitBuff[offset];
            for(i=0;i<count;i++)
            {
                offset = offset+6;

            }
        }
    }else
    {
        return -1;
    }
    return 1;
}

uint8 GetCollectorInfo()
{
//    uint16 i=0;
//    uint8  j=0;
//
//    metercount =0;
//    for(i=0;i<MeterFileAll.MeterCount;i++)
//    {
//        if((MeterFileAll.MeterFile[i].extend.add.len ==0x05))
//        {
//            //TASK_FMT_TRACE( 0, REC_LOG,"Sendframe msgindex %d \n",Collector.count);
//
//            for(j =0;j<Collector.count;j++)
//            {
//                //TASK_FMT_TRACE( 0, REC_LOG,"采集器地址 %d,%d,%d,%d,%d,%d j is %d\n",Collector.collectaddr[j].addr[0],Collector.collectaddr[j].addr[1],Collector.collectaddr[j].addr[2],Collector.collectaddr[j].addr[3],Collector.collectaddr[j].addr[4],Collector.collectaddr[j].addr[5],j);
//                //TASK_FMT_TRACE( 0, REC_LOG,"电表地址 %d,%d,%d,%d,%d,%d \n",MeterFileAll.MeterFile[i].extend.add.add[0],MeterFileAll.MeterFile[i].extend.add.add[1],MeterFileAll.MeterFile[i].extend.add.add[2],MeterFileAll.MeterFile[i].extend.add.add[3],MeterFileAll.MeterFile[i].extend.add.add[4],MeterFileAll.MeterFile[i].extend.add.add[5]);
//                if(taskmng_addr_isvalid_check(MeterFileAll.MeterFile[i].extend.add.add)<0)
//                {
//                    continue;
//                }
//                if(memcmp(Collector.collectaddr[j].addr,MeterFileAll.MeterFile[i].extend.add.add,6)==0)
//                {
//                    break;
//                }
//            }
//
//            if(j>=Collector.count)
//            {
//                Collector.count++;
//                memcpy(Collector.collectaddr[j].addr,MeterFileAll.MeterFile[i].extend.add.add,6);
//            }
//        }
//    }
    return Collector.count;
}

#ifdef AREA_JIANGSU

/*******************************************************************************
* 函数名称: taskmng_paral_amr_frame_make
* 函数功能: 并行抄表组帧
* 输入参数:
* 输出参数:
* 返 回 值: 1                  全部完成
*           0                  帧未抄读完成，切换到发送
*******************************************************************************/
int taskmng_paral_cjq_frame_make(uint8 Pipe)
{
    uint8 i;
    uint16 Pn;
    uint8 MtrAddr[6];
    uint8 index;
    int16       FrameLen = 0;
    uint8        FrameBuff[500]={0};  //帧数据缓冲区
    uint8        PayLoad[500];
    MESSAGE_INFO_T    *MsgSend;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[Pipe];
    //uint8   j;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    int result = 0;
    int          MsIndex;    //当前线程中的索引
    uint8 prtl;

    for(i=0;i<5;i++)
    {
        FrameLen = 0;
        MsIndex =pAmrThreadArg->TmnAmrInfo.collectorIndex;
        TASK_FMT_TRACE( Pipe, REC_LOG,"pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag is %d,i is %d\n",pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag,i);
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            FrameLen=NamedCollector(Pipe,FrameBuff,&Pn,MtrAddr);
            if(FrameLen>0)
            {
                taskmng_caj_addr_prtl_get( Pn,MtrAddr, &prtl);
                TASK_FMT_TRACE( Pipe, REC_LOG,"Pn is %d,prtl is %d\n",Pn,prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//当前电表最近一次发送时间
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
           }
        }
        else
        {
            if((time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>60)
            {
                memset(&pAmrThreadArg->AmrSendFrameInfo.Queue[i],0x00,sizeof(pAmrThreadArg->AmrSendFrameInfo.Queue[i]));
                pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
                TASK_FMT_TRACE( Pipe, REC_LOG,"pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt is %d\n",pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt);
            }
            continue;
        }
        if(FrameLen>0)
        {
            memset(PayLoad,0x00,sizeof(PayLoad));
            index = 0;

            memcpy_r(&PayLoad[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            memcpy_r(&PayLoad[index],(uint8*)&Pn,2);
            index+=2;
            index++;
            memcpy_r(&PayLoad[index],MtrAddr,6);
            index+=6;
            if(FrameLen<0x80)
            {
                PayLoad[index++] = FrameLen;
            }
            else
            {
                PayLoad[index++] = 0x82;
                PayLoad[index++] = (uint8)(FrameLen>>8);
                PayLoad[index++] = (uint8)FrameLen;
            }
            memcpy(&PayLoad[index],FrameBuff,FrameLen);
            index+=FrameLen;          //主动抄表PayLoad长度

            MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
            memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
            MsgSend->rpm = 1;
            MsgSend->index = GetMsgSeq();
            MsgSend->label = Pipe;
            MsgSend->priority = 1;
            MsgSend->IID = 0x0003;
            MsgSend->IOP = 0x003E;
            MsgSend->msglen = index;
            MsgSend->sourlen = strlen(DownName);
            memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
            MsgSend->destlen = strlen(CCOName);
            memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
            memcpy(MsgSend->playload,PayLoad,index);

            TASK_FMT_TRACE( Pipe, REC_LOG,"Sendframe msgindex %d \n",MsgSend->index);

            TASK_BUF_TRACE( Pipe, REC_LOG,FrameBuff, FrameLen);

            taskmng_plc_queue_write(pAmrSendDown[Pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
            free(MsgSend);
        }
        else
        {
            return FrameLen;
        }
    }

    return result;
}

int16 NamedCollector(uint8 pipe,uint8 *FrameBuff,uint16 *Pn,uint8 *MtrAddr)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[pipe];
    OOP_TSA_T sa;
    OOP_OAD_U subOAD;
    uint16 FrameLen;
    uint8 buf[10];
    uint8 cjqindex=0;
    uint8 addr[6];

    if(pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt >= Collector.count)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"抄读完毕\n");
        return -2;
    }
    if(pAmrThreadArg->TmnAmrInfo.collectorIndex >= Collector.count)
    {
        //pAmrThreadArg->TmnAmrInfo.CurMtrIndex =0;
        finishstate =1;
        TASK_FMT_TRACE( pipe, REC_LOG,"发送完毕\n");
        return -3;
    }
    cjqindex = pAmrThreadArg->TmnAmrInfo.collectorIndex;
    memcpy_r(addr,Collector.collectaddr[cjqindex].addr,6);
    *Pn= taskmng_pn_get_from_caj_addr(addr);
    //如果该表已发送则跳过
    if(taskmng_caj_in_queue_is_exist(pipe,*Pn)>0)
    {
        pAmrThreadArg->TmnAmrInfo.collectorIndex++;
        TASK_FMT_TRACE( pipe, REC_LOG,"发送队列已经存在该采集器\n");
        return -1;
    }

    subOAD.value =0x60437F00;
    sa.af=0;
    sa.len =0x05;
    sa.oldvxd = 0x01;
    memcpy(MtrAddr,Collector.collectaddr[cjqindex].addr,6);
    memcpy_r(sa.add,Collector.collectaddr[cjqindex].addr,6);
    memcpy_r(buf,(uint8*)&subOAD,4);
    buf[4] =0x11;
    buf[5] =0x01;
    OopAction.requst(pipe,&sa,1,buf, 6);
    LinkLayer_t* pLinkLayer = FindLinkLayer(pipe,NULL);
    FrameLen = pLinkLayer->sendLen;
    memcpy(FrameBuff,pLinkLayer->sendBuff,FrameLen);
    TASK_BUF_DEBUG( pipe, RELAY_LOG,FrameBuff, FrameLen);

    return FrameLen;
}

#endif
/*******************************************************************************
* 函数名称: taskmng_router_state_check
* 函数功能: 检查路由完成标志
* 输入参数:  pipe 端口
* 输出参数:
* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
*******************************************************************************/
int taskmng_router_version_query(uint8 pipe)
{

    //static uint32 lastsendtime ;
    static uint16 sendcnt;
    //static uint8 routestate;
    int ret;
    uint8 payload[200]={0};
    uint16 index = 0;
    uint16 overtime = 10;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    uint16 recvlen;
    uint8 recvbuf[500]={0};
    uint8 frame[] = {0X68,0X0F,0X00,0X43,0X00,0X00,0X00,0X00,0X00,0X0E,0X03,0X02,0X01,0X57,0X16};
    if((sendcnt>2))
    {
        TASK_FMT_TRACE(pipe,COMM_LOG, "连续查询 %d 次 不再查询  \n",sendcnt );
        sendcnt = 0;
        return 0;
    }
    //过秒时判断下 如果路由学习未完成一分钟发一次
    //if(gPastTimeFlag[pipe].BitSect.Second == TRUE)
    {
        //if(time(NULL)-lastsendtime > 60)
        {
            sendcnt++;
            TASK_FMT_TRACE(pipe,COMM_LOG, "第%d次查询路由版本 \n",sendcnt );
            //lastsendtime = time(NULL);
            //查询路由状态
            memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
            index+=4;
            payload[index++] = 0;
            memcpy_r(&payload[index],(uint8*)&overtime,2);
            index+=2;
            memcpy_r(&payload[index],(uint8*)&overtime,2);
            index+=2;
            payload[index++] = 15;
            memcpy(&payload[index],frame,15);
            index+=15;
            ret = taskmng_msg_fun_deal(pipe,0x0003,0x0033,index,payload,&recvlen,recvbuf);

            if(ret <0)
            {
                //如果是超时未回复 则认为是测试环境
                sendcnt=0;
                return 0;
            }else
            {
                //判断收到的回码中路由完成标志
                ret = taskmng_router_version_query_frame_unpack(recvbuf,recvlen);
                if(ret ==0)
                {
                    sendcnt=0;
                    routecollectstate =1;
                    TASK_FMT_TRACE(pipe,COMM_LOG, "CCO支持分钟冻结 \n" );
                    return 0;
                }else
                {
                    routecollectstate =0;
                    TASK_FMT_TRACE(pipe,COMM_LOG, "CCO不支持分钟冻结 \n" );
                }
            }
        }
    }


    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_router_state_check
* 函数功能: 检查路由完成标志
* 输入参数:  pipe 端口
* 输出参数:
* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
*******************************************************************************/
int taskmng_router_state_check(uint8 pipe)
{
    if(gHPLCRouterState[pipe]==1)
        return 0;
    static uint32 lastsendtime ;
    static uint16 sendcnt;
    int ret;
    uint8 payload[200]={0};
    uint16 index = 0;
    uint16 overtime = 10;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    uint16 recvlen;
    uint8 recvbuf[500]={0};
    uint8 frame[] = {0X68,0X0F,0X00,0X43,0X00,0X00,0X00,0X00,0X00,0X0E,0X10,0X08,0X00,0X69,0X16};
    if(sendcnt>15)
    {
        TASK_FMT_TRACE(pipe,COMM_LOG, "连续查询 %d 次 不再查询  \n",sendcnt );
        gHPLCRouterState[pipe]=1;
        sendcnt = 0;
        return 0;
    }
    //过秒时判断下 如果路由学习未完成一分钟发一次
    //if(gPastTimeFlag[pipe].BitSect.Second == TRUE)
    {
        if(time(NULL)-lastsendtime > 60)
        {
            sendcnt++;
            TASK_FMT_TRACE(pipe,COMM_LOG, "第%d次查询路由学习状态 \n",sendcnt );
            lastsendtime = time(NULL);
            //查询路由状态
            memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
            index+=4;
            index+=2;
            memcpy_r(&payload[index],(uint8*)&overtime,2);
            index+=2;
            memcpy_r(&payload[index],(uint8*)&overtime,2);
            index+=2;
            payload[index++] = 15;
            memcpy(&payload[index],frame,15);
            index+=15;
            ret = taskmng_msg_fun_deal(pipe,0x0003,0x0033,index,payload,&recvlen,recvbuf);

            if(ret <0)
            {
                //如果是超时未回复 则认为是测试环境
                gHPLCRouterState[pipe]=1;
                sendcnt=0;
                return 0;
            }else
            {
                //判断收到的回码中路由完成标志
                ret = taskmng_router_state_frame_unpack(recvbuf,recvlen);
                if(ret ==0)
                {
                    gHPLCRouterState[pipe]=1;
                    sendcnt=0;
                    TASK_FMT_TRACE(pipe,COMM_LOG, "路由学习已完成 \n" );
                    return 0;
                }else
                {
                    TASK_FMT_TRACE(pipe,COMM_LOG, "当前正在路由学习 \n" );
                }
            }
        }
    }


    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_router_state_check
* 函数功能: 检查路由完成标志
* 输入参数:  pipe 端口
* 输出参数:
* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
*******************************************************************************/
int taskmng_search_meter_state_check(uint8 pipe)
{
    if(gHPLCSearchState[pipe]==1)
        return 0;
    static uint32 lastsendtime;
    static uint16 sendcnt=0;
    uint8 i=0;

    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    if(sendcnt>26)
    {
        TASK_FMT_TRACE(pipe,COMM_LOG, "连续查询 %d 次 不再查询  \n",sendcnt );
        gHPLCSearchState[pipe]=1;
        sendcnt = 0;
        return 0;
    }
    //过秒时判断下 如果路由学习未完成一分钟发一次
    if(time(NULL)-lastsendtime > 60)
    {
        sendcnt++;
        lastsendtime = time(NULL);
        if(sendcnt<3)
        {
            return -1;
        }
        if(pAmrThreadArg->IsNeedSearch ==0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表完成！！！\n");
            gHPLCSearchState[pipe]=1;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "采集器地址数量Collector.count is %d\n",Collector.count);
            for(i =0;i<Collector.count;i++)
            {
                taskmng_caj_info_add(pipe,&Collector.collectaddr[i],1);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "同步采集器地址给CCO！！！\n");
            }
            return 0;
        }
//        for(i =0;i<Collector.count;i++)
//        {
//            taskmng_caj_info_add(pipe,&Collector.collectaddr[i],1);
//            TASK_FMT_DEBUG( pipe, RELAY_LOG, "同步采集器地址给CCO！！！\n");
//        }
    }

    return -1;
}
//
///*******************************************************************************
//* 函数名称: taskmng_router_state_check
//* 函数功能: 检查路由完成标志
//* 输入参数:  pipe 端口
//* 输出参数:
//* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
//*******************************************************************************/
//int taskmng_node_minute_collect_check(uint8 pipe)
//{
//
//    static uint32 lastsendtime ;
//    static uint16 sendcnt;
//    int ret;
//    uint8 payload[200]={0};
//    uint16 index = 0;
//    uint16 overtime = 10;
//    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
//    uint16 recvlen;
//    uint8 recvbuf[500]={0};
//    uint8 datalen = 12;
//    uint8 frame[] = {0X68,0X0F,0X00,0X43,0X00,0X00,0X00,0X00,0X00,0X0E,0X10,0X40};
//    if(sendcnt>15)
//    {
//        TASK_FMT_TRACE(pipe,COMM_LOG, "连续查询 %d 次 不再查询  \n",sendcnt );
//        gHPLCRouterState[pipe]=1;
//        sendcnt = 0;
//        return 0;
//    }
//    //过秒时判断下 如果路由学习未完成一分钟发一次
//    //if(gPastTimeFlag[pipe].BitSect.Second == TRUE)
//    {
//        if(time(NULL)-lastsendtime > 60)
//        {
//            sendcnt++;
//            TASK_FMT_TRACE(pipe,COMM_LOG, "第%d次查询路由学习状态 \n",sendcnt );
//            lastsendtime = time(NULL);
//            //查询路由状态
//            memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
//            index+=4;
//            payload[index++] = 0;
//            memcpy_r(&payload[index],(uint8*)&overtime,2);
//            index+=2;
//            memcpy_r(&payload[index],(uint8*)&overtime,2);
//            index+=2;
//            payload[index++] = 15;
//            memcpy(&payload[index],frame,15);
//            index+=15;
//            ret = taskmng_msg_fun_deal(pipe,0x0003,0x0033,index,payload,&recvlen,recvbuf);
//
//            if(ret <0)
//            {
//                //如果是超时未回复 则认为是测试环境
//                gHPLCRouterState[pipe]=1;
//                sendcnt=0;
//                return 0;
//            }else
//            {
//                //判断收到的回码中路由完成标志
//                ret = taskmng_node_minute_collect_query_frame_unpack(recvbuf,recvlen);
//                if(ret ==0)
//                {
//                    gHPLCRouterState[pipe]=1;
//                    sendcnt=0;
//                    TASK_FMT_TRACE(pipe,COMM_LOG, "支持分钟冻结 \n" );
//                    return 0;
//                }else
//                {
//                    TASK_FMT_TRACE(pipe,COMM_LOG, "不支持分钟冻结 \n" );
//                }
//            }
//        }
//    }
//
//
//    return -1;
//}

/*******************************************************************************
* 函数名称: taskmng_router_state_check
* 函数功能: 检查路由完成标志
* 输入参数:  pipe 端口
* 输出参数:
* 返 回 值: 0 路由学习完成可以抄表 -1 路由未完成
*******************************************************************************/
int taskmng_node_minute_collect_check(uint8 pipe)
{
    uint16 tmppn=0;

    tmppn = taskmng_get_tsa_count();
    TASK_FMT_TRACE(pipe,COMM_LOG, "带采集器地址的电表nodecount is %d,tmppn is %d\n",nodecount,tmppn);
    if(nodecount == tmppn)
    {
        nodecount =0;
        if(nodecollect != 0xAA)
        {
            TASK_FMT_TRACE(pipe,COMM_LOG, "有从节点不支持分钟采集\n");
            nodecollectstate =0;
            return -1;
        }
        TASK_FMT_TRACE(pipe,COMM_LOG, "从节点支持分钟采集\n");
        nodecollectstate =1;
        return 0;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_task_runlist_check
* 函数功能: 检查是否有需要运行的抄表任务
* 输入参数: 端口
* 输出参数: TaskInfo  待运行的任务 如没有则为空
* 返 回 值: >0                  有任务
*           <0               抄表任务全部完成
*******************************************************************************/
int taskmng_task_runlist_check(uint8 Pipe,CTS_RUN_TASK_T **TaskInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    CTS_RUN_TASK_T *tmpTask = pAmrThreadArg->pTaskHead;


    while(1)
    {
        if(tmpTask!=NULL)
        {

            if(tmpTask->ThisTurnPeriodStat != AMR_IN_PERIOD ||tmpTask->RunResult == SCHEME_FINISHED||tmpTask->TaskState!=1||tmpTask->NeedMtrNum==0)
                tmpTask = tmpTask->next;
            else
            {
                *TaskInfo = tmpTask;
                return 1;

            }
        }
        else
            return -1;
    }
}



/*******************************************************************************
* 函数名称: taskmng_oop_meter_event_oad_get
* 函数功能: 获取事件采集方案面向对象表需要抄读的Oad
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
uint8 taskmng_oop_meter_event_oad_get(uint8 Pipe,uint16 Pn,DI_INFO_T* DiInfo, int MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask)
{
    uint16 i;
    //uint16               oad_id;
    //uint16               OadLen = 0;
    uint8                OadNum = 0;
    uint8                RoadIndex=0;
    OadInfoMap OadMap;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    TASK_FMT_TRACE(Pipe,REC_LOG,"taskmng_oop_meter_event_oad_get  ROADIndex %d  CSDIndex %d\n",pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex,pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex);

    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex>=TaskInfo->CsdTask.RoadNum)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex = 0xff;
        CurTask->SucNum++;
        TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 抄读完成  \n",Pn );

        return 0;
    }
    TASK_FMT_TRACE(Pipe,REC_LOG,"mainoad 0x%08x SubOadNum %d\n",TaskInfo->CsdTask.RoadTask[pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex].MainOad.value,
        TaskInfo->CsdTask.RoadTask[pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex].SubOadNum);

    if(taskmng_oad_info_get(TaskInfo->CsdTask.RoadTask[pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex].MainOad,&OadMap)==FALSE)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex++;
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=0;
    }
    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex>=TaskInfo->CsdTask.RoadTask[pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex].SubOadNum)
    {
        //一个事件抄完 存下数据
        TASK_FMT_TRACE( Pipe, REC_LOG, " 电表 %d 一个事件抄完 %08x 存储数据\n",Pn,TaskInfo->CsdTask.RoadTask[pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex].MainOad.value );
        taskmng_cache_buf_write_to_db(Pipe,Pn,LOGIC_ID_YC);
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex++;
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=0;
    }

    RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex>=TaskInfo->CsdTask.RoadNum)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex = 0xff;
        CurTask->SucNum++;
        TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 抄读完成  \n",Pn );

        return 0;
    }
    //TASK_FMT_TRACE( Pipe, REC_LOG, " 当前RoadIndex  %d CSDIndex %d  \n",RoadIndex,pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex );
    for(i=pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;i<TaskInfo->CsdTask.RoadTask[RoadIndex].SubOadNum;i++)
    {
        DiInfo->SubOad[OadNum] = TaskInfo->CsdTask.RoadTask[RoadIndex].OadListInfo[i];
        OadNum++;
    }
    DiInfo->MainOad = TaskInfo->CsdTask.RoadTask[RoadIndex].MainOad;
    TASK_FMT_TRACE( Pipe, REC_LOG, " 当前RoadIndex  %d CSDIndex %d SigFrameNum %d \n",RoadIndex,pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex,OadNum );

    pAmrThreadArg->pMtrAmrInfo[MsIndex].SigFrameNum = OadNum;
    //pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex+=OadNum;
    return OadNum;
}

/*******************************************************************************
* 函数名称: taskmng_oop_meter_oad_get
* 函数功能: 获取面向对象表需要抄读的Oad
* 输入参数:Pipe TaskInfo CurTask
* 输出参数:DiInfo
        MsIndex     当前线程中的电表索引
* 返 回 值: 0    当前表已抄读完成
*******************************************************************************/
uint8 taskmng_oop_meter_oad_get(uint8 Pipe,uint16 Pn,DI_INFO_T* DiInfo, int MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask)
{
    uint16 i;
    //uint16               oad_id;
    uint16               OadLen = 0;
    uint8                OadNum = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint8                MtrSecModEn = 0;
    uint8                MtrEsamSerialNo[8];
    OOP_TI_T ti;

    time_t               ThisTurnTime;
    DateTimeBcd_t        TempTimeBcd;
    OadInfoMap OadMap ;
    uint8 datalen;
    OOP_METER_BASIC_T MtrAddr;


    //判断安全模式是否启用
    if(taskmng_oop_meter_security_para_read(Pipe,Pn,MtrEsamSerialNo,&MtrSecModEn)<0)
    {
        DiInfo->SubOad[0].nObjID = 0x4002;
        DiInfo->SubOad[0].attID = 2;
        DiInfo->SubOad[0].nIndex=0;
        DiInfo->SubOad[1].nObjID = 0xF101;
        DiInfo->SubOad[1].attID = 2;
        DiInfo->SubOad[1].nIndex=0;

        pAmrThreadArg->pMtrAmrInfo[MsIndex].IsNeedGetSafeMode = TRUE;
        return 2;
    }
    taskmng_mtr_basic_info_get(Pn,0,&MtrAddr);

   //事件采集方案
    if(TaskInfo->SchemeType ==EVENTPLAN)
        return taskmng_oop_meter_event_oad_get(Pipe, Pn, DiInfo, MsIndex, TaskInfo, CurTask);

#ifdef AREA_SHANDONG
    //抄读非介入式负荷辨识模组的数据48000300,48000400，抄表时需要带逻辑地址抄读，抄表前需要抄读40310200，查看是否有非介入式负荷辨识模组
    //（其中模组描述符：“type=NILM”，逻辑地址：3，模组类别：000AH。）；有此模组，用逻辑地址3去抄模组的数据（电表收到逻辑地址3，会透传到给模组）
    if(TRUE == TaskInfo->NILMFlag)
    {

        if(pAmrThreadArg->pMtrAmrInfo[MsIndex].NILMModuleInfo == 0)
        {
            DiInfo->SubOad[0].value = 0x40310200;
            TASK_FMT_TRACE( Pipe, REC_LOG, "电表%d 40310200  \n",Pn );
            return 1;
        }
    }
#endif

    //TASK_FMT_TRACE(Pipe,REC_LOG,"taskmng_oop_meter_oad_get CSDIndex %d SubOadNum %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex,TaskInfo->CsdTask.RoadTask[0].SubOadNum);
    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex >=TaskInfo->CsdTask.RoadTask[0].SubOadNum)
    {
        if((TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003 ||TaskInfo->AcqInfo.Acqtype == ACQTYP_BC) &&(TaskInfo->AcqInfo.Acqtype != ACQTYP_NULL))
        {
            ti.unit = TaskInfo->AcqInfo.TI.unit;
            ti.value = TaskInfo->AcqInfo.TI.value;
            DT_TimeAddInv(&pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, ti);

            //先将下一抄表周期开始时间换算成秒，并减去任务延时
            memcpy_r((uint8*)&TempTimeBcd,(uint8*)CurTask->ThisTurnBeginTime,7);
            DT_DateTimeBCD2Time(&TempTimeBcd, &ThisTurnTime);
            ti.unit = TaskInfo->DelayTimeUnit;
            ti.value = TaskInfo->DelayTime;
            DT_TimeSubInv(&ThisTurnTime, ti);
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime>=ThisTurnTime)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xff;
                CurTask->SucNum++;
                TASK_FMT_TRACE( Pipe, REC_LOG, "电表%d 抄读完成  \n",Pn );

                return 0;
            }else
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "电表%d 存储部分数据\n",Pn );
                taskmng_cache_buf_write_to_db(Pipe,Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            }
        }
        else
        {
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xff;
            CurTask->SucNum++;
            TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 抄读完成  \n",Pn );

            return 0;
        }
    }

    for(i=pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;i<TaskInfo->CsdTask.RoadTask[0].SubOadNum;i++)
    {
        //TASK_FMT_TRACE(Pipe,REC_LOG,"taskmng_oop_meter_oad_get i %d oad %08x len %d \n",i,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value,OadMap.dataLen);
        if(taskmng_oad_info_get(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i],&OadMap)==FALSE)
        {
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;
            continue;
        }
#ifdef AREA_SHANDONG
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nObjID == 0x4800)
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].NILMModuleInfo!=1)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "电表%d 不支持抄读 %08x  \n",Pn,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value );
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;
                continue;
            }
        }
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nObjID == 0x2046 ||TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nObjID == 0x2047)
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid_5_info!=1)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "电表%d 不支持抄读 %08x  \n",Pn,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value );
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;
                continue;
            }

        }
#endif
        //TASK_FMT_TRACE(Pipe,REC_LOG,"taskmng_oop_meter_oad_get i %d oad %08x len %d \n",i,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value,OadMap.dataLen);
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nIndex==0)
            datalen = OadMap.dataLen;
        else
            datalen = OadMap.subLen;
        if(OadLen+datalen >MAX_OAD_RD_LEN&&OadLen!=0 )
            break;
        //原来一次最多18个 出现误码的情况 先改成12个试下
        /*全量数据采集的6号日冻结补抄任务    拆成两帧抄了看下*/
/*         if(TaskInfo->AcqInfo.Acqtype == ACQTYP_BC && TaskInfo->AcqInfo.LastN == 62)
        {
            if(OadNum>=11)
            break;
        }else
        { */
            if(OadNum>=20)
                break;
        //}

        OadLen+=datalen;
        DiInfo->SubOad[OadNum] = TaskInfo->CsdTask.RoadTask[0].OadListInfo[i];
        OadNum++;
#ifdef AREA_SHANDONG
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x48000300||TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x48000400)
        {
            if(OadNum == 2 && TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x48000400
                                &&DiInfo->SubOad[0].value ==0x20210200)
            {
                break;
            }
            if(OadNum >= 2)
               OadNum -= 1;
            break;
        }
        if(TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nObjID == 0x2046||TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].nObjID == 0x2047)
        {
            if(OadNum == 2 && DiInfo->SubOad[0].value ==0x20210200)
            {
                break;
            }
            if(OadNum >= 2)
               OadNum -= 1;
            break;
        }
#endif
        #ifdef AREA_JIANGSU
        if(MtrAddr.userType == 127)
        {
            break;
        }
        #endif
        if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
        {
            #ifdef AREA_HUNAN
            //抄负荷识别数据需要用逻辑地址3 其它不需要 所以这个oad需要单独抄
            if((TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x20010400 && taskmng_usertype_get_from_no(Pn, LOGIC_ID_YC)!=LOT_METER_SPRDT)
                    ||TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x48000400)
            {
                if(OadNum == 2 && TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x48000400
                                &&DiInfo->SubOad[0].value ==0x20210200)
                {
                    break;
                }
                if(OadNum >= 2)
                    OadNum -= 1;
                break;
            }
            #endif
            #ifdef AREA_SHANDONG
            //山东除了电能量类的其他曲线都用实时转 所以要分开抄
            if(OadNum > 1)
            {
                if(taskmng_energy_oad_check(DiInfo->SubOad[OadNum-1])!=taskmng_energy_oad_check(DiInfo->SubOad[0]))
                {
                    OadNum -=1;
                    //如果单独只有一个冻结时间就不去抄
                    if(OadNum == 1 && DiInfo->SubOad[0].value == 0x20210200)
                    {
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;
                        DiInfo->SubOad[0].value = DiInfo->SubOad[1].value;
                    }
                    break;
                }
            }
            #endif
            if(OadNum>=9)
                break;
        }
    }
    //如果到这是0则表示配的oad可能在表里没找到 也算成功 要不然会一直刷
    if(OadNum ==0 )
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xff;
        CurTask->SucNum++;
        TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 配的oad暂不支持 抄读完成  \n",Pn );

    }
    DiInfo->MainOad = TaskInfo->CsdTask.RoadTask[0].MainOad;
    pAmrThreadArg->pMtrAmrInfo[MsIndex].SigFrameNum = OadNum;
    //pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex+=OadNum;//不在接收中进行判断
    return OadNum;
}

/*******************************************************************************
* 函数名称: taskmng_645_event_DI_search
* 函数功能: 获取事件方案需要抄读的数据项
* 输入参数: Pipe 端口号
            MsIndex  电表索引
            prtl 电表规约类型
            TaskInfo 任务参数
            CurTask 任务运行时参数
* 输出参数: CurTask 645表对应的数据标识
* 返 回 值: 1 获取到数据标识
        0 该表所有项都抄完
*******************************************************************************/
int taskmng_645_event_DI_search(uint8 Pipe,uint16 MsIndex,uint8 prtl,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask, DI_INFO_T* DI)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint16                   CSDIndex;
   // uint16                   RoadIndex;
    uint16  i,j,MapIndex;
    const EventMapOopTo645_t *pEventMapList;
    const MapOopTo645_NEW *pMap;
    if(prtl == PRTL_97)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex=0xff;
        CurTask->SucNum++;

        return 0;
    }
    else if(prtl == PRTL_07)
    {
        TASK_FMT_TRACE(Pipe, REC_LOG,"645事件 ROADIndex  %d RoadNum %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex,TaskInfo->CsdTask.RoadNum);

        for(i=pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;i<TaskInfo->CsdTask.RoadNum;i++)
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"mainoad 0x%08x \n",TaskInfo->CsdTask.RoadTask[i].MainOad);
            pEventMapList = taskmng_event_map_get(TaskInfo->CsdTask.RoadTask[i].MainOad);
            if(pEventMapList==NULL)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=0;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex=0;
                continue;
            }
            pMap = pEventMapList->EventMap;
            TASK_FMT_TRACE(Pipe, REC_LOG,"CSDIndex %d  SubOadNum %d\n",pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex,TaskInfo->CsdTask.RoadTask[i].SubOadNum);
            for(j=pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;j<TaskInfo->CsdTask.RoadTask[i].SubOadNum;j++)
            {
                MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
                TASK_FMT_TRACE(Pipe, REC_LOG,"mapindex %d \n",MapIndex);
                while(pMap[MapIndex].Oad.value!=0xffffffff)
                {
                    if(TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value == 0x20000200
                        ||TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value == 0x20010200)
                    {
                        if((TaskInfo->CsdTask.RoadTask[i].MainOad.attID==7&&(pMap[MapIndex].Oad.value ==TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value+1))
                            ||(TaskInfo->CsdTask.RoadTask[i].MainOad.attID==8&&(pMap[MapIndex].Oad.value ==TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value+2))
                            ||(TaskInfo->CsdTask.RoadTask[i].MainOad.attID==9&&(pMap[MapIndex].Oad.value==TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value+3)))
                        {
                            if(pMap[MapIndex].DI07 == 0xffffffff)
                                continue;
                            //memcpy(DI,&pMap[MapIndex].DI07,4);
                            DI->SubOadNum = 1;
                            DI->SubOad[0] = TaskInfo->CsdTask.RoadTask[i].OadListInfo[j];
                            DI->diinfo[0].dicount = 1;
                            DI->diinfo[0].diinfo[0] = pMap[MapIndex].DI07;
                            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = MapIndex;
                            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=j;
                            pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex = i;
                            return 1;
                        }
                    }

                    if(pMap[MapIndex].Oad.value==TaskInfo->CsdTask.RoadTask[i].OadListInfo[j].value)
                    {
                        if(pMap[MapIndex].DI07 == 0xffffffff)
                            continue;
                        //memcpy(DI,&pMap[MapIndex].DI07,4);
                        DI->SubOadNum = 1;
                        DI->SubOad[0] = TaskInfo->CsdTask.RoadTask[i].OadListInfo[j];
                        DI->diinfo[0].dicount = 1;
                        DI->diinfo[0].diinfo[0] = pMap[MapIndex].DI07;
                        TASK_FMT_TRACE(Pipe, REC_LOG,"找到对应DI %08x \n",pMap[MapIndex].DI07);
                        TASK_FMT_TRACE(Pipe, REC_LOG,"MapIndex %d csdindex %d roadindex %d \n",MapIndex,j,i);
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = MapIndex;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=j;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex = i;
                        return 1;
                    }
                    MapIndex++;
                }
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex=0;
            }
            if(TaskInfo->rptFlag == TRUE)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, " 一个事件抄完 该方案需要上报  \n" );
                time_t StartTime;
                DateTimeBcd_t  tmpTime;
                memcpy_r((uint8*)&tmpTime,(uint8*)CurTask->ThisTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTime,(time_t*)&StartTime);
                taskmng_645_event_data_save(Pipe, pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,TaskInfo->logicid,TaskInfo->TaskId,&TaskInfo->CsdTask.RoadTask[i] , StartTime);
            }
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex=0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex=0;
        }
        pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex=0xff;
        CurTask->SucNum++;
        TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 抄读完成  \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn );
        return 0;
    }
    return 0;
}



/*******************************************************************************
* 函数名称: taskmng_645_next_di_get
* 函数功能: 在转换列表里找到与要转换的字典索引相同的信息。若有多条信息使用同一字典索引
*           会把每一条都找到并返回
* 输入参数: pipe        端口号
*           MtrIndex            电表索引
*           suboad            子oad
*           pMap               对应的645字典
* 输出参数: di            找到的对应的645di，可能有多个
*
* 返 回 值: =0                  这个oad没找到对应的di
            >0               找到的di个数
*******************************************************************************/
int taskmng_645_di_get(uint8 pipe,uint16 MsIndex,uint8 prtl,OOP_OAD_U suboad,SCHEME_TASK_RUN_INFO *TaskInfo,const MapOopTo645_NEW* pMap,uint16 mapnum,DI_645_INFO_T *di)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    int dinum =0;
    uint32 ditmp=0;
    for(i=0;i<mapnum;i++)
    {
        if(pMap[i].Oad.value == suboad.value || taskmng_curve_DI_adapt_new(TaskInfo,suboad,pMap[i].Oad, pMap) >0 )
        {
            if((prtl == PRTL_07&&pMap[i].DI07==0xffffffff)||(prtl == PRTL_97&&pMap[i].DI97==0xffff))
                continue;
            if(i>1 && prtl == PRTL_07 && pMap[i].DI07==pMap[i-1].DI07)
                continue;
            if(prtl == PRTL_07)
            {
                ditmp = pMap[i].DI07;
                if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID!=0x0000&&TaskInfo->AcqInfo.Acqtype!=ACQTYP_NULL
                        &&pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum!=0xff)
                {
                    if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5000
                            ||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5003
                            ||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
                    {
                         //根据冻结搜索索引，修改07表数据标识
                        if((ditmp & 0xFF000000) == 0x05000000)     //只处理05XXXXXX格式的数据标识
                        {
                            ditmp = (ditmp&0xFFFFFF00)|pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum;
                        }
                    }
                }
                di->diinfo[dinum] = ditmp;
                di->mapindex[dinum] = i;
                dinum++;
            }else
            {
                di->diinfo[dinum] = pMap[i].DI97;
                di->mapindex[dinum] = i;
                dinum++;
            }
        }
    }
    di->dicount = dinum;
    return dinum;
}


/*******************************************************************************
* 函数名称: taskmng_645_DI_search
* 函数功能: 在转换列表里找到与要转换的字典索引相同的信息。若有多条信息使用同一字典索引
*           会把每一条都找到并返回
* 输入参数: Tmp_AmrLink         方案任务节点
*           MtrIndex            抄表索引
*           PrtlMode            电表规约
*           DI                  97/07数据标识
* 输出参数: MapIndex            搜索的字典索引在转换列表里的位置
*           DI                  找到的字典索引对应的97或07DI
* 返 回 值: =0                  该测点全部抄完
            >0                  在列表找到转换信息
*******************************************************************************/
int taskmng_645_DI_search(uint8 Pipe,uint16 MsIndex,uint8 prtl,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask, DI_INFO_T* DiInfo)
{
    const   MapOopTo645_NEW*  pMap;
    uint16      MapNum;
    int16      i,j;
    //uint32                   DI07 = 0;
    int dinum;
    uint8 oadnum = 0;
    DI_645_INFO_T ditmp;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    if(TaskInfo->AcqInfo.Acqtype == ACQTYP_BC)
        CurTask->PatchNextTurn = 0;
    if(TaskInfo->SchemeType == EVENTPLAN)
    {
        return taskmng_645_event_DI_search(Pipe,MsIndex,prtl,TaskInfo,CurTask,DiInfo);
    }
    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskmng_645_DI_search mainoad 0x%08x CurNum %d endnum %d \n",TaskInfo->CsdTask.RoadTask[0].MainOad.value,pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum,pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum);
    pMap = taskmng_map_get_from_mainoad(Pipe,MsIndex,TaskInfo,&MapNum);
    if(pMap == NULL)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xff;
        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
        CurTask->SucNum++;  //找不到默认成功

        return 0;
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"taskmng_645_DI_search PN %d  CSDIndex %d MapIndex %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex,pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex);
    //TASK_FMT_TRACE(Pipe, REC_LOG,"taskid %d SubOadNum %d MapNum %d \n",TaskInfo->TaskId,TaskInfo->CsdTask.RoadTask[0].SubOadNum,MapNum);
    //新流程 645多帧一起抄
    pAmrThreadArg->pMtrAmrInfo[MsIndex].SigFrameNum = 0;
    for(i = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;i<TaskInfo->CsdTask.RoadTask[0].SubOadNum;i++ )
    {
        memset(&ditmp,0x00,sizeof(ditmp));
        pAmrThreadArg->pMtrAmrInfo[MsIndex].SigFrameNum++; //用这个记录下一次抄了多个oad 这个数据和645的di个数不对应
        dinum = taskmng_645_di_get(Pipe,MsIndex,prtl,TaskInfo->CsdTask.RoadTask[0].OadListInfo[i],TaskInfo,pMap,MapNum,&ditmp);
        if(dinum>0)
        {
            //日冻结任务抄之前把成功标志置0 抄完后会置1
            if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].DayFrozenFlag = 0;
            }
            ditmp.csdindex = i;
            DiInfo->SubOad[oadnum] = TaskInfo->CsdTask.RoadTask[0].OadListInfo[i];
            DiInfo->diinfo[oadnum] = ditmp;
            oadnum++;
            DiInfo->SubOadNum = oadnum;
            for(j=0;j<ditmp.dicount;j++)
            {
                TASK_FMT_TRACE(Pipe, REC_LOG,"645找到对应DI %08x OAD 0X%08x\n",ditmp.diinfo[j],TaskInfo->CsdTask.RoadTask[0].OadListInfo[i].value);
            }
            //如果抄的是6001就直接一个块
            if(pMap == MapOopTo645_Min)
            {
                return 1;
            }
            //山东会存在电量用曲线 其他用实时转的情况 所以山东一次抄一个
#ifdef AREA_SHANDONG
            if(oadnum>=1)
#else
            //原来5个 采集压力测试实时任务14个数据项要发三帧   适配台体改成7
            if(oadnum>=MAX_DI_645_NUM)
#endif
            {
                return 1;
            }
        }
    }
    if(oadnum > 0)
    {
        return 1;
    }

    //加上补抄处理流程，不影响之前其他采集类型逻辑
    if(TaskInfo->AcqInfo.Acqtype == ACQTYP_BC)
    {
        time_t  StartTime;
        StartTime = pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime;
        if(TaskInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN)
            pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime += TaskInfo->AcqInfo.TI.value * 60;
        else if (TaskInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR)
                pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime += TaskInfo->AcqInfo.TI.value * 3600;
        else if (TaskInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
            pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime += TaskInfo->AcqInfo.TI.value * 3600 * 24;
        if(StartTime < CurTask->StartTime)
        {
            TASK_FMT_TRACE(0, REC_LOG, "startTime is %d, LastN is %u\n", pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, TaskInfo->AcqInfo.LastN);
            DateTimeBcd_t       tmpTime;
            DT_Time2DateTimeBCD(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, &tmpTime);
            TASK_FMT_TRACE(0, REC_LOG,"计算补抄开始时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            CurTask->PatchNextTurn = 1;
            return 1;
        }
    }
    //没有找到di就认为已经抄完
    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xff;
    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
    CurTask->SucNum++;
    TASK_FMT_TRACE( Pipe, REC_LOG, " 当前任务电表%d 抄读完成  \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn );
    if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0x5004)
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].DayFrozenFlag = 1;

    }
    if(CurTask->SucNum>=CurTask->NeedMtrNum)
        CurTask->RunResult = SCHEME_FINISHED;
    return 0;


}

/*******************************************************************************
* 函数名称: taskmng_next_broad_DI_get
* 函数功能: 获取当前可抄读测量点的数据标识集
* 输入参数:
* 输出参数:
* 返 回 值:
*           1                  存在需要抄读的数据
*           0                  全部抄完
*******************************************************************************/
int taskmng_next_broad_DI_get(uint8 Pipe,DI_INFO_T* DiInfo, int *MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask)
{

    uint16 Pn;
    int ret;
    uint8 prtl;
    uint8 cnt=0;
    uint8 logicid;
    uint8 failcnt = 0;
    //每次都从第一个表进行搜索
    CurTask->MeterIndex = 0;
    while(1)
    {

        if(CurTask->SucNum >= CurTask->NeedMtrNum)
        {
            CurTask->RunResult = SCHEME_FINISHED;
            return SEARCH_FINISH;
        }
        if(CurTask->MeterIndex>=TaskInfo->TotalNum)
        {
            cnt++;
            CurTask->MeterIndex = 0;
        }
        //找了两遍都没找到要抄的表 则退出
        if(cnt>=2)
            return SEARCH_FAIL;
        Pn = TaskInfo->MsInfo[CurTask->MeterIndex].Sn;
        logicid = TaskInfo->logicid;
        //TASK_FMT_TRACE( Pipe, REC_LOG, "TotalNum %d MeterIndex %d  PN %d \n",TaskInfo->TotalNum,CurTask->MeterIndex,Pn);
        //如果不属于当前线程则跳过
        if(TaskInfo->MsInfo[CurTask->MeterIndex].Pipe!=Pipe)
        {
            CurTask->MeterIndex++;
            continue;
        }

        //如果该表已发送则跳过
        if(taskmng_mtr_in_queue_is_exist(Pipe,Pn)>0)
        {
            CurTask->MeterIndex++;
            continue;
        }

        if(taskmng_mtr_result_check(CurTask->MeterIndex,CurTask->MtrResult)==TRUE)
        {
            CurTask->MeterIndex++;
            continue;
        }

        prtl = TaskInfo->MsInfo[CurTask->MeterIndex].Prtl;
        *MsIndex = taskmng_ms_index_get(Pipe,Pn,logicid);
        if(Pipe == 1 && gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].blePort ==0)
        {
            TASK_FMT_TRACE( Pipe, REC_LOG, "重新配置蓝牙端口 \n");
            bt_port_config(Pipe,*MsIndex);
        }
        TASK_FMT_TRACE( Pipe, REC_LOG, " getnextmeter PN %d prtl %d MSindex %d  SucNum %d \n",Pn,prtl,*MsIndex,CurTask->SucNum );
//#ifdef AREA_SHANDONG
        //TASK_FMT_TRACE( Pipe, REC_LOG, " getnextmeter PN %d prtl %d MSindex %d  roundFailCnt %u \n",Pn, prtl, *MsIndex, gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt);
        //如果该表当前轮次抄表失败次数大于最大次数 则本轮不再继续抄
        failcnt = GET_MAX_METER_FAIL_COUNT(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt);

        //转实时后的失败次数改为1次
        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645change ==1)
        {
            failcnt = 1;
        }
        //实时任务首轮的失败重试限制改为5次
        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt==0 &&TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID==0 &&TaskInfo->MinTaskFlag==0)
        {
            failcnt = 5;
        }
        //日冻结的首轮失败重试次数改为1 等重试
        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt==0 &&TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID== 0x5004 )
        {
            failcnt = 1;
        }

        //如果total_col_flg为1，代表在跑全量数据采集任务，此时失败重试次数改为1
        if(total_col_flg[Pipe] == 1)
            failcnt = 2;
        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].BroadFailCnt > 0)
        {
            taskmng_time_error_result_check(Pipe,Pn,CurTask);
        }
        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].BroadFailCnt >=failcnt )
        {
            TASK_FMT_TRACE( Pipe, REC_LOG, "ROUND_FAIL_CNT_LIMIT %d roundFailCnt %u \n",ROUND_FAIL_CNT_LIMIT, gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt);

//#else
//        //如果该表当前轮次抄表失败次数大于最大次数 则本轮不再继续抄
//        if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].BroadFailCnt >= MAX_METER_FAILURE_COUNT)
//        {
//#endif
            /* 轮次失败计数 重启的时候不算轮次失败*/
            if(CurTask->MinFrozenRebootCnt==0)
            {
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt++;
            }
            //现场遇到三相表抄0600不支持也不回码 该情况下判断下日冻结有没有抄通 如果通了就转实时
            //单相表也遇到了
            //if(prtl == PRTL_07 && taskmng_mtr_conntype_get(Pn,logicid)!=1&&TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002&&
#if CURRENT_TO_CURVE_ENABLE
            if(prtl == PRTL_07 && TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002&&CurTask->MinFrozenRebootCnt==0&&
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645 == 0 && gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].DayFrozenFlag == 1&&
                ((taskmng_mtr_conntype_get(Pn,logicid)!=1 && gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].CSDIndex == 0)||
                    taskmng_mtr_conntype_get(Pn,logicid)==1))
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "该表不支持负荷曲线，转为实时抄读！！ \n");
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645=1;
                if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645change == 0)
                {
                    gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645change = 1;    //标记从负荷曲线开始转实时
                }
                //gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].CSDIndex = 0;
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].MapIndex = 0;
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].BroadFailCnt = 0;
                continue;
            }
            //转实时还不通就切回原来的 等补抄
            if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645change == 1)
            {
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645=0;

            }
#endif
            TASK_FMT_TRACE( Pipe, REC_LOG, " 该表本轮失败次数 %d 不再抄读 \n",gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].BroadFailCnt);

            //现场遇到表抄电量回复正常，但是抄需量时回复的报文无法解析，会占用缓存空间，导致下面的表缓存不了
            //所以失败了也存一下
            taskmng_cache_buf_write_to_db(Pipe,Pn,logicid);
            taskmng_mtr_result_set(CurTask->MeterIndex, CurTask->MtrResult);
            gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].LastResult = FALSE;
            CurTask->MeterIndex++;
            CurTask->SucNum++;
            //统计日冻结任务的失败电表个数
            if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004||TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5005)
            {
                CurTask->DayFrozenFailCnt++;
                TASK_FMT_TRACE( Pipe, REC_LOG,"日冻结失败个数 %d \n", CurTask->DayFrozenFailCnt);
            }
            if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5006)
            {
                CurTask->MonFrozenFailCnt++;
                TASK_FMT_TRACE( Pipe, REC_LOG,"月冻结失败个数 %d \n", CurTask->MonFrozenFailCnt);
            }
            //统计分钟冻结失败电表个数
            if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
            {
                CurTask->MinFrozenFailCnt++;
                TASK_FMT_TRACE( Pipe, REC_LOG,"分钟冻结失败个数 %d \n", CurTask->MinFrozenFailCnt);
            }
            continue;
        }
        if(prtl == PRTL_OOP)
        {
            DiInfo->SubOadNum = taskmng_oop_meter_oad_get(Pipe,Pn,DiInfo, *MsIndex,TaskInfo,CurTask);

            if(DiInfo->SubOadNum >0)
            {
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].LastTaskId = CurTask->TaskId;
                return SEARCH_SUCCESS;
            }else
            {
                taskmng_task_monitor_update(Pipe,CurTask->TaskId,CurTask->logicid,UDTASK_SUCCESSNUM,NULL);

                taskmng_mtr_result_set(CurTask->MeterIndex, CurTask->MtrResult);

                //写文件很耗时 所以分钟级任务不写记录文件
                if(TaskInfo->MinTaskFlag == 0)
                {
                    taskmng_task_record_file_set(Pipe,*MsIndex,CurTask->TaskId,logicid);
                }
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].LastResult = TRUE;
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].AllFailCnt = 0;

                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt = 0;

                if(logicid==LOGIC_ID_YC && TaskInfo->MinTaskFlag == 0)
                    taskmng_patch_result_set(Pipe,Pn,CurTask,0);
                //事件采集方案抄完一个就会存 这儿不用再存了
                if(TaskInfo->SchemeType != EVENTPLAN)
                    taskmng_cache_buf_write_to_db(Pipe,Pn,logicid);
                if(CurTask->SucNum >= CurTask->NeedMtrNum)
                {
                    CurTask->RunResult = SCHEME_FINISHED;
                    return SEARCH_FINISH;
                }
                CurTask->MeterIndex++;//该表全部抄完

            }

        }
        else if(prtl == PRTL_188)
        {
            return SEARCH_SUCCESS;//188协议只有一个固定的数据项901F，所有不用获取，直接返回成功
        }
        else
        {
            gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].LastTaskId = CurTask->TaskId;
            ret = taskmng_645_DI_search(Pipe,*MsIndex,prtl,TaskInfo,CurTask,DiInfo);
            if(ret >0)
            {
                return SEARCH_SUCCESS;
            }else if(ret ==0)
            {
                taskmng_task_monitor_update(Pipe,CurTask->TaskId,CurTask->logicid,UDTASK_SUCCESSNUM,NULL);
                taskmng_mtr_result_set(CurTask->MeterIndex, CurTask->MtrResult);
                if(TaskInfo->MinTaskFlag == 0)
                {
                    taskmng_task_record_file_set(Pipe,*MsIndex,CurTask->TaskId,logicid);
                }
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].LastResult = TRUE;
                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].AllFailCnt = 0;

                gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].roundFailCnt = 0;
                //如果转实时抄完了 就不再切回去了
                if(gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645change == 1)
                {
                    gAmrThreadArg[Pipe].pMtrAmrInfo[*MsIndex].flag645=2;

                }
                if(logicid==LOGIC_ID_YC && TaskInfo->MinTaskFlag == 0)
                    taskmng_patch_result_set(Pipe,Pn,CurTask,0);
                if(TaskInfo->SchemeType != EVENTPLAN && (TaskInfo->AcqInfo.Acqtype == ACQTYP_NULL || TaskInfo->CsdTask.RoadTask[0].MainOad.value == 0x50040200))
                    taskmng_cache_buf_write_to_db(Pipe,Pn,logicid);
                if(CurTask->SucNum>=CurTask->NeedMtrNum)
                {
                    CurTask->RunResult = SCHEME_FINISHED;
                    return SEARCH_FINISH;
                }
                CurTask->MeterIndex++;//该表全部抄完
            }
        }
    }

}

/*******************************************************************************
* 函数名称: taskmng_paral_amr_frame_make
* 函数功能: 并行抄表组帧
* 输入参数:
* 输出参数:
* 返 回 值: 1                  全部完成
*           0                  帧未抄读完成，切换到发送
*******************************************************************************/
int taskmng_paral_amr_frame_make(uint8 Pipe,CTS_RUN_TASK_T *CurTask,SCHEME_TASK_RUN_INFO SchemeTaskInfo)
{
    uint8 i;
    int ret;
    uint16 Pn;
    uint8 prtl;
    uint8 MtrAddr[6];
    uint16 index;
    uint16       FrameLen = 0;
    uint8        FrameBuff[500]={0};  //帧数据缓冲区
    uint8        PayLoad[500];
    int          MsIndex;    //当前线程中的索引
    DI_INFO_T    TmpDiInfo;
    MESSAGE_INFO_T    *MsgSend;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint8   j;
    uint8 logicid;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    int result = 0;
    if(gParallelNum !=PARALLEL_QUEUE_NUM && gParallelNum != 5)
        gParallelNum = 5;
    //如果total_col_flg为1，代表在跑全量数据采集任务，此时并发数改为1
    if(total_col_flg[Pipe] == 1 || (SchemeTaskInfo.MinTaskFlag == 1 && CurTask->NeedMtrNum <= 20))
        gParallelNum = 1;
    //并发数改为按照档案个数自适应
    for(i=0;i<gParallelNum;i++)
    {
        FrameLen = 0;
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            memset(&TmpDiInfo,0x00,sizeof(DI_INFO_T));
            ret = taskmng_next_broad_DI_get(Pipe,&TmpDiInfo,&MsIndex,&SchemeTaskInfo,CurTask);
            //TASK_FMT_TRACE( Pipe, REC_LOG, "taskmng_next_broad_DI_get %d \n",ret);
            if(ret == SEARCH_SUCCESS)
            {
#ifdef AREA_HUNAN
                if(taskmng_HPLC_module_curve_flag_get()==1&&SchemeTaskInfo.CsdTask.RoadTask[0].MainOad.nObjID ==0x5002 &&
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].stacurvestate == 1)
                {
                    TmpDiInfo.stacurveflag = 1;
                }
#endif
                if(SchemeTaskInfo.AcqInfo.Acqtype == ACQTYP_BC && CurTask->PatchNextTurn == 1)
                    continue;
                Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
                logicid = CurTask->logicid;
                taskmng_mtr_addr_prtl_get( Pn, logicid,MtrAddr, &prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//当前电表最近一次发送时间
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                memcpy((void *)&pAmrThreadArg->AmrSendFrameInfo.Queue[i].DiInfo,(void*)&TmpDiInfo,sizeof(DI_INFO_T));
                //TASK_FMT_TRACE( Pipe, REC_LOG, "Pn %d MsIndex %d prtl %d CommBeginTime %ld \n",Pn,MsIndex,prtl,time(NULL));
#ifdef AREA_SHANDONG
                FrameLen = taskmng_meter_frame_make(Pipe, i, pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime - pAmrThreadArg->pMtrAmrInfo[MsIndex].timeOffset, pAmrThreadArg->AmrSendFrameInfo.Queue[i], FrameBuff, CurTask->TaskId, CurTask->logicid);
#else
                FrameLen = taskmng_meter_frame_make(Pipe,i,pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime,pAmrThreadArg->AmrSendFrameInfo.Queue[i],FrameBuff,CurTask->TaskId,CurTask->logicid);
#endif
                result = 0 ;
            }
            else if(ret == SEARCH_FINISH)
            {
                result = 1 ;
                continue;
                //return 1; //该任务完成
            }else     //没有要抄的表了
            {
                result = 0 ;
                continue;
                //return 0;
            }
        }
        else
        {
            //判断是否需要发送分帧确认帧
            if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg == 1)
            {
                LinkLayer_t* pLinkLayer = FindLinkLayer(oop_pipe_get(Pipe,i),NULL);
                if(NULL==pLinkLayer)
                {
                    pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg = 0;
                    continue;
                }
                FrameLen = pLinkLayer->sendLen;
                memcpy(FrameBuff,pLinkLayer->sendBuff,FrameLen);
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].NeedSendSeg = 0;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn;
                memcpy(MtrAddr,pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,6);
                TASK_FMT_TRACE( Pipe, REC_LOG,"Pn %d 发送分帧确认帧 \n",Pn);
                result = 0 ;
            }
        }
        if(FrameLen>0)
        {
            memset(PayLoad,0x00,sizeof(PayLoad));
            index = 0;

            memcpy_r(&PayLoad[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            index+=2;
            memcpy_r(&PayLoad[index],MtrAddr,6);
            index+=6;
            if(FrameLen<0x80)
            {
                PayLoad[index++] = FrameLen;
            }
            else
            {
                PayLoad[index++] = 0x82;
                PayLoad[index++] = (uint8)(FrameLen>>8);
                PayLoad[index++] = (uint8)FrameLen;
            }
            memcpy(&PayLoad[index],FrameBuff,FrameLen);
            index+=FrameLen;          //主动抄表PayLoad长度

#ifdef AREA_HUNAN
            if(TmpDiInfo.stacurveflag == 1)
            {
                memset(PayLoad,0x00,sizeof(PayLoad));
                index = taskmng_sta_curve_get_msg_pack(Pipe,PayLoad,MtrAddr,FrameBuff,FrameLen);
            }
#endif

            MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
            memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
            MsgSend->rpm = 1;
            MsgSend->index = GetMsgSeq();
            MsgSend->label = Pipe;
            MsgSend->priority = 1;
            MsgSend->IID = 0x0003;
            MsgSend->IOP = 0x003E;
#ifdef AREA_HUNAN
            if(TmpDiInfo.stacurveflag == 1)
            {
                MsgSend->IOP = 0x0070;
            }
#endif
            MsgSend->msglen = index;
            MsgSend->sourlen = strlen(DownName);
            memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
            MsgSend->destlen = strlen(CCOName);
            memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
            memcpy(MsgSend->playload,PayLoad,index);

            TASK_FMT_TRACE( Pipe, REC_LOG,"Sendframe msgindex %d \n",MsgSend->index);

            TASK_BUF_TRACE( Pipe, REC_LOG,FrameBuff, FrameLen);

            taskmng_task_monitor_update(Pipe,CurTask->TaskId,logicid,UDTASK_SENDFRMNUM,NULL);
            taskmng_plc_queue_write(pAmrSendDown[Pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
            free(MsgSend);
        }
    }

    return result;
}

/*******************************************************************************
* 函数名称:     taskmng_645_recv_frame_judge
* 函数功能:判断收到的645帧是否正确
* 输入参数:
* 输出参数:     > 0 需要下移
                < 0 不需要下移
* 返 回 值:
*******************************************************************************/
int taskmng_645_recv_frame_judge(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 *pRecvDI, char CtrlCode, uint16* DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const EventMapOopTo645_t *pEventMapList=NULL;
    const MapOopTo645_NEW *pMap = NULL;
    uint16                 MapNum = 0;
    uint16 Pn;
    uint8 prtl;
    uint16 RoadIndex;
    uint16                 MapIndex;
    uint32                 SendDI = 0;
    uint32                 RecvDI = 0;
    uint16                 TmpDI;

    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&prtl);
    if(pTmp_NormalLink->SchemeType==EVENTPLAN)
    {
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
        pEventMapList = taskmng_event_map_get(pTmp_NormalLink->CsdTask.RoadTask[RoadIndex].MainOad);
        if(pEventMapList ==NULL)
            return -1;
        pMap = pEventMapList->EventMap;
        if(pMap==NULL)
            return -1;
    }
    else
    {
        pMap = taskmng_map_get_from_mainoad(Pipe,MsIndex,pTmp_NormalLink,&MapNum);
        if(pMap==NULL)
            return -1;
    }
    MapIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex;
    if(prtl == PRTL_97)
    {
        if(pRecvDI!=NULL)
            memcpy(&RecvDI,pRecvDI,2);
        memcpy(&SendDI,&pMap[MapIndex].DI97,2);
        TmpDI = (pMap[MapIndex].DI97 & 0xfff0);
        if(TmpDI == 0xB010 || TmpDI == 0xB020 || TmpDI == 0xB110 || TmpDI == 0xB120)
        {
            if(*DataLen == 6)
            {
                pRecvDI[6] = gCurBcdTime[Pipe].Year; //97需量时间没有年，填充字节保持格式一致
                *DataLen = 5;
            }
        }
    }
    else
    {
        if(pRecvDI != NULL)
            memcpy(&RecvDI,pRecvDI,4);
        memcpy(&SendDI,&pMap[MapIndex].DI07,4);
        if((SendDI & 0xfff00000) == 0x05000000)    //冻结类发送时会修改DI
            SendDI = ((SendDI & 0xffffff00)|pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum);
    }
    TASK_FMT_TRACE(Pipe, REC_LOG, "RecvDI %08x SendDI %08x \n",RecvDI,SendDI);
     //验证发送的数据标识与接收到的数据标识是否一致
    if(pRecvDI != NULL)
    {
        //printf("RecvDI = %08lx  SendDI = %08lx \n",RecvDI,SendDI);
        if((RecvDI != SendDI) || ((CtrlCode & 0x80) == 0))
            return -1;
    }
    else
    {
        printf("收到否认帧 \n");
    }
    return 1;
}

/*******************************************************************************
* 函数名称:     taskmng_645_recv_frame_judge
* 函数功能:判断收到的645帧是否正确
* 输入参数:
* 输出参数:     > 0 需要下移
                < 0 不需要下移
* 返 回 值:
*******************************************************************************/
int taskmng_645_recv_frame_judge_new(uint8 Pipe,uint16 MsIndex,DI_INFO_T *diinfo, SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 *pRecvDI,uint16* DataLen,uint16* mapindex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];

    //uint16                 MapNum = 0;
    uint16 Pn;
    uint8 prtl;

    uint32                 RecvDI = 0;
    uint16                 TmpDI;
    uint8 i,j;
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&prtl);

    if(prtl == PRTL_97)
    {
        if(pRecvDI!=NULL)
            memcpy(&RecvDI,pRecvDI,2);
        for(i=0;i<diinfo->SubOadNum;i++)
        {
            for(j=0;j<diinfo->diinfo[i].dicount;j++)
            {
                if(RecvDI == diinfo->diinfo[i].diinfo[j])
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "RecvDI %08x Sendoad %08x \n",RecvDI,diinfo->SubOad[i].value);
                    TmpDI = (RecvDI & 0xfff0);
                    if(TmpDI == 0xB010 || TmpDI == 0xB020 || TmpDI == 0xB110 || TmpDI == 0xB120)
                    {
                        if(*DataLen == 6)
                        {
                            pRecvDI[6] = gCurBcdTime[Pipe].Year; //97需量时间没有年，填充字节保持格式一致
                            *DataLen = 5;
                        }
                    }
                    *mapindex = diinfo->diinfo[i].mapindex[j];
                    return i;
                }
            }
        }

    }
    else
    {
        if(pRecvDI != NULL)
            memcpy(&RecvDI,pRecvDI,4);
        for(i=0;i<diinfo->SubOadNum;i++)
        {
            for(j=0;j<diinfo->diinfo[i].dicount;j++)
            {
                if(RecvDI == diinfo->diinfo[i].diinfo[j])
                {
                    TASK_FMT_TRACE(Pipe, REC_LOG, "RecvDI %08x Sendoad %08x \n",RecvDI,diinfo->SubOad[i].value);
                    *mapindex = diinfo->diinfo[i].mapindex[j];
                    return i;
                }
            }
        }

    }

    return -1;
}
/*******************************************************************************
* 函数名称: taskmng_645_map_index_move
* 函数功能: 下移当前方案的CSD,统计成功数并打印、记录统计信息
* 输入参数: MtrIndex            测量点索引
*           Tmp_AmrLink         当前任务信息结构体
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_map_index_move(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex,BOOL flag)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    const MapOopTo645_NEW *pMap;
    const EventMapOopTo645_t *pEventMapList;
    uint16 RoadIndex;
    if(Tmp_AmrLink->SchemeType == EVENTPLAN)
    {
        if(flag==FALSE)
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"EVENTPLAN 当前未发生新事件 无需继续抄读！ \n");
            pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex++;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            return;
        }
        RoadIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
        pEventMapList = taskmng_event_map_get(Tmp_AmrLink->CsdTask.RoadTask[RoadIndex].MainOad);
        if(pEventMapList == NULL)
        {
            //容错处理,找不到对应抄读数据表那么直接置为成功
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex < 0xff)
                pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex += 1;
            //printx("未找到对应任务列表\n");
            return;
        }

        pMap = pEventMapList->EventMap;
        if(pMap == NULL)
        {
            //容错处理,找不到对应抄读数据表那么直接置为成功
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex < 0xff)
                pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex += 1;
            //printx("未找到对应07事件列表\n");
            return;
        }
        TASK_FMT_TRACE(Pipe, REC_LOG,"EVENTPLAN一帧抄完后 pn %d mapindex %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex);
        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex++;
    }
    else
    {
        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex++;
    }

}
/*******************************************************************************
* 函数名称: taskmng_645_frz_move_judge
* 函数功能: 1、若时标不在上一周期内，继续搜索下一条记录
*           2、若抄读时标回否认，则不再抄读该任务后续数据
* 输入参数: Tmp_AmrLink         方案任务节点
*           MtrIndex            抄表索引
*           Result              抄表结果 1:成功 0:失败
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_645_frz_move_judge(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RunTask ,int MsIndex, uint8 Result)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    DateTimeBcd_t       tmpTime;
    time_t              endTime;
    uint16              Pn;
    uint8               PrtlMode = 0;


    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&PrtlMode);


    if(PrtlMode != PRTL_07)
        return ;
    if(Tmp_AmrLink->SchemeType == EVENTPLAN)
    {
        if(Result == 0)    //抄读事件回复否认，不再抄读该事件剩余数据项
        {
            //pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
            pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex++;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
        }
        return;
    }
    if( Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003&& taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
        return;
    //以实时方式抄读不管
    //只有分钟冻结任务时修改标记 修复湖南现场发现的问题
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
#if CURRENT_TO_CURVE_ENABLE
       // OOP_OAD_U subOad;
        //uint8 CSDIndex;
       // CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex;
       // subOad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];

        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1)
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==1 ||(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0&& taskmng_task_period_check(Tmp_AmrLink)==TRUE))
            {
                return;
            }
            //如果单相表抄负荷曲线回否认则更换方式 0610回否认用0600试下
            //只有抄正向有功电量回否认时才切
            //山东有个任务只抄电压
            //if(Result==0 && 0x0010 == subOad.nObjID )
            if(Result==0 )
            {
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=2;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                    return;
                }

            }
            //用0600抄的时候第一个一般是冻结时间 不会抄到正向有功
            if(Result==0 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==2)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=1;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                return;
            }
        }else
        {

            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0)
            {
                return ;
            }
            //如果非单相表抄负荷曲线回否认则用实时抄
            if(Result==0 )
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=1;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                return;
            }
        }
#else
        //抄曲线回否认 则不再抄读后面的
        if(Result==0)
        {
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex= 0xfe;
            return;
        }
#endif
    }
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0xff)
        return;
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x0000)
    {
        if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM||Tmp_AmrLink->AcqInfo.Acqtype ==ACQTYP_TI)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x5002)
            {
                if(Result==0&&pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex==0)
                {
                    //若抄读时标回否认，则不再抄读该任务后续数据
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex= 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum = 1;      //不再继续搜索时标
                    return;
                }
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm.year_h==0)
                {
                    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum <= pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum)
                    {
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum++;//继续搜索下一条记录
                    }
                }
#ifndef AREA_HUBEI
                //湖北禁用日冻结数据实时转冻结
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0
                        &&pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum < 2)
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum = 0xff;
#endif
            }
            else
            {
                 //负荷曲线处理
                OOP_TI_T TI;
                TI.unit = Tmp_AmrLink->AcqInfo.TI.unit;
                TI.value = Tmp_AmrLink->AcqInfo.TI.value;

                //if(IsCnntStyleOne(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid, Tmp_AmrLink) > 0)
                //0610的负荷曲线 抄完所有数据项后再加时间
                if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;    //抄读下一个曲线
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                    TASK_FMT_TRACE( Pipe, REC_LOG, "负荷曲线 csdindex %d mapindex %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex,pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex);

                    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex < Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum)
                        return;
                    TI.unit = TASK_CYCLE_FLAG_MIN;
                    TI.value = 15*taskmng_curve_point_get( Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid);    //一次15min*8个点
                }

                DT_TimeAddInv(&pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, TI);
                //DT_Time2DateTimeBCD(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, &tmpTime);
                //printx("负荷曲线当前时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                 //      tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);

                memcpy_r((uint8*)&tmpTime,(uint8*)RunTask->ThisTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTime,&endTime);
                //减去任务开始时间里包含的延时
                TI.unit = Tmp_AmrLink->DelayTimeUnit;
                TI.value = Tmp_AmrLink->DelayTime;
                DT_TimeSubInv(&endTime, TI);
                //DT_Time2DateTimeBCD(endTime, &tmpTime);
                //printx("负荷曲线结束时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                //       tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);

                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime < endTime)
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                }
                else
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                }
            }


        }


    }
}

/*******************************************************************************
* 函数名称: taskmng_645_frz_move_judge
* 函数功能: 1、若时标不在上一周期内，继续搜索下一条记录
*           2、若抄读时标回否认，则不再抄读该任务后续数据
* 输入参数: Tmp_AmrLink         方案任务节点
*           MtrIndex            抄表索引
*           Result              抄表结果 1:成功 0:失败
* 输出参数: 无
* 返 回 值: -1 切换抄读方式直接转到发送 0正常
*******************************************************************************/
int taskmng_645_frz_move_judge_new(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RunTask ,int MsIndex, uint8 Result)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    DateTimeBcd_t       tmpTime;
    time_t              endTime;
    uint16              Pn;
    uint8               PrtlMode = 0;

   // uint8 CSDIndex;
    //OOP_OAD_U subOad;
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    taskmng_mtr_prtl_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid,&PrtlMode);
    //CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex;
    //subOad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
    if(PrtlMode != PRTL_07)
        return 0;
    if(Tmp_AmrLink->SchemeType == EVENTPLAN)
    {
        if(Result == 0)    //抄读事件回复否认，不再抄读该事件剩余数据项
        {
            //pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
            pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex++;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
        }
        return 0;
    }
    if( Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5003&& taskmng_task_period_get(Tmp_AmrLink->TaskId,Tmp_AmrLink->logicid) < 86400L)
        return 0;
    //以实时方式抄读不管
    //只有分钟冻结任务时修改标记 修复湖南现场发现的问题
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002)
    {
#ifdef AREA_SHANDONG
        uint8               CSDIndex = pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex;
        OOP_OAD_U suboad = Tmp_AmrLink->CsdTask.RoadTask[0].OadListInfo[CSDIndex];
        TASK_FMT_TRACE( Pipe, REC_LOG, "taskmng_645_frz_move_judge_new suboad %08x \n",suboad);
        if(taskmng_energy_oad_check(suboad) == FALSE)
        {
            return 0;
        }
#endif
        if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1)
        {
            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==1 ||(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0&& taskmng_task_period_check(Tmp_AmrLink)==TRUE))
            {
                return 0;
            }
            //如果单相表抄负荷曲线回否认则更换方式 0610回否认用0600试下
            //只有抄正向有功电量回否认时才切
            //if(Result==0 && 0x0010 == subOad.nObjID )
            if(Result==0)
            {
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=2;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                    return -1;
                }

            }
            //用0600抄的时候第一个一般是冻结时间 不会抄到正向有功
            if(Result==0 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==2)
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=1;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                return -1;
            }
        }else
        {

            if(pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645!=0)
            {
                return 0;
            }
            //如果非单相表抄负荷曲线回否认则用实时抄
            if(Result==0 )
            {
                pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645=1;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                return -1;
            }
        }
    }
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0xff)
        return 0;
    if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x0000)
    {
        if(Tmp_AmrLink->AcqInfo.Acqtype == ACQTYP_FRZTM||Tmp_AmrLink->AcqInfo.Acqtype ==ACQTYP_TI)
        {
            if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID!=0x5002)
            {
                if(Result==0&&pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex==0)
                {
                    //若抄读时标回否认，则不再抄读该任务后续数据
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex= 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum = 1;      //不再继续搜索时标
                    return -1;
                }
                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].storeTm.year_h==0)
                {
                    if(pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum <= pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum)
                    {
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum++;//继续搜索下一条记录
                    }
                }
                if(Tmp_AmrLink->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004 && pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum == 0
                        &&pAmrThreadArg->pMtrAmrInfo[MsIndex].CurNum < 2)
                {
                        TASK_FMT_TRACE( Pipe, REC_LOG, "日冻结时标不对 \n");
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                        pAmrThreadArg->pMtrAmrInfo[MsIndex].EndNum = 0xff;
                        return -1;
                }
            }
            else
            {
                 //负荷曲线处理
                OOP_TI_T TI;
                TI.unit = Tmp_AmrLink->AcqInfo.TI.unit;
                TI.value = Tmp_AmrLink->AcqInfo.TI.value;

                //if(IsCnntStyleOne(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid, Tmp_AmrLink) > 0)
                //0610的负荷曲线 抄完所有数据项后再加时间
                if(taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid)==1 && pAmrThreadArg->pMtrAmrInfo[MsIndex].flag645==0)
                {
                    //pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex++;    //抄读下一个曲线
                    //pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                    TASK_FMT_TRACE( Pipe, REC_LOG, "负荷曲线 csdindex %d mapindex %d \n",pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex,pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex);

                    if((pAmrThreadArg->pMtrAmrInfo[MsIndex].tmpcsdindex++) < Tmp_AmrLink->CsdTask.RoadTask[0].SubOadNum)
                        return 0;
                    TI.unit = TASK_CYCLE_FLAG_MIN;
                    TI.value = 15*taskmng_curve_point_get( Tmp_AmrLink->TaskId, Tmp_AmrLink->logicid);    //一次15min*8个点
                }

                DT_TimeAddInv(&pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, TI);
                //DT_Time2DateTimeBCD(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime, &tmpTime);
                //printx("负荷曲线当前时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                 //      tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);

                memcpy_r((uint8*)&tmpTime,(uint8*)RunTask->ThisTurnBeginTime,7);
                DT_DateTimeBCD2Time(&tmpTime,&endTime);
                //减去任务开始时间里包含的延时
                TI.unit = Tmp_AmrLink->DelayTimeUnit;
                TI.value = Tmp_AmrLink->DelayTime;
                DT_TimeSubInv(&endTime, TI);
                //DT_Time2DateTimeBCD(endTime, &tmpTime);
                //printx("负荷曲线结束时间:%02x%02x-%02x-%02x %02x:%02x:%02x\n", tmpTime.year_h, tmpTime.year_l,
                //       tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.min, tmpTime.sec);

                if(pAmrThreadArg->pMtrAmrInfo[MsIndex].StartTime < endTime)
                {
                    //TASK_FMT_TRACE( Pipe, REC_LOG, "负荷曲线存储数据 \n");
                    //taskmng_cache_buf_write_to_db(Pipe,Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0;
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                }
                else
                {
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].CSDIndex = 0xfe; //给一个较大的不等于0xff的索引，进入正常的任务完成流程
                    pAmrThreadArg->pMtrAmrInfo[MsIndex].MapIndex = 0;
                }
            }
        }

    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_send_queue_valid_check
* 函数功能: 检查抄表发送的5个并发队列是否有效
* 输入参数: 端口号
* 输出参数: 无
* 返 回 值:  TRUE  有已发送的数据
*        FALSE 队列全部无效
*******************************************************************************/
BOOL taskmng_send_queue_valid_check(uint8 pipe)
{
    BOOL flag = FALSE;
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    time_t timenow = time(NULL);
    uint8 i;
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
        {
            //可能会往前校时
            if(abs(timenow - pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime) >100)
            {
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag = 0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                continue;
            }
            flag = TRUE;
            break;
        }
    }
    return flag;
}


void Reply8802ConfirmFrame(uint8* AcqAddr,uint8 Pipe,uint16 Pn)
{
    AppData_T    AppData;
    uint16       Temp16 = 0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    uint8        Payload[500];
    uint16 index=0;
    //uint16 OverTime=100;
    //uint16 MsgIndex;
    MESSAGE_INFO_T    *MsgSend;

    AppData.DataUnitBuff[0] = 0x68;
    AppData.DataUnitBuff[1] = 0x18;      //长度L
    AppData.DataUnitBuff[2] = 0x00;
    AppData.DataUnitBuff[3] = 0x03;      //控制域
    AppData.DataUnitBuff[4] = 0x15;

    AppData.DataUnitBuff[5] =  AcqAddr[0];
    AppData.DataUnitBuff[6] = AcqAddr[1];
    AppData.DataUnitBuff[7] = AcqAddr[2];
    AppData.DataUnitBuff[8] = AcqAddr[3];
    AppData.DataUnitBuff[9] = AcqAddr[4];
    AppData.DataUnitBuff[10] = AcqAddr[5];

    AppData.DataUnitBuff[11] = 0x18;

    //帧头校验
    Temp16 = CRC16_Get(&AppData.DataUnitBuff[1], 11);
    memcpy(&AppData.DataUnitBuff[12], (char *)&Temp16, 2);
    //offset = offset +2;

    //apdu-固定数据
    AppData.DataUnitBuff[14] = 0x08;
    AppData.DataUnitBuff[15] = 0x02;
    AppData.DataUnitBuff[16] = gReponsePIIDCAD++;
    AppData.DataUnitBuff[17] = 0x01;          //5个OAD

    AppData.DataUnitBuff[18] = 0x60;
    AppData.DataUnitBuff[19] = 0x12;
    AppData.DataUnitBuff[20] = 0x03;
    AppData.DataUnitBuff[21] = 0x00;

    AppData.DataUnitBuff[22] = 0x00;                //00 标识NULL

    //帧校验
    Temp16 = CRC16_Get(&AppData.DataUnitBuff[1], 22);
    memcpy(&AppData.DataUnitBuff[23], (char *)&Temp16, 2);


    AppData.DataUnitBuff[25] = 0x16;
    AppData.DataUnitLen = 26;

    memset(Payload,0x00,sizeof(Payload));
    memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    memcpy_r(&Payload[index],(uint8*)&Pn,2);
    index+=2;
    index++;
    memcpy_r(&Payload[index],AcqAddr,6);
    index+=6;

    if(AppData.DataUnitLen<0x80)
    {
        Payload[index++] = AppData.DataUnitLen;
    }
    else
    {
        Payload[index++] = 0x82;
        Payload[index++] = (uint8)(AppData.DataUnitLen>>8);
        Payload[index++] = (uint8)AppData.DataUnitLen;
    }
    memcpy(&Payload[index],AppData.DataUnitBuff,AppData.DataUnitLen);
    index+=AppData.DataUnitLen;

    TASK_FMT_DEBUG( Pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);
    TASK_BUF_TRACE( Pipe, RELAY_LOG, Payload, index);

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 1;
    MsgSend->index = GetMsgSeq();
    MsgSend->label = Pipe;
    MsgSend->priority = 1;
    MsgSend->IID = 0x0003;
    MsgSend->IOP = 0x003E;

    MsgSend->msglen = index;
    MsgSend->sourlen = strlen(DownName);
    memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
    MsgSend->destlen = strlen(CCOName);
    memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
    memcpy(MsgSend->playload,Payload,index);

    taskmng_plc_queue_write(pAmrSendDown[Pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);

}


void Reply8806ConfirmFrame(uint8* AcqAddr,uint8 Pipe ,uint16 Pn)
{
    AppData_T    AppData;
    uint16       Temp16 = 0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    uint8        Payload[500];
    uint16 index=0;
    //uint16 OverTime=100;
    //uint16 MsgIndex;
    MESSAGE_INFO_T    *MsgSend;


    AppData.DataUnitBuff[0] = 0x68;
    AppData.DataUnitBuff[1] = 0x13;      //长度L
    AppData.DataUnitBuff[2] = 0x00;
    AppData.DataUnitBuff[3] = 0x03;      //控制域
    AppData.DataUnitBuff[4] = 0x15;

    AppData.DataUnitBuff[5] =  AcqAddr[0];
    AppData.DataUnitBuff[6] = AcqAddr[1];
    AppData.DataUnitBuff[7] = AcqAddr[2];
    AppData.DataUnitBuff[8] = AcqAddr[3];
    AppData.DataUnitBuff[9] = AcqAddr[4];
    AppData.DataUnitBuff[10] = AcqAddr[5];

    AppData.DataUnitBuff[11] = 0x18;

    //帧头校验
    Temp16 = CRC16_Get(&AppData.DataUnitBuff[1], 11);
    memcpy(&AppData.DataUnitBuff[12], (char *)&Temp16, 2);
    //offset = offset +2;

    //apdu-固定数据
    AppData.DataUnitBuff[14] = 0x08;
    AppData.DataUnitBuff[15] = 0x06;
    AppData.DataUnitBuff[16] = gReponsePIIDCAD++;
    AppData.DataUnitBuff[17] = 0x00;          //5个OAD


    //帧校验
    Temp16 = CRC16_Get(&AppData.DataUnitBuff[1], 17);
    memcpy(&AppData.DataUnitBuff[18], (char *)&Temp16, 2);


    AppData.DataUnitBuff[20] = 0x16;
    AppData.DataUnitLen = 21;

    memset(Payload,0x00,sizeof(Payload));
    memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    memcpy_r(&Payload[index],(uint8*)&Pn,2);
    index+=2;
    index++;
    memcpy_r(&Payload[index],AcqAddr,6);
    index+=6;

    if(AppData.DataUnitLen<0x80)
    {
        Payload[index++] = AppData.DataUnitLen;
    }
    else
    {
        Payload[index++] = 0x82;
        Payload[index++] = (uint8)(AppData.DataUnitLen>>8);
        Payload[index++] = (uint8)AppData.DataUnitLen;
    }
    memcpy(&Payload[index],AppData.DataUnitBuff,AppData.DataUnitLen);
    index+=AppData.DataUnitLen;

    TASK_FMT_DEBUG( Pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);
    TASK_BUF_TRACE( Pipe, RELAY_LOG, Payload, index);

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 1;
    MsgSend->index = GetMsgSeq();
    MsgSend->label = Pipe;
    MsgSend->priority = 1;
    MsgSend->IID = 0x0003;
    MsgSend->IOP = 0x003E;

    MsgSend->msglen = index;
    MsgSend->sourlen = strlen(DownName);
    memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
    MsgSend->destlen = strlen(CCOName);
    memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
    memcpy(MsgSend->playload,Payload,index);

    taskmng_plc_queue_write(pAmrSendDown[Pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);

}

/****************************************************************************
*函数名称：Oop_AUTK_Recv
*功能：  面向对象规约485抄表返回数据按照0AD进行存储

*输入参数：
*输出参数：
*返回：无
****************************************************************************/

uint16 save_freez_Data(uint8 pipe,OOP_OAD_U oad,OOP_OAD_U MainOad,uint8 *pBuf, uint16 rcvLen, uint16 pn, DateTimeBcd_t *StoreTime)
{
    uint16 offSet=0;
    time_t longTm;
    time_t colStartTm;

//    uint16 MsIndex;
    OadStoreInfo StoreInfo;
//    uint16 CsdIndex;
    uint8 MtrAddr[6];
    uint8 desbuffer[2000];
//    uint16 MeterIndex;
    uint16 deslength=0;
    OadInfoMap pOadInfo;
//    DateTimeBcd_t taskStoreTime;
//    DateTimeBcd_t taskStartTime;
    uint8 pwrType;


    //根据OAD查找OAD属性描述

    if(FALSE==taskmng_oad_info_get(oad, &pOadInfo))
    {
        return 0;
    }
    //保存采集到数据
    pwrType=taskmng_mtr_conntype_get(pn,0);
    Convert_Oop_Data_To_Struct(pBuf,&offSet,rcvLen,desbuffer,&pOadInfo,&deslength,pwrType);
    colStartTm=time(NULL);
    if(MainOad.nObjID == 0x5004 || MainOad.nObjID == 0x5006)// 日月数据
    {
        if(oad.nObjID == 0x2021)
        {
            if(StoreTime->sec == 0xFF)//林洋表回日冻结时间秒数可能为0xFF
            {
                StoreTime->sec = 0;
            }
            if(DT_DateTimeBCD2Time(StoreTime,&longTm)==FALSE)//应答的时间不合法
            {
                //不存储
                return 0;
            }
            //检测是否需要补抄
        }
        DT_DateTimeBCD2Time(StoreTime,&longTm);

    }
   else
    {
        DT_DateTimeBCD2Time(StoreTime,&longTm);
    }

    taskmng_mtr_addr_get(pn,0,MtrAddr);
    memcpy(StoreInfo.MAC.nValue,MtrAddr,6);
    StoreInfo.recordOAD.infoNum=pn;
    StoreInfo.recordOAD.logicId=0;
    if(MainOad.nObjID==0x0000)
    {

        StoreInfo.recordOAD.road.oadMain.value= 0x00;
        StoreInfo.recordOAD.road.oadCols.nNum=1;
        StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
        StoreInfo.recordOAD.road.oadCols.rsv=0;
    }
    else
    {
        StoreInfo.recordOAD.road.oadMain=MainOad;
        StoreInfo.recordOAD.road.oadCols.nNum=1;
        StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
        StoreInfo.recordOAD.road.oadCols.rsv=0;
    }
    StoreInfo.recordOAD.classtag = 5;
    StoreInfo.colStartTm=colStartTm;
    StoreInfo.colStoreTm=longTm;
    StoreInfo.colEndTm=time(NULL);
    //TASK_FMT_TRACE( pipe,COMM_LOG, "当前任务号:pCbTask->TaskId is %d\n",pCbTask->TaskId);

    taskmng_meter_data_save(desbuffer,deslength,StoreInfo,pipe);


    return offSet;


}

int SaveFreezData(uint16 pn,uint8 *apdu,uint16 len,uint8 pipe)
{
    uint16 offset=0;
    uint8 oadnum=0;
    uint8 i=0;
    uint16  rslt = 0;
    OOP_OAD_U oad[32];
    OOP_OAD_U MainOad;
    uint16 FindOffset;
    DateTimeBcd_t StoreTm;
    uint8  DarFlag = 0;
    uint8  sequenceOfARecordRow = 0;

    offset= offset+6;
    memcpy_r((uint8*)&MainOad.value,apdu+offset,4);
    offset= offset+4;
    TASK_FMT_TRACE ( pipe,COMM_LOG, "MainOad.value is %d\n",MainOad.value);
    oadnum = apdu[offset++];

    for(i=0;i<oadnum;i++)
    {
        memcpy_r((uint8*)&oad[i].value,apdu+offset,4);
        offset =offset+4;
    }
    DarFlag = apdu[offset++];
    sequenceOfARecordRow = apdu[offset];
    if((DarFlag==0x00)&&(sequenceOfARecordRow == 0xC8))
    {
        gLastFrameFlag = 1;
        return 1;
    }
    else if(DarFlag ==1)
    {

    }
    else
    {
         return -1;
    }
    offset++;
    while(sequenceOfARecordRow--)
    {
        //if(tmprecNum>1)
        {
            offset=offset+2;
        }
        FindOffset = offset;
        for(i=0; i<oadnum; i++)
        {
            if(oad[i].nObjID == 0x2021)//冻结时标
            {
                ((DateTimeHex_t*)(apdu+FindOffset+1))->sec = 0x00;
                //测试时标有回复为空的情况
                if(apdu[FindOffset] !=0)
                {
                    DT_DateTimeHex2BCD((DateTimeHex_t*)(apdu+FindOffset+1),&StoreTm);
                }
                break;
            }
            taskmng_oop_next_read_data_get(apdu,&FindOffset,len,NULL_DATA,FALSE);
        }
        for(i=0; i<oadnum; i++)
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "len is %d,offset is %d\n",len,offset);
            if(len<offset)
                return -1 ;


            //需要判断OAD是否在抄表CSD中，判断返回的报文中是否有相应的CSD并且存储;
            rslt = save_freez_Data(pipe, oad[i],MainOad,apdu+offset, len-offset, pn, &StoreTm);
            TASK_FMT_TRACE ( pipe,REC_LOG, "rslt is %d\n",rslt);
            //TASK_FMT_TRACE ( pipe,COMM_LOG, "OAD rslt is %d\n",rslt);
            if(rslt == 0)
            {
                taskmng_oop_next_read_data_get(apdu,&offset,len,NULL_DATA,FALSE);
            }
            else
            {

                offset+=rslt;
            }

        }

    }
    return 0;

}

//int SaveFreezData(uint16 pn,uint8 *apdu,uint16 len,uint8 pipe)
//{
//    uint16 offset=0;
//    uint8 oadnum=0;
//    uint8 i=0;
//    uint8 recNum;
//    uint8 tmprecNum;
//    uint16  rslt = 0;
//    OOP_OAD_U oad[32];
//    OOP_OAD_U MainOad;
//    uint16 FindOffset;
//    DateTimeBcd_t StoreTm;
//    uint8  DarFlag = 0;
//    uint8  sequenceOfARecordRow = 0;
//    uint16 tempOadNum = 0;
//
//    offset= offset+14;
//    tempOadNum = apdu[offset];
//    sequenceOfARecordRow = apdu[offset];
//    if (tempOadNum > 80)
//  {
//      printx("ERROR: Read OadNum = %d \n",tempOadNum);
//      return RET_ERR;
//  }
//    //判断DAR标志
//  offset += 4*tempOadNum+1;
//  DarFlag = apdu[offset];
//  offset += 1;
//  sequenceOfARecordRow = apdu[offset];
//    if(DarFlag == 0x01)
//  {
//      printx("DarFlag = 0x01:需回复确认帧 \n");
//  }
//  else if((DarFlag == 0x00) && (sequenceOfARecordRow == 0xC8))
//  {
//      printx("DarFlag = 0x00, sequenceOfARecordRow == 0xC8,日数据最后一帧)\n");
//      gLastFrameFlag = 1;
//  }
//  else
//  {
//      printx("数据格式错误,DarFlag = %d,sequenceOfARecordRow = %d\n",DarFlag,sequenceOfARecordRow);
//      return RET_ERR;
//  }
//
//}


void Deal8802ReportData(uint8 *apdu,uint16 apdulen,uint8 Pipe,uint16 pn)
{
    int ret=0;
    //当前8802只考虑有日冻结数，6012任务配置表,属性3(记录表)::array 记录单元
    if((apdu[0] == 0x60) && (apdu[1] == 0x12) && (apdu[2] == 0x03) && (apdu[3] == 0x00))
    {
        //日冻结数据
        if((apdu[6] == 0x50) && (apdu[7] == 0x04) && (apdu[8] == 0x02) && (apdu[9] == 0x00))
        {

            ret = SaveFreezData(pn,apdu,apdulen,Pipe);
            if(ret < 0)
            {
                TASK_FMT_DEBUG( Pipe, REC_LOG,"日冻结数据解析错误\n");
                ret = -3;
            }

        }
        else if((apdu[6] == 0x50) && (apdu[7] == 0x06) && (apdu[8] == 0x02) && (apdu[9] == 0x00))
        {
            TASK_FMT_TRACE( Pipe,REC_LOG,"月冻结复合数据开始解析\n");
            ret = SaveFreezData(pn,apdu,apdulen,Pipe);
            if(ret < 0)
            {
                TASK_FMT_TRACE( Pipe,REC_LOG,"月冻结数据解析错误\n");
                ret = -4;
            }


        }
        else
        {
            TASK_FMT_TRACE( Pipe,REC_LOG,"Incorrect Data apdu[10] = %02x,apdu[11] = %02x,apdu[12] = %02x,apdu[13] = %02x!\n",apdu[10],apdu[11],apdu[12],apdu[13]);
            ret = -5;
        }

    }
    else
    {
        TASK_FMT_TRACE( Pipe,REC_LOG,"Incorrect Data apdu[4] = %02x,apdu[5] = %02x,apdu[6] = %02x,apdu[7] = %02x!\n",apdu[4],apdu[5],apdu[6],apdu[7]);
        ret = -6;
    }



}
int DealOneMinuteData(uint8 *apdu,uint16 apdulen,uint8 Pipe)
{

    //uint8 *headOadInfo   =NULL;
    uint8  DarFlag       =0;
    uint8  readMeterCount=0;   //从报文中读取的表的个数
    uint8  MeterAddr[20][6];     //新增临时变量用于查找相应的pn
    OOP_OAD_U Oad[36];
    uint8  OadType[36];
    uint8 Oadlen=0;
    OOP_OAD_U MainOad;
    uint16 offset = 0;
    uint16 tmpoffset1=0;
    uint16 tmpoffset2=0;
    uint16 tmpoffset=0;
    uint16 len = 0;              //单个记录数据长度
    uint16 pn=0;
    uint16 recordSumNum;         //报文解析后的总个数
    uint16 OadNum = 0;
    uint16 frezeeintv;
    uint8  unit;
    uint32 intv;
    uint16 recordNum;
    time_t Storetime;
    DateTimeBcd_t StoreTm;
    OadInfoMap pOadInfo;
    uint8 desbuffer[500];
    uint16 deslength;
    OadStoreInfo StoreInfo;
    time_t longTm;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    BOOLEAN isvaliad = FALSE;
    //time_t colStartTm;

    int i = 0,j = 0,k = 0,n=0,m=0;

    for(i=0;i<20;i++)
    {
        memset(&MeterAddr[i][0],0,6);
    }
    TASK_FMT_TRACE(Pipe, REC_LOG,"apdulen is %d\n",apdulen);
    if (apdulen <= 0)
    {
        return RET_ERR;
    }
    offset += 26;    //判断有几个数据表地址
    if(apdulen<26)
    {
        gLastFrameFlag = 1;
        pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
        return RET_ERR;
    }
    readMeterCount = apdu[offset];
    TASK_FMT_TRACE(Pipe, REC_LOG,"readMeterCount is %d\n",readMeterCount);
    if(readMeterCount >= 20)
    {

        return RET_ERR;
    }

    //把所有表地址都读取出来
    offset++;
    for(i=0;i<readMeterCount;i++)
    {
        if((apdu[offset] == 0x07) && (apdu[offset+1] == 0x05))
        {
            offset+=2;
            for(j=5;j>=0;j--)
            {
                MeterAddr[i][j]= apdu[offset];
                offset++;

            }
        }
        else
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"offset is %d\n",offset);
            return RET_ERR;
        }

    }
    //从测量点中获取采集器地址并判断是否已在档案中
    for (i=0; i < readMeterCount; i++)
    {
        //找到电表地址
        pn = taskmng_pn_get_from_addr(MeterAddr[i],0);

        //填时间
        offset = 8;
        if(apdu[offset] == 0xFF || apdu[offset] == 0x00)  //时间可能为0xFF 无效
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"1min ERROR: 时间无效 Start:%d-%d-%d %d:%d:%d\n",((uint16)apdu[offset]<<8)+(uint16)apdu[offset+1],apdu[offset+2],apdu[offset+3],apdu[offset+4],apdu[offset+5],apdu[offset+6]);
            //gNeedReply8806ConfirmFlag = 1;
            return RET_OK;
        }
        TASK_FMT_TRACE(Pipe, REC_LOG,"Start:%d-%d-%d %d:%d:%d\n",((uint16)apdu[offset]<<8)+(uint16)apdu[offset+1],apdu[offset+2],apdu[offset+3],apdu[offset+4],apdu[offset+5],apdu[offset+6]);
        TASK_FMT_TRACE(Pipe, REC_LOG,"End  :%d-%d-%d %d:%d:%d\n",((uint16)apdu[offset+7]<<8)+(uint16)apdu[offset+8],apdu[offset+9],apdu[offset+10],apdu[offset+11],apdu[offset+12],apdu[offset+13]);
        DT_DateTimeHex2BCD((DateTimeHex_t*)&apdu[8],&StoreTm);
        DT_DateTimeBCD2Time(&StoreTm,&Storetime);

        //填时间间隔
        offset += 14;
        unit = apdu[offset++];
        TASK_FMT_TRACE(Pipe, REC_LOG,"apdu[%d] = 0x%02x,apdu[%d] = 0x%02x\n",offset,apdu[offset],offset+1,apdu[offset+1]);
        frezeeintv= ((uint16)apdu[offset]<<8)+(uint16)apdu[offset+1];
        TASK_FMT_TRACE(Pipe, REC_LOG,"时间间隔 %d min\n",frezeeintv);
        if(frezeeintv != 1)
        {
            //return RET_ERR;
        }
        if(unit==1)
        {
            intv = frezeeintv;

        }else if(unit ==2)
        {
            intv = frezeeintv*60;

        }
        else if(unit ==3)
        {
            intv = frezeeintv*1440;

        }
        else
        {
            intv = frezeeintv;


        }

        //一组用户地址
        offset+=2;
        if(apdu[offset] == 0x03)
        {
           TASK_FMT_TRACE(Pipe, REC_LOG,"MS:一组用户地址\n");
        }

        //上传的表地址个数
        offset++;
        //填ROAD数量
        offset+= readMeterCount*8+1;
//      if((apdu[offset] != 0x50) ||(apdu[offset+1] != 0x02))
//      {
//          TASK_FMT_TRACE(Pipe, REC_LOG,"1min ERROR: ROAD 数据类型不匹配，非分钟冻结数据\n");
//          return RET_ERR;
//      }
        MainOad.value =bin2_int32u(&apdu[offset]);
        //读OAD数量
        offset += 4;
        OadNum = apdu[offset];
        if(OadNum == 0)
        {
            gLastFrameFlag = 1;
            pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
            return RET_ERR;
        }
        if (OadNum> 24)
        {
            //return RET_ERR;
        }
        offset += 1;
        for(m =0;m<OadNum;m++)
        {
            Oad[m].value = bin2_int32u(&apdu[offset]);
            offset = offset+4;
        }

        //判断DAR标志
        DarFlag = apdu[offset];
        if((DarFlag == 0x01) || (DarFlag == 0xC8))
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"DarFlag =0x%02x\n",DarFlag);
            if(DarFlag == 0xC8)
            {
                gLastFrameFlag = 1;
                pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
                //return RET_ERR;
            }
        }
        else
        {
            TASK_FMT_TRACE(Pipe, REC_LOG,"1min ERROR: DarFlag = %d\n",DarFlag);
            return RET_ERR;
        }

        //填单个数据点数据长度
        offset++;

        for (j = 0; j < OadNum; j++)
        {
            OadType[j] = apdu[offset + j];
        }

        TASK_FMT_TRACE(Pipe, REC_LOG,"一条数据长度 %d 个字节\n",len);

        //记录个数 M条记录数据值 SEQUENCE OF A-SimplifyRecordRow
        offset+=j;
        recordSumNum = apdu[offset];  //报文读取后的总记录数
        recordNum= recordSumNum/readMeterCount;
        TASK_FMT_TRACE(Pipe, REC_LOG,"总记录个数: %02x,每块表包含%d个记录\n",recordSumNum,recordNum);

        offset += 1;


        offset=tmpoffset*i+offset;
        tmpoffset1 = offset;
        for(k = 0; k < recordNum; k++)
        {

            longTm=Storetime+k*frezeeintv*60;

            for(n =0;n<OadNum;n++)
            {
                isvaliad = FALSE;
                if(FALSE==taskmng_oad_info_get(Oad[n], &pOadInfo))
                {
                    return 0;
                }
                Oadlen = getDataTypeLength(OadType[n]);
                TASK_BUF_DEBUG( Pipe, REC_LOG, apdu+offset,Oadlen);
                if(check_is_all_ch(apdu+offset, 0xFF, Oadlen))
                {
                    isvaliad = TRUE;
                }
                Convert_Minute_Data_To_Struct(apdu+offset,Oadlen,desbuffer,&pOadInfo,&deslength);
                TASK_BUF_DEBUG( Pipe, REC_LOG, desbuffer,deslength);
                memcpy(StoreInfo.MAC.nValue,MeterAddr[i],6);
                StoreInfo.recordOAD.infoNum=pn;
                StoreInfo.recordOAD.logicId=0;

                {
                    StoreInfo.recordOAD.road.oadMain=MainOad;
                    StoreInfo.recordOAD.road.oadCols.nNum=1;
                    StoreInfo.recordOAD.road.oadCols.oad[0]=Oad[n];
                    StoreInfo.recordOAD.road.oadCols.rsv=0;
                }
                StoreInfo.recordOAD.classtag = 5;
                StoreInfo.colStartTm=time(NULL);
                StoreInfo.colStoreTm=longTm;
                StoreInfo.colEndTm=time(NULL)+1;
                if(isvaliad ==FALSE)
                {
                    taskmng_leak_data_save(desbuffer,deslength,StoreInfo,Pipe,intv);
                }
                offset = offset + Oadlen;
            }
            tmpoffset2 = offset;
            tmpoffset =tmpoffset2 -tmpoffset1;

        TASK_FMT_TRACE(Pipe, REC_LOG,"tmpoffset is %d,i is %d\n",tmpoffset,i);
        }

    }

    //判断是否需要回复确认帧
    if(DarFlag == 0x01)
    {
        TASK_FMT_TRACE(Pipe, REC_LOG,"1min 需要回复确认帧\n");
    }
    else    //DarFlag == 0xC8
    {
        TASK_FMT_TRACE(Pipe, REC_LOG,"1min 最后一帧 \n");
        //gLastFrameFlag = 1;
    }
    //gNeedReply8806ConfirmFlag = 1;
    return RET_OK;




}
void DealCuredata8806(uint8 *buf,uint16 buflen,uint8 Pipe,uint16 Pn)
{
    uint16 offset=0;
    OOP_OAD_U Tempoad;

    if((buf[0] !=0x88)||(buf[1] !=0x06))
    {
        return;
    }
    Tempoad.value=bin2_int32u(buf+3);
    if(Tempoad.value !=0x60120300)
    {
       return;
    }
    offset = offset+7;
    if(buf[22]==1)
    {
        DealOneMinuteData(buf,buflen,Pipe);

    }
    else if(buf[22]==2)
    {

    }
    else if(buf[22]==3)
    {

    }
    else if(buf[22]==4)
    {

    }
    else
    {

    }


}
#ifdef AREA_JIANGSU
/*******************************************************************************
* 函数名称: ActionRequestNormal
* 函数功能: 设备查询
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  完成
*           <0                  未完成
*******************************************************************************/
int AppointRequestNormal(uint8 Pipe)
{
    char            apdu[2];                                  //apdu的服务类型
    //uint8           startSec;
    //uint8           endSec;
    //uint8         task_report_flag = 0;
    uint8           RecvBuf[1024] = {0};        //接收数据缓冲区

    uint16          MtrIndex = 0;                             //测量点索引
    //uint16            len = 0;

    int             i;
    int             RecvLen = 0;  //接收数据长度
    int             ret = 0;
    //int           nRet = -1;

    //static time_t   currTime=0;
    //struct timeval  tp;
    //struct tm       p_tm;
    //AppData_T       AppData = {0};                            //应用层数据

    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[Pipe];
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8           PayloadBuf[1000] ;        //payload缓冲区
    AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
    uint8           SendIndex=0;
    MeterPrtlFmt_T  MeterPrtlData;
    uint16 index;
    static uint32 revtime;
    uint16 Pn;
    uint16 caqpn=0;
   //while(1)
    {
    switch (pAmrThreadArg->TotalState.MdlGetDataDeal)
    {
        case MDL_METER_IDLE:

            TASK_FMT_TRACE(Pipe,REC_LOG, "进入设置全网上报流程\n");
            revtime = time(NULL);
            taskmng_comm_recv_buff_clear(Pipe);                               //清通讯接收缓冲区
            pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_SEND;
            pAmrThreadArg->TmnAmrInfo.collectorIndex = 0;
            pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt = 0;
            memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
            pAmrThreadArg->AmrSendFrameInfo.usetype = 0;
            pAmrThreadArg->SendQueueFlag = 0;
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            gLastFrameFlag = 0;
            TASK_FMT_TRACE(Pipe,REC_LOG, "开启集中器点名上报\n");
            break;
        case MDL_METER_SEND:
            if(Collector.count == 0)
            {
                TASK_FMT_TRACE(Pipe,REC_LOG, "集中器下无采集器,不执行点名操作\n");
                pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_IDLE;
                return 1;
            }
           ret=taskmng_paral_cjq_frame_make(Pipe);    //并发点名
           if(ret == -2)
           {
                pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_IDLE;
                TASK_FMT_TRACE(Pipe,REC_LOG, "点抄完毕\n");
                return 1;
           }
            usleep(10000);
            //currTime = time(NULL);
            pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_RESPONSE;
            break;

        case MDL_METER_RESPONSE:

            if(pAmrThreadArg->TmnAmrInfo.collectorIndex >= Collector.count)
            {
                for(i=0;i<5;i++)
                {
                    if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1) break;
                }
                if(i>=5)
                {
                    TASK_FMT_TRACE(Pipe,REC_LOG, "所有点名结束,关闭 AppointRequestNormal\n");
                    pAmrThreadArg->TotalState.MdlGetDataDeal= MDL_METER_IDLE;
                    return 1;
                }
            }
             RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[Pipe], RecvBuf);
            if (RecvLen > 0)
            {
                    revtime = time(NULL);
                    MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
                    memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
                    TASK_FMT_TRACE( Pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);

                    TASK_BUF_TRACE( Pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
                    switch (MsgInfo->IOP)
                    {
                        case 0x0031:
                        {
                            if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                                break;

                            if(taskmng_seq_is_caj_valid(Pipe,&SendIndex,AmrReportInfo.nMeterFilesID)==0)
                            {
                                TASK_FMT_DEBUG(Pipe, REC_LOG,"未获取到发送队列索引 \n");
                                break;
                            }
                            pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].CommBeginTime = time(NULL);
                            TASK_FMT_DEBUG(Pipe, REC_LOG,"刷新时间 \n");
                            ret = taskmng_paral_caj_read_data_unpack(Pipe,SendIndex,pAmrThreadArg->TmnAmrInfo.CurrentLogicid,AmrReportInfo,&MeterPrtlData,&MtrIndex,apdu,pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex]);

                            if(ret<0)
                            {
                                taskmng_caj_state_refresh(Pipe,SendIndex);
                                pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
                                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                                break;
                            }
                            if(MeterPrtlData.PrtlMode == PRTL_OOP)
                            {
                                OOP_TSA_T tsa;
                               taskmng_caj_addr_get(pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn,tsa.add);
                                if((apdu[0] == 0x88) && (apdu[1] == 0x02))
                                {
                                    index =3;
                                    OOP_TSA_T tsa;
                                    Pn=taskmng_pn_get_from_addr(MeterPrtlData.MtrAddr,0);
                                    taskmng_caj_addr_get_from_meteraddr(&caqpn,MeterPrtlData.MtrAddr,tsa.add);
                                    uint8 num = MeterPrtlData.DataBuff[index++];
                                    //uint16 offset = 0;
                                    TASK_FMT_DEBUG(Pipe, REC_LOG,"num is %d, MeterPrtlData.Pn is %d\n",num,MeterPrtlData.Pn);
                                    TASK_BUF_DEBUG(Pipe, REC_LOG,MeterPrtlData.DataBuff,MeterPrtlData.DataLen);
                                    for(i=0;i<num;i++)
                                    {
//                                        offset = event_report_data_save(Pipe,MeterPrtlData.DataBuff+index,MeterPrtlData.DataLen-index,MeterPrtlData.Pn);
//                                        index +=offset;
                                        SaveFreezData(Pn,MeterPrtlData.DataBuff+index,MeterPrtlData.DataLen-index,Pipe);
                                        data_report_data_save(Pipe,MeterPrtlData.DataBuff+index,MeterPrtlData.DataLen-index,Pn);
                                    }
                                    if(ret < 0)
                                    {

                                    }
                                    Reply8802ConfirmFrame(tsa.add,Pipe,caqpn);
                                }
                            if((apdu[0] == 0x88) && (apdu[1] == 0x06))
                            {

                                index =3;
                                ret=DealOneMinuteData(MeterPrtlData.DataBuff,MeterPrtlData.DataLen,Pipe);
                                if(ret >= 0)
                                {
                                    TASK_FMT_DEBUG(Pipe, REC_LOG,"ret is %d\n",ret);

                                    taskmng_event_report_data_write(Pipe,MeterPrtlData.DataBuff+index,MeterPrtlData.DataLen-index,MinuteReportOad);
                                }
                                //uint8 num = MeterPrtlData.DataBuff[index++];
                                //uint16 offset = 0;
                                //TASK_FMT_DEBUG(Pipe, REC_LOG,"num is %d, MeterPrtlData.Pn is %d\n",num,MeterPrtlData.Pn);
                                if(ret < 0)
                                {


                                }
                                if(gLastFrameFlag==1)
                                {
                                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                                    memset((uint8*)&pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].AcqAddr,0x00,sizeof(FRAME_SEQ_INFO_T));
                                    gLastFrameFlag =0;
                                }
                                Reply8806ConfirmFrame(tsa.add,Pipe,pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn);
                                //task_report_flag = 1;

                            }
                            if((apdu[0] == 0x87) && (apdu[1] == 0x01))
                            {
                                pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt++;
                            }
                            }
                            }


                            pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_SEND; //切换到发送状态
                            break;
                    default:

                        break;
                }
            }
            else
            {
                if(((time(NULL)-revtime)>60)&&(finishstate == 1))
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"ret is %d\n",ret);
                    pAmrThreadArg->TmnAmrInfo.AmrSucceedCnt = Collector.count;
                    finishstate = 0;
                }
                pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_SEND;//切换到发送状态
                break;
            }
            break;
        default:
            pAmrThreadArg->TotalState.MdlGetDataDeal = MDL_METER_IDLE;
            break;
    }
    }

    return -1;
}
#endif
/*******************************************************************************
* 函数名称: taskmng_data_reading_normal
* 函数功能: 日常抄表任务
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_data_reading_normal(uint8 Pipe)
{
    if(Pipe == 1)
    {
        return bt_data_reading_normal(Pipe);
    }
    //TASK_FMT_DEBUG( Pipe, REC_LOG, "taskmng_data_reading_normal\n");
    int             ret;
    uint16          i;
    int             RecvLen = 0;                              //接收数据长度
    uint8           PayloadBuf[4096] ;        //payload缓冲区
    //MSG_T           Msg;
    AmrThreadArg_T  *pAmrThreadArg;
    MESSAGE_INFO_T      *MsgInfo = NULL;
    uint8           RecvBuf[4096] ;
    AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
    MeterPrtlFmt_T  MeterPrtlData;
    uint8           SendIndex=0;
    uint16          MtrIndex = 0;
    char            apdu[2];                                  //apdu的服务类型
    uint16          Pn;
    uint8           TaskId;
    uint8 logicid;
    int             OverTimeCnt = 100;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    BOOL  EventRet = TRUE;
    DI_INFO_T *diinfo = NULL;
    CTS_RUN_TASK_T *tmpTask = NULL;
    uint8 dataflag=0;
    uint16 recvmapindex;

    if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
        return 1;
    if(pAmrThreadArg->TaskModifyFlag!=TASK_CFG_INITED)
        return 1;//任务未初始化 结束

    ret = taskmng_task_runlist_check(Pipe,&tmpTask);
    if(ret < 0)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "当前无需要抄表任务 结束\n");
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        gFreeFlag[Pipe] = 1;
        pAmrThreadArg->SendQueueFlag = 0;
        return 1; //当前无需要抄表任务 结束
    }
//#if IS_NEED_GET_ROUTER_STATE
    #if AREA_SHANDONG
    if(((1==gRouterStudy)||(pAmrThreadArg->TmnAmrInfo.MtrCnt>100))&&(taskmng_router_state_check(Pipe)<0))
    {
        return -1;
    }
    #else
    if((1==gRouterStudy)&&(taskmng_router_state_check(Pipe)<0))
    {
        return -1;
    }
    #endif

//#endif
    SCHEME_TASK_RUN_INFO SchemeTaskInfo ;
    //memset(&SchemeTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    SCHEME_TASK_RUN_INFO RecvTaskInfo ;
    //memset(&RecvTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    CTS_RUN_TASK_T *RecvRunTask = NULL;
    //新一轮任务开始执行时，如果当前正好有这块表在补抄，则无法识别上报的抄表报文是对应哪个的，故而将
    //本轮任务稍微延时，最多延时两分钟
    //只有补抄时才判断这个 如果不判这个有可能一分钟任务会不执行
    if(tmpTask->IsNewTurn==1 && pAmrThreadArg->AmrSendFrameInfo.usetype == 1 &&TRUE ==taskmng_send_queue_valid_check(Pipe) )
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "task %d queue valid! SendQueueFlag %d \n",tmpTask->TaskId,pAmrThreadArg->SendQueueFlag);
        if(pAmrThreadArg->SendQueueFlag<2)
        {
            pAmrThreadArg->SendQueueFlag++;
            return 1;
        }
    }

    //获取抄表任务信息
    taskmng_task_info_get(tmpTask->TaskId,tmpTask->logicid,&SchemeTaskInfo);

    if(SchemeTaskInfo.AcqInfo.Acqtype == ACQTYP_BC)
    {
        //TASK_FMT_DEBUG( Pipe, REC_LOG, "执行采集类型为补抄任务 task %d \n",tmpTask->TaskId);
    }

    //如果需要抄的任务和当前的不一致则抄当前优先级更高的admin
    if(tmpTask->TaskId != pAmrThreadArg->TmnAmrInfo.CurrentTaskId ||tmpTask->logicid!=pAmrThreadArg->TmnAmrInfo.CurrentLogicid)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "taskchange lastid %d now %d  logicid %d \n",pAmrThreadArg->TmnAmrInfo.CurrentTaskId,tmpTask->TaskId,tmpTask->logicid);
        pAmrThreadArg->TmnAmrInfo.CurrentTaskId = tmpTask->TaskId;
        pAmrThreadArg->TmnAmrInfo.CurrentLogicid = tmpTask->logicid;
        //所有表都重新开始抄读
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    }
    //任务到新一轮 重新抄读
    if(tmpTask->IsNewTurn==1)
    {
        TASK_FMT_DEBUG( Pipe, REC_LOG, "task %d IsNewTurn\n",tmpTask->TaskId);
        tmpTask->IsNewTurn=0;
        taskmng_time_error_num_init_check(Pipe,&SchemeTaskInfo);

        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
    }
    if(SchemeTaskInfo.SchemeType == ENCRYPTPLAN)
    {
        return taskmng_data_reading_encrypt(Pipe,&SchemeTaskInfo,tmpTask);
    }else
    {
        //如果在执行别的任务了 也给转加密的置-1
        //直接赋值有问题 如果485口正在执行 就不对了
        if(gEncryptPipe == Pipe)
        {
            TASK_FMT_TRACE(Pipe, REC_LOG, "gEncryptPipe %d 置为-1\n",gEncryptPipe);
            gEncryptPipe = -1;
        }
    }

    if(SchemeTaskInfo.SchemeType == TRANSPARENTPLAN)
    {
        return taskmng_data_reading_trans(Pipe,&SchemeTaskInfo,tmpTask);
    }
    //如果是分钟采集任务 超时时间改为10秒
    if(SchemeTaskInfo.MinTaskFlag == 1)
    {
        OverTimeCnt = 10;
    }
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
    case RTS_IDLE:
         for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
         {
            pAmrThreadArg->pMtrAmrInfo[i].CSDIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].ROADIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].SigFrameNum = 0;
            pAmrThreadArg->pMtrAmrInfo[i].MapIndex = 0;
            pAmrThreadArg->pMtrAmrInfo[i].BroadFailCnt=0;
            //不能在初始化的时候给所有日冻结标志置0 这样在现场有多个日冻结任务的时候会有问题
            //            if(SchemeTaskInfo.CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
            //            {
            //                pAmrThreadArg->pMtrAmrInfo[i].DayFrozenFlag = 0;
            //
            //            }

            taskmng_task_info_init(Pipe,&SchemeTaskInfo,tmpTask,&pAmrThreadArg->pMtrAmrInfo[i]);
        }
        taskmng_task_record_file_load(Pipe,&SchemeTaskInfo,tmpTask);
        taskmng_comm_recv_buff_clear(Pipe);
        //TASK_FMT_TRACE( Pipe, REC_LOG, "NormalAmr 清空发送队列 设置路由暂停\n");
        memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
        memset(&pAmrThreadArg->meterData,0x00,sizeof(pAmrThreadArg->meterData));
        pAmrThreadArg->AmrSendFrameInfo.usetype = 0;
        pAmrThreadArg->SendQueueFlag = 0;
        //互换性测试收到路由暂停时回复的抄表数据帧序号有问题 先注释
        //taskmng_router_ctrl_set ( Pipe,RCS_PAUSE );
        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        break;
    case RTS_SEND:
        if(taskmng_paral_amr_frame_make(Pipe,tmpTask,SchemeTaskInfo)==1)
        {
            TASK_FMT_DEBUG( Pipe, REC_LOG, "全部抄完 任务%d 结束&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n",tmpTask->TaskId);
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            //此处返回2配合taskmng_plc_task_run中 继续执行该函数
            return 2;
        }
        //补抄判断一下是否所有时间间隔抄完，没有的话继续抄
        if(SchemeTaskInfo.AcqInfo.Acqtype == ACQTYP_BC && tmpTask->PatchNextTurn == 1)
        {
            //TASK_FMT_TRACE( Pipe, REC_LOG, "补抄判断一下是否所有时间间隔抄完，没有的话继续抄\n");
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }
        else
        {
            //TASK_FMT_TRACE( Pipe, REC_LOG, "TotalState.NormalAmr：%x\n", pAmrThreadArg->TotalState.NormalAmr);
            pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
        }
        break;
    case RTS_RECV:
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[Pipe], RecvBuf);
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_TRACE( Pipe, REC_LOG, "Recv msgindex %d IOP %04X payload \n",MsgInfo->index,MsgInfo->IOP);

            TASK_BUF_TRACE( Pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            //测试代码
//            uint8 test[] = {0xF2,0x09,0x02,0x01,0x04,0x98,0x30,0x30,0x00,0x04,0x02,0x17,0x68,0x04,0x00,0x30,0x30,0x98,0x04,0x68,0x91,0x0b,0x34,0x34,0x43,0x39,0x33,0x46,0x58,0x3b,0x55,0x63,0x55,0x69,0x16,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//            memcpy(PayloadBuf,test,sizeof(test));
//            TASK_FMT_TRACE( Pipe, REC_LOG, "test  %d \n",sizeof(test));
//
//            TASK_BUF_TRACE( Pipe, REC_LOG,PayloadBuf,sizeof(test)) ;
//            MsgInfo->msglen = sizeof(test);
            switch (MsgInfo->IOP)
            {
//            case 0x003E:    //主动抄表回复的确认帧
//            {
//                if(PayloadBuf[0] == TRUE)
//                    break;
//                else
//                {
//                }
//                break;
//            }
//
            case 0x0031:  //上报的抄读数据
#ifdef AREA_HUNAN
            case 0x0071:
#endif
            {
                //TASK_FMT_TRACE(Pipe, REC_LOG,"@@@@@收到抄表上报数据@@@@@@@@@@ \n");
#ifdef AREA_HUNAN
                if(MsgInfo->IOP == 0x0071)
                {
                    if(taskmng_amr_report_data_unpack_0071(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                        break;
                }else
                {
                    if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                        break;
                }
#else
                if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                    break;
#endif
                //TASK_FMT_TRACE(Pipe, REC_LOG,"解消息成功 获取发送队列索引 \n");
                //新加物联网表后不能用信息点号来匹配 得用地址
                if(taskmng_seq_is_valid(Pipe,&SendIndex,AmrReportInfo.addr)==0)
                {
                    TASK_FMT_DEBUG(Pipe, REC_LOG,"未获取到发送队列索引 \n");
                    break;
                }
                TASK_FMT_DEBUG(Pipe, REC_LOG,"pn %d 上报数据\n",pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn);
                memset(&MeterPrtlData,0x00,sizeof(MeterPrtlData));
                AmrReportInfo.nMeterFilesID = pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn;
                ret = taskmng_paral_meter_read_data_unpack(Pipe,SendIndex,pAmrThreadArg->TmnAmrInfo.CurrentLogicid,AmrReportInfo,&MeterPrtlData,&MtrIndex,apdu,pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex]);
                //TASK_FMT_TRACE(Pipe, REC_LOG,"解帧ret %d 电表号 %d prtl %d \n",ret,AmrReportInfo.nMeterFilesID,MeterPrtlData.PrtlMode);
                if(ret<0)
                {
                    taskmng_pn_state_refresh(Pipe,SendIndex);
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }

                if(AmrReportInfo.isExitReadingInfo ==1 )
                {
                    memcpy(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].ReadingInfo,&AmrReportInfo.stPlcReadingInfo,sizeof(AMR_PLC_READING_INFO_T));
                }
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime = time(NULL);
                taskmng_col_state_write(Pipe,MtrIndex);
                Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn;
                //TASK_FMT_TRACE( Pipe, REC_LOG,"收到上报数据 taskid %d CurrentTaskId %d \n",pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId,pAmrThreadArg->TmnAmrInfo.CurrentTaskId);
                TaskId = pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId;
                logicid = pAmrThreadArg->TmnAmrInfo.CurrentLogicid;
                //如果当前任务已变更 则不处理
                if(TaskId!=pAmrThreadArg->TmnAmrInfo.CurrentTaskId )
                    break;
                taskmng_task_info_get(TaskId,logicid,&RecvTaskInfo);
                taskmng_run_task_get( Pipe,TaskId,logicid,&RecvRunTask);
                taskmng_task_monitor_update(Pipe,TaskId,logicid,UDTASK_RECVFRMNUM,NULL);
                if(AmrReportInfo.nProtocolType==PRTL_OOP &&ret == eOopComm_SegRecv)
                {
                    TASK_FMT_TRACE( Pipe, REC_LOG,"收到链路分帧数据\n");
                    pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                    pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    break;
                }
                if(MeterPrtlData.PrtlMode == PRTL_OOP)
                {
                    if(ret ==eOopComm_GetNext )
                    {
                        TASK_FMT_TRACE( Pipe, REC_LOG,"收到应用分帧数据 继续解析\n");
                        pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].NeedSendSeg=1;
                        taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,1);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    //TASK_FMT_TRACE( Pipe, REC_LOG, "面向对象报文解析后DataLen %d \n",MeterPrtlData.DataLen);

                    //TASK_BUF_TRACE( Pipe, REC_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                    //实时数据采用的是GetResponseNormalList数据类型
                    if(apdu[0] == 0x85 && apdu[1] == 0x01)
                    {
                        //printx("返回的服务类型:GetResponseNormalList!\n");
                        //按照GetRequestNormalList数据类型服务进行解析
                        taskmng_oop_meter_data_normal_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask);
                    }
                    if(apdu[0] == 0x85 && apdu[1] == 0x02)
                    {
                        //printx("返回的服务类型:GetResponseNormalList!\n");
                        //按照GetRequestNormalList数据类型服务进行解析
                        taskmng_oop_meter_data_normal_list_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask);
                    }

                    //采用的是GetRequestRecord数据类型
                    if(apdu[0] == 0x85 && (apdu[1] == 0x03||apdu[1] == 0x05))
                    {
                        //printx("返回的服务类型:GetRequestRecord!\n");
                        #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
                        //按照GetRequestRecord数据类型服务进行解析
                        if(MeterPrtlData.Istasktable == 1)
                        {
                            taskmng_oop_meter_data_tasktable_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                        }
                        else
                        {
                            taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,0);
                        }
                        #else
                        taskmng_oop_meter_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL,0);
                        #endif
                    }
                }
                else if(MeterPrtlData.PrtlMode == PRTL_188)
                {
                    taskmng_188_data_record_save(Pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen,MtrIndex,&RecvTaskInfo,RecvRunTask);
                }
                else
                {
                    diinfo = &pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].DiInfo;
                    for(i=0;i<MeterPrtlData.framenum;i++)
                    {
                        if(MeterPrtlData.frameinfo[i].result == CHECK_RESULT_SUCCEED)
                        {
                            ret = taskmng_645_recv_frame_judge_new(Pipe,MtrIndex,diinfo,&RecvTaskInfo,MeterPrtlData.frameinfo[i].buf,&MeterPrtlData.frameinfo[i].len,&recvmapindex);
                            if(ret>=0)
                            {
                                TASK_FMT_TRACE(Pipe, REC_LOG,"645 DataLen %d sendoad %08x \n",MeterPrtlData.frameinfo[i].len,diinfo->SubOad[ret].value);
                                //TASK_BUF_TRACE( Pipe, REC_LOG,MeterPrtlData.frameinfo[i].buf, MeterPrtlData.frameinfo[i].len);
                                if(MeterPrtlData.PrtlMode==PRTL_07 && MeterPrtlData.frameinfo[i].len<=4 )
                                {
                                    continue;
                                }
                                dataflag = 1;
                                pAmrThreadArg->pMtrAmrInfo[MtrIndex].tmpcsdindex = diinfo->diinfo[ret].csdindex;
                                if(RecvTaskInfo.SchemeType == EVENTPLAN)
                                {
                                    EventRet = taskmng_645_meter_data_save_event(Pipe,&MeterPrtlData.frameinfo[i].buf[4], MeterPrtlData.frameinfo[i].len-4, MtrIndex,&RecvTaskInfo,RecvRunTask);
                                    taskmng_645_map_index_move(Pipe,&RecvTaskInfo, MtrIndex,EventRet);
                                }else
                                {
                                    if(MeterPrtlData.PrtlMode==PRTL_97)//97表全部以实时数据存储
                                    {
                                        taskmng_645_meter_data_save_new(Pipe,recvmapindex,&MeterPrtlData.frameinfo[i].buf[2],MeterPrtlData.frameinfo[i].len-2,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                    }
                                    else
                                    {
                                        if(RecvTaskInfo.AcqInfo.Acqtype ==ACQTYP_NULL)//采集当前数据
                                            taskmng_645_meter_data_save_new(Pipe,recvmapindex,&MeterPrtlData.frameinfo[i].buf[4], MeterPrtlData.frameinfo[i].len-4,MtrIndex,&RecvTaskInfo,RecvRunTask,NULL);
                                        else//07表冻结类数据
                                            taskmng_645_meter_data_save_frz_new(Pipe,recvmapindex,&MeterPrtlData.frameinfo[i].buf[4], MeterPrtlData.frameinfo[i].len-4,MtrIndex,&RecvTaskInfo,RecvRunTask);
                                    }
                                }
                                if(taskmng_645_frz_move_judge_new(Pipe,&RecvTaskInfo, RecvRunTask,MtrIndex,1)<0)
                                {
                                    dataflag = 0;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            pAmrThreadArg->pMtrAmrInfo[MtrIndex].tmpcsdindex = pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex;
                            if(RecvTaskInfo.SchemeType == EVENTPLAN)
                            {
                                taskmng_645_map_index_move(Pipe,&RecvTaskInfo, MtrIndex,EventRet);
                            }
                            if(taskmng_645_frz_move_judge_new(Pipe,&RecvTaskInfo, RecvRunTask,MtrIndex,0)<0)
                            {
                                dataflag = 0;
                                break;
                            }
                        }
                    }
                    //如果回复的全部没数据 就认为抄读失败 后续实时转冻结
                    if(dataflag==0)
                    {
                        taskmng_pn_state_refresh(Pipe,SendIndex);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                        break;
                    }
                    //三相表用负荷曲线抄读方式时不用加
                    if(RecvTaskInfo.CsdTask.RoadTask[0].MainOad.value == 0x50020200 && taskmng_mtr_conntype_get(Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid)!=1 &&
                                pAmrThreadArg->pMtrAmrInfo[MtrIndex].flag645 == 0)
                    {

                    }else
                    {
                        pAmrThreadArg->pMtrAmrInfo[MtrIndex].CSDIndex+=pAmrThreadArg->pMtrAmrInfo[MtrIndex].SigFrameNum;
                    }

                }
#ifdef AREA_JIANGXI
                taskmng_event_power_out_limit(Pipe);
                taskmng_event_zero_wire_current(Pipe, MtrIndex);
#endif
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt=0;
                pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                break;
            }
            default:
                break;
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
                    if(abs(time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>OverTimeCnt)
                    {
                        TASK_FMT_DEBUG( Pipe, REC_LOG, "Queue index %d 超时 PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        taskmng_pn_state_refresh( Pipe, i);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    }
                }
            }
            if(flag==0)
            {
                TASK_FMT_TRACE( Pipe, REC_LOG, "发送队列无效 切换到发送 \n");
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
        }
        //pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        return -1;
        break;
    default:

        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        break;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_task_run_info_reset
* 函数功能: 重置任务运行时参数
* 输入参数: TaskInfo 任务运行时参数

* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void taskmng_task_run_info_reset(CTS_RUN_TASK_T *TaskInfo)
{
    DateTimeBcd_t tmpTime;
    uint8        tmpmin;
    OOP_TI_T          TI;
    TaskInfo->SucNum = 0;
    TaskInfo->MinFrozenFailCnt = 0;
    TaskInfo->DayFrozenFailCnt = 0;
    TaskInfo->MonFrozenFailCnt = 0;
    TaskInfo->MeterIndex =0;
    TaskInfo->CsdIndex=0;
    TaskInfo->SubOadIndex=0;
    TaskInfo->CbNum = 0;
    TaskInfo->IsAlreadyBc=TASKBC_START;
    TaskInfo->RunResult = SCHEME_UNDONE;
    TaskInfo->ThisTurnPeriodStat = AMR_PRE_PERIOD;
    TaskInfo->IsNewTurn = 1;
    memset(TaskInfo->MtrResult,0x00,128);
    TaskInfo->encryptFailCnt = 0;
    TaskInfo->encryptStatus = ENCRYPT_INIT;
    //memset(TaskInfo->CbResult,0xFF,128);//修改的地方
    //memset(TaskInfo->TransResult,0x00,sizeof(OOP_TRANSRES_T)*OOP_MAX_TRANS_INFO);
    memcpy_r((uint8*)&tmpTime,(uint8*)TaskInfo->LastTurnBeginTime,7);
    tmpTime.sec=0;
    //取整，若起始时间的分钟不是15的整数倍，从最近的下一个15分钟开始
    DT_DateTimeBCD2Time(&tmpTime,&TaskInfo->StartTime);
    tmpmin = commfun_BcdToHex(tmpTime.min);
    if(tmpmin%15 != 0)
    {
        tmpmin = (tmpmin/15)*15;
        tmpTime.min = commfun_HexToBcd(tmpmin);
        DT_DateTimeBCD2Time(&tmpTime,&TaskInfo->StartTime);
        //去掉不能整除的分钟后早于了任务开始时间，需要加上一个15分钟
        TI.unit = 1;
        TI.value = 15;
        DT_TimeAddInv(&TaskInfo->StartTime, TI);
    }
}

/*******************************************************************************
* 函数名称:         taskmng_plc_task_queue_add
* 函数功能:         任务链表添加
* 输入参数:         PLCTASK_QUEUE *PlcTaskQueue//任务队列
                    PlcTask                    //当前任务
* 输出参数:         int
* 返 回 值:         > 0  添加成功
                    < 0  添加失败

*******************************************************************************/
int taskmng_plc_task_queue_add ( uint8 CommPipe,MAIN_CYC_STATE_T TaskName )
{
    int             i = 0;
    PLCTASK_QUEUE*  PlcAddTask = NULL;
    PLCTASK_QUEUE*  PlcTaskQueue = NULL;

    PlcTaskQueue = gAmrThreadArg[CommPipe].PlcTaskQueue;

    //判断新插入的任务是否重复
    if ( Remove_RepeatPlcTask ( CommPipe,TaskName ) < 0 )
    {
        return -1;
    }

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
    while ( PlcTask_FunList[i].TaskName != 0xFF )
    {
        if ( PlcTask_FunList[i].TaskName == TaskName )
        {
            PlcAddTask->TaskStat    = 1;
            PlcAddTask->TaskName    = TaskName;
            PlcAddTask->Priority    = PlcTask_FunList[i].Priority;
            PlcAddTask->DelaySec    = PlcTask_FunList[i].DelaySec;
            PlcAddTask->RoutrtCtrl  = PlcTask_FunList[i].RoutrtCtrl;
            switch ( TaskName )
            {
                case MS_FILE_CMP:
                    PlcAddTask->gInnerStat = &gAmrThreadArg[CommPipe].TotalState.MeterFileCmp;
                    break;
                case MS_FILE_MODIFY:
                    PlcAddTask->gInnerStat = &gAmrThreadArg[CommPipe].TotalState.MeterFileModify;
                    break;
                case MS_RELAY_TRANS:
                    PlcAddTask->gInnerStat = &gAmrThreadArg[CommPipe].TotalState.RelayTrans;
                    break;
                case MS_EVENT_REPORT:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_NORMAL_AMR:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_PATCH_AMR:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_BROAD_TIMING:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_RENETWORKING:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_FILE_TRANSFER:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_NODE_VERSION_GET:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_METER_TIME_MANAGER:
                    PlcAddTask->gInnerStat = NULL;
                    break;
                case MS_METER_TIME_DELAY:
                    PlcAddTask->gInnerStat = NULL;
                    break;
#ifdef AREA_HUNAN
                case MS_STA_CURVE_STATUS_GET:
                    PlcAddTask->gInnerStat = NULL;
                    break;
#endif
                default:
                    return -1;
            }
            PlcAddTask->pPlcTaskFun = PlcTask_FunList[i].pPlcTaskFun;      //任务的处理函数
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
* 函数名称: taskmng_all_flag_init
* 函数功能: 标志初始化
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_all_flag_init ( uint8 CommPipe )
{
    gPowerOnFlag[CommPipe] = TRUE;                        //置上电标志

}

/*******************************************************************************
* 函数名称: taskmng_local_addr_set
* 函数功能: //给本地通信APP设置终端通信地址
* 输入参数: pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_local_addr_set(uint8 pipe)
{

    uint8 payload[20]={0};
    uint8 len = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[len],(uint8*)&pipeoad.value,4);
    len+=4;
    memcpy_r(&payload[len],LocalAddr,6);
    len+=6;
    uint16 recvlen;
    uint8 recvbuf[100]={0};
    taskmng_msg_fun_deal(pipe,0x0003,0x003B,len,payload,&recvlen,recvbuf);
    if(recvlen==1&&recvbuf[0]==1)
    {
        TASK_FMT_DEBUG(pipe,COMM_LOG,"设置模块地址成功 \n");
        IsSetAddr[pipe] = 1;
    }

}

/*******************************************************************************
* 函数名称: taskmng_search_meter_status_write
* 函数功能://往数据中心中写搜表状态 0 空闲 1 搜表中
* 输入参数: pipe 端口号
          status 搜表状态 0 空闲 1 搜表中
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_search_meter_status_write(uint8 pipe,uint8 status)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    NOMAL_OAD_T NormalOad ;
    NormalOad.logicId = 0;
    NormalOad.infoNum = 0;
    NormalOad.oad.value = 0x60020A00;
    NormalOad.classtag = 3;
    db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,&status,1);

}

/*******************************************************************************
* 函数名称: taskmng_search_meter_result_delete
* 函数功能: //删除搜表结果
* 输入参数: Pipe 端口号
        searchtype 1表示命令搜表 2表示周期搜表
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_search_meter_result_delete(uint8 pipe,uint8 searchtype)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.classtag = CLASS_DATA_NORMAL;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum=1;
    RecordDel.road.oadCols.oad[0].value = 0x60020200;

    CLEAR_DATA_T cleardata;
    cleardata.bClassTag = FALSE;

    RecordDel.infoNum = 0xffff;
    cleardata.recordOAD = RecordDel;
    int clearchoice = taskmng_search_meter_clear_param_get();
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "搜表结果清空选项 %d \n",clearchoice);
    if(clearchoice == 0) //不请空
    {
        return ;
    }
    if((clearchoice == 1&&searchtype == 2) ||clearchoice == 2||clearchoice == -1)
    {
        if(MeterSearchResultClearFlag ==0)
        {

            db_clear_oad_data(pAmrThreadArg->DBClientHandle,&cleardata);

            //将内存中搜表结果清空
            taskmng_meter_search_result_clear();
            MeterSearchResultClearFlag = 1;

        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_search_meter_exec
* 函数功能: //下发搜表命令
* 输入参数: pipe 端口号
        IOP 0x0034 启动搜表 0x0035停止搜表
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_search_meter_exec(uint8 pipe,uint16 IOP)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 Payload[MAX_MSG_LEN] = {0};
    uint16 index=0;
    uint16 MsgIndex;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    if(IOP == 0X0034)
    {
        time_t timenow = time(NULL);
        pAmrThreadArg->MeterSearch.IsInSearch = 1;
        pAmrThreadArg->MeterSearch.SearchStartTime = timenow;
        #if IS_NEED_ADD_8HOUR
        timenow += 8*60*60;
        #endif
        memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
        index+=4;
        memcpy_r(&Payload[index],(uint8*)&timenow,4);
        index+=4;
        memcpy_r(&Payload[index],(uint8*)&pAmrThreadArg->MeterSearch.SearchTime,4);
        index+=4;
        Payload[index++] = 1;
        Payload[index++] = 1;

        TASK_FMT_DEBUG( pipe, RELAY_LOG, "启动搜表 搜表时长%d len %d payload \n",pAmrThreadArg->MeterSearch.SearchTime,index);

        TASK_BUF_DEBUG( pipe, RELAY_LOG, Payload, index);
    }else if(IOP == 0X0035)
    {
        memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
        index+=4;
        pAmrThreadArg->MeterSearch.IsInSearch = 0;
    }
    taskmng_msg_data_pack(pipe,0x0003,IOP,index,Payload,&MsgIndex);
}

/*******************************************************************************
* 函数名称: taskmng_amr_state_get_from_cco
* 函数功能: 0060查询抄表状态
* 输入参数: pipe
* 输出参数: state 抄表状态
* 返 回 值:
*******************************************************************************/
BOOL taskmng_amr_state_get_from_cco(uint8 pipe,AMR_STATE_T *state)
{
    uint16 IID = 0x0003;
    uint16 IOP = 0x0060;
    uint16 len=4;
    uint8 Buffer[10]={0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    int ret;
    uint16 index = 0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    memcpy_r(Buffer,(uint8*)&pipeOAD.value,4);

    ret = taskmng_msg_fun_deal(pipe,IID,IOP,len,Buffer,&RecvLen,RecvBuf);
    if(ret ==1)
    {
        memcpy_r(&state->lastrecvtime,&RecvBuf[index],4);
        index+=4;
        memcpy_r(&state->amrstate,&RecvBuf[index],4);
        index+=4;
        memcpy_r(&state->metersearchstate,&RecvBuf[index],4);
        index+=4;
        memcpy_r(&state->autoamrstate,&RecvBuf[index],4);
        index+=4;
        memcpy_r(&state->routeclearcnt,&RecvBuf[index],2);
        index+=2;
        memcpy_r(&state->dataclearcnt,&RecvBuf[index],2);
        index+=2;
        memcpy_r(&state->paramclearcnt,&RecvBuf[index],2);
        index+=2;
        memcpy_r(&state->rebootcnt,&RecvBuf[index],2);
        index+=2;
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称: taskmng_search_meter_end_check
* 函数功能: 如果当前的搜表任务持续时间为0 则需要定时查询模块的搜表状态已获取搜表结束标志
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_search_meter_end_check(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    AMR_STATE_T state;
    memset(&state,0x00,sizeof(AMR_STATE_T));
    static int amrstate[PLC_NUM]={-1};
    if(pAmrThreadArg->MeterSearch.IsInSearch==1&&pAmrThreadArg->MeterSearch.SearchTime==0)
    {
        TASK_FMT_DEBUG( Pipe, RELAY_LOG, "当前为不限时间搜表，检查模块搜表状态\n");
        if(TRUE == taskmng_amr_state_get_from_cco(Pipe,&state))
        {
            TASK_FMT_DEBUG( Pipe, RELAY_LOG, "amrstate %d 获取到当前模块搜表状态为 %d \n",amrstate[Pipe],state.metersearchstate);
            if(amrstate[Pipe]==1 && state.metersearchstate == 0)
            {
                TASK_FMT_DEBUG( Pipe, RELAY_LOG, "CCO搜表结束，写搜表结束状态 \n");
                //taskmng_router_ctrl_set(Pipe,RCS_RESUME);
                taskmng_search_meter_status_write(Pipe,0);
                pAmrThreadArg->MeterSearch.IsInSearch = 0;
                pAmrThreadArg->IsNeedSearch=0;
                MeterSearchResultClearFlag = 0;
            }
            else
            {
                amrstate[Pipe] = state.metersearchstate;
            }
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_renetworking_count_clear
* 函数功能: //过日清空每个表的重组网次数 一天只能做一次
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_renetworking_count_clear(uint8 pipe)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    MtrAmrInfo_T *pMeterInfo = NULL;
    uint16 i;
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        pMeterInfo = &pAmrThreadArg->pMtrAmrInfo[i];
        pMeterInfo->ReNetworkingCount = 0;
    }
}

/**********************************************************************
名称 taskmng_645_flag_clear
功能 每天中午12点时重置flag645
参数  无
返回值:
***********************************************************************/
void taskmng_645_flag_clear(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index ;
    for(index=0;index<pAmrThreadArg->TmnAmrInfo.MtrCnt;index++)
    {
        pAmrThreadArg->pMtrAmrInfo[index].flag645 = 0;
    }
}

/*******************************************************************************
* 函数名称: taskmng_plc_task_creat
* 函数功能: 任务队列产生判断
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_plc_task_creat(uint8 Pipe)
{
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    int ret;
    //上电任务
    if(gPowerOnFlag[Pipe]==TRUE)
    {
        gPowerOnFlag[Pipe]=FALSE;
        //上电产生档案比对任务
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "开始产生档案同步任务 ！\n");
        taskmng_plc_task_queue_add(Pipe,MS_FILE_CMP);
#ifdef AREA_HUNAN
        //上电需要查询下
        taskmng_plc_task_queue_add(Pipe,MS_STA_CURVE_STATUS_GET);
#endif
        Init_ReadMtr(Pipe);
        taskmng_plc_pipe_set(Pipe,1);

    }
    if(gPastTimeFlag[Pipe].BitSect.Second==TRUE)
    {
        //改为5分钟查一次
        if(pAmrThreadArg->IsReportHPLCInfo ==0 && time(NULL)-pAmrThreadArg->HPLCInfoSendTime >60*5)
        {
            taskmng_HPLC_info_get(Pipe);
            //测试时暂时注释
            //taskmng_plc_task_queue_add(Pipe,MS_NODE_VERSION_GET);
        }
    }
    if(IsSetAddr[Pipe]==0&&taskmng_addr_isvalid_check(LocalAddr,6)>0)
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "设置模块地址 ！\n");
        taskmng_local_addr_set(Pipe);
    }
#ifdef AREA_FUJIAN
    if(IsSetGatherMode[Pipe] == 0 )
    {
        gGatherMode = 0;

        TASK_FMT_DEBUG(Pipe, COMM_LOG, "设置通信协议模式 ！\n");
        fujian_gather_mode_set(Pipe);

    }
#endif

    //档案同步任务
    if(gMeterFileCmpFlag[Pipe] == TRUE)
    {
        gMeterFileCmpFlag[Pipe] = FALSE;
        taskmng_plc_task_queue_add(Pipe,MS_FILE_CMP);
    }
    //档案变更 需要同步
    if(pAmrThreadArg->MtrParaModifyFlag!=METER_FILE_NULL)
    {
        taskmng_plc_task_queue_add(Pipe,MS_FILE_CMP);
        if(pAmrThreadArg->MtrParaModifyFlag == METER_FILE_MODIFY)
        {
            gMeterFileModifyType[Pipe] = MODEFY_F10;
        }
        if(pAmrThreadArg->MtrParaModifyFlag == METER_FILE_CLEAR)
        {
            gMeterFileModifyType[Pipe] = MODEFY_CLEAR;
        }
        pAmrThreadArg->MtrParaModifyFlag=METER_FILE_NULL;
#ifdef AREA_HUNAN
        //档案变更时查询
        taskmng_plc_task_queue_add(Pipe,MS_STA_CURVE_STATUS_GET);
#endif
    }
    if(gMeterFileModifyFlag[Pipe] == TRUE)
    {
        gMeterFileModifyFlag[Pipe] = FALSE;
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "创建档案修改任务 ！\n");
        taskmng_plc_task_queue_add(Pipe,MS_FILE_MODIFY);
    }
    //每分钟创建正常抄表任务
    if(gPastTimeFlag[Pipe].BitSect.Minute==TRUE)
    {
        //TASK_FMT_TRACE( Pipe, REC_LOG, "过分创建抄表任务 ！\n");
        taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR);
#if PATCH_FUN
        taskmng_plc_task_queue_add(Pipe,MS_PATCH_AMR);
#endif
#ifdef AREA_ZHEJIANG
        if(TRUE == taskmng_meter_file_cmp_check(Pipe))
        {
            TASK_FMT_DEBUG( Pipe, COMM_LOG, "浙江到点产生档案同步任务 ！\n");
            taskmng_plc_task_queue_add(Pipe,MS_FILE_MODIFY);
        }
#endif
        if(pAmrThreadArg->meterTimeManager != NULL)
        {
            taskmng_plc_task_queue_add(Pipe,MS_METER_TIME_MANAGER);
        }
        if(pAmrThreadArg->meterTimeDelay != NULL)
        {
            taskmng_plc_task_queue_add(Pipe,MS_METER_TIME_DELAY);
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
           taskmng_plc_task_queue_add(Pipe,MS_RELAY_TRANS);
       }
    }
    if(gPastTimeFlag[Pipe].BitSect.Second==TRUE)
    {
        //判断搜表任务
        if(pAmrThreadArg->MeterSearch.IsInSearch==0)
        {
            if(pAmrThreadArg->IsNeedSearch==1||pAmrThreadArg->IsNeedSearch==2)
            {
                //下发搜表命令
                taskmng_search_meter_status_write(Pipe,1);
                taskmng_router_ctrl_set(Pipe,RCS_PAUSE);
                taskmng_search_meter_result_delete(Pipe,pAmrThreadArg->IsNeedSearch);
                taskmng_search_meter_exec(Pipe,0X0034);

            }
        }else
        {
            if(pAmrThreadArg->MeterSearch.SearchTime!=0 &&(time(NULL)-pAmrThreadArg->MeterSearch.SearchStartTime>pAmrThreadArg->MeterSearch.SearchTime*60))
            {
                TASK_FMT_DEBUG( Pipe, RELAY_LOG, "搜表持续时间到 下发结束搜表命令\n");
                taskmng_search_meter_exec(Pipe,0X0035);
                //taskmng_router_ctrl_set(Pipe,RCS_RESUME);
                taskmng_search_meter_status_write(Pipe,0);
                pAmrThreadArg->IsNeedSearch=0;
                MeterSearchResultClearFlag = 0;
            }
        }
#ifdef AREA_HUNAN
        //判断是否需要重新同步模块曲线配置
        if(taskmng_curve_sync_flag_get(Pipe)==1)
        {
            taskmng_curve_sync_flag_set(Pipe,0);
            taskmng_curve_sync_check(Pipe);
        }
#endif
    }
    if(gPastTimeFlag[Pipe].BitSect.Minute==TRUE)
    {
        taskmng_search_meter_end_check(Pipe);
    }
#ifndef AREA_SHANDONG    /* 山东地区连续9个任务周期不通，重试次数改为1次，不对该表进行重组网 */
    if(gPastTimeFlag[Pipe].BitSect.Hour == TRUE && gCurBinTime[Pipe].Hour%3==0)
    {
        TASK_FMT_DEBUG( Pipe, RELAY_LOG, "产生重组网任务\n");
        taskmng_plc_task_queue_add(Pipe,MS_RENETWORKING);
    }
#endif
    if(gPastTimeFlag[Pipe].BitSect.Day== TRUE)
    {
        //过日的时候要清下事件上报总次数
        pAmrThreadArg->EventReport.AllCount = 0;
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "过日产生档案同步任务 ！\n");
        #if (!defined AREA_JIANGSU)
        //适配跑分 档案太多时过日不同步档案
        if(pAmrThreadArg->TmnAmrInfo.MtrCnt < 200)
            taskmng_plc_task_queue_add(Pipe,MS_FILE_CMP);
        #endif
        taskmng_renetworking_count_clear(Pipe);
#ifdef AREA_HUNAN
        //过日查询
        taskmng_plc_task_queue_add(Pipe,MS_STA_CURVE_STATUS_GET);
#endif
    }
#if IS_NEED_CLEAR_645_FLAG
    //湖南现场出现07表负荷曲线回否认 且补抄也回否认 为保证丢点少 转实时后不再切回来
    if(gPastTimeFlag[Pipe].BitSect.Hour == TRUE && gCurBinTime[Pipe].Hour==12)
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "taskmng_645_flag_clear！\n");
        taskmng_645_flag_clear(Pipe);
    }
#endif
}

/*******************************************************************************
* 函数名称: taskmng_3762_frame_pack
* 函数功能:组3762报文
* 输入参数:appdata 3762信息

* 输出参数: frame 组的3762报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_3762_frame_pack(AppData_T appdata,uint8 *frame)
{
    uint16 index = 0;
    uint8 buf[200] = {0};
    uint8 CheckSum = 0;
    uint8 i;
    buf[index++] = 0x68;
    index+=2;   //暂时跳过长度域
    buf[index++] = 0x43; //控制域
    index+=5;
    buf[index++] = appdata.RInfo[5];    //信息域前5个字节0 第6个为帧序号
    //不需要地址域
    buf[index++] = appdata.AFN;
    buf[index++] = 1 << ((appdata.Fn-1) % 8);
    buf[index++] = (appdata.Fn-1) / 8;
    if(appdata.DataUnitLen>0)
    {
        memcpy(&buf[index],appdata.DataUnitBuff,appdata.DataUnitLen);
        index+=appdata.DataUnitLen;
    }
    for (i =3; i < index; i++)
    {
        CheckSum += buf[i];
    }
    buf[index++] = CheckSum;
    buf[index++] = 0x16;
    memcpy(&buf[1],&index,2);
    memcpy(frame,buf,index);
    return index;
}


/*******************************************************************************
* 函数名称: taskmng_node_version_info_get_send
* 函数功能:获取节点版本信息组帧发送
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 0 所有发送完毕，任务结束 1 切换接收态
*******************************************************************************/

int taskmng_node_version_info_get_send(uint8 pipe)
{
    GET_NODE_VERSION_INFO_T *nodeinfo = &gAmrThreadArg[pipe].NodeVersionInfo;
    uint8 payload[2000] = {0};
    uint16 index = 0;
    uint8 frame[200] = {0};
    uint16 framelen;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    uint16 overtime = 30;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    if(nodeinfo->FailCnt>=3)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_node_version_info_get_send 失败次数过多 任务结束\n");
        return 0;
    }

    appdata.AFN = 0x10;
    appdata.Fn = 104;
    appdata.DataUnitLen = 3;
    memcpy(appdata.DataUnitBuff,&nodeinfo->BeginIndex,2);
    appdata.DataUnitBuff[2] = MAX_NODE_VERSION_INFO_GET_NUM;
    framelen = taskmng_3762_frame_pack(appdata,frame);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "组的3762报文\n");
    TASK_BUF_DEBUG( pipe, RELAY_LOG,frame,framelen);

    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    memcpy_r(&payload[index],(uint8*)&overtime,2);
    index+=2;
    payload[index++] = framelen;
    memcpy(&payload[index],frame,framelen);
    index+=framelen;

    taskmng_msg_data_pack(pipe,0x0003,0x0033,index,payload,&nodeinfo->SendSeq);
    return 1;

}

/*******************************************************************************
* 函数名称: taskmng_node_version_info_get_recv
* 函数功能:收到回码后的处理
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值: 0 数据处理完毕 -1 回码为空 1 收到个数小于15个表示所有节点查询完毕
*******************************************************************************/

int taskmng_node_version_info_get_recv(uint8 pipe,uint8 *buf,uint16 buflen)
{
    GET_NODE_VERSION_INFO_T *nodeinfo = &gAmrThreadArg[pipe].NodeVersionInfo;
    int ret=0;
    uint16 frameend;
    uint16 framelen;
    uint8 lenoffset;
    uint16 totalnum;
    uint8 i,num;
    uint16 index = 0;
    OOP_NODEVERSIONINFO_T  nodeversioninfo;
    AppData_T appdata;
    memset(&appdata,0x00,sizeof(AppData_T));
    OOP_OAD_U oad = {0xF2090C00};
    uint16 infonum;
    uint8 addr[6]={0};
    framelen = get_len_offset(buf, &lenoffset);
    if(framelen == 0)
    {
        return -1;
    }
    ret = taskmng_frame_3762_format_check(framelen,buf+lenoffset,&frameend);
    if(ret == 0 )
    {
        //识别出3762 开始解析
        ret = taskmng_frame_3762_unpack(framelen,buf+lenoffset,&appdata);
        if(ret == 0&&appdata.AFN ==0x10&&appdata.Fn ==104)
        {
            //对3762数据域进行解析
            memcpy(&totalnum,appdata.DataUnitBuff,2);
            index+=2;
            num = appdata.DataUnitBuff[index++];
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "节点总数量%d      本次数量    %d  \n",totalnum,num);
            if(appdata.DataUnitLen<(3+num*(6+9)))
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据长度不对 \n");
                return -1;
            }

            for(i=0;i<num;i++)
            {
                memset(&nodeversioninfo,0x00,sizeof(OOP_NODEVERSIONINFO_T));
                //地址
                memcpy(addr,&appdata.DataUnitBuff[index],6);
                memcpy_r(nodeversioninfo.addr.nValue,&appdata.DataUnitBuff[index],6);
                index+=6;
                //版本号
                nodeversioninfo.wVer = Bcd2Hex(appdata.DataUnitBuff[index+1])*100+Bcd2Hex(appdata.DataUnitBuff[index]);
                index+=2;
                //日
                nodeversioninfo.dtVer.mday = Bcd2Hex(appdata.DataUnitBuff[index++]);
                //月
                nodeversioninfo.dtVer.month = Bcd2Hex(appdata.DataUnitBuff[index++]);
                //年
                nodeversioninfo.dtVer.year = Bcd2Hex(appdata.DataUnitBuff[index++])+2000;
                //模块厂商代码
                nodeversioninfo.factory.nNum = 2;
                memcpy_r(nodeversioninfo.factory.value,&appdata.DataUnitBuff[index],2);
                index+=2;
                //芯片代码
                nodeversioninfo.ic.nNum = 2;
                memcpy_r(nodeversioninfo.ic.value,&appdata.DataUnitBuff[index],2);
                index+=2;

                if(nodeinfo->BeginIndex ==0 && i ==0)
                {
                    nodeversioninfo.type = 0;
                    infonum = 0;
                }else
                {
                    nodeversioninfo.type = 1;
                    infonum = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
                    if(infonum==0)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "无法在档案中找到该地址 \n");
                        TASK_BUF_DEBUG( pipe, RELAY_LOG, addr,6);
                        continue;
                    }
                }

                taskmng_normal_data_write_pipe(pipe,(uint8*)&nodeversioninfo,sizeof(OOP_NODEVERSIONINFO_T),oad,infonum,LOGIC_ID_YC);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "节点版本信息结果写入 %d\n",infonum);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "ccosta地址 0x%02x%02x%02x%02x%02x%02x \n",nodeversioninfo.addr.nValue[5],nodeversioninfo.addr.nValue[4],nodeversioninfo.addr.nValue[3],nodeversioninfo.addr.nValue[2],nodeversioninfo.addr.nValue[1],nodeversioninfo.addr.nValue[0]);
            }
            nodeinfo->BeginIndex+=num;
            if(num<MAX_NODE_VERSION_INFO_GET_NUM)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "所有节点查询完毕 \n");
                return 1;
            }
            return 0;
        }
    }

    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_node_version_info_get
* 函数功能:根据F2090600周期获取节点版本信息，自组3762 10F104查询载波模块节点信息写入F2090C00
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值: 1 处理完成 -1 未完成
*******************************************************************************/
int taskmng_node_version_info_get(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    GET_NODE_VERSION_INFO_T *nodeinfo = &pAmrThreadArg->NodeVersionInfo;
    OOP_OAD_U oad = {0xF2090C00};
    int ret;
    static time_t sendtime[PLC_NUM];
    uint16 RecvLen;
    uint8 RecvBuf[2000]={0};
    MESSAGE_INFO_T* MsgInfo = NULL;
    switch(pAmrThreadArg->TotalState.GetNodeVersionInfoState)
    {
    case INNER_RTS_IDLE:
    {
        memset(nodeinfo,0x00,sizeof(GET_NODE_VERSION_INFO_T));
        //删除F2090C00信息
        taskmng_normal_data_delete_pipe(pipe,0xffff,oad);
        pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_SEND;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret = taskmng_node_version_info_get_send(pipe);
        if(ret == 1)
        {
            sendtime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_RECV;
        }else
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_node_version_info_get 任务结束  \n");
            pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_IDLE;
            return 1;
        }
    }
        break;
    case INNER_RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            if(MsgInfo->index!=nodeinfo->SendSeq)
            {
                break;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_node_version_info_get 收到消息  \n");
            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
            ret = taskmng_node_version_info_get_recv(pipe,MsgInfo->playload, MsgInfo->msglen);
            if(ret ==0)
            {
                pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_SEND;
            }else if(ret == -1)
            {
                pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_RETRY;
            }else
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_node_version_info_get 任务结束  \n");
                pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_IDLE;
                return 1;
            }
        }else
        {
            if(time(NULL)-sendtime[pipe] >30 )
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "起始序号%d 超时 \n",nodeinfo->BeginIndex);

                pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        nodeinfo->FailCnt++;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "失败次数 %d \n",nodeinfo->FailCnt);
        pAmrThreadArg->TotalState.GetNodeVersionInfoState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }

    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_plc_task_run
* 函数功能: 任务执行处理
* 输入参数:
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_plc_task_run ( uint8 CommPipe )
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
            if ( PlcTask_RunLink->DelaySec != 0 )
            {
                //不用路由控制 直接到延时或者任务执行
                //TASK_FMT_TRACE(CommPipe,COMM_LOG, " 路由控制 : %d  \n",PlcTask_RunLink->RoutrtCtrl );
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERCTRL_DELAY;
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
                if(Ret==2 && PlcTask_RunLink->TaskName == MS_NORMAL_AMR)
                {
                    TASK_FMT_TRACE(CommPipe,REC_LOG, "当前任务抄表完毕，继续下一个任务  \n",PlcTask_RunLink->TaskName );
                    break;
                }
                if(Ret==2 && PlcTask_RunLink->TaskName == MS_PATCH_AMR)
                {
                    taskmng_plc_task_queue_add(CommPipe,MS_NORMAL_AMR);
                    return;
                }

//                if ( PlcTask_RunLink->RoutrtCtrl == RCS_PAUSE )
//                {
//                    gDelaySec[CommPipe] = 0;//路由直接恢复，原来默认60s
//                    PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERRECOVER_DELAY;
//                }
//                else
                {
                    pTmpLink = PlcTask_RunLink->next;
                    PlcTask_RunLink->TaskStat = 0;
                    PlcTask_RunLink = pTmpLink;

                }
            }
            else
                return;  //抄表过程中添加的事件上报任务如果不return到下面会被删除

            break;

        case ROUTERRECOVER_DELAY:                //路由恢复延时操作
            if ( PlcTask_RunLink->RoutrtCtrl == RCS_PAUSE ) //如果当前任务时暂停任务
            {
                pTmpLink = PlcTask_RunLink->next;
                if ( pTmpLink != NULL && ( pTmpLink->TaskStat > 0 ) ) //判断下一个任务是否一样需要路由暂停的操作
                {
                    if ( pTmpLink->RoutrtCtrl == RCS_PAUSE )
                    {
                        if ( pTmpLink->RoutrtCtrl == PlcTask_RunLink->RoutrtCtrl )
                        {
                            pTmpLink->PlcTaskDeal_Stat = FUN_DEAL;
                        }
                        PlcTask_RunLink->TaskStat = 0;
                        PlcTask_RunLink = pTmpLink;
                        gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
                        return;
                    }
                }
            }

            Ret = taskmng_delay_deal ( CommPipe,&gDelaySec[CommPipe] ); //延时处理
            if ( Ret > 0 )
            {
                PlcTask_RunLink->PlcTaskDeal_Stat = ROUTERRECOVER_DEAL;
            }
            break;

        case ROUTERRECOVER_DEAL:                //路由恢复操作
            TASK_FMT_TRACE(CommPipe,COMM_LOG, "路由恢复操作 \n" );
            Ret = taskmng_router_ctrl_set ( CommPipe,RCS_RESUME );
            if ( Ret > 0 )
            {
                PlcTask_RunLink->TaskStat = 0;
                pTmpLink = PlcTask_RunLink->next;
                PlcTask_RunLink = pTmpLink;
            }
            break;
    }
    gAmrThreadArg[CommPipe].PlcTask_RunLink = PlcTask_RunLink;
}

/*******************************************************************************
* 函数名称: taskmng_router_ctrl_set
* 函数功能: 设置路由控制
* 输入参数: CtrlCmd             控制命令(1:重启 / 2:暂停 / 3:恢复)
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_router_ctrl_set ( uint8 CommPipe,ROUTER_CTRL_STATE_T CtrlCmd )
{
    //HPLC暂不需要进行路由控制
    return 1;
    uint16 IID = 0x0003;
    uint16 IOP = 0;
    uint16 len=4;
    uint8 Buffer[10]={0};
    uint16 RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(CommPipe);
    switch(CtrlCmd)
    {
    case RCS_PAUSE:
        IOP = 0x0040;
        break;
//    case RCS_RESUME:
//        IOP = 0x0041;
//        break;
//    case RCS_RESTART:
//        IOP = 0x0042;
//        break;
    default:
       return 1;  //目前只发暂停
    }
    memcpy_r(Buffer,(uint8*)&pipeOAD.value,4);

    return taskmng_msg_fun_deal(CommPipe,IID,IOP,len,Buffer,&RecvLen,RecvBuf);
}
/*******************************************************************************
* 函数名称:         taskmng_msg_fun_deal
* 函数功能:         消息收发函数
* 输入参数:
* 输出参数:
* 返 回 值:         > 0 接收成功
                    < 0 接收失败

*******************************************************************************/
int taskmng_msg_fun_deal(uint8 pipe,uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,uint16 *RecvLoadLen,uint8 *RecvPayLoad)
{
    ACTIVE_COMM_STATE_T         Stat = ACT_COMM_IDLE;
    uint16                      SendLen = 0;
    static time_t               CommBeginTime[PLC_NUM];                      //通讯开始时间
    uint8                       RecvBuf[MAX_MSG_LEN];
    int                         RecvLen;
    //uint16                      RecvIID,RecvIOP;
    //uint16                      index=0;
    //uint8                       lenoffset;
    int8                       RecvResult =0 ;
    uint8                       RetryCnt=0;
    MESSAGE_INFO_T *MsgRecv = NULL;
    uint16 MsgIndex;
    while(1)
    {
        switch(Stat)
        {
        case ACT_COMM_IDLE:
            Stat = ACT_COMM_SEND;
            break;
        case ACT_COMM_SEND:
            SendLen = taskmng_msg_data_pack(pipe,IID,IOP,len,PayLoad,&MsgIndex);
            if(SendLen >0)
            {
                CommBeginTime[pipe] = time(NULL);
                Stat = ACT_COMM_RECV;
            }
            break;
        case ACT_COMM_RECV:

            RecvLen = taskmng_plc_queue_read(pAmrRecvDown[pipe], RecvBuf);
            if(RecvLen>0)
            {
                TASK_FMT_TRACE( pipe, COMM_LOG, "收到本地回复 \n");

                MsgRecv = (MESSAGE_INFO_T *)RecvBuf;
                TASK_FMT_TRACE( pipe, COMM_LOG, "IID 0x%04x IOP 0x%04x \n",MsgRecv->IID,MsgRecv->IOP);
                if(IID==MsgRecv->IID && IOP==MsgRecv->IOP && MsgIndex==MsgRecv->index)
                {
                    *RecvLoadLen = MsgRecv->msglen;
                    memcpy(RecvPayLoad,MsgRecv->playload,*RecvLoadLen);

                    TASK_BUF_TRACE( pipe, COMM_LOG, RecvPayLoad, *RecvLoadLen);
                    return 1;
                }else
                {
                    RecvResult = -1;
                }
            }else
                RecvResult = -1;
            if(RecvResult == -1)
            {
                if(abs(time(NULL)-CommBeginTime[pipe])>MAX_MSG_DELAY_TIME)
                {
                    RetryCnt++;
                    Stat = ACT_COMM_RETRY;
                    TASK_FMT_TRACE( pipe, COMM_LOG, "超时重试RetryCnt %d \n",RetryCnt);
                }
            }

            break;
        case ACT_COMM_RETRY:
            if(RetryCnt>3)
            {
                Stat = ACT_COMM_IDLE;
                return -1;
            }
            Stat = ACT_COMM_SEND;
            break;
        default:
            break;
        }
        usleep(20000);
    }
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_delay_deal
* 函数功能: 延时处理
* 输入参数: 无
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
int taskmng_delay_deal ( uint8 CommPipe,uint8* PlcTaskDelaySec )
{

    if ( ( *PlcTaskDelaySec ) == 0 )
    {
        return 1;
    }


    if ( gPastTimeFlag[CommPipe].BitSect.Second )
    {
        ( *PlcTaskDelaySec )--;
        PIPEPRINT ( CommPipe, "倒计时%d\n",*PlcTaskDelaySec );
    }

    return 0;
}

/*******************************************************************************
* 函数名称: PackFrameData
* 函数功能: 组消息
* 输入参数:
* 输出参数: 无
* 返 回 值: 数据帧长度
*******************************************************************************/
uint16 taskmng_msg_data_pack(uint8 pipe,uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,uint16* MsgIndex)
{

    MESSAGE_INFO_T *MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+len);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+len);

    MsgSend->rpm = 1;
    MsgSend->priority = 1;
    MsgSend->index = GetMsgSeq();
    *MsgIndex = MsgSend->index;
    MsgSend->label = pipe;
    MsgSend->sourlen = strlen(DownName);
    memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
    MsgSend->destlen = strlen(CCOName);
    memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
    TASK_FMT_TRACE( pipe, COMM_LOG, "SourLen %d SourAddr %s DestLen %d DestAddr %s \n",MsgSend->sourlen,MsgSend->souraddr,MsgSend->destlen,MsgSend->destaddr);
    MsgSend->IID = IID;
    MsgSend->IOP = IOP;
    MsgSend->msglen = len;
    memcpy(MsgSend->playload,PayLoad,len);
    TASK_FMT_TRACE( pipe, COMM_LOG, "Send IID=0X%04x IOP=0x%04x MsgIndex %d \n",IID,IOP,MsgSend->index);

    TASK_BUF_TRACE( pipe, COMM_LOG, PayLoad, len);
    taskmng_plc_queue_write ( pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+len );
    free(MsgSend);
    return sizeof(MESSAGE_INFO_T)+len;

}

/*******************************************************************************
* 函数名称: taskmng_plc_queue_read
* 函数功能: 读队列
* 输入参数：pLockQueue          锁缓冲区结构指针
*           buf                 读数据缓冲区，将队列中的数据取出放入该缓冲区
* 输出参数：plockqueue          队列中的数据
* 内部处理: 判队列中是否有数据，有则从队列中取出数据放入buf中，每执行一次，取一个数据
* 返 回 值: len                 读到的数据长度
*******************************************************************************/
int32 taskmng_plc_queue_read(LockQueue_T *pLockQueue, uint8 *buf)
{
    int32 len = 0;
    uint8 *pdata_buf;

    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);

    //若队列中有数据，则取到buf中，解开锁
    if ((pLockQueue->QueueHead != pLockQueue->QueueTail)
        || ((pLockQueue->QueueHead == pLockQueue->QueueTail)
            && (pLockQueue->Queue[pLockQueue->QueueHead].QueueLen != 0)))
    {
        pdata_buf = pLockQueue->Queue[pLockQueue->QueueHead].QueueBuf;
        len = pLockQueue->Queue[pLockQueue->QueueHead].QueueLen;

        //取队列中的数据
        memcpy(buf, pdata_buf, len);

        //清数据
        memset(pdata_buf, 0, len);
        pLockQueue->Queue[pLockQueue->QueueHead].QueueLen = 0;

        //队列头向后移动一位
        pLockQueue->QueueHead= (pLockQueue->QueueHead + 1) % (MAX_QUEUE_CNT);
    }

    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return len;
}

/*******************************************************************************
* 函数名称: taskmng_plc_queue_write
* 函数功能: 写队列
* 输入参数：plockqueue          锁缓冲区结构指针
            buf                 需要写入队列的数据
            buf_len             数据长度
* 输出参数：无
* 内部处理: 若队列不满，将buf中的数据放入队列中；若队列满，丢弃该数据
* 返 回 值: >0                  成功
            =0                  失败
*******************************************************************************/
int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len)
{
    int32 flags = 0;
        //加上防错 防止越界
    if(buf_len >MAX_QUEUE_SIZE )
        buf_len = MAX_QUEUE_SIZE;
    //锁缓冲区
    pthread_mutex_lock(&pLockQueue->QueueLock);
    //判队列中是否还有空的位置
    if (pLockQueue->Queue[pLockQueue->QueueTail].QueueLen == 0)
    {
        pLockQueue->Queue[pLockQueue->QueueTail].QueueLen = buf_len;
        memcpy(pLockQueue->Queue[pLockQueue->QueueTail].QueueBuf, buf, buf_len);
        flags = 1;
        pLockQueue->QueueTail = (pLockQueue->QueueTail + 1) % (MAX_QUEUE_CNT);
    }

    //解缓冲区
    pthread_mutex_unlock(&pLockQueue->QueueLock);

    return flags;
}

/*******************************************************************************
* 函数名称: taskmng_HPLC_info_get
* 函数功能:查询HPLC信息
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_HPLC_info_get(uint8 pipe)
{
    //return 1;
    uint8 i;
    uint8 payload[10] = {0};
    uint16 index = 0;
    //uint8 RecvBuf[200]={0};
    //uint16 RecvLen;
    uint16 MsgIndex;
    OOP_OAD_U pipeOAD ;
    pipeOAD = taskmng_pipe_oad_get(pipe);
    //TASK_FMT_DEBUG( pipe, RELAY_LOG, "删除上一轮查询的HPLC信息  \n");
    //Timetaskmng_HPLC_info_getSend[pipe] = time(NULL);
    //DeleteHPLCInfo(pipe);
    taskmng_net_topology_seq_reset();
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    pAmrThreadArg->HPLCInfoSendTime = time(NULL);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "查询HPLC信息发送时间 %ld \n",pAmrThreadArg->HPLCInfoSendTime);
    //把8和9都加上
    for(i=1;i<10;i++)
    {
        memset(payload,0x00,sizeof(payload));
        index = 0;
        memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
        index+=4;
        payload[index++] = i;
        //taskmng_msg_fun_deal(pipe,0x0003,0x0050,index,payload,&RecvLen,RecvBuf);
        taskmng_msg_data_pack(pipe,0x0003,0x0050,index,payload,&MsgIndex);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "查询HPLC信息 信息类型 %d \n",i);
    }
    //0061查询抄表模块信息 模块版本通过8获取 暂时不查了
//    memset(payload,0x00,sizeof(payload));
//    index = 0;
//    memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
//    index+=4;
//    //memset(RecvBuf,0x00,sizeof(RecvBuf));
//
//    taskmng_msg_data_pack(pipe,0x0003,0x0061,index,payload,&MsgIndex);
    //0061查询可能会不回，改为异步接口
//    if(taskmng_msg_fun_deal(pipe,0x0003,0x0061,index,payload,&RecvLen,RecvBuf)>0)
//    {
//        TASK_FMT_DEBUG( pipe, RELAY_LOG, "获取到抄表模块信息\n");
//        TASK_BUF_DEBUG( pipe, RELAY_LOG, RecvBuf,RecvLen);
//        taskmng_msg_0061_unpack(pipe,RecvBuf,RecvLen);
//    }
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_HPLC_info_get
* 函数功能:查询HPLC信息
* 输入参数:pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_HPLC_node_info_get(uint8 pipe)
{
    //return 1;

    uint8 payload[10] = {0};
    uint16 index = 0;
    uint8 RecvBuf[200]={0};
    uint16 RecvLen;
    OOP_OAD_U pipeOAD ;
    pipeOAD = taskmng_pipe_oad_get(pipe);

    //taskmng_net_topology_seq_reset();
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    pAmrThreadArg->HPLCInfoSendTime = time(NULL);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "查询HPLC信息发送时间 %ld \n",pAmrThreadArg->HPLCInfoSendTime);

    memset(payload,0x00,sizeof(payload));
    index = 0;
    memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    payload[index++] = 7;
    taskmng_msg_fun_deal(pipe,0x0003,0x0050,index,payload,&RecvLen,RecvBuf);
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "查询HPLC信息 信息类型 %d \n",7);

    return 1;
}


/*******************************************************************************
* 函数名称: taskmng_normal_data_write_pipe
* 函数功能:写普通数据到数据中心
* 输入参数:pipe 端口号
        DataBuf 数据内容
        DataLen 数据长度
        Oad
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_normal_data_write_pipe(uint8 pipe,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad,uint16 infonum,uint8 logicid)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //uint8 log[2000]={0};
    int ret;
    NOMAL_OAD_T NomalOad;
    NomalOad.logicId = logicid;
    NomalOad.oad =Oad;
    NomalOad.classtag = 3;
    NomalOad.infoNum = infonum;
    //TASK_FMT_TRACE( pipe, RELAY_LOG,"写普通数据到数据中心oad %08x infonum %d len%d \n",Oad.value,NomalOad.infoNum,DataLen);

    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NomalOad,DataBuf,DataLen);
    if(ret !=0)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG,"普通数据写入数据库失败 ret %d oad %08x\n",ret,Oad.value);
    }
}


/*******************************************************************************
* 函数名称: taskmng_normal_data_read_pipe
* 函数功能:读普通数据
* 输入参数:pipe 端口号
        DataBuf 数据内容
        DataLen 数据长度
        Oad
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_normal_data_read_pipe(uint8 pipe,OOP_OAD_U Oad,uint16 infonum,uint8 logicid,uint8* DataBuf,uint16 *DataLen)
{
    int   ret;
    uint32 Len;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad = Oad;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,MAX_MSG_LEN,DataBuf,&Len);
    *DataLen = Len;
    return ret;
}


/*******************************************************************************
* 函数名称: taskmng_normal_data_delete_pipe
* 函数功能:删除普通数据
* 输入参数:pipe 端口号
        infoNum 信息点号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_normal_data_delete_pipe(uint8 pipe,uint16 infoNum,OOP_OAD_U Oad)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 0;
    RecordDel.classtag = CLASS_DATA_NORMAL;
    RecordDel.infoNum = infoNum;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum= 1;
    RecordDel.road.oadCols.oad[0] = Oad;
    CLEAR_DATA_T clear;
    clear.recordOAD = RecordDel;
    clear.bClassTag = FALSE;
    int ret;
    ret = db_clear_oad_data(pAmrThreadArg->DBClientHandle,&clear);
    if(ret==0)
    {
        TASK_FMT_TRACE(pipe, RELAY_LOG, "删除成功 infonum %d oad %08x \n",infoNum,Oad.value);
    }else
    {
        TASK_FMT_TRACE(pipe, RELAY_LOG,"删除失败 infonum %d oad %08x \n",infoNum,Oad.value);
    }

}

/*******************************************************************************
* 函数名称: taskmng_update_cycle_check
* 函数功能:过分检查是否到了周期查询HPLC信息时间
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_update_cycle_check(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint32 updatecycle=0;
    uint8 setflag = 0;
    pthread_mutex_lock(&UpdateCycle.Lock);
    updatecycle = UpdateCycle.Ti;
    setflag = UpdateCycle.isSet;
    pthread_mutex_unlock(&UpdateCycle.Lock);
    //设置周期为0时不查询HPLC信息
    if(updatecycle == 0)
        return;
    //如果档案个数超过200 则过日不查询HPLC信息
    if(setflag ==0 && pAmrThreadArg->TmnAmrInfo.MtrCnt > 200)
        return;
    //当日的分钟数
    uint16 curmin = gCurBinTime[pipe].Hour*60+gCurBinTime[pipe].Minute;

    if( curmin%updatecycle == 0)
    {
        taskmng_HPLC_info_get(pipe);
        //taskmng_plc_task_queue_add(pipe,MS_NODE_VERSION_GET);
    }
}

/*******************************************************************************
* 函数名称: taskmng_mtr_in_queue_is_exist
* 函数功能: 是否有相同的节点在抄读延时队列中
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_mtr_in_queue_is_exist(uint8 Pipe,uint16 Pn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //现在还需要判断下是不是有相同地址的
    uint8 addr[6]={0};
    uint8 i;
    taskmng_mtr_addr_get(Pn,LOGIC_ID_YC,addr);
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
            continue;
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn == Pn ||
                memcmp(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,addr,6)==0)
            return 1;
    }

    return -1;
}

#ifdef AREA_JIANGSU

/*******************************************************************************
* 函数名称: taskmng_mtr_in_queue_is_exist
* 函数功能: 是否有相同的节点在抄读延时队列中
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_caj_in_queue_is_exist(uint8 Pipe,uint16 Pn)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[Pipe];
    uint8 i;
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
            continue;
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn == Pn)
            return 1;
    }

    return -1;
}
#endif

/*******************************************************************************
* 函数名称: taskmng_trans_task_monitor_deal
* 函数功能://台体测试要用到 结束时间置为0xFF
* 输入参数:pipe 端口号
        TaskId 任务号
        cmd 更新命令
        indata 更新命令带的参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_trans_task_monitor_deal(uint8 pipe,uint16 taskID,UpdateTaskCmd_t cmd,void *indata)
{
    uint8 i;
    int ret;
    OOP_ACQ_MONITOR_T* taskMonitorUnit = NULL;

    pthread_mutex_lock(&TaskMonitor.MonitorLock);
    if(pipe!=AMR_THREAD_NUM)
        TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_trans_task_monitor_deal 任务ID-%d,更新命令:%d TaskMonitor.TaskCount %d \n",taskID,cmd,TaskMonitor.TaskCount);

    for(i=0;i<TaskMonitor.TaskCount;i++)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "TaskMonitor.TaskMonitorInfo[i].id is %d,i is %d\n",TaskMonitor.TaskMonitorInfo[i].id,i);
        if(taskID == TaskMonitor.TaskMonitorInfo[i].id)
        {
            taskMonitorUnit = &TaskMonitor.TaskMonitorInfo[i];
            break;
        }
    }
    if(taskMonitorUnit==NULL)
    {
        pthread_mutex_unlock(&TaskMonitor.MonitorLock);
        if(pipe!=AMR_THREAD_NUM)
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_task_monitor_update 未找到监控单元 \n");
        return;
    }
    if(taskMonitorUnit->taskState ==finish)
    {
        NOMAL_OAD_T NormalOad;
        NormalOad.logicId = 0;
        NormalOad.oad.value = 0x60340200;
        NormalOad.infoNum = taskID;
        NormalOad.classtag = 5;
        taskMonitorUnit->taskState =doing;
        memset((uint8*)&taskMonitorUnit->endTime,0xFF,sizeof(OOP_DATETIME_S_T));
        TASK_FMT_TRACE( pipe, REC_LOG, "结束时间写成FF\n");
        if(pipe !=AMR_THREAD_NUM)
        {
            ret = db_write_nomal(gAmrThreadArg[pipe].DBClientHandle, &NormalOad,(uint8*)taskMonitorUnit,sizeof(OOP_ACQ_MONITOR_T));
            TASK_FMT_TRACE( pipe, REC_LOG, "写任务监控单元 ret %d \n",ret);
        }
    }
    pthread_mutex_unlock(&TaskMonitor.MonitorLock);
    return;
}

/*******************************************************************************
* 函数名称: taskmng_task_record_file_set
* 函数功能://电表抄读成功后在任务记录文件中置为成功 index为线程中电表索引
* 输入参数: pipe 端口号
            MsIndex 电表索引
            TaskId 任务id
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_task_record_file_set(uint8 pipe,uint16 MsIndex,uint16 TaskId,uint8 logicid)
{

    Task_Record_T TaskRecord;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);
    //加上防护，以防溢出
    if(MsIndex >= MAX_AMR_COLLECT_CNT)
        return;
    read_pdata_xram(pfile,(char*) &TaskRecord,0, sizeof(Task_Record_T));
    TaskRecord.MeterRecord[MsIndex].Result = 1;
    write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));
}
/**
*********************************************************************
* @brief:      获取需要抄读透明方案报文内容
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9
* @param[in]：   SCHEME_TASK_RUN_INFO *TaskInfo  任务参数
* @param[in]：  CTS_RUN_TASK_T* RunTask 运行时任务信息
* @param[out]：MsIndex 电表索引
* @return：1 SEARCH_FINISH 全部抄完 SEARCH_FAIL 未找到要抄的 SEARCH_SUCCESS 找到要抄的
*********************************************************************
*/
int taskmng_next_trans_buf_get(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16* MsIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 cnt=0;
    uint16 Pn;
    while(1)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, " SucNum %d  NeedMtrNum %d MeterIndex %d  TotalNum %d  \n",RunTask->SucNum ,RunTask->NeedMtrNum,RunTask->MeterIndex, TaskInfo->TotalNum );
        if(RunTask->SucNum >= RunTask->NeedMtrNum)
        {
            RunTask->RunResult = SCHEME_FINISHED;
            return SEARCH_FINISH;
        }
        if(RunTask->MeterIndex >= TaskInfo->TotalNum)
        {
            cnt++;
            RunTask->MeterIndex =0;
        }
        if(cnt>=2)
        {
            return SEARCH_FAIL;
        }
        Pn = TaskInfo->MsInfo[RunTask->MeterIndex].Sn;
        if(TaskInfo->MsInfo[RunTask->MeterIndex].Pipe != pipe)
        {
            RunTask->MeterIndex++;
            continue;
        }
        if(taskmng_mtr_in_queue_is_exist(pipe, Pn)>0)
        {
            RunTask->MeterIndex++;
            continue;
        }
        if(taskmng_mtr_result_check(RunTask->MeterIndex,RunTask->MtrResult)==TRUE)
        {
            RunTask->MeterIndex++;
            continue;
        }
        *MsIndex = taskmng_ms_index_get(pipe,Pn,RunTask->logicid);
        TASK_FMT_TRACE( pipe, REC_LOG, " 透明方案获取到当前抄读电表 PN %d  MSindex %d  \n",Pn,*MsIndex );
        if(pAmrThreadArg->pMtrAmrInfo[*MsIndex].BroadFailCnt>=MAX_METER_FAILURE_COUNT)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, " 透明方案该表抄读失败次数过多 不再抄读 \n");
            taskmng_mtr_result_set(RunTask->MeterIndex, RunTask->MtrResult);
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].LastResult = FALSE;
            RunTask->MeterIndex++;
            RunTask->SucNum++;
            continue;
        }
        if(TaskInfo->transindex !=0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, " 添加一组报文的索引 TaskInfo->transindex %d ！！！ \n",TaskInfo->transindex);
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].ROADIndex = TaskInfo->transindex;
            TaskInfo->transindex =0;
        }
        //透明方案中借用ROADIndex表示每块表抄到的报文索引
        if(pAmrThreadArg->pMtrAmrInfo[*MsIndex].ROADIndex>=TaskInfo->TransInfo[RunTask->MeterIndex].cmd.nNum)
        {
            RunTask->SucNum++;
            TASK_FMT_TRACE( pipe, REC_LOG, " 透明方案当前电表 PN %d 抄完！！！ \n",Pn);
            taskmng_task_monitor_update(pipe,RunTask->TaskId,RunTask->logicid,UDTASK_SUCCESSNUM,NULL);
            TASK_FMT_TRACE(pipe, REC_LOG,"TaskInfo->TransInfo[RunTask->MeterIndex].cmd.nNum is %d\n",TaskInfo->TransInfo[RunTask->MeterIndex].cmd.nNum);
            if((TaskInfo->TransInfo[RunTask->MeterIndex].flag.bReport==TRUE)&&(TaskInfo->TransInfo[RunTask->MeterIndex].cmd.nNum<=1))
            {
                TASK_FMT_TRACE(pipe, REC_LOG,"需要处理结束时间 is %d\n",RunTask->TaskId);
                taskmng_trans_task_monitor_deal(pipe,RunTask->TaskId,UDTASK_RECVFRMNUM,NULL);
            }
            taskmng_mtr_result_set(RunTask->MeterIndex, RunTask->MtrResult);
            taskmng_task_record_file_set(pipe,*MsIndex,RunTask->TaskId,RunTask->logicid);
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].LastResult = TRUE;
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].AllFailCnt = 0;
#ifdef AREA_SHANDONG
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].roundFailCnt = 0;
#endif

            if(RunTask->SucNum >= RunTask->NeedMtrNum)
            {
                RunTask->RunResult = SCHEME_FINISHED;
                return SEARCH_FINISH;
            }
            RunTask->MeterIndex++;//该表全部抄完
        }
        else
        {
            pAmrThreadArg->pMtrAmrInfo[*MsIndex].LastTaskId = TaskInfo->TaskId;
            return SEARCH_SUCCESS;
        }

    }
}
/*******************************************************************************
* 函数名称: taskmng_trans_frame_send
* 函数功能://透明任务获取发送数据
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完
*******************************************************************************/
int taskmng_trans_frame_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    int ret;
    uint16 MsIndex;
    uint16 Pn;
    uint8 logicid;
    uint8 MtrAddr[6]={0};
    uint8 prtl;
    uint8 PayLoad[1000]={0};
    uint16 index=0;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    uint16 FrameLen;
    uint8 frameindex;
    MESSAGE_INFO_T* MsgSend = NULL;
    for(i=0;i<PARALLEL_QUEUE_NUM;i++)
    {
        if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==0)
        {
            ret = taskmng_next_trans_buf_get(pipe,TaskInfo,RunTask,&MsIndex);
            if(ret == SEARCH_SUCCESS)
            {
                Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
                logicid = pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid;
                taskmng_mtr_addr_prtl_get( Pn,logicid, MtrAddr, &prtl);
                pAmrThreadArg->AmrSendFrameInfo.QueueCnt++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag =1;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn = Pn;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].MsIndex = MsIndex;
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime = time(NULL);
                pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime = time(NULL);//当前电表最近一次发送时间
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].PrtlMode = prtl;
                memcpy(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,MtrAddr,6);
                //用FrameSeq存储透明方案当前发送电表在任务中的索引
                pAmrThreadArg->AmrSendFrameInfo.Queue[i].FrameSeq = RunTask->MeterIndex;
                frameindex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
                FrameLen = TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.nNum;
                TASK_FMT_TRACE( pipe, REC_LOG, "Pn %d MsIndex %d prtl %d frameindex %d CommBeginTime %ld \n",Pn,MsIndex,prtl,frameindex,time(NULL));

                memset(PayLoad,0x00,sizeof(PayLoad));
                index=0;
                memcpy_r(&PayLoad[index],(uint8*)&pipeOAD.value,4);
                index+=4;
                index+=2;
                memcpy_r(&PayLoad[index],MtrAddr,6);
                index+=6;
                if(FrameLen<0x80)
                {
                    PayLoad[index++] = FrameLen;
                }
                else
                {
                    PayLoad[index++] = 0x82;
                    PayLoad[index++] = (uint8)(FrameLen>>8);
                    PayLoad[index++] = (uint8)FrameLen;
                }
                memcpy(&PayLoad[index],TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.value ,FrameLen);
                index+=FrameLen;

                MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
                memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

                MsgSend->rpm = 1;
                MsgSend->index = GetMsgSeq();
                MsgSend->label = pipe;
                MsgSend->priority = 1;
                MsgSend->IID = 0x0003;
                MsgSend->IOP = 0x003E;
                MsgSend->msglen = index;
                MsgSend->sourlen = strlen(DownName);
                memcpy(MsgSend->souraddr,DownName,MsgSend->sourlen);
                MsgSend->destlen = strlen(CCOName);
                memcpy(MsgSend->destaddr,CCOName,MsgSend->destlen );
                memcpy(MsgSend->playload,PayLoad,index);

                TASK_FMT_TRACE( pipe, REC_LOG,"抄表报文 msgindex %d \n",MsgSend->index);

                TASK_BUF_TRACE( pipe, REC_LOG,TaskInfo->TransInfo[RunTask->MeterIndex].cmd.value[frameindex].data.value, FrameLen);
                taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
                taskmng_plc_queue_write(pAmrSendDown[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
                free(MsgSend);
            }
            else if(ret == SEARCH_FINISH)
            {
                return 1;
            }else
            {
                return 0;
            }

        }
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_trans_data_save
* 函数功能://存储透明方案数据
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_trans_data_save(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 MsIndex,uint8 seq,uint8* buf,uint16 buflen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 tmpaddr[6];
    uint32 storeNo = 0;
    int ret;
    time_t ResponseTime = time(NULL);
    uint16 Pn;
    uint8 cmdindex = pAmrThreadArg->pMtrAmrInfo[MsIndex].ROADIndex;
    //数据缓冲填命令结果，以前理解错了
    //OOP_TRANSRES_T TransResult;
    //memset(&TransResult,0x00,sizeof(OOP_TRANSRES_T));
    //TransResult.planID = TaskInfo->SchemeId;
    //DT_Time2DateTimeShort(RunTask->StartTime,&TransResult.eTime);
    Pn = pAmrThreadArg->pMtrAmrInfo[MsIndex].Pn;
    //TransResult.addr = taskmng_tsa_get( Pn,pAmrThreadArg->pMtrAmrInfo[MsIndex].logicid);
   // TransResult.res.nNum = 1;
   // TransResult.res.result[0].nIndex = TaskInfo->TransInfo[seq].cmd.value[cmdindex].nIndex;
    //DT_Time2DateTimeShort(ResponseTime,&TransResult.res.result[0].aTime);
   // TransResult.res.result[0].data.nNum = buflen;
    //memcpy(TransResult.res.result[0].data.value,buf,buflen);

    WRITE_RECORD_T inData;
    memset(&inData,0x00,sizeof(WRITE_RECORD_T));
    OOP_OAD_U MainOad={0x601A0200};
    OOP_OAD_U SubOad;
    MainOad.value = MainOad.value |TaskInfo->SchemeId;
    SubOad.value = TaskInfo->TransInfo[seq].cmd.value[cmdindex].nIndex;
    inData.recordOAD.optional = 1;
    inData.recordOAD.logicId = 0;
    inData.recordOAD.infoNum = Pn;
    inData.recordOAD.road.oadMain = MainOad;
    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0] = SubOad;
    inData.recordOAD.classtag = 5;
#if IS_NEED_ADD_8HOUR
    inData.colStartTm = RunTask->StartTime+8*60*60;
    inData.colEndTm = ResponseTime+8*60*60;
    inData.colStoreTm = ResponseTime+8*60*60;
#else
    inData.colStartTm = RunTask->StartTime;
    inData.colEndTm = ResponseTime;
    inData.colStoreTm = ResponseTime;
#endif
    inData.MAC.nNum = 6;
    taskmng_mtr_addr_get(Pn,LOGIC_ID_YC,tmpaddr);
    memcpy_r( inData.MAC.value,tmpaddr,6);
    //taskmng_mtr_addr_get( Pn, inData.MAC.value);
    inData.inDatalen = buflen;
    memcpy(inData.inDataBuf,buf,buflen);
    TASK_FMT_TRACE( pipe, REC_LOG,"存储透明方案结果集pn  %d 方案编号 %d 报文序号 %d\n",Pn,TaskInfo->SchemeId,TaskInfo->TransInfo[seq].cmd.value[cmdindex].nIndex );
    TASK_FMT_TRACE( pipe, REC_LOG,"Mainoad  %08x sub %08x 执行时间 %ld 响应时间 %ld \n",MainOad.value,SubOad.value,RunTask->StartTime,ResponseTime);
    TASK_BUF_TRACE( pipe, REC_LOG,buf,buflen);
    ret = db_write_record(pAmrThreadArg->DBClientHandle, &inData, &storeNo);
    if(ret!=0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG,"存储透明方案结果集失败 ret  %d \n",ret);
    }
}



/*******************************************************************************
* 函数名称: taskmng_data_reading_trans
* 函数功能://透明任务抄读
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完
*******************************************************************************/
int taskmng_data_reading_trans(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] = {0};
    uint8           PayloadBuf[MAX_MSG_LEN] = {0};        //payload缓冲区
    MESSAGE_INFO_T* MsgInfo = NULL;
    AMR_PLC_INFO_REPORT_T  AmrReportInfo;    //上报的抄读数据
    uint8           SendIndex=0;
    uint16 MtrIndex;
    uint16 TaskId;
    int             OverTimeCnt = 60;
    SCHEME_TASK_RUN_INFO RecvTaskInfo ;
    memset(&RecvTaskInfo,0x00,sizeof(SCHEME_TASK_RUN_INFO));
    CTS_RUN_TASK_T *RecvRunTask = NULL;
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
    case RTS_IDLE:
    {
        for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
        {
            pAmrThreadArg->pMtrAmrInfo[i].ROADIndex = 0;//透明任务中使用作为每块表抄到的方案报文的索引
            pAmrThreadArg->pMtrAmrInfo[i].BroadFailCnt=0;

        }
        taskmng_task_record_file_load(pipe,TaskInfo,RunTask);
        taskmng_comm_recv_buff_clear(pipe);
        TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_data_reading_trans 清空发送队列\n");
        memset(&pAmrThreadArg->AmrSendFrameInfo,0x00,sizeof(SendFrameQueue_T));
        RunTask->StartTime = time(NULL);
        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
    }
        break;
    case RTS_SEND:
    {
        if(taskmng_trans_frame_send(pipe,TaskInfo,RunTask)==1)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 透明方案任务%d 结束&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n",TaskInfo->TaskId);
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            return 1;
        }
        pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
    }
        break;
    case RTS_RECV:
    {

        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[pipe], RecvBuf);
        if(RecvLen > 0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_TRACE( pipe, REC_LOG, "透明抄表收到消息 msgindex %d IID %04X IOP %04X payload \n",MsgInfo->index,MsgInfo->IID,MsgInfo->IOP);

            TASK_BUF_TRACE( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
            switch(MsgInfo->IOP)
            {
            case 0x003E:
            {

            }
                break;
            case 0x0031:
            {
                TASK_FMT_TRACE(pipe, REC_LOG,"@@@@@收到抄表上报数据@@@@@@@@@@ \n");
                memset(&AmrReportInfo,0x00,sizeof(AMR_PLC_INFO_REPORT_T));
                if(taskmng_amr_report_data_unpack(PayloadBuf,MsgInfo->msglen,&AmrReportInfo)==FALSE)
                    break;
                if(taskmng_seq_is_valid(pipe,&SendIndex,AmrReportInfo.addr)==0)
                {
                    break;
                }
                MtrIndex = taskmng_ms_index_get(pipe,pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].Pn,LOGIC_ID_YC);
                if(AmrReportInfo.isExitReadingInfo == 1)
                    memcpy(&pAmrThreadArg->pMtrAmrInfo[MtrIndex].ReadingInfo,&AmrReportInfo.stPlcReadingInfo,sizeof(AMR_PLC_READING_INFO_T));
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime = time(NULL);
                taskmng_col_state_write( pipe, MtrIndex);
                TaskId = pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastTaskId;
                if(TaskId != pAmrThreadArg->TmnAmrInfo.CurrentTaskId)
                    break;
                taskmng_task_info_get(TaskId,LOGIC_ID_YC,&RecvTaskInfo);
                taskmng_run_task_get( pipe,TaskId,LOGIC_ID_YC,&RecvRunTask);
                taskmng_task_monitor_update(pipe,TaskId,LOGIC_ID_YC,UDTASK_RECVFRMNUM,NULL);
                uint8 seq = pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].FrameSeq;
                taskmng_trans_data_save(pipe,&RecvTaskInfo,RecvRunTask,MtrIndex,seq,AmrReportInfo.pDataBuf,AmrReportInfo.nDataBufLen);
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt = 0;
                pAmrThreadArg->pMtrAmrInfo[MtrIndex].ROADIndex++;
                pAmrThreadArg->AmrSendFrameInfo.Queue[SendIndex].ValidFlag=0;
                if(pAmrThreadArg->AmrSendFrameInfo.QueueCnt>0)
                    pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
            }
                break;
            default:
                break;
            }

        }
        else
        {
            for(i=0;i<PARALLEL_QUEUE_NUM;i++)
            {
                if(pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag==1)
                {
                    if(abs(time(NULL)-pAmrThreadArg->AmrSendFrameInfo.Queue[i].CommBeginTime)>OverTimeCnt)
                    {
                        TASK_FMT_TRACE( pipe, REC_LOG, "Queue index %d 超时 PN %d \n",i,pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn);
                        taskmng_pn_state_refresh( pipe, i);
                        pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
                    }
                }

            }
        }
    }
        break;
    default:
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
        break;
    }
    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_data_reading_encrypt
* 函数功能://转加密任务抄读
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完
*******************************************************************************/
int taskmng_data_reading_encrypt(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T  *pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t sSendTime[AMR_THREAD_NUM];       //每一帧发送的时间
    uint16 overtime = 30;       //转加密任务用透传 超时时间设为30秒

    int ret;
    int             RecvLen = 0;
    uint8           RecvBuf[MAX_MSG_LEN] = {0};
    uint8           payload[2000]={0};
    MESSAGE_INFO_T *MsgInfo=NULL;
    uint16 pn = TaskInfo->MsInfo[RunTask->MeterIndex].Sn;//转加密任务一个表一个表执行 用MeterIndex记录当前执行的表
    //测试物联表校时的时候 两个转加密任务只给了120秒 485执行完的时候 可能会等下一个过分的时候载波才执行
    //这个时候已经来不及了 所以载波这边就一直等 可以执行了就一直执行
    while(1)
    {
        if(gEncryptPipe != -1 && gEncryptPipe != pipe)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "当前端口 %d 在执行转加密任务\n",gEncryptPipe);
            sleep(1);
            //return 1;
        }else
        {
            gEncryptPipe = pipe;
            break;
        }
     }
    switch(pAmrThreadArg->TotalState.NormalAmr)
    {
        case RTS_IDLE:
        {
            taskmng_task_record_file_load(pipe,TaskInfo,RunTask);
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_data_reading_encrypt 转加密任务\n");
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;
        }
            break;

        case RTS_SEND:
        {
            ret = taskmng_encrypt_send(pipe,TaskInfo,RunTask);
            if(ret ==1)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 转加密方案任务%d \n",TaskInfo->TaskId);
                pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
                RunTask->RunResult = SCHEME_FINISHED;
                gEncryptPipe = -1;
                //返回2后会再找任务执行
                return 2;
            }else if(ret ==2)
            {
                break;
            }else
            {
                sSendTime[pipe] = time(NULL);
            }
            pAmrThreadArg->TotalState.NormalAmr = RTS_RECV;
        }
            break;
        case RTS_RECV:
        {
            RecvLen = taskmng_plc_queue_read(pAmrRecvDown[pipe], RecvBuf);
            if(RecvLen > 0)
            {
                MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
                memcpy(payload,MsgInfo->playload,MsgInfo->msglen);
                TASK_FMT_DEBUG( pipe, REC_LOG, "收到消息  Msgindex %d sendindex %d\n",MsgInfo->index,RunTask->encryptMsgIndex);

                TASK_BUF_DEBUG( pipe, REC_LOG,MsgInfo->playload, MsgInfo->msglen);
                if(MsgInfo->IOP!=0x0038|| MsgInfo->index!=RunTask->encryptMsgIndex)
                    break;

                if(MsgInfo->msglen<=1)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_RETRY;
                    break;
                }
                uint8 lenoffset;
                uint16 framelen = XADR_GetEleNum(payload, &lenoffset);
                ret = taskmng_encrypt_recv(pipe,payload+lenoffset,framelen,TaskInfo,RunTask);
                if(ret <0)
                {
                    pAmrThreadArg->TotalState.NormalAmr = RTS_RETRY;
                    break;
                }
                pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

            }else
            {
                if(abs(time(NULL)-sSendTime[pipe])>overtime)
                {

                    pAmrThreadArg->TotalState.NormalAmr = RTS_RETRY;
                }
            }

        }
            break;
        case RTS_RETRY:
        {
            RunTask->encryptFailCnt++;
            TASK_FMT_TRACE( pipe, REC_LOG, "电表 %d 失败次数 %d\n",pn,RunTask->encryptFailCnt);
            pAmrThreadArg->TotalState.NormalAmr = RTS_SEND;

        }
            break;
        default:
            pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE;
            break;
    }
    return -1;

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
void taskmng_task_record_file_load(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i,j;
    uint16 Pn;
    Task_Record_T TaskRecord;
    memset(&TaskRecord,0x00,sizeof(Task_Record_T));
    uint16 TaskId = RunTask->TaskId;
    uint8 logicid = RunTask->logicid;
    char      pfile[100]={0};
    memset(pfile,0x00,sizeof(pfile));
    sprintf(pfile,"%s/%s%03d_PIPE%02d_Logic%d",PrivateDir,TASK_RECORD_FILE,TaskId,pipe,logicid);

    read_pdata_xram(pfile, (char*)&TaskRecord,0, sizeof(Task_Record_T));
    uint16 sucnum = 0;
    int16 msindex = 0;
    //RunTask->SucNum = 0;
    //TASK_BUF_DEBUG ( pipe,COMM_LOG, RunTask->MtrResult,128);
    //如果任务记录文件中记录的信息和任务当前轮次相同则加载记录中的抄表结果
    if(memcmp(TaskRecord.ThisTurnBeginTime,RunTask->ThisTurnBeginTime,7)==0)
    {
        for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
        {
            //该表当前轮次已经抄读成功
            if(TaskRecord.MeterRecord[i].Result == 1)
            {
                Pn = TaskRecord.MeterRecord[i].Pn;
                TASK_FMT_TRACE(pipe, REC_LOG,"PN %d 当前轮次已经抄读成功 \n",Pn);
                for(j=0;j<TaskInfo->TotalNum;j++)
                {
                    if(TaskInfo->MsInfo[j].Sn == Pn)
                    {
                        TASK_FMT_TRACE(pipe, REC_LOG,"TaskInfo->MsInfo[j].Sn is %d, i is %d,j is %d\n",TaskInfo->MsInfo[j].Sn,i,j);
                        if(TaskInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
                        {
                            msindex =  taskmng_ms_index_get(pipe, Pn,logicid);
                            if(msindex!=-1)
                            {
                                pAmrThreadArg->pMtrAmrInfo[msindex].DayFrozenFlag = 1;
                            }
                        }
                        taskmng_mtr_result_set(j,RunTask->MtrResult);
                        sucnum++;
                        break;
                    }
                }
            }

        }
        //如果任务被打断重新执行 且上一次执行有失败的 要取上次执行完的结果
       // TASK_FMT_TRACE(pipe, REC_LOG,"SucNum %d\n",RunTask->SucNum);

        if(RunTask->SucNum < sucnum)
            RunTask->SucNum = sucnum;

    }
    else  //将任务记录文件中信息初始化为当前轮次
    {
        memcpy(TaskRecord.ThisTurnBeginTime,RunTask->ThisTurnBeginTime,7);
        for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
        {
            TaskRecord.MeterRecord[i].Result = 0;
        }
        TASK_FMT_DEBUG(pipe, REC_LOG,"任务 %d 任务记录文件更新至当前轮次\n",RunTask->TaskId);
        write_pdata_xram(pfile,(char*)&TaskRecord,0,sizeof(Task_Record_T));
    }
    TASK_FMT_DEBUG( pipe, REC_LOG, "任务 %d LOAD 任务记录文件后 SucNum %d \n",RunTask->TaskId,RunTask->SucNum);
}

/*******************************************************************************
* 函数名称: taskmng_ms_index_get
* 函数功能: 获取测量点号在当前线程中的电表索引
* 输入参数:Pipe
* 输出参数:
* 返 回 值:当前线程中的电表索引
*******************************************************************************/
int16 taskmng_ms_index_get(uint8 Pipe,uint16 Pn,uint8 logicid)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    uint16 i;
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        if(Pn == pAmrThreadArg->pMtrAmrInfo[i].Pn && logicid ==pAmrThreadArg->pMtrAmrInfo[i].logicid )
            return i;
    }
    return -1;
}


/*******************************************************************************
* 函数名称: taskmng_comm_recv_buff_clear
* 函数功能: 清除通讯接收数据
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void taskmng_comm_recv_buff_clear(uint8 CommPipe)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_MSG_LEN];        //接收数据缓冲区

    while (RecvLen > 0)
    {
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown_Cb[CommPipe], RecvBuf);
    }
}

/*******************************************************************************
* 函数名称: taskmng_amr_report_data_unpack
* 函数功能: 解上报的抄表报文
* 输入参数: PayloadBuf           消息内容
            Len         长度
* 输出参数: AmrReportInfo            抄表数据结构体
* 返 回 值:
*******************************************************************************/
BOOL taskmng_amr_report_data_unpack(uint8 *PayloadBuf,uint32 Len,AMR_PLC_INFO_REPORT_T  *AmrReportInfo)
{
    uint32 index =0;
    uint8   lenoffset;
    OOP_OAD_U pipeOad ;
    memset(&pipeOad,0x00,sizeof(OOP_OAD_U));
    uint8 addr[6]={0};
    memcpy_r((uint8*)&pipeOad.value, &PayloadBuf[index], 4); //端口号
    index+=4;
    AmrReportInfo->nPortID = taskmng_mtr_pipe_get(&pipeOad);
    if(AmrReportInfo->nPortID>1)
        return FALSE;
    memcpy_r(addr,&PayloadBuf[index], 6);
    index+=6;
    //增加物联网表后不能通过地址获取到信息点号 需要到根据地址到发送队列里匹配
//    AmrReportInfo->nMeterFilesID = taskmng_pn_get_from_addr(addr,LOGIC_ID_YC);
//    if(AmrReportInfo->nMeterFilesID == 0)
//    {
//        TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "上报的抄表地址找不到\n");
//        return FALSE;
//    }
    memcpy(AmrReportInfo->addr,addr,6);
    TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "上报电表地址 %02x%02x%02x%02x%02x%02x\n",addr[5],
            addr[4],addr[3],addr[2],addr[1],addr[0]);
    AmrReportInfo->nProtocolType = PayloadBuf[index++];
    if(index ==Len) //上报的否认没有数据帧
    {
        AmrReportInfo->nDataBufLen =0;
        return TRUE;
    }
    AmrReportInfo->nDataBufLen= XADR_GetEleNum(&PayloadBuf[index],&lenoffset);
    index+=lenoffset;
    TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "taskmng_amr_report_data_unpack  prtl %d  datalen %d \n",AmrReportInfo->nProtocolType,AmrReportInfo->nDataBufLen);
    //TASK_FMT_TRACE(AmrReportInfo->nPortID, REC_LOG, "Len %d index %d lenoffset %d \n",Len,index,lenoffset);
    if(index+AmrReportInfo->nDataBufLen>Len)
        return FALSE;
    memcpy(AmrReportInfo->pDataBuf,&PayloadBuf[index],AmrReportInfo->nDataBufLen);
    index+=AmrReportInfo->nDataBufLen;
    if(index==Len)
        return TRUE;
    if(PayloadBuf[index++]==0x00)
       return TRUE;
    AmrReportInfo->isExitReadingInfo = 1;
    AmrReportInfo->stPlcReadingInfo.nRelayRouterLevel = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nChannelIdentifier = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierReadPhase = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierSignalQuality = PayloadBuf[index++];
    AmrReportInfo->stPlcReadingInfo.nCarrierSymbol = PayloadBuf[index++];
    memcpy_r(AmrReportInfo->stPlcReadingInfo.aReserveForOtherUse,&PayloadBuf[index],3);
    index+=3;
    return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_seq_is_valid
* 函数功能: 判断上行帧是否有效帧
* 输入参数: InSeq     需判断的电表号
* 输出参数: OutSeq    输出对应的帧索引
* 返 回 值: 1         有效
*           0         无效
*******************************************************************************/
uint8 taskmng_seq_is_valid(uint8 Pipe,uint8* OutSeq, uint8 *addr)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i = 0; i < PARALLEL_QUEUE_NUM; i++)
    {
        //上行帧序号在所发有效帧之内

        if((memcmp(pAmrThreadArg->AmrSendFrameInfo.Queue[i].MeterAddr,addr,6)== 0)
                && (pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1))
        {
            *OutSeq = i;
            return 1;
        }
    }

    return 0;
}

#ifdef AREA_JIANGSU

/*******************************************************************************
* 函数名称: taskmng_seq_is_valid
* 函数功能: 判断上行帧是否有效帧
* 输入参数: InSeq     需判断的电表号
* 输出参数: OutSeq    输出对应的帧索引
* 返 回 值: 1         有效
*           0         无效
*******************************************************************************/
uint8 taskmng_seq_is_caj_valid(uint8 Pipe,uint8* OutSeq, uint16 Pn)
{
    int i = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[Pipe];
    for(i = 0; i < PARALLEL_QUEUE_NUM; i++)
    {
        //上行帧序号在所发有效帧之内

        if((pAmrThreadArg->AmrSendFrameInfo.Queue[i].Pn == Pn)
                && (pAmrThreadArg->AmrSendFrameInfo.Queue[i].ValidFlag == 1))
        {
            *OutSeq = i;
            return 1;
        }
    }

    return 0;
}
#endif
/*******************************************************************************
* 函数名称: taskmng_col_state_write
* 函数功能://写60320200采集状态集
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_col_state_write(uint8 pipe,uint16 MtrIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 Pn = pAmrThreadArg->pMtrAmrInfo[MtrIndex].Pn;

    OOP_OAD_U Oad = {0x60320200};
    OOP_ACQ_RLUESTATE_T AcqState;
    memset(&AcqState,0x00,sizeof(OOP_ACQ_RLUESTATE_T));

    AcqState.add = taskmng_tsa_get(Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid);
    AcqState.relayLevel = pAmrThreadArg->pMtrAmrInfo[MtrIndex].ReadingInfo.nRelayRouterLevel;
    AcqState.port = taskmng_pipe_oad_get(pipe);
    DT_Time2DateTimeShort(pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastRecvTime,&AcqState.time);
    AcqState.failTimes = pAmrThreadArg->pMtrAmrInfo[MtrIndex].AllFailCnt;
    AcqState.phase = pAmrThreadArg->pMtrAmrInfo[MtrIndex].phase;
    AcqState.phaseSeq = pAmrThreadArg->pMtrAmrInfo[MtrIndex].phaseseq;
    //TASK_FMT_TRACE(pipe, RELAY_LOG, "写PN %d 采集状态集 relayLevel %d failTimes %d phase %d phaseSeq %d \n",
    //    Pn,AcqState.relayLevel,AcqState.failTimes,AcqState.phase,AcqState.phaseSeq);

    taskmng_normal_data_write_pipe(pipe,(uint8*)&AcqState,sizeof(OOP_ACQ_RLUESTATE_T),Oad,Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].logicid);


}

/*******************************************************************************
* 函数名称: taskmng_pn_state_refresh
* 函数功能: 刷新Pn的失败数据列表
* 输入参数: SeqIndex  帧序号
            MeterPrtlData 上报的645帧信息
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_pn_state_refresh(uint8 Pipe,uint8 SeqIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //uint16 Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].Pn;
    uint16 MtrIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].MsIndex;

     pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].ValidFlag = 0;
    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;

    //失败次数加1
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt < 0xff)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt++;
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].AllFailCnt++;
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastFailTime = time(NULL);
        gAmrThreadArg[Pipe].pMtrAmrInfo[MtrIndex].LastResult = FALSE;
    }
    taskmng_col_state_write(Pipe,MtrIndex);
    //printf("Sn = %d 抄表失败次数:%d\n",Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt);

    return;
}

#ifdef AREA_JIANGSU
/*******************************************************************************
* 函数名称: taskmng_pn_state_refresh
* 函数功能: 刷新Pn的失败数据列表
* 输入参数: SeqIndex  帧序号
            MeterPrtlData 上报的645帧信息
* 输出参数: 无
* 返 回 值: 无
*******************************************************************************/
void taskmng_caj_state_refresh(uint8 Pipe,uint8 SeqIndex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gCajThreadArg[Pipe];
    uint16 Pn = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].Pn;
    uint16 MtrIndex = pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].MsIndex;

     pAmrThreadArg->AmrSendFrameInfo.Queue[SeqIndex].ValidFlag = 0;
    if( pAmrThreadArg->AmrSendFrameInfo.QueueCnt> 0)
        pAmrThreadArg->AmrSendFrameInfo.QueueCnt--;

    //失败次数加1
    if(pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt < 0xff)
    {
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt++;
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].AllFailCnt++;
        pAmrThreadArg->pMtrAmrInfo[MtrIndex].LastFailTime = time(NULL);
        gAmrThreadArg[Pipe].pMtrAmrInfo[MtrIndex].LastResult = FALSE;
    }
    printf("Sn = %d 抄表失败次数:%d\n",Pn,pAmrThreadArg->pMtrAmrInfo[MtrIndex].BroadFailCnt);

    return;
}
#endif
/*******************************************************************************
* 函数名称: taskmng_task_info_init
* 函数功能: 任务抄读前初始化
* 输入参数: pSchemeTaskRunInfo 任务信息
            tmpTask 当前线程中任务运行参数
            MtrInfo 当前线程中电表参数
* 输出参数:

* 返 回 值:
*******************************************************************************/
void taskmng_task_info_init(uint8 pipe,SCHEME_TASK_RUN_INFO *pSchemeTaskRunInfo, CTS_RUN_TASK_T *tmpTask,MtrAmrInfo_T *MtrInfo)
{
    DateTimeBcd_t  tmpTime;
    //uint8         tmpmin;
    //OOP_TI_T          TI;
    uint8 Prtl;
    taskmng_mtr_prtl_get(MtrInfo->Pn,MtrInfo->logicid,&Prtl);

    if(Prtl == PRTL_OOP)
    {
        if(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5002 || pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID== 0x5003)
        {

            //将时标间隔转化为分钟单位
            if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value/60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value*60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit > 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.value = 0;
            }
            //异常情况下使用默认值
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//默认15分钟
            }
            MtrInfo->CurNum = 0;
            MtrInfo->EndNum = 0;
            //初始化起始时间为15分钟的整数倍
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->LastTurnBeginTime,7);
            tmpTime.sec = 0;
            //取整，若起始时间的分钟不是15的整数倍，从最近的下一个15分钟开始
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
             #ifdef AREA_SHANDONG
            //山东的曲线 从当前周期开始抄
            OOP_TI_T          TI;
            TI.unit = pSchemeTaskRunInfo->DelayTimeUnit;
            TI.value = pSchemeTaskRunInfo->DelayTime;
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->ThisTurnBeginTime,7);
            tmpTime.sec = 0;
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
            //本轮开始时间有延时 减去延时
            DT_TimeSubInv(&MtrInfo->StartTime, TI);
            #endif
            if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_BC)
            {
                int ret;
                OOP_PLAN_NORMAL_T NormalPlanInfo;

                memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
                ret = taskmng_normal_plan_info_read(pSchemeTaskRunInfo->SchemeId, &NormalPlanInfo,LOGIC_ID_YC);
                if(ret<0)
                {
                    TASK_FMT_DEBUG( 0, REC_LOG, "补抄任务方案读取失败 \n");
                }

                pSchemeTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                pSchemeTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                pSchemeTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔：%u \n", NormalPlanInfo.recdRow.rd.ti.value);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔单位：%u \n", NormalPlanInfo.recdRow.rd.ti.unit);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时补抄周期：%u \n", NormalPlanInfo.recdRow.rd.last);
                if((pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN || pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR))
                    MtrInfo->StartTime = GetCurveTaskBeginTime(MtrInfo->StartTime, pSchemeTaskRunInfo);
                else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                    MtrInfo->StartTime = GetDayTaskBeginTime(pipe,MtrInfo->StartTime, pSchemeTaskRunInfo);
            }
            //现在有5分钟或者一分钟任务 不再最少15分钟
            #if 0
            tmpmin = commfun_BcdToHex(tmpTime.min);
            if(tmpmin%15 != 0)
            {
                tmpmin = (tmpmin/15)*15;
                tmpTime.min = commfun_HexToBcd(tmpmin);
                DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
                //去掉不能整除的分钟后早于了任务开始时间，需要加上一个15分钟
                TI.unit = 1;
                TI.value = 15;
                DT_TimeAddInv(&MtrInfo->StartTime, TI);
            }
            #endif

        }
        else if(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID == 0x5004)
        {
            MtrInfo->CurNum = 0; //用于日冻结补抄
            MtrInfo->EndNum = 0; //用于日冻结补抄
            //初始化起始时间为15分钟的整数倍
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->LastTurnBeginTime,7);
            tmpTime.hour = 0;
            tmpTime.min = 0;
            tmpTime.sec = 0;
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
            if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_BC)
            {
                int ret;
                OOP_PLAN_NORMAL_T NormalPlanInfo;

                memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
                ret = taskmng_normal_plan_info_read(pSchemeTaskRunInfo->SchemeId, &NormalPlanInfo,LOGIC_ID_YC);
                if(ret<0)
                {
                    TASK_FMT_DEBUG( 0, REC_LOG, "补抄任务方案读取失败 \n");
                }

                pSchemeTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                pSchemeTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                pSchemeTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔：%u \n", NormalPlanInfo.recdRow.rd.ti.value);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔单位：%u \n", NormalPlanInfo.recdRow.rd.ti.unit);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时补抄周期：%u \n", NormalPlanInfo.recdRow.rd.last);
                if((pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN || pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR))
                    MtrInfo->StartTime = GetCurveTaskBeginTime(MtrInfo->StartTime, pSchemeTaskRunInfo);
                else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                    MtrInfo->StartTime = GetDayTaskBeginTime(pipe,MtrInfo->StartTime, pSchemeTaskRunInfo);
            }
        }
    }

    if(Prtl == PRTL_07)
    {
        MtrInfo->CurNum = 1;
        MtrInfo->EndNum = 0;
        memset((uint8*)&MtrInfo->storeTm,0x0,sizeof(DateTimeBcd_t));
        switch(pSchemeTaskRunInfo->CsdTask.RoadTask[0].MainOad.nObjID)
        {
        case 0x5000:
            MtrInfo->EndNum = 3;
            break;
        case 0x5002:
            //将时标间隔转化为分钟单位
            if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value/60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = pSchemeTaskRunInfo->AcqInfo.TI.value*60;
            }
            else if(pSchemeTaskRunInfo->AcqInfo.TI.unit > 2)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.value = 0;
            }
            //异常情况下使用默认值
            if(pSchemeTaskRunInfo->AcqInfo.TI.value==0)
            {
                pSchemeTaskRunInfo->AcqInfo.TI.unit = 1;
                pSchemeTaskRunInfo->AcqInfo.TI.value = 15;//默认15分钟
            }
            MtrInfo->CurNum = 0;
            MtrInfo->EndNum = 0;
            //初始化起始时间为15分钟的整数倍
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->LastTurnBeginTime,7);
            tmpTime.sec = 0;
            //取整，若起始时间的分钟不是15的整数倍，从最近的下一个15分钟开始
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
            #ifdef AREA_SHANDONG
            //山东的曲线 从当前周期开始抄
            OOP_TI_T          TI;
            TI.unit = pSchemeTaskRunInfo->DelayTimeUnit;
            TI.value = pSchemeTaskRunInfo->DelayTime;
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->ThisTurnBeginTime,7);
            tmpTime.sec = 0;
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
            //本轮开始时间有延时 减去延时
            DT_TimeSubInv(&MtrInfo->StartTime, TI);
            #endif
            if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_BC)
            {
                int ret;
                OOP_PLAN_NORMAL_T NormalPlanInfo;

                memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
                ret = taskmng_normal_plan_info_read(pSchemeTaskRunInfo->SchemeId, &NormalPlanInfo,LOGIC_ID_YC);
                if(ret<0)
                {
                    TASK_FMT_DEBUG( 0, REC_LOG, "补抄任务方案读取失败 \n");
                }

                pSchemeTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                pSchemeTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                pSchemeTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;
                if((pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN || pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR))
                    MtrInfo->StartTime = GetCurveTaskBeginTime(MtrInfo->StartTime, pSchemeTaskRunInfo);
                else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                    MtrInfo->StartTime = GetDayTaskBeginTime(pipe,MtrInfo->StartTime, pSchemeTaskRunInfo);
            }
            #if 0
            tmpmin = commfun_BcdToHex(tmpTime.min);
            if(tmpmin%15 != 0)
            {
                tmpmin = (tmpmin/15)*15;
                tmpTime.min = commfun_HexToBcd(tmpmin);
                DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
                //去掉不能整除的分钟后早于了任务开始时间，需要加上一个15分钟
                TI.unit = 1;
                TI.value = 15;
                DT_TimeAddInv(&MtrInfo->StartTime, TI);
            }
            #endif

            break;
        case 0x5003:

            if(taskmng_task_period_get(pSchemeTaskRunInfo->TaskId,pSchemeTaskRunInfo->logicid) < 86400L)
                MtrInfo->EndNum = 1;
            else
                MtrInfo->EndNum = 48;

            break;
        case 0x5004:
            //初始化起始时间为15分钟的整数倍
            memcpy_r((uint8*)&tmpTime,(uint8*)tmpTask->LastTurnBeginTime,7);
            tmpTime.sec = 0;
            //取整，若起始时间的分钟不是15的整数倍，从最近的下一个15分钟开始
            DT_DateTimeBCD2Time(&tmpTime,&MtrInfo->StartTime);
            if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_BC)
            {
                int ret;
                OOP_PLAN_NORMAL_T NormalPlanInfo;

                memset(&NormalPlanInfo,0x00,sizeof(OOP_PLAN_NORMAL_T));
                ret = taskmng_normal_plan_info_read(pSchemeTaskRunInfo->SchemeId, &NormalPlanInfo,LOGIC_ID_YC);
                if(ret<0)
                {
                    TASK_FMT_DEBUG( 0, REC_LOG, "补抄任务方案读取失败 \n");
                }

                pSchemeTaskRunInfo->AcqInfo.TI.value = NormalPlanInfo.recdRow.rd.ti.value;
                pSchemeTaskRunInfo->AcqInfo.TI.unit = NormalPlanInfo.recdRow.rd.ti.unit;
                pSchemeTaskRunInfo->AcqInfo.LastN =NormalPlanInfo.recdRow.rd.last;
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔：%u \n", NormalPlanInfo.recdRow.rd.ti.value);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时间间隔单位：%u \n", NormalPlanInfo.recdRow.rd.ti.unit);
                TASK_FMT_DEBUG( pipe, REC_LOG, "补抄任务方案时补抄周期：%u \n", NormalPlanInfo.recdRow.rd.last);
                if((pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_MIN || pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_HOUR))
                    MtrInfo->StartTime = GetCurveTaskBeginTime(MtrInfo->StartTime, pSchemeTaskRunInfo);
                else if(pSchemeTaskRunInfo->AcqInfo.TI.unit == TASK_CYCLE_FLAG_DAY)
                    MtrInfo->StartTime = GetDayTaskBeginTime(pipe,MtrInfo->StartTime, pSchemeTaskRunInfo);
            }
            MtrInfo->EndNum = 1;//
            break;
        case 0x5005:
        case 0x5006:
            MtrInfo->EndNum = 1;//
            break;
        case 0x5007://暂定为定时冻结
            MtrInfo->EndNum = 60;
            break;
        case 0x5008:
        case 0x5009:
        case 0x500A:
        case 0x500B:
        case 0x5011:
            MtrInfo->EndNum = 2;
            break;
        default:
            break;
       }
       if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_LASTN)
        {
            MtrInfo->CurNum = pSchemeTaskRunInfo->AcqInfo.LastN;
            if(MtrInfo->CurNum==0)
                MtrInfo->CurNum = 1;
            MtrInfo->EndNum = MtrInfo->CurNum;
        }
        else if(pSchemeTaskRunInfo->AcqInfo.Acqtype == ACQTYP_NULL)
        {
            MtrInfo->CurNum = 0;
            MtrInfo->EndNum = 0;
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_patch_result_set
* 函数功能: //任务中一块表抄读成功后在补抄文件中置位
* 输入参数: Pipe 端口号
         Pn 电表号
        CurTask 任务运行时信息
        DayCnt

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_patch_result_set(uint8 pipe,uint16 Pn,CTS_RUN_TASK_T *CurTask,uint8 DayCnt)
{
    char    pfile[100] = {0};
    int ret;
    uint8 i;
    uint16 j;
    PATCH_TASK_T PatchTask;
    time_t tmptime;
    uint8 tmpbuff[7];
    sprintf(pfile,"%s/%s%03d_%02d_PIPE%02d",PrivateDir,PATCH_FILE,CurTask->TaskId,DayCnt,pipe);
    if(access(pfile,R_OK)!=0)
    {
        return ;
    }
    //如果当前正补抄到该任务 将任务结果同步到内存中
    uint8 flag=0;
    if(gAmrThreadArg[pipe].PatchTask.TaskId == CurTask->TaskId)
    {
        if(gAmrThreadArg[pipe].PatchTask.Time[0]==gCurBcdTime[pipe].Day)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "当前正补抄到该任务 \n");
            flag = 1;
        }
    }
    read_pdata_xram(pfile,(char*)&PatchTask,0,sizeof(PATCH_TASK_T));
    memcpy_r(tmpbuff,(uint8*)CurTask->LastTurnBeginTime,7);
    DT_DateTimeBCD2Time((DateTimeBcd_t*)tmpbuff,&tmptime);

    for(i=0;i<PatchTask.TimeNum;i++)
    {
        if(tmptime == PatchTask.TimeInfo[i].PatchTime)
        {
            for(j=0;j<PatchTask.TimeInfo[i].MeterNum;j++)
            {
                if(Pn ==PatchTask.TimeInfo[i].Pn[j] )
                {
                    //PatchTask.TimeInfo[i].MeterInfo[j].flag = 1;
                    taskmng_mtr_result_set(j,PatchTask.TimeInfo[i].flag);
                    if(flag==1)
                    {
                        //gAmrThreadArg[pipe].PatchTask.TimeInfo[i].MeterInfo[j].flag = 1;
                        taskmng_mtr_result_set(j,gAmrThreadArg[pipe].PatchTask.TimeInfo[i].flag);
                    }
                    ret = write_pdata_xram(pfile,(char*)&PatchTask,0,sizeof(PATCH_TASK_T));
                    if(ret<0)
                    {
                        TASK_FMT_TRACE( pipe, REC_LOG, "pn %d 写补抄文件失败 \n",Pn);
                    }
                    return ;
                }
            }
        }
    }
}

/*******************************************************************************
* 函数名称: taskmng_645_event_data_get
* 函数功能://从数据中心获取存储的事件数据
* 输入参数:pipe 端口号
    Pn 电表号
    MainOad 主oad
      SubOad 子oad
       StartTime 启动时标
* 输出参数: DataBuf 事件数据
        DataLen 数据长度
* 返 回 值:
*******************************************************************************/
int taskmng_645_event_data_get(uint8 pipe,uint16 Pn,uint8 logicid,OOP_OAD_U MainOad,OOP_OAD_U SubOad,time_t StartTime,uint8* DataBuf,uint16 *DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint8 tmpaddr[6];
    int ret;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    memset(&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    memset(&startReadRecord,0x00,sizeof(READ_RECORD_T));
    if(TRUE == taskmng_oad_info_get( SubOad,&pOadInfo))
    {
        //读数据中心的时候用总的读
        if(taskmng_need_merge_check(&pOadInfo))
        {
            SubOad.nIndex = 0;
        }
    }
    startReadRecord.recordOAD.road.oadMain = MainOad;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0] =SubOad;
    startReadRecord.recordOAD.logicId = logicid;
    startReadRecord.recordOAD.infoNum = Pn;
    startReadRecord.MAC.nNum = 6;
    taskmng_mtr_addr_get(Pn,logicid,tmpaddr);
    memcpy_r(startReadRecord.MAC.value,tmpaddr,6);
    //taskmng_mtr_addr_get( Pn, (uint8*)startReadRecord.MAC.value);
    startReadRecord.cType = 0x01;
#if IS_NEED_ADD_8HOUR
    startReadRecord.cStart = StartTime+8*60*60;
    startReadRecord.cEnd = time(NULL)+1+8*60*60;
#else
    startReadRecord.cStart = StartTime;
    startReadRecord.cEnd = time(NULL)+1;
#endif
    ret = db_read_record(pAmrThreadArg->DBClientHandle,&startReadRecord,DATA_LEN_MAX,&RecordOut);
    TASK_FMT_TRACE( pipe, REC_LOG, "从数据中心获取事件数据 mainoad %08x suboad %08x starttime %ld ret %d   \n",MainOad.value,SubOad.value,StartTime,ret);
    if(ret!=0)
        return ret;
    *DataLen = RecordOut.record[0].outDatalen;
    memcpy(DataBuf,RecordOut.record[0].outDataBuf,*DataLen);
    TASK_FMT_TRACE( pipe, REC_LOG, "datalen %d data \n",*DataLen);

    TASK_BUF_TRACE( pipe, REC_LOG,DataBuf, *DataLen);
    return ret;

}

/*******************************************************************************
* 函数名称: taskmng_645_event_data_save
* 函数功能://将需要上报的事件采集方案中645表 按照6012格式写入数据库
* 输入参数:pipe 端口号
    Pn 电表号
    TaskId 任务号
       RoadTask 任务中的事件抄读信息
       StartTime 启动时标
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_645_event_data_save(uint8 pipe,uint16 Pn,uint8 logicid,uint16 TaskId,CSD_TASK_INFO *RoadTask,time_t StartTime)
{

    TASK_FMT_TRACE( pipe, REC_LOG, " taskmng_645_event_data_save PN %d StartTime %ld \n",Pn,StartTime );
    uint8 i,j;
    int ret;
    OOP_OAD_U SubOad;
    const MapOopTo645_NEW*     Map;
    const EventMapOopTo645_t *pEventMapList;
    const MapOopTo645_NEW*     Maptmp = NULL;
    DATASTRUCT datatype = E_NULL;
    uint8 DataBuf[2000]={0};
    uint16 DataLen;
    time_t        RealTime_t = time(NULL);
    uint8 Addr[6]={0};
    uint16 rptLen = 25;
    //事件上报时把固定列CSD先填进去
    uint8 rptBuff[2000] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00};
    rptBuff[rptLen++] = 1;      //CSD类型1 记录型对象
    memcpy_r(&rptBuff[rptLen],(uint8*)&RoadTask->MainOad.value,4);
    rptLen+=4;
    rptBuff[rptLen++] = RoadTask->SubOadNum;
    TASK_FMT_TRACE( pipe, REC_LOG, " MainOad %08x SubOadNum %d  \n",RoadTask->MainOad.value,RoadTask->SubOadNum );
    for(i=0;i<RoadTask->SubOadNum;i++)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, " SubOadindex %d  suboad %08x   \n",i,RoadTask->OadListInfo[i].value );
        memcpy_r(&rptBuff[rptLen],(uint8*)&RoadTask->OadListInfo[i].value,4);
        rptLen+=4;
    }
    rptBuff[rptLen++] = 1;
    rptBuff[rptLen++] = 1;

    //填固定列数据
    //服务器地址
    rptBuff[rptLen++] = DT_TSA;
    rptBuff[rptLen++] = 7;
    rptBuff[rptLen++] = 5;
    taskmng_mtr_addr_get( Pn,logicid,Addr);
    memcpy_r(&rptBuff[rptLen],Addr, 6);
    rptLen+=6;
    //启动时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(StartTime,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //成功时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;
    //存储时标
    rptBuff[rptLen++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(RealTime_t,(DateTimeHex_t *)&rptBuff[rptLen]);
    rptLen+=7;

    rptBuff[rptLen++] = DT_ARRAY;
    rptBuff[rptLen++] =RoadTask->SubOadNum;

    pEventMapList = taskmng_event_map_get(RoadTask->MainOad);
    Map = pEventMapList->EventMap;
    for(j=0;j<RoadTask->SubOadNum;j++)
    {
        SubOad = RoadTask->OadListInfo[j];
        ret = taskmng_645_event_data_get(pipe,Pn,logicid,RoadTask->MainOad,SubOad,StartTime,DataBuf,&DataLen);
        if(ret!=0)
        {
            //如果事件序号也为空 则不上报
            if(SubOad.value == 0x20220200)
                return;
            rptBuff[rptLen++] = DT_NULL;
            continue;
        }

        Maptmp = Map;
        while (Maptmp->Oad.value != 0xFFFFFFFF)
        {
            if(Maptmp->Oad.value == SubOad.value)
            {
                datatype = Maptmp->CovertSruct;
            }
            Maptmp++;
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "  suboad %08x  datatype %d \n",SubOad.value,datatype );
        switch(datatype)
        {
        case E_OOP_ENERGY_T:
        {
            OOP_ENERGY_T Energy;
            memcpy(&Energy,DataBuf,sizeof(OOP_ENERGY_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=Energy.nNum)
            {
                rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[SubOad.nIndex-1], 4);
                rptLen+=4;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = Energy.nNum;
                for(i=0;i<Energy.nNum;i++)
                {
                    rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;//
                    memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[i], 4);
                    rptLen+=4;
                }
            }
        }
            break;
        case E_OOP_ENERGYS_T:
        {
            OOP_ENERGYS_T Energy;
            memcpy(&Energy,DataBuf,sizeof(OOP_ENERGYS_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=Energy.nNum)
            {
                rptBuff[rptLen++] = DT_DOUBLE_LONG;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[SubOad.nIndex-1], 4);
                rptLen+=4;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = Energy.nNum;
                for(i=0;i<Energy.nNum;i++)
                {
                    rptBuff[rptLen++] = DT_DOUBLE_LONG;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[i], 4);
                    rptLen+=4;
                }
            }
        }
            break;

        case E_OOP_HENERGY_T:
        {
            OOP_HENERGY_T Energy;
            memcpy(&Energy,DataBuf,sizeof(OOP_HENERGY_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=Energy.nNum)
            {
                rptBuff[rptLen++] = DT_LONG64_UNSIGNED;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[SubOad.nIndex-1], 8);
                rptLen+=8;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = Energy.nNum;
                for(i=0;i<Energy.nNum;i++)
                {
                    rptBuff[rptLen++] = DT_LONG64_UNSIGNED;//
                    memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[i], 8);
                    rptLen+=8;
                }
            }
        }
            break;
        case E_OOP_HENERGYS_T:
        {
            OOP_HENERGYS_T Energy;
            memcpy(&Energy,DataBuf,sizeof(OOP_HENERGYS_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=Energy.nNum)
            {
                rptBuff[rptLen++] = DT_LONG64;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[SubOad.nIndex-1], 8);
                rptLen+=8;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = Energy.nNum;
                for(i=0;i<Energy.nNum;i++)
                {
                    rptBuff[rptLen++] = DT_LONG64;//
                    memcpy_r(&rptBuff[rptLen], (uint8*)&Energy.nValue[i], 8);
                    rptLen+=8;
                }

            }
        }
            break;
        case E_OOP_DEMAND_T:
        {
            OOP_DEMAND_T demand;
            memcpy(&demand,DataBuf,sizeof(OOP_DEMAND_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=demand.nNum)
            {
                rptBuff[rptLen++]= 0x02;
                rptBuff[rptLen++] = 0x02;
                rptBuff[rptLen++] =DT_DOUBLE_LONG_UNSIGNED;
                memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[SubOad.nIndex-1].nValue, 4);
                rptLen+=4;
                rptBuff[rptLen++] = DT_DATETIME_S;
                memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[SubOad.nIndex-1].time.year, 2);
                rptLen+=2;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.month;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.day;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.hour;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.minute;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.second;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = demand.nNum;
                for(i=0;i<demand.nNum;i++)
                {
                    rptBuff[rptLen++]= 0x02;
                    rptBuff[rptLen++] = 0x02;
                    rptBuff[rptLen++] =DT_DOUBLE_LONG_UNSIGNED;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[i].nValue, 4);
                    rptLen+=4;
                    rptBuff[rptLen++] = DT_DATETIME_S;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[i].time.year, 2);
                    rptLen+=2;
                    rptBuff[rptLen++] =demand.demand[i].time.month;
                    rptBuff[rptLen++] =demand.demand[i].time.day;
                    rptBuff[rptLen++] =demand.demand[i].time.hour;
                    rptBuff[rptLen++] =demand.demand[i].time.minute;
                    rptBuff[rptLen++] =demand.demand[i].time.second;
                }
            }
        }
            break;
        case E_OOP_DEMANDS_T:
        {
            OOP_DEMAND_T demand;
            memcpy(&demand,DataBuf,sizeof(OOP_DEMAND_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=demand.nNum)
            {
                rptBuff[rptLen++]= 0x02;
                rptBuff[rptLen++] = 0x02;
                rptBuff[rptLen++] =DT_DOUBLE_LONG;
                memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[SubOad.nIndex-1].nValue, 4);
                rptLen+=4;
                rptBuff[rptLen++] = DT_DATETIME_S;
                memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[SubOad.nIndex-1].time.year, 2);
                rptLen+=2;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.month;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.day;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.hour;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.minute;
                rptBuff[rptLen++] =demand.demand[SubOad.nIndex-1].time.second;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = demand.nNum;
                for(i=0;i<demand.nNum;i++)
                {
                    rptBuff[rptLen++]= 0x02;
                    rptBuff[rptLen++] = 0x02;
                    rptBuff[rptLen++] =DT_DOUBLE_LONG;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[i].nValue, 4);
                    rptLen+=4;
                    rptBuff[rptLen++] = DT_DATETIME_S;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&demand.demand[i].time.year, 2);
                    rptLen+=2;
                    rptBuff[rptLen++] =demand.demand[i].time.month;
                    rptBuff[rptLen++] =demand.demand[i].time.day;
                    rptBuff[rptLen++] =demand.demand[i].time.hour;
                    rptBuff[rptLen++] =demand.demand[i].time.minute;
                    rptBuff[rptLen++] =demand.demand[i].time.second;
                }
            }
        }
            break;
        case E_OOP_DWORD3V_T://分相变量
        {
            OOP_DWORD3V_T DWORD3V_T;
            memcpy(&DWORD3V_T,DataBuf,sizeof(OOP_DWORD3V_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=DWORD3V_T.nNum)
            {
                rptBuff[rptLen++] = DT_DOUBLE_LONG_UNSIGNED;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&DWORD3V_T.nValue[SubOad.nIndex-1], 4);
                rptLen+=4;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = DWORD3V_T.nNum;
                for(i=0;i<DWORD3V_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_DOUBLE_LONG_UNSIGNED;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&DWORD3V_T.nValue[i], 4);
                    rptLen+=4;
                }
            }
        }
            break;
        case E_OOP_WORD3V_T://电压
        {
            OOP_WORD3V_T WORD3V_T;
            memcpy(&WORD3V_T,DataBuf,sizeof(OOP_WORD3V_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=WORD3V_T.nNum)
            {
                rptBuff[rptLen++] = DT_LONG_UNSIGNED;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&WORD3V_T.nValue[SubOad.nIndex-1], 2);
                rptLen+=2;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = WORD3V_T.nNum;
                for(i=0;i<WORD3V_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_LONG_UNSIGNED;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&WORD3V_T.nValue[i], 2);
                    rptLen+=2;
                }
            }
        }
            break;
        case E_OOP_INT3V_T://电流
        {
            OOP_INT3V_T  INT3V_T;
            memcpy(&INT3V_T,DataBuf,sizeof(OOP_INT3V_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=INT3V_T.nNum)
            {
                rptBuff[rptLen++] = DT_DOUBLE_LONG;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&INT3V_T.nValue[SubOad.nIndex-1], 4);
                rptLen+=4;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;
                rptBuff[rptLen++]= INT3V_T.nNum;
                for(i=0;i<INT3V_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_DOUBLE_LONG;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&INT3V_T.nValue[i], 4);
                    rptLen+=4;
                }
            }
        }
            break;
        case E_OOP_INT4V_T://功率变量
        {
            OOP_INT4V_T  INT4V_T;
            memcpy(&INT4V_T,DataBuf,sizeof(OOP_INT4V_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=INT4V_T.nNum)
            {
                rptBuff[rptLen++] = DT_DOUBLE_LONG;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&INT4V_T.nValue[SubOad.nIndex-1], 4);
                rptLen+=4;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;
                rptBuff[rptLen++]= INT4V_T.nNum;
                for(i=0;i<INT4V_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_DOUBLE_LONG;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&INT4V_T.nValue[i], 4);
                    rptLen+=4;
                }
            }
        }
            break;
        case E_FRZ_DATETIME_S:
        case E_REAL_DATETIME_S:
        case E_DATETIME_S:
        {
            OOP_DATETIME_S_T DATETIME_S_T;
            memcpy(&DATETIME_S_T,DataBuf,sizeof(OOP_DATETIME_S_T));
            rptBuff[rptLen++] = DT_DATETIME_S;
            memcpy_r(&rptBuff[rptLen],(uint8*)&DATETIME_S_T.year,2);
            rptLen+=2;
            rptBuff[rptLen++] = DATETIME_S_T.month;
            rptBuff[rptLen++] = DATETIME_S_T.day;
            rptBuff[rptLen++] = DATETIME_S_T.hour;
            rptBuff[rptLen++] = DATETIME_S_T.minute;
            rptBuff[rptLen++] = DATETIME_S_T.second;
        }
            break;
        case E_DOUBLE_LONG_UNSIGNED:
        {
            rptBuff[rptLen++] =DT_DOUBLE_LONG_UNSIGNED;
            memcpy_r(&rptBuff[rptLen], DataBuf, 4);
            rptLen+=4;
        }
            break;
        case E_DOUBLE_LONG:
        {
            rptBuff[rptLen++] =DT_DOUBLE_LONG;
            memcpy_r(&rptBuff[rptLen], DataBuf, 4);
            rptLen+=4;
        }
            break;
        case E_OOP_METWORD_T:
        {
            OOP_METWORD_T METWORD_T;
            memcpy(&METWORD_T,DataBuf,sizeof(OOP_METWORD_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=METWORD_T.nNum)
            {
                rptBuff[rptLen++] = DT_LONG_UNSIGNED;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&METWORD_T.metWord[SubOad.nIndex-1], 2);
                rptLen+=2;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = METWORD_T.nNum;
                for(i=0;i<METWORD_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_LONG_UNSIGNED;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&METWORD_T.metWord[i], 2);
                    rptLen+=2;
                }
            }
        }
            break;
        case E_OOP_LONG4V_T:
        {
            OOP_LONG4V_T LONG4V_T;
            memcpy(&LONG4V_T,DataBuf,sizeof(OOP_LONG4V_T));
            if(SubOad.nIndex!=0&&SubOad.nIndex<=LONG4V_T.nNum)
            {
                rptBuff[rptLen++] = DT_LONG;//
                memcpy_r(&rptBuff[rptLen], (uint8*)&LONG4V_T.nValue[SubOad.nIndex-1], 2);
                rptLen+=2;
            }else
            {
                rptBuff[rptLen++] = DT_ARRAY;  //数组

                rptBuff[rptLen++] = LONG4V_T.nNum;
                for(i=0;i<LONG4V_T.nNum;i++)
                {
                    rptBuff[rptLen++] =DT_LONG;
                    memcpy_r(&rptBuff[rptLen], (uint8*)&LONG4V_T.nValue[i], 2);
                    rptLen+=2;
            }
            }
        }
            break;
#ifdef AREA_HUNAN
    case E_BIN:
    {
            rptBuff[rptLen++] = DT_UNSIGNED;
            rptBuff[rptLen++] = DataBuf[0];
    }
        break;
    case E_DATA:
    {
        OOP_VARIANT_T data;
        memcpy(&data,DataBuf,sizeof(OOP_VARIANT_T));
        rptBuff[rptLen++] = data.type;
        rptBuff[rptLen++] = data.byVal;
    }
        break;
#endif
        default:
            rptBuff[rptLen++] = DT_NULL;
            break;
        }
    }
//    rptBuff[rptLen++] = 0x00;//跟随上报信息域
//    rptBuff[rptLen++] = 0x00;//时间标签域
    taskmng_event_report_data_write(pipe,rptBuff,rptLen,EventReportOad);
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_result_check
* 函数功能: 检查路由档案信息
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                 不相同
*           <0                 相同
*******************************************************************************/
uint8 taskmng_meter_file_result_check(uint8 Pipe,uint8 *MeterInfo,uint8 MeterNum)
{
    uint8       Result = 0;
    uint32      i = 0;
    uint32      j = 0;
    uint16      meterCnt = 0;
    uint16      mtrIndex[32] = {0}; /* 相同地址的档案最多32个 */
    uint8        MtrAddr[6] = {0};
    uint16      Pn = 0;
    uint16      index=0;
    uint8       prtl;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    if(MeterNum>0)
    {
        for (i = 0; i < MeterNum; i++)
        {
            memcpy_r(MtrAddr,&MeterInfo[index],6);
            index+=6;
            index+=6;       //MAC不用
            prtl = MeterInfo[index++];
            //index++;    //通信参数不用
            //现在不能直接用地址获取信息点号了
            //Pn = taskmng_pn_get_from_addr(MtrAddr,LOGIC_ID_YC);
            TASK_FMT_DEBUG(Pipe, COMM_LOG,"读取到的电表消息 %u  prtl %u\n",i,  prtl);
            TASK_FMT_DEBUG(Pipe, COMM_LOG,"addr 0x%02x%02x%02x%02x%02x%02x \n",MtrAddr[5],MtrAddr[4],MtrAddr[3],MtrAddr[2],MtrAddr[1],MtrAddr[0]);
            if(taskmng_addr_isvalid_check(MtrAddr,6)>0)
            {
                meterCnt = taskmng_addr_in_amr_info_check(Pipe,LOGIC_ID_YC,MtrAddr,prtl, mtrIndex);
                TASK_FMT_DEBUG(Pipe, COMM_LOG,"taskmng_addr_in_amr_info_check meterCnt=%u\n", meterCnt);

                if (meterCnt > 0)
                {
                    //找到对应的地址 则标记下
                    for (j = 0; j < meterCnt; j++)
                    {
                        pAmrThreadArg->pMtrAmrInfo[mtrIndex[j]].SpecialFlag = 1;
                    }
                }
                else
                {
                    Result = 1;
                    if(gMeterFileModifyType[Pipe] == MODEFY_F10)
                    {
                        pAmrThreadArg->pNeedDeleteMetHead = taskmng_module_head_node_add(Pn,MtrAddr,prtl,pAmrThreadArg->pNeedDeleteMetHead);
                    }else
                    {
                        break;
                    }
                }
            }
        }
    }
    return Result;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_cmp_result_check
* 函数功能: 检查终端档案是否均已经比较完成
* 输入参数: Pipe        端口号
* 输出参数: 无
* 返 回 值: >0                 不相同
*           <0                 相同
*******************************************************************************/
uint8 taskmng_meter_file_cmp_result_check(uint8 Pipe)
{
    uint16   i;
    uint16  Pn;
    uint8   Result=0;
    uint8   MtrAddr[6];
    uint8   prtl;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];

    for(i =0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        Pn = pAmrThreadArg->pMtrAmrInfo[i].Pn;
        if(pAmrThreadArg->pMtrAmrInfo[i].SpecialFlag == 0)
        {
            Result = 1;
            if(gMeterFileModifyType[Pipe]==MODEFY_CLEAR)
                break;
            taskmng_mtr_addr_prtl_get(Pn,LOGIC_ID_YC,MtrAddr,&prtl);
            pAmrThreadArg->pMtrAmrInfo[i].SpecialFlag = 1;
            pAmrThreadArg->pNeedAddMetHead = taskmng_module_head_node_add(Pn,MtrAddr,prtl,pAmrThreadArg->pNeedAddMetHead );
        }
    }
    return Result;
}

/*******************************************************************************
* 函数名称: taskmng_all_meter_file_add_to_link
* 函数功能: 将终端所有档案添加到当前链表
* 输入参数: Pipe
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void taskmng_all_meter_file_add_to_link(uint8 Pipe)
{
    uint16 i;
    uint16 Pn;
    uint8 MtrAddr[6];
    uint8 prtl;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {

        Pn = pAmrThreadArg->pMtrAmrInfo[i].Pn;
        taskmng_mtr_addr_prtl_get( Pn,LOGIC_ID_YC, MtrAddr,&prtl);
        pAmrThreadArg->pMtrAmrInfo[i].SpecialFlag=1;
        pAmrThreadArg->pNeedAddMetHead = taskmng_module_head_node_add(Pn,MtrAddr,prtl,pAmrThreadArg->pNeedAddMetHead );

    }
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_cmp_deal
* 函数功能: 电表档案比对处理
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
* 处理流程 :

路由档案的修改新机制:

----> 路由模块如果按照老的档案修改机制,可以再子模块中通过配置来处理

----> 每次档案的读取下发数量采取宏配置:MODULEDEAL_CNT

----> 档案修改分为两种情况:
      -->1)、模块定时判断:
                                   1) 读取路由档案信息
                                   2) 比较后如果有多余或者新增的路由档案,那么判定路由档案不一致
                                   3) 进行档案的修改处理
                                   4) 对路由进行参数初始化
                                   5) 添加终端档案到路由中
      -->2)、主站下发F10修改:
                                   1)读取路由档案信息
                                   2)比较后如果有多余的档案,添加到删除的链表中，如果有新增的档案，添加到新增档案的链表中
                                   3)进行档案的修改处理
                                   4)删除多余的路由档案
                                   5)添加新增的路由档案
                                   6)完成后进入档案的同步比较
                                   7) 读取路由档案信息
                                   8) 比较后如果有多余或者新增的路由档案,那么判定路由档案不一致
                                   9) 进行档案的修改处理
                                   10) 对路由进行参数初始化
                                   11) 添加终端档案到路由中

*******************************************************************************/
int taskmng_meter_file_cmp_deal(uint8 Pipe)
{
    uint16          i;
    int             ret;
    uint16          index;
    uint16      QueryMeterNum = MODULEDEAL_CNT;
    uint16 IID =0x0003;
    uint16 IOP ;
    uint16 SendLen=0,RecvLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN] = {0};
    uint8 MeterNum=0;
    AmrThreadArg_T* pAmrThreadArg;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    OOP_OAD_U plcpipe = taskmng_pipe_oad_get(Pipe);
        //如果当前通道没表 不进行档案比对
//    if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
//    {
//        return 1;
//    }

    switch (pAmrThreadArg->TotalState.MeterFileCmp)
    {
    case MFCS_IDLE:                             //空闲
    {

        //清下载标志
        for (i = 0; i < pAmrThreadArg->TmnAmrInfo.MtrCnt; i++)
        {
            pAmrThreadArg->pMtrAmrInfo[i].SpecialFlag = 0;
        }

        if(pAmrThreadArg->pNeedAddMetHead != NULL)//首次需要全部删除
            pAmrThreadArg->pNeedAddMetHead = taskmng_module_head_node_delete(pAmrThreadArg->pNeedAddMetHead,MAX_AMR_COLLECT_CNT);

        if(pAmrThreadArg->pNeedDeleteMetHead != NULL)
            pAmrThreadArg->pNeedDeleteMetHead = taskmng_module_head_node_delete(pAmrThreadArg->pNeedDeleteMetHead,MAX_AMR_COLLECT_CNT);
#ifdef PRODUCT_CCU
        pAmrThreadArg->CmpCurIndex = 0;//林洋的ccoRouter序号从0开始
#else
        pAmrThreadArg->CmpCurIndex = 1;//
#endif
        pAmrThreadArg->TotalState.MeterFileCmp = MFCS_READ_AMOUNT_FILE;
        break;
    }
    case MFCS_READ_AMOUNT_FILE:                 //读取从节点数量
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案同步任务 读取从节点数量 ！\n");
        IOP = 0x003D;
        SendLen = 4;

        memset(SendBuffer,0x00,sizeof(SendBuffer));
        memset(RecvBuffer,0x00,sizeof(RecvBuffer));
        memcpy_r(SendBuffer,(uint8*)&plcpipe.value,4);
        ret = taskmng_msg_fun_deal(Pipe,IID,IOP, SendLen, SendBuffer, &RecvLen, RecvBuffer);
        if (ret > 0)
        {
            memcpy_r((uint8*)&pAmrThreadArg->TmnAmrInfo.SlaveCnt,RecvBuffer,4);
            TASK_FMT_DEBUG( Pipe, COMM_LOG, "从节点数量 %d  ！\n",pAmrThreadArg->TmnAmrInfo.SlaveCnt);
            //从路由中读取的从节点数量大于系统支持的最大数量时-容错
            if(pAmrThreadArg->TmnAmrInfo.SlaveCnt> MAX_AMR_COLLECT_CNT)
            {
                pAmrThreadArg->TmnAmrInfo.SlaveCnt = pAmrThreadArg->TmnAmrInfo.MtrCnt;
            }
            pAmrThreadArg->CmpResult = 0;
            pAmrThreadArg->TotalState.MeterFileCmp = MFCS_READ_INFO_FILE;
        }
        break;
    }
    case MFCS_READ_INFO_FILE:                   //读取从节点信息
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案同步任务 读取从节点信息！\n");
#ifdef PRODUCT_CCU
        if(pAmrThreadArg->CmpCurIndex >= pAmrThreadArg->TmnAmrInfo.SlaveCnt)
#else
        if(pAmrThreadArg->CmpCurIndex > pAmrThreadArg->TmnAmrInfo.SlaveCnt)
#endif
        {
            pAmrThreadArg->TotalState.MeterFileCmp = MFCS_RESULT_DEAL;
        }
        else
        {
            IOP = 0x0043;
            RecvLen = 0;
            memset(SendBuffer,0x00,sizeof(SendBuffer));
            memset(RecvBuffer,0x00,sizeof(RecvBuffer));
            index=0;
            memcpy_r(SendBuffer,(uint8*)&plcpipe.value,4);
            index+=4;
            memcpy_r(&SendBuffer[index], (uint8*)&pAmrThreadArg->CmpCurIndex, 2);
            index+=2;
            QueryMeterNum = pAmrThreadArg->TmnAmrInfo.SlaveCnt -pAmrThreadArg->CmpCurIndex+1;
            if(QueryMeterNum>MODULEDEAL_CNT)
                QueryMeterNum = MODULEDEAL_CNT;
            memcpy_r(&SendBuffer[index], (uint8*)&QueryMeterNum, 2);
            index+=2;
            ret = taskmng_msg_fun_deal(Pipe,IID,IOP, index, SendBuffer, &RecvLen, RecvBuffer);
            if(ret > 0)
            {
                MeterNum = RecvBuffer[0];  //查询返回的电表个数
                TASK_FMT_DEBUG( Pipe, COMM_LOG, " 读取从节点信息个数 %d \n",MeterNum);
                if(taskmng_meter_file_result_check(Pipe,&RecvBuffer[1],MeterNum) > 0)
                    pAmrThreadArg->CmpResult = 1;

                pAmrThreadArg->CmpCurIndex += MeterNum;
                if(pAmrThreadArg->CmpResult > 0 && gMeterFileModifyType[Pipe] == MODEFY_CLEAR)
                {
                    pAmrThreadArg->TotalState.MeterFileCmp = MFCS_RESULT_DEAL;
                    break;
                }

            }
        }
        break;
    }
    case MFCS_RESULT_DEAL:                      //结果处理
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "进入结果处理 ！\n");
        if(taskmng_meter_file_cmp_result_check(Pipe) > 0)
            pAmrThreadArg->CmpResult = 1;
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "CmpResult %d \n",pAmrThreadArg->CmpResult);
        //比较结果不同，置档案修改标志
        if (pAmrThreadArg->CmpResult == 1)
        {
            if(gMeterFileModifyType[Pipe] == MODEFY_CLEAR)
            {
                taskmng_all_meter_file_add_to_link(Pipe);
            }
            pAmrThreadArg->CmpResult = 0;
            gMeterFileModifyFlag[Pipe] = TRUE;             //直接进行修改
        }

        gMeterFileCmpFlag[Pipe] = FALSE;
        pAmrThreadArg->TotalState.MeterFileCmp = MFCS_IDLE;
        return 1;

        break;
    }
    default:
        pAmrThreadArg->TotalState.MeterFileCmp = MFCS_IDLE;
        break;
    }

    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_modify_deal
* 函数功能: 电表档案修改处理
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  完成
*           <0                  未完成
*******************************************************************************/
int taskmng_meter_file_modify_deal(uint8 Pipe)
{
    int ret;
    uint16 IID =0x0003;
    uint16 IOP ;
    uint16 SendLen=0,RecvLen=0;
    uint8 SendBuffer[MAX_MSG_LEN]={0};
    uint8 RecvBuffer[MAX_MSG_LEN] = {0};
    AmrThreadArg_T* pAmrThreadArg;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    pAmrThreadArg = &gAmrThreadArg[Pipe];
    //如果当前通道没表 不进行档案比对
    //if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
     //   return 1;
    switch(pAmrThreadArg->TotalState.MeterFileModify)
    {
    case MFMS_IDLE:
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案修改任务 gMeterFileModifyType %d  \n",gMeterFileModifyType[Pipe]);
        if(gMeterFileModifyType[Pipe] == MODEFY_F10)
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_FILE_DELETE;
        }else
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_FILE_CLEAR;
        }
        break;
    }
    case MFMS_FILE_CLEAR:
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案全清  \n");
        IOP = 0x0022;       //删除表档案
        memcpy_r(SendBuffer,(uint8*)&pipeOAD.value,4);
        SendLen+=4;
        memset(RecvBuffer,0x00,sizeof(RecvBuffer));
        ret = taskmng_msg_fun_deal(Pipe,IID,IOP, SendLen, SendBuffer, &RecvLen, RecvBuffer);
        if(ret>0)
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_FILE_ADD;
        }else
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_IDLE;
        }

        break;
    }
    case MFMS_FILE_ADD:       //下发档案
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案下发  \n");
        if(pAmrThreadArg->pNeedAddMetHead == NULL)
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_RESTART;
        }else
        {
            ret = taskmng_slave_info_add(Pipe, pAmrThreadArg->pNeedAddMetHead, MODULEDEAL_CNT);
            if(ret>0)
            {
                pAmrThreadArg->pNeedAddMetHead = taskmng_module_head_node_delete(pAmrThreadArg->pNeedAddMetHead, MODULEDEAL_CNT);
            }
        }
        break;
    }
    case MFMS_FILE_DELETE:
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案删除  \n");
        if(pAmrThreadArg->pNeedDeleteMetHead == NULL)
        {
            pAmrThreadArg->TotalState.MeterFileModify = MFMS_FILE_ADD;
        }else
        {
            ret = taskmng_router_info_delete(Pipe,pAmrThreadArg->pNeedDeleteMetHead,MODULEDEAL_CNT);
            if(ret>0)
            {
                pAmrThreadArg->pNeedDeleteMetHead = taskmng_module_head_node_delete(pAmrThreadArg->pNeedDeleteMetHead, MODULEDEAL_CNT);
            }
        }
        break;
    }
    case MFMS_RESTART:
    {
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案同步后重启  \n");
#ifdef AREA_HUNAN
        //档案同步后需要下发曲线方案
        taskmng_curve_sync_check(Pipe);
#endif
        gHPLCRouterState[Pipe]=0;  //重新查询路由状态
        if(gMeterFileModifyType[Pipe] == MODEFY_F10)
        {
            gMeterFileModifyType[Pipe] = MODEFY_CLEAR;
        }
        gMeterFileCmpFlag[Pipe] = TRUE;
        gMeterFileModifyFlag[Pipe] = FALSE;
        pAmrThreadArg->TotalState.MeterFileModify = MFMS_IDLE;
        pAmrThreadArg->TotalState.NormalAmr = RTS_IDLE; //重新抄表
       // taskmng_plc_task_queue_add(Pipe,MS_NORMAL_AMR); //档案任务修改完成后重新抄表
        return 1;
        break;
    }
    default:
        pAmrThreadArg->TotalState.MeterFileModify = MFMS_IDLE;
        break;
    }
    return -1;
}

/*******************************************************************************
* 函数名称: taskmng_slave_info_add_New
* 函数功能: 通讯:组帧 增加从节点信息
* 输入参数: PModuleInfo         新增从节点信息的
            AddNum              新增数目
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_slave_info_add(uint8 Pipe,MODULE_METADDRHEAD *PModuleInfo,uint8 AddNum)
{
    uint8       i;
    uint8       AddCnt=0;
    uint16      IID =0x0003;
    uint16      IOP = 0x0020 ;
    uint16      SendLen=0,RecvLen=0;
    uint8       SendBuffer[MAX_MSG_LEN]={0};
    uint8       RecvBuffer[MAX_MSG_LEN] = {0};

    OOP_OAD_U   pipeOAD = taskmng_pipe_oad_get(Pipe);
    SendLen++;                      //第一个字节为档案个数
    for(i=0;i<AddNum;i++)
    {
        if(PModuleInfo!=NULL)
        {
            TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案序号 %d prtl %d   \n",PModuleInfo->RouterSn,PModuleInfo->Prtl);
            TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案地址 0x%02x%02x%02x%02x%02x%02x   \n",PModuleInfo->Module_MetAddr[5],PModuleInfo->Module_MetAddr[4]
            ,PModuleInfo->Module_MetAddr[3],PModuleInfo->Module_MetAddr[2],PModuleInfo->Module_MetAddr[1],PModuleInfo->Module_MetAddr[0]);
            AddCnt++;

            memcpy_r(&SendBuffer[SendLen],(uint8*)&pipeOAD.value,4);   //端口号
            SendLen+=4;
            memcpy_r(&SendBuffer[SendLen],PModuleInfo->Module_MetAddr,6); //地址
            SendLen+=6;
            SendLen+=6;      //MAC暂时为空
            SendBuffer[SendLen++] = PModuleInfo->Prtl;
            //SendLen++;
        }else
            break;
        PModuleInfo = PModuleInfo->PNext;
    }
    SendBuffer[0] = AddCnt;
    return taskmng_msg_fun_deal(Pipe,IID,IOP,SendLen,SendBuffer,&RecvLen,RecvBuffer);
}

/*******************************************************************************
* 函数名称: taskmng_slave_info_add_New
* 函数功能: 通讯:组帧 增加从节点信息
* 输入参数: PModuleInfo         新增从节点信息的
            AddNum              新增数目
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_caj_info_add(uint8 Pipe,Collect_File_T *Collector,uint8 AddNum)
{
    uint8       i;
    uint8       AddCnt=0;
    uint16      IID =0x0003;
    uint16      IOP = 0x0020 ;
    uint16      SendLen=0,RecvLen=0;
    uint8       SendBuffer[MAX_MSG_LEN]={0};
    uint8       RecvBuffer[MAX_MSG_LEN] = {0};

    OOP_OAD_U   pipeOAD = taskmng_pipe_oad_get(Pipe);
    SendLen++;                      //第一个字节为档案个数
    for(i=0;i<AddNum;i++)
    {

        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案序号 %d prtl %d   \n",Collector->pn,Collector->prtl);
        TASK_FMT_DEBUG( Pipe, COMM_LOG, "档案地址 0x%02x%02x%02x%02x%02x%02x   \n",Collector->addr[5],Collector->addr[4]
        ,Collector->addr[3],Collector->addr[2],Collector->addr[1],Collector->addr[0]);
        AddCnt++;

        memcpy_r(&SendBuffer[SendLen],(uint8*)&pipeOAD.value,4);   //端口号
        SendLen+=4;

        memcpy_r(&SendBuffer[SendLen],Collector->addr,6); //地址
        SendLen+=6;
        SendLen+=6;      //MAC暂时为空
        SendBuffer[SendLen++] = Collector->prtl;
        SendLen++;

    }
    SendBuffer[0] = AddCnt;
    return taskmng_msg_fun_deal(Pipe,IID,IOP,SendLen,SendBuffer,&RecvLen,RecvBuffer);
}

/*******************************************************************************
* 函数名称: taskmng_router_info_delete
* 函数功能: 通讯:组帧 删除从节点信息
* 输入参数: PModuleInfo          删除的从节点信息
            DeleteNum            删除的数目
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_router_info_delete(uint8 Pipe,MODULE_METADDRHEAD *PModuleInfo,uint8 DeleteNum)
{
    uint8       i;
    uint8       DelCnt=0;
    uint16      IID =0x0003;
    uint16      IOP = 0x0021 ;
    uint16      SendLen=0,RecvLen=0;
    uint8       SendBuffer[MAX_MSG_LEN]={0};
    uint8       RecvBuffer[MAX_MSG_LEN] = {0};
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(Pipe);
    memcpy_r(SendBuffer,(uint8*)&pipeOAD.value,4);
    SendLen+=4;
    SendLen++;                      //第5个字节为档案个数
    for(i=0;i<DeleteNum;i++)
    {
        if(PModuleInfo!=NULL)
        {
            DelCnt++;
            memcpy_r(&SendBuffer[SendLen],(uint8*)&PModuleInfo->Module_MetAddr,6);//序号
            SendLen+=6;
        }else
            break;
        PModuleInfo = PModuleInfo->PNext;
    }
    SendBuffer[4] = DelCnt;
    return taskmng_msg_fun_deal(Pipe,IID,IOP,SendLen,SendBuffer,&RecvLen,RecvBuffer);
}

/*******************************************************************************
* 函数名称: taskmng_broad_delay_send
* 函数功能://发送获取广播延时命令
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_broad_delay_send(uint8 pipe,uint8* buf,uint16 len,uint8 prtl,uint16 *MsgIndex)
{
    uint8 payload[200]={0};
    uint16 index = 0;
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = prtl;
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
    memcpy(&payload[index],buf,len);
    index+=len;
    taskmng_msg_data_pack(pipe,0x0003,0x0039,index,payload,MsgIndex);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "发送获取广播延时命令 prtl %d  \n",prtl);
    TASK_BUF_DEBUG(pipe, RELAY_LOG,payload,index);
}

/*******************************************************************************
* 函数名称: taskmng_data_broad
* 函数功能://发送广播消息
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_data_broad(uint8 pipe,uint8* buf,uint16 len,uint8 prtl,uint16 *MsgIndex)
{
    uint8 payload[200]={0};
    uint16 index = 0;

    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = prtl;
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
    memcpy(&payload[index],buf,len);
    index+=len;
    taskmng_msg_data_pack(pipe,0x0003,0x003A,index,payload,MsgIndex);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "发送广播校时命令 prtl %d  \n",prtl);
    TASK_BUF_DEBUG(pipe, RELAY_LOG,payload,index);
}
/*******************************************************************************
* 函数名称: taskmng_transparent_broad_timing_send
* 函数功能:透明转发广播校时帧加上广播延时后发送
* 输入参数:pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_transparent_broad_timing_send(uint8 pipe,uint16 *Sendindex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //uint16 MsgIndex;
    uint8 buf[200]={0};
    uint16 buflen = 0;
    time_t settime = pAmrThreadArg->RelayTrans.BroadTime+pAmrThreadArg->RelayTrans.BroadDelay;
    switch(pAmrThreadArg->RelayTrans.prtl)
    {
    case PRTL_07:
    {
        rtc_t prtc;

        MeterPrtlFmt_T    Meter;
        memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
        memcpy(Meter.MtrAddr,BroadAddr645,6);
        Meter.CtrlCode = 0x08 ;
        Meter.DataLen = 0x06;
        TimeT2RTC(settime,&prtc);
        Meter.DataBuff[0] = prtc.ss;
        Meter.DataBuff[1] = prtc.nn;
        Meter.DataBuff[2] = prtc.hh;
        Meter.DataBuff[3] = prtc.dd;
        Meter.DataBuff[4] = prtc.mm;
        Meter.DataBuff[5] = prtc.yy;
        buflen = taskmng_meter_prtl_645_pack(buf,Meter);
    }
        break;
    case PRTL_OOP:
    {
        LinkLayer_t *pLinkLayer = NULL;
        uint8 tmpbuf[100]={0};
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        DateTimeHex_t TimeHex;
        Tsa.af = 0xC0;
        Tsa.add[0] = 0xAA;

        DT_Time2DateTimeHex(settime,&TimeHex);
        tmpbuf[index++] = 0x40;
        tmpbuf[index++] = 0x00;
        tmpbuf[index++] = 0x7f;
        tmpbuf[index++] = 0x00;
        tmpbuf[index++] = DT_DATETIME_S;
        memcpy(&tmpbuf[index],&TimeHex,7);
        index+=7;
        OopAction.requst(pipe,&Tsa,1,tmpbuf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        if(pLinkLayer!=NULL)
        {
            buflen = pLinkLayer->sendLen;
            memcpy(buf,pLinkLayer->sendBuff,buflen);
        }
    }
        break;
    }
    if(buflen>0)
    {
        taskmng_data_broad(pipe,buf,buflen,pAmrThreadArg->RelayTrans.prtl,Sendindex);
    }
}

/*******************************************************************************
* 函数名称: taskmng_transparent_broad_send
* 函数功能:透明转发广播校时帧的发送
* 输入参数:pipe 端口号
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_transparent_broad_send(uint8 pipe,uint16 *Sendindex)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    //uint16 MsgIndex;
    switch (pAmrThreadArg->RelayTrans.BroadStat)
    {
        //获取广播延时
        case TMN_TO_METER_GET_DELAY:
        {
            taskmng_broad_delay_send(pipe,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,pAmrThreadArg->RelayTrans.prtl,Sendindex);
            pAmrThreadArg->RelayTrans.DownIOP = 0x0039;
        }
            break;
        //发送广播校时命令
        case TMN_TO_METER_SETTIME_INIT:
        {
            taskmng_transparent_broad_timing_send(pipe,Sendindex);
            pAmrThreadArg->RelayTrans.DownIOP = 0x003A;
        }
            break;
        default:
            break;
    }
    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_oop_relay_oad_get
* 函数功能: 获取组帧穿透面向对象表需要抄读的Oad
* 输入参数:Pipe
        MtrIndex 穿透中电表索引
* 输出参数:DiInfo

* 返 回 值: 0    当前表已抄读完成
    Oad个数
*******************************************************************************/
uint8 taskmng_oop_relay_oad_get(uint8 pipe,uint8 MtrIndex,DI_INFO_T* DiInfo)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i;
    OadInfoMap OadMap ;
    uint16               OadLen = 0;
    uint8                OadNum = 0;
    uint16 Pn = taskmng_pn_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].Mtraddr,LOGIC_ID_YC);
    uint8 MtrEsamSerialNo[8]={0};
    uint8 MtrSecModEn;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_oop_relay_oad_get 当前oadindex %d  \n",pAmrThreadArg->RelayTrans.OadIndex );
      //判断安全模式是否启用
    if(taskmng_oop_meter_security_para_read(pipe,Pn,MtrEsamSerialNo,&MtrSecModEn)<0)
    {
        DiInfo->SubOad[0].nObjID = 0x4002;
        DiInfo->SubOad[0].attID = 2;
        DiInfo->SubOad[0].nIndex=0;
        DiInfo->SubOad[1].nObjID = 0xF101;
        DiInfo->SubOad[1].attID = 2;
        DiInfo->SubOad[1].nIndex=0;
        pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].IsNeedGetSafeMode = TRUE;
        DiInfo->SubOadNum = 2;
        return 2;
    }

    if(pAmrThreadArg->RelayTrans.OadIndex >= pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum )
    {
        pAmrThreadArg->RelayTrans.OadIndex = 0;
        pAmrThreadArg->RelayTrans.MapIndex = 0;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_oop_relay_oad_get 当前电表穿透完成  \n" );
        return 0;
    }
    pAmrThreadArg->RelayTrans.OopSendNum =0;
    for(i =pAmrThreadArg->RelayTrans.OadIndex ;i<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum;i++ )
    {
        if(taskmng_oad_info_get(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i],&OadMap )==FALSE)
        {
            pAmrThreadArg->RelayTrans.OopSendNum ++;
            continue;
        }
        TASK_FMT_DEBUG(pipe,RELAY_LOG,"taskmng_oop_relay_oad_get i %d oad %08x len %d \n",i,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i].value,OadMap.dataLen);
        if(OadLen+OadMap.dataLen > MAX_OAD_RD_LEN && OadLen!=0)
            break;
        OadLen+= OadMap.dataLen;
        DiInfo->SubOad[OadNum] = pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i];
        OadNum++;
    }
    DiInfo->SubOadNum = OadNum;
    pAmrThreadArg->RelayTrans.OopSendNum += OadNum;

    return 1;
}
/*******************************************************************************
* 函数名称: taskmng_645_relay_DI_get
* 函数功能: 645查找下一个抄读的数据项
* 输入参数:
* 输出参数:
*           DI                  找到的字典索引对应的97或07DI
* 返 回 值: =0                  该测点全部抄完
            >0                  在列表找到转换信息
*******************************************************************************/
int taskmng_645_relay_DI_get(uint8 pipe,uint8 MtrIndex,uint8 prtl,uint8* DI)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i,j;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_645_relay_DI_get 当前oadindex %d mapindex %d  \n",pAmrThreadArg->RelayTrans.OadIndex,pAmrThreadArg->RelayTrans.MapIndex );

    const  MapOopTo645_NEW*  pMap = MapOopTo645_RT;
    uint16 MapNum = Map_RT_len;

    for(i=pAmrThreadArg->RelayTrans.OadIndex;i<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum;i++)
    {
        for(j=pAmrThreadArg->RelayTrans.MapIndex;j<MapNum;j++)
        {
            if(pMap[j].Oad.value ==pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i].value )
            {
                if((prtl == PRTL_07&&pMap[j].DI07==0xffffffff)||(prtl == PRTL_97&&pMap[j].DI97==0xffff))
                    continue;
                if(j>1)
                {
                     //存在698和07一对一但是对应97多个的 会出现多条记录
                     if(prtl == PRTL_07 &&pMap[j].DI07==pMap[j-1].DI07 )
                         continue;
                }

                if(prtl == PRTL_07)
                {
                    memcpy(DI,&pMap[j].DI07,4);
                }else if(prtl == PRTL_97)
                {
                    memcpy(DI,&pMap[j].DI97,2);
                }


                pAmrThreadArg->RelayTrans.MapIndex=j;
                pAmrThreadArg->RelayTrans.OadIndex = i;
                TASK_FMT_DEBUG(pipe, RELAY_LOG,"645找到对应DI OAD 0X%08x OadIndex %d  MapIndex %d\n",pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i].value,i,j);

                return 1;
            }
        }
        pAmrThreadArg->RelayTrans.MapIndex = 0;
    }
    pAmrThreadArg->RelayTrans.OadIndex = 0;
    pAmrThreadArg->RelayTrans.MapIndex = 0;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_645_relay_DI_get 当前电表穿透完成  \n" );
    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_next_relay_DI_get
* 函数功能: 获取0x0010组帧穿透要抄读的数据项
* 输入参数:
* 输出参数: DiInfo
* 返 回 值:
*           1                  存在需要抄读的数据
*           0                  全部抄完
*******************************************************************************/
int taskmng_next_relay_DI_get(uint8 pipe,DI_INFO_T* DiInfo,uint8* prtl)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 i;
    //uint8 prtl;
    uint8 Addr[6]={0};
    int ret;
    for(i = pAmrThreadArg->RelayTrans.AmrIndex;i<pAmrThreadArg->RelayTrans.AmrNum;i++)
    {
        taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr,LOGIC_ID_YC,prtl);
        memcpy_r(pAmrThreadArg->RelayTrans.MtrAddr,pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr.add,6);

        memcpy_r(Addr,pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr.add,6);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_next_relay_DI_get 当前电表index %d prtl %d \n",i,*prtl);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "地址 0x%02x%02x%02x%02x%02x%02x \n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
        if(*prtl == PRTL_NULL)
            continue;
        if(pAmrThreadArg->RelayTrans.OadIndex == 0&&pAmrThreadArg->RelayTrans.MapIndex == 0)
        {
            pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime = time(NULL);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表第一次发送时间 %ld \n",pAmrThreadArg->RelayTrans.AmrNormal[i].SendTime);
        }
        if(*prtl == PRTL_OOP)
        {
            ret = taskmng_oop_relay_oad_get(pipe,i,DiInfo);
            if(ret >0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "面向对象表穿透 SubOadNum %d \n",DiInfo->SubOadNum);
                return 1;
            }else
                pAmrThreadArg->RelayTrans.AmrIndex++;
        }
        else
        {
            ret = taskmng_645_relay_DI_get(pipe,i,*prtl,DiInfo->DI);
            if(ret>0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "645表穿透 DI 0X%02x%02x%02x%02x \n",DiInfo->DI[3],DiInfo->DI[2],DiInfo->DI[1],DiInfo->DI[0]);
                return 1;
            }else
                pAmrThreadArg->RelayTrans.AmrIndex++;
        }
    }
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_relay_data_01_make
* 函数功能: 代理组抄表帧
* 输入参数: pipe 端口号
        DiInfo 数据项
        prtl  规约类型
* 输出参数:  DataBuff 报文内容
* 返 回 值:  报文长度
*******************************************************************************/
uint16 taskmng_relay_data_01_make(uint8 pipe,DI_INFO_T DiInfo,uint8 prtl,uint8* DataBuff)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    LinkLayer_t*            pLinkLayer = NULL;
    uint16 len = 0;
    uint8           InDataBuf[255];
    uint16          offset = 0;
    uint8 i;
    MeterPrtlFmt_T  MeterPrtlData;                      //电表规约格式数据

    uint8 AmrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
    MeterPrtlData.DelayFlag = 0;

    MeterPrtlData.PrtlMode = prtl;
    memcpy_r(MeterPrtlData.MtrAddr,pAmrThreadArg->RelayTrans.AmrNormal[AmrIndex].Mtraddr.add,6);

    if(prtl == PRTL_97)
    {
        MeterPrtlData.CtrlCode = 0x01;
        MeterPrtlData.DataLen = 2;
        memcpy(MeterPrtlData.DataBuff, DiInfo.DI, MeterPrtlData.DataLen);
        memcpy(pAmrThreadArg->RelayTrans.SendDI,DiInfo.DI,2);
    }
    else if(prtl == PRTL_07)
    {
        MeterPrtlData.CtrlCode = 0x11;
        MeterPrtlData.DataLen = 4;
        memcpy(MeterPrtlData.DataBuff, DiInfo.DI, MeterPrtlData.DataLen);
        memcpy(pAmrThreadArg->RelayTrans.SendDI,DiInfo.DI,4);
    }

    if(prtl == PRTL_OOP)
    {
        offset = 0;
        for(i = 0; i < DiInfo.SubOadNum; i++)
        {
            InDataBuf[offset++] = (uint8)(DiInfo.SubOad[i].nObjID >> 8);
            InDataBuf[offset++] = (uint8)(DiInfo.SubOad[i].nObjID);
            InDataBuf[offset++] = DiInfo.SubOad[i].attID;
            InDataBuf[offset++] = DiInfo.SubOad[i].nIndex;
        }
        OopGet.requst(pipe,&pAmrThreadArg->RelayTrans.AmrNormal[AmrIndex].Mtraddr,2,InDataBuf,offset);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        len = pLinkLayer->sendLen;
        memcpy(DataBuff,pLinkLayer->sendBuff,len);
    }
    else
    {
        len = taskmng_meter_prtl_645_pack(DataBuff, MeterPrtlData);
    }
    return len;
}

/*******************************************************************************
* 函数名称: taskmng_msg_0038_send
* 函数功能://组从节点监控发送
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_msg_0038_send(uint8 pipe,OOP_TSA_T Addr,uint16 Overtime,uint8 *buf,uint16 len,uint16 *MsgIndex)
{
    uint8 payload[500]={0};
    uint16 index = 0;
    uint8 prtl;
    taskmng_prtl_get_from_tsa(Addr,LOGIC_ID_YC,&prtl);
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    memcpy_r(&payload[index],(uint8*)&pipeoad.value,4);
    index+=4;
    payload[index++] = prtl;
    memcpy(&payload[index],Addr.add,6);
    index+=6;
    index+=2;
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
    memcpy(&payload[index],buf,len);
    index+=len;
    taskmng_msg_data_pack(pipe,0x0003,0x0038,index,payload,MsgIndex);

    TASK_BUF_TRACE(pipe, RELAY_LOG,payload,index);
}
/*******************************************************************************
* 函数名称: 电表数据召测组帧处理
* 函数功能:
* 输入参数: taskmng_meter_relay_data_send       电表的数据
* 输出参数:
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_meter_relay_data_send ( uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 Payload[MAX_MSG_LEN] = {0};
    uint8 index=0;
    uint8 prtl = 0;
    uint8 i;
    uint16 OverTime = 40;//超时时间统一40秒

    if(OverTime>pAmrThreadArg->RelayTrans.OverTime)
        OverTime = pAmrThreadArg->RelayTrans.OverTime;
    OOP_OAD_U pipeOAD = taskmng_pipe_oad_get(pipe);
    uint16 MsgIndex;


    switch(pAmrThreadArg->RelayTrans.IOP)
    {

    case 0x0010:
    {
        //广播校时的处理 首先发送获取广播延时 然后修正后进行广播校时
        if(pAmrThreadArg->RelayTrans.isBroad ==1)
        {
            taskmng_transparent_broad_send(pipe,&MsgIndex);
            break;
        }
        //只有3762用0033透传 其他都用0038从节点监控
        if(pAmrThreadArg->RelayTrans.frametype == PRTL_3762)
        {
#ifdef AREA_FUJIAN
            //福建协议下透传用0100
            if(gGatherMode == OOP_GATHER_FJ)
            {
                index = fujian_send_payload_pack_0100(pipe,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,Payload);
                fujian_trans_task_buf_send(pipe, 0x0100, Payload, index, &MsgIndex);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传发送 MsgIndex  %d payload \n",MsgIndex);
                TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
                pAmrThreadArg->RelayTrans.DownIOP = 0x0100;
            }else
            {
#endif
            memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            index+=2;
            memcpy_r(&Payload[index], (uint8 *) &OverTime, 2);
            index+=2;
            memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
            index+=2;
            if(pAmrThreadArg->RelayTrans.RelayLen <0x80)
            {
                Payload[index++] = pAmrThreadArg->RelayTrans.RelayLen;
            }
            else
            {
                Payload[index++] = 0x82;
                Payload[index++] = (uint8)(pAmrThreadArg->RelayTrans.RelayLen>>8);
                Payload[index++] = (uint8)pAmrThreadArg->RelayTrans.RelayLen;
            }
            memcpy(&Payload[index],pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            index+=pAmrThreadArg->RelayTrans.RelayLen;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);

            TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
            pAmrThreadArg->RelayTrans.DownIOP = 0x0033;
            taskmng_msg_data_pack(pipe,0x0003,0x0033,index,Payload,&MsgIndex);
#ifdef AREA_FUJIAN
            }
#endif
        }
        else
        {
            memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            Payload[index++] = pAmrThreadArg->RelayTrans.frametype;
            memcpy_r(&Payload[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
            index+=6;
            index+=2; //通信字
            memcpy_r(&Payload[index], (uint8 *) &pAmrThreadArg->RelayTrans.OverTime, 2);
            index+=2;
            memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
            index+=2;
            if(pAmrThreadArg->RelayTrans.RelayLen<0x80)
            {
                Payload[index++] = pAmrThreadArg->RelayTrans.RelayLen;
            }
            else
            {
                Payload[index++] = 0x82;
                Payload[index++] = (uint8)(pAmrThreadArg->RelayTrans.RelayLen>>8);
                Payload[index++] = (uint8)pAmrThreadArg->RelayTrans.RelayLen;
            }
            memcpy(&Payload[index],pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            index+=pAmrThreadArg->RelayTrans.RelayLen;

            TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);
            TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
            pAmrThreadArg->RelayTrans.DownIOP = 0x0038;
            taskmng_msg_data_pack(pipe,0x0003,0x0038,index,Payload,&MsgIndex);
        }
    }
        break;
    case 0x0011: //代理
    {
        switch(pAmrThreadArg->RelayTrans.proxychoice)
        {
        case 0x01:
        {
            DI_INFO_T DiInfo;
            uint8 DataBuff[1024]={0};
            uint16 DataLen;
            uint8 MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
            if(taskmng_next_relay_DI_get( pipe,&DiInfo,&prtl)>0)
            {
                //通过从节点监控下发
                DataLen = taskmng_relay_data_01_make(pipe,DiInfo,prtl,DataBuff);
                if(DataLen>0)
                {
                    memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
                    index+=4;
                    Payload[index++] = prtl;
                    memcpy_r(&Payload[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                    index+=6;
                    index+=2; //通信字
                    memcpy_r(&Payload[index], (uint8 *) &pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime, 2);
                    index+=2;
                    memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
                    index+=2;
                    if(DataLen<0x80)
                    {
                        Payload[index++] = DataLen;
                    }
                    else
                    {
                        Payload[index++] = 0x82;
                        Payload[index++] = (uint8)(DataLen>>8);
                        Payload[index++] = (uint8)DataLen;
                    }
                    memcpy(&Payload[index],DataBuff,DataLen);
                    index+=DataLen;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "ProxyGetRequestList datalen   %d DataBuff \n",DataLen);

                    TASK_BUF_DEBUG( pipe, RELAY_LOG, DataBuff, DataLen);
                    pAmrThreadArg->RelayTrans.DownIOP = 0x0038;
                    taskmng_msg_data_pack(pipe,0x0003,0x0038,index,Payload,&MsgIndex);
                }
            }
            else
                return 0;
        }
            break;
        case 0x02:
        {
            memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
            index+=4;
            taskmng_prtl_get_from_addr(pAmrThreadArg->RelayTrans.MtrAddr,LOGIC_ID_YC,&prtl);
            Payload[index++] = prtl;
            memcpy_r(&Payload[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
            index+=6;
            index+=2; //通信字
            memcpy_r(&Payload[index], (uint8 *) &OverTime, 2);
            index+=2;
            memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
            index+=2;
            if(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen <0x80)
            {
                Payload[index++] = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
            }
            else
            {
                Payload[index++] = 0x82;
                Payload[index++] = (uint8)(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen>>8);
                Payload[index++] = (uint8)pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
            }
            memcpy(&Payload[index],pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen);
            index+=pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "ProxyGetRequestRecord payloadlen  %d payload \n",index);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,Payload, index);
            pAmrThreadArg->RelayTrans.DownIOP = 0x0038;
            taskmng_msg_data_pack(pipe,0x0003,0x0038,index,Payload,&MsgIndex);

        }
            break;
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        {
            i = pAmrThreadArg->RelayTrans.AmrIndex;

            if(pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter == 0)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "代理 %d  AmrIndex %d 无效表 \n",pAmrThreadArg->RelayTrans.proxychoice,i);
                return -1;
            }
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "代理 %d 发送 AmrIndex %d \n",pAmrThreadArg->RelayTrans.proxychoice,i);

            TASK_BUF_DEBUG( pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
            pAmrThreadArg->RelayTrans.DownIOP = 0x0038;
            taskmng_msg_0038_send(pipe,pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr,pAmrThreadArg->RelayTrans.METER_LIST[i].overtime,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen,&MsgIndex);
            if(pAmrThreadArg->RelayTrans.METER_LIST[i].sendcnt==0)
            {
                pAmrThreadArg->RelayTrans.METER_LIST[i].firstSendtime = time(NULL);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表首次发送时间 %ld \n",pAmrThreadArg->RelayTrans.METER_LIST[i].firstSendtime);
            }
            pAmrThreadArg->RelayTrans.METER_LIST[i].sendcnt++;
            //return 1;

        }
            break;
        case 0x07:
            {
                //广播校时的处理 首先发送获取广播延时 然后修正后进行广播校时
                if(pAmrThreadArg->RelayTrans.isBroad ==1)
                {
                    taskmng_transparent_broad_send(pipe,&MsgIndex);
                    break;
                }
                if(pAmrThreadArg->RelayTrans.frametype == PRTL_3762)
                {
#ifdef AREA_FUJIAN
                    //福建协议下透传用0100
                    if(gGatherMode == OOP_GATHER_FJ)
                    {
                        index = fujian_send_payload_pack_0100(pipe,pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen,Payload);
                        fujian_trans_task_buf_send(pipe, 0x0100, Payload, index, &MsgIndex);
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传发送 MsgIndex  %d payload \n",MsgIndex);
                        TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
                        pAmrThreadArg->RelayTrans.DownIOP = 0x0100;
                    }else
                    {
#endif
                    memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
                    index+=4;
                    index+=2;//
                    memcpy_r(&Payload[index], (uint8 *) &OverTime, 2);
                    index+=2;
                    memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
                    index+=2;
                    if(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen <0x80)
                    {
                        Payload[index++] = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    }
                    else
                    {
                        Payload[index++] = 0x82;
                        Payload[index++] = (uint8)(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen>>8);
                        Payload[index++] = (uint8)pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    }
                    memcpy(&Payload[index],pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen);
                    index+=pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);

                    TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
                    pAmrThreadArg->RelayTrans.DownIOP = 0x0033;
                    taskmng_msg_data_pack(pipe,0x0003,0x0033,index,Payload,&MsgIndex);
#ifdef AREA_FUJIAN
                        }
#endif
                }else
                {
                    memcpy_r(&Payload[index],(uint8*)&pipeOAD.value,4);
                    index+=4;
                    Payload[index++] = pAmrThreadArg->RelayTrans.frametype;
                    memcpy_r(&Payload[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                    index+=6;
                    index+=2; //通信字
                    memcpy_r(&Payload[index], (uint8 *) &pAmrThreadArg->RelayTrans.OverTime, 2);
                    index+=2;
                    memcpy_r(&Payload[index], (uint8 *)&pAmrThreadArg->RelayTrans.ByteOverTime, 2);
                    index+=2;
                    if(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen <0x80)
                    {
                        Payload[index++] = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    }
                    else
                    {
                        Payload[index++] = 0x82;
                        Payload[index++] = (uint8)(pAmrThreadArg->RelayTrans.METER_LIST[0].setlen>>8);
                        Payload[index++] = (uint8)pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    }
                    memcpy(&Payload[index],pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen);
                    index+=pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据透传 payloadlen  %d payload \n",index);

                    TASK_BUF_TRACE( pipe, RELAY_LOG, Payload, index);
                    pAmrThreadArg->RelayTrans.DownIOP = 0x0038;
                    taskmng_msg_data_pack(pipe,0x0003,0x0038,index,Payload,&MsgIndex);
                }

            }
        break;
#ifdef AREA_FUJIAN
        case 50:
            {
                FRAME_CONVERT_T convert;
                uint8 dstbuf[2000]={0};
                uint8 sendbuf[2000]={0};
                uint16 sendlen;
                memset(&convert,0x00,sizeof(FRAME_CONVERT_T));
                convert.srcBuf = pAmrThreadArg->RelayTrans.METER_LIST[0].setaction;
                convert.srcLen = pAmrThreadArg->RelayTrans.METER_LIST[0].setlen;
                convert.dstBuf = dstbuf;
                Convert_Request_Frame(pipe,&convert,pAmrThreadArg->RelayTrans.reqframeformat,&pAmrThreadArg->RelayTrans.tsa);
                TASK_FMT_DEBUG(pipe, RELAY_LOG,"代理请求报文转换前内容 \n");
                TASK_BUF_DEBUG(pipe,RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.METER_LIST[0].setlen);
                TASK_FMT_DEBUG(pipe, RELAY_LOG,"请求报文格式字 %02x 电表地址 %02x%02x%02x%02x%02x%02x 转换后报文\n",pAmrThreadArg->RelayTrans.reqframeformat,
                    pAmrThreadArg->RelayTrans.tsa.add[0],pAmrThreadArg->RelayTrans.tsa.add[1],pAmrThreadArg->RelayTrans.tsa.add[2],
                    pAmrThreadArg->RelayTrans.tsa.add[3],pAmrThreadArg->RelayTrans.tsa.add[4],pAmrThreadArg->RelayTrans.tsa.add[5]);
                TASK_BUF_DEBUG(pipe,RELAY_LOG,convert.dstBuf,convert.dstLen);

                sendlen = fujian_send_payload_pack_0101(pipe, &pAmrThreadArg->RelayTrans.tsa,convert.dstBuf, convert.dstLen, sendbuf);
                pAmrThreadArg->RelayTrans.DownIOP = 0x0101;
                fujian_trans_task_buf_send(pipe, 0x0101, sendbuf, sendlen, &MsgIndex);
            }
        break;
#endif
        default:
            break;
        }
    }
        break;
    default:
        return 1;

    }
    pAmrThreadArg->RelayTrans.Sendindex = MsgIndex;
    return 1;

}
/*******************************************************************************
* 函数名称: 获取代理中每块表的超时时间
* 函数功能:taskmng_proxy_overtime_get
* 输入参数: overtime 代理配置的一个目标服务器的超时时间 0表示默认
* 输出参数: 无
* 返 回 值: 超时时间
*******************************************************************************/
uint16 taskmng_proxy_overtime_get(uint16 overtime)
{
    uint16 ret;

    if(overtime==0 ||overtime>60)
        ret = 60;
    else
        ret = overtime;
    return ret;
}

/*******************************************************************************
* 函数名称: TransparentBroadSend
* 函数功能:透明转发广播校时帧的接收
* 输入参数:pipe 端口号
         buf 收到的报文
         buflen收到的报文长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_transparent_broad_recv(uint8 pipe,uint8* buf,uint16 buflen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    switch(pAmrThreadArg->RelayTrans.BroadStat)
    {
    case TMN_TO_METER_GET_DELAY:
    {
        memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.BroadDelay,buf,2);
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "获取到广播延时 %d  \n",pAmrThreadArg->RelayTrans.BroadDelay);
        pAmrThreadArg->RelayTrans.BroadStat = TMN_TO_METER_SETTIME_INIT;
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
    }
        break;
    case TMN_TO_METER_SETTIME_INIT:
    {
        if(buf[0]==1)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到广播的确认帧\n");
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
            if(pAmrThreadArg->RelayTrans.IOP == 0x0011&&pAmrThreadArg->RelayTrans.proxychoice==0x07)
            {
                uint8 index = pAmrThreadArg->RelayTrans.RelayLen;
                pAmrThreadArg->RelayTrans.RelayData[index++]= 0x01;
                pAmrThreadArg->RelayTrans.RelayData[index++]= 0x00;
                pAmrThreadArg->RelayTrans.RelayLen = index;
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                break;
            }
            //广播校时的穿透回个空
            taskmng_relay_up_msg_write( pipe, 1);
            pAmrThreadArg->RelayTrans.RelayResult =1;
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
        }
    }
        break;
    default:
        break;
    }

}
/*******************************************************************************
* 函数名称: taskmng_relay_up_msg_write
* 函数功能: 上行回复穿透结果
* 输入参数: pipe 端口号
          result表示穿透结果 0表示成功 -1表示超时 1表示其他错误
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_relay_up_msg_write(uint8 pipe,int result)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 Payload[MAX_MSG_LEN]={0};
    uint16 offset =0 ;
    MESSAGE_INFO_T* MsgSend=NULL;

    if(pAmrThreadArg->RelayTrans.IOP==0x0010||pAmrThreadArg->RelayTrans.IOP==0x0011)

    {
        if(result==0)
        {
            memcpy(&Payload[offset],pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            offset+=pAmrThreadArg->RelayTrans.RelayLen;
        }else
        {
            Payload[offset++] = 0;
        }
    }


    TASK_FMT_DEBUG( pipe, RELAY_LOG, "组穿透上行报文 穿透结果 %d payload,数据长度 is %d\n",result,pAmrThreadArg->RelayTrans.RelayLen);

    TASK_BUF_DEBUG( pipe, RELAY_LOG, Payload, offset);
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+offset);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+offset);

    MsgSend->rpm = 0;
    MsgSend->index = pAmrThreadArg->RelayTrans.MsgIndex;
    MsgSend->priority = pAmrThreadArg->RelayTrans.MsgPRIORITY;
    MsgSend->label = pAmrThreadArg->RelayTrans.MsgLabel;
    MsgSend->IID = pAmrThreadArg->RelayTrans.IID;
    MsgSend->IOP = pAmrThreadArg->RelayTrans.IOP;

    MsgSend->sourlen = pAmrThreadArg->RelayTrans.SourLen;
    memcpy(MsgSend->souraddr,pAmrThreadArg->RelayTrans.SourAddr,MsgSend->sourlen);
    MsgSend->destlen = pAmrThreadArg->RelayTrans.DestLen;
    memcpy(MsgSend->destaddr,pAmrThreadArg->RelayTrans.DestAddr,MsgSend->destlen );
    MsgSend->msglen = offset;
    memcpy(MsgSend->playload,Payload,offset);

    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+offset);
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: taskmng_relay_data_01_unpack
* 函数功能: //解穿透回码
* 输入参数: pipe 端口号
        DataBuff 报文内容
        DataLen  报文长度
* 输出参数:  MeterPrtlData 645表解出来的内容
        apdu 698表解出来的apdu
* 返 回 值:  -1 失败 其他成功
*******************************************************************************/
int taskmng_relay_data_01_unpack(uint8 pipe,uint8* DataBuff,uint16 DataLen,MeterPrtlFmt_T*  MeterPrtlData,char* apdu)
{
    int ret = -1;
    AppLayerList_t* pAppLayer = NULL;
    ret = taskmng_645_meter_data_unpack(DataLen,DataBuff, MeterPrtlData->MtrAddr, &MeterPrtlData->CtrlCode, &MeterPrtlData->DataLen, MeterPrtlData->DataBuff);
    if(ret == CHECK_RESULT_FAILUER)
    {
        ret = LinkDataReceive(pipe, DataBuff, DataLen,NULL);
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
        if(ret >=eOopComm_Fail)//解帧失败直接返回
            return -1;
        //获取电表地址
        pAppLayer = FindAppLayer(pipe, NULL);
        if(pAppLayer == NULL)
            return -1;
        memcpy_r(MeterPrtlData->MtrAddr,pAppLayer->sa.add,6);

        MeterPrtlData->DataLen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
        memcpy(MeterPrtlData->DataBuff, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, MeterPrtlData->DataLen);
        memcpy(apdu,pAppLayer->recvApduList->apdu,2);
    }
    return ret;
}
/*******************************************************************************
* 函数名称: taskmng_645_relay_isvalid
* 函数功能: 验证645回码是否和发送对应
* 输入参数: pipe 端口号
        prtl  规约类型
        pRecvDI  收到的数据项
        CtrlCode 控制码
* 输出参数:
* 返 回 值:  -1 失败 1 成功
*******************************************************************************/
int taskmng_645_relay_isvalid(uint8 pipe,uint8 prtl,uint8* pRecvDI,uint8 CtrlCode,uint16* DataLen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint32                 SendDI = 0;
    uint32                 RecvDI = 0;
    if(prtl == PRTL_97)
    {
        if(pRecvDI!=NULL)
            memcpy(&RecvDI,pRecvDI,2);
        memcpy(&SendDI,pAmrThreadArg->RelayTrans.SendDI,2);
    }else
    {
        if(pRecvDI != NULL)
            memcpy(&RecvDI,pRecvDI,4);
        memcpy(&SendDI,pAmrThreadArg->RelayTrans.SendDI,4);
    }
    if(pRecvDI != NULL)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"RecvDI = %08lx  SendDI = %08lx \n",RecvDI,SendDI);
        if((RecvDI != SendDI) || ((CtrlCode & 0x80) == 0))
            return -1;
    }
    return 1;
}
/*******************************************************************************
* 函数名称: 组代理超时或者无效的服务器对象数据
* 函数功能:taskmng_proxy_error_data_pack
* 输入参数: pipe 端口号
        buf 数据域
        index 数据偏移
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_proxy_error_data_pack(uint8 pipe)
{
    RELAY_TRANS_T* RelayTrans;
    RelayTrans = &gAmrThreadArg[pipe].RelayTrans;
    uint16 index = RelayTrans->RelayLen;
    uint16 meterindex = RelayTrans->AmrIndex;
    uint8 i;
    if(RelayTrans->IOP != 0x0011)
    {
        return;
    }
    RelayTrans->RelayData[index++]=0x07;//tsa长度
    RelayTrans->RelayData[index++]=5;//
    memcpy(&RelayTrans->RelayData[index],RelayTrans->METER_LIST[meterindex].Mtraddr.add,6);
    index+=6;
    RelayTrans->RelayData[index++] = RelayTrans->METER_LIST[meterindex].OadNum;
    switch(RelayTrans->proxychoice)
    {
        case 0x03:
        {
            for(i=0;i<RelayTrans->METER_LIST[meterindex].OadNum;i++)
            {
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].SubOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0xFF;
            }

        }
            break;
        case 0x04:
        {
            for(i=0;i<RelayTrans->METER_LIST[meterindex].OadNum;i++)
            {
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].SetOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0x21;
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].GetOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0x00;
                RelayTrans->RelayData[index++]=0x21;
            }
        }
            break;
        case 0x05:
        {
            for(i=0;i<RelayTrans->METER_LIST[meterindex].OadNum;i++)
            {
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].SubOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0xFF;
                RelayTrans->RelayData[index++]=0x00;
            }
        }
            break;
        case 0x06:
        {
            for(i=0;i<RelayTrans->METER_LIST[meterindex].OadNum;i++)
            {
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].SetOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0x21;
                RelayTrans->RelayData[index++]=0x00;
                memcpy_r(&RelayTrans->RelayData[index],&RelayTrans->METER_LIST[meterindex].GetOad[i],sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                RelayTrans->RelayData[index++]=0x00;
                RelayTrans->RelayData[index++]=0x21;
            }
        }
            break;
        default:
            break;
    }
    RelayTrans->RelayLen = index;
}

/*******************************************************************************
* 函数名称: taskmng_relay_oad_pack
* 函数功能: //组每个电表的oad对应的结果
* 输入参数: pipe 端口号
        Pn 电表序号
        Oad 数据项
* 输出参数:  Buf 报文内容
* 返 回 值:
*******************************************************************************/
uint16 taskmng_relay_oad_pack(uint8 pipe,uint16 Pn,OOP_OAD_U Oad,uint8* Buf)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 tmpaddr[6];
    uint16 index=0,numoffset;
    int ret;
    uint8 i,num=0;
    OOP_OAD_U temoad;
    RECORD_UNIT_T RecordOut;
    READ_RECORD_T startReadRecord;
    OadInfoMap pOadInfo;
    memset(&pOadInfo,0x00,sizeof(OadInfoMap));
    memcpy_r(&Buf[index],(uint8* )&Oad,4);
    index+=4;
    ret = taskmng_oad_info_get( Oad,&pOadInfo);
    if(ret ==FALSE)
    {
        Buf[index++]=0;
        Buf[index++]=0xFF;
        return index;
    }

    temoad=Oad;
    if(TRUE==taskmng_need_merge_check(&pOadInfo))
    {
        temoad.nIndex=0x00;
    }
    memset((uint8*)&startReadRecord,0x00,sizeof(READ_RECORD_T));
    memset((uint8*)&RecordOut,0x00,sizeof(RECORD_UNIT_T));
    startReadRecord.recordOAD.road.oadMain.value = 0x00000000;
    startReadRecord.recordOAD.road.oadCols.nNum = 1;
    startReadRecord.recordOAD.road.oadCols.oad[0] =temoad;
    startReadRecord.recordOAD.logicId = 100;
    startReadRecord.recordOAD.infoNum = Pn;
    startReadRecord.recordOAD.classtag =5;
    startReadRecord.MAC.nNum=6;
    taskmng_mtr_addr_get(Pn,LOGIC_ID_YC,tmpaddr);
    memcpy_r(startReadRecord.MAC.value,tmpaddr,6);
    //taskmng_mtr_addr_get( Pn, (uint8*)startReadRecord.MAC.value);
    startReadRecord.cType = 0x01;
    startReadRecord.cStart = 0;
#if IS_NEED_ADD_8HOUR
    startReadRecord.cEnd = time(NULL)+1+8*60*60;
#else
    startReadRecord.cEnd = time(NULL)+1;
#endif

    ret = db_read_record(pAmrThreadArg->DBClientHandle,&startReadRecord,DATA_LEN_MAX,&RecordOut);

    TASK_FMT_DEBUG( pipe, RELAY_LOG, " record_data_read ret %d PN %d Oad 0x%08x datalen %d \n",ret,Pn,Oad.value,RecordOut.record[0].outDatalen);

    if(ret!=0)
    {
        Buf[index++]=0;
        Buf[index++]=0x06;
        return index;
    }else
    {

        TASK_BUF_TRACE( pipe, RELAY_LOG, RecordOut.record[0].outDataBuf,RecordOut.record[0].outDatalen);
        Buf[index++]=1;
        switch(pOadInfo.CovertSruct)
        {
        case E_ESAM_COUNT:
        {
            OOP_ESAM_COUNT_T esam;
            memcpy(&esam,RecordOut.record[0].outDataBuf,sizeof(OOP_ESAM_COUNT_T));
            Buf[index++] = DT_STRUCTURE;
            Buf[index++] = 3;
            Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
            memcpy_r(&Buf[index], (uint8*)&esam.negoCnt, 4);
            index+=4;
            Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
            memcpy_r(&Buf[index], (uint8*)&esam.reportCnt, 4);
            index+=4;
            Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
            memcpy_r(&Buf[index], (uint8*)&esam.seq, 4);
            index+=4;

        }
        break;
        case E_OOP_ENERGY_T:
        {
            OOP_ENERGY_T Energy;
            num =0;
            memcpy(&Energy,RecordOut.record[0].outDataBuf,sizeof(OOP_ENERGY_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                numoffset=index;
                //Buf[index++] = Energy.nNum;
                index++;
                for(i=0;i<Energy.nNum;i++)
                {
                    if(Energy.nValue[i]==0xFFFFFFFF)
                    {
                        continue;
                    }
                    num++;
                    Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;

                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[i], 4);
                    index+=4;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[Oad.nIndex-1], 4);
                    index+=4;
            }
        }
            break;
        case E_OOP_ENERGYS_T:
        {
            OOP_ENERGYS_T Energy;
            num =0;
            memcpy(&Energy,RecordOut.record[0].outDataBuf,sizeof(OOP_ENERGYS_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                 numoffset=index;
                //Buf[index++] = Energy.nNum;
                index++;
                for(i=0;i<Energy.nNum;i++)
                {
                    if(Energy.nValue[i]==0xFFFFFFFF)
                    {
                        continue;
                    }
                    num++;
                    Buf[index++] = DT_DOUBLE_LONG;//
                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[i], 4);
                    index+=4;
                }
                Buf[numoffset] = num;
            }
            else
            {
                Buf[index++] = DT_DOUBLE_LONG;
                memcpy_r(&Buf[index], (uint8*)&Energy.nValue[Oad.nIndex-1], 4);
                index+=4;
            }
        }
            break;
        case E_OOP_HENERGY_T:
        {
            OOP_HENERGY_T Energy;

            memcpy(&Energy,RecordOut.record[0].outDataBuf,sizeof(OOP_HENERGY_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                //Buf[index++] = Energy.nNum;
                num = 0;
                numoffset=index;
                index++;

                for(i=0;i<Energy.nNum;i++)
                {
                    if(Energy.nValue[i]==0xFFFFFFFFFFFFFFFF)
                    {
                        continue;
                    }
                    num++;
                    
                    Buf[index++] = DT_LONG64_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[i], 8);
                    index+=8;
                }
                Buf[numoffset] = num;
            }
            else
            {
                Buf[index++] = DT_LONG64_UNSIGNED;
                memcpy_r(&Buf[index], (uint8*)&Energy.nValue[Oad.nIndex-1], 4);
                index+=4;
            }
        }
            break;
        case E_OOP_HENERGYS_T://高通精度电量带符号
        {
            OOP_HENERGYS_T Energy;

            memcpy(&Energy,RecordOut.record[0].outDataBuf,sizeof(OOP_HENERGYS_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                //Buf[index++] = Energy.nNum;
                num = 0;
                numoffset=index;
                index++;

                for(i=0;i<Energy.nNum;i++)
                {
                    if(Energy.nValue[i]==0xFFFFFFFFFFFFFFFF)
                    {
                        continue;
                    }
                    num++;
                    
                    Buf[index++] = DT_LONG64;
                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[i], 8);
                    index+=8;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] = DT_LONG64;
                    memcpy_r(&Buf[index], (uint8*)&Energy.nValue[Oad.nIndex-1], 8);
                    index+=8;
            }

        }
            break;
        case E_OOP_DEMAND_T:
        {
            OOP_DEMAND_T demand;
            memcpy(&demand,RecordOut.record[0].outDataBuf,sizeof(OOP_DEMAND_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                //Buf[index++] = demand.nNum;
                num = 0;
                numoffset=index;
                index++;

                for(i=0;i<demand.nNum;i++)
                {
                    if(0==get_bit_value((uint8*)&demand.rsv,32,i))
                    {
//                        Buf[index++] = DT_NULL;
//                        Buf[index++] = DT_NULL;
                        continue;
                    }
                    num++;
                    Buf[index++] = 0x02;
                    Buf[index++] = 0x02;
                    Buf[index++] =DT_DOUBLE_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[i].nValue, 4);
                    index+=4;
                    Buf[index++] = DT_DATETIME_S;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[i].time.year, 2);
                    index+=2;
                    Buf[index++] =demand.demand[i].time.month;
                    Buf[index++] =demand.demand[i].time.day;
                    Buf[index++] =demand.demand[i].time.hour;
                    Buf[index++] =demand.demand[i].time.minute;
                    Buf[index++] =demand.demand[i].time.second;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] = 0x02;
                    Buf[index++] = 0x02;
                    Buf[index++] =DT_DOUBLE_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[Oad.nIndex-1].nValue, 4);
                    index+=4;
                    Buf[index++] = DT_DATETIME_S;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[Oad.nIndex-1].time.year, 2);
                    index+=2;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.month;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.day;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.hour;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.minute;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.second;
            }
        }
            break;
        case E_OOP_DEMANDS_T:
        {
            OOP_DEMAND_T demand;
            memcpy(&demand,RecordOut.record[0].outDataBuf,sizeof(OOP_DEMAND_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组
                //Buf[index++] = demand.nNum;
                
                num = 0;
                numoffset=index;
                index++;

                for(i=0;i<demand.nNum;i++)
                {
                    if(0==get_bit_value((uint8*)&demand.rsv,32,i))
                    {
//                        Buf[index++] = DT_NULL;
//                        Buf[index++] = DT_NULL;
                        continue;
                    }
                    num++;
                    Buf[index++] = 0x02;
                    Buf[index++] = 0x02;
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[i].nValue, 4);
                    index+=4;
                    Buf[index++] = DT_DATETIME_S;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[i].time.year, 2);
                    index+=2;
                    Buf[index++] =demand.demand[i].time.month;
                    Buf[index++] =demand.demand[i].time.day;
                    Buf[index++] =demand.demand[i].time.hour;
                    Buf[index++] =demand.demand[i].time.minute;
                    Buf[index++] =demand.demand[i].time.second;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] = 0x02;
                    Buf[index++] = 0x02;
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[Oad.nIndex-1].nValue, 4);
                    index+=4;
                    Buf[index++] = DT_DATETIME_S;
                    memcpy_r(&Buf[index], (uint8*)&demand.demand[Oad.nIndex-1].time.year, 2);
                    index+=2;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.month;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.day;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.hour;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.minute;
                    Buf[index++] =demand.demand[Oad.nIndex-1].time.second;
            }
        }
            break;
        case E_OOP_DWORD3V_T://分相变量
        {
            OOP_DWORD3V_T DWORD3V_T;
            num =0;
            memcpy(&DWORD3V_T,RecordOut.record[0].outDataBuf,sizeof(OOP_DWORD3V_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                 numoffset=index;
                //Buf[index++] = Energy.nNum;
                index++;
                //Buf[index++] = DWORD3V_T.nNum;

                for(i=0;i<DWORD3V_T.nNum;i++)
                {
                    if(DWORD3V_T.nValue[i]==0xFFFFFFFF)
                    {
                        continue;
                    }
                    Buf[index++] =DT_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&DWORD3V_T.nValue[i], 2);
                    index+=2;
                    num++;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] =DT_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&DWORD3V_T.nValue[Oad.nIndex-1], 2);
                    index+=2;
            }
        }
            break;
        case E_OOP_WORD3V_T://电压
        {
            OOP_WORD3V_T WORD3V_T;
            num =0;
            memcpy(&WORD3V_T,RecordOut.record[0].outDataBuf,sizeof(OOP_WORD3V_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                 numoffset=index;
                //Buf[index++] = Energy.nNum;
                index++;
                //Buf[index++] = DWORD3V_T.nNum;

                for(i=0;i<WORD3V_T.nNum;i++)
                {
                    if(WORD3V_T.nValue[i]==0xFFFF)
                    {
                        continue;
                    }
                    Buf[index++] =DT_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&WORD3V_T.nValue[i], 2);
                    index+=2;
                    num++;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] =DT_LONG_UNSIGNED;
                    memcpy_r(&Buf[index], (uint8*)&WORD3V_T.nValue[Oad.nIndex-1], 2);
                    index+=2;
            }
        }
            break;
        case E_OOP_INT3V_T://功率变量
        {
            OOP_INT3V_T  INT3V_T;
            num =0;
            memcpy(&INT3V_T,RecordOut.record[0].outDataBuf,sizeof(OOP_INT3V_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组
                numoffset=index;
                Buf[index++] = INT3V_T.nNum;

                for(i=0;i<INT3V_T.nNum;i++)
                {
                    if(INT3V_T.nValue[i]==0xFFFFFFFF)
                    {
                        continue;
                    }
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&INT3V_T.nValue[i], 4);
                    index+=4;
                    num++;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&INT3V_T.nValue[Oad.nIndex-1], 4);
                    index+=4;
            }
        }
            break;
        case E_OOP_INT4V_T://功率变量
        {
            OOP_INT4V_T  INT4V_T;
            num =0;
            memcpy(&INT4V_T,RecordOut.record[0].outDataBuf,sizeof(OOP_INT4V_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组
                numoffset=index;
                Buf[index++] = INT4V_T.nNum;

                for(i=0;i<INT4V_T.nNum;i++)
                {
                    if(INT4V_T.nValue[i]==0xFFFFFFFF)
                    {
                        continue;
                    }
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&INT4V_T.nValue[i], 4);
                    index+=4;
                    num++;
                }
                Buf[numoffset] = num;
            }
            else
            {
                    Buf[index++] =DT_DOUBLE_LONG;
                    memcpy_r(&Buf[index], (uint8*)&INT4V_T.nValue[Oad.nIndex-1], 4);
                    index+=4;
            }
        }
            break;
        case E_FRZ_DATETIME_S:
        case E_REAL_DATETIME_S:
        case E_DATETIME_S:
        {
            Buf[index++] = DT_DATETIME_S;
            memcpy_r(&Buf[index], RecordOut.record[0].outDataBuf, 2);
            index+=2;
            memcpy(&Buf[index],RecordOut.record[0].outDataBuf+2,5);
            index+=5;
        }
            break;
        case E_DOUBLE_LONG_UNSIGNED:
        {
            Buf[index++] =DT_DOUBLE_LONG_UNSIGNED;
            memcpy_r(&Buf[index], RecordOut.record[0].outDataBuf, 4);
            index+=4;
        }
            break;
        case E_DOUBLE_LONG:
        {
            Buf[index++] =DT_DOUBLE_LONG;
            memcpy_r(&Buf[index], RecordOut.record[0].outDataBuf, 4);
            index+=4;
        }
            break;
        case E_OOP_METWORD_T:
        {
            OOP_METWORD_T METWORD_T;
            memcpy(&METWORD_T,RecordOut.record[0].outDataBuf,sizeof(OOP_METWORD_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                Buf[index++] = METWORD_T.nNum;

                for(i=0;i<METWORD_T.nNum;i++)
                {
                    Buf[index++] =DT_BIT_STRING;
                    Buf[index++] =0x10;
                    memcpy(&Buf[index], (uint8*)&METWORD_T.metWord[i], 2);
                    membitrev(&Buf[index],16);
                    index+=2;
                }
            }
            else
            {
                    Buf[index++] =DT_BIT_STRING;
                    Buf[index++] =0x10;
                    memcpy(&Buf[index], (uint8*)&METWORD_T.metWord[Oad.nIndex-1], 2);
                    membitrev(&Buf[index],16);
                    index+=2;
            }
        }
            break;
        case E_OOP_LONG4V_T:
        {
            OOP_LONG4V_T LONG4V_T;
            memcpy(&LONG4V_T,RecordOut.record[0].outDataBuf,sizeof(OOP_LONG4V_T));
            if(Oad.nIndex ==0)
            {
                Buf[index++] = DT_ARRAY;  //数组

                Buf[index++] = LONG4V_T.nNum;

                for(i=0;i<LONG4V_T.nNum;i++)
                {
                    Buf[index++] =DT_LONG;
                    memcpy_r(&Buf[index], (uint8*)&LONG4V_T.nValue[i], 2);
                    index+=2;
                }
            }
            else
            {
                    Buf[index++] =DT_LONG;
                    memcpy_r(&Buf[index], (uint8*)&LONG4V_T.nValue[Oad.nIndex-1], 2);
                    index+=2;
            }
        }
            break;
        case E_OOP_EVTSTAT_T:
        {

            OOP_EVTSTAT_T evtStat;

            memcpy(&evtStat,RecordOut.record[0].outDataBuf,sizeof(OOP_EVTSTAT_T));

            Buf[index++] = DT_ARRAY;  //数组
            Buf[index++] = evtStat.nNum;

            for(i=0;i<evtStat.nNum;i++)
            {
                Buf[index++] =DT_STRUCTURE;
                Buf[index++] =0x02;

                switch(evtStat.stat[i].source.choice)
                {
                    case DT_OAD:
                    {
                        Buf[index++] =DT_OAD;
                        memcpy_r(Buf+index, (uint8*)&evtStat.stat[i].source.oad.value, sizeof(OOP_OAD_U));
                        index=index+sizeof(OOP_OAD_U);

                    }break;

                    case DT_TSA:
                    {
                        Buf[index++] =DT_TSA;
                        Buf[index++] =evtStat.stat[i].source.add.af+2;
                        memcpy(Buf+index,(uint8*)&evtStat.stat[i].source.add,evtStat.stat[i].source.add.af+2);
                        index=index+evtStat.stat[i].source.add.af+2;
                    }break;

                    case DT_OI:
                    {
                        Buf[index++] =DT_TSA;
                        memcpy_r(Buf+index,(uint8*)&evtStat.stat[i].source.oi,sizeof(OI));
                        index=index+sizeof(OI);
                    }break;

                    case DT_UNSIGNED:
                    {
                        Buf[index++] = DT_UNSIGNED;
                        Buf[index++] = evtStat.stat[i].source.nValue;
                    }break;

                    case DT_NULL:
                    {
                        Buf[index++] = 0x00;
                    }
                    break;  //为空，没东西需要转换

                    default:
                    {

                    }break;
                }

                Buf[index++] = DT_STRUCTURE;
                Buf[index++] = 0x02;
                Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
                memcpy_r(Buf+index,(uint8*)&evtStat.stat[i].dwCount,sizeof(uint32));
                index=index+sizeof(uint32);
                Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
                memcpy_r(Buf+index,(uint8*)&evtStat.stat[i].dwTimes,sizeof(uint32));
                index=index+sizeof(uint32);

            }
        }
        break;
        case E_OOP_EVTSTAT1_T:
        {

            OOP_MEVTSTAT_T evtStat;
            memcpy(&evtStat,RecordOut.record[0].outDataBuf,sizeof(OOP_MEVTSTAT_T));
            Buf[index++] = DT_ARRAY;  //数组
            Buf[index++] = evtStat.nNum;

            for(i=0;i<evtStat.nNum;i++)
            {
                Buf[index++] = DT_STRUCTURE;
                Buf[index++] = 0x02;
                Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
                memcpy_r(Buf+index,(uint8*)&evtStat.stat[i].dwCount,sizeof(uint32));
                index=index+sizeof(uint32);
                Buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
                memcpy_r(Buf+index,(uint8*)&evtStat.stat[i].dwTimes,sizeof(uint32));
                index=index+sizeof(uint32);

            }
        }
        break;
        case E_OOP_OCTETVAR64_T:
        {

            OOP_OCTETVAR64_T data;
            memcpy(&data,RecordOut.record[0].outDataBuf,sizeof(OOP_OCTETVAR64_T));
            Buf[index++] = DT_OCTET_STRING;  //数组
            Buf[index++] = data.nNum;
            memcpy(Buf+index,data.value,data.nNum);
            index+=data.nNum;
        }
        break;
        case E_OOP_HARMONIC_T:
        {
            OOP_HARMONIC_T data;
            memcpy(&data,RecordOut.record[0].outDataBuf,sizeof(data));
            Buf[index++] = DT_ARRAY;  //数组
            Buf[index++] = data.nNum;
            for(i=0;i<data.nNum;i++)
            {
                Buf[index++] = DT_LONG;
                memcpy(&Buf[index],&data.nValue[i],2);
                index+=2;
            }
        }
            break;
        case E_OOP_LONG3V_T:
        {
            OOP_LONG3V_T data;
            memcpy(&data,RecordOut.record[0].outDataBuf,sizeof(data));
            Buf[index++] = DT_ARRAY;  //数组
            Buf[index++] = data.nNum;
            for(i=0;i<data.nNum;i++)
            {
                Buf[index++] = DT_LONG;
                memcpy(&Buf[index],&data.nValue[i],2);
                index+=2;
            }
        }
            break;
        default:
            break;
        }
    }
    //删除存储的临时数据
    RECORD_OAD_T RecordDel;
    RecordDel.logicId = 100;
    RecordDel.infoNum = Pn;
    RecordDel.road.oadMain.value = 0;
    RecordDel.road.oadCols.nNum=1;
    RecordDel.road.oadCols.oad[0]=temoad;
    RecordDel.classtag = 5;
    RecordDel.optional =0;
    CLEAR_DATA_T cleardata;
    cleardata.recordOAD = RecordDel;
    cleardata.bClassTag = FALSE;
    ret = db_clear_oad_data(pAmrThreadArg->DBClientHandle,&cleardata);

    if(ret==0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据删除成功！\n");
    }else
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "数据删除失败！\n");
    }
    return index;
}

/*******************************************************************************
* 函数名称: taskmng_relay_0011_1_pack
* 函数功能: //获取每个电表的组帧穿透0011查询到的若干个对象属性及结果
* 输入参数: pipe 端口号
        MtrIndex 电表索引
        Pn 电表序号

* 输出参数:  Buf 报文内容
* 返 回 值:
*******************************************************************************/
uint16 taskmng_relay_0011_1_pack(uint8 pipe,uint8 MtrIndex,uint16 Pn,uint8* Buf)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 index=0;
    uint8 i;
    OOP_OAD_U Oad;
    Buf[index++] = pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum;
    for(i=0;i<pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OadNum;i++)
    {
        Oad = pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SubOad[i];
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"taskmng_relay_0011_1_packMtrIndex%d  i %d oad 0x%08x \n",MtrIndex,i,Oad.value);
        index+=taskmng_relay_oad_pack(pipe,Pn,Oad,&Buf[index]);
    }
//  Buf[index++] = 0x00;  //跟随上报信息域
//    Buf[index++] = 0x00;  //时间标签域
    return index;
}

/*******************************************************************************
* 函数名称: taskmng_proxy_get_response_list_up_msg_write
* 函数功能: 请求代理读取若干个服务器的若干个对象属性回码
* 输入参数: pipe 端口号

* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_proxy_get_response_list_up_msg_write(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 i,j;
    uint16 index=0;
    uint16 Pn;
    OOP_TSA_T tsa;
    uint8 tmpPayLoad[MAX_MSG_LEN] = {0};
    uint8 PayLoad[MAX_MSG_LEN] = {0};
    uint16 PayLoadLen = 0;

    MESSAGE_INFO_T* MsgSend =NULL;
    tmpPayLoad[index++] = 0x89;
    tmpPayLoad[index++] = 0x01;
    tmpPayLoad[index++] = pAmrThreadArg->RelayTrans.piid;
//    tmpPayLoad[index++]=pAmrThreadArg->RelayTrans.AmrNum;
    tmpPayLoad[index++]=pAmrThreadArg->RelayTrans.TotalNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.TotalNum);
    //for(i=0;i<pAmrThreadArg->RelayTrans.AmrNum;i++)
    for(i=0;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter is %d,i is %d\n",pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter,i);
        if(pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter==0x00)
        {
            tsa = pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr;
            tmpPayLoad[index++] = tsa.len+2; //TSA长度
            tmpPayLoad[index++] = tsa.af;
            if(tsa.flag == 1)
            {
                tmpPayLoad[index++] = tsa.vxd;
                memcpy(&tmpPayLoad[index],tsa.add,tsa.len);
                index+=tsa.len;
            }else
            {
                memcpy(&tmpPayLoad[index],tsa.add,tsa.len+1);
                index+=tsa.len+1;
            }

            tmpPayLoad[index++] = pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;
            for(j=0;j<pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;j++)
            {
                memcpy_r(tmpPayLoad+index,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[j].value,sizeof(OOP_OAD_U));
                index=index+sizeof(OOP_OAD_U);
                tmpPayLoad[index++] =0x00;
                tmpPayLoad[index++] =0xFF;
            }
        }
        else
        {
            tsa = pAmrThreadArg->RelayTrans.AmrNormal[i].Mtraddr;
            tmpPayLoad[index++] = tsa.len+2; //TSA长度
            tmpPayLoad[index++] = tsa.af;
            if(tsa.flag == 1)
            {
                tmpPayLoad[index++] = tsa.vxd;
                memcpy(&tmpPayLoad[index],tsa.add,tsa.len);
                index+=tsa.len;
            }else
            {
                memcpy(&tmpPayLoad[index],tsa.add,tsa.len+1);
                index+=tsa.len+1;
            }
            Pn = taskmng_pn_get_from_tsa(tsa,LOGIC_ID_YC);
            index+=taskmng_relay_0011_1_pack(pipe,i,Pn,&tmpPayLoad[index]);
        }
    }
    tmpPayLoad[index++] = 00;
    tmpPayLoad[index++] = 00;

    if(index<0x80)
    {
        PayLoad[PayLoadLen++]=index;
    }else
    {
        PayLoad[PayLoadLen++] = 0x82;
        PayLoad[PayLoadLen++] = (uint8)(index>>8) ;
        PayLoad[PayLoadLen++] = (uint8)index ;
    }
    memcpy(&PayLoad[PayLoadLen],tmpPayLoad,index);
    PayLoadLen+=index;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "ProxyGetResponseList 组穿透上行报文 payloadlen %d payload \n",PayLoadLen);

    TASK_BUF_DEBUG( pipe, RELAY_LOG, PayLoad, PayLoadLen);
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+PayLoadLen);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+PayLoadLen);

    MsgSend->rpm = 0;
    MsgSend->index = pAmrThreadArg->RelayTrans.MsgIndex;
    MsgSend->priority = pAmrThreadArg->RelayTrans.MsgPRIORITY;
    MsgSend->label = pAmrThreadArg->RelayTrans.MsgLabel;
    MsgSend->IID = pAmrThreadArg->RelayTrans.IID;
    MsgSend->IOP = pAmrThreadArg->RelayTrans.IOP;

    MsgSend->sourlen = pAmrThreadArg->RelayTrans.SourLen;
    memcpy(MsgSend->souraddr,pAmrThreadArg->RelayTrans.SourAddr,MsgSend->sourlen);
    MsgSend->destlen = pAmrThreadArg->RelayTrans.DestLen;
    memcpy(MsgSend->destaddr,pAmrThreadArg->RelayTrans.DestAddr,MsgSend->destlen );
    MsgSend->msglen = PayLoadLen;
    memcpy(MsgSend->playload,PayLoad,PayLoadLen);

    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+PayLoadLen);
    free(MsgSend);
}
/*******************************************************************************
* 函数名称: 中继转发内部流程
* 函数功能:
* 输入参数: 无
* 输出参数: 无
* 返 回 值: >0                  成功
*           <0                  失败
*******************************************************************************/
int taskmng_relay_trans_deal ( uint8 pipe )
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    static time_t  TaskBeginTime[PLC_NUM];
    static time_t  BeginTime[PLC_NUM];
    static uint16  OverTime[PLC_NUM]; //超时时间
    int ret;
    int RecvLen=0;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    uint8 meterindex=0;
    MESSAGE_INFO_T* MsgInfo=NULL;
    uint8 PayloadBuf[MAX_MSG_LEN]={0};

    MeterPrtlFmt_T  MeterPrtlData;
    char            apdu[2];
    OOP_OAD_U oad;
    uint8 tmppipe;
    uint8 MtrIndex;
    uint16 index;
    if((pAmrThreadArg->RelayTrans.proxychoice==0x07)&&(pAmrThreadArg->RelayTrans.IOP == 0x0011))
    {
        oad.value = pAmrThreadArg->RelayTrans.OadPort;
        tmppipe = taskmng_mtr_pipe_get(&oad);
        if(tmppipe !=pipe)
        {
            TASK_FMT_TRACE( pipe, RELAY_LOG, "端口不匹配\n");
            pAmrThreadArg->RelayTrans.isValid = 0;
            return 1;
        }
    }

    switch(pAmrThreadArg->TotalState.RelayTrans)
    {
    case INNER_RTS_IDLE:
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透任务初始化\n");

        OverTime[pipe] = 60;
        TaskBeginTime[pipe] = time(NULL);
        if(pAmrThreadArg->RelayTrans.IOP==0x0011)
        {
            index = 0;
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x02:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++]=2;
                    pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.piid;
                }
                    break;
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.TotalNum;
                }
                    break;
                case 0x07:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                    index +=4;


                }
                    break;
               case 0x50:
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++] = 0x89;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.proxychoice;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.piid;
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],(uint8*)&pAmrThreadArg->RelayTrans.OadPort, 4);
                    index +=4;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.af;
                    memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);

                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                    pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;
                }
                    break;
            }
            pAmrThreadArg->RelayTrans.RelayLen = index;
        }
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;//进入组帧处理
    }
    /* 初始化后直接发送 节约一个周期 */
        break;
    case INNER_RTS_SEND:
    {
        ret = taskmng_meter_relay_data_send(pipe);
        if(ret>0)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"Sendindex %d \n",pAmrThreadArg->RelayTrans.Sendindex );
            BeginTime[pipe] = time(NULL);
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RECV;
            if(pAmrThreadArg->RelayTrans.IOP==0x0011)
            {
                MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                switch(pAmrThreadArg->RelayTrans.proxychoice)
                {
                    case 0x01:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime);
                    }
                        break;
                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x06:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.METER_LIST[meterindex].overtime);
                    }
                        break;
                    case 0x02:
                    case 0x07:
                    case 0x50:
                    {
                        OverTime[pipe] = taskmng_proxy_overtime_get(pAmrThreadArg->RelayTrans.OverTime);
                    }
                        break;

                }
            }
        }else if(ret<0)
        {
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
        }else
        {
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
        }

    }
        break;
    case INNER_RTS_RECV:
    {
        RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
        if(RecvLen>0)
        {
            MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
            memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透收到消息 IID %04X IOP %04X MsgLen %d payload \n",MsgInfo->IID,MsgInfo->IOP,MsgInfo->msglen);
            if(MsgInfo->IOP!=pAmrThreadArg->RelayTrans.DownIOP)
                break;
            if(MsgInfo->index!=pAmrThreadArg->RelayTrans.Sendindex)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "RecvIndex error %d \n",MsgInfo->index);
                break;
            }
            //本地回复超时 数据为00 透传的645设置时间回的就是00
//            if(MsgInfo->MsgLen==1&&MsgInfo->MsgData[0]==0)
//            {
//                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
//                break;
//            }
            meterindex=pAmrThreadArg->RelayTrans.AmrIndex;
            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "IOP %04x isBroad %d stat %d \n",pAmrThreadArg->RelayTrans.IOP,pAmrThreadArg->RelayTrans.isBroad ,pAmrThreadArg->RelayTrans.BroadStat);
            switch(pAmrThreadArg->RelayTrans.IOP)
            {

            case 0x0010:
            {
                //广播校时接收处理
                if(pAmrThreadArg->RelayTrans.isBroad == 1)
                {
                    taskmng_transparent_broad_recv(pipe,MsgInfo->playload,MsgInfo->msglen);
                    break;
                }
#ifdef AREA_FUJIAN
                //福建协议下透传用0100
                if(gGatherMode == OOP_GATHER_FJ)
                {
                    pAmrThreadArg->RelayTrans.RelayLen = fujian_recv_payload_unpack_0100(MsgInfo->playload,MsgInfo->msglen,pAmrThreadArg->RelayTrans.RelayData);
                }else
                {
#endif
                pAmrThreadArg->RelayTrans.RelayLen = MsgInfo->msglen;
                memcpy(pAmrThreadArg->RelayTrans.RelayData,MsgInfo->playload,MsgInfo->msglen);
#ifdef AREA_FUJIAN
                }
#endif

                TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透收到数据 任务结束 \n");
                taskmng_relay_up_msg_write( pipe, 0);
                pAmrThreadArg->RelayTrans.RelayResult =1;
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
                break;
            case 0x0011:
            {
                switch(pAmrThreadArg->RelayTrans.proxychoice)
                {
                    case 0x01:
                    {
                        uint8 lenoffset = 0;
                        uint16 index = 0;
                        uint16 DataLen = 0;
                        uint8 DataBuff[MAX_MSG_LEN] = {0};
                        uint8 prtl;
                        DataLen = XADR_GetEleNum(&PayloadBuf[index],&lenoffset);
                        index+=lenoffset;
                        memcpy(DataBuff,&PayloadBuf[index],DataLen);
                        taskmng_prtl_get_from_tsa(pAmrThreadArg->RelayTrans.AmrNormal[pAmrThreadArg->RelayTrans.AmrIndex].Mtraddr,LOGIC_ID_YC, &prtl);
                        ret = taskmng_relay_data_01_unpack(pipe,DataBuff,DataLen,&MeterPrtlData,apdu);
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"0x0011 01收到回码 解帧 ret %d \n",ret);
                        if(ret <0 )
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                            break;
                        }
                        if(prtl == PRTL_OOP)
                        {
                            if(apdu[0]==0x85&&apdu[1]==0x02)
                            {
                                TASK_FMT_DEBUG( pipe, RELAY_LOG,"oop表解析后 datalen %d \n",MeterPrtlData.DataLen);

                                TASK_BUF_TRACE( pipe, RELAY_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                                taskmng_oop_relay_data_normallist_save(pipe,MeterPrtlData.DataBuff,MeterPrtlData.DataLen);
                            }
                        }
                        else
                        {
                            if(ret == CHECK_RESULT_SUCCEED)
                            {
                                if(taskmng_645_relay_isvalid(pipe,prtl,MeterPrtlData.DataBuff,MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen)>0)
                                {
                                    TASK_FMT_DEBUG(pipe, RELAY_LOG,"645验证成功 DataLen %d\n",MeterPrtlData.DataLen);

                                    TASK_BUF_DEBUG( pipe, RELAY_LOG,MeterPrtlData.DataBuff, MeterPrtlData.DataLen);
                                    if(prtl == PRTL_97)
                                    {
                                        taskmng_645_relay_data_save(pipe,prtl,&MeterPrtlData.DataBuff[2], MeterPrtlData.DataLen-2);
                                    }else
                                    {
                                        taskmng_645_relay_data_save(pipe,prtl,&MeterPrtlData.DataBuff[4], MeterPrtlData.DataLen-4);
                                    }
                                }
                                pAmrThreadArg->RelayTrans.MapIndex++;
                            }else
                                pAmrThreadArg->RelayTrans.MapIndex++;
                        }
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;


                    }
                        break;
                    case 0x02:
                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x06:
                    {
                        uint8 tmpbuffer[MAX_MSG_LEN]={0};
                        AppLayerList_t* pAppLayer;
                        memcpy(tmpbuffer,MsgInfo->playload,MsgInfo->msglen);
                        uint8 lenoffset;
                        uint16 len = XADR_GetEleNum(tmpbuffer, &lenoffset);

                        ret = LinkDataReceive(pipe, &tmpbuffer[lenoffset], len,NULL);
                        TASK_FMT_TRACE(pipe, RELAY_LOG,"LinkDataReceive 698解帧返回值 %d \n",ret);
                        if(ret >=eOopComm_Fail)//解帧失败直接返回
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                            break;
                        }
                        //获取电表地址
                        pAppLayer = FindAppLayer(pipe, NULL);
                        if(pAppLayer == NULL)
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
                            break;
                        }

                        index=pAmrThreadArg->RelayTrans.RelayLen;

                        TASK_FMT_TRACE(pipe, RELAY_LOG,"pAmrThreadArg->RelayTrans.AmrIndex is %d,pAmrThreadArg->RelayTrans.TotalNum is %d\n",pAmrThreadArg->RelayTrans.AmrIndex,pAmrThreadArg->RelayTrans.TotalNum);

                        pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                        pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                        if(pAmrThreadArg->RelayTrans.proxychoice == 0x02)
                            memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                        else
                            memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.METER_LIST[meterindex].Mtraddr.add,6);
                        index+=6;

                        memcpy(&pAmrThreadArg->RelayTrans.RelayData[index],&pAppLayer->recvApduList->apdu[3],pAppLayer->recvApduList->apduLen-5);
                        index+=(pAppLayer->recvApduList->apduLen-5);

                        pAmrThreadArg->RelayTrans.RelayLen = index;
                        if(pAmrThreadArg->RelayTrans.proxychoice==0x02)
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                            break;
                        }
                        pAmrThreadArg->RelayTrans.AmrIndex++;
                        if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                        } else
                        {
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        }
                    }
                        break;
                    case 0x07:
                    {
                        //广播校时接收处理
                        if(pAmrThreadArg->RelayTrans.isBroad == 1)
                        {
                            taskmng_transparent_broad_recv(pipe,MsgInfo->playload,MsgInfo->msglen);
                            break;
                        }

                        index = pAmrThreadArg->RelayTrans.RelayLen;

                        if(MsgInfo->msglen<=2)//如果消息返回的长度小于等于2个字节,认为超时
                        {
                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                            pAmrThreadArg->RelayTrans.RelayLen = index;
                            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                        }
                        else
                        {
                            //结果有数据
                            pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;
                        }

#ifdef AREA_FUJIAN
                        //福建协议下透传用0100
                        if(gGatherMode == OOP_GATHER_FJ)
                        {
                            memcpy(pAmrThreadArg->RelayTrans.RelayData+index,MsgInfo->playload+1,MsgInfo->msglen-1);
                            index += (MsgInfo->msglen-1);

                        }else
                        {
#endif
                        memcpy(pAmrThreadArg->RelayTrans.RelayData+index,MsgInfo->playload,MsgInfo->msglen);
                        index +=MsgInfo->msglen;
#ifdef AREA_FUJIAN
                        }
#endif
                        pAmrThreadArg->RelayTrans.RelayLen = index;

                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                    }
                    break;
                    case 0x50:
                    {
                        //广播校时接收处理
                        if(pAmrThreadArg->RelayTrans.isBroad == 1)
                        {
                            taskmng_transparent_broad_recv(pipe,MsgInfo->playload,MsgInfo->msglen);
                            break;
                        }

                        index = pAmrThreadArg->RelayTrans.RelayLen;
                        //通信地址
                        pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.af;
                        memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                        index = index+pAmrThreadArg->RelayTrans.tsa.len+1;
                        //通信对象类型
                        pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                        //响应报文格式字
                        pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;

                        //结果有数据
                        pAmrThreadArg->RelayTrans.RelayData[index++] = 0x01;

                        memcpy(pAmrThreadArg->RelayTrans.RelayData+index,MsgInfo->playload,MsgInfo->msglen);
                        index +=MsgInfo->msglen;
                        pAmrThreadArg->RelayTrans.RelayLen = index;

                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;

                    }
                    break;
                default:
                    break;
                }
            }
                break;
            default:
                break;

            }

        }
        else
        {
            if((time(NULL)-BeginTime[pipe]>OverTime[pipe]) ||(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime))
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 未收到回复 \n");

                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        switch(pAmrThreadArg->RelayTrans.IOP)
        {

        case 0x0010:
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
            if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                taskmng_relay_up_msg_write( pipe, -1);
                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                break;
            }
            pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
        }
            break;
        case 0x0011:
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "MtrIndex %d 发送时间 %ld 超时时间 %d \n",MtrIndex,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime,pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime);
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "timenow %ld TaskBeginTime %ld OverTime %d \n",time(NULL),TaskBeginTime[pipe],pAmrThreadArg->RelayTrans.OverTime);

                    if(time(NULL)- pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].SendTime>pAmrThreadArg->RelayTrans.AmrNormal[MtrIndex].OverTime)
                    {
                        pAmrThreadArg->RelayTrans.AmrIndex++;
                        pAmrThreadArg->RelayTrans.OadIndex=0;
                        pAmrThreadArg->RelayTrans.MapIndex=0;
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表穿透超时 AmrIndex %d  \n",pAmrThreadArg->RelayTrans.AmrIndex);
                    }
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                {
                    MtrIndex = pAmrThreadArg->RelayTrans.AmrIndex;
                    if(time(NULL)- pAmrThreadArg->RelayTrans.METER_LIST[MtrIndex].firstSendtime>pAmrThreadArg->RelayTrans.METER_LIST[MtrIndex].overtime)
                    {
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "该表穿透超时 AmrIndex %d  \n",pAmrThreadArg->RelayTrans.AmrIndex);
                        break;
                    }
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 任务结束 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                case 0x02:
                case 0x07:
                {
                    TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 超时时间 %d 当前时间 %ld TaskBeginTime %ld \n",pAmrThreadArg->RelayTrans.OverTime,time(NULL),TaskBeginTime[pipe]);
                    if(time(NULL)-TaskBeginTime[pipe]>pAmrThreadArg->RelayTrans.OverTime)
                    {
                        TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时 组无效报文 \n");
                        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_INVALID;
                        break;
                    }
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }
                    break;
                default:
                    break;
            }
        }
            break;
        default:
            break;
        }

    }
        break;
    case INNER_RTS_INVALID: //代理时某块表无效或者超时
    {
        if(pAmrThreadArg->RelayTrans.IOP==0x0011)
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
            case 0x02:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x07;//tsa长度
                pAmrThreadArg->RelayTrans.RelayData[index++]=5;//
                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.MtrAddr,6);
                index+=6;
                //A-ResultRecord
                memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.Road.MainOad,sizeof(OOP_OAD_U));
                index+=sizeof(OOP_OAD_U);
                pAmrThreadArg->RelayTrans.RelayData[index++]=pAmrThreadArg->RelayTrans.Road.SubOadNum;
                uint8 i;
                for(i=0;i<pAmrThreadArg->RelayTrans.Road.SubOadNum;i++)
                {
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                    memcpy_r(&pAmrThreadArg->RelayTrans.RelayData[index],&pAmrThreadArg->RelayTrans.Road.OadListInfo[i],sizeof(OOP_OAD_U));
                    index+=sizeof(OOP_OAD_U);
                }
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++]=0x21;
            }
                break;
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            {
                taskmng_proxy_error_data_pack(pipe);
                pAmrThreadArg->RelayTrans.AmrIndex++;
                if(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                {
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_SEND;
                }else
                {
                    pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
                }
            }
                break;
            case 0x07:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                pAmrThreadArg->RelayTrans.RelayLen = index;

                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
                break;
            case 0x50:
            {
                index = pAmrThreadArg->RelayTrans.RelayLen;

                //通信地址
                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.tsa.af;
                memcpy((uint8*)&pAmrThreadArg->RelayTrans.RelayData[index],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                index = index+pAmrThreadArg->RelayTrans.tsa.len+1;
                //通信对象类型
                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.objiecttype;
                //响应报文格式字
                pAmrThreadArg->RelayTrans.RelayData[index++] = pAmrThreadArg->RelayTrans.resframeformat;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x00;
                pAmrThreadArg->RelayTrans.RelayData[index++] = 0x21;
                pAmrThreadArg->RelayTrans.RelayLen = index;

                pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_END;
            }
                break;
            default:
                break;
            }
        }
    }
        break;
    case INNER_RTS_END:
    {
        switch(pAmrThreadArg->RelayTrans.IOP)
        {

        case 0x0011:
        {

            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {
                case 0x01:
                {
                    //上报数据
                    taskmng_proxy_get_response_list_up_msg_write(pipe);
                }
                    break;
                case 0x02:
                case 0x03:
                case 0x04:
                case 0x05:
                case 0x06:
                case 0x07:
                case 0x50:
                {
                    if(pAmrThreadArg->RelayTrans.proxychoice !=0x07&&pAmrThreadArg->RelayTrans.proxychoice !=0x02)
                    {
                        //判断下是否由于超时未抄完所有表结束
                        while(pAmrThreadArg->RelayTrans.AmrIndex<pAmrThreadArg->RelayTrans.TotalNum)
                        {
                            TASK_FMT_DEBUG( pipe, RELAY_LOG, "穿透超时组未抄完表的无效报文 amrindex %d \n",pAmrThreadArg->RelayTrans.AmrIndex);
                            taskmng_proxy_error_data_pack(pipe);
                            pAmrThreadArg->RelayTrans.AmrIndex++;
                        }
                    }
                    index=pAmrThreadArg->RelayTrans.RelayLen;
                    uint8  tmpbuffer[MAX_MSG_LEN]={0};
                    uint8  Byteoffset=0;

                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//跟随上报信息域
                    pAmrThreadArg->RelayTrans.RelayData[index++]=0x00;//时间标签
                    if(index<0x80)
                    {
                         tmpbuffer[Byteoffset++]=index;
                    }else
                    {
                         tmpbuffer[Byteoffset++] = 0x82;
                         tmpbuffer[Byteoffset++] = (uint8)(index>>8) ;
                         tmpbuffer[Byteoffset++] = (uint8)index ;
                    }
                    memcpy(tmpbuffer+Byteoffset,pAmrThreadArg->RelayTrans.RelayData,index);
                    index=index+Byteoffset;
                    memcpy(pAmrThreadArg->RelayTrans.RelayData,tmpbuffer,index);
                    pAmrThreadArg->RelayTrans.RelayLen = index;

                    taskmng_relay_up_msg_write( pipe, 0);
                    pAmrThreadArg->RelayTrans.RelayResult =1;

                }
                break;
            default:
                break;
            }
        }
            break;

        default:
            break;
        }
        pAmrThreadArg->RelayTrans.isValid = 0;
        memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));
        pAmrThreadArg->TotalState.RelayTrans = INNER_RTS_IDLE;
        //透传任务结束后3秒内检查下是否有新的透传任务
        time_t checktime = time(NULL);
        while(1)
        {
            if(abs(time(NULL )- checktime) >= 3)
                break;
            ret = taskmng_relay_exist_check(pipe);
            if(ret == 1)
            {
                TASK_FMT_DEBUG( pipe, RELAY_LOG, "产生穿透任务 ！\n");
                taskmng_plc_task_queue_add(pipe,MS_RELAY_TRANS);
                break;
            }
            usleep(10000);
        }
        return 1;
    }
        break;
    default:
        break;
    }
    return -1;
}

/**
*********************************************************************
* @brief:      解析透明转发消息，判断消息的合法性；
* @param[in]：  uint8 pipe  线程号
* @param[in]：  uint32 len  消息内容长度
* @param[in]：  uint8* buf  消息内容
* @return：TRUE表示解析成功，FALSE表示解析失败
*********************************************************************
*/
BOOL taskmng_transparent_msg_unpack(uint8 pipe,uint32 len,uint8* buf)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset = 0;
    time_t settime;
    uint8 lenoffset;
    int ret;
    uint8 Addr[6];
    uint8 ctrlcode;
    uint8 DataBuff[200];
    uint16 DataLen = 0;

    uint16 Len;
    offset+=4;       //oad
    //mm = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb = taskmng_com_dcb_get_new(&buf[offset]);
    offset+=2;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.ByteOverTime,&buf[offset],2);
    offset+=2;
    pAmrThreadArg->RelayTrans.RelayLen = XADR_GetEleNum(&buf[offset],&lenoffset);
    offset+=lenoffset;
    memcpy(pAmrThreadArg->RelayTrans.RelayData,&buf[offset],pAmrThreadArg->RelayTrans.RelayLen);

    //先按照3762解析
    ret = taskmng_frame_3762_format_check(pAmrThreadArg->RelayTrans.RelayLen, pAmrThreadArg->RelayTrans.RelayData,&Len);
    if(ret ==0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发为3762帧\n");
        pAmrThreadArg->RelayTrans.frametype = PRTL_3762;
        return TRUE;
    }
    //先按照698解帧
    if(0==LinkDataReceive(pipe,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,NULL))
    {
        ret = CHECK_RESULT_FAILUER;
    }
    else
    {
        ret = taskmng_645_meter_data_unpack(pAmrThreadArg->RelayTrans.RelayLen, pAmrThreadArg->RelayTrans.RelayData , Addr, &ctrlcode,&DataLen, DataBuff);
    }
    if(ret ==CHECK_RESULT_FAILUER )
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "645解帧失败 按照698解帧\n");
        //645解帧失败 按照698解帧
        offset =0;
        DataLen =0;
        DataLen = GetFrame(pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,&offset);
        if(DataLen ==0 )
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "698解帧失败\n");
            return FALSE;
        }
        taskmng_698_addr_get(pAmrThreadArg->RelayTrans.RelayData+offset ,DataLen,Addr);
        pAmrThreadArg->RelayTrans.frametype = PRTL_OOP;
        if(TRUE==taskmng_oop_frame_broadtiming_check(pAmrThreadArg->RelayTrans.RelayData+offset ,DataLen,&settime))
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发帧为698广播校时帧\n");
            pAmrThreadArg->RelayTrans.isBroad = 1;
            pAmrThreadArg->RelayTrans.prtl = PRTL_OOP;
            pAmrThreadArg->RelayTrans.BroadTime = settime;
            pAmrThreadArg->RelayTrans.BroadStat = TMN_TO_METER_GET_DELAY;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "Broadtime %ld \n",pAmrThreadArg->RelayTrans.BroadTime);
            return TRUE;
        }
    }
    else
    {
        //判断是否为广播校时帧
        pAmrThreadArg->RelayTrans.prtl =PRTL_07;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "ctrlcode %d DataLen %d \n",ctrlcode,DataLen);
        TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuff,DataLen );
        if(memcmp(Addr,BroadAddr645,6)==0 && ctrlcode==0x08&&DataLen==6)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发帧为645广播校时帧\n");
            pAmrThreadArg->RelayTrans.isBroad = 1;
            pAmrThreadArg->RelayTrans.prtl = PRTL_07;
            rtc_t  rtctime;
            memset(&rtctime,0x00,sizeof(rtc_t));
            rtctime.ss = DataBuff[0];
            rtctime.nn = DataBuff[1];
            rtctime.hh = DataBuff[2];
            rtctime.dd = DataBuff[3];
            rtctime.mm = DataBuff[4];
            rtctime.yy = DataBuff[5];
            pAmrThreadArg->RelayTrans.BroadTime = commfun_rtc2time2(rtctime);
            pAmrThreadArg->RelayTrans.BroadStat = TMN_TO_METER_GET_DELAY;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "Broadtime %ld \n",pAmrThreadArg->RelayTrans.BroadTime);
            return TRUE;
        }
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发获取的电表地址 0x%02x%02x%02x%02x%02x%02x\n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
    memcpy(pAmrThreadArg->RelayTrans.MtrAddr,Addr,6);
    return TRUE;
}

void taskmng_relay_up_msg_write_22(uint8 pipe,int result,uint32 len,uint8* buf)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 Payload[MAX_MSG_LEN]={0};
    uint16 offset =0 ;
    MESSAGE_INFO_T* MsgSend=NULL;

    if(pAmrThreadArg->RelayTrans.IOP==0x0010||pAmrThreadArg->RelayTrans.IOP==0x0011)

    {
        if(result==0)
        {
            memcpy(&Payload[offset],pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            offset+=pAmrThreadArg->RelayTrans.RelayLen;
        }else
        {
            Payload[offset++] = 0x00;

        }
    }


    TASK_FMT_DEBUG( pipe, RELAY_LOG, "组穿透上行报文 穿透结果 %d payload,数据长度 is %d\n",result,pAmrThreadArg->RelayTrans.RelayLen);
    TASK_BUF_DEBUG( pipe, RELAY_LOG, Payload, offset);

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+offset);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+offset);

    MsgSend->rpm = 0;
    MsgSend->index = pAmrThreadArg->RelayTrans.MsgIndex;
    MsgSend->priority = pAmrThreadArg->RelayTrans.MsgPRIORITY;
    MsgSend->label = pAmrThreadArg->RelayTrans.MsgLabel;
    MsgSend->IID = pAmrThreadArg->RelayTrans.IID;
    MsgSend->IOP = pAmrThreadArg->RelayTrans.IOP;

    MsgSend->sourlen = pAmrThreadArg->RelayTrans.SourLen;
    memcpy(MsgSend->souraddr,pAmrThreadArg->RelayTrans.SourAddr,MsgSend->sourlen);
    MsgSend->destlen = pAmrThreadArg->RelayTrans.DestLen;
    memcpy(MsgSend->destaddr,pAmrThreadArg->RelayTrans.DestAddr,MsgSend->destlen );
    MsgSend->msglen = offset;
    memcpy(MsgSend->playload,Payload,offset);

    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+offset);
    free(MsgSend);
}

/**
*********************************************************************
* @brief:      解析代理消息0901类型的消息，判断消息的合法性；
* @param[in]：  uint8 pipe  线程号
* @param[in]：  uint32 len  消息内容长度
* @param[in]：  uint8* buf  消息内容
* @return：TRUE表示解析成功，FALSE表示解析失败
*********************************************************************
*/

BOOL taskmng_proxy_msg_unpack_get_request_list(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset=0;
    uint16 tmpoffset;
    uint16 TotalNum=0;
    uint8 lenoffset;
    uint8 i,j,n;
    OOP_TSA_T tsa;
    uint8 tmppipe;
    uint16 MtrOverTime;
    uint8 oadnum;
    uint16 AllAmrNum;//消息中电表个数
    uint8  AmrNum=0;//属于本端口的电表总个数
    pAmrThreadArg->RelayTrans.ValidNum=0;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    AllAmrNum = XADR_GetEleNum(&buf[offset],&lenoffset);

    offset+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "ProxyGetRequestList穿透 总电表个数 %d \n",AllAmrNum);
    //解析每个电表的对象
    for(i=0;i<AllAmrNum;i++)
    {
        offset++;//TSA第一个字节为octct-string长度
        tsa.af = buf[offset++];
        //如果表地址带逻辑地址 需要再判断下
        if(tsa.flag == 1)
        {
            tsa.vxd = buf[offset++];
            memcpy(tsa.add,&buf[offset],tsa.len);
            offset += tsa.len;
        }else
        {
            memcpy(tsa.add,&buf[offset],tsa.len+1);
            offset+=tsa.len+1;
        }

        memcpy_r((uint8*)&MtrOverTime,&buf[offset],2);
        offset+=2;
        oadnum = XADR_GetEleNum(&buf[offset],&lenoffset);
        offset+=lenoffset;
        if(oadnum==0)
        {
            return FALSE;
        }
        pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr=tsa;
        pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum = oadnum;
        tmpoffset=offset;
        for(n=0;n<oadnum;n++)
        {
            if(n<OOP_MAX_CSDS)
            {
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[n],&buf[tmpoffset],4);
            }
            tmpoffset+=4;
            //TASK_FMT_TRACE( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
        }
        TASK_FMT_DEBUG( pipe, RELAY_LOG, " %d oadnum %d MtrOverTime %d \n",i,oadnum,MtrOverTime );
        if(MtrOverTime == 0)
        {
            MtrOverTime = pAmrThreadArg->RelayTrans.OverTime/AllAmrNum;
        }
        //sleep(1);
        while(MeterFileAll.FileState==TASK_CFG_MODIFY)
        {
            sleep(1);
            continue;
        }
        tmppipe = taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC);
        if(tmppipe!=pipe)
        {
            if(tmppipe==0xFF)
            {
                TotalNum++;
            }
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x00;
            offset+=(4*oadnum);
            continue;
        }
        else
        {
            TotalNum++;
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x01;
            pAmrThreadArg->RelayTrans.METER_LIST[i].overtime = MtrOverTime;
            pAmrThreadArg->RelayTrans.ValidNum++;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].Mtraddr = tsa;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OverTime = MtrOverTime;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = oadnum;
            if(oadnum>OOP_MAX_CSDS)
                pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = OOP_MAX_CSDS;
            for(j=0;j<oadnum;j++)
            {
                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            AmrNum++;
        }
        //每个端口最多支持100个表
        if(AmrNum>=100)
            break;

    }
    pAmrThreadArg->RelayTrans.AmrNum = AmrNum;
    pAmrThreadArg->RelayTrans.TotalNum= TotalNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前端口电表个数 %d \n",AmrNum);
    if(AmrNum==0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_get_request_record
* 函数功能: //请求代理读取一个服务器的一个记录型对象属性 直接组698帧下发
* 输入参数: pipe 端口号
        buf 消息内容
        len 消息长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_get_request_record(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OadInfoMap  pOadInfo;
    uint16 offset = 0;
    uint16 offsetbak = 0;
    uint16 offsetbak1 = 0;
    uint8 tmpbuff[100]={0};
    uint8 i=0;
    GetRecord_t getRecord;
    LinkLayer_t      *pLinkLayer;
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);//超时时间
    offset += 2;
    offset++;
    memcpy(&tsa.af,&buf[offset],1);
    offset++;
    memcpy(tsa.add,&buf[offset],tsa.len+1);
    offset+=(tsa.len+1);
    sleep(1);
    while(MeterFileAll.FileState==TASK_CFG_MODIFY)
    {
        sleep(1);
        continue;
    }
    if(pipe!= taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC))
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_get_request_record 表地址不存在\n");
        return FALSE;
    }
    memcpy_r((uint8*)&getRecord.oad,&buf[offset],4);
    offset += 4;
    pAmrThreadArg->RelayTrans.Road.MainOad= getRecord.oad;
    if(FALSE==taskmng_oad_info_get(getRecord.oad,&pOadInfo))
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_get_request_record OAD 不存在\n");
        return FALSE;
    }
    offsetbak = offset;
    taskmng_oop_data_extract(buf,&offset,len,tmpbuff,DT_RSD,TRUE);
    getRecord.rsdLen = offset - offsetbak;
    getRecord.rsd = &buf[offsetbak];

    offsetbak = offset;
    //为了适应表计不响应的处理
    offsetbak1 = offset;
    pAmrThreadArg->RelayTrans.Road.SubOadNum=buf[offsetbak1++];
    for(i=0;i<pAmrThreadArg->RelayTrans.Road.SubOadNum;i++)
    {
        offsetbak1++;
        memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.Road.OadListInfo[i].value,&buf[offsetbak1],4);
        offsetbak1 += 4;
    }
    taskmng_oop_data_extract(buf,&offset,len,tmpbuff,DT_RCSD,TRUE);
    getRecord.rcsdLen = offset - offsetbak;
    getRecord.rcsd = &buf[offsetbak];
    OopGet.requst(pipe,&tsa,3,(uint8*)&getRecord,sizeof(GetRecord_t));
    pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
    memcpy(pAmrThreadArg->RelayTrans.RelayData,pLinkLayer->sendBuff,pLinkLayer->sendLen);
    pAmrThreadArg->RelayTrans.RelayLen = pLinkLayer->sendLen;
    memcpy(pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pLinkLayer->sendBuff,pLinkLayer->sendLen);
    pAmrThreadArg->RelayTrans.METER_LIST[0].setlen = pLinkLayer->sendLen;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "0x1102\n");
    TASK_BUF_DEBUG(pipe, RELAY_LOG, pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
    memcpy_r(pAmrThreadArg->RelayTrans.MtrAddr,tsa.add,6);

    return TRUE;
}
/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_set_request_list
* 函数功能: //请求代理设置若干个服务器的若干个对象属性解帧
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_set_request_list(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset=0;
    uint16 offset1=0;
    uint16 offset2=0;
    uint16 tmpoffset;
    uint8 lenoffset;
    uint8 i,j,n;
    OOP_TSA_T tsa;
    uint8 tmppipe;
    uint16 MtrOverTime,setlen=0;
    uint8 oadnum;
    uint16 AllAmrNum;//消息中电表个数
    uint8  AmrNum=0;//属于本端口的电表总个数
    LinkLayer_t      *pLinkLayer;
    pAmrThreadArg->RelayTrans.ValidNum=0;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    AllAmrNum = XADR_GetEleNum(&buf[offset],&lenoffset);
    pAmrThreadArg->RelayTrans.TotalNum=AllAmrNum;
    offset+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_set_request_list 总电表个数 %d \n",AllAmrNum);
    //解析每个电表的对象
    for(i=0;i<AllAmrNum;i++)
    {
        offset++;//TSA第一个字节为octct-string长度
        tsa.af = buf[offset++];
        memcpy(tsa.add,&buf[offset],tsa.len+1);
        offset+=tsa.len+1;

        memcpy_r((uint8*)&MtrOverTime,&buf[offset],2);
        offset+=2;
        offset1=offset;

        oadnum = XADR_GetEleNum(&buf[offset],&lenoffset);
        offset+=lenoffset;
        if(oadnum==0)
        {
            return FALSE;
        }

        pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr=tsa;
        pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum = oadnum;
        tmpoffset=offset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "oadnum is %d,offset1 is %d,tmpoffset is %d\n",oadnum,offset1,tmpoffset);
        for(n=0;n<oadnum;n++)
        {
            //offset1=tmpoffset;
            if(n<OOP_MAX_CSDS)
            {
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[n],&buf[tmpoffset],4);
            }
            tmpoffset+=4;
            taskmng_oop_next_read_data_get(buf, &tmpoffset, 1000, DT_NULL, FALSE);

        }
        offset2=tmpoffset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "offset2 is %d\n",offset2);
        setlen=offset2-offset1;
        pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen=4*oadnum+lenoffset;
        //pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen=setlen;
        memcpy(pAmrThreadArg->RelayTrans.METER_LIST[i].relaydata,buf+offset1,pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "setlen is %d\n",setlen);
        TASK_BUF_DEBUG(pipe, RELAY_LOG,buf+offset1,setlen);
        OopSet.requst(pipe,&tsa,2,buf+offset1,setlen);
        pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
        memcpy( pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pLinkLayer->sendBuff,pLinkLayer->sendLen);
        pAmrThreadArg->RelayTrans.METER_LIST[i].setlen = pLinkLayer->sendLen;


        TASK_BUF_DEBUG(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
        //TASK_FMT_TRACE( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );


        TASK_FMT_DEBUG( pipe, RELAY_LOG, " %d oadnum %d MtrOverTime %d \n",i,oadnum,MtrOverTime );
        sleep(1);
        while(MeterFileAll.FileState==TASK_CFG_MODIFY)
        {
            sleep(1);
            continue;
        }
        tmppipe = taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC);
        if(tmppipe!=pipe)
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x00;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_analysis(buf, &offset, 1000, DT_NULL, FALSE);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            continue;
        }
        else
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x01;
            pAmrThreadArg->RelayTrans.METER_LIST[i].overtime = MtrOverTime;
            pAmrThreadArg->RelayTrans.ValidNum++;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].Mtraddr = tsa;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OverTime = MtrOverTime;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = oadnum;
            if(oadnum>OOP_MAX_CSDS)
                pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = OOP_MAX_CSDS;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            AmrNum++;
        }
        //每个端口最多支持100个表
        if(AmrNum>=100)
            break;

    }
    pAmrThreadArg->RelayTrans.AmrNum = AmrNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前端口电表个数 %d \n",AmrNum);
    if(AmrNum==0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
    //return TRUE;
}

/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_set_then_get_request_list
* 函数功能: //请求代理设置若干个服务器的若干个对象属性解帧
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_set_then_get_request_list(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset=0;
    uint16 offset1=0;
    uint16 offset2=0;
    uint16 tmpoffset;
    uint8 lenoffset;
    uint8 i,j,n;
    OOP_TSA_T tsa;
    uint8 tmppipe;
    uint16 MtrOverTime,setlen=0;
    uint8 oadnum;
    uint16 AllAmrNum;//消息中电表个数
    uint8  AmrNum=0;//属于本端口的电表总个数
    LinkLayer_t      *pLinkLayer;
    pAmrThreadArg->RelayTrans.ValidNum=0;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    AllAmrNum = XADR_GetEleNum(&buf[offset],&lenoffset);
    pAmrThreadArg->RelayTrans.TotalNum=AllAmrNum;
    offset+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_set_then_get_request_list 总电表个数 %d \n",AllAmrNum);
    //解析每个电表的对象
    for(i=0;i<AllAmrNum;i++)
    {
        offset++;//TSA第一个字节为octct-string长度
        tsa.af = buf[offset++];
        memcpy(tsa.add,&buf[offset],tsa.len+1);
        offset+=tsa.len+1;

        memcpy_r((uint8*)&MtrOverTime,&buf[offset],2);
        offset+=2;
        offset1=offset;

        oadnum = XADR_GetEleNum(&buf[offset],&lenoffset);
        offset+=lenoffset;
        if(oadnum==0)
        {
            return FALSE;
        }

        pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr=tsa;
        pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum = oadnum;
        tmpoffset=offset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "oadnum is %d,offset1 is %d,tmpoffset is %d\n",oadnum,offset1,tmpoffset);
        for(n=0;n<oadnum;n++)
        {
            //offset1=tmpoffset;
            if(n<OOP_MAX_CSDS)
            {
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[n],&buf[tmpoffset],4);
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SetOad[n],&buf[tmpoffset],4);
            }
            tmpoffset+=4;
            taskmng_oop_next_read_data_get(buf, &tmpoffset, 1000, DT_NULL, FALSE);
            memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].GetOad[n],&buf[tmpoffset],4);
            tmpoffset+=5;
        }

        offset2=tmpoffset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "offset2 is %d\n",offset2);
        setlen=offset2-offset1;
        pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen=setlen;
        memcpy(pAmrThreadArg->RelayTrans.METER_LIST[i].relaydata,buf+offset1,setlen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "setlen is %d\n",setlen);
        TASK_BUF_DEBUG(pipe, RELAY_LOG,buf+offset1,setlen);
        OopSet.requst(pipe,&tsa,3,buf+offset1,setlen);
        pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
        memcpy( pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pLinkLayer->sendBuff,pLinkLayer->sendLen);
        pAmrThreadArg->RelayTrans.METER_LIST[i].setlen = pLinkLayer->sendLen;


        TASK_BUF_DEBUG(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
        //TASK_FMT_TRACE( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );

        TASK_FMT_DEBUG( pipe, RELAY_LOG, " %d oadnum %d MtrOverTime %d \n",i,oadnum,MtrOverTime );
        sleep(1);
        while(MeterFileAll.FileState==TASK_CFG_MODIFY)
        {
            sleep(1);
            continue;
        }
        tmppipe = taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC);
        if(tmppipe!=pipe)
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x00;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                offset=offset+5;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            continue;
        }
        else
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x01;
            pAmrThreadArg->RelayTrans.METER_LIST[i].overtime = MtrOverTime;
            pAmrThreadArg->RelayTrans.ValidNum++;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].Mtraddr = tsa;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OverTime = MtrOverTime;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = oadnum;
            if(oadnum>OOP_MAX_CSDS)
                pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = OOP_MAX_CSDS;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                offset=offset+5;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            AmrNum++;
        }
        //每个端口最多支持100个表
        if(AmrNum>=100)
            break;

    }
    pAmrThreadArg->RelayTrans.AmrNum = AmrNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前端口电表个数 %d \n",AmrNum);
    if(AmrNum==0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}
/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_act_request_list
* 函数功能: //请求代理操作若干个服务器的若干个对象方法
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_act_request_list(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset=0;
    uint16 offset1=0;
    uint16 offset2=0;
    uint16 tmpoffset;
    uint8 lenoffset;
    uint8 i,j,n;
    OOP_TSA_T tsa;
    uint8 tmppipe;
    uint16 MtrOverTime,setlen=0;
    uint8 oadnum;
    uint16 AllAmrNum;//消息中电表个数
    uint8  AmrNum=0;//属于本端口的电表总个数
    LinkLayer_t      *pLinkLayer;
    pAmrThreadArg->RelayTrans.ValidNum=0;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    AllAmrNum = XADR_GetEleNum(&buf[offset],&lenoffset);
    pAmrThreadArg->RelayTrans.TotalNum=AllAmrNum;
    offset+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_act_request_list 总电表个数 %d \n",AllAmrNum);
    //解析每个电表的对象
    for(i=0;i<AllAmrNum;i++)
    {
        offset++;//TSA第一个字节为octct-string长度
        tsa.af = buf[offset++];
        memcpy(tsa.add,&buf[offset],tsa.len+1);
        offset+=tsa.len+1;

        memcpy_r((uint8*)&MtrOverTime,&buf[offset],2);
        offset+=2;
        offset1=offset;

        oadnum = XADR_GetEleNum(&buf[offset],&lenoffset);
        offset+=lenoffset;
        if(oadnum==0)
        {
            return FALSE;
        }

        pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr=tsa;
        pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum = oadnum;
        tmpoffset=offset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "oadnum is %d,offset1 is %d,tmpoffset is %d\n",oadnum,offset1,tmpoffset);
        for(n=0;n<oadnum;n++)
        {
            //offset1=tmpoffset;
            if(n<OOP_MAX_CSDS)
            {
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[n],&buf[tmpoffset],4);
            }
            tmpoffset+=4;
            taskmng_oop_next_read_data_get(buf, &tmpoffset, 1000, DT_NULL, FALSE);

        }
        offset2=tmpoffset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "offset2 is %d\n",offset2);
        setlen=offset2-offset1;
        pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen=4*oadnum+lenoffset;
        memcpy(pAmrThreadArg->RelayTrans.METER_LIST[i].relaydata,buf+offset1,pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "setlen is %d\n",setlen);
        TASK_BUF_DEBUG(pipe, RELAY_LOG,buf+offset1,setlen);
        OopAction.requst(pipe,&tsa,2,buf+offset1,setlen);
        pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
        memcpy( pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pLinkLayer->sendBuff,pLinkLayer->sendLen);
        pAmrThreadArg->RelayTrans.METER_LIST[i].setlen = pLinkLayer->sendLen;


        TASK_BUF_DEBUG(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
        //TASK_FMT_TRACE( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );


        TASK_FMT_DEBUG( pipe, RELAY_LOG, " %d oadnum %d MtrOverTime %d \n",i,oadnum,MtrOverTime );
        sleep(1);
        while(MeterFileAll.FileState==TASK_CFG_MODIFY)
        {
            sleep(1);
            continue;
        }
        tmppipe = taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC);
        if(tmppipe!=pipe)
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x00;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            continue;
        }
        else
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x01;
            pAmrThreadArg->RelayTrans.METER_LIST[i].overtime = MtrOverTime;
            pAmrThreadArg->RelayTrans.ValidNum++;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].Mtraddr = tsa;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OverTime = MtrOverTime;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = oadnum;
            if(oadnum>OOP_MAX_CSDS)
                pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = OOP_MAX_CSDS;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            AmrNum++;
        }
        //每个端口最多支持100个表
        if(AmrNum>=100)
            break;

    }
    pAmrThreadArg->RelayTrans.AmrNum = AmrNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前端口电表个数 %d \n",AmrNum);
    if(AmrNum==0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}


/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_act_then_get_request_list
* 函数功能: 请求代理操作后读取若干个服务器的若干个对象方法和属性解帧
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_act_then_get_request_list(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset=0;
    uint16 offset1=0;
    uint16 offset2=0;
    uint16 tmpoffset;
    uint8 lenoffset;
    uint8 i,j,n;
    OOP_TSA_T tsa;
    uint8 tmppipe;
    uint16 MtrOverTime,setlen=0;
    uint8 oadnum;
    uint16 AllAmrNum;//消息中电表个数
    uint8  AmrNum=0;//属于本端口的电表总个数
    LinkLayer_t      *pLinkLayer;
    pAmrThreadArg->RelayTrans.ValidNum=0;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    AllAmrNum = XADR_GetEleNum(&buf[offset],&lenoffset);
    pAmrThreadArg->RelayTrans.TotalNum=AllAmrNum;
    offset+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack_set_then_get_request_list 总电表个数 %d \n",AllAmrNum);
    //解析每个电表的对象
    for(i=0;i<AllAmrNum;i++)
    {
        offset++;//TSA第一个字节为octct-string长度
        tsa.af = buf[offset++];
        memcpy(tsa.add,&buf[offset],tsa.len+1);
        offset+=tsa.len+1;

        memcpy_r((uint8*)&MtrOverTime,&buf[offset],2);
        offset+=2;
        offset1=offset;

        oadnum = XADR_GetEleNum(&buf[offset],&lenoffset);
        offset+=lenoffset;
        if(oadnum==0)
        {
            return FALSE;
        }

        pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr=tsa;
        pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum = oadnum;
        tmpoffset=offset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "oadnum is %d,offset1 is %d,tmpoffset is %d\n",oadnum,offset1,tmpoffset);
        for(n=0;n<oadnum;n++)
        {
            //offset1=tmpoffset;
            if(n<OOP_MAX_CSDS)
            {
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SubOad[n],&buf[tmpoffset],4);
                memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SetOad[n],&buf[tmpoffset],4);
            }
            tmpoffset+=4;
            taskmng_oop_next_read_data_get(buf, &tmpoffset, 1000, DT_NULL, FALSE);
            memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].GetOad[n],&buf[tmpoffset],4);
            tmpoffset+=5;
        }

        offset2=tmpoffset;
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "offset2 is %d\n",offset2);
        setlen=offset2-offset1;
        pAmrThreadArg->RelayTrans.METER_LIST[i].relaydatalen=setlen;
        memcpy(pAmrThreadArg->RelayTrans.METER_LIST[i].relaydata,buf+offset1,setlen);
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "setlen is %d\n",setlen);
        TASK_BUF_DEBUG(pipe, RELAY_LOG,buf+offset1,setlen);
        OopAction.requst(pipe,&tsa,3,buf+offset1,setlen);
        pLinkLayer = FindLinkLayer(pipe,NULL);//获取协议帧缓冲区
        memcpy( pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pLinkLayer->sendBuff,pLinkLayer->sendLen);
        pAmrThreadArg->RelayTrans.METER_LIST[i].setlen = pLinkLayer->sendLen;


        TASK_BUF_DEBUG(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.METER_LIST[i].setaction,pAmrThreadArg->RelayTrans.METER_LIST[i].setlen);
        //TASK_FMT_TRACE( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );

        TASK_FMT_DEBUG( pipe, RELAY_LOG, " %d oadnum %d MtrOverTime %d \n",i,oadnum,MtrOverTime );
        sleep(1);
        while(MeterFileAll.FileState==TASK_CFG_MODIFY)
        {
            sleep(1);
            continue;
        }
        tmppipe = taskmng_pipe_get_from_tsa(tsa,LOGIC_ID_YC);
        if(tmppipe!=pipe)
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x00;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                offset=offset+5;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            continue;
        }
        else
        {
            pAmrThreadArg->RelayTrans.METER_LIST[i].VliadMeter=0x01;
            pAmrThreadArg->RelayTrans.METER_LIST[i].overtime = MtrOverTime;
            pAmrThreadArg->RelayTrans.ValidNum++;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].Mtraddr = tsa;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OverTime = MtrOverTime;
            pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = oadnum;
            if(oadnum>OOP_MAX_CSDS)
                pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].OadNum = OOP_MAX_CSDS;
            for(j=0;j<oadnum;j++)
            {

                if(j<OOP_MAX_CSDS)
                {
                    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j],&buf[offset],4);
                }
                offset+=4;
                taskmng_oop_next_read_data_get(buf, &offset, 1000, DT_NULL, FALSE);
                offset=offset+5;
                TASK_FMT_DEBUG( pipe, RELAY_LOG, " oadindex %d oad 0x%08x \n",j,pAmrThreadArg->RelayTrans.AmrNormal[AmrNum].SubOad[j].value );
            }
            AmrNum++;
        }
        //每个端口最多支持100个表
        if(AmrNum>=100)
            break;

    }
    pAmrThreadArg->RelayTrans.AmrNum = AmrNum;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "当前端口电表个数 %d \n",AmrNum);
    if(AmrNum==0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

}
/*******************************************************************************
* 函数名称: taskmng_proxy_msg_unpack_trans_command_request
* 函数功能: 请求代理操作后读取若干个服务器的若干个对象方法和属性解帧
* 输入参数: pipe 端口号
        buf 消息内容
        len  长度
* 输出参数:
* 返 回 值:  TRUE 解析成功
    FALSE 解析失败
*******************************************************************************/
BOOL taskmng_proxy_msg_unpack_trans_command_request(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 offset = 0;
    time_t settime;
    uint8 lenoffset;
    int ret;
    uint8 Addr[6];
    uint8 ctrlcode;
    uint8 DataBuff[200];
    uint16 DataLen = 0;

    uint16 Len;

    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OadPort,buf+offset,4);
    offset+=4;       //端口oad
    //端口波特率
    pAmrThreadArg->RelayTrans.comdcb.baud = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.parity = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.databits = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.stopbits = buf[offset++];
    pAmrThreadArg->RelayTrans.comdcb.flowctrl = buf[offset++];
    pAmrThreadArg->RelayTrans.priority =20;//福建要求默认0907的优先级为20；
    if(pAmrThreadArg->RelayTrans.comdcb.flowctrl>2 && pipe>1)
    {
        return FALSE;
    }
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.OverTime,&buf[offset],2);
    offset+=2;
    memcpy_r((uint8*)&pAmrThreadArg->RelayTrans.ByteOverTime,&buf[offset],2);
    offset+=2;
    pAmrThreadArg->RelayTrans.RelayLen = XADR_GetEleNum(&buf[offset],&lenoffset);
    offset+=lenoffset;
    memcpy(pAmrThreadArg->RelayTrans.RelayData,&buf[offset],pAmrThreadArg->RelayTrans.RelayLen);
    memcpy(pAmrThreadArg->RelayTrans.METER_LIST[0].setaction,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
    pAmrThreadArg->RelayTrans.METER_LIST[0].setlen = pAmrThreadArg->RelayTrans.RelayLen;
    //先按照3762解析
    ret = taskmng_frame_3762_format_check(pAmrThreadArg->RelayTrans.RelayLen, pAmrThreadArg->RelayTrans.RelayData,&Len);
    if(ret ==0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发为3762帧\n");
        pAmrThreadArg->RelayTrans.frametype = PRTL_3762;
        return TRUE;
    }
#ifdef PRODUCT_ECU
    OOP_OAD_U pipeoad ;
    pipeoad.value = pAmrThreadArg->RelayTrans.OadPort;
    if(pipeoad.nObjID==0xF220 && (pipe == PLC_NUM+MAX_485_NUM-1 || pipe == PLC_NUM+MAX_485_NUM-2))
    {
        //MBUS协议报文 直接透传
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发为MBUS\n");
        pAmrThreadArg->RelayTrans.frametype = PRTL_MBUS;
        return TRUE;
    }

#endif
    //先按照645解帧
    //TASK_BUF_DEBUG(pipe, RELAY_LOG,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
    //进一步判断是否为698帧，遇到厂家有透传给能源控控制器698报文包裹0907代理07表的报文，会把这整帧报文误判断为645报文
    if(0==LinkDataReceive(pipe,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,NULL))
    {
        ret =CHECK_RESULT_FAILUER;
    }
    else
    {
        ret = taskmng_645_meter_data_unpack(pAmrThreadArg->RelayTrans.RelayLen, pAmrThreadArg->RelayTrans.RelayData , Addr, &ctrlcode, &DataLen, DataBuff);
    }
    if(ret ==CHECK_RESULT_FAILUER )
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "645解帧失败 按照698解帧\n");
        //645解帧失败 按照698解帧
        offset =0;
        DataLen =0;
        DataLen = GetFrame(pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen,&offset);
        if(DataLen ==0 )
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "698解帧失败\n");

            #ifdef AREA_SHANDONG
            return taskmng_pv_control_unpack(pipe,pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            #else
            pAmrThreadArg->RelayTrans.prtl =PRTL_UNKNOWN;//未知协议
            return TRUE;
            #endif
        }
        pAmrThreadArg->RelayTrans.frametype = PRTL_OOP;
        taskmng_698_addr_get(pAmrThreadArg->RelayTrans.RelayData+offset ,DataLen,Addr);
        if(TRUE==taskmng_oop_frame_broadtiming_check(pAmrThreadArg->RelayTrans.RelayData+offset ,DataLen,&settime))
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发帧为698广播校时帧\n");
            pAmrThreadArg->RelayTrans.isBroad = 1;
            pAmrThreadArg->RelayTrans.prtl = PRTL_OOP;
            pAmrThreadArg->RelayTrans.BroadTime = settime;
            pAmrThreadArg->RelayTrans.BroadStat = TMN_TO_METER_GET_DELAY;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "Broadtime %ld \n",pAmrThreadArg->RelayTrans.BroadTime);
            return TRUE;
        }
    }
    else
    {
        //判断是否为广播校时帧
        TASK_FMT_DEBUG( pipe, RELAY_LOG, "ctrlcode %d DataLen %d \n",ctrlcode,DataLen);
        TASK_BUF_DEBUG( pipe, RELAY_LOG,DataBuff,DataLen );
        pAmrThreadArg->RelayTrans.frametype = PRTL_07;
        if(memcmp(Addr,BroadAddr645,6)==0 && ctrlcode==0x08&&DataLen==6)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发帧为645广播校时帧\n");
            pAmrThreadArg->RelayTrans.isBroad = 1;
            pAmrThreadArg->RelayTrans.prtl = PRTL_07;
            rtc_t  rtctime;
            memset(&rtctime,0x00,sizeof(rtc_t));
            rtctime.ss = DataBuff[0];
            rtctime.nn = DataBuff[1];
            rtctime.hh = DataBuff[2];
            rtctime.dd = DataBuff[3];
            rtctime.mm = DataBuff[4];
            rtctime.yy = DataBuff[5];
            pAmrThreadArg->RelayTrans.BroadTime = commfun_rtc2time2(rtctime);
            pAmrThreadArg->RelayTrans.BroadStat = TMN_TO_METER_GET_DELAY;
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "Broadtime %ld \n",pAmrThreadArg->RelayTrans.BroadTime);
            return TRUE;
        }
    }
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "透明转发获取的电表地址 0x%02x%02x%02x%02x%02x%02x\n",Addr[5],Addr[4],Addr[3],Addr[2],Addr[1],Addr[0]);
    memcpy(pAmrThreadArg->RelayTrans.MtrAddr,Addr,6);
    return TRUE;

}

/**
*********************************************************************
* @brief:      解析代理消息，判断消息的合法性；
* @param[in]：  uint8 pipe  线程号
* @param[in]：  uint32 len  消息内容长度
* @param[in]：  uint8* buf  消息内容
* @return：TRUE表示解析成功，FALSE表示解析失败
*********************************************************************
*/

BOOL taskmng_proxy_msg_unpack(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];

    uint16 index =0;
    uint8 lenoffset;
    uint16 datalen;
    uint8 choice;
    //uint8 proxybuf[MAX_MSG_LEN] = {0};
    datalen = XADR_GetEleNum(buf, &lenoffset);
    index+=lenoffset;
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack datalen %d len %d \n",datalen,len);


    if(index+datalen!=len)
        return FALSE;
    if(buf[index++]!=0x09)//9 PROXY-Request
        return FALSE;

    choice = buf[index++];
    TASK_FMT_DEBUG( pipe, RELAY_LOG, "taskmng_proxy_msg_unpack choice %d\n",choice);
    pAmrThreadArg->RelayTrans.proxychoice = choice;
    pAmrThreadArg->RelayTrans.piid = buf[index++];
    switch (choice)
    {
    case 0x01: //请求代理读取若干个服务器的若干个对象属性
    {
        if(taskmng_proxy_msg_unpack_get_request_list(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
    case 0x02:
    {
        if(taskmng_proxy_msg_unpack_get_request_record(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
    case 0x03:
    {
        if(taskmng_proxy_msg_unpack_set_request_list(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
    case 0x04:
    {
        if(taskmng_proxy_msg_unpack_set_then_get_request_list(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
     case 0x05:
    {
        if(taskmng_proxy_msg_unpack_act_request_list(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
    case 0x06:
    {
        if(taskmng_proxy_msg_unpack_act_then_get_request_list(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
    case 0x07:
    {
        if(taskmng_proxy_msg_unpack_trans_command_request(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
#ifdef AREA_FUJIAN
    case 50:
    {
        if(taskmng_smart_proxy_msg_unpack_trans_command_request(pipe,&buf[index],datalen-3)==TRUE)
        {
            return TRUE;
        }
    }
        break;
#endif
    default:
        break;
    }
    return FALSE;
}

void taskmng_relay_up_msg_write_0011(uint8 pipe,int result,uint32 len,uint8* buf)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint8 Payload[MAX_MSG_LEN]={0};
    uint8 Buffer[MAX_MSG_LEN]={0};
    uint8 i,j;
    uint8 tsalen=0;
    uint8 oadnum=0;
    uint8 Byteoffset=0;
    uint16 offset =0 ;
    uint16 buffoffset=0,buffoffset1=0,buffoffset2=0;
    MESSAGE_INFO_T* MsgSend=NULL;

    if(pAmrThreadArg->RelayTrans.IOP==0x0010||pAmrThreadArg->RelayTrans.IOP==0x0011)

    {
        if(result==0)
        {
            memcpy(&Payload[offset],pAmrThreadArg->RelayTrans.RelayData,pAmrThreadArg->RelayTrans.RelayLen);
            offset+=pAmrThreadArg->RelayTrans.RelayLen;
        }else
        {
            switch(pAmrThreadArg->RelayTrans.proxychoice)
            {

                case 0x01:
                {
                       if(pAmrThreadArg->RelayTrans.TotalNum==0)
                       {
                            return;
                       }
                       buffoffset=buffoffset+7;

                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = 0x01;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.TotalNum;
                       for(i=0;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
                       {

                            Buffer[offset++] =buf[buffoffset++];
                            tsalen=buf[buffoffset++];
                            Buffer[offset++] =tsalen;
                            memcpy(Buffer+offset,buf+buffoffset,tsalen+1);
                            offset=tsalen+offset+1;
                            buffoffset=tsalen+buffoffset+1;
                            buffoffset=buffoffset+2;
                            oadnum=buf[buffoffset++];
                            Buffer[offset++] =oadnum;
                            for(j=0;j<oadnum;j++)
                            {
                                memcpy(Buffer+offset,buf+buffoffset,4);
                                offset=offset+4;
                                Buffer[offset++] =0x00;
                                Buffer[offset++] =0x21;
                            }
                       }
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
                case 0x02:
                {
                       buffoffset=buffoffset+6;

                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = 0x02;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       Buffer[offset++] =buf[buffoffset++];
                       tsalen=buf[buffoffset++];
                       Buffer[offset++] =tsalen;
                       memcpy(Buffer+offset,buf+buffoffset,tsalen+1);
                       offset=tsalen+offset+1;
                       buffoffset=tsalen+buffoffset+1;
                       memcpy(Buffer+offset,buf+buffoffset,4);
                       offset=offset+4;
                       buffoffset=buffoffset+4;

                       TASK_FMT_TRACE( pipe, RELAY_LOG, "buffoffset is %d，buf[buffoffset] is %d\n",buffoffset,buf[buffoffset]);
                       taskmng_oop_next_read_data_get(buf,&buffoffset,1000,DT_RSD,TRUE);
                       buffoffset1=buffoffset;
                       TASK_FMT_TRACE( pipe, RELAY_LOG, "buffoffset1 is %d\n",buffoffset1);
                       taskmng_oop_next_read_data_get(buf,&buffoffset,1000,DT_RCSD,TRUE);
                       buffoffset2=buffoffset;
                       TASK_FMT_TRACE( pipe, RELAY_LOG, "buffoffset2 is %d\n",buffoffset2);
                       memcpy(Buffer+offset,buf+buffoffset1,buffoffset2-buffoffset1);
                       offset=offset+buffoffset2-buffoffset1;
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x21;
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
                case 0x03:
                case 0x05:
                {
                       buffoffset=buffoffset+7;

                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.proxychoice;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.TotalNum;
                       for(i=0;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
                       {

                            Buffer[offset++] =buf[buffoffset++];
                            tsalen=buf[buffoffset++];
                            Buffer[offset++] =tsalen;
                            memcpy(Buffer+offset,buf+buffoffset,tsalen+1);
                            offset=tsalen+offset+1;
                            buffoffset=tsalen+buffoffset+1;
                            buffoffset=buffoffset+2;
                            oadnum=buf[buffoffset++];
                            Buffer[offset++] =oadnum;
                            for(j=0;j<oadnum;j++)
                            {
                                memcpy(Buffer+offset,buf+buffoffset,4);
                                offset=offset+4;
                                buffoffset=buffoffset+4;
                                taskmng_oop_next_read_data_get(buf, &buffoffset, 1000, DT_NULL, FALSE);
                                Buffer[offset++] =0x21;
                            }
                       }
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
                case 0x04:
                {


                       uint8 mn;

                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.proxychoice;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.TotalNum;
                       for(i=0;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
                       {

                           Buffer[offset++]=0x07;//tsa长度
                           Buffer[offset++]=5;//
                           memcpy(&Buffer[offset],pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr.add,6);
                           offset+=6;
                           for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;mn++)
                           {
                               Buffer[offset++]=pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;
                               memcpy_r(Buffer+offset,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SetOad[mn].value,4);
                               offset=offset+4;
                               Buffer[offset++]=0x21;
                               Buffer[offset++]=pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;
                               memcpy_r(Buffer+offset,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].GetOad[mn].value,4);
                               offset=offset+4;
                               Buffer[offset++]=0x21;

                           }
                       }
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
                case 0x06:
                {


                       uint8 mn;

                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.proxychoice;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.TotalNum;
                       for(i=0;i<pAmrThreadArg->RelayTrans.TotalNum;i++)
                       {

                           Buffer[offset++]=0x07;//tsa长度
                           Buffer[offset++]=5;//
                           memcpy(&Buffer[offset],pAmrThreadArg->RelayTrans.METER_LIST[i].Mtraddr.add,6);
                            offset+=6;
                            Buffer[offset++]=pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;
                            for(mn=0;mn<pAmrThreadArg->RelayTrans.METER_LIST[i].OadNum;mn++)
                            {

                                memcpy_r(&Buffer[offset],(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].SetOad[mn].value,4);
                                offset=offset+4;
                                Buffer[offset++]=0xFF;
                                Buffer[offset++]=0x00;
                                memcpy_r(pAmrThreadArg->RelayTrans.RelayData+offset,(uint8*)&pAmrThreadArg->RelayTrans.METER_LIST[i].GetOad[mn].value,4);
                                offset=offset+4;
                                Buffer[offset++]=0x00;
                                Buffer[offset++]=0xFF;

                            }
                       }
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
                case 0x07:
                {
                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.proxychoice;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       memcpy_r((uint8*)&Buffer[offset], (uint8*)&pAmrThreadArg->RelayTrans.OadPort,4);
                       offset = offset+4;
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0xFF;
                       Buffer[offset++] =0x00;
                       Buffer[offset++] =0x00;
                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
                break;
#ifdef AREA_FUJIAN
                case 50:
                {
                       Buffer[offset++] = 0x89;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.proxychoice;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.piid;
                       memcpy_r((uint8*)&Buffer[offset], (uint8*)&pAmrThreadArg->RelayTrans.OadPort,4);
                       offset = offset+4;
                       //tsa
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.tsa.len+2;
                       Buffer[offset++] = pAmrThreadArg->RelayTrans.tsa.len;
                       memcpy(&Buffer[offset],pAmrThreadArg->RelayTrans.tsa.add,pAmrThreadArg->RelayTrans.tsa.len+1);
                       offset+=(pAmrThreadArg->RelayTrans.tsa.len+1);

                       Buffer[offset++] =pAmrThreadArg->RelayTrans.objiecttype;
                       Buffer[offset++] =pAmrThreadArg->RelayTrans.resframeformat;
                       Buffer[offset++] =0x00;  //透明转发命令返回结果 0 dar
                       Buffer[offset++] =0x0f;  //信道忙 698没有对应的错误码

                       if(offset<0x80)
                       {
                            Payload[Byteoffset++]=offset;
                       }else
                       {
                            Payload[Byteoffset++] = 0x82;
                            Payload[Byteoffset++] = (uint8)(offset>>8) ;
                            Payload[Byteoffset++] = (uint8)offset ;
                       }
                       memcpy(Payload+Byteoffset,Buffer,offset);
                       offset=offset+Byteoffset;

                }
#endif
            }
//            Payload[offset++] = 0;
        }
    }


    TASK_FMT_DEBUG( pipe, RELAY_LOG, "组穿透上行报文 穿透结果 %d payload,数据长度 is %d\n",result,pAmrThreadArg->RelayTrans.RelayLen);

    TASK_BUF_DEBUG( pipe, RELAY_LOG, Payload, offset);
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+offset);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+offset);

    MsgSend->rpm = 0;
    MsgSend->index = pAmrThreadArg->RelayTrans.MsgIndex;
    MsgSend->priority = pAmrThreadArg->RelayTrans.MsgPRIORITY;
    MsgSend->label = pAmrThreadArg->RelayTrans.MsgLabel;
    MsgSend->IID = pAmrThreadArg->RelayTrans.IID;
    MsgSend->IOP = pAmrThreadArg->RelayTrans.IOP;

    MsgSend->sourlen = pAmrThreadArg->RelayTrans.SourLen;
    memcpy(MsgSend->souraddr,pAmrThreadArg->RelayTrans.SourAddr,MsgSend->sourlen);
    MsgSend->destlen = pAmrThreadArg->RelayTrans.DestLen;
    memcpy(MsgSend->destaddr,pAmrThreadArg->RelayTrans.DestAddr,MsgSend->destlen );
    MsgSend->msglen = offset;
    memcpy(MsgSend->playload,Payload,offset);

    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+offset);
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: taskmng_amr_status_get
* 函数功能://0013 获取当日抄读状态
* 输入参数: pipe 端口号
        buf 消息内容
        len 消息长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_amr_status_get(uint8 pipe,uint8* buf,uint32 len)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    MESSAGE_INFO_T* MsgSend=NULL;
    uint8 payload[100] = {0};
    uint16 index=0;
    uint8 Addr[10]={0};
    uint16 Pn;
    uint16 MsIndex;
    memcpy_r(Addr,&buf[1],6);
    Pn = taskmng_pn_get_from_addr(Addr,LOGIC_ID_YC);
    MsIndex = taskmng_ms_index_get(pipe, Pn,LOGIC_ID_YC);

    payload[index++] = pAmrThreadArg->pMtrAmrInfo[MsIndex].LastResult;
    memcpy_r(&payload[index],(uint8*)&pAmrThreadArg->pMtrAmrInfo[MsIndex].AllFailCnt,2);
    index+=2;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastSendTime);
    index+=7;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastRecvTime);
    index+=7;
    MakeDateTimeS(&payload[index],pAmrThreadArg->pMtrAmrInfo[MsIndex].LastFailTime);
    index+=7;

    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);

    MsgSend->rpm = 0;
    MsgSend->index = pAmrThreadArg->RelayTrans.MsgIndex;
    MsgSend->priority = pAmrThreadArg->RelayTrans.MsgPRIORITY;
    MsgSend->label = pAmrThreadArg->RelayTrans.MsgLabel;
    MsgSend->IID = pAmrThreadArg->RelayTrans.IID;
    MsgSend->IOP = pAmrThreadArg->RelayTrans.IOP;

    MsgSend->sourlen = pAmrThreadArg->RelayTrans.SourLen;
    memcpy(MsgSend->souraddr,pAmrThreadArg->RelayTrans.SourAddr,MsgSend->sourlen);
    MsgSend->destlen = pAmrThreadArg->RelayTrans.DestLen;
    memcpy(MsgSend->destaddr,pAmrThreadArg->RelayTrans.DestAddr,MsgSend->destlen );
    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);
    //TASK_FMT_DEBUG(pipe, RELAY_LOG,"当日抄读状态\n");
    //TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgSend->playload,MsgSend->msglen);
    taskmng_plc_queue_write(pAmrSendUp[pipe], (uint8*)MsgSend, sizeof(MESSAGE_INFO_T)+index);
    free(MsgSend);

}


/**
*********************************************************************
* @brief:      检查是否有合法的穿透任务 现场由于查询当日抄读状态太多，可能会导致正常穿透任务延时很久才会执行
            如果判断出是0013则会一直执行
* @param[in]：  uint8 pipe  线程号
* @return：0 表示没有穿透任务 1表示有穿透任务 -1表示是查询当日抄读状态，此时要一直执行
*********************************************************************
*/
int taskmng_relay_exist_check(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    uint16 Recvlen ;
    uint8 RecvBuf[MAX_MSG_LEN]={0};
    MESSAGE_INFO_T * MsgInfo = NULL;
    uint8 PayloadBuf[MAX_MSG_LEN] = {0};

#ifdef AREA_FUJIAN
    int ret;

    if(pipe == PLC_NUM+MAX_485_NUM+COMM_MODULE_NUM)
    {
        ret = taskmng_relay_check_tg();
        if(ret == -2)
            return 0;
        else
            return -1;
    }
#endif
    Recvlen = taskmng_plc_queue_read(pAmrRecvUp[pipe], RecvBuf);
    if(Recvlen>0)
    {
        MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
        memcpy(PayloadBuf,MsgInfo->playload,MsgInfo->msglen);
        if(MsgInfo->IOP != 0x0013)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG, "存在穿透消息 IID %04X IOP %04X payload \n",MsgInfo->IID,MsgInfo->IOP);
            TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
        }
        memset(&pAmrThreadArg->RelayTrans,0x00,sizeof(RELAY_TRANS_T));

        pAmrThreadArg->RelayTrans.MsgRPM = 0;
        pAmrThreadArg->RelayTrans.MsgPRIORITY = MsgInfo->priority;
        pAmrThreadArg->RelayTrans.MsgIndex = MsgInfo->index;
        pAmrThreadArg->RelayTrans.MsgLabel = MsgInfo->label;
        pAmrThreadArg->RelayTrans.SourLen = MsgInfo->destlen;
        memcpy(pAmrThreadArg->RelayTrans.SourAddr,MsgInfo->destaddr,MsgInfo->destlen);
        pAmrThreadArg->RelayTrans.DestLen = MsgInfo->sourlen;
        memcpy(pAmrThreadArg->RelayTrans.DestAddr,MsgInfo->souraddr,MsgInfo->sourlen);
        pAmrThreadArg->RelayTrans.IID = MsgInfo->IID;
        pAmrThreadArg->RelayTrans.IOP = MsgInfo->IOP;
        switch(MsgInfo->IOP)
        {

        case 0x0010:
        {
            if(taskmng_transparent_msg_unpack(pipe,MsgInfo->msglen,MsgInfo->playload))
            {
                pAmrThreadArg->RelayTrans.isValid = 1;
                pAmrThreadArg->RelayTrans.RelayResult =0;
            }
            else
            {
                if(pipe==PLC_NUM)
                {
                    taskmng_relay_up_msg_write_22( pipe,1,MsgInfo->msglen,MsgInfo->playload);

                }
                return 0;
            }

        }
            break;
        case 0x0011: //代理 先解析 看属于哪种代理
        {
            if(taskmng_proxy_msg_unpack(pipe,MsgInfo->playload,MsgInfo->msglen))
            {
                pAmrThreadArg->RelayTrans.isValid = 1;
                pAmrThreadArg->RelayTrans.RelayResult =0;
            }else
            {
               OOP_OAD_U pipeoad ;
               pipeoad.value = pAmrThreadArg->RelayTrans.OadPort;
               if(pipe==PLC_NUM && pipeoad.nObjID != 0XF220)
               {
                    TASK_FMT_DEBUG(-1, UP_LOG, "代理0x11上行回复错误信息 \n");
                    taskmng_relay_up_msg_write_0011( pipe,1,MsgInfo->msglen,MsgInfo->playload);
               }

               return 0;
            }

        }
            break;

        case 0x0013: //收到查询当日抄表状态
        {
            taskmng_amr_status_get(pipe,MsgInfo->playload,MsgInfo->msglen);
            //直接查询到就回复 无需中继
            return -1;
        }
            break;
        default:
            break;
        }
        return 1;
    }
    return 0;

}
int froen_real_00100200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00100201, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[0], sizeof(uint32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00100203, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[2], sizeof(uint32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00100205, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[4], sizeof(uint32));
    }
    else
    {
        cJSON *jData_0010020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0010020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100201, pData->colStoreTm, pData->colEndTm, intv, jData_0010020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0010020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100202, pData->colStoreTm, pData->colEndTm, intv, jData_0010020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0010020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100203, pData->colStoreTm, pData->colEndTm, intv, jData_0010020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0010020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100204, pData->colStoreTm, pData->colEndTm, intv, jData_0010020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0010020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100205, pData->colStoreTm, pData->colEndTm, intv, jData_0010020X);
    }
    return 0;
}

int froen_real_00200200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        memcpy(ENERGY_T,pData->inDataBuf,pData->inDatalen);
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00200201, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[0], sizeof(uint32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00200203, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[2], sizeof(uint32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_00200205, pData->colStoreTm, (uint8*)&ENERGY_T->nValue[4], sizeof(uint32));
    }
    else
    {
        cJSON *jData_0020020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0020020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00200201, pData->colStoreTm, pData->colEndTm, intv, jData_0020020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0020020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00100202, pData->colStoreTm, pData->colEndTm, intv, jData_0020020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0020020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00200203, pData->colStoreTm, pData->colEndTm, intv, jData_0020020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0020020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00200204, pData->colStoreTm, pData->colEndTm, intv, jData_0020020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0020020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300205, pData->colStoreTm, pData->colEndTm, intv, jData_0020020X);
    }
    return 0;
}


int froen_real_00300200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0030020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0030020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300201, pData->colStoreTm, pData->colEndTm, intv, jData_0030020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0030020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300202, pData->colStoreTm, pData->colEndTm, intv, jData_0030020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0030020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300203, pData->colStoreTm, pData->colEndTm, intv, jData_0030020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0030020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300204, pData->colStoreTm, pData->colEndTm, intv, jData_0030020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0030020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00300205, pData->colStoreTm, pData->colEndTm, intv, jData_0030020X);
    }
    return 0;
}

int froen_real_00400200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0040020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0040020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00400201, pData->colStoreTm, pData->colEndTm, intv, jData_0040020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0040020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00400202, pData->colStoreTm, pData->colEndTm, intv, jData_0040020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0040020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00400203, pData->colStoreTm, pData->colEndTm, intv, jData_0040020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0040020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00400204, pData->colStoreTm, pData->colEndTm, intv, jData_0040020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0040020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00400205, pData->colStoreTm, pData->colEndTm, intv, jData_0040020X);
    }
    return 0;
}

int froen_real_00500200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0050020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0050020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00500201, pData->colStoreTm, pData->colEndTm, intv, jData_0050020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0050020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00500202, pData->colStoreTm, pData->colEndTm, intv, jData_0050020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0050020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00500203, pData->colStoreTm, pData->colEndTm, intv, jData_0050020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0050020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00500204, pData->colStoreTm, pData->colEndTm, intv, jData_0050020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0050020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00500205, pData->colStoreTm, pData->colEndTm, intv, jData_0050020X);
    }
    return 0;
}

int froen_real_00600200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0060020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0060020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00600201, pData->colStoreTm, pData->colEndTm, intv, jData_0060020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0060020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00600202, pData->colStoreTm, pData->colEndTm, intv, jData_0060020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0060020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00600203, pData->colStoreTm, pData->colEndTm, intv, jData_0060020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0060020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00600204, pData->colStoreTm, pData->colEndTm, intv, jData_0060020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0060020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00600205, pData->colStoreTm, pData->colEndTm, intv, jData_0060020X);
    }
    return 0;
}

int froen_real_00700200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0070020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00700201, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00700202, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00700203, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00700204, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00700205, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);
    }
    return 0;
}

int froen_real_00800200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_ENERGY_T *ENERGY_T = (OOP_ENERGY_T*)pData->inDataBuf;

    {
        cJSON *jData_0070020X = NULL;
        float  temp = 0;

        temp = ENERGY_T->nValue[0] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00800201, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[1] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00800202, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[2] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00800203, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[3] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00800204, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);

        temp = ENERGY_T->nValue[4] / 100.0;
        jData_0070020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x00800205, pData->colStoreTm, pData->colEndTm, intv, jData_0070020X);
    }
    return 0;
}

int froen_real_20000200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_WORD3V_T *WORD3V_T= (OOP_WORD3V_T*)pData->inDataBuf;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        memcpy(WORD3V_T,pData->inDataBuf,pData->inDatalen);
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000201, pData->colStoreTm, (uint8*)&WORD3V_T->nValue[0], sizeof(uint16));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000202, pData->colStoreTm, (uint8*)&WORD3V_T->nValue[1], sizeof(uint16));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000203, pData->colStoreTm, (uint8*)&WORD3V_T->nValue[2], sizeof(uint16));
    }
    else
    {
        cJSON *jData_2000020X = NULL;
        float  temp = 0;

        temp = WORD3V_T->nValue[0] / 10.0;
        jData_2000020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x20000201, pData->colStoreTm, pData->colEndTm, intv, jData_2000020X);

        temp = WORD3V_T->nValue[1] / 10.0;
        jData_2000020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x20000202, pData->colStoreTm, pData->colEndTm, intv, jData_2000020X);

        temp = WORD3V_T->nValue[2] / 10.0;
        jData_2000020X = cJSON_CreateNumber(temp);
        min_gather_insert_item(pGdate, type, 0x20000203, pData->colStoreTm, pData->colEndTm, intv, jData_2000020X);

    }
    return 0;
}

int froen_real_20010200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_INT3V_T  *INT3V_T=(OOP_INT3V_T*)pData->inDataBuf;;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000201, pData->colStoreTm, (uint8*)&INT3V_T->nValue[0], sizeof(int32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000202, pData->colStoreTm, (uint8*)&INT3V_T->nValue[1], sizeof(int32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20000203, pData->colStoreTm, (uint8*)&INT3V_T->nValue[2], sizeof(int32));
    }
    return 0;
}
int froen_real_20010400(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    int32 *value =(int32*)pData->inDataBuf;

    if(MIN_GATHER_TYPE_REAL == type)
    {

        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20010400, pData->colStoreTm, (uint8*)value, sizeof(int32));

    }
    return 0;
}

int froen_real_20010500(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    int32 *value =(int32*)pData->inDataBuf;;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        memcpy(value,pData->inDataBuf,pData->inDatalen);
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20010500, pData->colStoreTm, (uint8*)value, sizeof(int32));

    }
    return 0;
}

int froen_real_20040200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_INT4V_T  *INT4V_T=(OOP_INT4V_T*)pData->inDataBuf;

    if(MIN_GATHER_TYPE_REAL == type)
    {
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20040201, pData->colStoreTm, (uint8*)&INT4V_T->nValue[0], sizeof(int32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20040202, pData->colStoreTm, (uint8*)&INT4V_T->nValue[1], sizeof(int32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20040203, pData->colStoreTm, (uint8*)&INT4V_T->nValue[2], sizeof(int32));
        min_gather_insert_data(pGdate, MIN_GATHER_OAD_20040204, pData->colStoreTm, (uint8*)&INT4V_T->nValue[3], sizeof(int32));

    }
    return 0;
}

int froen_10100200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_DEMAND_T *DEMAND_T = (OOP_DEMAND_T*)pData->inDataBuf;
    char          tempstr[32] = {0};

    cJSON *jData_1010020X = NULL;
    float  temp = 0;

    temp = DEMAND_T->demand[0].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[0].time.year, DEMAND_T->demand[0].time.month,
                                                      DEMAND_T->demand[0].time.day, DEMAND_T->demand[0].time.hour,
                                                      DEMAND_T->demand[0].time.minute, DEMAND_T->demand[0].time.second);
    jData_1010020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1010020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1010020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100201, pData->colStoreTm, pData->colEndTm, intv, jData_1010020X);


    temp = DEMAND_T->demand[1].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[1].time.year, DEMAND_T->demand[1].time.month,
                                                      DEMAND_T->demand[1].time.day, DEMAND_T->demand[1].time.hour,
                                                      DEMAND_T->demand[1].time.minute, DEMAND_T->demand[1].time.second);
    jData_1010020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1010020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1010020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100202, pData->colStoreTm, pData->colEndTm, intv, jData_1010020X);

    temp = DEMAND_T->demand[2].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[2].time.year, DEMAND_T->demand[2].time.month,
                                                      DEMAND_T->demand[2].time.day, DEMAND_T->demand[2].time.hour,
                                                      DEMAND_T->demand[2].time.minute, DEMAND_T->demand[2].time.second);
    jData_1010020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1010020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1010020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100203, pData->colStoreTm, pData->colEndTm, intv, jData_1010020X);

    temp = DEMAND_T->demand[3].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[3].time.year, DEMAND_T->demand[3].time.month,
                                                      DEMAND_T->demand[3].time.day, DEMAND_T->demand[3].time.hour,
                                                      DEMAND_T->demand[3].time.minute, DEMAND_T->demand[3].time.second);
    jData_1010020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1010020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1010020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100204, pData->colStoreTm, pData->colEndTm, intv, jData_1010020X);

    temp = DEMAND_T->demand[4].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[4].time.year, DEMAND_T->demand[4].time.month,
                                                      DEMAND_T->demand[4].time.day, DEMAND_T->demand[4].time.hour,
                                                      DEMAND_T->demand[4].time.minute, DEMAND_T->demand[4].time.second);
    jData_1010020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1010020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1010020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100205, pData->colStoreTm, pData->colEndTm, intv, jData_1010020X);

    return 0;
}

int froen_10200200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_DEMAND_T *DEMAND_T = (OOP_DEMAND_T*)pData->inDataBuf;
    char          tempstr[32] = {0};

    cJSON *jData_1020020X = NULL;
    float  temp = 0;

    temp = DEMAND_T->demand[0].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[0].time.year, DEMAND_T->demand[0].time.month,
                                                      DEMAND_T->demand[0].time.day, DEMAND_T->demand[0].time.hour,
                                                      DEMAND_T->demand[0].time.minute, DEMAND_T->demand[0].time.second);
    jData_1020020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1020020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1020020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100201, pData->colStoreTm, pData->colEndTm, intv, jData_1020020X);


    temp = DEMAND_T->demand[1].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[1].time.year, DEMAND_T->demand[1].time.month,
                                                      DEMAND_T->demand[1].time.day, DEMAND_T->demand[1].time.hour,
                                                      DEMAND_T->demand[1].time.minute, DEMAND_T->demand[1].time.second);
    jData_1020020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1020020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1020020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100202, pData->colStoreTm, pData->colEndTm, intv, jData_1020020X);

    temp = DEMAND_T->demand[2].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[2].time.year, DEMAND_T->demand[2].time.month,
                                                      DEMAND_T->demand[2].time.day, DEMAND_T->demand[2].time.hour,
                                                      DEMAND_T->demand[2].time.minute, DEMAND_T->demand[2].time.second);
    jData_1020020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1020020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1020020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100203, pData->colStoreTm, pData->colEndTm, intv, jData_1020020X);

    temp = DEMAND_T->demand[3].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[3].time.year, DEMAND_T->demand[3].time.month,
                                                      DEMAND_T->demand[3].time.day, DEMAND_T->demand[3].time.hour,
                                                      DEMAND_T->demand[3].time.minute, DEMAND_T->demand[3].time.second);
    jData_1020020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1020020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1020020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100204, pData->colStoreTm, pData->colEndTm, intv, jData_1020020X);

    temp = DEMAND_T->demand[4].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[4].time.year, DEMAND_T->demand[4].time.month,
                                                      DEMAND_T->demand[4].time.day, DEMAND_T->demand[4].time.hour,
                                                      DEMAND_T->demand[4].time.minute, DEMAND_T->demand[4].time.second);
    jData_1020020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1020020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1020020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100205, pData->colStoreTm, pData->colEndTm, intv, jData_1020020X);

    return 0;
}

int froen_10300200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_DEMAND_T *DEMAND_T = (OOP_DEMAND_T*)pData->inDataBuf;
    char          tempstr[32] = {0};

    cJSON *jData_1030020X = NULL;
    float  temp = 0;

    temp = DEMAND_T->demand[0].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[0].time.year, DEMAND_T->demand[0].time.month,
                                                      DEMAND_T->demand[0].time.day, DEMAND_T->demand[0].time.hour,
                                                      DEMAND_T->demand[0].time.minute, DEMAND_T->demand[0].time.second);
    jData_1030020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1030020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1030020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100201, pData->colStoreTm, pData->colEndTm, intv, jData_1030020X);


    temp = DEMAND_T->demand[1].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[1].time.year, DEMAND_T->demand[1].time.month,
                                                      DEMAND_T->demand[1].time.day, DEMAND_T->demand[1].time.hour,
                                                      DEMAND_T->demand[1].time.minute, DEMAND_T->demand[1].time.second);
    jData_1030020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1030020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1030020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100202, pData->colStoreTm, pData->colEndTm, intv, jData_1030020X);

    temp = DEMAND_T->demand[2].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[2].time.year, DEMAND_T->demand[2].time.month,
                                                      DEMAND_T->demand[2].time.day, DEMAND_T->demand[2].time.hour,
                                                      DEMAND_T->demand[2].time.minute, DEMAND_T->demand[2].time.second);
    jData_1030020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1030020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1030020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100203, pData->colStoreTm, pData->colEndTm, intv, jData_1030020X);

    temp = DEMAND_T->demand[3].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[3].time.year, DEMAND_T->demand[3].time.month,
                                                      DEMAND_T->demand[3].time.day, DEMAND_T->demand[3].time.hour,
                                                      DEMAND_T->demand[3].time.minute, DEMAND_T->demand[3].time.second);
    jData_1030020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1030020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1030020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100204, pData->colStoreTm, pData->colEndTm, intv, jData_1030020X);

    temp = DEMAND_T->demand[4].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[4].time.year, DEMAND_T->demand[4].time.month,
                                                      DEMAND_T->demand[4].time.day, DEMAND_T->demand[4].time.hour,
                                                      DEMAND_T->demand[4].time.minute, DEMAND_T->demand[4].time.second);
    jData_1030020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1030020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1030020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100205, pData->colStoreTm, pData->colEndTm, intv, jData_1030020X);

    return 0;
}

int froen_10400200(int cid, uint16 type, WRITE_RECORD_T *pData, uint32 intv, void *pGdate)
{
    OOP_DEMAND_T *DEMAND_T = (OOP_DEMAND_T*)pData->inDataBuf;
    char          tempstr[32] = {0};

    cJSON *jData_1040020X = NULL;
    float  temp = 0;

    temp = DEMAND_T->demand[0].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[0].time.year, DEMAND_T->demand[0].time.month,
                                                      DEMAND_T->demand[0].time.day, DEMAND_T->demand[0].time.hour,
                                                      DEMAND_T->demand[0].time.minute, DEMAND_T->demand[0].time.second);
    jData_1040020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1040020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1040020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100201, pData->colStoreTm, pData->colEndTm, intv, jData_1040020X);


    temp = DEMAND_T->demand[1].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[1].time.year, DEMAND_T->demand[1].time.month,
                                                      DEMAND_T->demand[1].time.day, DEMAND_T->demand[1].time.hour,
                                                      DEMAND_T->demand[1].time.minute, DEMAND_T->demand[1].time.second);
    jData_1040020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1040020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1040020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100202, pData->colStoreTm, pData->colEndTm, intv, jData_1040020X);

    temp = DEMAND_T->demand[2].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[2].time.year, DEMAND_T->demand[2].time.month,
                                                      DEMAND_T->demand[2].time.day, DEMAND_T->demand[2].time.hour,
                                                      DEMAND_T->demand[2].time.minute, DEMAND_T->demand[2].time.second);
    jData_1040020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1040020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1040020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100203, pData->colStoreTm, pData->colEndTm, intv, jData_1040020X);

    temp = DEMAND_T->demand[3].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[3].time.year, DEMAND_T->demand[3].time.month,
                                                      DEMAND_T->demand[3].time.day, DEMAND_T->demand[3].time.hour,
                                                      DEMAND_T->demand[3].time.minute, DEMAND_T->demand[3].time.second);
    jData_1040020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1040020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1040020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100204, pData->colStoreTm, pData->colEndTm, intv, jData_1040020X);

    temp = DEMAND_T->demand[4].nValue / 10000.0;
    sprintf(tempstr, "%04d-%02d-%02d %02d:%02d:%02d", DEMAND_T->demand[4].time.year, DEMAND_T->demand[4].time.month,
                                                      DEMAND_T->demand[4].time.day, DEMAND_T->demand[4].time.hour,
                                                      DEMAND_T->demand[4].time.minute, DEMAND_T->demand[4].time.second);
    jData_1040020X = cJSON_CreateObject();
    cJSON_AddItemToObject(jData_1040020X, "mv", cJSON_CreateNumber(temp));
    cJSON_AddItemToObject(jData_1040020X, "mdt", cJSON_CreateString(tempstr));
    min_gather_insert_item(pGdate, type, 0x10100205, pData->colStoreTm, pData->colEndTm, intv, jData_1040020X);

    return 0;
}



MIN_GATHER_FUN_T g_FrozenJs[] =
{
    {0x00100200, froen_real_00100200},
    {0x00200200, froen_real_00200200},
    {0x00300200, froen_real_00300200},
    {0x00400200, froen_real_00400200},
    {0x00500200, froen_real_00500200},
    {0x00600200, froen_real_00600200},
    {0x00700200, froen_real_00700200},
    {0x00800200, froen_real_00800200},
    {0x10100200, froen_10100200},
    {0x10200200, froen_10200200},
    {0x10300200, froen_10300200},
    {0x10400200, froen_10400200},
};
uint32 g_FrozenJsNum = sizeof(g_FrozenJs)/sizeof(MIN_GATHER_FUN_T);

MIN_GATHER_FUN_T g_RealJs[] =
{
    {0x00100200, froen_real_00100200},
    {0x00200200, froen_real_00200200},
    {0x20000200, froen_real_20000200},
    {0x20010200, froen_real_20010200},
    {0x20010400, froen_real_20010400},
    {0x20010500, froen_real_20010500},
    {0x20040200, froen_real_20040200},
};
uint32 g_RealJsNum = sizeof(g_RealJs)/sizeof(MIN_GATHER_FUN_T);



int frozen_init_jiangsu()
{
    return min_gather_init(g_FrozenJs, g_FrozenJsNum, g_RealJs, g_RealJsNum, 59, 60);
}

//int JudgeIsSupportSlaveNodeSample(uint8 pipe)
//{
//    taskmng_HPLC_node_info_get(pipe)
//
//}

/*******************************************************************************
* 函数名称: taskmng_encrypt_init_send
* 函数功能:初始化过程 获取esam信息
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_init_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    uint16 meterindex = RunTask->MeterIndex;    //当前执行到的表
    uint16 pn = TaskInfo->MsInfo[meterindex].Sn;
    uint8 prtl = 0;
    taskmng_mtr_prtl_get_new(pn,LOGIC_ID_YC,&prtl);
    int ret;
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    uint8 addr[6]={0};
    TASK_FMT_TRACE( pipe, REC_LOG, "转加密初始化 pn %d  prtl %d \n",pn,prtl);

    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    switch(prtl)
    {
        case PRTL_OOP:
        {
            //获取单地址应用协商计数器 F1000701
            OOP_OAD_U oad = {0xF1000701};
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            LinkLayer_t *pLinkLayer = NULL;
            OopGet.requst(pipe,&tsa,1,(uint8*)&oad.value,4);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }
        }
        break;
        case PRTL_20_IOT:
        {
            //获取随机数
            uint8 rand1[100]={0};
            uint8 buf[100]={0};
            uint16 index =0;

            //获取8位随机数1
            ret = security_rand1_get(rand1);
            if(ret==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取随机数1失败 \n",pn);
                return -1;
            }
            //电表认证 F1000B00 终端和电能表的身份认证与原红外认证使用相同的协议
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            buf[index++] = 0xF1;
            buf[index++] = 0x00;
            buf[index++] = 0x0B;
            buf[index++] = 0x00;
            buf[index++] = DT_RN;
            buf[index++] = ret;
            memcpy(&buf[index],rand1,ret);
            index+=ret;
            OopAction.requst(pipe,&tsa,1,buf,index);
            LinkLayer_t *pLinkLayer = NULL;
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }

        }
        break;
        case PRTL_07:
        {
            uint32 DI = 0x04000402;
            MeterPrtlFmt_T meter;
            memset(&meter,0x00,sizeof(MeterPrtlFmt_T));
            taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
            memcpy(meter.MtrAddr,addr,6);
            meter.CtrlCode = 0x11;
            meter.DataLen = 4;
            memcpy(meter.DataBuff,&DI,4);

            sendlen = taskmng_meter_prtl_645_pack(sendbuf, meter);
        }
        break;
        default:
            return -1;

    }
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d taskmng_encrypt_init_send \n",pn);
    TASK_BUF_TRACE( pipe, REC_LOG, sendbuf,sendlen);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);

    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);

    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_encrypt_counter_send
* 函数功能:获取计数器
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/

int taskmng_encrypt_counter_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    //uint8 addr[6]={0};
    TASK_FMT_TRACE( pipe, REC_LOG, "获取计数器 prtl %d \n",prtl);

    switch(prtl)
    {
        //最新的改为F1001400终端与电表会话计数器 不再用广播计数器
        case PRTL_20_IOT://F1001300 获取
        {
            LinkLayer_t *pLinkLayer = NULL;
            OOP_TSA_T tsa;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            OOP_OAD_U oad = {0xF1001400};
            OopGet.requst(pipe,&tsa,1,(uint8*)&oad.value,4);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }

        }
            break;
        default:
            break;
    }
    //taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
    //SendFrame_0038(pipe,prtl,addr,sendbuf,sendlen,&RunTask->encryptMsgIndex);

    TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_encrypt_counter_send 发送报文\n");
    TASK_BUF_TRACE( pipe, REC_LOG, sendbuf,sendlen);

    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_encrypt_connect_send
* 函数功能:建立应用连接
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/

int taskmng_encrypt_connect_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    uint8 addr[6]={0};
    int ret;
    uint16 metindex = RunTask->MeterIndex;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    TASK_FMT_TRACE( pipe, REC_LOG, "建立应用连接 \n");
    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT:
        {
            //从esam获取密钥包 将任务里面的电表密钥包发给esam
            uint8 sessiondata1[32]={0};
            uint8 mac1[4]={0};
            OOP_OCTETVAR256_T *key = &TaskInfo->MsInfo[metindex].key;
            ret = TESAM_RcvSndDataAndCheck(key->value,key->nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取电表密钥包失败 \n",pn);
                return -1;
            }
            RunTask->key.nNum = recvlen;
            memcpy(RunTask->key.value,recvbuf,recvlen);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取电表密钥包 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);
            //发送esam获取SessionData1+MAC1
            memset(recvbuf,0x00,sizeof(recvbuf));
            ret = security_sessondata1_get(RunTask->key.value,RunTask->key.nNum,RunTask->encryptCounter,recvbuf);
            if(ret!=36)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_sessondata1_get失败 ret %d \n",pn,ret);
                return -1;
            }
            memcpy(sessiondata1,recvbuf,32);
            memcpy(mac1,recvbuf+32,4);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取sessiondata1 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,sessiondata1,32);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d MAC1 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,mac1,4);

            //和电表建立应用连接
            LinkLayer_t *pLinkLayer = NULL;
            OOP_TSA_T tsa;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
           //建应用连接用逻辑地址0 建应用连接还得用1
           // tsa.flag = 0;
            //tsa.oldvxd = 0;
            OopConnect.requst(pipe,&tsa,2,1800,sessiondata1,mac1);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }

        }
            break;
        case PRTL_07:
        {
            //把任务里的电表密钥发给esam
            OOP_OCTETVAR256_T *key = &TaskInfo->MsInfo[metindex].key;
            ret = TESAM_RcvSndDataAndCheck(key->value,key->nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取电表密钥包失败 \n",pn);
                return -1;
            }
            RunTask->key.nNum = recvlen;
            memcpy(RunTask->key.value,recvbuf,recvlen);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取电表密钥包 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);
            //获取R0随机数
            uint8 R0[8]={0};
            ret = security_rand1_get(R0);
            if(ret==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取R0失败 \n",pn);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取R0随机数 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,R0,8);
            //获取ER0
            uint8 ER0[8]={0};
            memset(recvbuf,0x00,sizeof(recvbuf));
            //身份认证任务中的电表身份认证密钥密文 只取前32个字节
            ret = security_ER0_get(RunTask->key.value,32,RunTask->encryptMetNo,R0,recvbuf);
            if(ret!=8)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_ER0_get ret %d \n",pn,ret);
                return -1;
            }
            memcpy(ER0,recvbuf,ret);
            //电表认证
            uint32 DI = 0x070000FF;
            uint16 index = 0;
            MeterPrtlFmt_T meter;
            memset(&meter,0x00,sizeof(MeterPrtlFmt_T));
            taskmng_mtr_addr_get(pn,0,addr);
            memcpy(meter.MtrAddr,addr,6);
            meter.CtrlCode = 0x03;
            meter.DataLen = 32;
            memcpy(meter.DataBuff,&DI,4);
            index+=4;
            //4字节操作者
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            memcpy_r(&meter.DataBuff[index],ER0,8);
            index+=8;
            memcpy_r(&meter.DataBuff[index],R0,8);
            index+=8;
            //meter.DataBuff[index++] = 0;
            //meter.DataBuff[index++] = 0;
            memcpy(&meter.DataBuff[index],RunTask->encryptMetNo,8);
            index+=8;
            sendlen = taskmng_meter_prtl_645_pack(sendbuf, meter);
        }
            break;
        default:
            break;
    }
    //taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
    //SendFrame_0038(pipe,prtl,addr,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 发送 MsgIndex %d \n",pn,RunTask->encryptMsgIndex);
    TASK_BUF_TRACE( pipe, REC_LOG,sendbuf,sendlen);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_encrypt_timing_send
* 函数功能:对时
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/

int taskmng_encrypt_timing_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    uint8 addr[6]={0};
    int ret;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    TASK_FMT_TRACE( pipe, REC_LOG, "电表校时 pn %d \n",pn);
    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT://F1011300 获取
        {
            //将任务中的报文发给esam

            ret = TESAM_RcvSndDataAndCheck(TaskInfo->EncryptInfo.apdu.value ,TaskInfo->EncryptInfo.apdu.nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 失败 \n",pn);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);

            //发送esam 计算校时密文数据
            uint8 databuf[2000]={0};
            ret = security_taskdata_get(RunTask->sessionKey.value,RunTask->sessionKey.nNum,recvbuf,recvlen,databuf);
            //4字节标识 2字节附加数据 32字节endata 4字节mac
            //698表38个字节
            if(ret==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_taskdata_get ret %d \n",pn,ret);
                return -1;
            }

            //发送密文至电表
            LinkLayer_t *pLinkLayer = NULL;
            OOP_TSA_T tsa;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            //此处append_data 要给esam回的前8个字节 包括两个字节长度 还有后面的6字节sid
            uint8 appenddata[100] = {0};
            uint8 tasklen;

            tasklen = ret -10;
            if(prtl == PRTL_20_IOT)
            {
                appenddata[0] = 0;
                appenddata[1] = ret;
                memcpy(appenddata+2,databuf,6);
                security_request_encrypt(pipe,&tsa,databuf+6,tasklen,databuf,appenddata,databuf+6+tasklen);
            }
            else
            {
                //698表把后面的四个字节也加上
                tasklen +=4;
                memcpy(appenddata,databuf+4,2);
                security_request_encrypt_698(pipe,&tsa,databuf+6,tasklen,databuf,appenddata);
            }
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }
        }
            break;
        case PRTL_07:
        {
            uint8 secdata[1000]={0};
            uint16 seclen;
            ret = TESAM_RcvSndDataAndCheck(TaskInfo->EncryptInfo.apdu.value ,TaskInfo->EncryptInfo.apdu.nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 失败 \n",pn);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);
            memcpy(secdata,recvbuf,recvlen);
            seclen = recvlen;
            memset(recvbuf,0x00,sizeof(recvbuf));

            ret = security_secdata_get_645(secdata,seclen,RunTask->key.value+32,32,RunTask->encryptR2,recvbuf);
            //4字节数据标识 1字节长度
            if(ret==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_secdata_get_645 ret %d \n",pn,ret);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_secdata_get_645 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,ret);
            uint8 databuf[2000]={0};
            uint16 datalen;
            uint8 mac[4];
            datalen = ret -4 -1-5-4;
            memcpy(databuf,recvbuf+5+5,datalen);
            memcpy(mac,recvbuf+5+5+datalen,4);
            TASK_FMT_TRACE( pipe, REC_LOG, "任务密文数据\n");
            TASK_BUF_TRACE(pipe, REC_LOG,databuf,datalen);
            TASK_FMT_TRACE( pipe, REC_LOG, "mac\n");
            TASK_BUF_TRACE(pipe, REC_LOG,mac,4);

            uint32 DI = 0x0400010C;
            uint16 index = 0;
            MeterPrtlFmt_T meter;
            memset(&meter,0x00,sizeof(MeterPrtlFmt_T));
            taskmng_mtr_addr_get(pn,0,addr);
            memcpy(meter.MtrAddr,addr,6);
            meter.CtrlCode = 0x14;
            memcpy(meter.DataBuff,&DI,4);
            index+=4;
            //密码
            meter.DataBuff[index++] = 0x98;// 98H级密码权限代表通过密文+MAC的方式进行数据传输，不需要进行密码验证，也不需要编程键配合。
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            //4字节操作者
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            meter.DataBuff[index++] = 0;
            memcpy_r(&meter.DataBuff[index],databuf,datalen);
            index+=datalen;
            memcpy_r(&meter.DataBuff[index],mac,4);
            index+=4;
            meter.DataLen = index;
            sendlen = taskmng_meter_prtl_645_pack(sendbuf, meter);
        }
            break;

        default:
            break;
    }
    //taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
    //SendFrame_0038(pipe,prtl,addr,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 发送 MsgIndex %d \n",pn,RunTask->encryptMsgIndex);
    TASK_BUF_TRACE( pipe, REC_LOG,sendbuf,sendlen);
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_encrypt_esamset_send
* 函数功能:设置esam参数
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/

int taskmng_encrypt_esamset_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    //uint8 addr[6]={0};
    int ret;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    uint8 taskdata[1000]={0};
    uint16 tasklen;
    uint8 paramdata[1000]={0};
    uint16 paramlen;
    uint8 endata2[1000]={0};
    uint16 endata2len;
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 设置esam参数 \n",pn);
    switch(prtl)
    {
        case PRTL_OOP:
        {

        }
            break;
        case PRTL_20_IOT:
        {
            //将任务中的报文发给esam

            ret = TESAM_RcvSndDataAndCheck(TaskInfo->EncryptInfo.apdu.value ,TaskInfo->EncryptInfo.apdu.nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 失败 \n",pn);
                return -1;
            }
            tasklen = recvlen;
            memcpy(taskdata,recvbuf,recvlen);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);
            //将任务中的参数发给esam
            ret = TESAM_RcvSndDataAndCheck(TaskInfo->EncryptInfo.param.value ,TaskInfo->EncryptInfo.param.nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取参数密文数据 失败 \n",pn);
                return -1;
            }
            paramlen = recvlen;
            memcpy(paramdata,recvbuf,recvlen);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取参数密文数据 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);
            //获取endata2
            ret = security_endata2_get(RunTask->sessionKey.value ,RunTask->sessionKey.nNum,paramdata,paramlen, recvbuf);
            if(ret == 0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取endata2 失败 \n",pn);
                return -1;
            }
            endata2len = ret;
            memcpy(endata2,recvbuf,ret);
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取endata2 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,endata2,endata2len);
            //获取esam设置 任务密文
            ret = security_esamset_data_get(RunTask->sessionKey.value ,RunTask->sessionKey.nNum,endata2,endata2len,taskdata,tasklen, recvbuf);
            if(ret == 0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取esam设置密文数据 失败 \n",pn);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取esam设置密文数据  \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,ret);
            //4字节标识 2字节附加数据 44字节endata3 4字节mac
            uint16 endata3len = ret-6-4;

            //发送密文至电表
            LinkLayer_t *pLinkLayer = NULL;
            OOP_TSA_T tsa;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            //此处append_data 要给esam回的前8个字节 包括两个字节长度 还有后面的6字节sid
            uint8 appenddata[100] = {0};
            appenddata[0] = 0;
            appenddata[1] = ret;
            memcpy(appenddata+2,recvbuf,6);
            security_request_encrypt(pipe,&tsa,recvbuf+6,endata3len,recvbuf,appenddata,recvbuf+6+endata3len);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }

        }
            break;
        default:
            break;
    }
    //taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
    //SendFrame_0038(pipe,prtl,addr,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 发送 MsgIndex %d \n",pn,RunTask->encryptMsgIndex);
    TASK_BUF_TRACE( pipe, REC_LOG,sendbuf,sendlen);
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_encrypt_send
* 函数功能:转加密任务报文发送 根据任务类型及电表类型发送不同的数据 和esam交互同步 和电表交互异步
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:1 全部抄完 -1 未抄完 2 不切换发送状态
*******************************************************************************/

int taskmng_encrypt_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{

    uint16 msindex; //电表索引
    int ret;
    //获取当前需要执行的电表 再根据状态机判断需要进行的操作
    uint16 meterindex = RunTask->MeterIndex;    //当前执行到的表
    uint16 pn = TaskInfo->MsInfo[meterindex].Sn;
    uint8 prtl;
    uint8 addr[6]={0};
    taskmng_mtr_prtl_get_new(pn,LOGIC_ID_YC,&prtl);
    if(RunTask->encryptFailCnt>=3)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 失败次数过多 \n",pn);
        RunTask->encryptStatus = ENCRYPT_END;
    }

    switch(RunTask->encryptStatus)
    {
    case ENCRYPT_INIT:
    {
        //程序重启从记录文件中加载出的结果
        if(RunTask->SucNum>=RunTask->NeedMtrNum)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 任务结束 \n");
            RunTask->RunResult = SCHEME_FINISHED;
            return 1;
        }
        //会出现前面的表失败重新抄读的情况
        while(1)
        {
            if(RunTask->MeterIndex>=TaskInfo->TotalNum)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 任务结束 \n");
                return 1;
            }
            if(taskmng_mtr_result_check(RunTask->MeterIndex,RunTask->MtrResult)==TRUE ||
                    TaskInfo->MsInfo[RunTask->MeterIndex].Pipe!=pipe)
            {
                RunTask->MeterIndex++;
                continue;
            }
            break;
        }

        ret = taskmng_encrypt_init_send(pipe,TaskInfo,RunTask);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "和esam交互失败 任务结束\n");
            return 1;
        }

    }
        break;
    case ENCRYPT_COUNTER_GET:
    {
        ret = taskmng_encrypt_counter_send(pipe,TaskInfo,RunTask,pn,prtl);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "ENCRYPT_COUNTER_GET error \n");
            return 1;
        }
    }
        break;
    case ENCRYPT_CONNECT:
    {
        ret = taskmng_encrypt_connect_send(pipe,TaskInfo,RunTask,pn,prtl);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "ENCRYPT_CONNECT error \n");
            return 1;
        }

    }
        break;
    case ENCRYPT_TIMING:
    {
        ret = taskmng_encrypt_timing_send(pipe,TaskInfo,RunTask,pn,prtl);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_encrypt_timing_send error \n");
            return 1;
        }

    }
        break;
    case ENCRYPT_ESAM_SET:
    {
        ret = taskmng_encrypt_esamset_send(pipe,TaskInfo,RunTask,pn,prtl);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_encrypt_timing_send error \n");
            return 1;
        }

    }
        break;
    case ENCRYPT_NORMAL_TASK:
    {
        ret = taskmng_encrypt_normaltask_send(pipe,TaskInfo,RunTask,pn,prtl);
        if(ret <0)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "taskmng_encrypt_normaltask_send error \n");
            return 1;
        }

    }
        break;
    case ENCRYPT_END:
    {
        //一块表抄完 记录下抄读状态 切换下一块
        taskmng_mtr_result_set(RunTask->MeterIndex, RunTask->MtrResult);
        RunTask->SucNum++;
        msindex = taskmng_ms_index_get(pipe,pn,RunTask->logicid);
        taskmng_mtr_addr_get(pn,0,addr);
        //失败的表不记录在文件里 重启时可以再抄一次
        if(RunTask->encryptFailCnt<3)
        {
            taskmng_task_monitor_update(pipe,RunTask->TaskId,RunTask->logicid,UDTASK_SUCCESSNUM,NULL);
            taskmng_task_record_file_set(pipe,msindex,RunTask->TaskId,RunTask->logicid);
            taskmng_encrypt_result_save(pipe,TaskInfo->EncryptPlanID,addr);
        }
        RunTask->MeterIndex++;
        RunTask->encryptStatus = ENCRYPT_INIT;
        RunTask->encryptFailCnt = 0;
        if(RunTask->MeterIndex>=TaskInfo->TotalNum)
        {
            TASK_FMT_TRACE( pipe, REC_LOG, "全部抄完 任务结束 \n");
            return 1;
        }
        return 2;
    }
        break;
    default:
        break;
    }

    return -1;

}

/*******************************************************************************
* 函数名称: taskmng_encrypt_init_recv
* 函数功能:初始化过程 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_init_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    uint16 meterindex = RunTask->MeterIndex;    //当前执行到的表
    uint16 pn = TaskInfo->MsInfo[meterindex].Sn;
    uint8 prtl;
    taskmng_mtr_prtl_get_new(pn,LOGIC_ID_YC,&prtl);
    int ret;
    uint8 databuf[2000]={0};
    uint16 datalen;
    uint16 index = 0;
    switch(prtl)
    {
        case PRTL_OOP:
        {
            //收到F1000701回码
             AppLayerList_t *pAppLayer = NULL;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_init_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            //从结果类型 开始
            datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
            if(databuf[0]!=1 && databuf[1]!=DT_DOUBLE_LONG_UNSIGNED)
                return -1;
            memcpy_r(&RunTask->encryptCounter,&databuf[2],4);
            TASK_FMT_DEBUG(pipe, REC_LOG,"获取到单地址应用协商计数器 %d \n",ret);
            RunTask->encryptStatus = ENCRYPT_CONNECT;   //下一步直接建立连接
        }
            break;
        case PRTL_20_IOT:
        {
            //收到f1000b00回码
            AppLayerList_t *pAppLayer = NULL;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_init_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            //从data optional 开始
            datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,datalen);

            if( databuf[1]!=04)
                return -1;
            index = 2;  //从表号开始
            if(databuf[index]!=9&&databuf[index+1]!=8)
                return -1;
            //获取到表号 下面的暂时不需要
            memset(RunTask->encryptMetNo,0x00,8);
            memcpy(RunTask->encryptMetNo,&databuf[index+2],8);
            RunTask->encryptStatus = ENCRYPT_COUNTER_GET;   //下一步获取计数器
        }
            break;
        case PRTL_07:
        {
            MeterPrtlFmt_T meterdata;
            memset(&meterdata,0x00,sizeof(MeterPrtlFmt_T));
            ret = taskmng_645_meter_data_unpack(len,buf,meterdata.MtrAddr,&meterdata.CtrlCode,&meterdata.DataLen,meterdata.DataBuff);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_init_recv 645 ret %d \n",ret);
            if(ret!=CHECK_RESULT_SUCCEED)
            {
                return -1;
            }
            //07表表号6个字节
            memcpy_r(RunTask->encryptMetNo+2,meterdata.DataBuff+4,6);
            TASK_FMT_DEBUG(pipe, REC_LOG,"645获取到表号 \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,RunTask->encryptMetNo,8);
            RunTask->encryptStatus = ENCRYPT_CONNECT;   //下一步身份认证
        }
            break;
        default:
            break;
    }
    return 0;
}
/*******************************************************************************
* 函数名称: taskmng_encrypt_counter_recv
* 函数功能:获取计数器 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_counter_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    int ret;
    uint8 databuf[2000]={0};
    uint16 datalen;

    switch(prtl)
    {

        case PRTL_20_IOT:
        {
            //收到F1001300 回码
            AppLayerList_t *pAppLayer = NULL;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_counter_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            //从oad后面的结果类型 开始
            datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,datalen);

            if(databuf[0]!=1 && databuf[1]!=DT_DOUBLE_LONG_UNSIGNED)
                return -1;

            memcpy_r(&RunTask->encryptCounter,&databuf[2],4);
            TASK_FMT_DEBUG(pipe, REC_LOG,"获取到计数器 %d \n",RunTask->encryptCounter);
            RunTask->encryptStatus = ENCRYPT_CONNECT;   //下一步建立应用连接
        }
            break;
        case PRTL_07:
        {

        }
            break;
        default:
            break;
    }
    return 0;

}

/*******************************************************************************
* 函数名称: taskmng_encrypt_connect_recv
* 函数功能:应用连接 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_connect_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    int ret;
    uint8 databuf[2000]={0};

    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT:
        {
            //收到应用连接回码
            AppLayerList_t *pAppLayer = NULL;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"建立应用连接 LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            if(prtl == PRTL_20_IOT)
                ret = security_sessonkey_get(RunTask->key.value,RunTask->key.nNum,pAppLayer->serverConnectInfo.rand,
                        pAppLayer->serverConnectInfo.randLen,pAppLayer->serverConnectInfo.sign,pAppLayer->serverConnectInfo.signLen,databuf);
            else
                ret = security_sessonkey_get_698(RunTask->key.value,RunTask->key.nNum,pAppLayer->serverConnectInfo.rand,
                        pAppLayer->serverConnectInfo.randLen,pAppLayer->serverConnectInfo.sign,pAppLayer->serverConnectInfo.signLen,databuf);
            if(ret==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"security_sessonkey_get error \n");
                return -1;
            }
            RunTask->sessionKey.nNum = ret;
            memcpy(RunTask->sessionKey.value,databuf,ret);
            TASK_FMT_DEBUG(pipe, REC_LOG,"获取到会话密钥包 \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,ret);
            TASK_FMT_DEBUG(pipe, REC_LOG,"方案类型 %d (0：普通任务数据；1：对时；2：设参；3：升级；)\n",TaskInfo->EncryptPlanType);
            if(TaskInfo->EncryptPlanType == 1)
                RunTask->encryptStatus = ENCRYPT_TIMING;
            else if(prtl ==PRTL_20_IOT && TaskInfo->EncryptPlanType == 2)
                RunTask->encryptStatus = ENCRYPT_ESAM_SET;
            else if(TaskInfo->EncryptPlanType == 0)
            {
                RunTask->encryptStatus = ENCRYPT_NORMAL_TASK;
            }else
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"暂不支持的方案类型 \n");
                RunTask->encryptStatus = ENCRYPT_END;
            }
        }
            break;
        case PRTL_07:
        {
            MeterPrtlFmt_T meterdata;
            memset(&meterdata,0x00,sizeof(MeterPrtlFmt_T));
            ret = taskmng_645_meter_data_unpack(len,buf,meterdata.MtrAddr,&meterdata.CtrlCode,&meterdata.DataLen,meterdata.DataBuff);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_connect_recv 645 ret %d \n",ret);
            if(ret!=CHECK_RESULT_SUCCEED)
            {
                return -1;
            }
            memcpy(RunTask->encryptR2,meterdata.DataBuff+4,4);
            TASK_FMT_DEBUG(pipe, REC_LOG,"获取到R2 \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,RunTask->encryptR2,4);
            if(TaskInfo->EncryptPlanType == 1)
                RunTask->encryptStatus = ENCRYPT_TIMING;
            else
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"645表暂不支持esam设置 \n");
                RunTask->encryptStatus = ENCRYPT_END;
            }
        }
            break;
        default:
            break;
    }
    return 0;


}

/*******************************************************************************
* 函数名称: taskmng_encrypt_timing_recv
* 函数功能:对时 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_timing_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    int ret;
    uint8 databuf[2000]={0};

    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT:
        {
            //收到对时报文安全响应回码
            AppLayerList_t *pAppLayer = NULL;
            uint8 secretdata[1000]={0};
            uint16 secretlen;
            uint8 mac[4]={0};
            uint16 index = 0;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_timing_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            index = 2;
            secretlen = pAppLayer->recvApduList->apdu[index++];
            memcpy(secretdata,pAppLayer->recvApduList->apdu+3,secretlen);
            index+=secretlen;
            if(pAppLayer->recvApduList->apdu[index++]==1 && pAppLayer->recvApduList->apdu[index++]==0 &&
                    pAppLayer->recvApduList->apdu[index++] == 4)
            {
                memcpy(mac,pAppLayer->recvApduList->apdu+index,4);
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码密文\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,secretdata,secretlen);
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码mac\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,mac,4);

            //发给esam解密
            if(prtl == PRTL_20_IOT)
                ret = security_decrypt_sec_mac(RunTask->sessionKey.value,RunTask->sessionKey.nNum,secretdata,secretlen,mac,databuf);
            else
                ret = security_decrypt_sec_mac_698(RunTask->sessionKey.value,RunTask->sessionKey.nNum,secretdata,secretlen,mac,databuf);
            if(ret==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"security_decrypt_sec_mac error \n");
                return -1;
            }

            TASK_FMT_DEBUG(pipe, REC_LOG,"解密后明文apdu \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,ret);
            OOP_OAD_U oad = {0};
            memcpy_r(&oad.value,databuf+3,4);
            if(oad.nObjID==0x4000 && databuf[7]==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 校时成功 \n",pn);
            }else
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 校时失败 \n",pn);
            }
            RunTask->encryptStatus = ENCRYPT_END;
        }
            break;
        case PRTL_07:
        {
            MeterPrtlFmt_T meterdata;
            memset(&meterdata,0x00,sizeof(MeterPrtlFmt_T));
            ret = taskmng_645_meter_data_unpack(len,buf,meterdata.MtrAddr,&meterdata.CtrlCode,&meterdata.DataLen,meterdata.DataBuff);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_timing_recv 645 ret %d \n",ret);
            if(ret!=CHECK_RESULT_CONFIRM)
            {
                return -1;
            }
            RunTask->encryptStatus = ENCRYPT_END;
        }
            break;
        default:
            break;
    }
    return 0;


}

/*******************************************************************************
* 函数名称: taskmng_encrypt_esamset_recv
* 函数功能:esam设置 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_esamset_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    int ret;
    uint8 databuf[2000]={0};

    switch(prtl)
    {
        case PRTL_OOP:
        {

        }
            break;
        case PRTL_20_IOT:
        {
            //收到对时报文安全响应回码
            AppLayerList_t *pAppLayer = NULL;
            uint8 secretdata[1000]={0};
            uint16 secretlen;
            uint8 mac[4]={0};
            uint16 index = 0;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_esamset_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            index = 2;
            secretlen = pAppLayer->recvApduList->apdu[index++];
            memcpy(secretdata,pAppLayer->recvApduList->apdu+3,secretlen);
            index+=secretlen;
            if(pAppLayer->recvApduList->apdu[index++]==1 && pAppLayer->recvApduList->apdu[index++]==0 &&
                    pAppLayer->recvApduList->apdu[index++] == 4)
            {
                memcpy(mac,pAppLayer->recvApduList->apdu+index,4);
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码密文\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,secretdata,secretlen);
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码mac\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,mac,4);

            //发给esam解密
            ret = security_decrypt_sec_mac(RunTask->sessionKey.value,RunTask->sessionKey.nNum,secretdata,secretlen,mac,databuf);
            if(ret==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"security_decrypt_sec_mac error \n");
                return -1;
            }

            TASK_FMT_DEBUG(pipe, REC_LOG,"解密后明文apdu \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,ret);
            OOP_OMD_U omd = {0};
            memcpy_r(&omd.value,databuf+3,4);
            if(omd.value==0xF1000900 && databuf[7]==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 设置esam参数成功 \n",pn);
            }else
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 设置esam参数失败 \n",pn);
            }
            RunTask->encryptStatus = ENCRYPT_END;
        }
            break;
        case PRTL_07:
        {

        }
            break;
        default:
            break;
    }
    return 0;


}


/*******************************************************************************
* 函数名称: taskmng_encrypt_recv
* 函数功能://转加密处理收到的报文
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值:
*******************************************************************************/
int taskmng_encrypt_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask)
{
    uint16 meterindex = RunTask->MeterIndex;    //当前执行到的表
    uint16 pn = TaskInfo->MsInfo[meterindex].Sn;
    uint8 prtl;
    taskmng_mtr_prtl_get_new(pn,LOGIC_ID_YC,&prtl);
   // uint8 lenoffset;
   // uint16 framelen = XADR_GetEleNum(buf, &lenoffset);
    taskmng_task_monitor_update(pipe,TaskInfo->TaskId,LOGIC_ID_YC,UDTASK_RECVFRMNUM,NULL);
    switch(RunTask->encryptStatus)
    {
        case ENCRYPT_INIT:
        {
            return taskmng_encrypt_init_recv(pipe,buf,len,TaskInfo,RunTask);
        }
            break;
        case ENCRYPT_COUNTER_GET:
        {
            return taskmng_encrypt_counter_recv(pipe,buf,len,TaskInfo,RunTask,pn,prtl);
        }
            break;
        case ENCRYPT_CONNECT:
        {
            return taskmng_encrypt_connect_recv(pipe,buf,len,TaskInfo,RunTask,pn,prtl);
        }
            break;
        case ENCRYPT_TIMING:
        {
            return taskmng_encrypt_timing_recv(pipe,buf,len,TaskInfo,RunTask,pn,prtl);
        }
            break;
        case ENCRYPT_NORMAL_TASK:
        {
            return taskmng_encrypt_normaltask_recv(pipe,buf,len,TaskInfo,RunTask,pn,prtl);
        }
            break;
        case ENCRYPT_ESAM_SET:
        {
            return taskmng_encrypt_esamset_recv(pipe,buf,len,TaskInfo,RunTask,pn,prtl);
        }
            break;
        default:
            break;

    }
    return 0;

}

/*******************************************************************************
* 函数名称: taskmng_encrypt_frame_send
* 函数功能://转加密任务报文发送 485和载波统一
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值:
*******************************************************************************/

void taskmng_encrypt_frame_send(uint8 pipe,uint16 pn,uint8 *buf,uint16 len,uint16 *MsgIndex)
{
    if(pipe == 0)
    {
        uint8 prtl;
        uint8 addr[6]={0};
        taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
        SendFrame_0038(pipe,prtl,addr,buf, len, MsgIndex);
    }else if(pipe >= PLC_NUM && pipe <AMR_THREAD_NUM)
    {
        //uint16 speed;
        OOP_METER_BASIC_T MeterBasicInfo;
        taskmng_mtr_basic_info_get(pn,LOGIC_ID_YC,&MeterBasicInfo);
        //speed = get_meter_read_speed(MeterBasicInfo.baud);
        //转加密多台终端一起跑的时候有的时候会过八秒多才回 所以转加密相关的超时时间放长
        rs485_RS232_send( pipe, 15,MAX_CHAR_OVERTIME,MeterBasicInfo.baud,PARITY_EVEN,buf, len);

    }

}
/*******************************************************************************
* 函数名称: taskmng_meter_search_report_check
* 函数功能:检查是否有搜表报文上报
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值:
*******************************************************************************/

void taskmng_meter_search_report_check(uint8 pipe)
{
    int recvlen;
    uint8 recvbuf[1000]={0};

    recvlen = taskmng_plc_queue_read(&gMeterSearchQueue,recvbuf);
    if(recvlen>0)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到搜表上报消息\n");
#ifdef AREA_ZHEJIANG
        taskmng_meter_search_result_write_area(pipe,recvbuf,recvlen);
#else
        taskmng_meter_search_result_write(pipe,recvbuf,recvlen);
#endif

    }
}
/*******************************************************************************
* 函数名称: taskmng_hplc_update_msg_amr
* 函数功能:收到模块升级消息 发给ccoRouter
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值:
*******************************************************************************/

void taskmng_hplc_update_msg_amr(uint8 pipe,uint8*buf,uint16 len)
{
    HPLC_UPDATE_T updateinfo;
    memcpy(&updateinfo,buf,len);
    uint16 msgindex;
    uint8 payload[200]={0};
    uint16 index = 0;
    OOP_OAD_U pipeOAD ;
    pipeOAD = taskmng_pipe_oad_get(pipe);
    memcpy_r(&payload[index],(uint8*)&pipeOAD.value,4);
    index+=4;
    payload[index++] = updateinfo.filepath.nNum;
    memcpy(&payload[index],updateinfo.filepath.value,updateinfo.filepath.nNum);
    index+=updateinfo.filepath.nNum;
    payload[index++] = updateinfo.fileflag;

    taskmng_msg_data_pack(pipe,0x0003,0x0063,index,payload,&msgindex);

}

/*******************************************************************************
* 函数名称: taskmng_encrypt_result_first_write
* 函数功能:转加密任务初始化后先写一条空的结果集
* 输入参数:pipe 端口号
        buf 收到的报文
        len 报文长度
* 输出参数:
* 返 回 值:
******************************************************************************/
int taskmng_encrypt_result_first_write(DB_CLIENT client,uint8 id,uint8 type,uint16 num,MS_INFO *pMsInfo)
{
    int ret;
    uint16 i;
    uint8 addr[8] = {0};
    OOP_OAD_U Oad = {0X60200200};
    OOP_ENCRYPTRES_T data;
    memset(&data,0x00,sizeof(data));
    data.planID = id;
    data.planType = type;
    data.nMetTotal = num;

    for(i=0;i<num;i++)
    {
        taskmng_mtr_addr_get(pMsInfo[i].Sn ,0,addr);
        memcpy_r(data.ms.addr[i].nValue,addr,6);
        TASK_FMT_DEBUG(-1,TASK_LOG,"%d 表地址 %02x%02x%02x%02x%02x%02x\n",i,addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
    }
    data.ms.nNum = num;
    NOMAL_OAD_T NomalOad;
    memset(&NomalOad,0,sizeof(NomalOad));
    NomalOad.oad =Oad;
    NomalOad.classtag = 5;
    NomalOad.infoNum = id;

    ret = db_write_nomal(client,&NomalOad,(uint8*)&data,sizeof(data));
    TASK_FMT_DEBUG(-1,TASK_LOG,"转加密方案 %d 写转加密执行结果集 ret %d num %d\n",id,ret,num);
    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_encrypt_result_save
* 函数功能:转加密任务写结果集
* 输入参数:pipe 端口号
=
* 输出参数:
* 返 回 值:
******************************************************************************/
int taskmng_encrypt_result_save(uint8 pipe,uint8 id,uint8 *addr)
{
    int ret;
    uint8 i,flag;
    OOP_OAD_U Oad = {0X60200200};
    uint8 tmp[6]={6};
    OOP_ENCRYPTRES_T data;
    memset(&data,0x00,sizeof(data));
    uint32 Len;
    uint8 DataBuf[MAX_MSG_LEN] = {0};
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    NOMAL_OAD_T NormalOad;
    memset(&NormalOad,0,sizeof(NormalOad));
    NormalOad.oad = Oad;
    NormalOad.infoNum = id;
    if(addr == NULL)
        return -1;
    memcpy_r(tmp,addr,6);
    ret = db_read_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,MAX_MSG_LEN,DataBuf,&Len);
    if(ret !=0 || Len != sizeof(OOP_ENCRYPTRES_T))
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "转加密方案 %d 读取结果集error ret %d Len %d\n",id,ret,Len);
        return ret;
    }
    memcpy(&data,DataBuf,sizeof(OOP_ENCRYPTRES_T));
    //把新抄的表加进去
    flag = 0;
    for(i=0;i<data.ms.nNum;i++)
    {
        if(memcmp(tmp,data.ms.addr[i].nValue,6 ) == 0)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 1)
    {
        TASK_FMT_DEBUG(pipe, REC_LOG, "转加密方案 %d 结果集中已有电表 %02x%02x%02x%02x%02x%02x\n",id,
            tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
        //再写一遍
        ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&data,sizeof(data));
        return 0;
    }
    memcpy(data.ms.addr[data.ms.nNum].nValue,tmp,6);
    data.ms.nNum++;
    TASK_FMT_DEBUG(pipe, REC_LOG, "转加密方案 %d 结果集中添加电表 %02x%02x%02x%02x%02x%02x 当前成功数 %d\n",id,
            tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5],data.ms.nNum);
    ret = db_write_nomal(pAmrThreadArg->DBClientHandle,&NormalOad,(uint8*)&data,sizeof(data));
    return ret;
}


/*******************************************************************************
* 函数名称: taskmng_file_transfer_start_pack
* 函数功能: 组织启动文件传输发送报文
* 输入参数:pipe 端口号

        size 长度
* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_start_pack(uint8 pipe,OOP_TSA_T tsa,OOPM_STARTTRANS_T *fileinfo,BOOL bEncode,uint8* outbuf)
{

    uint8 buf[MAX_MSG_LEN]={0};
    uint16 index = 0;
    OOP_OMD_U omd = {0xF0010E00};
    LinkLayer_t *pLinkLayer = NULL;
    uint16 len = 0;
    uint16 i;
    //组0701后面数据
    memcpy_r(buf,&omd,4);
    index+=4;
    buf[index++] = DT_STRUCTURE;
    buf[index++] = 6;
    buf[index++] = DT_STRUCTURE;
    buf[index++] = 6;
    buf[index++] = DT_VISIBLE_STRING;
    //源地址填空
    buf[index++] = 0;
    //memcpy(&buf[index],fileinfo->file.srceFile.value,fileinfo->file.srceFile.nNum);
    //index+=fileinfo->file.srceFile.nNum;
    buf[index++] = DT_VISIBLE_STRING;
    buf[index++] = fileinfo->file.destFile.nNum;
    memcpy(&buf[index],fileinfo->file.destFile.value,fileinfo->file.destFile.nNum);
    index+=fileinfo->file.destFile.nNum;
    buf[index++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&buf[index],&fileinfo->file.nSize,4);
    index+=4;
    buf[index++] = DT_BIT_STRING;
    buf[index++] = 3;
    buf[index++] = fileinfo->file.property.nValue;
    buf[index++] = DT_VISIBLE_STRING;
    buf[index++] = fileinfo->file.version.nNum;
    memcpy(&buf[index],fileinfo->file.version.value,fileinfo->file.version.nNum);
    index+=fileinfo->file.version.nNum;
    buf[index++] = DT_ENUM;
    buf[index++] = fileinfo->file.filetype;

    buf[index++] = DT_LONG_UNSIGNED;
    memcpy_r(&buf[index],&fileinfo->nBlockSize,2);
    index+=2;

    buf[index++] = DT_STRUCTURE;
    buf[index++] = 2;
    buf[index++] = DT_ENUM;
    buf[index++] = fileinfo->crc.type;
    buf[index++] = DT_OCTET_STRING;
    buf[index++] = fileinfo->crc.content.nNum;
    memcpy(&buf[index],fileinfo->crc.content.value,fileinfo->crc.content.nNum);
    index+=fileinfo->crc.content.nNum;

    buf[index++] = DT_ARRAY;
    buf[index++] = fileinfo->swlist.nNum;
    for(i=0;i<fileinfo->swlist.nNum;i++)
    {
        buf[index++] = DT_VISIBLE_STRING;
        buf[index++] = fileinfo->swlist.value[i].nNum;
        memcpy(&buf[index],fileinfo->swlist.value[i].value,fileinfo->swlist.value[i].nNum);
        index+=fileinfo->swlist.value[i].nNum;
    }

    buf[index++] = DT_ARRAY;
    buf[index++] = fileinfo->hwlist.nNum;
    for(i=0;i<fileinfo->hwlist.nNum;i++)
    {
        buf[index++] = DT_VISIBLE_STRING;
        buf[index++] = fileinfo->hwlist.value[i].nNum;
        memcpy(&buf[index],fileinfo->hwlist.value[i].value,fileinfo->hwlist.value[i].nNum);
        index+=fileinfo->hwlist.value[i].nNum;
    }

    buf[index++] = DT_VISIBLE_STRING;
    buf[index++] = fileinfo->identy.nNum;
    memcpy(&buf[index],fileinfo->identy.value,fileinfo->identy.nNum);
    index+=fileinfo->identy.nNum;

    TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动文件传输 明文 \n");
    TASK_BUF_DEBUG(pipe, RELAY_LOG,buf,index);

    if(tsa.type == 2 && bEncode == 1)
    {
        OopAction.requst(pipe,&tsa,4,buf,index);

    }else
    {
        OopAction.requst(pipe,&tsa,1,buf,index);
    }


    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        len = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,len);
    }
    return len;
}


/*******************************************************************************
* 函数名称: taskmng_file_encrypt_update_check
* 函数功能: 检查对应端口是否需要执行转加密升级
* 输入参数:pipe 端口号


* 输出参数:
* 返 回 值:
*******************************************************************************/

BOOL taskmng_file_encrypt_update_check(uint8 pipe)
{
    FILE_UPDATE_ENCRYPT_INFO_T* info;
    info = &gAmrThreadArg[pipe].UpdateEncryptInfo;

    uint16 i;
    uint8 flag = 0;
    if(gAmrThreadArg[pipe].UpdateInfo.bEncode == FALSE)
        return FALSE;
    for(i=0;i<gFileUpdateEncrypt.msnum;i++)
    {
        if(pipe == gFileUpdateEncrypt.msinfo[i].Pipe)
        {
            flag = 1;
            break;
        }
    }
    if(flag == 0)
        return FALSE;
    if(info->updateflag == 1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG,"启动文件传输已完成 \n");
        return FALSE;
    }
    return TRUE;
}


/*******************************************************************************
* 函数名称: taskmng_file_transfer_encrypt_init_pack
* 函数功能: 组织转加密文件升级初始化报文
* 输入参数:pipe 端口号

* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_encrypt_init_pack(uint8 pipe,uint16 pn,uint8* outbuf)
{
    //获取随机数
    uint8 rand1[100]={0};
    uint8 buf[100]={0};
    uint16 index =0;
    OOP_TSA_T tsa;
    int ret ;
    uint16 outlen=0;
    //获取8位随机数1
    ret = security_rand1_get(rand1);
    if(ret==0)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取随机数1失败 \n",pn);
        return 0;
    }
    //电表认证 F1000B00 终端和电能表的身份认证与原红外认证使用相同的协议
    tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
    buf[index++] = 0xF1;
    buf[index++] = 0x00;
    buf[index++] = 0x0B;
    buf[index++] = 0x00;
    buf[index++] = DT_RN;
    buf[index++] = ret;
    memcpy(&buf[index],rand1,ret);
    index+=ret;
    OopAction.requst(pipe,&tsa,1,buf,index);
    LinkLayer_t *pLinkLayer = NULL;
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        outlen = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,outlen);
    }
    return outlen;

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_encrypt_couter_pack
* 函数功能: 组织转加密文件升级获取计数器
* 输入参数:pipe 端口号

* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_encrypt_couter_pack(uint8 pipe,uint16 pn,uint8* outbuf)
{
    LinkLayer_t *pLinkLayer = NULL;
    uint16 outlen=0;
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
    OOP_OAD_U oad = {0xF1001300};
    OopGet.requst(pipe,&tsa,1,(uint8*)&oad.value,4);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        outlen = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,outlen);

    }
    return outlen;
}
/*******************************************************************************
* 函数名称: taskmng_file_transfer_encrypt_connect_pack
* 函数功能: 组织转加密文件升级建立应用连接
* 输入参数:pipe 端口号

* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_encrypt_connect_pack(uint8 pipe,uint16 msindex,uint8* outbuf)
{
    FILE_UPDATE_ENCRYPT_INFO_T* info;
    info = &gAmrThreadArg[pipe].UpdateEncryptInfo;

    int ret;
    uint16 outlen=0;
    uint16 pn = gFileUpdateEncrypt.msinfo[msindex].Sn;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    //从esam获取密钥包 将任务里面的电表密钥包发给esam
    uint8 sessiondata1[32]={0};
    uint8 mac1[4]={0};
    OOP_OCTETVAR256_T *key = &gFileUpdateEncrypt.msinfo[msindex].key;
    ret = TESAM_RcvSndDataAndCheck(key->value,key->nNum, recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取电表密钥包失败 \n",pn);
        return -1;
    }
    info->key.nNum = recvlen;
    memcpy(info->key.value,recvbuf,recvlen);
    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取电表密钥包 \n",pn);
    TASK_BUF_TRACE(pipe, RELAY_LOG,recvbuf,recvlen);
    //发送esam获取SessionData1+MAC1
    memset(recvbuf,0x00,sizeof(recvbuf));
    ret = security_sessondata1_get(info->key.value,info->key.nNum,info->encryptCounter,recvbuf);
    if(ret!=36)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d security_sessondata1_get失败 ret %d \n",pn,ret);
        return -1;
    }
    memcpy(sessiondata1,recvbuf,32);
    memcpy(mac1,recvbuf+32,4);
    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取sessiondata1 \n",pn);
    TASK_BUF_TRACE(pipe, RELAY_LOG,sessiondata1,32);
    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d MAC1 \n",pn);
    TASK_BUF_TRACE(pipe, RELAY_LOG,mac1,4);

    //和电表建立应用连接
    LinkLayer_t *pLinkLayer = NULL;
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);

    OopConnect.requst(pipe,&tsa,2,1800,sessiondata1,mac1);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        outlen = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,outlen);

    }

    return outlen;

}

/*******************************************************************************
* 函数名称: taskmng_file_transfer_encrypt_update_pack
* 函数功能: 组织转加密文件升级启动升级
* 输入参数:pipe 端口号

* 输出参数: outbuf 组织的698报文
* 返 回 值: 报文长度
*******************************************************************************/

uint16 taskmng_file_transfer_encrypt_update_pack(uint8 pipe,uint16 msindex,uint8* outbuf)
{
    FILE_UPDATE_ENCRYPT_INFO_T* info;
    info = &gAmrThreadArg[pipe].UpdateEncryptInfo;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    uint8 taskdata[1000]={0};
    uint16 tasklen;
   // uint8 paramdata[1000]={0};
   // uint16 paramlen;
  //  uint8 endata2[1000]={0};
   // uint16 endata2len;
    int ret;
    uint16 outlen=0;
    uint16 pn = gFileUpdateEncrypt.msinfo[msindex].Sn;

    ret = TESAM_RcvSndDataAndCheck(gFileUpdateEncrypt.planinfo.endata.apdu.value ,gFileUpdateEncrypt.planinfo.endata.apdu.nNum, recvbuf,&recvlen);
    if(ret!=ERR_OK)
    {
        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取任务密文数据 失败 \n",pn);
        return -1;
    }
    tasklen = recvlen;
    memcpy(taskdata,recvbuf,recvlen);
    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取任务密文数据 \n",pn);
    TASK_BUF_TRACE(pipe, RELAY_LOG,recvbuf,recvlen);
//    //将任务中的参数发给esam
//    ret = TESAM_RcvSndDataAndCheck(gFileUpdateEncrypt.planinfo.endata.param.value ,gFileUpdateEncrypt.planinfo.endata.param.nNum, recvbuf,&recvlen);
//    if(ret!=ERR_OK)
//    {
//        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取参数密文数据 失败 \n",pn);
//        return -1;
//    }
    //发送esam 计算密文数据
    uint8 databuf[2000]={0};
    ret = security_normal_taskdata_get(info->sessionKey.value,info->sessionKey.nNum,recvbuf,recvlen,databuf);
    //6SID+ENDATA+4MAC
    if(ret==0)
    {
        TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_normal_taskdata_get ret %d \n",pn,ret);
        return -1;
    }

//    paramlen = recvlen;
//    memcpy(paramdata,recvbuf,recvlen);
//    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取参数密文数据 \n",pn);
//    TASK_BUF_TRACE(pipe, RELAY_LOG,recvbuf,recvlen);
//    //获取endata2
//    ret = security_endata2_get(info->sessionKey.value ,info->sessionKey.nNum,paramdata,paramlen, recvbuf);
//    if(ret == 0)
//    {
//        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取endata2 失败 \n",pn);
//        return -1;
//    }
//    endata2len = ret;
//    memcpy(endata2,recvbuf,ret);
//    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取endata2 \n",pn);
//    TASK_BUF_TRACE(pipe, RELAY_LOG,endata2,endata2len);
//    //获取esam设置 任务密文
//    ret = security_esamset_data_get(info->sessionKey.value ,info->sessionKey.nNum,endata2,endata2len,taskdata,tasklen, recvbuf);
//    if(ret == 0)
//    {
//        TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取启动升级密文数据 失败 \n",pn);
//        return -1;
//    }
    TASK_FMT_TRACE( pipe, RELAY_LOG, "电表%d 获取启动升级设置密文数据  \n",pn);
    TASK_BUF_TRACE(pipe, RELAY_LOG,databuf,ret);

    tasklen = ret-6-4;

    //发送密文至电表
    LinkLayer_t *pLinkLayer = NULL;
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
    //此处append_data 要给esam回的前8个字节 包括两个字节长度 还有后面的6字节sid
    uint8 appenddata[100] = {0};
    appenddata[0] = 0;
    appenddata[1] = ret;
    memcpy(appenddata+2,databuf,6);

    security_request_encrypt(pipe,&tsa,databuf+6,tasklen,databuf,appenddata,databuf+6+tasklen);
    //security_request_encrypt(pipe,&tsa,recvbuf+6,endata3len,recvbuf,appenddata,recvbuf+6+endata3len);
    pLinkLayer = FindLinkLayer(pipe, NULL);
    if(pLinkLayer!=NULL)
    {
        outlen = pLinkLayer->sendLen;
        memcpy(outbuf,pLinkLayer->sendBuff,outlen);
    }
    return outlen;

}

/*******************************************************************************
* 函数名称: taskmng_encrypt_normaltask_send
* 函数功能:普通数据任务
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:
*******************************************************************************/

int taskmng_encrypt_normaltask_send(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    uint8 sendbuf[2000] = {0};
    uint16 sendlen;
    //uint8 addr[6]={0};
    int ret;
    uint8 recvbuf[1000]={0};
    uint16 recvlen;
    TASK_FMT_TRACE( pipe, REC_LOG, "电表普通任务数据 pn %d \n",pn);
    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT:
        {
            //将任务中的报文发给esam

            ret = TESAM_RcvSndDataAndCheck(TaskInfo->EncryptInfo.apdu.value ,TaskInfo->EncryptInfo.apdu.nNum, recvbuf,&recvlen);
            if(ret!=ERR_OK)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 失败 \n",pn);
                return -1;
            }
            TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 获取任务密文数据 \n",pn);
            TASK_BUF_TRACE(pipe, REC_LOG,recvbuf,recvlen);

            //发送esam 计算密文数据
            uint8 databuf[2000]={0};
            ret = security_normal_taskdata_get(RunTask->sessionKey.value,RunTask->sessionKey.nNum,recvbuf,recvlen,databuf);
            //6SID+ENDATA+4MAC
            if(ret==0)
            {
                TASK_FMT_TRACE( pipe, REC_LOG, "电表%d security_normal_taskdata_get ret %d \n",pn,ret);
                return -1;
            }

            //发送密文至电表
            LinkLayer_t *pLinkLayer = NULL;
            OOP_TSA_T tsa;
            memset(&tsa,0x00,sizeof(OOP_TSA_T));
            tsa = taskmng_tsa_get(pn,LOGIC_ID_YC);
            //此处append_data 要给esam回的前8个字节 包括两个字节长度 还有后面的6字节sid
            uint8 appenddata[100] = {0};
            uint8 tasklen;

            tasklen = ret -10;
            if(prtl == PRTL_20_IOT)
            {
                appenddata[0] = 0;
                appenddata[1] = ret;
                memcpy(appenddata+2,databuf,6);
                security_request_encrypt(pipe,&tsa,databuf+6,tasklen,databuf,appenddata,databuf+6+tasklen);
            }
            else
            {
                //698表把后面的四个字节也加上
                tasklen +=4;
                memcpy(appenddata,databuf+4,2);
                security_request_encrypt_698(pipe,&tsa,databuf+6,tasklen,databuf,appenddata);
            }
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                sendlen = pLinkLayer->sendLen;
                memcpy(sendbuf,pLinkLayer->sendBuff,sendlen);
            }
            break;
        }
        default:
            break;
    }
    //taskmng_mtr_addr_prtl_get( pn,LOGIC_ID_YC, addr, &prtl);
    //SendFrame_0038(pipe,prtl,addr,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    taskmng_task_monitor_update(pipe,RunTask->TaskId,LOGIC_ID_YC,UDTASK_SENDFRMNUM,NULL);
    taskmng_encrypt_frame_send(pipe,pn,sendbuf,sendlen,&RunTask->encryptMsgIndex);
    TASK_FMT_TRACE( pipe, REC_LOG, "电表%d 发送 MsgIndex %d \n",pn,RunTask->encryptMsgIndex);
    TASK_BUF_TRACE( pipe, REC_LOG,sendbuf,sendlen);
    return 0;
}


/*******************************************************************************
* 函数名称: taskmng_encrypt_normaltask_recv
* 函数功能: 收到电表回码
* 输入参数:pipe 端口号
        TaskInfo 任务参数
        RunTask 任务运行时参数
* 输出参数:
* 返 回 值:0
*******************************************************************************/

int taskmng_encrypt_normaltask_recv(uint8 pipe,uint8* buf,uint16 len,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask,uint16 pn,uint8 prtl)
{
    int ret;
    uint8 databuf[2000]={0};

    switch(prtl)
    {
        case PRTL_OOP:
        case PRTL_20_IOT:
        {
            //收到对时报文安全响应回码
            AppLayerList_t *pAppLayer = NULL;
            uint8 secretdata[1000]={0};
            uint16 secretlen;
            uint8 mac[4]={0};
            uint16 index = 0;
            ret = LinkDataReceive(pipe,buf,len,NULL);
            TASK_FMT_DEBUG(pipe, REC_LOG,"taskmng_encrypt_normaltask_recv LinkDataReceive ret %d \n",ret);
            if(ret>=eOopComm_Fail)
            {
                return -1;
            }
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
                return -1;
            index = 2;
            secretlen = pAppLayer->recvApduList->apdu[index++];
            memcpy(secretdata,pAppLayer->recvApduList->apdu+3,secretlen);
            index+=secretlen;
            if(pAppLayer->recvApduList->apdu[index++]==1 && pAppLayer->recvApduList->apdu[index++]==0 &&
                    pAppLayer->recvApduList->apdu[index++] == 4)
            {
                memcpy(mac,pAppLayer->recvApduList->apdu+index,4);
            }
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码密文\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,secretdata,secretlen);
            TASK_FMT_DEBUG(pipe, REC_LOG,"收到的回码mac\n");
            TASK_BUF_DEBUG(pipe, REC_LOG,mac,4);

            //发给esam解密
            if(prtl == PRTL_20_IOT)
                ret = security_decrypt_sec_mac(RunTask->sessionKey.value,RunTask->sessionKey.nNum,secretdata,secretlen,mac,databuf);
            else
                ret = security_decrypt_sec_mac_698(RunTask->sessionKey.value,RunTask->sessionKey.nNum,secretdata,secretlen,mac,databuf);
            if(ret==0)
            {
                TASK_FMT_DEBUG(pipe, REC_LOG,"security_decrypt_sec_mac error \n");
                return -1;
            }

            TASK_FMT_DEBUG(pipe, REC_LOG,"解密后明文apdu \n");
            TASK_BUF_DEBUG(pipe, REC_LOG,databuf,ret);
//            OOP_OAD_U oad = {0};
//            memcpy_r(&oad.value,databuf+3,4);
//            if(oad.nObjID==0x4000 && databuf[7]==0)
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 校时成功 \n",pn);
//            }else
//            {
//                TASK_FMT_DEBUG(pipe, REC_LOG,"电表 %d 校时失败 \n",pn);
//            }
            RunTask->encryptStatus = ENCRYPT_END;
        }
            break;

        default:
            break;
    }
    return 0;

}


void taskmng_event_report_clear(uint8 pipe)
{
    EVENT_REPORT_T* eventreport;
    eventreport = &gAmrThreadArg[pipe].EventReport;
    memset(eventreport,0x00,sizeof(EVENT_REPORT_T));

}
/*******************************************************************************
* 函数名称: taskmng_down_recv_buff_clear
* 函数功能: 清除下行接受队列
* 输入参数: 无
* 输出参数: 无
* 返 回 值:  无
*******************************************************************************/
void taskmng_down_recv_buff_clear(uint8 CommPipe)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[MAX_MSG_LEN];        //接收数据缓冲区

    while (RecvLen > 0)
    {
        RecvLen = taskmng_plc_queue_read(pAmrRecvDown[CommPipe], RecvBuf);
    }
}




uint16 g_timeErrorNum;//记录下时钟超差的表个数

void taskmng_time_error_num_init()
{
    g_timeErrorNum = 0;
}

void taskmng_time_error_num_add()
{
    g_timeErrorNum++;
}

uint16 taskmng_time_error_num_get()
{
    return g_timeErrorNum;
}
//任务到新一轮的时候检查 如果是实时任务 且会抄时钟就初始化
void taskmng_time_error_num_init_check(uint8 pipe,SCHEME_TASK_RUN_INFO *taskinfo)
{
    uint16 i;
    if(taskinfo->SchemeType == NORMALPLAN && taskinfo->CsdTask.RoadTask[0].MainOad.value == 0)
    {
        for(i=0;i<taskinfo->CsdTask.RoadTask[0].SubOadNum;i++)
        {
            if(taskinfo->CsdTask.RoadTask[0].OadListInfo[i].value == 0x40000200)
            {
                TASK_FMT_DEBUG( pipe, REC_LOG, "taskmng_time_error_num_init\n");
                taskmng_time_error_num_init();
                break;
            }
        }
    }

}
//如果任务100只有一个表抄不通且时钟超差是69个 则把这最后一块表也写个结果 表失败时调用
void taskmng_time_error_result_check(uint8 pipe,uint16 pn,CTS_RUN_TASK_T *runTask)
{
    WRITE_RECORDS_T record;
    memset(&record,0,sizeof(WRITE_RECORDS_T));
    OOP_DATETIME_S_T datetimes;
    memset(&datetimes,0,sizeof(OOP_DATETIME_S_T));
    time_t tmptime = 0;
    DateTimeBcd_t  timebcd;
    memset(&timebcd,0,sizeof(DateTimeBcd_t));
    uint8 addr[6] = {0};
    int ret;
    uint32 storeno;
    if(runTask->TaskId== 100 &&runTask->SucNum==runTask->NeedMtrNum-1 && taskmng_time_error_num_get()== 69)
    {
        TASK_FMT_DEBUG( pipe, REC_LOG, "pn %d 写时钟超差\n",pn);
        tmptime = time(NULL) - 500;
        DT_Time2DateTimeShort(tmptime,&datetimes);
        record.recordOAD.infoNum = pn;
        record.recordOAD.road.oadCols.nNum = 1;
        record.recordOAD.road.oadCols.oad[0].value = 0x40000200;
        record.recordOAD.classtag = 5;
        memcpy_r((uint8*)&timebcd,(uint8*)runTask->ThisTurnBeginTime,7);
        DT_DateTimeBCD2Time(&timebcd,&tmptime);     //开始时间
        record.colStartTm = tmptime;
        record.colStoreTm = tmptime;
        record.colEndTm = time(NULL);
        taskmng_mtr_addr_get( pn, 0,addr);
        record.MAC.nNum = 6;
        memcpy_r(record.MAC.value,addr,6);
        record.datas[0].len = sizeof(OOP_DATETIME_S_T);
        memcpy(record.datas[0].data,&datetimes,sizeof(OOP_DATETIME_S_T));
        ret = db_write_records(gAmrThreadArg[pipe].DBClientHandle,&record,&storeno);
        if(ret!=0)
        {
            TASK_FMT_TRACE ( pipe,REC_LOG, "db_write_records error ret  %d\n",ret);
        }
        taskmng_time_error_num_add();
    }

}

#if DESC("电表停上电新逻辑",1)

/*扩充的私有存数据中心的oad 存储每个电表的停上电状态 上电的时候加载一下 以防终端停电后收到电表上电事件匹配不到停电事件  */
const OOP_OAD_U meterPowerOad = {0x3011FF00};
METER_POWER_CACHE_T gMeterPowerCache;

METER_POWER_BUF_LIST_T gMeterPowerOffBufList;   //放停电事件的队列
METER_POWER_BUF_LIST_T gMeterPowerOnBufList;    //放上电事件的队列

uint8 gMeterPowerOnReportNum;   //上报的上电事件表个数
uint8 gMeterPowerOffReportNum;   //上报的停电事件表个数

/**********************************************************
* 函 数 名 : taskmng_meter_power_cache_init
* 函数功能 : 初始化函数
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
*
* 修改历史 :  2024/1/11
************************************************************/
void taskmng_meter_power_cache_init()
{
    memset(&gMeterPowerCache,0,sizeof(gMeterPowerCache));
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_buflist_init
* 函数功能 : 初始化函数
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 无
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_buflist_init()
{
    memset(&gMeterPowerOffBufList,0,sizeof(gMeterPowerOffBufList));
    memset(&gMeterPowerOnBufList,0,sizeof(gMeterPowerOnBufList));
    gMeterPowerOnReportNum = 0;
    gMeterPowerOffReportNum = 0;
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_report_num_clear
* 函数功能 : 清空电表事件的上报次数
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_report_num_clear(TMN_POWER_STATE_E powerstate)
{
    if(powerstate == POWER_OFF)
    {
        gMeterPowerOffReportNum = 0;
    }else
    {
        gMeterPowerOnReportNum = 0;
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_buf_add
* 函数功能 : 往电表缓存中添加内容
* 输入参数 : addr 地址
            powerstate 每个表的带电状态
            buf 缓存
            buflen 缓存长度
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/6/1
************************************************************/
int taskmng_meter_power_buf_add(uint8 pipe,uint8 *addr,TMN_POWER_STATE_E powerstate,uint8 *buf,uint16 buflen)
{
    if(NULL == addr || NULL == buf)
    {
        return -1;
    }
    /*根据停电状态放不同的队列  */
    METER_POWER_BUF_LIST_T *buflist = NULL;
    if(powerstate == POWER_OFF)
    {
        buflist = &gMeterPowerOffBufList;
    }else
    {
        buflist = &gMeterPowerOnBufList;
    }
    if(buflist->meterNum > MAX_METER_POWER_BUF_NUM)
    {
        TASK_FMT_TRACE ( pipe,RELAY_LOG, "缓存的电表停上电事件过多 %d\n",buflist->meterNum);
        return -1;
    }
    if(buflen > 256)
    {
        TASK_FMT_TRACE ( pipe,RELAY_LOG, "缓存的报文长度异常 %d\n",buflen);
        return -1;
    }
    struct timespec timenow;
    METER_POWER_BUF_T  *tmp = &buflist->powerBufList[buflist->meterNum];
    memcpy(tmp->addr,addr,6);
    tmp->powerState = powerstate;
    memcpy(tmp->databuf,buf,buflen);
    tmp->buflen = buflen;
    buflist->meterNum++;
    /*第一次收到停电事件后记录下时间  */
    if(powerstate == POWER_OFF && buflist->firstTime == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        buflist->firstTime = timenow.tv_sec;
    }
    TASK_FMT_TRACE ( pipe,RELAY_LOG, "meternum  %d\n",buflist->meterNum);
    return 0;
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_special_deal
* 函数功能 : 特殊处理，适配打高分一次性上报20个表的停电事件
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 无
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_special_deal(uint8 pipe)
{
    /*不是20个不处理  */
    if( gMeterPowerOffBufList.meterNum != 20)
    {
        return;
    }
    uint16 i;
    uint8 databuf[2048]={0};
    uint16 offset = 0;
    struct timespec timenow;
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "special_deal count %d\n",gMeterPowerCache.count);
    /* 第一个字节是个数 后面是每个表的报文 */
    databuf[offset++] = gMeterPowerOffBufList.meterNum;
    for(i=0;i<gMeterPowerOffBufList.meterNum;i++)
    {
        if(offset + gMeterPowerOffBufList.powerBufList[i].buflen > sizeof(databuf))
        {
            break;
        }
        memcpy(&databuf[offset],gMeterPowerOffBufList.powerBufList[i].databuf,gMeterPowerOffBufList.powerBufList[i].buflen);
        offset+=gMeterPowerOffBufList.powerBufList[i].buflen;
    }

    memcpy(gMeterPowerCache.bufList[gMeterPowerCache.count],databuf,offset);
    gMeterPowerCache.buflenList[gMeterPowerCache.count] = offset;
    gMeterPowerCache.count++;
    clock_gettime(CLOCK_MONOTONIC, &timenow);
    gMeterPowerCache.lastReportTime = timenow.tv_sec;

    /* 攒到100个就写数据中心 */
    if(gMeterPowerCache.count >= 5)
    {
        for(i=0;i<5;i++)
        {
            taskmng_meter_power_write(pipe,gMeterPowerCache.bufList[i],gMeterPowerCache.buflenList[i]);
        }
        memset(&gMeterPowerCache,0,sizeof(gMeterPowerCache));
    }

    memset(&gMeterPowerOffBufList,0,sizeof(gMeterPowerOffBufList));
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_off_report
* 函数功能 : 电表停电事件的上报处理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_off_report(uint8 pipe,TMN_POWER_STATE_T *tmnState)
{
    /*停电事件上报处理  */
    if(gMeterPowerOffBufList.meterNum == 0)
    {
        return ;
    }

    uint8 databuf[2048] = {0};
    uint16 index = 1;   //buf填的时候从第二个字节开始 第一个字节表个数
    uint16 i;
    struct timespec timenow;
    uint8 reportflag = 0; //上报标记 如果是1的情况下就上报所有 0的情况下要判总共只能上报5条
    /*终端上电状态下收到电表的停电事件要先缓存20秒 超过20秒后全部上报 */
    if(tmnState->powerState == POWER_ON )
    {
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        if(timenow.tv_sec - gMeterPowerOffBufList.firstTime < 20)
        {
            return;
        }
        reportflag = 1;
    }
    /*终端已经是停电状态 就直接上报 最多报5个表 */
    for(i = 0;i<gMeterPowerOffBufList.meterNum;i++)
    {
        if(reportflag == 0 && gMeterPowerOffReportNum >=5)
        {
            break;
        }
        //240701 上报那块最长的报文长度限制了2048 所以这块写的时候改小一点 变成1900
        //if(index + gMeterPowerOffBufList.powerBufList[i].buflen > sizeof(databuf))
        if(index + gMeterPowerOffBufList.powerBufList[i].buflen > 1900)    
        {
            /* 如果报文超长了 就先上报一部分*/
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "停电事件上报 count %d\n",databuf[0]);
            taskmng_meter_power_write(pipe,databuf,index);
            memset(databuf,0,sizeof(databuf));
            index  = 1;
        }
        memcpy(&databuf[index],gMeterPowerOffBufList.powerBufList[i].databuf,gMeterPowerOffBufList.powerBufList[i].buflen);
        index+=gMeterPowerOffBufList.powerBufList[i].buflen;
        databuf[0]++;
        if(reportflag == 0)
        {
            gMeterPowerOffReportNum++;
        }
    }
    if(index > 1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停电事件上报 count %d\n",databuf[0]);
        taskmng_meter_power_write(pipe,databuf,index);
    }
    memset(&gMeterPowerOffBufList,0,sizeof(gMeterPowerOffBufList));
}
/**********************************************************
* 函 数 名 : taskmng_meter_power_off_report
* 函数功能 : 电表上电事件的上报处理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_on_report(uint8 pipe)
{
    if(gMeterPowerOnBufList.meterNum == 0)
    {
        return;
    }
    uint16 i ;
    uint8 databuf[2048] = {0};
    uint16 index = 1;   //buf填的时候从第二个字节开始 第一个字节表个数
    for(i=0;i<gMeterPowerOnBufList.meterNum ;i++)
    {
        /*240606 上电事件不过滤了  */
/*         if(gMeterPowerOnReportNum >= 5)
        {
            break;
        } */
        if(index + gMeterPowerOnBufList.powerBufList[i].buflen > 1900)
        {
            /* 如果报文超长了 就先上报一部分*/
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电事件上报 count %d\n",databuf[0]);
            taskmng_meter_power_write(pipe,databuf,index);
            memset(databuf,0,sizeof(databuf));
            index  = 1;
        }
        memcpy(&databuf[index],gMeterPowerOnBufList.powerBufList[i].databuf,gMeterPowerOnBufList.powerBufList[i].buflen);
        index+=gMeterPowerOnBufList.powerBufList[i].buflen;
        databuf[0]++;
        /* gMeterPowerOnReportNum++; */

    }
    if(index > 1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电事件上报 count %d\n",databuf[0]);
        taskmng_meter_power_write(pipe,databuf,index);
    }
    memset(&gMeterPowerOnBufList,0,sizeof(gMeterPowerOnBufList));
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_report_check
* 函数功能 : 电表停上电事件的上报处理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/1
************************************************************/
void taskmng_meter_power_report_check(uint8 pipe)
{

    if(gPastTimeFlag[pipe].BitSect.Second != TRUE)
    {
        return;
    }
    TMN_POWER_STATE_T tmnState;
    memset(&tmnState,0,sizeof(TMN_POWER_STATE_T));
    /*获取一下当前的终端带电状态  */
    taskmng_power_state_get(&tmnState);

    /*如果是电表的停电事件 则需要判断下终端是否停电  */
    taskmng_meter_power_off_report(pipe,&tmnState);
    taskmng_meter_power_on_report(pipe);
}
/**********************************************************
* 函 数 名 : taskmng_meter_power_cache_check
* 函数功能 : 过秒检查是否过9秒没上报电表停上电事件
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/1/11
************************************************************/
void taskmng_meter_power_cache_check(uint8 pipe)
{
    struct timespec timenow;
    uint8 i;
    if(gPastTimeFlag[pipe].BitSect.Second != TRUE)
        return;
    clock_gettime(CLOCK_MONOTONIC, &timenow);
    if(gMeterPowerCache.count > 0 && gMeterPowerCache.lastReportTime > 0 &&timenow.tv_sec - gMeterPowerCache.lastReportTime > 9)
    {
         TASK_FMT_TRACE ( pipe,RELAY_LOG, "上报缓存的停上电事件\n");
         for(i = 0;i< gMeterPowerCache.count && i<5;i++)
         {
            taskmng_meter_power_write(pipe,gMeterPowerCache.bufList[i],gMeterPowerCache.buflenList[i]);
         }
         taskmng_meter_power_cache_init();
    }
}

/*******************************************************************************
* 函数名称: taskmng_meter_power_deal
* 函数功能://存储停上电事件 240601修改函数 组出一个表的报文 别的过滤逻辑放外面做
* 输入参数:
* 输出参数:
* 返 回 值:0 表示事件被过滤了
            否则返回组出来的报文长度
*******************************************************************************/
uint16 taskmng_meter_power_deal(uint8 pipe,uint8* Addr,uint8 EventType,uint8 *buf,uint16 maxlen)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get(pipe);
    int msindex = -1;
    time_t powerofftime;
    time_t timenow = time(NULL);
    uint32 Eventseq = 0;
    uint8 databuf[2000]={0};//存组的每个表的数据
    uint16 index = 0;
    int ret;
    METER_POWER_STATE_T powerState;
    memset(&powerState,0,sizeof(METER_POWER_STATE_T));

    databuf[index++] = DT_TSA;
    databuf[index++] = 7;
    databuf[index++] = 5;
    memcpy_r(&databuf[index],Addr,6);
    index+=6;
    msindex = GetMsindexFromAddr(pipe,Addr);
    if(msindex==-1)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表不在档案中 不进行存储  \n");
        return 0;
    }
    databuf[index++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&databuf[index]);
    index+=7;
    databuf[index++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&databuf[index]);
    index+=7;
    databuf[index++] = DT_DATETIME_S;
    DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&databuf[index]);
    index+=7;

    databuf[index++] = 1;
    databuf[index++] = 5;
    if(EventType==1)
    {
        databuf[index++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&databuf[index]);
        index+=7;
        //将停电时间写入内存
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate == POWER_OFF)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表重复上报停电事件 直接过滤 \n");
            return 0;
        }
        pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime = timenow;

        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = POWER_OFF;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "停电事件序号 %d \n",Eventseq);
        /*将电表的停电事件写入数据中心 以防终端彻底停电后查不到对应的停电记录  */
        powerState.pn  = pAmrThreadArg->pMtrAmrInfo[msindex].Pn;
        memcpy(powerState.addr,Addr,6);
        powerState.powerState = POWER_OFF;
        powerState.powerOffTime = timenow;
        powerState.eventSeq = Eventseq;
        taskmmng_meter_power_event_write(pipe,pAmrThreadArg->DBClientHandle,&powerState);
        //停电 事件结束时间为NULL
        databuf[index++] = DT_NULL;
    }else
    {
            /* 报上电事件的记录下来 */
        //240629测试的时候直接上报上电事件 没有对应的停电事件 就会被过滤 所以把校时逻辑放前面来    
        uint8 prtl;
        taskmng_prtl_get_from_addr(Addr,0,&prtl);
        taskmng_meter_time_add(pipe,pAmrThreadArg->pMtrAmrInfo[msindex].Pn,prtl,Addr);
        //只有停过电才报上电事件
        if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate!= POWER_OFF )
        {
            /*240601 当判断出表的状态不是停电的时候 再从数据中心读一遍 首次没有停电时间的读一下 有停电时间的不需要从数据中心加载 */
            if(pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime == 0)
            {
                powerState.pn  = pAmrThreadArg->pMtrAmrInfo[msindex].Pn;
                memcpy(powerState.addr,Addr,6);
                ret = taskmng_meter_power_event_read(pipe,pAmrThreadArg->DBClientHandle,&powerState);
                if(ret == 0 && pAmrThreadArg->pMtrAmrInfo[msindex].powerstate == POWER_OFF)
                {
                    pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = powerState.powerState;
                    pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime = powerState.powerOffTime;
                    pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq = powerState.eventSeq;
                }
                /*再判一次状态  */
                //240711 测试用例里面都是直接上报复电事件 所以暂时先不判
//                if(pAmrThreadArg->pMtrAmrInfo[msindex].powerstate != POWER_OFF)
//                {
//                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表没有停电事件 不上报  \n");
//                    return 0;
//                }
            }else
            {
//                TASK_FMT_DEBUG(pipe, RELAY_LOG, "该电表没有停电事件 不上报  \n");
//                return 0;
            }
        }
        powerofftime = pAmrThreadArg->pMtrAmrInfo[msindex].powerofftime;
        Eventseq = pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq;
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电事件序号 %d \n",Eventseq);
        pAmrThreadArg->pMtrAmrInfo[msindex].powerstate = POWER_ON;
        pAmrThreadArg->pMtrAmrInfo[msindex].powereventseq++;
       /*将电表的上电事件写入数据中心 以防终端彻底停电后查不到对应的停电记录  */
        powerState.pn  = pAmrThreadArg->pMtrAmrInfo[msindex].Pn;
        memcpy(powerState.addr,Addr,6);
        powerState.powerState = POWER_ON;
        powerState.powerOffTime = powerofftime;
        powerState.eventSeq = Eventseq;
        taskmmng_meter_power_event_write(pipe,pAmrThreadArg->DBClientHandle,&powerState);

        //电表上电事件的“发生时间”应为电表的停电时间，如果终端没有收到该电表的停电事件，则“发生时间”应为空
        if(powerofftime==0)
        {
            databuf[index++] = DT_NULL;
        }else
        {

            databuf[index++] = DT_DATETIME_S;
            DT_Time2DateTimeHex(powerofftime,(DateTimeHex_t *)&databuf[index]);
            index+=7;
        }
        //上电 事件结束时间有效
        databuf[index++] = DT_DATETIME_S;
        DT_Time2DateTimeHex(timenow,(DateTimeHex_t *)&databuf[index]);
        index+=7;
    }
    databuf[index++] = DT_DOUBLE_LONG_UNSIGNED;
    memcpy_r(&databuf[index],(uint8*)&Eventseq,4);
    index+=4;
    //事件发生源
    databuf[index++] = DT_OAD;
    memcpy_r(&databuf[index],&pipeoad.value,4);
    index+=4;
    //事件上报状态
    databuf[index++] = DT_ARRAY;
    databuf[index++] =1;
    databuf[index++] = DT_STRUCTURE;
    databuf[index++] = 2;
    databuf[index++] = DT_OAD;
    memcpy_r(&databuf[index],&pipeoad.value,4);
    index+=4;
    databuf[index++] = DT_UNSIGNED;
    if(EventType==1)
    {
        databuf[index++] = 1;
    }else
    {
        databuf[index++] = 5;
    }
    memcpy(buf,databuf,MIN(index,maxlen));
    return MIN(index,maxlen);

}

/*******************************************************************************
* 函数名称: taskmng_meter_power_write
* 函数功能://存储停上电事件
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void taskmng_meter_power_write(uint8 pipe,uint8 *buf,uint16 buflen)
{
    //240628 原来就2000 太小会溢出
    uint8 rptBuff[4096] = {0x60, 0x12, 0x03, 0x00, 0x05, 0x00, 0x20, 0x2A, 0x02, 0x00, 0x00,
        0x60, 0x40, 0x02, 0x00, 0x00, 0x60, 0x41, 0x02, 0x00, 0x00, 0x60, 0x42, 0x02, 0x00,
        0x01, 0x30, 0x11, 0x02, 0x00, 0x05, 0x20, 0x1E, 0x02, 0x00, 0x20, 0x20, 0x02, 0x00,
        0x20, 0x22, 0x02, 0x00,0x20, 0x24, 0x02, 0x00,0x33, 0x00, 0x02, 0x00,};
    uint16 rptLen = 51;
    rptBuff[rptLen++] = 1;  //结果类型
    memcpy(&rptBuff[rptLen],buf,buflen);
    rptLen+=buflen;

    taskmng_event_report_data_write(pipe,rptBuff,rptLen,PowerOnOffOad);
}


/**********************************************************
* 函 数 名 : taskmmng_meter_power_event_write
* 函数功能 : 将电表的停上电状态写入数据中心
* 输入参数 : METER_POWER_STATE_T *data     电表的停上电状态
* 输出参数 : 无
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/5/31
************************************************************/
int taskmmng_meter_power_event_write(uint8 pipe,DB_CLIENT clientid,METER_POWER_STATE_T *data)
{
    int ret;
    NOMAL_OAD_T NomalOad;
    memset(&NomalOad,0,sizeof(NomalOad));
    NomalOad.logicId = 0;
    NomalOad.oad =meterPowerOad;
    NomalOad.classtag = 5;
    NomalOad.infoNum = data->pn;
    TASK_FMT_DEBUG( pipe, RELAY_LOG,"pn %d 写停电状态 %d seq %d\n",data->pn,data->powerState,data->eventSeq);

    ret = db_write_nomal(clientid,&NomalOad,(uint8*)data,sizeof(METER_POWER_STATE_T));
    if(ret !=0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"写入失败 ret %d \n",ret);
    }
    return ret;
}

/**********************************************************
* 函 数 名 : taskmng_meter_power_event_read
* 函数功能 : 收到电表的上电事件后从数据中心读一下看是否有停电事件
* 输入参数 : DB_CLIENT clientid         数据中心句柄
*             data 入参带上pn和地址 用作和数据中心读出来的校验一下
* 输出参数 : METER_POWER_STATE_T *data     读出来的事件状态
* 返 回 值 : 成功 : 0
*            失败 : -1
* 修改历史 :  2024/6/1
************************************************************/
int taskmng_meter_power_event_read(uint8 pipe,DB_CLIENT clientid,METER_POWER_STATE_T *data)
{
    int ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    memset(&NormalOad,0,sizeof(NormalOad));
    NormalOad.logicId = 0;
    NormalOad.oad = meterPowerOad;
    NormalOad.infoNum = data->pn;
    METER_POWER_STATE_T outdata;
    memset(&outdata,0,sizeof(outdata));

    ret = db_read_nomal(clientid,&NormalOad,sizeof(METER_POWER_STATE_T),(uint8*)&outdata,&len);

    if(ret !=0 ||len!=sizeof(METER_POWER_STATE_T))
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"读取pn%d 停电状态失败 ret %d \n",data->pn,ret);
        return -1;
    }
    /*读出来后校验下地址对不对  */
    if(data->pn != outdata.pn ||  memcmp(data->addr,outdata.addr,6) != 0)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"读取pn%d 停电状态地址不对应 \n",data->pn);
        return -1;
    }
    memcpy(data,&outdata,sizeof(METER_POWER_STATE_T));
    return 0;
}

#endif

#if DESC("电表上电后的时钟管理",1)

/**********************************************************
* 函 数 名 : taskmng_meter_time_get
* 函数功能 : 根据电表规约组查询时钟的报文
* 输入参数 :
* 输出参数 : 无
* 返 回 值 : 成功 : 报文长度
*            失败 : 0
* 修改历史 :  2024/6/7
************************************************************/
uint16 taskmng_meter_time_get(uint8 pipe,METER_TIMEMANAGER_T *meterTime,uint8 *sendbuf,uint32 maxlen)
{
    if(NULL == meterTime || NULL == sendbuf)
    {
        return 0;
    }
    uint8 buf[2048]={0};
    uint16 len;
    MeterPrtlFmt_T    Meter;
    memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "查询电表 %d 时钟 prtl %d 0x%02x%02x%02x%02x%02x%02x\n",meterTime->pn,meterTime->prtl,
        meterTime->addr[5],meterTime->addr[4],meterTime->addr[3],meterTime->addr[2],meterTime->addr[1],meterTime->addr[0]);
    switch(meterTime->prtl)
    {
        case PRTL_97:
        {
            uint16 DI;
            memcpy(Meter.MtrAddr,meterTime->addr,6);
            Meter.CtrlCode = 0x01;
            Meter.DataLen = 2;
            if(meterTime->getIndex==0)
            {
                DI = 0XC010;
            }else
            {
                DI = 0xC011;
            }
            memcpy(Meter.DataBuff,&DI,2);
            len = taskmng_meter_prtl_645_pack(buf,Meter);
        }
        break;
        case PRTL_07:
        {
            uint32 DI;
            memcpy(Meter.MtrAddr,meterTime->addr,6);
            Meter.CtrlCode = 0x11;
            Meter.DataLen = 4;
            if(meterTime->getIndex==0)
            {
                DI = 0x04000101;
            }else
            {
                DI = 0x04000102;
            }
            memcpy(Meter.DataBuff,&DI,4);
            len = taskmng_meter_prtl_645_pack(buf,Meter);
        }
            break;
        case PRTL_OOP:
        {
            OOP_OAD_U Oad = {0x40000200};
            LinkLayer_t *pLinkLayer = NULL;
            uint16 index=0;
            OOP_TSA_T Tsa;
            memset(&Tsa,0x00,sizeof(OOP_TSA_T));
            Tsa.len = 5;
            memcpy_r(Tsa.add,meterTime->addr,6);
            memcpy(buf,(uint8*)&Oad.value,4);
            index+=4;
            OopGet.requst(pipe,&Tsa,1,buf,index);
            pLinkLayer = FindLinkLayer(pipe, NULL);
            if(pLinkLayer!=NULL)
            {
                len = pLinkLayer->sendLen;
                memcpy(buf,pLinkLayer->sendBuff,len);
            }
        }
            break;
        default:
            break;
    }
    if(len > 0 && len < maxlen)
    {
        memcpy(sendbuf,buf,len);
        return len;
    }else
    {
        return 0;
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_get
* 函数功能 : 根据电表规约组设置时钟的报文
* 输入参数 :
* 输出参数 : 无
* 返 回 值 : 成功 : 报文长度
*            失败 : 0
* 修改历史 :  2024/6/7
************************************************************/
uint16 taskmng_meter_time_set(uint8 pipe,METER_TIMEMANAGER_T *meterTime,uint8 *sendbuf,uint32 maxlen)
{
    if(NULL == meterTime || NULL == sendbuf)
    {
        return 0;
    }
    time_t timenow = time(NULL);
    uint8 buf[2048]={0};
    uint16 len;

    switch(meterTime->prtl)
    {
    case PRTL_07:
    case PRTL_97:
    {
        MeterPrtlFmt_T    Meter;
        memset(&Meter,0x00,sizeof(MeterPrtlFmt_T));
        rtc_t prtc;
        //645表校时用广播地址
        memset(Meter.MtrAddr,0x99,6);
        Meter.CtrlCode = 0x08 ;
        Meter.DataLen = 0x06;
        TimeT2RTC(timenow,&prtc);
        Meter.DataBuff[0] = prtc.ss;
        Meter.DataBuff[1] = prtc.nn;
        Meter.DataBuff[2] = prtc.hh;
        Meter.DataBuff[3] = prtc.dd;
        Meter.DataBuff[4] = prtc.mm;
        Meter.DataBuff[5] = prtc.yy;
        len = taskmng_meter_prtl_645_pack(buf,Meter);
    }
        break;
    case PRTL_OOP:
    {
        LinkLayer_t *pLinkLayer = NULL;
        uint16 index=0;
        OOP_TSA_T Tsa;
        memset(&Tsa,0x00,sizeof(OOP_TSA_T));
        DateTimeHex_t TimeHex;
        Tsa.len = 5;
        memcpy_r(Tsa.add,meterTime->addr,6);

        DT_Time2DateTimeHex(timenow,&TimeHex);
        buf[index++] = 0x40;
        buf[index++] = 0x00;
        buf[index++] = 0x7f;
        buf[index++] = 0x00;
        buf[index++] = DT_DATETIME_S;
        memcpy(&buf[index],&TimeHex,7);
        index+=7;
        OopAction.requst(pipe,&Tsa,1,buf,index);
        pLinkLayer = FindLinkLayer(pipe, NULL);
        if(pLinkLayer!=NULL)
        {
            len = pLinkLayer->sendLen;
            memcpy(buf,pLinkLayer->sendBuff,len);
        }

    }
        break;
    default:
        break;
    }

    if(len > 0 && len < maxlen)
    {
        memcpy(sendbuf,buf,len);
        return len;
    }else
    {
        return 0;
    }
}


/**********************************************************
* 函 数 名 : taskmng_meter_time_send
* 函数功能 : 电表时钟管理发送
* 输入参数 : _TIMEMANAGER_T *meterTime电表时钟结构体
* 输出参数 : 无
* 返 回 值 : 1 切换到接收态
*            0 任务结束
             -1 异常
* 修改历史 :  2024/6/7
************************************************************/
int taskmng_meter_time_send(uint8 pipe,METER_TIMEMANAGER_T *meterTime)
{
    uint8 sendbuf[2048]={0};
    uint16 sendlen;
    switch(meterTime->status)
    {
    /* 查询电表时钟 */
    case TMN_TO_METER_RDINIT_FIRST:
    {
        sendlen = taskmng_meter_time_get(pipe,meterTime,sendbuf,sizeof(sendbuf));
        if(sendlen == 0)
        {
            return -1;
        }
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "查询电表 %d 时钟 \n",meterTime->pn);
    }
        break;
    /*小于五分钟的给电表校时  */
    case TMN_TO_METER_SETTIME_INIT:
    {
        sendlen = taskmng_meter_time_set(pipe,meterTime,sendbuf,sizeof(sendbuf));
        if(sendlen == 0)
        {
            return -1;
        }
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "设置电表 %d 时钟 \n",meterTime->pn);

    }
    default:
        break;
    }
    if(pipe == 0)
    {
        SendFrame_0038(pipe,meterTime->prtl,meterTime->addr,sendbuf,sendlen,&meterTime->sendIndex);
    }else
    {
        uint8 speed = 6;
        OOP_METER_BASIC_T MeterBasicInfo;
        memset(&MeterBasicInfo,0,sizeof(MeterBasicInfo));
        taskmng_mtr_basic_info_get(meterTime->pn,0,&MeterBasicInfo);
        if(MeterBasicInfo.baud != 0)
        {
            speed = MeterBasicInfo.baud;
        }
        rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,speed,PARITY_EVEN,sendbuf, sendlen);
    }
    return 1;
}


/**********************************************************
* 函 数 名 : taskmng_meter_time_check
* 函数功能 : 查询电表时钟后的回码处理
* 输入参数 : _TIMEMANAGER_T *meterTime结构体
* 输出参数 : 无
* 返 回 值 : 1 表示一个表结束 要切下一个
            0 表示正常切换到发送逻辑
            -1 表示异常
* 修改历史 :  2024/6/7
************************************************************/
int  taskmng_meter_time_check(uint8 pipe,METER_TIMEMANAGER_T *meterTime,uint8* buf,uint16 len)
{
    if(NULL == meterTime || NULL == buf)
    {
        return -1;
    }
    int ret;
    time_t metertime = 0;
    uint32 timediff;
    uint32 offset = 60; //最低要校时的阈值 默认60秒
    time_t timenow = time(NULL);
    if(SyncTimingParam.P2PSycnTime.bValid == 1)
    {
        if(SyncTimingParam.P2PSycnTime.offset != 0)
        {
            offset = abs(SyncTimingParam.P2PSycnTime.offset);
        }
    }
    switch(meterTime->prtl)
    {
        case PRTL_07:
        case PRTL_97:
        {
            MeterPrtlFmt_T MeterPrtlData;
            memset(&MeterPrtlData,0x00,sizeof(MeterPrtlFmt_T));
            ret = taskmng_645_meter_data_unpack(len,buf,MeterPrtlData.MtrAddr,&MeterPrtlData.CtrlCode,&MeterPrtlData.DataLen,MeterPrtlData.DataBuff);
            if(ret ==CHECK_RESULT_SUCCEED)
            {
                //645刚抄了年月日
                if(meterTime->getIndex == 0)
                {
                    if(meterTime->prtl == PRTL_97)
                    {
                        meterTime->metertime.year = commfun_BcdToHex(MeterPrtlData.DataBuff[5])+2000;
                        meterTime->metertime.month = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                        meterTime->metertime.day = commfun_BcdToHex(MeterPrtlData.DataBuff[3]);
                    }else
                    {
                        meterTime->metertime.year = commfun_BcdToHex(MeterPrtlData.DataBuff[7])+2000;
                        meterTime->metertime.month = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
                        meterTime->metertime.day = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
                    }
                    meterTime->getIndex++;
                    return 0;
                }else
                {
                    if(meterTime->prtl == PRTL_97)
                    {
                        meterTime->metertime.hour = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                        meterTime->metertime.minute = commfun_BcdToHex(MeterPrtlData.DataBuff[3]);
                        meterTime->metertime.second = commfun_BcdToHex(MeterPrtlData.DataBuff[2]);
                    }else
                    {
                        meterTime->metertime.hour = commfun_BcdToHex(MeterPrtlData.DataBuff[6]);
                        meterTime->metertime.minute = commfun_BcdToHex(MeterPrtlData.DataBuff[5]);
                        meterTime->metertime.second = commfun_BcdToHex(MeterPrtlData.DataBuff[4]);
                     }
                }
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
            TASK_FMT_DEBUG(pipe, RELAY_LOG,"LinkDataReceive ret %d \n",ret);
            if(ret >=eOopComm_Fail)//解帧失败直接返回
            {
                return -1;
            }
            //获取电表地址
            pAppLayer = FindAppLayer(pipe, NULL);
            if(pAppLayer == NULL)
            {
                return -1;
            }
            datalen = pAppLayer->recvApduList->apduLen-pAppLayer->recvApduList->offSet;
            memcpy(databuf, pAppLayer->recvApduList->apdu+pAppLayer->recvApduList->offSet, datalen);
            TASK_BUF_TRACE(pipe, RELAY_LOG,databuf,datalen);
            //databuf从oad后面的结果类型开始
            if(databuf[index++]!=1)
            {
                return -1;
            }

            if(databuf[index++]!=DT_DATETIME_S)
            {
                return -1;
            }

            memcpy_r((uint8*)&meterTime->metertime.year,&databuf[index],2);
            index+=2;
            meterTime->metertime.month = databuf[index++];
            meterTime->metertime.day = databuf[index++];
            meterTime->metertime.hour = databuf[index++];
            meterTime->metertime.minute = databuf[index++];
            meterTime->metertime.second = databuf[index++];
        }
            break;
        default:
            break;
    }

    DT_DateTimeShort2Time(&meterTime->metertime,&metertime);
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表时钟 %d/%d/%d %d:%d:%d time_t %d offset %d\n",meterTime->metertime.year,
     meterTime->metertime.month,meterTime->metertime.day,meterTime->metertime.hour,meterTime->metertime.minute,
     meterTime->metertime.second,metertime,offset);

    timediff = abs(timenow-metertime);
    if(timediff < offset )
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差%d小于阈值 %d 无需校时 \n",meterTime->pn,
            timediff,offset);
        return 1;
    }else if(timediff < 300)
    {
     /*5分钟内的单地址校时  */
         TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 大于阈值 %d 小于5分钟 需要校时 \n",meterTime->pn,
             timediff,offset);
        //240711 新规范要求避开整点校时 避开5分钟 所以判一下当前时钟
        //240712 要执行对时 都放到延时对时那个任务里去判断
        //if(timenow%3600 < 300 || timenow % 3600 > 3300 )
        {
           // TASK_FMT_DEBUG(pipe, RELAY_LOG, "避开整点校时 \n");
            //添加到另外一个队列 过了5分钟再执行
            taskmng_meter_time_delay_add(pipe,meterTime->pn,meterTime->prtl,meterTime->addr,timediff);
            return 1;
        }
//         meterTime->diff = timediff;
//         meterTime->status = TMN_TO_METER_SETTIME_INIT;
         
    }else
    {
        /*大于5分钟的写数据中心 产生时钟超差事件  */
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前电表 %d 和终端时差 %d 大于5分钟 \n",meterTime->pn,timediff);
        WRITE_RECORDS_T record;
        uint32 storeno = 0;
        memset(&record,0,sizeof(WRITE_RECORDS_T));
        record.recordOAD.infoNum = meterTime->pn;
        record.recordOAD.road.oadCols.nNum = 1;
        record.recordOAD.road.oadCols.oad[0].value = 0x40000200;
        record.recordOAD.classtag = 5;

        record.colStartTm = time(NULL);
        record.colStoreTm = time(NULL);
        record.colEndTm = time(NULL);
        #if IS_NEED_ADD_8HOUR
        record.colStartTm+=8*60*60;
        record.colStoreTm+=8*60*60;
        record.colEndTm+=8*60*60;
        #endif
        record.MAC.nNum = 6;
        memcpy_r(record.MAC.value,meterTime->addr,6);
        record.datas[0].len = sizeof(OOP_DATETIME_S_T);
        memcpy(record.datas[0].data,&meterTime->metertime,sizeof(OOP_DATETIME_S_T));
        ret = db_write_records(gAmrThreadArg[pipe].DBClientHandle,&record,&storeno);
        if(ret!=0)
        {
            TASK_FMT_TRACE ( pipe,RELAY_LOG, "db_write_records error ret  %d\n",ret);
        }

        return 1;
    }

    return 0;
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_send
* 函数功能 : 电表时钟管理发送
* 输入参数 : _TIMEMANAGER_T *meterTime电表时钟结构体
* 输出参数 : 无
* 返 回 值 : 0 切换到发送
*            1 任务结束
             -1 异常
* 修改历史 :  2024/6/7
************************************************************/
int taskmng_meter_time_recv(uint8 pipe,METER_TIMEMANAGER_T *meterTime,uint8* buf,uint16 len)
{
    int ret = -1;
    switch(meterTime->status)
    {
        case TMN_TO_METER_RDINIT_FIRST:
        {
            uint16 datalen=len;
            uint8 lenoffset = 0;
            if(pipe == 0)
            {
                datalen = XADR_GetEleNum(buf, &lenoffset);
                if(datalen<=1)
                    break;
            }
            ret = taskmng_meter_time_check(pipe,meterTime,&buf[lenoffset],datalen);

        }
            break;
        case TMN_TO_METER_SETTIME_INIT:
        {

            time_t timenow = time(NULL);

            OOP_CHK_TIMEINFO_T Chktimeinfo;
            memset(&Chktimeinfo,0x00,sizeof(OOP_CHK_TIMEINFO_T));

            Chktimeinfo.add.nNum = 6;
            memcpy_r(Chktimeinfo.add.value,meterTime->addr,6);
            DT_Time2DateTimeShort(timenow,&Chktimeinfo.chkTime);
            memcpy(&Chktimeinfo.beforechkTime,&meterTime->metertime,sizeof(OOP_DATETIME_S_T));
            Chktimeinfo.error = (int16)meterTime->diff;

            taskmng_normal_data_write_pipe( pipe, (uint8*)&Chktimeinfo, sizeof(OOP_CHK_TIMEINFO_T), MtrTimingOad, meterTime->pn,0);

            TASK_FMT_DEBUG(pipe, RELAY_LOG, "写入电表校时信息 Pn %d \n",meterTime->pn);
            return 1;
        }
            break;
        default:
            break;

    }
    return ret;
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_manager
* 函数功能 : 电表上报上电事件后的时钟管理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 成功 : 1
*            失败 : -1
* 修改历史 :  2024/6/7
************************************************************/
int taskmng_meter_time_manager(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    METER_TIMEMANAGER_T *meterTime = pAmrThreadArg->meterTimeManager;
    if(NULL == meterTime)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理任务结束\n");
        pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_IDLE;
        return 1;
    }
    int ret;
    uint8 RecvBuf[2048]={0};
    int RecvLen;
    MESSAGE_INFO_T* MsgInfo =NULL;

    switch(pAmrThreadArg->TotalState.meterTimeState)
    {
    case INNER_RTS_IDLE:
    {
        /*初始态 暂时不需要干啥 */
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理任务开始\n");
        pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_SEND;
    }
        break;
    case INNER_RTS_SEND:
    {
        ret = taskmng_meter_time_send(pipe,meterTime);
        if(ret == 1)
        {
            meterTime->sendtime = time(NULL);
            pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RECV;
        }else if(ret == 0)
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理任务结束\n");
            pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_IDLE;
            return 1;
        }else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "异常\n");
            pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;

        }
    }
        break;
    case INNER_RTS_RECV:
    {
        if(pipe == 0)
        {
            RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
            if(RecvLen > 0)
            {
                MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
                TASK_FMT_DEBUG( pipe, RELAY_LOG,"recvindex %d  Sendindex %d \n",MsgInfo->index,meterTime->sendIndex );
                TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
                if(MsgInfo->index != meterTime->sendIndex)
                {
                    break;
                }
                if(MsgInfo->msglen == 0)
                {
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;
                    break;
                }
                ret = taskmng_meter_time_recv(pipe,meterTime,MsgInfo->playload, MsgInfo->msglen);
                if(ret == 0)
                {
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_SEND;
                }else if(ret == 1)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "切表\n");
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_IDLE;
                    pAmrThreadArg->meterTimeManager = meterTime->next;
                    free(meterTime);
                    meterTime = NULL;
                    if(NULL == pAmrThreadArg->meterTimeManager)
                    {
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                        return 1;
                    }
                }else
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "异常\n");
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;
                }
            }else
            {
                if(abs(time(NULL) - meterTime->sendtime) > BROAD_TIMING_OVERTIME)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理接收超时\n");
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;
                }
            }
        }else
        {
            RecvLen = rs485_RS232_recv(pipe,RecvBuf, sizeof(RecvBuf));
            if(RecvLen>0)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到消息 \n");
                TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
                ret = taskmng_meter_time_recv(pipe,meterTime,RecvBuf, RecvLen);
                if(ret == 0)
                {
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_SEND;
                }else if(ret == 1)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "切表\n");
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_IDLE;
                    pAmrThreadArg->meterTimeManager = meterTime->next;
                    free(meterTime);
                    meterTime = NULL;
                    if(NULL == pAmrThreadArg->meterTimeManager)
                    {
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                        return 1;
                    }
                }else
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "异常\n");
                    pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;
                }
            }
            else
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理接收超时\n");
                pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_RETRY;
            }
        }
    }
        break;
    case INNER_RTS_RETRY:
    {
        meterTime->failcnt++;
        if(meterTime->failcnt >= 3)
        {
            /*失败超过三次后切表  */
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表 %d 失败次数过多\n",meterTime->pn);
            pAmrThreadArg->meterTimeManager = meterTime->next;
            free(meterTime);
            meterTime = NULL;
            if(NULL == pAmrThreadArg->meterTimeManager)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_IDLE;
                return 1;
            }
        }
        pAmrThreadArg->TotalState.meterTimeState = INNER_RTS_SEND;
    }
        break;
    default:
        break;
    }
    return 0;
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_add
* 函数功能 : 电表上报上电事件后添加到链表中
* 输入参数 :
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/8
************************************************************/

void taskmng_meter_time_add(uint8 pipe,uint16 pn,uint8 prtl,uint8 *addr)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    METER_TIMEMANAGER_T *meterTime = pAmrThreadArg->meterTimeManager;
    METER_TIMEMANAGER_T *newnode = (METER_TIMEMANAGER_T*)malloc(sizeof(METER_TIMEMANAGER_T));
    if(NULL == newnode)
    {
        return;
    }
    memset(newnode,0,sizeof(METER_TIMEMANAGER_T));
    newnode->pn = pn;
    newnode->prtl = prtl;
    newnode->status = TMN_TO_METER_RDINIT_FIRST;
    memcpy(newnode->addr,addr,6);
    newnode->next = NULL;
    if(NULL == meterTime )
    {
        pAmrThreadArg->meterTimeManager = newnode;
        return;
    }
    while(meterTime)
    {
        if(NULL == meterTime->next)
        {
            meterTime->next = newnode;
            return;
        }
        meterTime = meterTime->next;
    }
}


/**********************************************************
* 函 数 名 : taskmng_meter_event_report_power_check
* 函数功能 : 收到电表直接上报的停电事件后判断下是不是上电事件
85 03 00 30 11 02 00 05 00 20 22 02 00 00 20 1e 02 00 00 20 20 02 00 00 33 00 02 00 00 20 24 02 00 01 01 06 00 00 00 1d 1c 07 e7 04 03 0b 11 0e 00 00 00 00 00

* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/8
************************************************************/
void taskmng_meter_event_report_power_check(uint8 pipe,uint16 pn,uint8 *buf,uint16 len)
{
    uint16 index = 0;
    uint8 i,csdnum;
    OOP_OAD_U oad;
    csdnum = buf[index++];
    for(i=0;i<csdnum ;i++)
    {
        if(buf[index++] != 0)
            return ;
        memcpy_r(&oad.value,&buf[index],4);
        index+=4;
        if(i == 0 && oad.value != 0x20220200)
        {
            TASK_FMT_DEBUG( pipe, RELAY_LOG,"第一个csd不是事件序号 %08x \n",oad.value);
            return ;
        }
    }
    if(buf[index++] == 0)
        return ;
    //记录条数
    if(buf[index++] == 0)
        return ;
    //解到第一个事件序号
    if(buf[index++] != DT_DOUBLE_LONG_UNSIGNED)
    {
        return;
    }
    index+=4;
    //事件发生时间
    if(buf[index++]!=DT_DATETIME_S)
    {
        return;
    }
    index += 7;
    //看有没有事件结束时间判断是不是上电事件
    if(buf[index++] == DT_DATETIME_S)
    {
        TASK_FMT_DEBUG( pipe, RELAY_LOG,"收到 pn %d 的上电事件 \n",pn);
        OOP_METER_BASIC_T basic;
        uint8 addr[6]={0};
        memset(&basic,0,sizeof(basic));
        taskmng_mtr_basic_info_get(pn,0,&basic);
        if(basic.protocol != 0)
        {
            memcpy_r(addr,basic.tsa.add,6);
            taskmng_meter_time_add(pipe,pn,basic.protocol,addr);
        }
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_485manager
* 函数功能 : 485的电表上电后时钟管理处理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/8
************************************************************/
void taskmng_meter_time_485manager(uint8 pipe)
{
    int ret = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    if(NULL == pAmrThreadArg->meterTimeManager)
    {
        return;
    }
    while(1)
    {
        ret = taskmng_meter_time_manager(pipe);
        if(ret ==1)
        {
            break;
        }
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_485delay
* 函数功能 : 485的电表延时对时处理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/8
************************************************************/
void taskmng_meter_time_485delay(uint8 pipe)
{
    int ret = 0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    if(NULL == pAmrThreadArg->meterTimeDelay)
    {
        return;
    }
    time_t timenow = time(NULL);
    if(timenow%3600 < 300 || timenow%3600>3300)
    {
        return;
    }
    while(1)
    {
        ret = taskmng_meter_time_delay(pipe);
        if(ret ==1)
        {
            break;
        }
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_add
* 函数功能 : 电表上报上电事件后添加到链表中
* 输入参数 :
* 输出参数 : 无
* 返 回 值 :
* 修改历史 :  2024/6/8
************************************************************/

void taskmng_meter_time_delay_add(uint8 pipe,uint16 pn,uint8 prtl,uint8 *addr,uint32 diff)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    METER_TIMEMANAGER_T *meterTime = pAmrThreadArg->meterTimeDelay;
    METER_TIMEMANAGER_T *newnode = (METER_TIMEMANAGER_T*)malloc(sizeof(METER_TIMEMANAGER_T));
    if(NULL == newnode)
    {
        return;
    }
    uint8 speed = 6;
    OOP_METER_BASIC_T MeterBasicInfo;
    memset(&MeterBasicInfo,0,sizeof(MeterBasicInfo));

    taskmng_mtr_basic_info_get(pn,0,&MeterBasicInfo);
    if(MeterBasicInfo.baud != 0)
    {
        speed = MeterBasicInfo.baud;
    }
    memset(newnode,0,sizeof(METER_TIMEMANAGER_T));
    newnode->pn = pn;
    newnode->prtl = prtl;
    memcpy(newnode->addr,addr,6);
    newnode->speed = speed; //把波特率记下来 485要用
    newnode->diff = diff;
    newnode->next = NULL;
    if(NULL == meterTime )
    {
        pAmrThreadArg->meterTimeDelay = newnode;
        return;
    }
    while(meterTime)
    {
        //485端口的都是用的广播 判一下链表里有645表的同波特率的就不添加了
        //测试的时候485端口的698表也可以用广播 但是我们现在还是用的单地址校时 后面看要不要改
        if(pipe >= PLC_NUM)
        {
            if(prtl == PRTL_07 && meterTime->prtl == prtl && meterTime->speed == speed)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "不重复添加 pn %d \n",pn);
                free(newnode);
                newnode = NULL;
                return;
            }
        }
        if(NULL == meterTime->next)
        {
            meterTime->next = newnode;
            return;
        }
        meterTime = meterTime->next;
    }
}

/**********************************************************
* 函 数 名 : taskmng_meter_time_delay_recv
* 函数功能 : 电表时钟管理发送
* 输入参数 : _TIMEMANAGER_T *meterTime电表时钟结构体
* 输出参数 : 无
* 返 回 值 : 0 切换到发送
*            1 任务结束
             -1 异常
* 修改历史 :  2024/6/7
************************************************************/
void taskmng_meter_time_delay_recv(uint8 pipe,METER_TIMEMANAGER_T *meterTime,uint8* buf,uint16 len)
{
    time_t timenow = time(NULL);
    
    OOP_CHK_TIMEINFO_T Chktimeinfo;
    memset(&Chktimeinfo,0x00,sizeof(OOP_CHK_TIMEINFO_T));
    
    Chktimeinfo.add.nNum = 6;
    memcpy_r(Chktimeinfo.add.value,meterTime->addr,6);
    DT_Time2DateTimeShort(timenow,&Chktimeinfo.chkTime);
    memcpy(&Chktimeinfo.beforechkTime,&meterTime->metertime,sizeof(OOP_DATETIME_S_T));
    Chktimeinfo.error = (int16)meterTime->diff;
    
    taskmng_normal_data_write_pipe( pipe, (uint8*)&Chktimeinfo, sizeof(OOP_CHK_TIMEINFO_T), MtrTimingOad, meterTime->pn,0);
    
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "写入电表校时信息 Pn %d \n",meterTime->pn);


}


/**********************************************************
* 函 数 名 : taskmng_meter_time_delay_send
* 函数功能 : 电表时钟管理发送
* 输入参数 : _TIMEMANAGER_T *meterTime电表时钟结构体
* 输出参数 : 无
* 返 回 值 : 1 切换到接收态
*            0 任务结束
             -1 异常
* 修改历史 :  2024/6/7
************************************************************/
int taskmng_meter_time_delay_send(uint8 pipe,METER_TIMEMANAGER_T *meterTime)
{
    uint8 sendbuf[2048]={0};
    uint16 sendlen;

    sendlen = taskmng_meter_time_set(pipe,meterTime,sendbuf,sizeof(sendbuf));
    if(sendlen == 0)
    {
        return -1;
    }
    TASK_FMT_DEBUG(pipe, RELAY_LOG, "设置电表 %d 时钟 \n",meterTime->pn);
    TASK_BUF_DEBUG(pipe,RELAY_LOG,sendbuf,sendlen);
    if(pipe == 0)
    {
        //645的用广播发
        if(meterTime->prtl == PRTL_07)
        {
            taskmng_data_broad(pipe,sendbuf,sendlen,meterTime->prtl,&meterTime->sendIndex);

        }else
        {
            SendFrame_0038(pipe,meterTime->prtl,meterTime->addr,sendbuf,sendlen,&meterTime->sendIndex);
        }
    }else
    {
        rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,meterTime->speed,PARITY_EVEN,sendbuf, sendlen);
    }
    return 1;
}


/**********************************************************
* 函 数 名 : taskmng_meter_time_manager
* 函数功能 : 电表上报上电事件后的时钟管理
* 输入参数 : 无
* 输出参数 : 无
* 返 回 值 : 成功 : 1
*            失败 : -1
* 修改历史 :  2024/6/7
************************************************************/
int taskmng_meter_time_delay(uint8 pipe)
{
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
    METER_TIMEMANAGER_T *meterTime = pAmrThreadArg->meterTimeDelay;
    if(NULL == meterTime)
    {
        TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表延时校时任务结束\n");
        pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
        return 1;
    }
    int ret;
    uint8 RecvBuf[2048]={0};
    int RecvLen;
    MESSAGE_INFO_T* MsgInfo =NULL;
    switch(pAmrThreadArg->TotalState.meterTimeDelay)
    {
        case INNER_RTS_IDLE:
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表延时校时任务开始\n");
            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_SEND;
        }
            break;
        case INNER_RTS_SEND:
        {
            //发送之前判断下是不是整点周围五分钟
            time_t timenow = time(NULL);
            if(timenow%3600 < 300 || timenow%3600>3300)
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "整点五分钟内不允许对时\n");
                return 1;
            }
            ret = taskmng_meter_time_delay_send(pipe,meterTime);
            if(ret == 1)
            {
                meterTime->sendtime = timenow;
                pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_RECV;
            }else
            {
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "发生异常\n");
                pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_RETRY;
            }
        }
        break;
        case INNER_RTS_RECV:
        {
            if(meterTime->prtl == PRTL_07)
            {
               //07表都是用的广播校时 没有回码 sleep后继续发
                sleep(2);
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "切表\n");
                pAmrThreadArg->meterTimeDelay = meterTime->next;
                free(meterTime);
                meterTime = NULL;
                if(NULL == pAmrThreadArg->meterTimeDelay)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                    pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
                    return 1;
                }
                pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_SEND;
            }else
            {
                if(pipe == 0)
                {
                    RecvLen = taskmng_plc_queue_read ( pAmrRecvDown[pipe], RecvBuf );
                    if(RecvLen > 0)
                    {
                        MsgInfo = (MESSAGE_INFO_T*)RecvBuf;
                        TASK_FMT_DEBUG( pipe, RELAY_LOG,"recvindex %d  Sendindex %d \n",MsgInfo->index,meterTime->sendIndex );
                        TASK_BUF_DEBUG( pipe, RELAY_LOG,MsgInfo->playload, MsgInfo->msglen);
                        if(MsgInfo->index != meterTime->sendIndex)
                        {
                            break;
                        }
                        if(MsgInfo->msglen == 0)
                        {
                            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_RETRY;
                            break;
                        }
                        taskmng_meter_time_delay_recv(pipe,meterTime,MsgInfo->playload, MsgInfo->msglen);
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "切表\n");
                        pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_SEND;
                        pAmrThreadArg->meterTimeDelay = meterTime->next;
                        free(meterTime);
                        meterTime = NULL;
                        if(NULL == pAmrThreadArg->meterTimeDelay)
                        {
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
                            return 1;
                        }
                    }else
                    {
                        if(abs(time(NULL) - meterTime->sendtime) > BROAD_TIMING_OVERTIME)
                        {
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理接收超时\n");
                            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_RETRY;
                        }

                    }
                }else
                {
                    RecvLen = rs485_RS232_recv(pipe,RecvBuf, sizeof(RecvBuf));
                    if(RecvLen>0)
                    {
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "收到消息 \n");
                        TASK_BUF_DEBUG( pipe, RELAY_LOG,RecvBuf, RecvLen);
                        taskmng_meter_time_delay_recv(pipe,meterTime,RecvBuf, RecvLen);
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "切表\n");
                        pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_SEND;
                        pAmrThreadArg->meterTimeDelay = meterTime->next;
                        free(meterTime);
                        meterTime = NULL;
                        if(NULL == pAmrThreadArg->meterTimeDelay)
                        {
                            TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
                            return 1;
                        }
                    }
                    else
                    {
                        TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表时钟管理接收超时\n");
                        pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_RETRY;
                    }
                }
            }
        }
        break;
        case INNER_RTS_RETRY:
        {
            meterTime->failcnt++;
            if(meterTime->failcnt >= 3)
            {
                /*失败超过三次后切表  */
                TASK_FMT_DEBUG(pipe, RELAY_LOG, "电表 %d 失败次数过多\n",meterTime->pn);
                pAmrThreadArg->meterTimeDelay = meterTime->next;
                free(meterTime);
                meterTime = NULL;
                if(NULL == pAmrThreadArg->meterTimeDelay)
                {
                    TASK_FMT_DEBUG(pipe, RELAY_LOG, "任务结束\n");
                    pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
                    return 1;
                }
            }
            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_SEND;

        }
        break;
        default:
        {
            pAmrThreadArg->TotalState.meterTimeDelay = INNER_RTS_IDLE;
        }
        break;
    }
    return 0;
}


#endif

