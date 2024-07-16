    

#include "CtsAll.h"
#include "area.h"

extern OOP_COLLECTOR_T Collector;

SPEED_LIST_485_T gSpeedList;

uint8 gParallelNum = 0; //以档案数量来判断并发数 如果载波档案少于10个 则并发数改为5

/*******************************************************************************
* 函数名称: taskmng_amr_info_read
* 函数功能: 遍历所有档案，将不同端口的档案加载到对应的线程中
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_amr_info_read(void)
{
    uint16 i;
    uint8 pipe;
    uint16 j;
    for(i=0;i<AMR_THREAD_NUM;i++)
    {
        memset(&gAmrThreadArg[i].TmnAmrInfo,0x00,sizeof(TmnAmrInfo_T));
        memset(gAmrThreadArg[i].pMtrAmrInfo,0x00,sizeof(MtrAmrInfo_T)*MAX_AMR_COLLECT_CNT);
        memset(gAmrThreadArg[i].PatchMtrInfo,0x00,sizeof(PATCH_MTR_INFO_T)*MAX_AMR_COLLECT_CNT);
    }
    TGThreadArg.Pn = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
        if(pipe==0xff)
            continue;
        //交采测量点
        if(pipe==AMR_THREAD_NUM)
        {
            TGThreadArg.Pn = MeterFileAll.MeterFile[i].nIndex;
            TASK_FMT_TRACE(-1, TASK_LOG,"交采测量点 Pn %d \n",MeterFileAll.MeterFile[i].nIndex);
            continue;
        }
        gAmrThreadArg[pipe].pMtrAmrInfo[gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt].Pn = MeterFileAll.MeterFile[i].nIndex;
        gAmrThreadArg[pipe].pMtrAmrInfo[gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt].logicid = MeterFileAll.MeterLogicid[i];
        gAmrThreadArg[pipe].PatchMtrInfo[gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt].Pn = MeterFileAll.MeterFile[i].nIndex;
        gAmrThreadArg[pipe].PatchMtrInfo[gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt].logicid = MeterFileAll.MeterLogicid[i];
        gAmrThreadArg[pipe].TmnAmrInfo.MtrCnt++;
        
    }
    for(i=0;i<AMR_THREAD_NUM;i++)
    {
        TASK_FMT_TRACE(-1, TASK_LOG, " pipe %d MtrCnt %d  \n",i,gAmrThreadArg[i].TmnAmrInfo.MtrCnt );
        for(j=0;j<gAmrThreadArg[i].TmnAmrInfo.MtrCnt;j++)
        {
            TASK_FMT_TRACE(-1, TASK_LOG,"index %d Pn %d logicid %d \n",j,gAmrThreadArg[i].pMtrAmrInfo[j].Pn,gAmrThreadArg[i].pMtrAmrInfo[j].logicid);
        }
    }
    if(gAmrThreadArg[0].TmnAmrInfo.MtrCnt > 10)
    {
        gParallelNum = PARALLEL_QUEUE_NUM;
    }else
    {
        gParallelNum = 5;
    }
    TASK_FMT_TRACE(-1, TASK_LOG, "载波档案个数 %d 并发数为 %d \n",gAmrThreadArg[0].TmnAmrInfo.MtrCnt, gParallelNum);
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

/*******************************************************************************
* 函数名称: taskmng_addr_isvalid_check
* 函数功能: 检测是否有效的测量点地址
* 输入参数: MtrAddr             要检测的测量点地址
*           
* 输出参数: 无
* 返 回 值:  >0                  有效
*           <0                  无效
*******************************************************************************/
int taskmng_addr_isvalid_check(uint8 *MtrAddr,uint8 addrlen)
{
    uint8   TestAddr[16];

    //判断采集器地址是否是6个字节0xff
    memset(TestAddr, 0xff, 16);
    if (memcmp(MtrAddr, TestAddr, addrlen) == 0)
    {
        return -1;
    }

    memset(TestAddr, 0xEE, 16);
    if (memcmp(MtrAddr, TestAddr, addrlen) == 0)
    {
        return -1;
    }

    //common bug(2014-6-24-14-07-42)
    //判断采集器地址或电表地址是否是6个字节0x00
    memset(TestAddr, 0x00, 16);
    if (memcmp(MtrAddr, TestAddr, addrlen) == 0)
    {
        return -1;
    }
    return 1;
}

