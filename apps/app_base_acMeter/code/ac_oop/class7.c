/*
*********************************************************************
* @file    class7.c
* @brief： class7处理
* @author：
* @date：   2019.12.06
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/
#include "baseCommon.h"
#include "oadInfoList.h"
#include "commClass.h"
#include "oopservice.h"
#include "ac_sample.h"
#include "ac_main.h"
#include "oopStorage.h"

//
// class7
//

const OOP_OAD_U g_evt_300f_obj[]= {//电能表电压逆相序事件
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
    {0x33000200},//通道上报状态
    
    {0x00102201},//事件发生时刻正向有功总电能
    {0x00202201},//事件发生时刻反向有功总电能
    {0x00302201},//事件发生时刻组合无功1总电能
    {0x00402201},//事件发生时刻组合无功2总电能
    {0x00112201},//事件发生时刻A相正向有功电能
    {0x00212201},//事件发生时刻A相反向有功电能
    {0x00312201},//事件发生时刻A相组合无功1电能
    {0x00412201},//事件发生时刻A相组合无功2电能
    {0x00122201},//事件发生时刻B相正向有功电能
    {0x00222201},//事件发生时刻B相反向有功电能
    {0x00322201},//事件发生时刻B相组合无功1电能
    {0x00422201},//事件发生时刻B相组合无功2电能
    {0x00132201},//事件发生时刻C相正向有功电能
    {0x00232201},//事件发生时刻C相反向有功电能
    {0x00332201},//事件发生时刻C相组合无功1电能
    {0x00432201},//事件发生时刻C相组合无功2电能
    {0x00108201},//事件结束后正向有功总电能
    {0x00208201},//事件结束后反向有功总电能
    {0x00308201},//事件结束后组合无功1总电能
    {0x00408201},//事件结束后组合无功2总电能
    {0x00118201},//事件结束后A相正向有功电能
    {0x00218201},//事件结束后A相反向有功电能
    {0x00318201},//事件结束后A相组合无功1电能
    {0x00418201},//事件结束后A相组合无功2电能
    {0x00128201},//事件结束后B相正向有功电能
    {0x00228201},//事件结束后B相反向有功电能
    {0x00328201},//事件结束后B相组合无功1电能
    {0x00428201},//事件结束后B相组合无功2电能
    {0x00138201},//事件结束后C相正向有功电能
    {0x00238201},//事件结束后C相反向有功电能
    {0x00338201},//事件结束后C相组合无功1电能
    {0x00438201},//事件结束后C相组合无功2电能
};

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文（临时函数）
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_oad(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
//#ifdef AREA_HUNAN
//    return data_to_buf_evt_oad_hunan( oad, dstBuf, dstLen, pOffset, srcData, srcLen);
//#endif 
    int32 nRet = ERR_NORMAL;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    uint32 offset = *pOffset;
    uint8 i = 0;
    OOP_DAR_E dar = DATA_SUCCESS;

    switch(oad.value)
    {
        case 0x810c2200: 
        {
            OOP_CFGUNIT_810C_T data;
            if(srcLen != sizeof(OOP_CFGUNIT_810C_T))
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. srcLen(%d)!=OOP_CFGUNIT_810C_T(%d)\n", srcLen, sizeof(OOP_CFGUNIT_810C_T));
                return DATA_TYPE_UNMATCHED;
            }
            memcpy(&data, srcData, sizeof(data));

            //结构类型
            dstBufTmp[offset++] = DT_STRUCTURE;

            //结构成员数
            dstBufTmp[offset++] = 8;

            dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen, &offset, &data.object, sizeof(uint16));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),object(%d)\n", dar, data.object);
                return DATA_TYPE_UNMATCHED;
            }  

            dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen, &offset, &data.number, sizeof(uint32));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.number);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.flag, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.type, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.flag);
                return DATA_TYPE_UNMATCHED;
            }

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &data.purElectricity, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.purElectricity);
                return DATA_TYPE_UNMATCHED;
            } 

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &data.alarmLimit, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
                return DATA_TYPE_UNMATCHED;
            }  

            dar = basic_data_to_buf(E_LONG64, dstBufTmp, dstLen, &offset, &data.tripLimit, sizeof(int64));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),limit(%ll)\n", dar, data.alarmLimit);
                return DATA_TYPE_UNMATCHED;
            } 

            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen, &offset, &data.mode, sizeof(uint8));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d),delay(%d)\n", dar, data.mode);
                return DATA_TYPE_UNMATCHED;
            }
             
        }break;    
        case 0x43002302:   //事件发生前软件版本号
        case 0x43004302:   //事件发生后软件版本号
        {
            OOP_VERSION_T version;
            memcpy(&version.szSoftVer, srcData, srcLen);
            
            dstBufTmp[offset++] = DT_VISIBLE_STRING;
            dstBufTmp[offset++] = 4;
            memcpy(&dstBufTmp[offset], version.szSoftVer.value, 4);    
            offset += 4;  

        }break;
        
        case 0x60412200:  //事件发生前最近一次抄表成功时间
        case 0x201E4200:  //事件发生时间
        case 0x40002200:  //对时前时间
        case 0x40008200:  //对时后时间        
        {
            if(basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, srcData, srcLen) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
        }break;
    
        case 0xF2034201:  //第1路状态变位发生后状态
        case 0xF2034202:  //第2路状态变位发生后状态
        case 0xF2034203:  //第3路状态变位发生后状态
        case 0xF2034204:  //第4路状态变位发生后状态   
        case 0xF2034205:  //第5路状态变位发生后状态
        case 0xF2034206:  //第6路状态变位发生后状态
        case 0xF2034207:  //第7路状态变位发生后状态
        case 0xF2034208:  //第8路状态变位发生后状态
#ifdef PRODUCT_ZCU_1
        case 0xF2034209:  //第9路状态变位发生后状态
        case 0xF203420A:  //第10路状态变位发生后状态
        case 0xF203420B:  //第11路状态变位发生后状态
        case 0xF203420C:  //第12路状态变位发生后状态   
        case 0xF203420D:  //第13路状态变位发生后状态
        case 0xF203420E:  //第14路状态变位发生后状态
        case 0xF203420F:  //第15路状态变位发生后状态
        case 0xF2034210:  //第16路状态变位发生后状态
#endif
        {
            INPUTSTATE swstate;
            memcpy(&swstate, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_STRUCTURE;
    
            //结构成员数
            dstBufTmp[offset++] = 2; 
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.st, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
            
            if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &swstate.cd, sizeof(uint8)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            }
  
        }break;
    
        case 0x22004202:   //事件发生后，当月已发生通信流量
        case 0x31100601:   //月通信流量门限
        {
            if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, srcData, sizeof(uint32)) < 0)
            {
                nRet = ERR_NORMAL;
                return nRet;
            } 

        }break;
    
        case 0x20034200:   //事件发生时电压电流相角
        {
            OOP_WORD3V_T var; 
            memcpy(&var, srcData, srcLen);
    
            dstBufTmp[offset++] = DT_ARRAY;
    
            //数组成员个数
            dstBufTmp[offset++] = var.nNum;
    
            for(i = 0; i < var.nNum; i++)
            {
                if(basic_data_to_buf(E_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &var.nValue[i], sizeof(uint16)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break;  
    
        case 0x00102201:  //事件发生前的正向有功总电能
        case 0x00112201:  //事件发生前的A相正向有功电能 
        case 0x00122201:  //事件发生前的B相正向有功电能        
        case 0x00132201:  //事件发生前的C相正向有功电能        
        
        case 0x00104201:  //事件发生时的正向有功总电能
        
        case 0x00202201:  //事件发生前的反向有功总电能
        case 0x00212201:  //事件发生前的A相反向有功电能 
        case 0x00222201:  //事件发生前的B相反向有功电能        
        case 0x00232201:  //事件发生前的C相反向有功电能  
        
        case 0x00108201:  //事件结束后的正向有功总电能  
        case 0x00118201:  //事件结束后的A相正向有功电能 
        case 0x00128201:  //事件结束后的B相正向有功电能        
        case 0x00138201:  //事件结束后的C相正向有功电能     

        case 0x00208201:  //事件结束后的反向有功总电能  
        case 0x00218201:  //事件结束后的A相反向有功电能 
        case 0x00228201:  //事件结束后的B相反向有功电能        
        case 0x00238201:  //事件结束后的C相反向有功电能         
        {
            uint32 energys;
            
            if(srcLen != sizeof(uint32))
            {
                ACUDP_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(uint32));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(uint32)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        
        case 0x00302201:  //事件发生前的组合无功1总电能 
        case 0x00312201:  //事件发生前的A相组合无功1电能 
        case 0x00322201:  //事件发生前的B相组合无功1电能        
        case 0x00332201:  //事件发生前的C相组合无功1电能  
        
        case 0x00402201:  //事件发生前的组合无功2总电能  
        case 0x00412201:  //事件发生前的A相组合无功2电能 
        case 0x00422201:  //事件发生前的B相组合无功2电能        
        case 0x00432201:  //事件发生前的C相组合无功2电能  

        case 0x00308201:  //事件结束后的组合无功1总电能  
        case 0x00318201:  //事件结束后的A相组合无功1电能 
        case 0x00328201:  //事件结束后的B相组合无功1电能        
        case 0x00338201:  //事件结束后的C相组合无功1电能     

        case 0x00408201:  //事件结束后的组合无功2总电能  
        case 0x00418201:  //事件结束后的A相组合无功2电能 
        case 0x00428201:  //事件结束后的B相组合无功2电能        
        case 0x00438201:  //事件结束后的C相组合无功2电能         
        {
            int energys;

            if(srcLen != sizeof(int))
            {
                ACUDP_FMT_DEBUG("srcLen check failed. srcLen(%d) != %d \n", srcLen, sizeof(int));
                return ERR_NORMAL;
            }
            
            memcpy(&energys, srcData, srcLen);  

            if(energys == 0) //TODO 姑且使用0作为不存在的判据
            {
                dstBufTmp[offset++] = DT_NULL;
            }
            else
            {
                if(basic_data_to_buf(E_DOUBLE_LONG, dstBufTmp, dstLen-offset, &offset, &energys, sizeof(int)) < 0)
                {
                    nRet = ERR_NORMAL;
                    return nRet;
                }
            }
        }break; 
        default: nRet = ERR_NORMAL; break;
    }

    *pOffset = offset;
    
    return ERR_OK;
}


/**
*********************************************************************
* @brief：      事件上报状态，子结构体拼装报文
* @param[in] ： srcBuf     源数据区，通常是一个变量地址
*               srcLen     源数据长度，通常是变量大小
*               dstLen     目的数据长度，698报文的buffer长度

* @param[out] ：pOffset     构造该数据后的目的数据区偏移
*               dstData    目的数据区，698报文buffer
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_report(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_REPORTS_T evtreport;
    uint8 offlen = 0; //数组数量长度
    uint8 lenBuf[3] = {0}; //数组数量编码
    uint8 i = 0;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&evtreport, srcData, srcLen);

    //数组类型
    dstBufTmp[offset++] = DT_ARRAY;

    //数组成员个数
    offlen = set_len_offset(evtreport.nNum, lenBuf);
    memcpy(&dstBufTmp[offset], lenBuf, offlen);
    offset += offlen;

    //数组成员
    for(i = 0; i < evtreport.nNum; i++)
    {
        //结构类型
        dstBufTmp[offset++] = DT_STRUCTURE;

        //结构成员个数
        dstBufTmp[offset++] = 2;

        //OAD
        dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &evtreport.state[i].oad, sizeof(OOP_OAD_U));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), oad(0x%08x)\n", i, dar, evtreport.state[i].oad.value);
            return dar;
        }      

        //上报状态
        dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtreport.state[i].state, sizeof(uint8));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("basic_data_to_buf failed. [%d] dar(%d), state(0x%08x)\n", i, dar, evtreport.state[i].state);
            return dar;
        }         
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      事件发生源，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_source(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_EVTSOURCE_T evtsrc;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    memcpy(&evtsrc, srcData, srcLen);

    switch(evtsrc.choice)
    {
        case DT_OAD: 
        {
            dar = basic_data_to_buf(E_OAD, dstBufTmp, dstLen-offset, &offset, &evtsrc.oad, sizeof(evtsrc.oad));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), oad(0x%08x)\n", dar, evtsrc.oad.value);
                return dar;
            }
        }break;

        case DT_TSA: 
        {
            dar = basic_data_to_buf(E_TSA, dstBufTmp, dstLen-offset, &offset, &evtsrc.add, sizeof(evtsrc.add));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), add.af(%d)\n", dar, evtsrc.add.af);
                return dar;
            }
        }break;  

        case DT_OI: 
        {
            dar = basic_data_to_buf(E_OI, dstBufTmp, dstLen-offset, &offset, &evtsrc.oi, sizeof(evtsrc.oi));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), oi(%d)\n", dar, evtsrc.oi);
                return dar;
            }
        }break;   

         case DT_UNSIGNED: 
        {
            dar = basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break; 

        case DT_NULL:
        {
            dar = basic_data_to_buf(E_NULL, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break;
            
        case DT_ENUM:
        {
            dar = basic_data_to_buf(E_ENUM, dstBufTmp, dstLen-offset, &offset, &evtsrc.nValue, sizeof(evtsrc.nValue));
            if(dar != DATA_SUCCESS)
            {
                ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nValue(%d)\n", dar, evtsrc.nValue);
                return dar;
            }
        }break;

        default: 
        {
            ACUDP_FMT_DEBUG("data_to_buf_evt_source check type failed. choice(%d)\n", evtsrc.choice);
            return DATA_TYPE_UNMATCHED;
        }
        break;
    }
    
    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
* @brief：      事件发生源，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_basic(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    EVT_BASIC_T evtBasic;
    uint8 *dstBufTmp = (uint8 *)dstBuf; 
    
    memcpy(&evtBasic, srcData, srcLen);
    
    /*事件记录序号*/
    dar = basic_data_to_buf(E_DOUBLE_LONG_UNSIGNED, dstBufTmp, dstLen-offset, &offset, &evtBasic.nIndex, sizeof(uint32));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), nIndex(%d)\n", dar, evtBasic.nIndex);
        return dar;
    }
    
    //事件发生时间
    dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtBasic.tmStart, sizeof(OOP_DATETIME_S_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), cStart(%04d-%02d-%02d %02d:%02d:%02d)\n", 
            dar, evtBasic.tmStart.year, evtBasic.tmStart.month, evtBasic.tmStart.day, 
            evtBasic.tmStart.hour, evtBasic.tmStart.minute, evtBasic.tmStart.second);
        return dar;
    }      

    if(evtBasic.tmEnd.year == 0) //TODO 姑且使用0作为不存在的判据
    {
       dstBufTmp[offset++] = DT_NULL; 
    }
    else
    {
        //事件结束时间
        dar = basic_data_to_buf(E_DATETIME_S, dstBufTmp, dstLen-offset, &offset, &evtBasic.tmEnd, sizeof(OOP_DATETIME_S_T));
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("basic_data_to_buf failed. dar(%d), tmEnd(%04d-%02d-%02d %02d:%02d:%02d)\n", 
                dar, evtBasic.tmEnd.year, evtBasic.tmEnd.month, evtBasic.tmEnd.day, 
                evtBasic.tmEnd.hour, evtBasic.tmEnd.minute, evtBasic.tmEnd.second);
            return dar;
        }
    }
    
     //事件发生源
    dar = data_to_buf_evt_source(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic.source, sizeof(OOP_EVTSOURCE_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("data_to_buf_evt_source failed. dar(%d), choice(%d)\n", dar, evtBasic.source.choice);
        return dar;
    }
        
    //事件上报状态
    dar = data_to_buf_evt_report(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic.status, sizeof(OOP_REPORTS_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("data_to_buf_evt_report failed. dar(%d), nNum(%d)\n", dar, evtBasic.status.nNum);
        return dar;
    }

    *pOffset = offset;

    return dar;    
}

/**
*********************************************************************
* @brief：      事件关联OAD，子结构体拼装报文
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_coload(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{   
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    OOP_EVTBUF_T coload;
    uint8 i = 0;
    uint32 srcOffset = 0;

    memcpy(&coload, srcData, srcLen);

    if(coload.oadNum > OOP_MAX_EVTOADS)
    {
        ACUDP_FMT_DEBUG("fatal error. oda(0x%08x), coload.oadNum(%d) > OOP_MAX_OADS(%d)\n", pOadInfo->pTab->oad.value, coload.oadNum, OOP_MAX_OADS);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    for(i = 0; i < coload.oadNum; i++)
    {
        //从OAD的结构中，拼装698格式报文 实现 normal_oad_data_to_buf 函数
        dar = data_to_buf_evt_oad(coload.cols.oad[i], dstBuf, dstLen, &offset, &coload.buf[srcOffset], coload.idLen[i]);
        if(dar != DATA_SUCCESS)
        {
            ACUDP_FMT_DEBUG("data_to_buf_evt_oad failed. [%d] oda(0x%08x), idLen(%d)\n", i, coload.cols.oad[i].value, coload.idLen[i]);
            return DATA_TYPE_UNMATCHED;          
        }
        
        srcOffset += coload.idLen[i];
    }

    *pOffset = offset;

    return dar;
}

/**
*********************************************************************
*@brief         配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
int32 data_to_buf_evt_cfgdelay(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    int32 nRet = ERR_NORMAL;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DELAY_T evtcfg;
    uint8 *dstBufTmp = (uint8 *)dstBuf;
    
    memcpy(&evtcfg, srcData, srcLen);

    //结构类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    
    //结构成员数
    dstBufTmp[offset++] = 1;
               
    //判定延时时间
    if(basic_data_to_buf(E_UNSIGNED, dstBufTmp, dstLen, &offset, &evtcfg.nDelay, sizeof(uint8)) < 0)
    {
        nRet = ERR_NORMAL;
        return nRet;
    }  

    *pOffset = offset;
     
    return ERR_OK;
}

/**
*********************************************************************
* @brief        配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in]:   srcBuf     源数据区698报文
*               srcLen     源数据长度
*               dstLen     目的长度，通常是变量的长度
*
* @param[out] ：pOffset     解出该数据后的源数据区偏移
*               dstData    目的数据区，通常是变量的地址
* @return：      =0            - 成功
*               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E buf_to_data_evt_cfgdelay(const OAD_INFO_T *pOadInfo, const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32  offset = *pOffset;
    OOP_EVTCFG_DELAY_T evtcfg;
    uint8 *srcBufTmp = (uint8 *)srcBuf;

    //结构类型
     if(srcBufTmp[offset++] != DT_STRUCTURE)
     {
         ACUDP_BUF_TRACE(srcBufTmp, srcLen, "check struct type failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }

     //结构成员数
     if(srcBufTmp[offset++] != 1)
     {
         ACUDP_BUF_TRACE(srcBufTmp, srcLen, "check struct num failed. offset(%d)", offset);
         return DATA_TYPE_UNMATCHED;
     }
    
    //判定延时时间
    dar = basic_buf_to_data(E_UNSIGNED, srcBufTmp, srcLen, &offset, &evtcfg.nDelay, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_BUF_TRACE(srcBufTmp, srcLen, "basic_buf_to_data failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }    

    memcpy(dstData, &evtcfg, dstLen);
    *pOffset = offset;
     
    return dar;
}


/**
*********************************************************************
* @brief：      标准事件记录单元
* @param[in]： ppOadInfo     - 数据表指针
               srcData       - 源数据
               srcLen        - 源数据长度
* @param[out]：dstBuf        - 目的数据
               dstLen        - 目的数据长度
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
OOP_DAR_E data_to_buf_evt_std(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    uint32     offset = *pOffset;
    uint8 *dstBufTmp = (uint8 *)dstBuf;

    /*入参检查*/
    if ((pOadInfo == NULL) || (srcData == NULL) || (dstBuf == NULL) )
    {
        ACUDP_FMT_DEBUG("data_to_buf_evt_std param err\n");
        return DATA_OTHER_REASON;
    }

    if (pOadInfo->pTab->eData != E_OOP_EVENT_T)
    {
         ACUDP_FMT_DEBUG("data_to_buf_evt_std check data type failed. expect: E_OOP_EVENT_T<->value: %d\n", pOadInfo->pTab->eData);
         return DATA_TYPE_UNMATCHED;
    }
    
    OOP_EVENT_T oopLog;

    if (srcLen != sizeof(oopLog))
    {
         ACUDP_FMT_DEBUG("data_to_buf_evt_std check data len failed.");
         return DATA_TYPE_UNMATCHED;
    }

    memcpy(&oopLog, srcData, srcLen);

    //结构数据类型
    dstBufTmp[offset++] = DT_STRUCTURE;
    dstBufTmp[offset++] = 5 + oopLog.data.oadNum;

    EVT_BASIC_T evtBasic;
    evtBasic.nIndex = oopLog.nIndex;
    evtBasic.tmStart = oopLog.tmStart;
    evtBasic.tmEnd = oopLog.tmEnd;
    evtBasic.source = oopLog.source;
    evtBasic.status = oopLog.state;

    //*********************************************以下为事件记录单元的公共数据******************************************/
    dar = data_to_buf_evt_basic(pOadInfo, dstBufTmp, dstLen-offset, &offset, &evtBasic, sizeof(evtBasic));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("data_to_buf_evt_basic failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    //*********************************************以下为不同事件的特殊数据******************************************/

    //关联对象属性数据
    dar = data_to_buf_evt_coload(pOadInfo, dstBufTmp, dstLen-offset, &offset, &oopLog.data, sizeof(oopLog.data));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_DEBUG("data_to_buf_evt_coload failed. dar(%d), offset(%d)\n", dar, offset);
        return DATA_TYPE_UNMATCHED;
    }

    *pOffset = offset;

    if(offset > dstLen)
    {
        ACUDP_FMT_DEBUG("fatal error. dstLen(%d), offset(%d)\n", dstLen, offset);
        return DATA_SCOPE_OF_ACCESS_VIOLATED;       
    }

    return ERR_OK;
}