/*******************************************************************************
* 函数名称: taskmng_addr_in_amr_info_check
* 函数功能: 判断本线程档案中是否存在指定的电表
* 输入参数: pipe 端口号
            Pn 测量点号
            MtrAddr             要检测的测量点地址
            prtl    规约类型
* 输出参数: MtrIndex        电表索引
* 返 回 值:  >0                  有效
*           ==0                  无效
*******************************************************************************/
uint16 taskmng_addr_in_amr_info_check(uint8 Pipe, uint8 logicid, uint8 *MtrAddr, uint8 prtl, uint16 *MtrIndex)
{
    uint16  i;
    
    uint8   tmpPrtl;
    uint8   tmpAddr[6]={0};
    AmrThreadArg_T* pAmrThreadArg;
    uint16  meterCnt = 0;

    pAmrThreadArg = &gAmrThreadArg[Pipe];
    for(i=0;i<pAmrThreadArg->TmnAmrInfo.MtrCnt;i++)
    {
        //根据每个信息点号获取电表地址来比对
        taskmng_mtr_addr_prtl_get(pAmrThreadArg->pMtrAmrInfo[i].Pn,logicid,tmpAddr,&tmpPrtl);
        if(memcmp(MtrAddr,tmpAddr,6)==0 && prtl==tmpPrtl)
        {
            MtrIndex[meterCnt] = i;
            meterCnt++;
        }
        
    }

    return meterCnt;
}

 /*******************************************************************************
* 函数名称: taskmng_mtr_addr_prtl_get
* 函数功能: 根据序号获取电表地址和规约类型
* 输入参数: Pn             要检测的测量点

* 输出参数: MtrAddr        电表地址
        prtl        规约类型
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_addr_prtl_get(uint16 Pn,uint8 logicid,uint8 *MtrAddr,uint8 *prtl)
{
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i] )
        {
            memcpy_r(MtrAddr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}
  /*******************************************************************************
 * 函数名称: taskmng_mtr_addr_prtl_get
 * 函数功能: 根据序号获取电表地址和规约类型
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: MtrAddr        电表地址
         prtl        规约类型
 * 返 回 值:  
 *******************************************************************************/
 int16 taskmng_caj_addr_prtl_get(uint16 Pn,uint8 *MtrAddr,uint8 *prtl)
 {
     uint8 i;

     for(i=0;i<Collector.count;i++)
     {
         if(Pn == Collector.collectaddr[i].pn)
         {
             memcpy(MtrAddr,Collector.collectaddr[i].addr,6);
             *prtl = Collector.collectaddr[i].prtl;
             return i;
         }
     }
        return -1;
 }

  /*******************************************************************************
 * 函数名称: taskmng_mtr_addr_prtl_get
 * 函数功能: 根据序号获取电表地址和规约类型
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: MtrAddr        电表地址
         prtl        规约类型
 * 返 回 值:  
 *******************************************************************************/
 int16 taskmng_caj_addr_get(uint16 Pn,uint8 *MtrAddr)
 {
     uint8 i;

     for(i=0;i<Collector.count;i++)
     {
         if(Pn == Collector.collectaddr[i].pn)
         {
             memcpy(MtrAddr,Collector.collectaddr[i].addr,6);
             return i;
         }
     }
        return -1;
 }

  /*******************************************************************************
 * 函数名称: taskmng_mtr_addr_prtl_get
 * 函数功能: 根据序号获取电表地址和规约类型
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: MtrAddr        电表地址
         prtl        规约类型
 * 返 回 值:  
 *******************************************************************************/
 int16 taskmng_caj_addr_get_from_meteraddr(uint16 *Pn,uint8 *MtrAddr,uint8* cajaddr)
 {
     uint8 i,k=0;

     for(i=0;i<Collector.count;i++)
     {
        TASK_FMT_DEBUG(0, REC_LOG,"Collector.collectaddr[i].meterinfo.num is %d ,i is %d \n",Collector.collectaddr[i].meterinfo.num,i);
        for(k=0;k<Collector.collectaddr[i].meterinfo.num;k++)
        {
            TASK_BUF_DEBUG(0, REC_LOG,MtrAddr,6);
            TASK_BUF_DEBUG(0, REC_LOG,(uint8*)&Collector.collectaddr[i].meterinfo.meteraddr[k][0],6);
             if(memcmp((uint8*)&Collector.collectaddr[i].meterinfo.meteraddr[k][0],MtrAddr,6)==0)
             {
                 memcpy(cajaddr,Collector.collectaddr[i].addr,6);
                 *Pn = Collector.collectaddr[i].pn;
                 return i;
             }
         }
     }
        return -1;
 }

 /*******************************************************************************
* 函数名称: taskmng_mtr_prtl_get
* 函数功能: 根据序号获取电表规约类型
* 输入参数: Pn             要检测的测量点

* 输出参数: 
        prtl        规约类型
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_prtl_get(uint16 Pn,uint8 logicid,uint8 *prtl)
{
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i] )
        {
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}
 /*******************************************************************************
* 函数名称: taskmng_mtr_addr_get
* 函数功能: 根据序号获取电表地址和规约类型
* 输入参数: Pn             要检测的测量点

* 输出参数: MtrAddr        电表地址
        prtl        规约类型
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_addr_get(uint16 Pn,uint8 logicid,uint8 *MtrAddr)
{
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
        {
            memcpy_r(MtrAddr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

  /*******************************************************************************
 * 函数名称: taskmng_mtr_addr_get
 * 函数功能: 根据序号获取电表地址和规约类型
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: MtrAddr        电表地址
         prtl        规约类型
 * 返 回 值:  
 *******************************************************************************/
 void taskmng_mtr_addr_pn_get_by_userType(uint8 userType,uint8 logicid,uint8 *MtrAddr,uint16 *pn)
 {
     uint16 i;
     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0;i<MeterFileAll.MeterCount;i++)
     {
         if(userType == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
         {
             memcpy_r(MtrAddr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
             *pn = MeterFileAll.MeterFile[i].nIndex;
             pthread_rwlock_unlock(&MeterFileAll.rwLock);
             return;
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
 }

 /*******************************************************************************
* 函数名称: taskmng_mtr_basic_info_get
* 函数功能: 根据序号获取电表基本信息
* 输入参数: Pn             要检测的测量点

* 输出参数: MtrAddr        电表基本信息
  
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_basic_info_get(uint16 Pn,uint8 logicid,OOP_METER_BASIC_T *MtrAddr)
{
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
        {
            memcpy((uint8 *)MtrAddr,(uint8*)&MeterFileAll.MeterFile[i].basic,sizeof(OOP_METER_BASIC_T));
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

 
  /*******************************************************************************
 * 函数名称: taskmng_mtr_basic_info_get
 * 函数功能: 根据序号获取电表基本信息
 * 输入参数: Pn             要检测的测量点
 
 * 输出参数: MtrAddr        电表基本信息
   
 * 返 回 值:  
 *******************************************************************************/
 void taskmng_mtr_extend_info_get(uint16 Pn,uint8 logicid,OOP_METER_EXTENDED_T *extend)
 {
     uint16 i;
     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0;i<MeterFileAll.MeterCount;i++)
     {
         if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
         {
             memcpy((uint8 *)extend,(uint8*)&MeterFileAll.MeterFile[i].extend,sizeof(OOP_METER_EXTENDED_T));
             pthread_rwlock_unlock(&MeterFileAll.rwLock);
             return;
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
 }

 void taskmng_mtr_basic_info_get_by_tsa(OOP_TSA_T *tsa,OOP_METER_BASIC_T *MtrAddr)
 {
     uint16 i;
     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0;i<MeterFileAll.MeterCount;i++)
     {
         if(memcmp(tsa->add, MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 )
         {
             memcpy((uint8 *)MtrAddr,(uint8*)&MeterFileAll.MeterFile[i].basic,sizeof(OOP_METER_BASIC_T));
             pthread_rwlock_unlock(&MeterFileAll.rwLock);
             return;
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
 }

 /*******************************************************************************
* 函数名称: taskmng_tsa_get
* 函数功能: 根据序号获取电表地址
* 输入参数: Pn             要检测的测量点

* 输出参数: 
  
* 返 回 值:  电表地址
*******************************************************************************/
OOP_TSA_T taskmng_tsa_get(uint16 Pn,uint8 logicid)
{
    OOP_TSA_T tsa;
    memset(&tsa,0x00,sizeof(OOP_TSA_T));
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
        {
            tsa=MeterFileAll.MeterFile[i].basic.tsa;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return tsa;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return tsa;
}

 /*******************************************************************************
* 函数名称: taskmng_mtr_conntype_get
* 函数功能: //获取接线方式
* 输入参数: Pn             要检测的测量点

* 输出参数: 
  
* 返 回 值:  电表接线方式
*******************************************************************************/
uint8 taskmng_mtr_conntype_get(uint16 Pn,uint8 logicid)
{
    uint16 i;
    uint8 type =0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
        {
        
            type = MeterFileAll.MeterFile[i].basic.pwrType;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return type;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
   return type;
}

 /*******************************************************************************
* 函数名称: taskmng_mtr_annex_get
* 函数功能: //获取接线方式
* 输入参数: Pn             要检测的测量点

* 输出参数: 
  
* 返 回 值:  获取附加属性
*******************************************************************************/
OOP_METER_ANNEX_T taskmng_mtr_annex_get(uint16 Pn,uint8 logicid)
{
    uint16 i;
    OOP_METER_ANNEX_T       annex;
	
	memset((uint8*)&annex,0x00,sizeof(annex));
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i])
        {
        
            annex = MeterFileAll.MeterFile[i].annex;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return annex;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
   return annex;
}

#if 0
/*******************************************************************************
* 函数名称: IsCnntStyleOne
* 函数功能: 判断测量点是否是单相表
* 输入参数: Pn                  测量点号
* 输出参数: 无
* 返 回 值: TRUE                是单相表
*           FALSE               不是单相表
*******************************************************************************/
int IsCnntStyleOne(uint16 Pn,uint8 logicid, SCHEME_TASK_RUN_INFO* Tmp_AmrLink)
{

    uint32     TaskPeriod;
    uint32     TIPeriod;


    TaskPeriod = taskmng_task_period_get(Tmp_AmrLink->TaskId,logicid);
    TIPeriod = Tmp_AmrLink->AcqInfo.TI.value;
    switch (Tmp_AmrLink->AcqInfo.TI.unit)   //执行频率单位
    {
    case TASK_CYCLE_FLAG_SEC:       //秒
        break;

    case TASK_CYCLE_FLAG_MIN:       //分钟
        TIPeriod *= 60L;
        break;

    case TASK_CYCLE_FLAG_HOUR:      //小时
        TIPeriod *= 3600L;
        break;

    case TASK_CYCLE_FLAG_DAY:       //日
    case TASK_CYCLE_FLAG_MON:       //月
    case TASK_CYCLE_FLAG_YEAR:      //月
        TIPeriod = 86400L;
        break;

    default:
        TIPeriod = 86400L;
        break;
    }

    if(TaskPeriod == TIPeriod)      //周期与时标间隔相同时仍用实时转存
        return -2;

    if(taskmng_mtr_conntype_get(Pn,logicid) == 1)
        return 1;
    else
        return -1;
}
#endif

 /*******************************************************************************
* 函数名称: taskmng_pipe_get_from_tsa
* 函数功能: //根据tsa获取属于哪个端口
* 输入参数: tsa             电表地址

* 输出参数: 
  
* 返 回 值:  电表所属端口
*******************************************************************************/
uint8 taskmng_pipe_get_from_tsa(OOP_TSA_T tsa,uint8 logicid)
{
    uint16 i;
    uint8 pipe = 0xff;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        
        if(logicid ==MeterFileAll.MeterLogicid[i] && memcmp(tsa.add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 )
        {
        
            pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return pipe;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return pipe;
}

  /*******************************************************************************
 * 函数名称: taskmng_pipe_get_from_tsa
 * 函数功能: //根据tsa获取属于哪个端口
 * 输入参数: tsa             电表地址
 
 * 输出参数: 
   
 * 返 回 值:  电表所属端口
 *******************************************************************************/
 uint16 taskmng_get_tsa_count()
 {
     uint16 i =0;
     uint16 count =0;
     

     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0;i<MeterFileAll.MeterCount;i++)
     {
         
         if((0 ==MeterFileAll.MeterLogicid[i]) && (taskmng_addr_isvalid_check(MeterFileAll.MeterFile[i].extend.add.add,MeterFileAll.MeterFile[i].extend.add.len+1)==TRUE ))
         {
            count++;
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
     return count;
 }

 /*******************************************************************************
* 函数名称: taskmng_pipe_get_from_addr
* 函数功能: //根据地址获取属于哪个端口
* 输入参数: Addr             电表地址

* 输出参数: 
  
* 返 回 值:  电表所属端口
*******************************************************************************/
uint8 taskmng_pipe_get_from_addr(uint8 *Addr,uint8 logicid)
{
    uint16 i;
    uint8 pipe = 0xff;
    uint8 tmpadd[6];
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        memcpy_r(tmpadd,MeterFileAll.MeterFile[i].basic.tsa.add,6);
        if(memcmp(Addr,tmpadd,6)==0 &&MeterFileAll.MeterLogicid[i]==logicid )
        {
        
            pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return pipe;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return pipe;
}


   /*******************************************************************************
  * 函数名称: taskmng_pipe_get_from_addr
  * 函数功能: //根据地址获取属于哪个端口
  * 输入参数: Addr             电表地址
  
  * 输出参数: 
    
  * 返 回 值:  电表所属端口
  *******************************************************************************/
  uint8 taskmng_pipe_get_by_pn(uint16 Pn)
  {
      uint16 i;
      uint8 pipe = 0xff;

      pthread_rwlock_rdlock(&MeterFileAll.rwLock);
      for(i=0;i<MeterFileAll.MeterCount;i++)
      {
         
          if(Pn == MeterFileAll.MeterFile[i].nIndex)
          {
          
              pipe = taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port);
              pthread_rwlock_unlock(&MeterFileAll.rwLock);
              return pipe;
          }
      }
      pthread_rwlock_unlock(&MeterFileAll.rwLock);
      return pipe;
  }


  /*******************************************************************************
 * 函数名称: taskmng_pipe_get_from_addr
 * 函数功能: //根据地址获取属于哪个端口
 * 输入参数: Addr             电表地址
 
 * 输出参数: 
   
 * 返 回 值:  电表所属端口
 *******************************************************************************/
 BOOLEAN taskmng_check_caq_from_addr(uint8 *Addr,uint8 logicid)
 {
     uint16 i;
     uint8 tmpadd[6];
     pthread_rwlock_rdlock(&MeterFileAll.rwLock);
     for(i=0;i<MeterFileAll.MeterCount;i++)
     {
         memcpy_r(tmpadd,MeterFileAll.MeterFile[i].basic.tsa.add,6);
         if(memcmp(Addr,tmpadd,6)==0 &&MeterFileAll.MeterLogicid[i]==logicid )
         {
            if(taskmng_addr_isvalid_check(MeterFileAll.MeterFile[i].extend.add.add,MeterFileAll.MeterFile[i].extend.add.len+1)==TRUE )
            {
                 pthread_rwlock_unlock(&MeterFileAll.rwLock);
                 return TRUE;
            }
         }
     }
     pthread_rwlock_unlock(&MeterFileAll.rwLock);
     return FALSE;
 }

 /*******************************************************************************
* 函数名称: taskmng_baud_get_from_tsa
* 函数功能: //根据tsa获取属于该档案的波特率
* 输入参数: tsa             电表地址

* 输出参数: 
  
* 返 回 值:  波特率
*******************************************************************************/
uint8 taskmng_baud_get_from_tsa(OOP_TSA_T tsa,uint8 logicid)
{
    uint16 i;
    uint8 baud = 0xff;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tsa.add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 && logicid ==MeterFileAll.MeterLogicid[i])
        {
        
            baud =MeterFileAll.MeterFile[i].basic.baud ;
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return baud;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return baud;
}


 /*******************************************************************************
* 函数名称: taskmng_meter_check_from_pipe
* 函数功能: //查看指定的通道是否有电表信息
* 输入参数: pipe             端口号

* 输出参数: 
  
* 返 回 值:  TRUE有电表 FALSE没有
*******************************************************************************/
BOOL taskmng_meter_check_from_pipe(uint8 pipe)
{
    //uint16 i=0;
    AmrThreadArg_T* pAmrThreadArg;
    pAmrThreadArg = &gAmrThreadArg[pipe];
   
    if(pAmrThreadArg->TmnAmrInfo.MtrCnt==0)
    {
        if(pipe==5)
        {
            pAmrThreadArg->MaintainPipe =1;
        }
        return FALSE;
    }
    else
    {
        if((pipe==5)&&(pAmrThreadArg->MaintainPipe ==1))
        {
            pAmrThreadArg->MaintainPipe = 0;
            sleep(10);
        }

    }
    return TRUE;
    
}
 /*******************************************************************************
* 函数名称: taskmng_prtl_get_from_addr
* 函数功能: 根据电表地址获取规约类型
* 输入参数: Addr   电表地址

* 输出参数: prtl   规约类型
  
* 返 回 值:  
*******************************************************************************/
void taskmng_prtl_get_from_addr(uint8* Addr,uint8 logicid,uint8* prtl)
{
    uint16 i;
    uint8 tmpadd[6];
    memcpy_r(tmpadd,Addr,6);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tmpadd,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 && logicid ==MeterFileAll.MeterLogicid[i] )
        {
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

 /*******************************************************************************
* 函数名称: taskmng_prtl_get_from_tsa
* 函数功能: 根据tsa电表地址获取规约类型
* 输入参数: tsa   电表地址

* 输出参数: prtl   规约类型
  
* 返 回 值:  
*******************************************************************************/
void taskmng_prtl_get_from_tsa(OOP_TSA_T tsa,uint8 logicid,uint8* prtl)
{
    uint16 i;

    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tsa.add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 &&logicid ==MeterFileAll.MeterLogicid[i] )
        {
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

 /*******************************************************************************
* 函数名称: taskmng_pn_get_from_tsa
* 函数功能: 根据tsa电表地址获取PN
* 输入参数: tsa   电表地址

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 taskmng_pn_get_from_tsa(OOP_TSA_T tsa,uint8 logicid)
{
    uint16 i;
    uint16 Pn = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        //if(memcmp(tsa.add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 &&logicid ==MeterFileAll.MeterLogicid[i])
        if(memcmp(&tsa,&MeterFileAll.MeterFile[i].basic.tsa,sizeof(OOP_TSA_T))==0 &&logicid ==MeterFileAll.MeterLogicid[i])

        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}
 /*******************************************************************************
* 函数名称: taskmng_pn_get_from_addr
* 函数功能: 根据电表地址获取PN
* 输入参数: Addr   电表地址

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 taskmng_pn_get_from_addr(uint8* Addr,uint8 logicid)
{
    uint16 i;
    uint16 Pn = 0;
    uint8 tmpadd[6];
    memcpy_r(tmpadd,Addr,6);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tmpadd,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 &&logicid ==MeterFileAll.MeterLogicid[i])
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}

  /*******************************************************************************
 * 函数名称: taskmng_pn_get_from_addr
 * 函数功能: 根据电表地址获取PN
 * 输入参数: Addr   电表地址
 
 * 输出参数: 
   
 * 返 回 值: 电表测量点号
 *******************************************************************************/
 uint16 taskmng_pn_get_from_caj_addr(uint8* Addr)
 {
     uint16 i;
     uint16 Pn = 0;
     uint8 tmpadd[6];
     memcpy_r(tmpadd,Addr,6);

     for(i=0;i<Collector.count;i++)
     {
         if(memcmp(tmpadd,Collector.collectaddr[i].addr,6)==0)
         {
             Pn = Collector.collectaddr[i].pn;
             break;
         }
     }

     return Pn;
 }

 /*******************************************************************************
* 函数名称: taskmng_prtl_check
* 函数功能: //检查当前档案中是否有prtl对应的表
* 输入参数: prtl   规约类型

* 输出参数: 
  
* 返 回 值: TRUE 存在 FALSE 不存在
*******************************************************************************/
BOOL taskmng_prtl_check(uint8 prtl)
{
    uint16 i;
    BOOL ret= FALSE;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(prtl == MeterFileAll.MeterFile[i].basic.protocol)
        {
            ret = TRUE;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return ret;
}

 /*******************************************************************************
* 函数名称: taskmng_usertype_get_from_tsa
* 函数功能: 根据tsa电表地址获取用户类型
* 输入参数: tsa   电表地址

* 输出参数: 
  
* 返 回 值:  
*******************************************************************************/
uint8 taskmng_usertype_get_from_tsa(OOP_TSA_T tsa,uint8 logicid)
{
    uint16 i;
    uint8 usertype=0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tsa.add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 &&logicid ==MeterFileAll.MeterLogicid[i] )
        {
            usertype = MeterFileAll.MeterFile[i].basic.userType;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return usertype;
}

/*******************************************************************************
* 函数名称:	taskmng_usertype_get_from_no
* 函数功能:	根据序号获取电表用户类型
* 输入参数:	Pn             要检测的测量点

* 输出参数:	
  
* 返 回 值:	电表地址
*******************************************************************************/
uint8 taskmng_usertype_get_from_no(uint16 Pn,uint8 logicid)
{
    uint8 usertype = 0;
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0; i<MeterFileAll.MeterCount; i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid == MeterFileAll.MeterLogicid[i])
        {
            usertype = MeterFileAll.MeterFile[i].basic.userType;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return usertype;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_addr_check
* 函数功能: 判断新的档案是否和现有的重复
* 输入参数: 
            pMeterFileAll   //电表档案
           meterFile  待检查的档案
* 输出参数: 无
* 返 回 值:   TRUE 有重复的 FALSE 没有重复的
*******************************************************************************/
BOOL taskmng_meter_file_addr_check(Meter_File_T *pMeterFileAll,OOP_METER_T *meterFile)
{
    uint16 i;
    BOOL ret = FALSE;
    for(i=0;i<pMeterFileAll->MeterCount;i++)
    {
        if (memcmp(meterFile->basic.tsa.add,pMeterFileAll->MeterFile[i].basic.tsa.add,6) == 0 &&
            meterFile->basic.tsa.oldvxd == pMeterFileAll->MeterFile[i].basic.tsa.oldvxd &&
            meterFile->basic.tsa.flag == pMeterFileAll->MeterFile[i].basic.tsa.flag &&
            meterFile->basic.tsa.vxd == pMeterFileAll->MeterFile[i].basic.tsa.vxd
#ifdef AREA_FUJIAN
            && meterFile->basic.port.value == pMeterFileAll->MeterFile[i].basic.port.value
#endif
		)
        {
            ret = TRUE;
            break;
        }
    }
    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_init_with_logicid
* 函数功能: 电表档案初始化 带逻辑设备号
* 输入参数: 
            pMeterFileAll   //电表档案
           logicid 逻辑设备号
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
uint16 taskmng_meter_file_init_with_logicid(Meter_File_T *pMeterFileAll,uint8 logicid)
{
    uint16  MeterCount=0;
    uint16   MeterNum=0;     //档案个数
    int   ret;
    uint16 i;
    OOP_METER_T gMeterFile;     //每个电表的档案信息
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    TASK_FMT_DEBUG(-1,TASK_LOG, "开始档案初始化! logicid %d \n",logicid);
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    inReadRecord.recordOAD.logicId = logicid;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = Search_MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60000200;

    stepReadRecord.MAC.nNum = 0;
    ret = db_read_record_start(gDBClientMain, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读档案记录失败 ret  %d recNum %d logicid %d \n",ret,recNum,logicid);
    }
    else
    {
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读档案记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60000200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_METER_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读取的档案记录有问题 \n");
                continue;
            }
            memcpy(&gMeterFile,stepRecordOut.record[0].outDataBuf,sizeof(OOP_METER_T));
            if(FALSE==taskmng_prtl_isvalid_check(gMeterFile.basic.protocol))
            {
                continue;
            }
            if(taskmng_addr_isvalid_check(gMeterFile.basic.tsa.add,gMeterFile.basic.tsa.len+1)<0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "地址非法 \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.basic.tsa.add,6);
                continue;
            }
            TASK_FMT_DEBUG(-1,TASK_LOG, "sizeof(OOP_METER_T) %d 电表序号 %d ret = %d !\n",sizeof(OOP_METER_T),gMeterFile.nIndex,ret);
            TASK_FMT_DEBUG(-1,TASK_LOG, "port =%08x prtl = %d pwrType =%d \n",gMeterFile.basic.port.value,gMeterFile.basic.protocol,gMeterFile.basic.pwrType);
            TASK_FMT_DEBUG(-1,TASK_LOG, "地址 oldvxd %d vxd %d 0X%02x%02x%02x%02x%02x%02x \n",gMeterFile.basic.tsa.oldvxd,gMeterFile.basic.tsa.vxd,gMeterFile.basic.tsa.add[0],gMeterFile.basic.tsa.add[1],gMeterFile.basic.tsa.add[2],gMeterFile.basic.tsa.add[3],gMeterFile.basic.tsa.add[4],gMeterFile.basic.tsa.add[5]);
            if(TRUE==taskmng_meter_file_addr_check(pMeterFileAll,&gMeterFile))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "地址重复 过滤 \n");
                continue;
            }
            
            MeterNum=pMeterFileAll->MeterCount;
            pMeterFileAll->MeterFile[MeterNum]=gMeterFile;
            pMeterFileAll->MeterLogicid[MeterNum] = logicid;
            pMeterFileAll->MeterCount++;
       }
    }
    db_read_record_end(gDBClientMain, handle);

    MeterCount = pMeterFileAll->MeterCount;
    TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d  加载完成后档案个数为 %d \n",logicid,pMeterFileAll->MeterCount);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return MeterCount;

    
}


/*******************************************************************************
* 函数名称: taskmng_meter_file_init
* 函数功能: 电表档案初始化
* 输入参数: 
            pMeterFileAll   //电表档案
           档案地址TSA中统一使用大端
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/
uint16 taskmng_meter_file_init(Meter_File_T *pMeterFileAll)
{
    uint16  MeterCount=0;
    pMeterFileAll->MeterCount = 0 ;
    memset(pMeterFileAll->MeterFile,0x00,sizeof(OOP_METER_T)*MTR_MAX_NUM);
    memset(pMeterFileAll->MeterLogicid,0x00,sizeof(uint8)*MTR_MAX_NUM);
    MeterCount = taskmng_meter_file_init_with_logicid(pMeterFileAll,LOGIC_ID_YC);
    //MeterCount = taskmng_meter_file_init_with_logicid(pMeterFileAll,LOGIC_ID_PD);
    return MeterCount;
    
}
/*******************************************************************************
* 函数名称: taskmng_meter_file_delete_with_logicid
* 函数功能: 按照逻辑设备号删除电表档案
* 输入参数: 
            pMeterFileAll   //电表档案
           
* 输出参数: 无
* 返 回 值:   档案中剩余的电表个数
*******************************************************************************/
uint16 taskmng_meter_file_delete_with_logicid(Meter_File_T *pMeterFileAll,uint8 logicid)
{
    uint16 leftcount = 0;
    uint16 i;
    OOP_METER_T   leftmeter[MTR_MAX_NUM];
    uint8       leftlogicid[MTR_MAX_NUM] = {0};
    memset(leftmeter,0x00,sizeof(OOP_METER_T)*MTR_MAX_NUM);
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);
    if(logicid == 0xff)
    {
        pMeterFileAll->MeterCount = leftcount;
        memset(pMeterFileAll->MeterFile,0x00,sizeof(OOP_METER_T)*MTR_MAX_NUM);
        memset(pMeterFileAll->MeterLogicid,0x00,sizeof(uint8)*MTR_MAX_NUM);
    }else
    {
        for(i=0;i<pMeterFileAll->MeterCount;i++)
        {
            if(pMeterFileAll->MeterLogicid[i]!=logicid)
            {
                leftmeter[leftcount] = pMeterFileAll->MeterFile[i];
                leftlogicid[leftcount] = pMeterFileAll->MeterLogicid[i];
                leftcount++;
            }
        }
        pMeterFileAll->MeterCount = leftcount;
        memset(pMeterFileAll->MeterFile,0x00,sizeof(OOP_METER_T)*MTR_MAX_NUM);
        memset(pMeterFileAll->MeterLogicid,0x00,sizeof(uint8)*MTR_MAX_NUM);
        if(leftcount!=0)
        {
            memcpy(pMeterFileAll->MeterFile,leftmeter,sizeof(OOP_METER_T)*leftcount);
            memcpy(pMeterFileAll->MeterLogicid,leftlogicid,sizeof(uint8)*leftcount);
        }
    }
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);
    return leftcount;
}
/*******************************************************************************
* 函数名称: taskmng_meter_file_change_fun 
* 函数功能:档案变更的处理
* 输入参数: pMeterFileAll 电表档案
          choice 1 表示档案变更 新增或者改变
          choice 0 表示档案删除
          gMeterFile中nindex为删除的序号
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_meter_file_change_fun(Meter_File_T *pMeterFileAll,uint8 choice,OOP_METER_T gMeterFile,uint8 logicid)
{
    uint16 i;
    int index =-1;
    
    pthread_rwlock_wrlock(&pMeterFileAll->rwLock);

    TASK_FMT_DEBUG(-1,TASK_LOG, "进入档案修改流程 choice %d \n",choice);
    if(choice==0)
    {
        for(i=0;i<pMeterFileAll->MeterCount;i++)
        {
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex && pMeterFileAll->MeterLogicid[i] ==logicid )
            {
                index = i;
                memset(&pMeterFileAll->MeterFile[i],0x00,sizeof(OOP_METER_T));
                
                break;
            }
        }
        if(index!=-1)
        {
            for(i=index;i<pMeterFileAll->MeterCount-1;i++)
            {
                pMeterFileAll->MeterFile[i] = pMeterFileAll->MeterFile[i+1];
                pMeterFileAll->MeterLogicid[i] = pMeterFileAll->MeterLogicid[i+1];
            }
            memset(&(pMeterFileAll->MeterFile[pMeterFileAll->MeterCount-1]),0x00,sizeof(OOP_METER_T));
            pMeterFileAll->MeterLogicid[pMeterFileAll->MeterCount-1] = 0;
            pMeterFileAll->MeterCount--;
        }
        //taskmng_rs485_patch_file_delete_by_pn(pMeterFileAll->MeterFile[i].nIndex);
    }
    else
    {
        for(i=0;i<pMeterFileAll->MeterCount;i++)
        {
            //如果当前档案中找到相同的 则直接替换
            if(pMeterFileAll->MeterFile[i].nIndex == gMeterFile.nIndex && pMeterFileAll->MeterLogicid[i] ==logicid )
            {
                index = i;
                memcpy(&pMeterFileAll->MeterFile[i],&gMeterFile,sizeof(OOP_METER_T));
                //taskmng_rs485_patch_file_delete_by_pn(pMeterFileAll->MeterFile[i].nIndex);
                break;
            }
        }
        //如果没有找到 则新增
        if(index==-1)
        {
            if(TRUE==taskmng_meter_file_addr_check(pMeterFileAll,&gMeterFile))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "新添加的档案地址重复\n");

            }else
            {
                pMeterFileAll->MeterFile[pMeterFileAll->MeterCount] = gMeterFile;
                pMeterFileAll->MeterLogicid[pMeterFileAll->MeterCount] = logicid;
                pMeterFileAll->MeterCount++;
            }
        }
    }
    TASK_FMT_DEBUG(-1,TASK_LOG, "档案个数  %d \n",pMeterFileAll->MeterCount);
    pthread_rwlock_unlock(&pMeterFileAll->rwLock);

}



/*******************************************************************************
* 函数名称: taskmng_meter_file_ergodic
* 函数功能: 电表档案遍历
* 输入参数: 
            pMeterFileAll   //电表档案
           
* 输出参数: 无
* 返 回 值:   无
*******************************************************************************/

void taskmng_meter_file_ergodic(Meter_File_T *pMeterFileAll)
{
    time_t RealTime_t;
    uint8 i;
    uint16 MeterCount=0;
    int ret;
    OOP_METER_T gMeterFile;
    static uint8 MeterFileChangeflag;
    static uint8 MeterChangeflag;
    uint16 leftcount=0;
#ifdef AREA_ZHEJIANG
    static BOOL lastCheckResult = TRUE;
    BOOL checkResult;
#endif
    //OOP_OAD_U oad = {0xF1010200};
    switch(pMeterFileAll->FileState)
    {
    case TASK_CFG_UNINIT:
        MeterCount = taskmng_meter_file_init(pMeterFileAll);
        if(MeterCount==0)
        {   
            TASK_FMT_DEBUG(-1,TASK_LOG, "档案读取个数为0!\n");
            //break;
        }
        //读测量点信息
        taskmng_amr_info_read();
        taskmng_speed_list_get();
        MeterFileFlag = METER_FILE_NULL;
        
        pMeterFileAll->FileState = TASK_CFG_INITED;
        break;
    case TASK_CFG_MODIFY:
        //收到档案变更消息后进行处理
#ifdef AREA_ZHEJIANG
        gMeterfileSyncFlag = 1;

#endif
        //0xff表示所有档案删除
        if(dbupdateMeter.infonum == 0xffff)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "logicid %d 档案清空 !\n",dbupdateMeter.logicid);
            leftcount = taskmng_meter_file_delete_with_logicid(pMeterFileAll,dbupdateMeter.logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "剩余档案个数 %d \n",leftcount);
            MeterFileChangeflag = METER_FILE_CLEAR;
            if(leftcount == 0)
            {
                for(i=0;i<PLC_NUM;i++)
                {
                    task_send_msg(gMainSock,MSG_METERFILE_CLEAR,(uint8*)&dbupdateMeter.infonum,2,IPC_AMR+i);
                    TASK_FMT_DEBUG(-1,TASK_LOG, "通知抄表线程档案初始化\n");
                }
            }

            for(i=0;i<SchemeTaskCfgList.TaskNum;i++)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "删除logicid %d 任务%d记录文件\n",dbupdateMeter.logicid,SchemeTaskCfgList.OopTaskCfg[i].TaskId);
                taskmng_task_record_file_delete(SchemeTaskCfgList.OopTaskCfg[i].TaskId,dbupdateMeter.logicid);
            }
            //taskmng_rs485_delete_all_patch_file();
        }else
        {
            memset(&gMeterFile,0x00,sizeof(OOP_METER_T));
            ret = taskmng_meter_file_read_from_pn(dbupdateMeter.infonum,&gMeterFile,dbupdateMeter.logicid);
            TASK_FMT_DEBUG(-1,TASK_LOG, "读取档案%d logicid %d ret %d !\n",dbupdateMeter.infonum,dbupdateMeter.logicid,ret);
            if(ret==0)
            {
                //测点删除时同时删除数据中心中安全模式
                //taskmng_normal_data_delete(dbupdateMeter.infonum,oad);
                //读取到档案信息 表示档案变更或者新增
                TASK_FMT_DEBUG(-1,TASK_LOG, "档案地址 \n");
                TASK_BUF_DEBUG(-1,TASK_LOG, gMeterFile.basic.tsa.add,6);
                if(TRUE==taskmng_prtl_isvalid_check(gMeterFile.basic.protocol)&&taskmng_addr_isvalid_check(gMeterFile.basic.tsa.add,gMeterFile.basic.tsa.len+1)>0)
                {
                    taskmng_meter_file_change_fun(pMeterFileAll,1,gMeterFile,dbupdateMeter.logicid);
#ifdef AREA_ZHEJIANG
                    checkResult = taskmng_meter_file_in_search_result_check(&gMeterFile);
                    if(checkResult == TRUE && lastCheckResult == TRUE)
                    {
                        gMeterfileSyncFlag = 0;
                    }
                    lastCheckResult = checkResult;
#endif
                    MeterChangeflag = 0xAA;//0xAA表示有合法的档案变更
                }
                else
                {
                    if(MeterChangeflag != 0xAA)
                    {
                        TASK_FMT_DEBUG(-1,TASK_LOG, "档案error  protocol %d !\n",gMeterFile.basic.protocol);
                        pMeterFileAll->FileState = TASK_CFG_INITED;
                        break;
                    }
                }
            }
            else
            {
                //未读取到档案信息，表示档案删除
                gMeterFile.nIndex = dbupdateMeter.infonum;
                taskmng_meter_file_change_fun(pMeterFileAll,0,gMeterFile,dbupdateMeter.logicid);
            }
            MeterFileChangeflag = METER_FILE_MODIFY;
        }
        //进入30秒延迟 预防多次修改档案
        pMeterFileAll->ModifyDelaySec = 5; //原来30秒，现在先给缩短到5秒
        RealTime_t = time(NULL);
        pMeterFileAll->FunLastRunTime = RealTime_t;
        pMeterFileAll->FileState = TASK_CFG_MODIFY_DELAY;
        break;
    case TASK_CFG_MODIFY_DELAY:

        MeterChangeflag = 0xBB;
        RealTime_t = time(NULL);
        if(RealTime_t!=pMeterFileAll->FunLastRunTime)
        {   
            pMeterFileAll->ModifyDelaySec--;
            pMeterFileAll->FunLastRunTime=RealTime_t;
            TASK_FMT_DEBUG(-1,TASK_LOG, "延时%d \n",pMeterFileAll->ModifyDelaySec);
        }
        //延时过后 通知各抄表线程档案变更
        if(pMeterFileAll->ModifyDelaySec<=0)
        {
            TASK_FMT_DEBUG(-1,TASK_LOG, "5秒延时过后通知各线程更新!\n");
            taskmng_amr_info_read();
            taskmng_speed_list_get();

#ifdef AREA_ZHEJIANG
            if(gMeterfileSyncFlag == 1)
            {
#endif
            for(i=0;i<AMR_THREAD_NUM;i++)
            {
                gAmrThreadArg[i].MtrParaModifyFlag = MeterFileChangeflag;
            }
            
#ifdef AREA_ZHEJIANG
            }
            lastCheckResult = TRUE;
#endif
            
            pMeterFileAll->FileState = TASK_CFG_INITED;
            //SchemeTaskCfgList.TaskCfgState = TASK_CFG_UNINIT;//档案变更后任务重新初始化
            ReloadTaskFlag = 1;

        }
        break;
    default:
        break;   
    }
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_read_from_pn
* 函数功能: 根据信息点号从数据中心读取电表档案
* 输入参数: 
            infonum
           
* 输出参数: gMeterFile
* 返 回 值:  0 成功

*******************************************************************************/
int taskmng_meter_file_read_from_pn(uint16 infonum,OOP_METER_T *gMeterFile,uint8 logicid)
{
    int   ret;
    uint32 len=0;
    NOMAL_OAD_T NormalOad;
    NormalOad.logicId = logicid;
    NormalOad.oad.value = 0x60000200;
    NormalOad.infoNum = infonum;

    ret = db_read_nomal(gDBClientMain,&NormalOad,sizeof(OOP_METER_T),(uint8*)gMeterFile,&len);

    if(ret !=0 ||len!=sizeof(OOP_METER_T))
        return -1 ;
    
    return 0;
}

/*******************************************************************************
* 函数名称: taskmng_meter_search_result_read 
* 函数功能: //上电时加载数据中心中的搜表结果并按照端口号加载到各个线程中 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void taskmng_meter_search_result_read(void)
{
    int ret;
    READ_RECORD_T      inReadRecord;
    uint32             handle = 0;
    uint32             recNum = 0;
    int i;
    uint8 pipe = 0;

    STEP_READ_RECORD_T stepReadRecord;
    RECORD_UNIT_T      stepRecordOut;
    OOP_SEARCHMET_RES_T  SearchResult;
    
    memset(&SearchResult,0x00,sizeof(OOP_SEARCHMET_RES_T));
    memset(&inReadRecord, 0, sizeof(READ_RECORD_T));
    memset(&stepReadRecord, 0, sizeof(STEP_READ_RECORD_T));
    memset(&stepRecordOut, 0, sizeof(RECORD_UNIT_T));
    
    TASK_FMT_DEBUG(-1,TASK_LOG, "开始加载搜表结果! \n");


    inReadRecord.recordOAD.logicId = 0;
    inReadRecord.recordOAD.infoNum = 0xffff;
    inReadRecord.cType = COL_NO_INFO;
    inReadRecord.cStart = 1;
    inReadRecord.cEnd = MTR_MAX_NUM;
    inReadRecord.sortType = UP_SORT;
    inReadRecord.recordOAD.road.oadMain.value = 0;
    inReadRecord.recordOAD.road.oadCols.nNum = 0x01;
    inReadRecord.recordOAD.road.oadCols.oad[0].value = 0x60020200;

    stepReadRecord.MAC.nNum = 0;
    pthread_mutex_lock(&MeterSearchResult.lock);
    
    ret = db_read_record_start(gDBClientMain, &inReadRecord, &handle, &recNum);
    if(ret!=0 || recNum==0)
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "开始读搜表结果记录失败 ret  %d recNum %d \n",ret,recNum);
    }else
    {
        TASK_FMT_DEBUG(-1,TASK_LOG, "搜表结果记录 recNum %d \n",recNum);
        for(i=0;i<recNum;i++)
        {
            ret = db_read_record_step(gDBClientMain, handle, &stepReadRecord, &stepRecordOut);
            if(ret!=0)
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读搜表结果记录失败 ret  %d i %d \n",ret,i);
                continue;
            }
            if(stepRecordOut.recordnum!=1 || stepRecordOut.record[0].road.oadCols.oad[0].value!=0x60020200
                ||stepRecordOut.record[0].outDatalen!=sizeof(OOP_SEARCHMET_RES_T))
            {
                TASK_FMT_DEBUG(-1,TASK_LOG, "单步读取的搜表结果记录有问题 \n");
                continue;
            }
            memcpy(&SearchResult,stepRecordOut.record[0].outDataBuf,sizeof(OOP_SEARCHMET_RES_T));
            //TASK_FMT_DEBUG(-1,TASK_LOG, "i %d oad %08x pipe %08x  \n",i,SearchResult.info[0].oad.value,SearchResult.info[0].value.oad.value);
#ifndef AREA_FUJIAN
            if(SearchResult.info[0].oad.value != 0x60060200)
            {
                continue;
            }
            pipe = taskmng_mtr_pipe_get(&SearchResult.info[0].value.oad);
            if(pipe>AMR_THREAD_NUM)
            {
                continue;
            }
#endif
            if(stepRecordOut.infoNum>MeterSearchResult.infonum)
            {
                MeterSearchResult.infonum = stepRecordOut.infoNum;
            }
            MeterSearchResult.result[MeterSearchResult.count].infonum = stepRecordOut.infoNum;
            MeterSearchResult.result[MeterSearchResult.count].prtl = SearchResult.proto;
            MeterSearchResult.result[MeterSearchResult.count].pipe = pipe;
            memcpy_r(MeterSearchResult.result[MeterSearchResult.count].Addr,SearchResult.commAdd.add,6);
            MeterSearchResult.count++;

            TASK_FMT_DEBUG(-1,TASK_LOG, "加载搜表结果 pipe %d MeterCount %d  \n",pipe,MeterSearchResult.count);
            TASK_FMT_DEBUG(-1,TASK_LOG,"prtl %d addr 0x%02x%02x%02x%02x%02x%02x \n",SearchResult.proto,SearchResult.commAdd.add[0],
                SearchResult.commAdd.add[1],SearchResult.commAdd.add[2],SearchResult.commAdd.add[3],SearchResult.commAdd.add[4],SearchResult.commAdd.add[5]);

       }
    }
    TASK_FMT_DEBUG(-1,TASK_LOG, "搜表结果当前infonum %d  \n",MeterSearchResult.infonum);
    db_read_record_end(gDBClientMain, handle);
    pthread_mutex_unlock(&MeterSearchResult.lock);
}

/*******************************************************************************
* 函数名称: taskmng_meter_file_update_check 
* 函数功能: 检查是否收到档案更新的消息 
* 输入参数: 
* 输出参数: 档案的更新消息
* 返 回 值: 
*******************************************************************************/
void taskmng_meter_file_update_check(DB_UPDATE_T *dbupdate)
{
    int Recvlen = 0;

    Recvlen = taskmng_db_update_queue_read(&pMeterFileUpdate, dbupdate);
    if(Recvlen>0)
    {

        TASK_FMT_DEBUG(-1,TASK_LOG, "收到mqtt发送的更新消息 infonum %d oad %08x !\n",dbupdate->infonum,dbupdate->oad);
        MeterFileAll.FileState = TASK_CFG_MODIFY;
    }
}

 /*******************************************************************************
* 函数名称: taskmng_mtr_prtl_get_new
* 函数功能: 根据序号获取电表规约类型 判断附加属性中是否有40360200 如果有则判断为20版/物联网表
* 输入参数: Pn             要检测的测量点

* 输出参数: 
        prtl        规约类型
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_prtl_get_new(uint16 Pn,uint8 logicid,uint8 *prtl)
{
    uint16 i;
    OOP_METER_ANNEX_T *annex=NULL;
    uint8 j;
    uint16 value = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i] )
        {
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            //判断是否为物联网表
            if(*prtl == PRTL_OOP)
            {
                annex = &MeterFileAll.MeterFile[i].annex;
                for(j=0;j<annex->nNum;j++)
                {
                    if (annex->annexObj[j].oad.value == 0x40360200)
                    {
                        if (annex->annexObj[j].nLen == 2)
                        {
                            memcpy(&value,annex->annexObj[j].value,2);

                            if ((value & 0xff) == 1 && ((value >> 8) & 0xFF) == 1)
                            {
                                *prtl = PRTL_20_IOT;
                                break;
                            }
                            //0x0100 698协议的20表 也认为是物联表
                            if ((value & 0xff) == 0 && ((value >> 8) & 0xFF) == 1)
                            {
                                *prtl = PRTL_20_IOT;
                                break;
                            }
                        }
                    }
                }
            }
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

 /*******************************************************************************
* 函数名称: taskmng_mtr_addr_prtl_get_new
* 函数功能: 根据序号获取电表地址和规约类型
* 输入参数: Pn             要检测的测量点

* 输出参数: MtrAddr        电表地址
        prtl        规约类型
* 返 回 值:  
*******************************************************************************/
void taskmng_mtr_addr_prtl_get_new(uint16 Pn,uint8 logicid,uint8 *MtrAddr,uint8 *prtl)
{
    uint16 i;
    OOP_METER_ANNEX_T *annex=NULL;
    uint32 j;
    uint16 value = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(Pn == MeterFileAll.MeterFile[i].nIndex && logicid ==MeterFileAll.MeterLogicid[i] )
        {
            memcpy_r(MtrAddr,MeterFileAll.MeterFile[i].basic.tsa.add,6);
            *prtl = MeterFileAll.MeterFile[i].basic.protocol;
            //判断是否为物联网表
            if(*prtl == PRTL_OOP)
            {
                annex = &MeterFileAll.MeterFile[i].annex;
                for(j=0;j<annex->nNum;j++)
                {
                    if (annex->annexObj[j].oad.value == 0x40360200)
                    {
                        if (annex->annexObj[j].nLen == 2)
                        {
                            memcpy(&value,annex->annexObj[j].value,2);

                            if ((value & 0xff) == 1 && ((value >> 8) & 0xFF) == 1)
                            {
                                *prtl = PRTL_20_IOT;
                                break;
                            }
                            //0x0100 698协议的20表 也认为是物联表
                            if ((value & 0xff) == 0 && ((value >> 8) & 0xFF) == 1)
                            {
                                *prtl = PRTL_20_IOT;
                                break;
                            }
                        }
                    }
                }
            }
            pthread_rwlock_unlock(&MeterFileAll.rwLock);
            return;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}


 /*******************************************************************************
* 函数名称: taskmng_iot_prtl_check_new
* 函数功能: 检查有没有物联网表档案
* 输入参数: 

* 输出参数: 
        
* 返 回 值:  
*******************************************************************************/
BOOL taskmng_iot_prtl_check(uint8 pipe)
{
    uint16 i;
    OOP_METER_ANNEX_T *annex=NULL;
    uint32 j;
    uint16 value = 0;
    uint8 prtl ; 
    OOP_OAD_U pipeoad = taskmng_pipe_oad_get( pipe);
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {

        prtl = MeterFileAll.MeterFile[i].basic.protocol;
        //判断是否为物联网表
        if(prtl == PRTL_OOP && pipeoad.value == MeterFileAll.MeterFile[i].basic.port.value)
        {
            annex = &MeterFileAll.MeterFile[i].annex;
            for(j=0;j<annex->nNum;j++)
            {
                if (annex->annexObj[j].oad.value == 0x40360200)
                {
                    //存的是报文 第一个字节类型 后面两个字节数据
                    if (annex->annexObj[j].nLen == 2)
                    {
                        //value = *(uint16 *)&annex->annexObj[j].value[1];
                        memcpy(&value,annex->annexObj[j].value,2);
                        if ((value & 0xff) == 1 && ((value >> 8) & 0xFF) == 1)
                        {
                            pthread_rwlock_unlock(&MeterFileAll.rwLock);
                            return TRUE;
                        }
                    }
                }
            }
        }

    
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return FALSE;
}

 /*******************************************************************************
* 函数名称: taskmng_pn_get_from_tsa
* 函数功能: 根据tsa电表地址获取PN
* 输入参数: tsa   电表地址

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 taskmng_pn_get_from_tsa_new(OOP_TSA_T *tsa)
{
    uint16 i;
    uint16 Pn = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tsa,&MeterFileAll.MeterFile[i].basic.tsa,sizeof(OOP_TSA_T))==0 )
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}

void taskmng_speed_list_add(uint8 speed)
{
    uint8 i;
    if(speed == 0xff)
        return;
    for(i=0;i<gSpeedList.num;i++)
    {
        if(gSpeedList.speed[i] == speed)
        {
            return;
        }
    }
    gSpeedList.speed[gSpeedList.num] = speed;
    gSpeedList.num++;
    TASK_FMT_DEBUG(-1, TASK_LOG,"波特率统计 %d  %d \n",gSpeedList.num,speed);
}

 /*******************************************************************************
* 函数名称: taskmng_speed_list_get
* 函数功能: 遍历485表获取所有波特率
* 输入参数: 

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
void taskmng_speed_list_get()
{
    gSpeedList.num = 0;
    //0也是有效的波特率
    memset(gSpeedList.speed,0xff,sizeof(gSpeedList.speed));
    uint16 i;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(MeterFileAll.MeterFile[i].basic.port.nObjID == 0xF201)
        {
            taskmng_speed_list_add(MeterFileAll.MeterFile[i].basic.baud);
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
}

#ifdef AREA_FUJIAN
 /*******************************************************************************
* 函数名称: taskmng_pn_get_from_tsa_pipe
* 函数功能: 根据tsa电表地址获取PN
* 输入参数: tsa   电表地址

* 输出参数: 
  
* 返 回 值: 电表测量点号
*******************************************************************************/
uint16 taskmng_pn_get_from_tsa_pipe(uint8 pipe,OOP_TSA_T *tsa)
{
    uint16 i;
    uint16 Pn = 0;
    pthread_rwlock_rdlock(&MeterFileAll.rwLock);
    for(i=0;i<MeterFileAll.MeterCount;i++)
    {
        if(memcmp(tsa->add,MeterFileAll.MeterFile[i].basic.tsa.add,6)==0 
                && pipe == taskmng_mtr_pipe_get(&MeterFileAll.MeterFile[i].basic.port))
        {
            Pn = MeterFileAll.MeterFile[i].nIndex;
            break;
        }
    }
    pthread_rwlock_unlock(&MeterFileAll.rwLock);
    return Pn;
}
#endif