/**
*********************************************************************
* @name：       evt_cfgdelay_set
* @brief       配置参数-
*               300E-电能表辅助电源掉电事件
*               300F-电能表电压逆相序事件
*               3010-电能表电流逆相序事件
*               3011-电能表掉电事件
*               3030-通信模块变更事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdelay_set(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 inLen, uint32 *offset)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    OOP_EVTCFG_DELAY_T evtcfg;

    //从报文中提取数据
    dar = buf_to_data_evt_cfgdelay(pOadInfo, inData, inLen, offset, &evtcfg, sizeof(OOP_EVTCFG_DELAY_T));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("buf_to_data_evt_cfgdelay failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
    normal.logicId = 1;//交采参数
    
    ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&evtcfg, sizeof(OOP_EVTCFG_DELAY_T));
    ACUDP_FMT_TRACE("evtcfg.nDelay=%d \n", evtcfg.nDelay);
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("evt_cfgdelay db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       设置上报标记
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_report_flag_set(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = basic_buf_to_data(E_ENUM, bufData, bufLen, &offset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("buf_to_data_evt_asobjattr failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
	normal.logicId = 1;//交采参数

    ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(uint8));
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("evt_report_flag db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       设置有效标记
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_valid_flag_set(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;
    NOMAL_OAD_T normal = {0};
    uint8 data;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    //从报文中提取数据
    dar = basic_buf_to_data(E_BOOL, bufData, bufLen, &offset, &data, sizeof(uint8));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("basic_buf_to_data failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    //写入数据
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.classtag = CLASS_DATA_INIT;
	normal.logicId = 1;//交采参数

    ret = db_write_nomal(gDbUdpClient, &normal, (uint8*)&data, sizeof(uint8));
    if(ret != ERR_OK)
    {
        ACUDP_FMT_TRACE("evt_valid_flag db_write_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return DATA_OTHER_REASON;
    }

    return dar;
}

/**
*********************************************************************
* @name：       事件配置参数
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgparam_set(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;
    const uint8 *bufData = inData + 4; //偏掉OAD
    uint16 bufLen = InDataLen - 4;
    uint32 offset = 0;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVTCFG_DELAY_T://延时
            dar = evt_cfgdelay_set(pOadInfo, bufData, bufLen, &offset);
        break;

        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    if(offset > bufLen)
    {
        dar = DATA_SCOPE_OF_ACCESS_VIOLATED;
    }

    return dar;
}


/**
*********************************************************************
* @name：       class7_invoke_set
* @brief：      class7的SET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class7_invoke_set(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_OAD_U oad;
    OOP_DAR_E dar = DATA_SUCCESS;

    oad.value = pOadInfo->pTab->oad.value;

    switch(oad.attID)
    {
        case 6:   //配置参数
        {
            dar = evt_cfgparam_set(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 8:   //上报标识
        {
            dar = evt_report_flag_set(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;  
        case 9:   //有效标识
        {
            dar = evt_valid_flag_set(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;
        
        default: 
        {
            dar = DATA_READ_WRITE_DENIED; 
        }break;
    }

    if(dar == DATA_SUCCESS)
    {
        //拷贝OAD
        memcpy_r(outData->pResultData, inData, sizeof(uint32));
        outData->resultDataLen += sizeof(uint32);

        //不包含DAR,统一外面处理
    }

    return dar;
}

/**
*********************************************************************
* @brief：      获取事件的所有记录列
* @param[in]： pRSD          - 行选择
               pCSD          - 列选择
* @param[out]：pPlanTypeInfo - 目的数据
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int32 evt_all_rcsd_get(OAD_INFO_T *pOadInfo, uint16 *pOffset, uint8 *pOutData, uint16 outDataMax)
{
    int32 nRet = ERR_OK;
    uint32 offset = *pOffset;
    uint8 oadNum = 0;
    const OOP_OAD_U *pOad = NULL;
    uint32 i = 0;

    switch(pOadInfo->pTab->oad.nObjID)
    {
        case 0x300F:
        {
            oadNum = sizeof(g_evt_300f_obj)/sizeof(OOP_OAD_U);
            pOad = g_evt_300f_obj;
        }break;

        default:break;
    }

    //SEQUENCE OF CSD  
    pOutData[offset++] = oadNum;

    //关联的OAD和顺序写死的，后面优化
    while(i++ < oadNum)
    {
        pOutData[offset++] = 0; //CSD type 
        memcpy_r(&pOutData[offset], pOad++, sizeof(OOP_OAD_U));
        offset += 4;   
    }
    
    *pOffset = offset;
    
    return nRet;
}

/**
*********************************************************************
* @name：       evt_rectable_recmethod_get
* @brief：      用读记录型数据方式读事件记录表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E event_rectable_rec_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E  dar = DATA_SUCCESS;
    int nRet = ERR_OK;
    uint32 offset = 0;   //输入报文buf的偏移
    uint32 csdStart = 0;
    RECORD_INFO_T recordInfo;
    BUFFER_INFO_T outRecord;
    OOP_OAD_U oopOad;
    OOP_PRSD_T oopRsd;
    OOP_RCSD_T oopRcsd;   
    //uint16 innerOffset = 0;
    OAD_INFO_T oadInfo;

    //参数打印
    ACUDP_BUF_TRACE(inData, InDataLen, "event_rectable_rec_get. oad(0x%08x)\n", pOadInfo->pTab->oad.value);

    //初始化
    MEMZERO(&recordInfo, sizeof(recordInfo));
    MEMZERO(&outRecord, sizeof(outRecord));
    MEMZERO(&oopOad, sizeof(oopOad));
    MEMZERO(&oopRsd, sizeof(oopRsd));
    MEMZERO(&oopRcsd, sizeof(oopRcsd));
    memcpy(&oadInfo, pOadInfo, sizeof(OAD_INFO_T));

    //报文中提取OAD
    memcpy_r(&oopOad, &inData[offset], sizeof(uint32));
    offset += sizeof(uint32);

    //报文中提取RSD
    dar = apdu_rsd_fromfmt698(inData, InDataLen, &offset, &oopRsd, sizeof(oopRsd));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("RSD解析错误\n");
        return dar;
    }

    //报文中提取RCSD
    csdStart = offset;
    dar = apdu_rcsd_fromfmt698(inData, InDataLen, &offset, &oopRcsd, sizeof(oopRcsd));
    if(dar != DATA_SUCCESS)
    {
        ACUDP_FMT_TRACE("RCSD解析错误\n");
        return dar;
    }

//    if(oopRcsd.nNum == 0)
//    {
//        //TODO,读所有列
//        oadInfo.noParse = TRUE;
//        all_rcsd_get(&oadInfo, gDbUdpClient, &innerOffset, (uint8*)&oopRcsd, sizeof(OOP_RCSD_T));
//    }

    //先封装记录响应头
    read_record_response(&oadInfo, gDbUdpClient, inData, InDataLen, offset, csdStart, NULL, outData, outDataMax);

    //分配读取记录的空间
    recordInfo.oopOad = oopOad;
    recordInfo.oopRsd = oopRsd;   
    recordInfo.oopRcsd = oopRcsd;
    recordInfo.append.pOadInfo = pOadInfo;
  
    outRecord.buffer = outData->pResultData;  
    outRecord.offset = &outData->resultDataLen;
    outRecord.bufLen = outDataMax;
    
    /*读统计记录表接口，可以读出apdu数据域，可返回断点信息*/
    nRet = record_event_read(gDbUdpClient, &recordInfo, &outRecord);
    if (nRet != 0)
    {
        ACUDP_FMT_DEBUG("record_tmn_frozen_read failed. nRet(%d)\n", nRet);    
        return nRet;
    }
    
    return dar;
}

/**
*********************************************************************
* @name：       evt_cfgdelay_get
* @brief       配置参数-
*              300E-电能表辅助电源掉电事件
*              300F-电能表电压逆相序事件
*              3010-电能表电流逆相序事件
*              3011-电能表掉电事件
*              3030-通信模块变更事件
* @param[in] ：pOdaInfo        OAD信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgdelay_get(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    OOP_EVTCFG_DELAY_T evtcfg;
    uint32 len = 0;
    uint32 offset = 0;

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    memset(&evtcfg, 0, sizeof(OOP_EVTCFG_DELAY_T));

    ret = db_read_nomal(gDbUdpClient, &normal, sizeof(OOP_EVTCFG_DELAY_T), (uint8*)&evtcfg, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return oop_null_data_res(pOadInfo, inData, InDataLen, outData, outDataMax);
    }

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    outData->pResultData[offset++] = 1;

    dar = data_to_buf_evt_cfgdelay(pOadInfo, outData->pResultData, outDataMax, &offset, &evtcfg, len);
    if(dar != DATA_SUCCESS)
    {
        PRTL_FMT_DEBUG("data_to_buf_evt_cfgdelay failed. dar(%d), oad(0x%08x)\n", dar, pOadInfo->pTab->oad.value);
        return dar;
    }

    outData->resultDataLen = offset;

    return dar;    
}

/**
*********************************************************************
* @name：       evt_rectable_get
* @brief：      读关联对象属性表
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_cfgparam_get(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    OOP_DAR_E dar = DATA_SUCCESS;

    switch(pOadInfo->pTab->eData)
    {
        case E_OOP_EVTCFG_DELAY_T://通信模块变更事件
            dar = evt_cfgdelay_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        break;         
        default:
            dar = DATA_READ_WRITE_DENIED;
        break;
    }

    return dar;
}

/**
*********************************************************************
* @name：       evt_report_flag_get
* @brief：      上报标识
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_report_flag_get(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    uint8 data = 0;
    uint32 len = 0;
    uint32 offset = 0;

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);

    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 0;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    //读取无数据视为0
    ret = db_read_nomal(gDbUdpClient, &normal, sizeof(uint8), &data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        
        if(ret == ERR_NOTEXIST)
        {
            len = sizeof(uint8);
        }
        else
        {
            return DATA_OTHER_REASON;
        }
    }

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_ENUM;
    memcpy_r(&outData->pResultData[offset], &data, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       evt_report_flag_get
* @brief：      上报标识
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E evt_valid_flag_get(const OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    int ret = ERR_OK;
    OOP_DAR_E dar = DATA_SUCCESS;    
    NOMAL_OAD_T normal = {0};   
    uint8 data = 0;
    uint32 len = 0;
    uint32 offset = 0;

    memcpy(&outData->pResultData[offset], inData, sizeof(OOP_OAD_U));
    offset += sizeof(OOP_OAD_U);
    
    normal.oad.value = pOadInfo->pTab->oad.value;
    normal.logicId = 1;
    normal.infoNum = pOadInfo->pTab->oad.nIndex;

    //读取无数据视为0
    ret = db_read_nomal(gDbUdpClient, &normal, sizeof(uint8), &data, &len);
    if(ret != ERR_OK)
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        
        if(ret == ERR_NOTEXIST)
        {
            len = sizeof(uint8);
        }
        else
        {
            return DATA_OTHER_REASON;
        }
    }

    outData->pResultData[offset++] = 1;
    outData->pResultData[offset++] = DT_BOOL;
    memcpy_r(&outData->pResultData[offset], &data, len);
    offset += len;

    outData->resultDataLen = offset;

    return dar;
}

/**
*********************************************************************
* @name：       class7_invoke_get
* @brief：      class7的GET服务
* @param[in] ：pOdaInfo        OAD信息
               pBpInfo        应用分帧断点信息
               inData         输入报文，不含OAD
               inLen          输入报文长度
               outDataMax     输出报文缓冲区最大长度

* @param[out]：outData     输出报文
* @return    ：输出报文实际长度
* @author    : 
* @Date      ：2019-12-6
*********************************************************************
*/
OOP_DAR_E class7_invoke_get(OAD_INFO_T *pOadInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax)
{
    
    switch (pOadInfo->pTab->oad.attID)
    {
        case 2:   //事件记录表
        {
           return event_rectable_rec_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 6:  //配置参数
        {
            return evt_cfgparam_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;
        case 8:  //上报标识
        {
            return evt_report_flag_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;

        case 9:  //有效标识
        {
            return evt_valid_flag_get(pOadInfo, inData, InDataLen, outData, outDataMax);
        }break;        
        default:
        {
            return DATA_READ_WRITE_DENIED;
        }break;   
    }

    return DATA_SUCCESS;
}


