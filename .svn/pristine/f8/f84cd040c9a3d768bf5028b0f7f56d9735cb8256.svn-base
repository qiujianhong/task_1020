/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：MetDLMS.c
 * 版本号：1.0
 * 功能：该程序完成ABB ALPHA表规约的组帧、析帧及相关数据的存储功能
 * 开发人：尤斌
 * 开发时间：2006年12月22日
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/

//#define FIRST_DEFINE_METPARAM

#include "../../include/CtsAll.h"
#include "../../include/metdlms.h"
extern RS485_RS232_PARA Rs485ComNo[8];
//#include "MetOop.h"

extern uint8 IsSendSn[8];





#define TESTRECV     0  //测试的时候用,无须无手直接进入数据收发
#define TESTNOREADSN 0  //测试的时候用,无须无手直接进入数据收发

#define DLMS_SNRM_STAT          0   //-握手第一帧报文-
#define DLMS_AARQ_STAT          1   //-握手第二帧报文-
#define DLMS_READ_SN_STAT       2   //-握手第三帧报文-
#define DLMS_RR_FIRST_STAT      4   //-握手第三帧报文-
#define DLMS_DISC_STAT          5   //-握手断连报文-
#define DLMS_SNRM_AGAIN_STAT    6   //-握手第三帧报文-
#define DLMS_AARQ_AGAIN_STAT    7   //-握手第二帧报文-

#define DLMS_DLMS_CAPTURE_PERIOD_DAY 10  //-获取周期-日冻结
#define DLMS_DLMS_CAPTURE_OBJECT_DAY 11  //-获取对象-日冻结  
#define DLMS_DLMS_CAPTURE_DATA_DAY  12   //读捕获数据-日冻结

#define DLMS_DLMS_CAPTURE_PERIOD_MON 13  //-获取周期-月冻结
#define DLMS_DLMS_CAPTURE_OBJECT_MON 14  //-获取对象-月冻结
#define DLMS_DLMS_CAPTURE_DATA_MON  15   //读捕获数据-月冻结

#define DLMS_DLMS_CAPTURE_PERIOD_CURVE1 16  //-获取周期-曲线1冻结
#define DLMS_DLMS_CAPTURE_OBJECT_CURVE1 17  //-获取对象-负荷曲线1   
#define DLMS_DLMS_CAPTURE_DATA_CURVE1   18  //读捕获数据-曲线1冻结

#define DLMS_DLMS_CAPTURE_PERIOD_CURVE2 19  //-获取周期-曲线2冻结
#define DLMS_DLMS_CAPTURE_OBJECT_CURVE2 20  //-获取对象-负荷曲线2   
#define DLMS_DLMS_CAPTURE_DATA_CURVE2   21  //读捕获数据-曲线2冻结

#define DLMS_FRZ_DAY_ID 0xfffe  //日冻结代号 特殊处理
#define DLMS_FRZ_MON_ID 0xfffd  //月冻结代号 特殊处理
#define DLMS_FRZ_CURVE1_ID 0xfffc  //曲线1冻结代号 特殊处理
#define DLMS_FRZ_CURVE2_ID 0xfffb  //曲线2冻结代号 特殊处理

#define DLMS_LN_CMDID_READ      0xC0    /*-读数据-*/
#define DLMS_SN_CMDID_READ      0x05    /*-SN读数据-*/

#define     HDLC_MSA        0x21        //源地址

#define     CLASS_01        0x01        //带单位和精度的类
#define     CLASS_02        0x02        //单个时间类数据的抄读
#define     CLASS_03        0x03        //单个数据的抄读
#define     CLASS_04        0x04        //历史数据的整体抄读

#define     CLASS_07        0x07        //冻结数据的整体抄读


#define     CLASS_01_Offset     0x10    //带单位和精度的类返回数据的偏移位置

#define     CLASS_04_Offset     0x0a    //带单位和精度的类返回数据的偏移位置
//收发帧序号
//收发帧序号
uint8 RECV_SEQ;         
uint8 SEND_SEQ;


//static void  DLMS_HDSK_Cmd_Init(S_COMM_INFO_MET *Comm_Info_dlms);
uint8 DLMS_HandShake_Cmd(uint8 pipe, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms);
//static uint8 DLMS_AUTK_Cmd(uint8 pipe, const S_CbIDInfo **pCbTaskID, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms,DLMS_METER_COMM_INFO *pdlms_comm);
uint8 DLMS_CheckRspFrame(uint8 pipe, cbMtrPara MtrPara, S_COMM_INFO_MET *Comm_Info_dlms);

static uint16  pppfcs16(uint16 fcs, uint8 *cp, int16 len);
static void fcs16(uint8 *frame, int16 len);
static void HdlcAddrTran( uint8 *hdlc_addr, uint16 log_addr, uint16 phy_addr);
static int SNRM_MakeFrame(uint8 *pBuf, uint16 metaddr);
static int AARQ_MakeFrame(uint8 *pBuf, uint16 metaddr);
static int RR_MakeFrame(uint8 *pBuf, uint16 metaddr,S_COMM_INFO_MET *pCommInfo);
static int CheckRespondFrame(uint8 *recvBuf, uint16 recvLen, uint8 *met_addr, uint8 step,uint8 RRFlag,S_COMM_INFO_MET *Comm_Info_dlms);
uint8 DISC_MakeFrame(uint8 *pBuf, uint16 metaddr);
uint8  make_mac_frame(uint8 *frame, uint8 jzq[4], uint8 ctrl_code, uint8 *llc_info, int len_llc_info);
//static void DLMS_AUTK_Recv(uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms, DLMS_METER_COMM_INFO *pdlms_comm);
//static int DLMS_AUTK_Recv_04(uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms, DLMS_METER_COMM_INFO *pdlms_comm);
//static void DLMS_MakeFrame(uint8 *pMetAddr, uint16 ShortName, uint8 Class, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, DLMS_METER_COMM_INFO *pdlms_comm);
//static void FormatToDLMS(uint8 pipe, uint8 srcDataType, uint16 eDataId, dataFormat *pDF, uint8 bStdType);
//static char *memchr_dlms(char *buf, uint8 sync, uint32 buf_len);
static int make_llc_frame(unsigned char *info,int info_len);
//static void InverseBuf(uint8 *pBuf, uint16 len);
static int Read_SN_MakeFrame(uint8 *pBuf, uint16 metaddr);
//static int Read_PERIOD_MakeFrame(DLMS_METER_COMM_INFO *pdlms_comm, uint8 *pBuf, uint16 metaddr);
//static int Read_CAPTURE_OBJECT_MakeFrame(DLMS_METER_COMM_INFO *pdlms_comm, uint8 *pBuf, uint16 metaddr);

//从过程冻结数据缓冲区中按命令帧要求抽取所需的数据项数据，并仿真表计回码组帧
//int32 DLMS_getFrzData(S_COMM_INFO_MET *pComm_Info_dlms, DLMS_METER_COMM_INFO *pdlms_comm);
//保存冻结数据，type为冻结类型
//static int DLMS_saveFrzParaAndData(DLMS_METER_COMM_INFO *pdlms_comm,uint8 *recvBuf, uint16 recvLen, uint8 *met_addr, uint8 * RRFlag);
//表计回码组帧
//static int  make_mac_frame_meter(uint8 *frame, uint8 jzq[4], uint8 ctrl_code, uint8 *llc_info, int len_llc_info);
//static int Read_CAPTURE_DATA_MakeFrame(DLMS_METER_COMM_INFO *pdlms_comm, uint8 *pBuf, uint16 metaddr);
//void  DLMS_Frz_Cmd_Init(DLMS_METER_COMM_INFO *pdlms_comm);
//获取数据项的短名，用来代替 SD_MET_PRTL_DLMS_PRTL 中存放的
//uint16 getShortNameForDataId(struct MET_PRTL_DLMS *pTmp, DLMS_METER_COMM_INFO *pdlms_comm);
//通过长名获取对象的基地址
//DLMS_Association_OBJ * get_DLMS_Association_OBJ(DLMS_METER_COMM_INFO *pdlms_comm,const uint8 *in_obis);

//uint8 cbTaskNewDlmsAutkMapID(struct MET_PRTL_DLMS *pMET_PRTL_DLMS_MAP, const S_CbIDInfo *pCbTaskID, S_COMM_INFO_MET* pMetInfo);
//void  STDDlms_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask);




//冻结对象长名
typedef struct 
{
    uint8 obis[6];
}DLMS_FRZ_OBJ_OBIS;

const DLMS_FRZ_OBJ_OBIS g_frzObjObis[]=
{
    {{0x00,0x00,0x62,0x01,0x00,0x7e}},//日冻结长名
    {{0x00,0x00,0x62,0x02,0x00,0x7e}},//月冻结长名
    {{0x01,0x00,0x63,0x01,0x00,0xff}},//曲线1冻结长名，   1-0:99.1.0是负荷曲线1的OBIS代码
    {{0x01,0x00,0x63,0x02,0x00,0xff}},//曲线2冻结长名 第二条是1-0:99.2.0  ，10年后的C8表有两条负荷曲线
};

cbMetPara SD_DLMSPara =
{
    FPGJ,/*-表征费率顺序-*/   
    9600,/*波特率*/                                                        
    8,   /*数据位, 4-8 */                                                 
    'n', /*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/                         
    1,   /*停止位 0,1,2 = 1, 1.5, 2*/                                     
    0x7e,/*-同步头--*/                                                    
    0xff,/*结束字节1*/                                                     
    0xFF,/*结束字节2*/                                                     
    0x01,/*长度1    */                                                     
    0xFF,/*长度2    */                                                     
    0x06,/*附加长度,*/                                                     
    50, /*--电表反应时间:命令码发出后，收到电表返回的第一个字节的时间--*/
    100, /*--命令发送时间:发送命令报文的时间--*/                          
    10,  /*--字节之间的间隔时间--*/                      
    7,   /*-给有握手过程的电表使用,握手帧总数-*/                          
    3    /*-给有握手过程的电表使用,读取数据帧总数-*/                      
};

static const uint16 fcstab[]=
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,

    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
static void HdlcAddrTran( uint8 *hdlc_addr, uint16 log_addr, uint16 phy_addr)
{   
    hdlc_addr[1] = (uint8 )((log_addr&0x7f)<<1);
    hdlc_addr[0] = (uint8 )((log_addr>>7)<<1);  
    
    hdlc_addr[3] = (uint8 )((phy_addr&0x7f)<<1)+1;
    
    hdlc_addr[2] = (uint8 )((phy_addr>>7)<<1);
}

static uint16  pppfcs16(uint16 fcs, uint8 *cp, int16 len)
{
   while(len--)
   {
      fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
   }
   return (fcs);
}

/*+++
   功能: 计算FCS校验
   参数: unsigned char *frame  祯数据,从祯格式开始
         INT16S len               代计算校验的字节数量
   描述: 校验值被放在祯数据的末尾2字节
---*/
static void fcs16(uint8 *frame, int16 len)
{
   uint16 trialfcs;

   /* add on output */
   trialfcs = pppfcs16(0xffff, frame, len);
   trialfcs ^= 0xffff;                          /* complement */
   frame[len] = (trialfcs & 0x00ff);            /* least significant byte first */
   frame[len+1] = ((trialfcs >> 8) & 0x00ff);
}


/*-
***************************************************************************
*模块编号:
*名称:make_llc_frame
*功能:组织抄表主站发起的LLC命令祯,info后移3位
*输入参数:info
*输出参数:info
*返回:LLC 信息长度
***************************************************************************
-*/
static int make_llc_frame(unsigned char *info,int info_len)
{
    int i;
    for(i=info_len-1; i>=0;i--)
    {
        info[i+3] = info[i];
    }
    info[0] = 0xE6;
    info[1] = 0xE6;
    info[2] = 0x00;

    return info_len + 3;
}

/*-
***************************************************************************
*模块编号:
*名称:发送AARQ帧
*功能:对AARQ进行组帧
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
static int SNRM_MakeFrame(uint8 *pBuf, uint16 metaddr)
{
    //参数协商     
    //使用默认值     
    // 发送窗口=1     
    // 接收窗口=1     
    // 发送最大信息域长度=128   
    // 接收最大信息域长度=128  
    // 0x93  SNRM 
    uint8 info_SNRM[] = {0x81, 0x80, 0x12, 0x05, 0x01, 0x80, 0x06, 0x01, 
                    0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 
                    0x00, 0x00, 0x07};   
    
    uint8 info[1000];
    int   info_len;  
    uint8 hdlcaddr[10];   
    
    info_len = info_SNRM[2] + 3;  
    
    if(info_len == 3)
    {   
        info_len =0;  
    }
    memcpy(info, info_SNRM, info_len);
    //组织MAC层数据祯  
    //     首先，获取集中器的HDLC扩展地址   
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    info_len = 0;
    return make_mac_frame(pBuf, hdlcaddr, 0x93, info, info_len); 
}




#ifdef AREA_SHANDONG 
MET_PRTL_DLMS SD_MET_PRTL_DLMS_PRTL[]=
{
    {{0x00100201},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功2电能
    {{0x00100204},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功3电能
    {{0x00100205},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功4电能

    {{0x00200201},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功总电能
    {{0x00200202},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功1电能
    {{0x00200203},         {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功2电能
    {{0x00200204},         {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功3电能
    {{0x00200205},         {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功4电能
    {{0x00300201},         {0x01,0x01,0x84,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  //132.8.0 正向无功I+IV 总
    {{0x00300203},         {0x01,0x01,0x84,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300204},         {0x01,0x01,0x84,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300205},         {0x01,0x01,0x84,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300202},         {0x01,0x01,0x84,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  

    {{0x00400201},         {0x01,0x01,0x85,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  //133.8.0 反向无功II+III 总        
    {{0x00400202},         {0x01,0x01,0x85,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400203},         {0x01,0x01,0x85,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400204},         {0x01,0x01,0x85,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400205},         {0x01,0x01,0x85,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          


    {{0x00500201},       {0x01,0x01,0x05,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },   //sd     
    {{0x00500202},       {0x01,0x01,0x05,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500203},       {0x01,0x01,0x05,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500204},       {0x01,0x01,0x05,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500205},       {0x01,0x01,0x05,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
 
    {{0x00600201},      {0x01,0x01,0x06,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600202},      {0x01,0x01,0x06,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600203},      {0x01,0x01,0x06,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600204},      {0x01,0x01,0x06,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600205},      {0x01,0x01,0x06,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         

    {{0x00700201},      {0x01,0x01,0x07,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700202},      {0x01,0x01,0x07,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700203},      {0x01,0x01,0x07,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700204},      {0x01,0x01,0x07,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700205},      {0x01,0x01,0x07,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         

    {{0x00800201},       {0x01,0x01,0x08,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800202},       {0x01,0x01,0x08,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800203},       {0x01,0x01,0x08,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800204},       {0x01,0x01,0x08,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800205},       {0x01,0x01,0x08,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   }, 
    //高精度
    {{0x00100401},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100402},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100403},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功2电能
    {{0x00100404},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功3电能
    {{0x00100405},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)正向有功4电能

    {{0x00200401},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功总电能
    {{0x00200402},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功1电能
    {{0x00200403},         {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功2电能
    {{0x00200404},         {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功3电能
    {{0x00200405},         {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  3,  2   },  //(当前)反向有功4电能
    {{0x00300401},         {0x01,0x01,0x84,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  //132.8.0 正向无功I+IV 总
    {{0x00300403},         {0x01,0x01,0x84,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300404},         {0x01,0x01,0x84,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300405},         {0x01,0x01,0x84,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00300402},         {0x01,0x01,0x84,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  

    {{0x00400401},         {0x01,0x01,0x85,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  //133.8.0 反向无功II+III 总        
    {{0x00400402},         {0x01,0x01,0x85,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400403},         {0x01,0x01,0x85,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400404},         {0x01,0x01,0x85,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          
    {{0x00400405},         {0x01,0x01,0x85,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },          


    {{0x00500401},       {0x01,0x01,0x05,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },   //sd     
    {{0x00500402},       {0x01,0x01,0x05,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500403},       {0x01,0x01,0x05,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500404},       {0x01,0x01,0x05,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
    {{0x00500405},       {0x01,0x01,0x05,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },        
 
    {{0x00600401},      {0x01,0x01,0x06,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600402},      {0x01,0x01,0x06,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600403},      {0x01,0x01,0x06,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600404},      {0x01,0x01,0x06,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00600405},      {0x01,0x01,0x06,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         

    {{0x00700401},      {0x01,0x01,0x07,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700402},      {0x01,0x01,0x07,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700403},      {0x01,0x01,0x07,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700404},      {0x01,0x01,0x07,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         
    {{0x00700405},      {0x01,0x01,0x07,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },         

    {{0x00800401},       {0x01,0x01,0x08,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800402},       {0x01,0x01,0x08,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800403},       {0x01,0x01,0x08,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800404},       {0x01,0x01,0x08,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   },  
    {{0x00800405},       {0x01,0x01,0x08,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  3,  2   }, 
    #if 1
    //需量数据
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   },  
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   },  
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   },  
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   },  
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   },  
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },
     
    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   }, 
    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   }, 
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   }, 
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   }, 
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  5,  4   }, 
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  
     
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   },  
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   },  
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   },  
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   },  
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   },  
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
     
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   }, 
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   }, 
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   }, 
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   }, 
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  5,  4   }, 
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  6,  0,  0   },  
    #endif 

    //电压电流
    {{0x20000201},      {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //A电压
    {{0x20000202},      {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //B电压
    {{0x20000203},      {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //C电压      

    {{0x20010201},      {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //A电流
    {{0x20010202},      {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //B电流
    {{0x20010203},      {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //C电流     

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //有功功率
    {{0x20040202},         {0x01,0x01,0x15,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //有功功率
    {{0x20040203},         {0x01,0x01,0x29,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //有功功率
    {{0x20040204},         {0x01,0x01,0x3D,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //有功功率


    {{0x20050201},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //无功功率
    {{0x20050202},         {0x01,0x01,0x16,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //无功功率
    {{0x20050203},         {0x01,0x01,0x2A,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //无功功率
    {{0x20050204},         {0x01,0x01,0x3E,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  5,  4   },  //无功功率
    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20020201},      {0x01,0x01,0x51,0x07,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020202},      {0x01,0x01,0x51,0x07,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020203},      {0x01,0x01,0x51,0x07,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20030201},      {0x01,0x01,0x51,0x07,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},      {0x01,0x01,0x51,0x07,0x05,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},      {0x01,0x01,0x51,0x07,0x06,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
//时间
    //{OAD_FREZZE_TM,        {0x01,0x01,0x01,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_02,  1,   1,  4,  3,  2   },
    {{0x40000200},      {0x00,0x00,0x01,0x00,0x00,0xff},  0x08, 0x0000, CLASS_02,  1,   1,  6,  0,  0   },  //电表日期时钟

    //00 00 62 01 00 7e 日冻结长名,特殊处理
    {{0x50060200},       {0x00,0x00,0x62,0x02,0x00,0x7e},  0x08, 0x0000, CLASS_07,  1,  1,  0,  0,  0   },  //月冻结长名
    {{0x50040200},       {0x00,0x00,0x62,0x01,0x00,0x7e},  0x08, 0x0000, CLASS_07,  1,  1,  0,  0,  0   },  //日冻结长名
    {{0x50030200},    {0x01,0x00,0x63,0x01,0x00,0xff},  0x08, 0x0000, CLASS_07,  1, 1,  0,  0,  0   },  //曲线1冻结长名
    {{0x50020200},    {0x01,0x00,0x63,0x02,0x00,0xff},  0x08, 0x0000, CLASS_07,  1, 1,  0,  0,  0   },  //曲线2冻结长名

    {{0xffffffff},                    {0xff,0xff,0xff,0xff,0xff,0xff},0xff,0xff,0xff,0xff,0xff}
};

#else
MET_PRTL_DLMS SD_MET_PRTL_DLMS_PRTL[]=
{
    {{0x00100200},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {{0x00100204},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {{0x00100201},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {{0x00200201},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {{0x00200203},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {{0x00200204},         {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {{0x00200205},         {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {{0x00200202},         {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {{0x00300201},         {0x01,0x01,0x03,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300203},         {0x01,0x01,0x03,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300204},         {0x01,0x01,0x03,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300205},         {0x01,0x01,0x03,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300202},         {0x01,0x01,0x03,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {{0x00400201},         {0x01,0x01,0x04,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400203},         {0x01,0x01,0x04,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400204},         {0x01,0x01,0x04,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400205},         {0x01,0x01,0x04,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400202},         {0x01,0x01,0x04,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {{0x00500201},       {0x01,0x01,0x05,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500203},       {0x01,0x01,0x05,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500204},       {0x01,0x01,0x05,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500205},       {0x01,0x01,0x05,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500202},       {0x01,0x01,0x05,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {{0x00600201},      {0x01,0x01,0x06,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600203},      {0x01,0x01,0x06,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600204},      {0x01,0x01,0x06,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600205},      {0x01,0x01,0x06,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600202},      {0x01,0x01,0x06,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00700201},      {0x01,0x01,0x07,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700203},      {0x01,0x01,0x07,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700204},      {0x01,0x01,0x07,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700205},      {0x01,0x01,0x07,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700202},      {0x01,0x01,0x07,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00800201},       {0x01,0x01,0x08,0x08,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800203},       {0x01,0x01,0x08,0x08,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800204},       {0x01,0x01,0x08,0x08,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800205},       {0x01,0x01,0x08,0x08,0x03,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800202},       {0x01,0x01,0x08,0x08,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
    #if 1
    //需量数据
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
     
    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
     
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
     
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    #endif 

    //电压电流
    {{0x20000201},      {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //A电压
    {{0x20000202},      {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //B电压
    {{0x20000203},      {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //C电压      

    {{0x20010201},      {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //A电流
    {{0x20010202},      {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //B电流
    {{0x20010203},      {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //C电流     

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {{0x20050201},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20030201},      {0x01,0x01,0x51,0x07,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},      {0x01,0x01,0x51,0x07,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},      {0x01,0x01,0x51,0x07,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20000201},      {0x01,0x01,0x51,0x07,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20000202},      {0x01,0x01,0x51,0x07,0x05,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20000203},      {0x01,0x01,0x51,0x07,0x06,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    //电表事件类数据
    //{PRAM_TMS_ID,             {0x00,0x00,0x60,0x02,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{LAST_PRAM_TD_ID,           {0x00,0x00,0x60,0x02,0x01,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  3   },   
    //{BAT_WORK_STATE_ID,         {0x00,0x00,0x60,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  0   },   
    {{0x40000200},      {0x00,0x00,0x01,0x00,0x00,0xff},  0x08, 0x0000, CLASS_02,  1,   1,  6,  0,  0   },  //电表日期时钟
        
    {{0xffffffff},                    {0xff,0xff,0xff,0xff,0xff,0xff},0xff,0xff,0xff,0xff,0xff}
};
#endif


enum eDLMS_FRZ_CAPTURE_PARA
{
    eCAPTURE_DAY=0,     //日冻结
    eCAPTURE_MON,       //月冻结
    eCAPTURE_CURVE_1,   //曲线1
    eCAPTURE_CURVE_2    //曲线2
};

////冻结对象长名
//struct DLMS_FRZ_OBJ_OBIS
//{
//  uint8 obis[6];
//};
//
//const struct DLMS_FRZ_OBJ_OBIS g_frzObjObis[]=
//{
//  {0x00,0x00,0x62,0x01,0x00,0x7e},//日冻结长名
//  {0x00,0x00,0x62,0x02,0x00,0x7e},//月冻结长名
//  {0x01,0x00,0x63,0x01,0x00,0xff},//曲线1冻结长名，   1-0:99.1.0是负荷曲线1的OBIS代码
//  {0x01,0x00,0x63,0x02,0x00,0xff},//曲线2冻结长名 第二条是1-0:99.2.0  ，10年后的C8表有两条负荷曲线
//};


MET_PRTL_DLMS SD_MET_PRTL_DLMS_PRTL_RT[]=
{
    {{0x00100200},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {{0x00100204},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {{0x00100201},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {{0x00200201},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {{0x00200203},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {{0x00200204},         {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {{0x00200205},         {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {{0x00200202},         {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {{0x00300201},         {0x01,0x01,0x03,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300203},         {0x01,0x01,0x03,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300204},         {0x01,0x01,0x03,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300205},         {0x01,0x01,0x03,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300202},         {0x01,0x01,0x03,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {{0x00400201},         {0x01,0x01,0x04,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400203},         {0x01,0x01,0x04,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400204},         {0x01,0x01,0x04,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400205},         {0x01,0x01,0x04,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400202},         {0x01,0x01,0x04,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {{0x00500201},      {0x01,0x01,0x05,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500203},      {0x01,0x01,0x05,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500204},      {0x01,0x01,0x05,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500205},      {0x01,0x01,0x05,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500202},      {0x01,0x01,0x05,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {{0x00600201},     {0x01,0x01,0x06,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600203},     {0x01,0x01,0x06,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600204},     {0x01,0x01,0x06,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600205},     {0x01,0x01,0x06,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600202},     {0x01,0x01,0x06,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00700201},     {0x01,0x01,0x07,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700203},     {0x01,0x01,0x07,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700204},     {0x01,0x01,0x07,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700205},     {0x01,0x01,0x07,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700202},     {0x01,0x01,0x07,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00800201},      {0x01,0x01,0x08,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800203},      {0x01,0x01,0x08,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800204},      {0x01,0x01,0x08,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800205},      {0x01,0x01,0x08,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800202},      {0x01,0x01,0x08,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
    #if 1
    //需量数据
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff}, 0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },

    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  

    {{0x10300201},      {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   },  
    {{0x10300201},      {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10300202},      {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   },  
    {{0x10300202},      {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10300203},      {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   },  
    {{0x10300203},      {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10300204},      {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   },  
    {{0x10300204},      {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10300205},      {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   },  
    {{0x10300205},      {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },

    {{0x10400201},      {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   }, 
    {{0x10400201},      {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10400202},      {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   }, 
    {{0x10400202},      {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10400203},      {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   }, 
    {{0x10400203},      {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10400204},      {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   }, 
    {{0x10400204},      {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },
    {{0x10400205},      {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  3,  0,  1   }, 
    {{0x10400205},      {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,   1,  5,  0,  0   },  
    #endif 

    //电压电流
    {{0x20000201},      {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //A电压
    {{0x20000202},      {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //B电压
    {{0x20000203},      {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  1   },  //C电压      

    {{0x20010201},      {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //A电流
    {{0x20010202},      {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //B电流
    {{0x20010203},      {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,   1,  2,  0,  3   },  //C电流     

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {{0x20050202},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20020201},      {0x01,0x01,0x51,0x07,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020202},      {0x01,0x01,0x51,0x07,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020203},      {0x01,0x01,0x51,0x07,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20030201},      {0x01,0x01,0x51,0x07,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},      {0x01,0x01,0x51,0x07,0x05,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},      {0x01,0x01,0x51,0x07,0x06,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    //电表事件类数据
    //{PRAM_TMS_ID,             {0x00,0x00,0x60,0x02,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{LAST_PRAM_TD_ID,           {0x00,0x00,0x60,0x02,0x01,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  3   },   
    //{BAT_WORK_STATE_ID,         {0x00,0x00,0x60,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  0   },   
    //{MET_CUR_DATE_TIME_ID,      {0x00,0x00,0x01,0x00,0x00,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  //电表日期时钟
//块抄******************************************************************
#if 1
    {{0x00100200},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {{0x00100204},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {{0x00100201},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {{0x00200201},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {{0x00200203},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {{0x00200204},         {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {{0x00200205},         {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {{0x00200202},         {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {{0x00300201},      {0x01,0x01,0x03,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300203},      {0x01,0x01,0x03,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300204},      {0x01,0x01,0x03,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300205},      {0x01,0x01,0x03,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300202},      {0x01,0x01,0x03,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {{0x00400201},      {0x01,0x01,0x04,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400203},      {0x01,0x01,0x04,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400204},      {0x01,0x01,0x04,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400205},      {0x01,0x01,0x04,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400202},      {0x01,0x01,0x04,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {{0x00500201},  {0x01,0x01,0x05,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500203},  {0x01,0x01,0x05,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500204},  {0x01,0x01,0x05,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500205},  {0x01,0x01,0x05,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500202},  {0x01,0x01,0x05,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {{0x00600201},    {0x01,0x01,0x06,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00600203},    {0x01,0x01,0x06,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00600204},    {0x01,0x01,0x06,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00600205},    {0x01,0x01,0x06,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00600202},    {0x01,0x01,0x06,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         

    {{0x00700201},    {0x01,0x01,0x07,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00700203},    {0x01,0x01,0x07,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00700204},    {0x01,0x01,0x07,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00700205},    {0x01,0x01,0x07,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         
    {{0x00700202},    {0x01,0x01,0x07,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,   4,  0,  1   },         

    {{0x00800201},  {0x01,0x01,0x08,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800203},  {0x01,0x01,0x08,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800204},  {0x01,0x01,0x08,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800205},  {0x01,0x01,0x08,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800202},  {0x01,0x01,0x08,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
#if 1
    //需量数据
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },

    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  

    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },

    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
#endif 

    //电压电流
    {{0x20000201},   {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //A电压
    {{0x20000202},   {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //B电压
    {{0x20000203},   {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //C电压    

    {{0x20010201},   {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //A电流
    {{0x20010202},   {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //B电流
    {{0x20010203},   {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //C电流   

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {{0x20050202},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20020201},   {0x01,0x01,0x51,0x07,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020202},   {0x01,0x01,0x51,0x07,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020203},   {0x01,0x01,0x51,0x07,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20030201},   {0x01,0x01,0x51,0x07,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},   {0x01,0x01,0x51,0x07,0x05,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},   {0x01,0x01,0x51,0x07,0x06,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
#endif  

    {{0xffffffff},                    {0xff,0xff,0xff,0xff,0xff,0xff},0xff,0xff,0xff,0xff,0xff}
};

#if SHANDONG_FUN==1
struct MET_PRTL_DLMS SD_MET_PRTL_DLMS_PRTL_Frz[]=
{

    {{0x00100200},         {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},         {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},         {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {{0x00100204},         {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {{0x00100201},         {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {{0x00200201},         {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {{0x00200203},         {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {{0x00200204},     {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {{0x00200205},     {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {{0x00200202},     {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {{0x00300201},      {0x01,0x01,0x03,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300203},      {0x01,0x01,0x03,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300204},      {0x01,0x01,0x03,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300205},      {0x01,0x01,0x03,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300202},      {0x01,0x01,0x03,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {{0x00400201},      {0x01,0x01,0x04,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400203},      {0x01,0x01,0x04,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400204},      {0x01,0x01,0x04,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400205},      {0x01,0x01,0x04,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400202},      {0x01,0x01,0x04,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {{0x00500201},      {0x01,0x01,0x05,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500203},      {0x01,0x01,0x05,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500204},      {0x01,0x01,0x05,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500205},      {0x01,0x01,0x05,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500202},      {0x01,0x01,0x05,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {{0x00600201},     {0x01,0x01,0x06,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600203},     {0x01,0x01,0x06,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600204},     {0x01,0x01,0x06,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600205},     {0x01,0x01,0x06,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600202},     {0x01,0x01,0x06,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00700201},     {0x01,0x01,0x07,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700203},     {0x01,0x01,0x07,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700204},     {0x01,0x01,0x07,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700205},     {0x01,0x01,0x07,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700202},     {0x01,0x01,0x07,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00800201},      {0x01,0x01,0x08,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800203},      {0x01,0x01,0x08,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800204},      {0x01,0x01,0x08,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800205},      {0x01,0x01,0x08,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800202},      {0x01,0x01,0x08,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
#if 1
    //需量数据
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100201},    {0x01,0x01,0x01,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100202},    {0x01,0x01,0x01,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100203},    {0x01,0x01,0x01,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100204},    {0x01,0x01,0x01,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100205},    {0x01,0x01,0x01,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },

    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200201},    {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200202},    {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200203},    {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200204},    {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200205},    {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  

    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300201},     {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300202},     {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300203},     {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300204},     {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300205},     {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },

    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400201},     {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400202},     {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400203},     {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400204},     {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400205},     {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
#endif 

    //电压电流
    {{0x20000201},   {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //A电压
    {{0x20000202},   {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //B电压
    {{0x20000203},   {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //C电压      

    {{0x20010201},   {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //A电流
    {{0x20010202},   {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //B电流
    {{0x20010203},   {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //C电流     

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {{0x20050202},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20020201},   {0x01,0x01,0x51,0x07,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020202},   {0x01,0x01,0x51,0x07,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020203},   {0x01,0x01,0x51,0x07,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20030201},   {0x01,0x01,0x51,0x07,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},   {0x01,0x01,0x51,0x07,0x05,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},   {0x01,0x01,0x51,0x07,0x06,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    //电表事件类数据
    //{PRAM_TMS_ID,             {0x00,0x00,0x60,0x02,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{LAST_PRAM_TD_ID,           {0x00,0x00,0x60,0x02,0x01,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  3   },   
    //{BAT_WORK_STATE_ID,         {0x00,0x00,0x60,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  0   },   
    //{MET_CUR_DATE_TIME_ID,      {0x00,0x00,0x01,0x00,0x00,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  //电表日期时钟
//块抄******************************************************************
    {{0xffffffff},                  {0xff,0xff,0xff,0xff,0xff,0xff},0xff,0xff,0xff,0xff,0xff}
};
#else
MET_PRTL_DLMS SD_MET_PRTL_DLMS_PRTL_Frz[]=
{

    {{0x00100200},        {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {{0x00100202},        {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {{0x00100203},        {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {{0x00100204},        {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {{0x00100201},        {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {{0x00200201},        {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {{0x00200203},        {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {{0x00200204},        {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {{0x00200205},        {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {{0x00200202},        {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {{0x00300201},       {0x01,0x01,0x03,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300203},        {0x01,0x01,0x03,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300204},        {0x01,0x01,0x03,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300205},        {0x01,0x01,0x03,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00300202},        {0x01,0x01,0x03,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {{0x00400201},        {0x01,0x01,0x04,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400203},        {0x01,0x01,0x04,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400204},        {0x01,0x01,0x04,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400205},        {0x01,0x01,0x04,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {{0x00400202},       {0x01,0x01,0x04,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {{0x00500201},        {0x01,0x01,0x05,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500203},        {0x01,0x01,0x05,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500204},        {0x01,0x01,0x05,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500205},        {0x01,0x01,0x05,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {{0x00500202},        {0x01,0x01,0x05,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {{0x00600201},       {0x01,0x01,0x06,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600203},       {0x01,0x01,0x06,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600204},       {0x01,0x01,0x06,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600205},       {0x01,0x01,0x06,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00600202},       {0x01,0x01,0x06,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00700201},       {0x01,0x01,0x07,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700203},       {0x01,0x01,0x07,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700204},       {0x01,0x01,0x07,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700205},       {0x01,0x01,0x07,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {{0x00700202},       {0x01,0x01,0x07,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {{0x00800201},        {0x01,0x01,0x08,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800203},        {0x01,0x01,0x08,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800204},        {0x01,0x01,0x08,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800205},        {0x01,0x01,0x08,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {{0x00800202},        {0x01,0x01,0x08,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
#if 1
    //需量数据
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100201},     {0x01,0x01,0x01,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100202},     {0x01,0x01,0x01,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100203},     {0x01,0x01,0x01,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100204},     {0x01,0x01,0x01,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {{0x10100205},     {0x01,0x01,0x01,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },

    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200201},     {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200202},     {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200203},     {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200204},     {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {{0x10200205},     {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  

    {{0x10300201},      {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300201},      {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300202},      {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300202},      {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300203},      {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300203},      {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300204},      {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300204},      {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10300205},      {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {{0x10300205},      {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },

    {{0x10400201},      {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400201},      {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400202},      {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400202},      {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400203},      {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400203},      {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400204},      {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400204},      {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {{0x10400205},      {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {{0x10400205},      {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
#endif 

    //电压电流
    {{0x20000201},      {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //A电压
    {{0x20000202},      {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //B电压
    {{0x20000203},      {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //C电压      

    {{0x20010201},      {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //A电流
    {{0x20010202},      {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //B电流
    {{0x20010203},      {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //C电流     

    {{0x20040201},         {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {{0x20050202},         {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {{0x20020201},      {0x01,0x01,0x51,0x07,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020202},      {0x01,0x01,0x51,0x07,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20020203},      {0x01,0x01,0x51,0x07,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {{0x20030201},      {0x01,0x01,0x51,0x07,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030202},      {0x01,0x01,0x51,0x07,0x05,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {{0x20030203},      {0x01,0x01,0x51,0x07,0x06,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    //电表事件类数据
    //{PRAM_TMS_ID,             {0x00,0x00,0x60,0x02,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{LAST_PRAM_TD_ID,           {0x00,0x00,0x60,0x02,0x01,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  3   },   
    //{BAT_WORK_STATE_ID,         {0x00,0x00,0x60,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  0   },   
    //{MET_CUR_DATE_TIME_ID,      {0x00,0x00,0x01,0x00,0x00,0xff},  0x08, 0x0000, CLASS_02,  1, 1,  6,  0,  0   },  //电表日期时钟
//块抄******************************************************************
#if 0
    {OAD_POS_AE_BLK,           {0x01,0x01,0x01,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功总电能(表里是WH故扩大10倍存)
    {OAD_POS_AE_BLK,           {0x01,0x01,0x01,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功1电能(dlms与红相相连,发费率1发了后导致红相表长发)
    {OAD_POS_AE_BLK,           {0x01,0x01,0x01,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功2电能
    {OAD_POS_AE_BLK,           {0x01,0x01,0x01,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功3电能
    {OAD_POS_AE_BLK,           {0x01,0x01,0x01,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)正向有功4电能

    {OAD_NEG_AE_BLK,           {0x01,0x01,0x02,0x08,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功总电能
    {OAD_NEG_AE_BLK,           {0x01,0x01,0x02,0x08,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功1电能
    {OAD_NEG_AE_BLK,           {0x01,0x01,0x02,0x08,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功2电能
    {OAD_NEG_AE_BLK,           {0x01,0x01,0x02,0x08,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功3电能
    {OAD_NEG_AE_BLK,           {0x01,0x01,0x02,0x08,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //(当前)反向有功4电能

    {OAD_CMB_RE1_BLK,       {0x01,0x01,0x03,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_CMB_RE1_BLK,       {0x01,0x01,0x03,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_CMB_RE1_BLK,       {0x01,0x01,0x03,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_CMB_RE1_BLK,       {0x01,0x01,0x03,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_CMB_RE1_BLK,       {0x01,0x01,0x03,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  

    {OAD_CMB_RE2_BLK,       {0x01,0x01,0x04,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {OAD_CMB_RE2_BLK,       {0x01,0x01,0x04,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {OAD_CMB_RE2_BLK,       {0x01,0x01,0x04,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {OAD_CMB_RE2_BLK,       {0x01,0x01,0x04,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          
    {OAD_CMB_RE2_BLK,       {0x01,0x01,0x04,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },          

    {OAD_Q1_RE_BLK,     {0x01,0x01,0x05,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {OAD_Q1_RE_BLK,     {0x01,0x01,0x05,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {OAD_Q1_RE_BLK,     {0x01,0x01,0x05,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {OAD_Q1_RE_BLK,     {0x01,0x01,0x05,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
    {OAD_Q1_RE_BLK,     {0x01,0x01,0x05,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },        
 
    {OAD_Q2_RE_BLK,    {0x01,0x01,0x06,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q2_RE_BLK,    {0x01,0x01,0x06,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q2_RE_BLK,    {0x01,0x01,0x06,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q2_RE_BLK,    {0x01,0x01,0x06,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q2_RE_BLK,    {0x01,0x01,0x06,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {OAD_Q3_RE_BLK,    {0x01,0x01,0x07,0x08,0x00,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q3_RE_BLK,    {0x01,0x01,0x07,0x08,0x01,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q3_RE_BLK,    {0x01,0x01,0x07,0x08,0x02,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q3_RE_BLK,    {0x01,0x01,0x07,0x08,0x03,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         
    {OAD_Q3_RE_BLK,    {0x01,0x01,0x07,0x08,0x04,0xff}, 0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },         

    {OAD_Q4_RE_BLK,     {0x01,0x01,0x08,0x08,0x00,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_Q4_RE_BLK,     {0x01,0x01,0x08,0x08,0x01,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_Q4_RE_BLK,     {0x01,0x01,0x08,0x08,0x02,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_Q4_RE_BLK,     {0x01,0x01,0x08,0x08,0x03,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   },  
    {OAD_Q4_RE_BLK,     {0x01,0x01,0x08,0x08,0x04,0xff},    0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  1   }, 
#if 1
    //需量数据
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   },  
    {OAD_POS_MAX_AD_BLK,      {0x01,0x01,0x01,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },

    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  3,  0,  1   }, 
    {OAD_NEG_MAX_AD_BLK,      {0x01,0x01,0x02,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1, 1,  5,  0,  0   },  

    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   },  
    {OAD_CMB_MAX_RD1_BLK,      {0x01,0x01,0x03,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },

    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x00,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x01,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x01,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x02,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x02,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x03,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x03,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x04,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  3,  0,  1   }, 
    {OAD_CMB_MAX_RD2_BLK,      {0x01,0x01,0x04,0x06,0x04,0xff},  0x20, 0x0000, CLASS_02,  1,    1,  5,  0,  0   },  
#endif 

    //电压电流
    {OAD_VLT_BLK,    {0x01,0x01,0x20,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //A电压
    {OAD_VLT_BLK,    {0x01,0x01,0x34,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //B电压
    {OAD_VLT_BLK,    {0x01,0x01,0x48,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  1   },  //C电压    

    {OAD_CUR_BLK,    {0x01,0x01,0x1f,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //A电流
    {OAD_CUR_BLK,    {0x01,0x01,0x33,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //B电流
    {OAD_CUR_BLK,    {0x01,0x01,0x47,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,  1,  2,  0,  3   },  //C电流   

    {OAD_AP_BLK,           {0x01,0x01,0x10,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //有功功率
    {OAD_RP_BLK,           {0x01,0x01,0x83,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1,    1,  4,  0,  1   },  //无功功率

    //功率因素好像有点问题(可能是表的问题) 山东DLMS表 未提供此数据项
    //{RT_Z_PF_EV_ID,             {0x01,0x01,0x0d,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },   
    //{RT_PF_A_PHASE_EV_ID,       {0x01,0x01,0x21,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //{RT_PF_B_PHASE_EV_ID,       {0x01,0x01,0x35,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },      
    //{RT_PF_C_PHASE_EV_ID,       {0x01,0x01,0x49,0x07,0x00,0xff},  0x08, 0x0000, CLASS_01,  1, 1,  4,  0,  3   },                                                            
    //电压电流相位角
    {OAD_VLT_ANG_BLK,    {0x01,0x01,0x51,0x07,0x00,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {OAD_VLT_ANG_BLK,    {0x01,0x01,0x51,0x07,0x01,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {OAD_VLT_ANG_BLK,    {0x01,0x01,0x51,0x07,0x02,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   

    {OAD_CUR_ANG_BLK,    {0x01,0x01,0x51,0x07,0x04,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {OAD_CUR_ANG_BLK,    {0x01,0x01,0x51,0x07,0x05,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
    {OAD_CUR_ANG_BLK,    {0x01,0x01,0x51,0x07,0x06,0xff},   0x08, 0x0000, CLASS_01,  1, 1,  2,  0,  1   },   
#endif
    {{0xffffffff},                    {0xff,0xff,0xff,0xff,0xff,0xff},0xff,0xff,0xff,0xff,0xff}
};

#endif




//根据数据类型获取数据块长度（含类型字节及数据），firByte为类型后的第一个字节,返回0失败
uint16 getDataLenByType(uint8 datatype,uint8 *firByte)
{
//  int ret;
    switch (datatype)
    {
    case 0:
        return 1+0;
    case 1:
        return (*firByte)*getDataLenByType(*(firByte+1),firByte+2)+2;
    case 2:
        {
            uint8 i,num;
            int sum=0,len=0;
//          uint8 *p;
            num=*firByte;
            for (i=0; i<num; i++)
            {
                len=getDataLenByType(*(firByte+1),firByte+2);
                if (len==0)
                    return 0;
                sum+=len;
                firByte+=len;
            }
            return sum+2;
        }
    case 3:
    case 4:
    case 0x11:
        return 1+1;
    case 5:
    case 6:
    case 0x17:
    case 0x1B:
        return 4+1;
    case 9:
    case 0x0A:
        return (*firByte)+2;
    case 0x0D:
    case 0x0F:
    case 0x16:
        return 1+1;
    case 0x10:
    case 0x12:
        return 2+1;
    case 0x13:
        return 0;
    case 0x14:
    case 0x15:
    case 0x18:
        return 8+1;
    case 0x19:
        return 12+1;
    case 0x1A:
        return 5+1;
    default:
        return 0;
    }
}

/*-
***************************************************************************
*模块编号:
*名称:CheckRespondFrame
*功能:数据返回帧进行检测
*输入参数:电表的物理地址
*输出参数:recvBuf, met_addr, step(对独立的步骤进行单独处理)
*返回:检测结果0:失败; 1:正确
***************************************************************************
-*/
static int CheckRespondFrame(uint8 *recvBuf, uint16 recvLen, uint8 *met_addr, uint8 step,uint8 RRFlag,S_COMM_INFO_MET *Comm_Info_dlms)
{
    uint16 i = 0;
    uint16 k = 0;
    uint16 len = 0;
    uint16 trialfcs;
    uint16 phyaddr = 0;
    uint8 hdlcaddr[4];
    uint8 pBuf[10];
    DLMS_Association_OBJ_LIST * pDlmsObjbjlist;

    
    pDlmsObjbjlist=&Comm_Info_dlms->dlmsAssociation_OBJ_LIST;
    
    phyaddr = ((met_addr[0])>>4)*10+(met_addr[0]&0x0f)+\
        (met_addr[1]&0x0f)*100 + ((met_addr[1])>>4)*1000;

    HdlcAddrTran(hdlcaddr, 0x01, phyaddr);


    while(i < (recvLen-8))
    {
        if(((recvBuf[i] == 0x7e)&&(recvBuf[i+4] == hdlcaddr[0])
            &&(recvBuf[i+5] == hdlcaddr[1])&&(recvBuf[i+6] == hdlcaddr[2])
            &&(recvBuf[i+7] == hdlcaddr[3]))
            )//||(recvLen == 0x1a))
        {
            pBuf[1] = recvBuf[i+1]&0x03; //后面三位是长度的高位，高4位是帧类型，高第5位是长短帧标志
            pBuf[0] = recvBuf[i+2];
            memcpy(&len, pBuf, 2);
        
            len &= 0x03ff; //获取长度
            trialfcs = pppfcs16(0xffff, &recvBuf[i+1], len-2);//数据帧校验
            trialfcs ^= 0xffff;                             /* complement */
            pBuf[0] = (trialfcs & 0x00ff);          /* least significant byte first */
            pBuf[1] = ((trialfcs >> 8) & 0x00ff);
            

    
            if((pBuf[0] == recvBuf[i+len-1])&&(pBuf[1] == recvBuf[i+len]))  //计算出得校验与发送来的校验一致
            {
                if(RRFlag == 1)
                {
                    //若是长帧，则接收序列号不变
                }
                else
                {
                    RECV_SEQ = (recvBuf[i+8]&0xe0);
                    Comm_Info_dlms->recvseq = RECV_SEQ;
                }


                SEND_SEQ = (recvBuf[i+8]&0x0f);//0001
                Comm_Info_dlms->sendseq = SEND_SEQ;
                if(step == DLMS_READ_SN_STAT ||step == DLMS_RR_FIRST_STAT )
                {
                    MET_PRTL_DLMS *pDLMS_PRTL;
                    pDLMS_PRTL = SD_MET_PRTL_DLMS_PRTL;
                    k = i;
                    while(k < (recvLen-3))
                    {
                        if((recvBuf[k] == 0x02)&&(recvBuf[k+1] == 0x04)&&(recvBuf[k+2] == 0x10))
                        {
                            pDLMS_PRTL = SD_MET_PRTL_DLMS_PRTL;
                            
                            while(pDLMS_PRTL->oad.value != 0xffffffff)
                            {
                                if(memcmp(pDLMS_PRTL->oBis, &recvBuf[k+12], 6) == 0)
                                {
    
                                    TASK_FMT_DEBUG( 2, REC_LOG,"pDLMS_PRTL->oad.value is %x\n", pDLMS_PRTL->oad.value);
                                    TASK_BUF_TRACE( 2, REC_LOG, pDLMS_PRTL->oBis, 6);
                                    TASK_BUF_DEBUG( 2, REC_LOG,&recvBuf[k+3],2);
                                    if(pDLMS_PRTL->ClassNo !=7)
                                    {
                                        pDLMS_PRTL->ShortName = (recvBuf[k+3]<<8)&0xff00;
                                        pDLMS_PRTL->ShortName |= recvBuf[k+4];
                                        pDLMS_PRTL->ShortName += pDLMS_PRTL->offset;
                                    }
                                    else
                                    {
                                        pDLMS_PRTL->ShortName = (recvBuf[k+3]<<8)&0xff00;
                                        pDLMS_PRTL->ShortName |= recvBuf[k+4];
                                    }
                                    TASK_FMT_DEBUG( 2, REC_LOG,"pDLMS_PRTL->ShortName is %x\n", pDLMS_PRTL->ShortName);
                                    TASK_FMT_DEBUG( 2, REC_LOG,"pDLMS_PRTL->offset is %x\n", pDLMS_PRTL->offset);
                                    memcpy(pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].oBis,&recvBuf[k+12],6);
                                    pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].sn=((recvBuf[k+3]<<8)&0xff00);
                                    pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].sn|=recvBuf[k+4];
                                    pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].class_id=recvBuf[k+7];
                                    pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].version=recvBuf[k+9];
                                    pDlmsObjbjlist->obj[pDlmsObjbjlist->objNum].oad.value = pDLMS_PRTL->oad.value;
                                    pDlmsObjbjlist->objNum++;
                                }
                                pDLMS_PRTL++;
                            }
                        }
                        k++;
                    }
                    //if(cmpResult == 0)
                    //{
                    //  return 0;
                    //}                 
                }
                
                if((step == DLMS_AARQ_STAT)||(step == DLMS_AARQ_AGAIN_STAT))
                {
                    if(recvLen > 45)
                    {
                        if((recvBuf[i+31] != 0x00)||(recvBuf[i+38] != 0x00))
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                }
                
                /*if(step == 0xff)
                {
                    static int cnttimes = 0;

                    if(cnttimes == 2)
                    {
                        cnttimes++;
                        recvBuf[i+8] = 0x93;
                    }
                    else
                    {
                        cnttimes++;
                    }
                }
                */
                if(recvBuf[i+8] == 0x97)//拒绝连接的帧
                {
                    return 6;
                }
                
                if(step == 0xff)//读取数据时               
                {
                    if((recvBuf[i+11] == 0xe6)&&(recvBuf[i+12] == 0xe7)
                        &&(recvBuf[i+13] == 0x00)&&(recvBuf[i+14] == 0x0C))
                    {
                        if(recvBuf[i+1] == 0xa8)    //有后续数据
                        {
                            return 2;
                        }
                        else
                        {
                            return 1;
                        }
                    }
                    else
                    {
                        return 0;
                    }
                }
                
                if(step == 0xee)//读取数据对后续帧进行检测              
                {
                    if(recvBuf[i+1] == 0xa8)    //有后续数据
                    {
                        return 2;
                    }
                    else
                    {
                        return 1;
                    }
                }
                return 1;
            }
            else
            {
                if(RRFlag == 1)
                {
                    RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
                    Comm_Info_dlms->recvseq = RECV_SEQ;
                }
                return 0;
            }
        }
        else
        {
            i++;
        }
    }
    if(RRFlag == 1)
    {
        RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
        Comm_Info_dlms->recvseq = RECV_SEQ;
    }
    
    return 0;
}

/*-
***************************************************************************
*模块编号:7-10-5
*名称:ABB规约根据接收到的报文，判断帧的有效性
*功能:ABB规约根据接收到的报文，判断帧的有效性
*输入参数:通道号（pipe）、抄表任务结构体（*pTaskAllot）、接收数据存放的缓冲区的地址（*pBuf）、帧长存放地址（rcvLen）
*输出参数:
*处理:检测第三个字节是否为0x00，如果是，返回1；否则，返回0。
*返回:  0校验失败
*        1校验成功
***************************************************************************
-*/

uint8 DLMS_CheckRspFrame(uint8 pipe, cbMtrPara MtrPara, S_COMM_INFO_MET *Comm_Info_dlms)
{
    int     framelen=0;
    uint8   fraNum = 0;

    
    if (Comm_Info_dlms->HDSKFraNum < Comm_Info_dlms->HDSKFraTotal)/*-握手帧报文判断-*/
    {
        fraNum = Comm_Info_dlms->HDSKFraNum;
        
   
        
        switch (Comm_Info_dlms->pHDSKCont[fraNum])
        {
            case DLMS_SNRM_STAT:/*-需要得到密钥-*/
            case DLMS_SNRM_AGAIN_STAT:
                {
                    return CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, Comm_Info_dlms->pHDSKCont[fraNum], 0,Comm_Info_dlms);
                }
                break;
                
            case DLMS_DISC_STAT:
            case DLMS_AARQ_STAT:/*-校验密码帧-*/
            case DLMS_AARQ_AGAIN_STAT:
                {
                    int nret =  CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, Comm_Info_dlms->pHDSKCont[fraNum], 0,Comm_Info_dlms);
                    return nret;
                }
                break;
                
            case DLMS_READ_SN_STAT:
                {
                    framelen = CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, Comm_Info_dlms->pHDSKCont[fraNum], 0,Comm_Info_dlms);
                    RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
                    Comm_Info_dlms->recvseq = RECV_SEQ;
                    return framelen;
                }
                break;

            case DLMS_RR_FIRST_STAT:/*-校验密码帧-*/
                {
                    return CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, Comm_Info_dlms->pHDSKCont[fraNum], 1,Comm_Info_dlms);
                }
                break;
                
            default:return 0;
        }
    }
    else/*-读取报文帧判断-*/
    {
        if (Comm_Info_dlms->FraTimes != 0)
        {
            return CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, 0xee, 1,Comm_Info_dlms);
        }
        else
        {
            return CheckRespondFrame(Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara.MetAddr, 0xff, 0,Comm_Info_dlms);
        }
    }
    return 0;
}


/*+++  功能：组织从主站（客户端）到集中器的MAC Frame  
参数： unsigned char jzq[4],
目的地址，为采集器地址（已经遵从了HDLC扩展地址规则）         
unsigned char ctrl_code 控制字节         
unsigned char *llc_info LLC 数据         
int     len_llc_info    LLC 数据长度  
返回:  MAC层数据祯的长度  
描述： 主站应该知道自己的地址，会自动加入 HDLC_MSA---*/
uint8  make_mac_frame(uint8 *frame, uint8 jzq[4], uint8 ctrl_code, uint8 *llc_info, int len_llc_info)
{    
    int pos=0;    
    int frameLen=len_llc_info+12;   

    //如果信息域长度为零，则HCS就作为了FCS. 
    if(len_llc_info==0) 
        frameLen-=2;   
    
    frame[0] = 0x7E; 
    //flag   
    frame[1] = 0xA0 | ((frameLen >>8) & 0x03);    
    frame[2] = (uint8 )frameLen;       

    //目的地址    
    frame[3] = jzq[0];   
    frame[4] = jzq[1];   
    frame[5] = jzq[2];   
    frame[6] = jzq[3];   
    //源地址    
    frame[7] = HDLC_MSA;   
    //控制   
    frame[8] = ctrl_code;   
    //HCS    
    fcs16(frame+1,8);  
    //信息   
    pos = 11;    
    if(len_llc_info > 0)   
    {       
        memcpy(frame+11, llc_info, len_llc_info);     
        //FCS      
        fcs16(frame+1, 10+len_llc_info);      
        pos += len_llc_info + 2;  //FCS为2个字节    
    }   
    //标志   
    frame[pos++] = 0x7E;   
    //memset(&frame[pos], 0, 5);
    //返回祯全部长度   
    //pos += 5;
    return pos;
}

/*-
***************************************************************************
*模块编号:
*名称:发送DISC断连帧
*功能:对DISC进行组帧
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
uint8 DISC_MakeFrame(uint8 *pBuf, uint16 metaddr)
{
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   

    len_llc_frame = 0;
    ctrl_code = 0x53;
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    return make_mac_frame(pBuf,hdlcaddr,ctrl_code,axdr,len_llc_frame);
}

/*+++
  功能: 初始化BER编码器
---*/
static void BER_init(PBER pBer)
{
   pBer->len = 0;
   pBer->pos = 0;
}


/*+++
  功能：设置一个COSEM_OID对象的值。
  描述：COSEM Object Identifier的前5个整数为固定值：
        DLMS_UA ::=
        joint-iso-ccitt(2) country(16) country-name(756) identified-organnization(5)
        dlms_UA(8)
  参数：
        DLMS_UA(2-16-756-5-8) name_type(?) name_id(?)

        name_type,name_id都是小于127的值。

---*/
static void set_cosem_oid(COSEM_OID *pOid,int name_type,int name_id)
{
    //默认使用universal 的OBJECT IDENTIFIER的 tag
    //可以根据实际的tag值进行修改
    pOid->object_identifier_tag = 0x06;

    //对于COSEM_OID来讲，为7
    pOid->object_identifier_len = 07;

    pOid->object_id[0] = 0x60;   //2*40+16
    pOid->object_id[1] = 0x85;   //country-name(756)
    pOid->object_id[2] = 0x74;   //
    pOid->object_id[3] = 0x05;   //identified-organization(5)
    pOid->object_id[4] = 0x08;   //dlms_UA(8)

    pOid->object_id[5] = name_type;   //name_type
    pOid->object_id[6] = name_id;   //name_id
}


/*+++
   功能: 向BER编码器中添加一个长度域
---*/
static void BER_add_length(PBER pBer,unsigned int len)
{
   unsigned char count,lenBytes[5];

   if(len < 128)
   {
       //short form. One octet, Bit8 has value "0" and bit7-1 give the length
       pBer->bercode[pBer->pos++] = (unsigned char)len;
   }
   else
   {
       //long form. Two to 127 octets. Bit8 of first octet has value "1" and
       //     bit7-1 give the number of additional length octets. Second and
       //     following octets give the length, base 256, most significant digit first
       count = 0;
       while(len > 0)
       {
          lenBytes[count++] =(unsigned char )len;
          len >>=8;
       }
       pBer->bercode[pBer->pos++] = 0x80 | count;
       while(count > 0)
       {
           --count;
           pBer->bercode[pBer->pos++] = lenBytes[count];
       }
   }
   pBer->len = pBer->pos;
}

/*+++
  功能：向A-XDR编码对象中添加一个ASN.1编码标记值
  参数: int tag 标记值   ,只考虑2字节
        unsigned char class_type 标签值得类别：universal，application,context_speci,private
---*/
static void  AXDR_add_asn1_tag(A_XDR *pAXDR,uint16 tag,unsigned char class_type)
{
   unsigned char count,tagBytes[5];

   if(tag < 31)
   {
       // low_tag_number_form
       pAXDR->code[pAXDR->pos++] = class_type | tag;
   }
   else
   {
       // High_tag_number_form   first byte's bit5-1 all have value "1"
       pAXDR->code[pAXDR->pos++] = class_type | 0x1F;
       // Second and following octets give the tag number,base 128.
       // most significant digit first,with as few digits as possible,
       // and with the bit 8 of each octet except the last set to "1"
       count = 0;
       while(tag > 0)
       {
          tagBytes[count++] =(unsigned char ) (tag & 0x7F);
          tag >>=7;
       }
       while(count > 1)     //用5f就行了, 无须用5f、1f
       {
           --count;
           if(count!=0) tagBytes[count] |= 0x80;
           pAXDR->code[pAXDR->pos++] = tagBytes[count];
       }
   }
   pAXDR->len  = pAXDR->pos;

}


/*+++
  功能: 向BER编码器中添加一个对象
  参数:
---*/
static void BER_add(PBER pBer,uint16 tag,unsigned char class_type,unsigned char *content,unsigned int content_len)
{
    AXDR_add_asn1_tag((PAXDR)pBer,tag,class_type);
    BER_add_length(pBer,content_len);
    while(content_len > 0)
    {
       content_len--;
       pBer->bercode[pBer->pos ++ ] = *content ++;
    }

    pBer->len = pBer->pos;
}

/*+++
  功能：初始化一个BITSTRING，
  参数：int bitlen 比特数，最大值等于40
---*/

static void BITSTRING_init(BITSTRING *pBitString,int bitlen)
{
   int i;

   pBitString->bytelen = bitlen>>3;
   pBitString->unused = (unsigned char )(bitlen & 0x07);
   if(pBitString->unused > 0) pBitString->bytelen ++;

   for(i=0;i<pBitString->bytelen;i++) pBitString->bits[i] = 0x00;
 }

/*+++
  功能：初始化一个A-XDR编码对象
---*/
static void AXDR_init(A_XDR *pAXDR)
{
   pAXDR->len = 0;
   pAXDR->pos = 0;
}

/*+++
  功能：向A-XDR编码对象中添加一个编码标记值
---*/
static void  AXDR_add_tag(A_XDR *pAXDR,int tag)
{
   pAXDR->code[pAXDR->pos] = tag;

   pAXDR->len ++;
   pAXDR->pos ++;
}

/*+++
  功能：向BER-AXDR编码对象中添加Unsigned8类型的数据
  描述：unsigned8类型的数据的A-XDR编码就是它的值
---*/
static void AXDR_add_unsigned8(A_XDR *pAXDR,uint8 value)
{
   pAXDR->code[pAXDR->pos] = value;
   pAXDR->len ++;
   pAXDR->pos ++;
}


/*+++
  功能:以指定的标签包裹当前BER对象
---*/
static void BER_wrap(PBER pBer,uint16 tag,unsigned char class_type)
{
   int len,pos;

   //首先,把当前编码值右移,使得靠近右边
   len = pBer->len;
   pos =  MAX_BER_CODE_LEN;
   while(len > 0)
     pBer->bercode[--pos] = pBer->bercode[--len];
   //第二步,添加新的标签
   pBer->pos = 0;
   len = pBer->len;
   AXDR_add_asn1_tag((PAXDR)pBer,tag,class_type);
   BER_add_length(pBer,len);
   
   //第三步,把原来的编码转移过来
   while(len > 0)
   {
      pBer->bercode[pBer->pos ++] = pBer->bercode[pos++];
      --len;
   }
   pBer->len = pBer->pos;
}

/*++
  功能: 向BER-AXDR编码对象中添加BITSTRING类型的数据
  描述: 首先添加编码长度,然后是最后字节中不使用比特数,然后是比特数据
---*/
static void AXDR_add_bitstring(A_XDR *pAXDR,BITSTRING *pBitString)
{
    int i;

    //首先添加编码长度,注意要包含一个字节,用来指示最后字节中的不使用比特数
    pAXDR->code[pAXDR->pos ++] = pBitString->bytelen + 1;

    //BITSTRING的最后字节中不使用的比特数
    pAXDR->code[pAXDR->pos ++] = pBitString->unused;

    //位串值的编码
    for(i=0; i<pBitString->bytelen;i++)
        pAXDR->code[pAXDR->pos ++] = pBitString->bits[i];

    pAXDR->len = pAXDR->pos;

}

/*+++
  功能：向BER-AXDR编码对象中添加Unsigned16类型的数据
  描述：unsigned16类型的数据的A-XDR编码就是它的值,高字节在前
---*/
static void AXDR_add_unsigned16(A_XDR *pAXDR,uint16 value)
{
   pAXDR->code[pAXDR->pos++] = (unsigned char)(value >>8);
   pAXDR->code[pAXDR->pos++] = (unsigned char)value;
   pAXDR->len +=2;
}
/*-
***************************************************************************
*模块编号:
*名称:make_llc_frame
*功能:向BER编码器追加一组编码
*输入参数:info
*输出参数:info
*返回:LLC 信息长度
***************************************************************************
-*/
static void BER_append(PBER pBer,unsigned char *code,unsigned int code_len)
{
    while(code_len > 0)
    {
       --code_len;
       pBer->bercode[pBer->pos ++ ] = *code ++;
    }
    pBer->len = pBer->pos;
}

/*-
***************************************************************************
*模块编号:
*名称:发送AARQ帧
*功能:对AARQ进行组帧
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
static int AARQ_MakeFrame(uint8 *pBuf, uint16 metaddr)
{
    //int i;
    BER   aarq;
    A_XDR   initiateRequest;
    BITSTRING  conformance;
    BER      application_context_name;
    BER       user_information;
    COSEM_OID   cosem_oid;
    COSEM_OID   mechanism_name;
    uint8 hdlcaddr[10];   
    uint8 ctrl_code = 0;
    //Memo_AARE->Lines->Clear();

    //组件Application-context-name
    BER_init(&application_context_name);

    //使用短名
    set_cosem_oid(&cosem_oid,1,2);
    BER_add(&application_context_name,01,CLASS_CONTEXT_SPECIFIC | ENCODE_CONSTRUCTED,(unsigned char *)&cosem_oid,9);

    //proposed_conformance bitstring
    BITSTRING_init(&conformance,24);
    //直接写死,其实可读就行
    conformance.bits[0] = 0x18;
    conformance.bits[1] = 0x03;
    conformance.bits[2] = 0x20;

    //计入User-Information的长度
    AXDR_init(&initiateRequest);

    //   DLMS PDU中InitiateRequest的编码标记
    AXDR_add_tag(&initiateRequest,0x01);

    if(0)//(CheckBox_dedicated_key->Checked)
    {
       //   dedicated-key 组件的使用标记：使用,添加数据NULL
        AXDR_add_tag(&initiateRequest,0x01);
        AXDR_add_unsigned8(&initiateRequest,0x00);
    }
    else
    {
      //   dedicated-key 组件的使用标记：不使用
      AXDR_add_tag(&initiateRequest,0x00);
    }

    //   response-allowed组件de使用标志：不使用，缺省值为真
    AXDR_add_tag(&initiateRequest,0x00);
    //   proposed-quality-of-service组件的使用标志：不出现
    AXDR_add_tag(&initiateRequest,0x00);
    //   proposed-dlms-version-number组件的编码（unsigned8,value=6）
    AXDR_add_unsigned8(&initiateRequest,0x06);
    //   对Conformance block[APPLICATION 31] IMPLICIT BITSTRING(SIZE(24))进行编码
    AXDR_add_asn1_tag(&initiateRequest,31,CLASS_APPLICATION);
    //   Conformance的内容进行编码
    AXDR_add_bitstring(&initiateRequest,&conformance);
    //   对client-max-receive-pdu-size组件的编码(unsigned16,)
    AXDR_add_unsigned16(&initiateRequest,0);
    
    //user-information字段组件([30],Context-specific)
    BER_init(&user_information);
    //   user-information 的内容为A-XDR编码的initiateRequest的OCTETSTRING
    BER_add(&user_information,04,CLASS_UNIVERSAL,initiateRequest.code,initiateRequest.len);

    BER_wrap(&user_information,30,CLASS_CONTEXT_SPECIFIC | ENCODE_CONSTRUCTED);

    // AARQ APDU [APPLICATION 0] 的编码标记
    BER_init(&aarq);

    // 80020780
    //添加组件Protocol version tag,length and encoded value  (bit-string)
    if(1)
    {
         unsigned char protocol_version[4];
         protocol_version[0] = 0x80;
         protocol_version[1] = 0x02;
         protocol_version[2] = 0x07;
         protocol_version[3] = 0x80;
         BER_append(&aarq,protocol_version,4);
    }
    // 添加组件application-context-name
    BER_append(&aarq,application_context_name.bercode,application_context_name.len);

    //添加组件：sender-acse-requirements
    if(0)       //sender-acse-requirements置位
    {
         //如果出现，它包含BIT-STRING{authentication(0)}的值
         unsigned char sender_acse_req[4];
         sender_acse_req[0] = 0x8A;  //tag
         sender_acse_req[1] = 0x02;  //length
         sender_acse_req[2] = 0x07;  //unused bit
         sender_acse_req[3] = 0x80;  //bits
         BER_append(&aarq,sender_acse_req,4);
    }

    //添加组件：身份验证机制
    if(0)
    {
         //组件Mechanism-name
         if(0)                  //P0级
            set_cosem_oid(&mechanism_name,2,0);
         else if(1)             //P1级
            set_cosem_oid(&mechanism_name,2,1);
         else                   //P2级
            set_cosem_oid(&mechanism_name,2,2);
         mechanism_name.object_identifier_tag = 0x8B;

         BER_append(&aarq,(unsigned char *)&mechanism_name,9);
    }

    //添加组件:Calling-Authentication-value
    if(0)               //添加密码默认为"00000000"
    {
        unsigned char authentication_value[50];
        int len;
        authentication_value[0]=  CLASS_CONTEXT_SPECIFIC | 0x00;  //charstring
        len = 0x08;
        authentication_value[1] = len;
        authentication_value[2] = 0x30;
        authentication_value[3] = 0x30;
        authentication_value[4] = 0x30;
        authentication_value[5] = 0x30;
        authentication_value[6] = 0x30;
        authentication_value[7] = 0x30;
        authentication_value[8] = 0x30;
        authentication_value[9] = 0x30;
        BER_add(&aarq,12,CLASS_CONTEXT_SPECIFIC | ENCODE_CONSTRUCTED,authentication_value,authentication_value[1]+2);
    }
    
    // 添加组件user-information
    BER_append(&aarq,user_information.bercode,user_information.len);

    // 包裹
    BER_wrap(&aarq,0,CLASS_APPLICATION | ENCODE_CONSTRUCTED);

    //组织LLC数据祯
    aarq.len = make_llc_frame(aarq.bercode,aarq.len);

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    uint8 sendseq = ((SEND_SEQ) >>1);
    ctrl_code = ((RECV_SEQ| (sendseq <<1))|0x10);
    return make_mac_frame(pBuf,hdlcaddr,ctrl_code,aarq.bercode,aarq.len);
}


int Read_CAPTURE_DATA_MakeFrame(S_COMM_INFO_MET *pdlms_comm, uint8 *pBuf, uint16 metaddr)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   
    //uint8 ShortNameLen = 0;
    //int returnLen;
    DateTimeHex_t dateTimeHex;
    uint16 base_name;
    uint16 year=0;

    if (!pdlms_comm)
        return 0;
    
    axdr[pos++] = DLMS_SN_CMDID_READ;  //[195] Get_request
    axdr[pos++] = 0x01;
    axdr[pos++] = 0x04;//条件访问

    if (pdlms_comm->frzType>=4)
        return 0;
    
    base_name=get_DLMS_Association_OBJ(pdlms_comm,g_frzObjObis[pdlms_comm->frzType].obis);
    if((base_name ==0xFFFF)||(base_name ==0x0000))
        return 0;

    
    base_name+=8; //3. capture_objects      (static)     class_id = 7 属性3
    axdr[pos++] = (uint8)((base_name>>8)&0x00ff); //0x42;//42 98 
    axdr[pos++] = (uint8)(base_name&0x00ff);  //0x98+24;

////////////////////
//  axdr[pos++] = 0x01;
//  axdr[pos++] = 0x04;//条件访问
//  axdr[pos++] = (uint8)(ShortName>>8);
//  axdr[pos++] = (uint8)(ShortName&0xff);  

    uint8 fromto[]={0x01,
        0x02,0x04,
            0x02,0x04,
                0x12,0x00,0x08,
                0x09,0x06,0x00,0x00,0x01,0x00,0x00,0xFF,
                0x0F,0x02,
                0x12,0x00,0x00};
//              0x09,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//              0x09,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    //      0x01,0x00};
    memcpy(&axdr[pos],fromto,sizeof(fromto));   
    pos+=sizeof(fromto);

    struct tm ptm;
    Date_Time_t_dlms fromTime,toTime;
    
    time_t now_t = time(NULL);
    localtime_r(&now_t, &ptm);

    DT_Time2DateTimeHex(now_t,(DateTimeHex_t*)(&dateTimeHex));//方法2中的结束时间
    year =(dateTimeHex.year_h<<8)+dateTimeHex.year_l;
    // 07D603160F0A35000080000F From data : 2006-03-22 10:53:00
    fromTime.year_h = dateTimeHex.year_h;
    fromTime.year_l = dateTimeHex.year_l;
    fromTime.month = dateTimeHex.month;
    fromTime.day_of_month = dateTimeHex.day;
    fromTime.day_of_week = CalWeekday(year,dateTimeHex.month,dateTimeHex.day);;
    fromTime.hour = (dateTimeHex.hour);
    fromTime.min = (dateTimeHex.min);
    fromTime.sec = 0;//(ptm.tm_sec);
    fromTime.hundredths = 0;
    fromTime.deviation_h = 0x80;
    fromTime.deviation_l = 0;
    fromTime.clockstatus = 0x0f;

    memcpy(&toTime,&fromTime,sizeof(fromTime));

    switch(pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId])
    {
    case DLMS_DLMS_CAPTURE_DATA_DAY:
        fromTime.hour = 0;//(ptm.tm_hour);
        fromTime.min = 0;//(ptm.tm_min);
        toTime.hour = 12;//(ptm.tm_hour); //跨度 当日00:00~当日12:00
        toTime.min = 0;//(ptm.tm_min);
        break;
        
    case DLMS_DLMS_CAPTURE_DATA_MON:
        now_t = now_t-(fromTime.day_of_month-1)*24*3600;//转到当月的1号
        //localtime_r(&now_t, &ptm);

        DT_Time2DateTimeHex(now_t,(DateTimeHex_t*)(&dateTimeHex));//方法2中的结束时间
        // 07D603160F0A35000080000F From data : 2006-03-22 10:53:00
        year =(dateTimeHex.year_h<<8)+dateTimeHex.year_l;
        
        fromTime.year_h = dateTimeHex.year_h;
        fromTime.year_l = dateTimeHex.year_l;
        fromTime.month = dateTimeHex.month;
        fromTime.day_of_month = dateTimeHex.day;
        fromTime.day_of_week = CalWeekday(year,dateTimeHex.month,dateTimeHex.day);
        fromTime.hour = 0;//(ptm.tm_hour);
        fromTime.min = 0;//(ptm.tm_min);
        fromTime.sec = 0;//(ptm.tm_sec);
        fromTime.hundredths = 0;
        fromTime.deviation_h = 0x80;
        fromTime.deviation_l = 0;
        fromTime.clockstatus = 0x0f;

        memcpy(&toTime,&fromTime,sizeof(fromTime));
        toTime.hour = 12;//(ptm.tm_hour); //跨度 1日00:00~1日12:00
        break;
        
    case DLMS_DLMS_CAPTURE_DATA_CURVE1://取最近一个整15分钟的数据，电表中冻结的曲线1是15分钟一个点，整15分钟
    case DLMS_DLMS_CAPTURE_DATA_CURVE2://电表中冻结的曲线2是5分钟一个点，整5分钟
        now_t=(now_t/900)*900;//-60;//往前1分钟
        //localtime_r(&now_t, &ptm);

        DT_Time2DateTimeHex(now_t,(DateTimeHex_t*)(&dateTimeHex));//方法2中的结束时间
        year =(dateTimeHex.year_h<<8)+dateTimeHex.year_l;
        fromTime.year_h = dateTimeHex.year_h;
        fromTime.year_l = dateTimeHex.year_l;
        fromTime.month = dateTimeHex.month;
        fromTime.day_of_month = dateTimeHex.day;
        fromTime.day_of_week = CalWeekday(year,dateTimeHex.month,dateTimeHex.day);;
        fromTime.hour = (dateTimeHex.hour);
        fromTime.min = (dateTimeHex.min);
        fromTime.sec = 0;//(ptm.tm_sec);
        fromTime.hundredths = 0;
        fromTime.deviation_h = 0x80;
        fromTime.deviation_l = 0;
        fromTime.clockstatus = 0x0f;
        memcpy(&toTime,&fromTime,sizeof(fromTime));

        now_t=now_t+240;  //跨度为最近整15分钟开始的4分钟
        DT_Time2DateTimeHex(now_t,(DateTimeHex_t*)(&dateTimeHex));//方法2中的结束时间
        //localtime_r(&now_t, &ptm);
        toTime.min = (dateTimeHex.min);
#if 0
        toTime.year_h = ((ptm.tm_year+1900)&0xFF00)>>8;
        toTime.year_l = (uint8)(ptm.tm_year+1900);
        toTime.month = (ptm.tm_mon+1);
        toTime.day_of_month = (ptm.tm_mday);
        toTime.day_of_week = (ptm.tm_wday);
        toTime.hour = (ptm.tm_hour);
        toTime.min = (ptm.tm_min);
        toTime.sec = 0;//(ptm.tm_sec);
        toTime.hundredths = 0;
        toTime.deviation_h = 0x80;
        toTime.deviation_l = 0;
        toTime.clockstatus = 0x0f;
#endif

        break;
        
    default:
        return 0;
    }   

    axdr[pos++] = 0x09;
    axdr[pos++] = 0x0C;
    memcpy(&axdr[pos],&fromTime,sizeof(fromTime));
    pos+=sizeof(fromTime);
    
    axdr[pos++] = 0x09;
    axdr[pos++] = 0x0C;
    memcpy(&axdr[pos],&toTime,sizeof(toTime));
    pos+=sizeof(toTime);

    axdr[pos++] = 0x01;//selected_entries: type = array 
    axdr[pos++] = 0x00;//00:所有 capture_object 的数据都返回 

/////////////////

    len_llc_frame = make_llc_frame(axdr,pos);

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址

    uint8 sendseq = ((pdlms_comm->sendseq) >>1);
    sendseq++;
    ctrl_code = ((pdlms_comm->recvseq| (sendseq <<1))|0x10);
    
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    return make_mac_frame(pBuf, hdlcaddr, ctrl_code, axdr, len_llc_frame);
}

int Read_CAPTURE_OBJECT_MakeFrame(S_COMM_INFO_MET *pdlms_comm, uint8 *pBuf, uint16 metaddr)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   
    //uint8 ShortNameLen = 0;
    //int returnLen;
    uint16 base_name;

    if (!pdlms_comm)
        return 0;
    
    axdr[pos++] = DLMS_SN_CMDID_READ;  //[195] Get_request
    axdr[pos++] = 0x01;  //[1] IMPLICIT Get-Request-Normal
    axdr[pos++] = 0x02;

    if (pdlms_comm->frzType>=4)
        return 0;
    
    base_name=get_DLMS_Association_OBJ(pdlms_comm,g_frzObjObis[pdlms_comm->frzType].obis);
    if ((base_name ==0xFFFF)||(base_name ==0x0000))
        return 0;


    base_name+=16; //3. capture_objects      (static)     class_id = 7 属性3
    axdr[pos++] = (uint8)((base_name>>8)&0x00ff); //0x42;//42 98 
    axdr[pos++] = (uint8)(base_name&0x00ff);  //0x98+24;

//  axdr[pos++] = 0x42;//42 98 
//  axdr[pos++] = 0x98+16;

    len_llc_frame = make_llc_frame(axdr,pos);

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址

    uint8 sendseq = ((pdlms_comm->sendseq) >>1);
    sendseq++;
    ctrl_code = ((pdlms_comm->recvseq| (sendseq <<1))|0x10);
    
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    return make_mac_frame(pBuf, hdlcaddr, ctrl_code, axdr, len_llc_frame);
}


//通过长名获取对象的基地址
uint16 get_DLMS_Association_OBJ(S_COMM_INFO_MET *pdlms_comm,const uint8 *in_obis)
{
    uint16 k_mark=0;

    while (SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value != 0xFFFFFFFF)
    {
        if (memcmp(SD_MET_PRTL_DLMS_PRTL[k_mark].oBis,in_obis,6)==0)
        {
            return SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName;
        }
        k_mark++;
    }
    return 0xFFFF;
}

/*-
***************************************************************************
*模块编号:
*名称:读捕获周期
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
int Read_PERIOD_MakeFrame(S_COMM_INFO_MET *pdlms_comm, uint8 *pBuf, uint16 metaddr)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   
    //uint8 ShortNameLen = 0;
    //int returnLen;
    uint16 base_name;

    if (!pdlms_comm)
        return 0;
    
    axdr[pos++] = DLMS_SN_CMDID_READ;  //[195] Get_request
    axdr[pos++] = 0x01;  //[1] IMPLICIT Get-Request-Normal
    axdr[pos++] = 0x02;

    if (pdlms_comm->frzType>=4)
        return 0;
    
    base_name=get_DLMS_Association_OBJ(pdlms_comm,g_frzObjObis[pdlms_comm->frzType].obis);
    if ((base_name ==0xFFFF)||(base_name ==0x0000))
    {
        printf("\n==============frzType=%d 获取短名失败！！！ obis=[hex]%02x-%02x-%02x-%02x-%02x-%02x=============\n\n",
            pdlms_comm->frzType,
            g_frzObjObis[pdlms_comm->frzType].obis[0],
            g_frzObjObis[pdlms_comm->frzType].obis[1],
            g_frzObjObis[pdlms_comm->frzType].obis[2],
            g_frzObjObis[pdlms_comm->frzType].obis[3],
            g_frzObjObis[pdlms_comm->frzType].obis[4],
            g_frzObjObis[pdlms_comm->frzType].obis[5]);
        return 0;
    }
    
    base_name+=24; //4. capture_period       (static)   class_id = 7 属性4
    axdr[pos++] = (uint8)((base_name>>8)&0x00ff); //0x42;//42 98 
    axdr[pos++] = (uint8)(base_name&0x00ff);  //0x98+24;

    len_llc_frame = make_llc_frame(axdr,pos);

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址

    uint8 sendseq = ((pdlms_comm->sendseq) >>1);
    sendseq++;
    ctrl_code = ((pdlms_comm->recvseq| (sendseq <<1))|0x10);
    
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    return make_mac_frame(pBuf, hdlcaddr, ctrl_code, axdr, len_llc_frame);
}

/*-
***************************************************************************
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧 
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
*输出参数：组帧后的缓冲区、帧长
           电表类型 bStdType   0-普通国标 1-重庆简易表

*处理：根据电表规约组帧
*返回：无
***************************************************************************
-*/
static void DLMS_MakeFrame(uint8 *pMetAddr, uint16 ShortName, uint8 Class, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    uint8 axdr[1000];
    uint8 hdlcaddr[10];   

    int returnLen;
    uint16 phyaddr;
    axdr[pos++] = cmdDI;  //[05] Read_request

    if(Class == CLASS_01)
    {
        axdr[pos++] = 0x02;  //[1] IMPLICIT Get-Request-Normal
        axdr[pos++] = 0x02;
        axdr[pos++] = (uint8)((ShortName + 0x0008)>>8);
        axdr[pos++] = (uint8)((ShortName + 0x0008)&0xff);
        axdr[pos++] = 0x02;
        axdr[pos++] = (uint8)(ShortName>>8);
        axdr[pos++] = (uint8)(ShortName&0xff);  
    }
    else if(Class == CLASS_02)
    {
        axdr[pos++] = 0x01;
        axdr[pos++] = 0x02;
        axdr[pos++] = (uint8)(ShortName>>8);
        axdr[pos++] = (uint8)(ShortName&0xff);  
    }
    else if(Class == CLASS_04)
    {
        axdr[pos++] = 0x01;
        axdr[pos++] = 0x02;
        axdr[pos++] = (uint8)(ShortName>>8);
        axdr[pos++] = (uint8)(ShortName&0xff);  
    }
    
    //组织LLC数据祯
    len_llc_frame = make_llc_frame(axdr,pos);

    phyaddr = ((pMetAddr[0])>>4)*10+(pMetAddr[0]&0x0f)+\
    (pMetAddr[1]&0x0f)*100 + ((pMetAddr[1])>>4)*1000;

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址
    uint8 sendseq = ((SEND_SEQ) >>1);
    sendseq++;
    ctrl_code = ((RECV_SEQ| (sendseq <<1))|0x10);
    HdlcAddrTran(hdlcaddr, 0x01, phyaddr);
    returnLen = make_mac_frame(&pBuf[0], hdlcaddr, ctrl_code, axdr, len_llc_frame)+0;
    *pSndLen = (uint8)returnLen;
    return;
}



/*-
***************************************************************************
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧 
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
*输出参数：组帧后的缓冲区、帧长
           电表类型 bStdType   0-普通国标 1-重庆简易表

*处理：根据电表规约组帧
*返回：无
***************************************************************************
-*/
void Dlms_Make_Record_Frame(uint8 *pMetAddr, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen,S_COMM_INFO_MET *pCommInfo,SCHEME_TASK_RUN_INFO *pCbTask)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    uint8 axdr[1000];
    uint8 hdlcaddr[10]; 
    //cbLPPara_t *pCbLPPara;
    //time_t tmpTime;
    //OOP_TI_T    ti;

    //DateTimeHex_t dateTimeHex;
    
    int returnLen;
    uint16 phyaddr;
    
//    ti.unit = pCbTask->AcqInfo.TI.unit;
//    ti.value = pCbTask->AcqInfo.TI.value;

    axdr[pos++] = cmdDI;  //[05] Read_request

//    pCbLPPara = &pCommInfo->cbLPPara;
//
    {
//      axdr[pos++] = 0x01;  //[1] IMPLICIT Get-Request-Normal
//      axdr[pos++] = 0x04; //条件访问
//      axdr[pos++] = 0x62;
//      axdr[pos++] = 0x78;
//      axdr[pos++] = 0x01;
//      axdr[pos++] = 0x02;
//      axdr[pos++] = 0x04; 
//        axdr[pos++] = 0x02;
//      axdr[pos++] = 0x04; 
//        axdr[pos++] = 0x12;
//      axdr[pos++] = 0x00; 
//        axdr[pos++] = 0x08;
//        axdr[pos++] = 0x09;   
//        axdr[pos++] = 0x06;
//        axdr[pos++] = 0x00;   
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x01;   
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x00;   
//        axdr[pos++] = 0xFF;
//        axdr[pos++] = 0x0F;
//        axdr[pos++] = 0x02;
//        axdr[pos++] = 0x12;
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x09;
//        axdr[pos++] = 0x0C;
//        DT_Time2DateTimeHex(pCbLPPara->startTime,(DateTimeHex_t*)(&dateTimeHex));
//        axdr[pos++] = dateTimeHex.year_h;
//        axdr[pos++] = dateTimeHex.year_l;
//        axdr[pos++] = dateTimeHex.month;
//        axdr[pos++] = dateTimeHex.day;
//        pos++;
//        axdr[pos++] = dateTimeHex.hour;
//        axdr[pos++] = dateTimeHex.min;
//        axdr[pos++] = dateTimeHex.sec;
//        pos = pos+4;
//        if(ti.value==0)
//        {
//            tmpTime = pCbLPPara->endTime;
//        }
//        else
//        {
//            tmpTime = pCbLPPara->startTime ;
//            tmpTime = tmpTime+1;
//        }
//        axdr[pos++] = 0x09;
//        axdr[pos++] = 0x0C;
//        DT_Time2DateTimeHex(tmpTime,(DateTimeHex_t*)(&dateTimeHex));//方法2中的结束时间
//        axdr[pos++] = dateTimeHex.year_h;
//        axdr[pos++] = dateTimeHex.year_l;
//        axdr[pos++] = dateTimeHex.month;
//        axdr[pos++] = dateTimeHex.day;
//        pos++;
//        axdr[pos++] = dateTimeHex.hour;
//        axdr[pos++] = dateTimeHex.min;
//        axdr[pos++] = dateTimeHex.sec;
//        pos = pos+4;
//        axdr[pos++] = 0x01;
//        axdr[pos++] = 0x00;
//        
//        axdr[pos++] = 0x02;
//        axdr[pos++] = 0x04;
//        
//        axdr[pos++] = 0x12;
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x03;

//        axdr[pos++] = 0x10;
//        axdr[pos++] = 0x24;
//        axdr[pos++] = 0x10;

//        axdr[pos++] = 0x09;
//        axdr[pos++] = 0x06;
//        axdr[pos++] = 0x01;
//        axdr[pos++] = 0x01;
//        axdr[pos++] = 0x02;
//        axdr[pos++] = 0x08;
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0xFF;
        
//        axdr[pos++] = 0x0F;
//        axdr[pos++] = 0x02;
//        
//        axdr[pos++] = 0x12;
//        axdr[pos++] = 0x00;
//        axdr[pos++] = 0x00;
        
        axdr[pos++] = 0x01;  //[1] IMPLICIT Get-Request-Normal
        axdr[pos++] = 0x02; //条件访问
        axdr[pos++] = 0x62;
        axdr[pos++] = 0x80;//读

    }
    //组织LLC数据祯
    len_llc_frame = make_llc_frame(axdr,pos);

    phyaddr = ((pMetAddr[0])>>4)*10+(pMetAddr[0]&0x0f)+\
    (pMetAddr[1]&0x0f)*100 + ((pMetAddr[1])>>4)*1000;

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址
    uint8 sendseq = ((SEND_SEQ) >>1);
    sendseq++;
    ctrl_code = ((RECV_SEQ| (sendseq <<1))|0x10);
    HdlcAddrTran(hdlcaddr, 0x01, phyaddr);
    returnLen = make_mac_frame(&pBuf[0], hdlcaddr, ctrl_code, axdr, len_llc_frame)+0;
    *pSndLen = (uint8)returnLen;
    return;
}

/*-
***************************************************************************
*模块编号:
*名称:读短名和OBIS帧
*功能:读短名和OBIS帧进行组帧
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
static int Read_SN_MakeFrame(uint8 *pBuf, uint16 metaddr)
{
    int pos=0;
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   
    //uint8 ShortNameLen = 0;
    //int returnLen;
    axdr[pos++] = DLMS_SN_CMDID_READ;  //[195] Get_request
    axdr[pos++] = 0x01;  //[1] IMPLICIT Get-Request-Normal

    axdr[pos++] = 0x02;
    axdr[pos++] = 0xfa;

    axdr[pos++] = 0x08;

    len_llc_frame = make_llc_frame(axdr,pos);

    //组织MAC层数据祯
    //     首先，获取集中器的HDLC扩展地址

    uint8 sendseq = ((SEND_SEQ) >>1);
    sendseq++;
    ctrl_code = ((RECV_SEQ| (sendseq <<1))|0x10);
    
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    return make_mac_frame(pBuf, hdlcaddr, ctrl_code, axdr, len_llc_frame);
}



/*-
***************************************************************************
*模块编号:
*名称:读后续帧
*功能:读后续帧进行组帧
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
static int RR_MakeFrame(uint8 *pBuf, uint16 metaddr,S_COMM_INFO_MET *pCommInfo)
{
    int  len_llc_frame;
    uint8 ctrl_code;
    unsigned char axdr[1000];
    uint8 hdlcaddr[10];   

    len_llc_frame = 0;
    uint8 recvseq = (RECV_SEQ>>5);
    
    recvseq++;
    RECV_SEQ = ((recvseq)<<5);
    pCommInfo->recvseq = RECV_SEQ;
    ctrl_code = (RECV_SEQ|0x11);
    HdlcAddrTran(hdlcaddr, 0x01, metaddr);
    
    return make_mac_frame(pBuf,hdlcaddr,ctrl_code,axdr,len_llc_frame);
}

/*-
***************************************************************************
*模块编号:7-9-5-4
*名称:DLMS规约握手报文组织
*功能:DLMS规约握手报文组织
*输入参数:通道号（pipe）、抄表任务结构体（*pTaskAllot）、组帧后存放的缓冲区的地址（*pBuf）、帧长存放地址（*pSndLen）
*输出参数:组帧后的缓冲区、帧长度
*处理:如果pTaskAllot.HDSK_FraNum< pTaskAllot.HDSK_FraTotal:
*   根据pTaskAllot.（*pClassCont）的值发送不同的报文:
*   DLMS_SNRM_STAT:02(STX同步头)、0x18、0x06(FUNC)、0x00（PAD）、0x01(长度)、相应表序号的表地址、CRCH、CRCL。
*   报文长度＝ 8。
*   DLMS_AARQ_STAT:02(STX同步头)、0x18、0x01、0x00、0x04、密码（4字节）、CRCH、CRCL。
*   报文长度＝11；其中四个字节的密码根据ABB规约提供的函数计算得到Password(m_Key,commkey)。
*   CRC校验是从同步头开始至校验码前一个字节的CRC校验值。
*   其它:返回。
*   将上面组织好的报文拷贝入相应缓冲区中，即**pBuf；发送报文长度拷贝入到*SndLen。
*返回:是否正常组帧
***************************************************************************
-*/
uint8 DLMS_HandShake_Cmd(uint8 pipe, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms)
{
    uint8   commkey[15];
    uint16  phyaddr = 0;
    uint8   hdskFraNum;
    uint8  sendLen = 0;

    Comm_Info_dlms->IsKnownMonFrm = 0;/*-开始抄表时初始化为０，表示非确定上月月份帧-*/
    Comm_Info_dlms->Class12Frm = 0;
    Comm_Info_dlms->LastMonNonData = 0;

    memset(commkey, 0x00, sizeof(commkey));

    hdskFraNum = Comm_Info_dlms->HDSKFraNum;

    phyaddr = ((MtrPara->MetAddr[0])>>4)*10+(MtrPara->MetAddr[0]&0x0f)+\
        (MtrPara->MetAddr[1]&0x0f)*100 + ((MtrPara->MetAddr[1])>>4)*1000;
    
    TASK_FMT_TRACE( pipe, REC_LOG, "phyaddr is %d\n",phyaddr);
    
    TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->HDSKFraNum %d,Comm_Info_dlms->HDSKFraTotal %d\n",Comm_Info_dlms->HDSKFraNum,Comm_Info_dlms->HDSKFraTotal);
    if (Comm_Info_dlms->HDSKFraNum < Comm_Info_dlms->HDSKFraTotal)
    {
        switch(Comm_Info_dlms->pHDSKCont[hdskFraNum])
        {
            case DLMS_DISC_STAT:
                sendLen = DISC_MakeFrame(&Comm_Info_dlms->pSndBuf[0], phyaddr);
                break;
        
            case DLMS_SNRM_STAT:
            case DLMS_SNRM_AGAIN_STAT:
                sendLen = SNRM_MakeFrame(&Comm_Info_dlms->pSndBuf[0], phyaddr);
                break;
                
            case DLMS_AARQ_STAT:
            case DLMS_AARQ_AGAIN_STAT:
                RECV_SEQ = 0;
                SEND_SEQ = 0;       
               Comm_Info_dlms->recvseq = RECV_SEQ;
               Comm_Info_dlms->sendseq = SEND_SEQ;
                sendLen = AARQ_MakeFrame(&Comm_Info_dlms->pSndBuf[0], phyaddr);
                break;
                
            case DLMS_READ_SN_STAT:
                sendLen = Read_SN_MakeFrame(&Comm_Info_dlms->pSndBuf[0], phyaddr);
                break;
                
            case DLMS_RR_FIRST_STAT:
                //RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
                sendLen = RR_MakeFrame(&Comm_Info_dlms->pSndBuf[0], phyaddr,Comm_Info_dlms);
                break;      
                
            //case DLMS_RR_SECOND_STAT:
            //  *pSndLen = RR_MakeFrame(&Comm_Info_dlms->pSndBuf[5], phyaddr)+5;
            //  break;
            default:
                  break;;
        }
    }
    
    return sendLen;
}


void STDDlms_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T   *RunTaskInfo)
{

    uint16 CsdIndex;            
    uint16 taskid; 


    taskid=pCbTask->TaskId;
    CsdIndex=RunTaskInfo->CsdIndex;

    if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
    ||(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003))//分钟曲线或小时曲线
    {
        OOP_TI_T Ti;
        OOP_TI_T TaskTi;
        OOP_TI_T delayTi;
        DateTimeBcd_t tmpTime;
        get_task_run_frequence(taskid,&TaskTi);
        //采集间隔和任务间隔一样,且有任务延时的采集本周期的数据,否则采集上一周期
        if((TaskTi.unit == pCbTask->AcqInfo.TI.unit)&&(TaskTi.value = pCbTask->AcqInfo.TI.value)&&(pCbTask->DelayTime !=0))
        {
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
            delayTi.value = pCbTask->DelayTime;
            delayTi.unit = pCbTask->DelayTimeUnit;
            DT_TimeSubInv(&pCbLpPara->startTime, delayTi);
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnEndTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);

        }
        else
        {
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->LastTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->startTime);
            memcpy_r((uint8*)&tmpTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);

            DT_DateTimeBCD2Time(&tmpTime,&pCbLpPara->endTime);
        }

        pCbLpPara->tmInv.unit = pCbTask->AcqInfo.TI.unit;
        pCbLpPara->tmInv.value= pCbTask->AcqInfo.TI.value;


        if(pCbLpPara->tmInv.value==0)//配置的采集方式数据冻结时标
        {
            memset(&Ti,0,sizeof(Ti));
            if(0x5002 == pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID)
            {
                //时标间隔为任务频率
                get_task_run_frequence(taskid,&Ti);
                if((Ti.unit >= TASK_CYCLE_FLAG_HOUR)
                ||((Ti.unit == TASK_CYCLE_FLAG_MIN)&&(Ti.value > 15)))
                {
                    //任务频率大于15分钟，时标间隔设为15分钟
                    Ti.unit = TASK_CYCLE_FLAG_MIN;
                    Ti.value = 15;
                }
            }
            else
            {
                Ti.unit = TASK_CYCLE_FLAG_HOUR;//小时
                Ti.value = 1;//默认1小时
            }
            pCbLpPara->tmInv.unit = Ti.unit;
            pCbLpPara->tmInv.value = Ti.value;
        }
        pCbLpPara->tmInv.value *= 4; 
    }
    else
    {
        pCbLpPara->startTime = time(NULL);
        pCbLpPara->tmInv.value = 15;
    }

}




//分析抄表数据类型，是否为从表中取冻结数据
void AnalysisIfFrzDataFromMeter(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *pdlms_comm)
{
    uint8 CsdIndex;

    CSD_TASK_INFO  *csd;
    CsdIndex=RunTaskInfo->CsdIndex;
    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];
    
    switch (csd->MainOad.nObjID)
    {
    case 0x5004://日冻结
        pdlms_comm->frzType=eCAPTURE_DAY;
        pdlms_comm->bFrzValid=TRUE; 
        break;
    case 0x5006://月冻结
        pdlms_comm->frzType=eCAPTURE_MON;
        pdlms_comm->bFrzValid=TRUE; 
        break;
    case 0x5003:
    case 0x5002:
        pdlms_comm->frzType=eCAPTURE_CURVE_1;
        pdlms_comm->bFrzValid=TRUE;  
        break;
    default:
        break;//认为不需要从表中取冻结数据
    }
    return;
    

        
}



/**
*********************************************************************
* @brief:       抄表前的准备工作，为后面抄表组帧获取抄读开始时间和结束时间，初始化；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[out]：  SCHEME_TASK_RUN_INFO *pCbTask,  抄读是的任务信息；
* @param[out]： CTS_RUN_TASK_T   *RunTaskInfo,  方案抄读过程中运行信息；
* @param[out]： S_COMM_INFO_MET *Comm_Info_Oop,  方案抄读过程中运行信息；
* @param[out]： cbMtrPara *MtrPara,  抄读的电表信息；
* @return：
*********************************************************************
*/

void Prepare_dlms_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
{


    uint16 sn;
    uint16 MeterIndex;
    OOP_METER_BASIC_T MeterBasicInfo;
    //task_id=pCbTask->TaskId;
    MeterIndex=RunTaskInfo->MeterIndex;
    sn=pCbTask->MsInfo[MeterIndex].Sn;
    //读取测量点信息
    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));

    //读取测量点信息
    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
    MtrPara->MtrNum=sn;
    //MtrPara->Speed=get_meter_read_speed(MeterBasicInfo.baud);
    MtrPara->Speed=MeterBasicInfo.baud;
//    if(MtrPara->Speed==0)
//    {
//        MtrPara->Speed = 9600;
//    }
    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
 
    Comm_Info_Oop->pMtrPara = MtrPara;
    Comm_Info_Oop->MetStat = CB_STATE_INIT;
    //读取通信端口信息COM
    TASK_FMT_TRACE( pipe, REC_LOG, "%d-%d-%d-%d-%d-%d\n",MtrPara->MetAddr[0],MtrPara->MetAddr[1],MtrPara->MetAddr[2],MtrPara->MetAddr[3],MtrPara->MetAddr[4],MtrPara->MetAddr[5]);
  
    TASK_FMT_TRACE( pipe, REC_LOG, "Prepare_dlms_meter_read\n");
    STDDlms_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);
    TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_Oop->MetStat is %d\n",Comm_Info_Oop->MetStat);
    DLMS_HDSK_Cmd_Init(Comm_Info_Oop);
    AnalysisIfFrzDataFromMeter(pCbTask,pipe,RunTaskInfo,Comm_Info_Oop);
    DLMS_Frz_Cmd_Init(Comm_Info_Oop,pCbTask, pipe, RunTaskInfo);
	pCbTask->StoreTimeType = STRTMTYP_STARTTM;
    pCbTask->AcqInfo.Acqtype = ACQTYP_NULL;
}

/*-
***************************************************************************
*模块编号:7-9-5-1
*名称:初始化握手报文类型
*功能:初始化握手报文类型
*输入参数:抄表任务结构体（*pTaskAllot）
*输出参数:初始化后的任务结构体
*处理:
*返回:无
***************************************************************************
-*/
void  DLMS_HDSK_Cmd_Init(S_COMM_INFO_MET *Comm_Info_dlms)
{
    if (IsSendSn[2]==TRUE)
    {
        Comm_Info_dlms->HDSKFraTotal    = 3;
        Comm_Info_dlms->HDSKFraNum  = 0;
        Comm_Info_dlms->pHDSKCont[0]    = DLMS_DISC_STAT;
        Comm_Info_dlms->pHDSKCont[1]    = DLMS_SNRM_STAT;
        Comm_Info_dlms->pHDSKCont[2]    = DLMS_AARQ_STAT;
        Comm_Info_dlms->IsLastSeq = 0;
    }
    else
    {
        Comm_Info_dlms->HDSKFraTotal    = 8;
        Comm_Info_dlms->HDSKFraNum  = 0;
        Comm_Info_dlms->pHDSKCont[0]    = DLMS_DISC_STAT;
        Comm_Info_dlms->pHDSKCont[1]    = DLMS_SNRM_STAT;
        Comm_Info_dlms->pHDSKCont[2]    = DLMS_AARQ_STAT;
        Comm_Info_dlms->pHDSKCont[3]    = DLMS_READ_SN_STAT;
        Comm_Info_dlms->pHDSKCont[4]    = DLMS_RR_FIRST_STAT;
        Comm_Info_dlms->pHDSKCont[5]    = DLMS_DISC_STAT;
        Comm_Info_dlms->pHDSKCont[6]    = DLMS_SNRM_AGAIN_STAT;
        Comm_Info_dlms->pHDSKCont[7]    = DLMS_AARQ_AGAIN_STAT;

        Comm_Info_dlms->IsLastSeq = 0;
    }
    return;
}

/**
*********************************************************************
* @brief:       打开485抄读通道；
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]： S_COMM_INFO_MET *Comm_Info_Oop,  方案抄读过程中运行信息；
* @param[in]： cbMtrPara *MtrPara,  抄读的电表信息；
* @return：
*********************************************************************
*/

//void Cb_State_Init_Dlms(S_COMM_INFO_MET *Comm_Info_dlms,uint8 pipe,cbMtrPara  *MtrPara)
//{
//    TASK_FMT_TRACE( pipe, COMM_LOG, "Cb_State_Init_Dlms\n");
//    if(TRUE==get_rs485_dev(pipe))
//      {
//        
//        if(rs485_Dlms_Init(&Rs485ComNo[pipe-2],MtrPara->Speed)!=0)
//        {
//            rs485_Dlms_Init(&Rs485ComNo[pipe-2],MtrPara->Speed);//先临时添加
//            TASK_FMT_TRACE( pipe, COMM_LOG, "初始化失败,pipe is %d\n",pipe);
//            Comm_Info_dlms->MetStat = CB_STATE_FINISH;
//            return;
//        }
//        
//      }
//      else
//      {
//        
//        TASK_FMT_TRACE( pipe, COMM_LOG, "没有获取到模组,pipe is %d\n",pipe);
//        Comm_Info_dlms->MetStat = CB_STATE_FINISH; 
//        return;
//      }
//      Comm_Info_dlms->MetStat = CB_STATE_HANDSEND;
//      TASK_FMT_TRACE( pipe, COMM_LOG, "Comm_Info_dlms->MetStat is %d\n",Comm_Info_dlms->MetStat);
//      return;
//}
//



/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Hnadsend(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    int ret;
    TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->HDSKFraNum %d,Comm_Info_dlms->HDSKFraTotal %d\n",Comm_Info_dlms->HDSKFraNum,Comm_Info_dlms->HDSKFraTotal);
    if (Comm_Info_dlms->HDSKFraNum >= Comm_Info_dlms->HDSKFraTotal)  /*-握手结束-*/
    {
        Comm_Info_dlms->MetStat = CB_STATE_SEND;        
        Comm_Info_dlms->FraTimes=0;
        Comm_Info_dlms->ErrFrame=0;
    }
    else
    {

        if (Comm_Info_dlms->ErrFrame >= 1)/*-一帧握手不成功，即失败-*/
        {               
            Comm_Info_dlms->MetStat = CB_STATE_FINISH;
            Comm_Info_dlms->AutkStat = CB_RET_FAIL;                                       
        }
        else
        {                           
                    Comm_Info_dlms->SndLen = DLMS_HandShake_Cmd(pipe, MtrPara, Comm_Info_dlms);  /*-调用自动任务组织握手发送报文函数-*/                                

                    
                    //ret = rs485_RS232_send(&Rs485ComNo[pipe-2],Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);
                    ret = rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_NO, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);

                    TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);

                    if(ret ==0)  
                    {                                       
                        Comm_Info_dlms->MetStat = CB_STATE_HANDRECV;                                                          
                    }
                    else                
                    {
                        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
                        Comm_Info_dlms->ErrTimes++;
                    }                               
         }


     }
}

//void Getindex(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
//{
//    CSD_TASK_INFO  *csd;
//    uint8 CsdIndex;
//    uint8 k=0,j=0,m;
//    uint8 itemnum;
//    DateTimeHex_t datetimehex,StoreTime;
//    uint16 datalen=0;
//    uint16 offset;
//    uint8 frzType=eCAPTURE_CURVE_1;
//    uint8 tmpbuf[500];
//    
//    CsdIndex=RunTaskInfo->CsdIndex;
//    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];
//
//    {
//        datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
//        datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
//        datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
//        datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
//        datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
//        datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
//        datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
//
//    }
//    for(k=0;k<csd->SubOadNum;k++)
//    {
//        
//        TASK_FMT_TRACE( pipe, REC_LOG, "csd->OadListInfo[%d].value is %x\n",k,csd->OadListInfo[k].value);
//        for(j=0;j<itemnum;j++)
//        {
//
//            if(csd->OadListInfo[k].nObjID == Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[j].nObjID)
//            {
//                break;
//            }
//        }
//
//
//        if(j<itemnum)
//        {
//            for(m=0;m<j;m++)
//            {
//
//                if(csd->OadListInfo[k].nObjID ==0x2021)
//                {
//                                    //dlms电表的第一个冻结数据是冻结时间
//                    datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
//                    datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
//                    datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
//                    datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
//                    datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
//                    datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
//                    datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
//                }
//                datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
//                offset=offset+datalen;
//            }
//            
//            datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
//            TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d\n",offset,m);
//
//            DT_DateTimeHex2BCD((DateTimeHex_t*)(&datetimehex),&StoreTime);
//            
//            TASK_BUF_TRACE( pipe, REC_LOG, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen);
//            
//            DLMS_AUTK_Recv(pipe, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen,pCbTask,RunTaskInfo,MtrPara,&StoreTime,csd->OadListInfo[k],0);
//        }
//    }  
//
//
//}
void SaveFrzData(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    uint8 recordnum=0;
    uint8 CsdIndex;
    uint8 frzType=eCAPTURE_CURVE_1;
    uint16 offset=0;
    uint16 i=0;
    uint16 j=0;
    uint16 k=0;
    uint16 m=0;
    uint8 n=0;
    uint8 itemnum=0;
    CSD_TASK_INFO  *csd;
    //uint8 tmpbuf[50];
    uint16 datalen=0;
    static DateTimeBcd_t StoreTime;
    DateTimeHex_t datetimehex;
    uint16 tmpoffset=0;
    uint8 num=1;
    BOOL needcurve2 =FALSE;
    
    

    CsdIndex=RunTaskInfo->CsdIndex;
    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];
    
    if(csd->MainOad.nObjID==0x5002)
    {
        if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++] !=0x01)
        {
            return;
        }
        recordnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
        for(i=0;i<recordnum;i++)
        {
            if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++]==DT_STRUCTURE)
            {
               itemnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
               datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
                for(k=0;k<csd->SubOadNum;k++)
                {
                    
                    TASK_FMT_TRACE( pipe, REC_LOG, "csd->OadListInfo[%d].value is %x\n",k,csd->OadListInfo[k].value);
                    for(j=0;j<itemnum;j++)
                    {

                        if(csd->OadListInfo[k].nObjID == Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[j].nObjID)
                        {
                            num =1;
                            if(csd->OadListInfo[k].nIndex == 0x00)
                            {
                                for(n=j+1;n<j+5;n++)
                                {
                                    if(csd->OadListInfo[k].nObjID != Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[n].nObjID)
                                    {
                                        break;
                                    }
                                    num++;
                                    TASK_FMT_TRACE( pipe, REC_LOG, "num is %d\n",num);
                                }
                            }
                            break;
                        }
                    }
                    

                    tmpoffset = 4;
                    if(j<itemnum)
                    {
                        offset =tmpoffset;
                        for(m=0;m<j;m++)
                        {
                            datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                            offset=offset+datalen;
                            TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d,datalen is %d\n",offset,m,datalen);
                        }
                        if(j==0)
                        {
                            datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                            datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                            datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                            datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                            datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                            datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                            datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
      
                        }
                        
                        datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                        TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d\n",offset,m);

                        DT_DateTimeHex2BCD((DateTimeHex_t*)(&datetimehex),&StoreTime);
                        if(num >1 )
                        {
                            datalen =datalen*num;
                        }
                        TASK_BUF_TRACE( pipe, REC_LOG, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen);
                        
                        DLMS_AUTK_Recv(pipe, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen,pCbTask,RunTaskInfo,MtrPara,&StoreTime,csd->OadListInfo[k],num);
                    }
                    else
                    {
                        needcurve2 =TRUE;
                    }
                }  
            }
        }
        if(needcurve2 ==TRUE)
        {
            frzType=eCAPTURE_CURVE_2;
            offset =0;
            if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++] !=0x01)
            {
                return;
            }
            recordnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
            for(i=0;i<recordnum;i++)
            {
                if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++]==DT_STRUCTURE)
                {
                   itemnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
                   datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                    datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                    datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                    datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                    datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                    datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                    datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
                    for(k=0;k<csd->SubOadNum;k++)
                    {
                        
                        TASK_FMT_TRACE( pipe, REC_LOG, "csd->OadListInfo[%d].value is %x\n",k,csd->OadListInfo[k].value);
                        if(csd->OadListInfo[k].value == 0x20210200)
                        {
                            continue;
                        }
                        for(j=0;j<itemnum;j++)
                        {

                            if(csd->OadListInfo[k].nObjID == Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[j].nObjID)
                            {
                                num =1;
                                for(n=j+1;n<j+5;n++)
                                {
                                    if(csd->OadListInfo[k].nObjID != Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[n].nObjID)
                                    {
                                        break;
                                    }
                                    num++;
                                    TASK_FMT_TRACE( pipe, REC_LOG, "num is %d\n",num);
                                }
                                break;
                            }
                        }
                        

                        tmpoffset = 4;
                        if(j<itemnum)
                        {
                            offset =tmpoffset;
                            for(m=0;m<j;m++)
                            {
                                datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                                offset=offset+datalen;
                                TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d,datalen is %d\n",offset,m,datalen);
                            }
                            if(j==0)
                            {
                                datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                                datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                                datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                                datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                                datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                                datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                                datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
          
                            }
                            
                            datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                            TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d\n",offset,m);

                            DT_DateTimeHex2BCD((DateTimeHex_t*)(&datetimehex),&StoreTime);
                            if(num >1 )
                            {
                                datalen =datalen*num;
                            }
                            TASK_BUF_TRACE( pipe, REC_LOG, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen);
                            
                            DLMS_AUTK_Recv(pipe, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen,pCbTask,RunTaskInfo,MtrPara,&StoreTime,csd->OadListInfo[k],num);
                        }

                    }  
                }
            }
        }
    }
    else if((csd->MainOad.nObjID==0x5004)||(csd->MainOad.nObjID==0x5006))
    {
        if(csd->MainOad.nObjID==0x5004)
        {
            frzType=eCAPTURE_DAY;
        }
        else
        {
            frzType=eCAPTURE_MON;
        }
        if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++] !=0x01)
        {
            return;
        }
        recordnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
        for(i=0;i<recordnum;i++)
        {
            if(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++]==DT_STRUCTURE)
            {
               itemnum = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset++];
               datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
                for(k=0;k<csd->SubOadNum;k++)
                {
                    
                    TASK_FMT_TRACE( pipe, REC_LOG, "csd->OadListInfo[%d].value is %x\n",k,csd->OadListInfo[k].value);
                    for(j=0;j<itemnum;j++)
                    {

                        if(csd->OadListInfo[k].nObjID == Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[j].nObjID)
                        {
                            num =1;
                            for(n=j+1;n<j+5;n++)
                            {
                                if(csd->OadListInfo[k].nObjID != Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].frzoad[n].nObjID)
                                {
                                    break;
                                }
                                num++;
                                TASK_FMT_TRACE( pipe, REC_LOG, "num is %d\n",num);
                            }
                            break;
                        }
                    }
                    

                    tmpoffset = 4;
                    if(j<itemnum)
                    {
                        offset =tmpoffset;
                        for(m=0;m<j;m++)
                        {
                            datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                            offset=offset+datalen;
                            TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d,datalen is %d\n",offset,m,datalen);
                        }
                        if(j==0)
                        {
                            datetimehex.year_h = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+2];
                            datetimehex.year_l = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+3];
                            datetimehex.month = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+4];
                            datetimehex.day = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+5];
                            datetimehex.hour = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+7];
                            datetimehex.min = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+8];
                            datetimehex.sec = Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+9];
      
                        }
                        
                        datalen=getDataLenByType(Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],&Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset+1]);
                        TASK_FMT_TRACE( pipe, REC_LOG, "offset is %d,m is %d\n",offset,m);

                        DT_DateTimeHex2BCD((DateTimeHex_t*)(&datetimehex),&StoreTime);
                        if(num >1 )
                        {
                            datalen =datalen*num;
                        }
                        TASK_BUF_TRACE( pipe, REC_LOG, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen);
                        
                        DLMS_AUTK_Recv(pipe, &Comm_Info_dlms->dlmsFrzCaptureParaAndData[frzType].dataBuf[offset],datalen,pCbTask,RunTaskInfo,MtrPara,&StoreTime,csd->OadListInfo[k],num);
                    }
                }  
            }
        }
    }
}




/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Frzsend(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    int ret=0;
    uint16 pn;
    uint16 MsIndex;
    uint16 MeterIndex;

    
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if (Comm_Info_dlms->bFrzValid)
    {
        switch (Comm_Info_dlms->frzType)
        {
        case eCAPTURE_DAY:
            Comm_Info_dlms->shortname[0]=DLMS_FRZ_DAY_ID;
            Comm_Info_dlms->DlmsNum=1;
            Comm_Info_dlms->DlmsIndex=0;
            break;
            
        case eCAPTURE_MON:
            Comm_Info_dlms->shortname[0]=DLMS_FRZ_MON_ID;
            Comm_Info_dlms->DlmsNum=1;
            Comm_Info_dlms->DlmsIndex=0;
            break;
    
        case eCAPTURE_CURVE_1:
            Comm_Info_dlms->shortname[0]=DLMS_FRZ_CURVE1_ID;
            Comm_Info_dlms->DlmsNum=1;
            Comm_Info_dlms->DlmsIndex=0;
            break;
            
        case eCAPTURE_CURVE_2:
            Comm_Info_dlms->shortname[0]=DLMS_FRZ_CURVE2_ID;
            Comm_Info_dlms->DlmsNum=1;
            Comm_Info_dlms->DlmsIndex=0;
            break;
    
        default:
            Comm_Info_dlms->DlmsNum=0;
            break;
        }
    }
    
    if (Comm_Info_dlms->DlmsNum==0 || Comm_Info_dlms->bFrzValid==FALSE)
    {
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
        Comm_Info_dlms->FraTimes=0;
        Comm_Info_dlms->ErrFrame=0;
        Comm_Info_dlms->ErrTimes=0;
        return;
    }
    
    if(Comm_Info_dlms->ErrFrame >= ERR_FRAME_MAX)
    {
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
        Comm_Info_dlms->AutkStat = CB_RET_FAIL;
        return;
    }
    TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->FrzFrmTypeId %d ,Comm_Info_dlms->FrzFrmTypeTotal\n",Comm_Info_dlms->FrzFrmTypeId,Comm_Info_dlms->FrzFrmTypeTotal);
    if (Comm_Info_dlms->FrzFrmTypeId>=Comm_Info_dlms->FrzFrmTypeTotal) //获取表冻结数据需要的步骤都已经完成
    {
        Comm_Info_dlms->MetStat = CB_STATE_FINISH; //转常规发送
        Comm_Info_dlms->FraTimes=0;
        Comm_Info_dlms->ErrFrame=0;
        Comm_Info_dlms->ErrTimes=0;
        
        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
        SaveFrzData(pCbTask,RunTaskInfo,pipe,Comm_Info_dlms,MtrPara);
        return;
    }
    DLMS_MakeFrzFrame(MtrPara->MetAddr,7,0x05,Comm_Info_dlms->pSndBuf,&Comm_Info_dlms->SndLen,Comm_Info_dlms);
    
    //ret=rs485_RS232_send(&Rs485ComNo[pipe-2],Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);/*-调用发送函数-*/
    ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_NO, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);/*-调用发送函数-*/

    if ((ret == 0)&&(Comm_Info_dlms->SndLen>0)) 
    {           

        TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送\n",MtrPara->MtrNum);
        TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);
        Comm_Info_dlms->MetStat = CB_STATE_FRZRECV;         //发送成功，准备接收
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
        //gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
    }
    else
    {                                       
        //Comm_Info_dlms->ErrTimes++;
        
        TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d\n,发送结果 ret is %d\n",MtrPara->MtrNum,ret);
        
        taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
        taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
       // gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
        return;
    }  
    Comm_Info_dlms->AutkStat =CB_STATE_FRZRECV;
    return;
    
}








/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Frzrcv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    BOOL        actResult = FALSE;  //对回码处理的结果
    uint8       ret = 0;
    uint8       errMaxTime;
    
    Comm_Info_dlms->RcvLen = rs485_RS232_recv(pipe,Comm_Info_dlms->pRcvBuf, sizeof(Comm_Info_dlms->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG, "日冻结抄读接收报文\n");
    TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen);
    if (Comm_Info_dlms->RcvLen == 0)
    {   
        actResult = FALSE;
    }
    else 
    {
    
        ret = DLMS_CheckRspFrame(pipe, *MtrPara, Comm_Info_dlms);  /*-电表返回帧有效-*/
        if (ret) /*-有效-*/
        {
            actResult = TRUE;
        }
        else
        {
            actResult = FALSE;
        }
    }

    if (actResult)  /*-该帧交换正确-*/
    {
        if ((ret == 1)||(ret == 2))  /*-有效,且有数据返回-*/
        {
            //数据存入缓冲区
            uint8  RRFlag;
            int saveRet=DLMS_saveFrzParaAndData(Comm_Info_dlms,Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen, MtrPara->MetAddr,&RRFlag,pCbTask,pipe,RunTaskInfo);
            if (saveRet==4)//失败，表回码没数据
            {
                Comm_Info_dlms->MetStat = CB_STATE_FINISH;//退出表库处理
                Comm_Info_dlms->AutkStat = CB_RET_FAIL;
                return;
            }
            

            if (RRFlag || saveRet==1)//回码为RR帧或回码指示无后续帧，全部接收完了
            {
                Comm_Info_dlms->MetStat = CB_STATE_FRZSEND;     
                Comm_Info_dlms->FraTimes=0;
                return;
            }
        }
        
        if (ret == 2)//有后续帧
        {
            Comm_Info_dlms->FraTimes++;
            Comm_Info_dlms->ErrFrame = 0;
            Comm_Info_dlms->ErrTimes = 0;       
            //帧正确切换到读后续帧,序号减1,在发送的时候再加1
            if (Comm_Info_dlms->FraTimes == 1)
            {
               // dlmsComm.RECV_SEQ = (((dlmsComm.RECV_SEQ>>5)-1)<<5);
            }
        }
        else
        {                        
            Comm_Info_dlms->FraTimes = 0;
            Comm_Info_dlms->ComFrame++;                       
            Comm_Info_dlms->ErrFrame = 0;
            Comm_Info_dlms->ErrTimes = 0;             
        }
    }
    else  /*-该帧交换失败-*/
    {
        Comm_Info_dlms->ErrTimes++;
        errMaxTime = 2;//1;
    
        if (Comm_Info_dlms->ErrTimes >= errMaxTime)
        {
            Comm_Info_dlms->ErrFrame++;
            Comm_Info_dlms->ErrTimes = 0;
        }
        else if (Comm_Info_dlms->RcvLen==0)
            return;
        
        //帧失败读后续帧的时候,序号减1,在重发的时候再加1
        if (Comm_Info_dlms->FraTimes != 0)
        {
            Comm_Info_dlms->sendseq = (((Comm_Info_dlms->recvseq>>5)-1)<<5);
            SEND_SEQ = (((RECV_SEQ>>5)-1)<<5);
        }
    }

    Comm_Info_dlms->MetStat = CB_STATE_FRZSEND;
    if(ret == 6)
    {
        if(Comm_Info_dlms->ComFrame==0)
            Comm_Info_dlms->ComFrame=1;//收到对方断连，至少有1次通信
        Comm_Info_dlms->ErrFrame = ERR_FRAME_MAX+Comm_Info_dlms->ComFrame;//失败重抄
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
        Comm_Info_dlms->AutkStat = CB_RET_FAIL;
    }
}



/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Hnadrecv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    int ret=0;
    BOOL actResult;
    uint8 errMaxTime =15;
    uint16 CsdIndex;
    CSD_TASK_INFO  *csd;
    
    CsdIndex=RunTaskInfo->CsdIndex;

    
    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];

    //Comm_Info_dlms->RcvLen = rs485_RS232_recv(&Rs485ComNo[pipe-2],Comm_Info_dlms->pRcvBuf, sizeof(Comm_Info_dlms->pRcvBuf), 100, 20, 0xFF);
    
    Comm_Info_dlms->RcvLen = rs485_RS232_recv(pipe,Comm_Info_dlms->pRcvBuf, sizeof(Comm_Info_dlms->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG, "协商接收报文\n");
    TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen);
    if (Comm_Info_dlms->RcvLen == 0)
    {   
        if (Comm_Info_dlms->pHDSKCont[Comm_Info_dlms->HDSKFraNum] == DLMS_RR_FIRST_STAT)
        {
            RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
            Comm_Info_dlms->recvseq = RECV_SEQ;
            
        }
        actResult = FALSE;
    }
    else 
    {

        ret = DLMS_CheckRspFrame(pipe, *MtrPara, Comm_Info_dlms);  /*-电表返回帧有效-*/
        TASK_FMT_TRACE( pipe, REC_LOG, "解析结果 %d\n",ret);
        if (ret) /*-有效-*/
        {
            actResult = TRUE;
        }
        else
        {
            actResult = FALSE;
        }
    }

    if (actResult)  /*-该帧交换正确-*/
    {
        // 序号不加1,继续读下一帧

        if ((SEND_SEQ != 1)&&(Comm_Info_dlms->HDSKFraNum == DLMS_RR_FIRST_STAT))
        {
    
        }
        else
        {
            if (Comm_Info_dlms->HDSKFraNum == DLMS_RR_FIRST_STAT)
            {
                if (Comm_Info_dlms->ComCount < 3)
                {
                    RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
                    Comm_Info_dlms->recvseq = RECV_SEQ;
                    Comm_Info_dlms->ComCount++;
                }
                else
                {
                    Comm_Info_dlms->HDSKFraNum++;
                    Comm_Info_dlms->ComCount=0;
                    RECV_SEQ = 0;
                    SEND_SEQ = 0;
                    Comm_Info_dlms->recvseq = RECV_SEQ;
                    Comm_Info_dlms->sendseq = SEND_SEQ;
                }
            }
            else
            {
                Comm_Info_dlms->HDSKFraNum++;
            }
        }
        Comm_Info_dlms->ErrFrame = 0;
        Comm_Info_dlms->ErrTimes = 0;
    }
    else  /*-该帧交换失败-*/
    {
        Comm_Info_dlms->ErrTimes++;
    
        if (Comm_Info_dlms->ErrTimes >= errMaxTime)
        {
            Comm_Info_dlms->ErrFrame++;
            Comm_Info_dlms->HDSKFraNum = 0;
            Comm_Info_dlms->ErrTimes = 0;
        }
    }
        Comm_Info_dlms->MetStat = CB_STATE_HANDSEND;//HANDSHAKE_SEND

        TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->HDSKFraNum is  %d\n",Comm_Info_dlms->HDSKFraNum);
        if (Comm_Info_dlms->HDSKFraNum >= Comm_Info_dlms->HDSKFraTotal)  /*-握手结束-*/
        {
            if((csd->MainOad.nObjID==0x0000)||(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL))
            {
                Comm_Info_dlms->MetStat = CB_STATE_SEND;  //MET_SEND      
            }
            else
            {
                Comm_Info_dlms->MetStat = CB_STATE_FRZSEND;  //MET_SEND     
            }
            IsSendSn[pipe] =TRUE;
        }
        
        if(ret == 6)
        {
            Comm_Info_dlms->MetStat = CB_STATE_FINISH;
            Comm_Info_dlms->AutkStat = CB_RET_FAIL;
        }

        TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->MetStat is  %d\n",Comm_Info_dlms->MetStat);
}


/*-
***************************************************************************
*模块编号：7-9-1
*名称：部颁规约自动任务组织发送报文模块 
*功能：部颁规约电表自动任务组帧
*输入参数：通道号（pipe）、组帧后存放地址pBuf、帧长存放地址pSndLen、抄表任务结构体（*pTaskAllot）
*输出参数：组帧后的缓冲区、帧长度
           电表类型 bStdType   0-普通国标 1-重庆简易表 2-普通简易表
*处理：根据pTaskAllot->pTasks对应的数据ID，和表地址pTaskAllot->pMetAddr发送报文：
*      pTasks对应的命令标识上面已经列出
*返回：是否正常组帧
***************************************************************************
-*/
uint8 DLMS_AUTK_Cmd(uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms,SCHEME_TASK_RUN_INFO *pCbTask,CSD_TASK_INFO  *csd)
{
    //uint8 flag;
    //uint8 index;      
//    uint8 tmpFlag = 0;
    uint16  k_mark = 0;
//  cbtasknew_autkTag   tmpAutkTag;                             //自动任务数据填充标志结
    OOP_OAD_U subOAD;
    uint16 shortname;

    Comm_Info_dlms->CmdDI = DLMS_SN_CMDID_READ;
    Comm_Info_dlms->FraDI = 0xFFFF;             
    shortname = Comm_Info_dlms->shortname[Comm_Info_dlms->DlmsIndex];

    if(shortname ==0x0000)
    {
        return 2;
    }
    if((csd->MainOad.nObjID==0x0000)||(pCbTask->AcqInfo.Acqtype==ACQTYP_NULL))
    {
        while (RunTaskInfo->SubOadIndex<csd->SubOadNum)
        {   
            k_mark = 0;
            subOAD = csd->OadListInfo[RunTaskInfo->SubOadIndex];
            TASK_FMT_TRACE( pipe, REC_LOG, "subOAD.value  is  %x\n",subOAD.value);
            TASK_FMT_TRACE( pipe, REC_LOG, "RunTaskInfo->SubOadIndex  is  %d\n",RunTaskInfo->SubOadIndex);
      
            {
                while (SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value != 0xFFFFFFFF)
                {
                    if (shortname == SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName)
                    {
                
                        if(SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName == 0x0000)
                        {
                            k_mark++;           
                        }
                        else
                        {
                            Comm_Info_dlms->SN_DLMS = SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName;
                            Comm_Info_dlms->CLASS_DLMS = SD_MET_PRTL_DLMS_PRTL[k_mark].ClassNo;
                        
                            if(Comm_Info_dlms->FraTimes != 0)
                            {
                                // 读后续帧特殊处理
                                uint16 phyaddr = ((MtrPara->MetAddr[0])>>4)*10+(MtrPara->MetAddr[0]&0x0f)+\
                                    (MtrPara->MetAddr[1]&0x0f)*100 + ((MtrPara->MetAddr[1])>>4)*1000;
                            
                                Comm_Info_dlms->SndLen = RR_MakeFrame(Comm_Info_dlms->pSndBuf, phyaddr,Comm_Info_dlms);
                            }
                            else
                            {
                                // 正常组帧处理
                                
                                TASK_FMT_TRACE( pipe, REC_LOG, "subOAD.value  is  %x\n",subOAD.value);
                                DLMS_MakeFrame(MtrPara->MetAddr, SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName,\
                                              SD_MET_PRTL_DLMS_PRTL[k_mark].ClassNo, DLMS_SN_CMDID_READ,\
                                              Comm_Info_dlms->pSndBuf, &(Comm_Info_dlms->SndLen));
                            }

                            
                            return 1;    
                        }
                    }                       
                    else
                    {
                        k_mark++;           
                    }
                }                   

            }
            if((SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value == 0xFFFFFFFF))
            {                                           
                RunTaskInfo->SubOadIndex++;  /*数据项ID加1*/        
                Comm_Info_dlms->SucOADNum++;
                continue;                
            }
           
        }/*-end while-*/   
    }
    else
    {
        if(csd->MainOad.nObjID==0x5002)
        {
            if(Comm_Info_dlms->FraTimes != 0)
            {
                // 读后续帧特殊处理
                uint16 phyaddr = ((MtrPara->MetAddr[0])>>4)*10+(MtrPara->MetAddr[0]&0x0f)+\
                    (MtrPara->MetAddr[1]&0x0f)*100 + ((MtrPara->MetAddr[1])>>4)*1000;
            
                Comm_Info_dlms->SndLen = RR_MakeFrame(Comm_Info_dlms->pSndBuf, phyaddr,Comm_Info_dlms);
            }
            else
           {
                Dlms_Make_Record_Frame(MtrPara->MetAddr,DLMS_SN_CMDID_READ,Comm_Info_dlms->pSndBuf, &(Comm_Info_dlms->SndLen),Comm_Info_dlms,pCbTask);
           }
            return 1;
        }
    }
    return 0;                
}     



/*-
***************************************************************************
*模块编号：7-9-1
*名称：部颁规约自动任务组织发送报文模块 
*功能：部颁规约电表自动任务组帧
*输入参数：通道号（pipe）、组帧后存放地址pBuf、帧长存放地址pSndLen、抄表任务结构体（*pTaskAllot）
*输出参数：组帧后的缓冲区、帧长度
           电表类型 bStdType   0-普通国标 1-重庆简易表 2-普通简易表
*处理：根据pTaskAllot->pTasks对应的数据ID，和表地址pTaskAllot->pMetAddr发送报文：
*      pTasks对应的命令标识上面已经列出
*返回：是否正常组帧
***************************************************************************
-*/
uint8 Dlms_Read_Frame_Make(uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, cbMtrPara *MtrPara, S_COMM_INFO_MET *Comm_Info_dlms,SCHEME_TASK_RUN_INFO *pCbTask,CSD_TASK_INFO  *csd)
{
    //uint8 flag;
    //uint8 index;      
//    uint8 tmpFlag = 0;
    uint16  k_mark = 0;
//  cbtasknew_autkTag   tmpAutkTag;                             //自动任务数据填充标志结
    OOP_OAD_U subOAD;

    Comm_Info_dlms->CmdDI = DLMS_SN_CMDID_READ;
    Comm_Info_dlms->FraDI = 0xFFFF;             

     uint16 phyaddr = ((MtrPara->MetAddr[0])>>4)*10+(MtrPara->MetAddr[0]&0x0f)+\
            (MtrPara->MetAddr[1]&0x0f)*100 + ((MtrPara->MetAddr[1])>>4)*1000;
     //分帧了需要读后续帧
    if(Comm_Info_dlms->FraTimes != 0)
    {
        Comm_Info_dlms->SndLen = RR_MakeFrame(Comm_Info_dlms->pSndBuf, phyaddr,Comm_Info_dlms);
        return 1;
    }
    if(csd->MainOad.nObjID==0x0000) 
    {
        while (RunTaskInfo->SubOadIndex<csd->SubOadNum)
        {   
            k_mark = 0;
            subOAD = csd->OadListInfo[RunTaskInfo->SubOadIndex];
            {
                while (SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value != 0xFFFFFFFF)
                {
                    if (subOAD.value == SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value)
                    {
                        if(SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName == 0x0000)
                        {
                            k_mark++;           
                        }
                        else
                        {
                            Comm_Info_dlms->SN_DLMS = SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName;
                            Comm_Info_dlms->CLASS_DLMS = SD_MET_PRTL_DLMS_PRTL[k_mark].ClassNo;

                            // 正常组帧处理
                            DLMS_MakeFrame(MtrPara->MetAddr, SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName,
                                          SD_MET_PRTL_DLMS_PRTL[k_mark].ClassNo, DLMS_SN_CMDID_READ,
                                          Comm_Info_dlms->pSndBuf, &(Comm_Info_dlms->SndLen));
                            return 1;    
                        }
                    }                       
                    else
                    {
                        k_mark++;           
                    }
                }                   
            }  

            if((SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value == 0xFFFFFFFF))
            {                                           
                RunTaskInfo->SubOadIndex++;                                                            
                continue;                
            }
           
        }    
    }
    else
    {
        if(csd->MainOad.nObjID==0x5002)
        {
            Dlms_Make_Record_Frame(MtrPara->MetAddr,DLMS_SN_CMDID_READ,Comm_Info_dlms->pSndBuf, &(Comm_Info_dlms->SndLen),Comm_Info_dlms,pCbTask);

        }

    }
    return 0;                
}     
//
//
//
///****************************************************************************
//*模块编号：
//*名称：根据OAD查找EDMI映射表
//*功能：
//*输入参数：主OAD，从OAD
//*输出参数：645映射表指针，OAD索引
//*处理：
//*返回：无
//****************************************************************************/
MET_PRTL_DLMS * FindDlmsInfoPtrByOad(OOP_OAD_U mainOad, OOP_OAD_U subOad)
{

    MET_PRTL_DLMS *pMapOopToDlms = NULL;
    OadInfoMap OadInfo;

    if (mainOad.nObjID == 0x0000 && mainOad.attID == 0x00 && mainOad.nIndex == 0x00)//实时数据
    {
        pMapOopToDlms = SD_MET_PRTL_DLMS_PRTL;
    }
    else if (mainOad.nObjID == 0x5004 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//日冻结数据
    {
        pMapOopToDlms = SD_MET_PRTL_DLMS_PRTL;
    }
	else if (mainOad.nObjID == 0x5002 && mainOad.attID == 0x02 && mainOad.nIndex == 0x00)//日冻结数据
    {
        pMapOopToDlms = SD_MET_PRTL_DLMS_PRTL;
    }
    else
        return NULL;
    if(pMapOopToDlms==NULL)
        return NULL;
    subOad.attPro= 0;
    if(taskmng_oad_info_get(subOad,&OadInfo)==FALSE)
    {
        return NULL;
    }
    if(mainOad.nObjID == 0x0000&&mainOad.attID==0x00&&mainOad.nIndex==0x00)//实时数据
    {
        if(subOad.value==0x20210200)
            return NULL;
    }
    if(((OadInfo.dataType==DT_ARRAY)||(OadInfo.dataType==DT_STRUCTURE))&&(TRUE==taskmng_need_merge_check(&OadInfo)))
    {
        while(pMapOopToDlms->oad.value != 0xFFFFFFFF)
        {
            if((OadInfo.Oad.nObjID == pMapOopToDlms->oad.nObjID)&&(OadInfo.Oad.attID==pMapOopToDlms->oad.attID))
            {   
                //oadID->value= OadInfo->Oad.value;
                return pMapOopToDlms;
            }
            pMapOopToDlms++;
        }
    }
    else
    {
        while(pMapOopToDlms->oad.value != 0xFFFFFFFF)
        {
            
            if(OadInfo.Oad.value == pMapOopToDlms->oad.value)
            {   
                //oadID->value= OadInfo->Oad.value;
                return pMapOopToDlms;
            }
            pMapOopToDlms++;
        }
    }
    pMapOopToDlms = NULL;
    return pMapOopToDlms;
}
//
///****************************************************************************
//*模块编号：
//*名称：
//*功能：根据CSD和从OAD索引查找到EDMI数据标识
//*输入参数：
//*输出参数：
//*处理：
//*返回：无
//****************************************************************************/
uint8 GetDI_DLMS_485(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint16 *DIDlmsBuff, SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe)//assume index always 0
{
    const  MET_PRTL_DLMS *pMapOopToDlms;

    OadInfoMap pOadInfo;
    OOP_OAD_U mainOAD,subOAD;
    uint8  diDlmsNum = 0;

	mainOAD.value = csd->MainOad.value;
	subOAD = csd->OadListInfo[subOadIndex];
    if(csd->MainOad.nObjID==0)//OAD
    {
        mainOAD.nObjID = 0x0000;
        mainOAD.attID = 0x00;
        mainOAD.nIndex = 0x00;
        subOAD = csd->OadListInfo[subOadIndex];
    }

    if(FALSE==taskmng_oad_info_get(csd->OadListInfo[subOadIndex], &pOadInfo))
    {
       TASK_FMT_TRACE( pipe, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[subOadIndex].value); 
        return 0;
    }
    else
    {
        pMapOopToDlms= FindDlmsInfoPtrByOad(mainOAD,subOAD);//this return OAD_ID is "OAD_FREZZE_TM"

        if(pMapOopToDlms==NULL)
            return 0;  

        if(((pOadInfo.dataType==DT_ARRAY)||(pOadInfo.dataType==DT_STRUCTURE))&&(TRUE==taskmng_need_merge_check(&pOadInfo))&&(subOAD.nIndex==0))
        {
            while(pMapOopToDlms->oad.value != 0xFFFFFFFF)
            {
                if((pOadInfo.Oad.nObjID == pMapOopToDlms->oad.nObjID)&&(pOadInfo.Oad.attID==pMapOopToDlms->oad.attID))
                {   
                    if(DIDlmsBuff!=NULL)
                    {
                        if(pMapOopToDlms->ShortName !=0x0000)
                        {   
                            DIDlmsBuff[diDlmsNum]=pMapOopToDlms->ShortName;
                            diDlmsNum++;  
                        }
                    }
                    //diDlmsNum++;    
                }
                 pMapOopToDlms++;
            }
        }
        else    
        {
            while(pMapOopToDlms->oad.value != 0xFFFFFFFF)
            {
                if(pMapOopToDlms->oad.value == subOAD.value)
                {   
                    if(DIDlmsBuff!=NULL)
                    {
                        DIDlmsBuff[diDlmsNum]=pMapOopToDlms->ShortName;
                    }
                    diDlmsNum++;
                    break;
                }
                pMapOopToDlms++;
            }
        }
        return diDlmsNum;
    }
    return 0;
}


/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Send(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{
    int ret;
    
    uint8 CsdIndex,SubOadIndex;
    uint16 pn,tmpMsIndex,MeterIndex;
    CSD_TASK_INFO  *csd;


    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    CsdIndex=RunTaskInfo->CsdIndex;
    SubOadIndex=RunTaskInfo->SubOadIndex;
    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];
    
    if(CsdIndex<pCbTask->CsdTask.RoadNum)
    {
        if((csd->MainOad.nObjID==0x0000)||(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL))
        {
            Comm_Info_dlms->DlmsNum = GetDI_DLMS_485(pCbTask->AcqInfo,&pCbTask->CsdTask.RoadTask[CsdIndex],SubOadIndex,Comm_Info_dlms->shortname,pCbTask,pipe);//获取抄表的数据标识
        }
        else
        {
            Comm_Info_dlms->DlmsNum = 1;
        }
        if(Comm_Info_dlms->DlmsNum ==0)
        {
            Dlms_ToNextReadItem(FALSE,Comm_Info_dlms,pCbTask,RunTaskInfo,pipe,pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex]);
            return;
        }
        TASK_FMT_TRACE( pipe, REC_LOG, "Comm_Info_dlms->DlmsNum  %d ,SubOadIndex is %d\n",Comm_Info_dlms->DlmsNum,SubOadIndex);
        ret = DLMS_AUTK_Cmd(pipe,RunTaskInfo, MtrPara, Comm_Info_dlms,pCbTask,&pCbTask->CsdTask.RoadTask[CsdIndex]);  /*-调用自动任务组织发送报文函数-*/              

        if(ret == 0)    /*-已经到达最后的ID-*/
        {
            Comm_Info_dlms->MetStat = CB_STATE_FINISH;   //MET_IDLE;
            Comm_Info_dlms->AutkStat = CB_RET_OK;        //AUTK_SUCCESS;
            return;
        }
        else if(ret ==2)
        {
            Comm_Info_dlms->DlmsIndex++;
            Comm_Info_dlms->MetStat = CB_STATE_SEND;   //MET_IDLE;
            if(Comm_Info_dlms->DlmsIndex >=Comm_Info_dlms->DlmsNum)
            {
                Comm_Info_dlms->MetStat = CB_STATE_RECV;   //MET_IDLE;

            }
            
            return;
        }
        else
        {                                                   
        
            ret=rs485_RS232_send(pipe,MAX_FRAME_OVERTIME,MAX_CHAR_OVERTIME,MtrPara->Speed,PARITY_NO, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);/*-调用发送函数-*/

            if (ret == 0)  
            {           
    
                TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d ,报文发送\n",MtrPara->MtrNum);
                TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pSndBuf, Comm_Info_dlms->SndLen);
                Comm_Info_dlms->MetStat = CB_STATE_RECV;         //发送成功，准备接收
                taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
            }
            else
            {                                       
                //Comm_Info_dlms->ErrTimes++;
                
                TASK_FMT_TRACE( pipe, REC_LOG, "测量点号 %d\n,发送结果 ret is %d\n",MtrPara->MtrNum,ret);
                Comm_Info_dlms->MetStat = CB_STATE_FINISH;
                taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_SENDFRMNUM,NULL);
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastSendTime=time(NULL);
                return;
            }           
        }/*-end else-*/
    }
    else
    {
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
    }

}


/*-
***************************************************************************
*模块编号:
*名称:memchr_dlms
*功能:检索有效帧的帧头
*输入参数:buf
*输出参数:无
*返回:有效帧头地址
***************************************************************************
-*/
static char *memchr_dlms(char *buf, uint8 sync, uint32 buf_len)
{
    uint16 dataLen = 0;
    int i;
    char *head;

    if ((buf == NULL))
    {   
        return NULL;
    }
    
    head = buf;

    for(i=0; i<buf_len; i++)
    {
        if(*head == sync)
        {
            dataLen = ((*(head+1)&0x03)<<8)+(*(head+2));
        }

        if((*head == sync)&&(*(head+dataLen+1) == sync))
        {
            return head;
        }
        head++;
    }
    return NULL;
}

/*-
***************************************************************************
*模块编号:
*名称:InverseBuf
*功能:对pBuf缓冲区的数据进行逆序
*输入参数:电表的物理地址
*输出参数:组好的帧放入pBuf中
*返回:组帧的长度
***************************************************************************
-*/
void InverseBuf(uint8 *pBuf, uint16 len)
{
    uint8 temp = 0;
    uint16 tmplen = 0;

    for(tmplen=0; tmplen<len/2; tmplen++)
    {
        temp = pBuf[tmplen];
        pBuf[tmplen] = pBuf[len-tmplen-1];
        pBuf[len-tmplen-1] = temp;
    }
    return;
}
//
//
///*-
//***************************************************************************
//*模块编号:
//*名称:ABB规约数据解析
//*功能:ABB规约数据解析
//*输入参数:通道号（pipe）、接收数据存放的缓冲区的地址（*pBuf）、帧长存放地址（rcvLen）、抄表任务结构体（*pTaskAllot）
//*输出参数:将接收到的数据放入到XBBF[Pipe]的相应位置中
//*处理:首先根据数据类型，确定在xram中的位置，再依次按照DataId，截取数据；
//* 然后根据jl_rt_data_len[*pTasks]和偏移地址DataOffset[Pipe]，将转换后的数据存储到XBBF中，pTasks++。
//*返回:  正确解析，返回1；否则，返回0。
//***************************************************************************
//-*/
//static int DLMS_AUTK_Recv_04(uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms)
//{
//  uint8       idNum = 0;              /*-表示一帧报文能够读取的数据ID数量-*/       
//  uint8       fraOffs = 0;            /*-数据项在返回数据帧中的偏移地址-*/
//  uint8       oldFraOffs;
//  uint8       i;
//  uint8       j;
////    uint16      eDataId;
//  uint8       eDataNum;
//  uint8       k;  
////    uint16      index;
//  uint8       ret;
//  struct MET_PRTL_DLMS  *pSTDMapTmp;;
//  dataFormat  StdData;    
//  uint8       num = 0;
//  uint8   idOffMap[24];/*-回码中的数据在STD_FRADI_MAP的偏移量-*/
//  //uint8     offset;
//  dataFormat *pStdData;
//  uint8     offLen = 0;
//  uint8     offLenEnd = 0;
//  //uint8     dataStore = 0;
//  uint8     dataExite = 0;
//    
//  pStdData = (dataFormat *)&StdData;  
//  
//  //offset = 0;
//  pSTDMapTmp = &SD_MET_PRTL_DLMS_PRTL[Comm_Info_dlms->bCurMapIDIndex];
//  
//    fraOffs = CLASS_04_Offset;    
//  if (Comm_Info_dlms->FraTimes == 0)
//  {
//      fraOffs += 11;
//  }
//  else
//  {
//      fraOffs += 1;
//  }
//
//  i = fraOffs;
//    
//  while (i < Comm_Info_dlms->RcvLen)
//  {
//      switch(Comm_Info_dlms->pRcvBuf[i])
//      {
//          case 0x05:
//              pStdData->SrcDataLen = 4;
//              offLen = 1;
//              break;
//              
//          case 0x06:
//              pStdData->SrcDataLen = 4;
//              offLen = 1;
//              break;
//              
//          case 0x09:          //字符串 
//              pStdData->SrcDataLen = 8;
//              offLen = 2+4;
//              break;
//              
//          case 0x14:          //字符串 
//              pStdData->SrcDataLen = 8;
//              offLen = 1;
//              break;                              
//          default:
//              break;
//      }
//      //printf("pBuf[i] = %d offLen = %d pStdData->SrcDataLen = %d\n", pBuf[i], offLen, pStdData->SrcDataLen);
//      if ((Comm_Info_dlms->pRcvBuf[i] == 0x05)&&(Comm_Info_dlms->pRcvBuf[i+offLen+pStdData->SrcDataLen] == 0x09))
//      {
//          if(dataExite == 0)
//          {
//              fraOffs = i;
//          }
//          dataExite++;
//      }
//
//      i = i + offLen + pStdData->SrcDataLen;
//        
//      if(i > Comm_Info_dlms->RcvLen)
//      {
//          break;
//      }
//  }
//  //不存在数据返回
//  if(dataExite == 0)
//    { 
//      return 0;
//  }   
//  
//  //printf("dataExite = %d  fraOffs = %d  *(pTaskAllot->pTaskID) = %d *(pTaskAllot->pTasks) = %d\n", dataExite, fraOffs, *(pTaskAllot->pTaskID), *(pTaskAllot->pTasks));
//  //printf("pTaskAllot->offset = %d\n", pTaskAllot->offset);
//  while (pSTDMapTmp->bDataId != 0xffff)
//  {
//      if (Comm_Info_dlms->SN_DLMS == pSTDMapTmp->ShortName)
//      {
//          idNum = dataExite*2;                //有一对05和09时代表有大小和时间是两个值
//          idOffMap[num] = Comm_Info_dlms->bCurMapIDIndex;     
//          num++;
//          if(num >= idNum) 
//          {   
//              Comm_Info_dlms->bCurMapIDIndex++;
//              break;  
//          }
//      }
//      Comm_Info_dlms->bCurMapIDIndex++;
//      pSTDMapTmp = &SD_MET_PRTL_DLMS_PRTL[Comm_Info_dlms->bCurMapIDIndex];                
//  }                   
//
//  //将位置定死了,防止数据会乱
///*    if(((pTaskAllot->FraTimes == 1)&&(MET_PRTL_DLMS_PRTL[idOffMap[0]].bDataId == SY_POS_Z_AD_MAX_EV_ID))
//      ||((pTaskAllot->FraTimes == 2)&&(MET_PRTL_DLMS_PRTL[idOffMap[0]].bDataId == SY_NEG_Z_AD_MAX_EV_ID))
//      ||((pTaskAllot->FraTimes == 3)&&(MET_PRTL_DLMS_PRTL[idOffMap[0]].bDataId == SY_NEG_2_AD_MAX_EV_ID))
//      )
//  {
//
//  }
//  else
//  {
//      return 0;
//  }
//*/
//
//  for(i = 0; i < idNum; i++)
//  {   
//      memset(pStdData, 0, sizeof(dataFormat));
//        
//      pStdData->DataId2 = 0xFFFF;     
//      pSTDMapTmp = &SD_MET_PRTL_DLMS_PRTL[idOffMap[i]];   
//      pStdData->DataNum = pSTDMapTmp->dataNum;
//      pStdData->DataId = pSTDMapTmp->bDataId;
//      pStdData->SrcDataLen    = pSTDMapTmp->srcDataLen;
//      pStdData->SrcDecLen = pSTDMapTmp->srcDecLen;
//      pStdData->Rate  = pSTDMapTmp->rate;
//        //pStdData->DataType = Comm_Info_dlms->pCurcbTask->pQueueTaskID->type;
//      eDataNum = 0;
//      
//      if(pStdData->DataId != 0xFFFF) 
//      {
//          eDataNum++;
//      }
//      if(pStdData->DataId2 != 0xFFFF) 
//      {
//          eDataNum++;
//      }   
//      
//      oldFraOffs = fraOffs;
//
//      for(k = 0; k < eDataNum; k++)/*-同一数据内容对应几个数据ID-*/
//      {   
//          fraOffs = oldFraOffs;                   
//          
////            if(k == 0) eDataId = pStdData->DataId;
////            else eDataId = pStdData->DataId2;
//
//          //ret = cbtasknew_IsInTaskId(pipe, StdData.type, eDataId, Comm_Info_dlms->pCurcbTask->pQueueTaskID, &index, Comm_Info_dlms->TaskType);
//
//          for(j = 0; j < pStdData->DataNum; j++)
//          {
//              //dataStore = 0; 
//                  
//              if(ret)         
//              {                                                               
//                  //printf(" pBuf[fraOffs] = %02x\n", pBuf[fraOffs]);
//                  //时间仓促暂时先用偏移来算数据,求有缘人花点时间将数据整成结构体的形式
//                  switch(Comm_Info_dlms->pRcvBuf[fraOffs])
//                  {
//                      case 0x05:
//                          pStdData->SrcDataLen = 4;
//                          offLen = 1;
//                          offLenEnd = 0;
//                          break;
//                          
//                      case 0x06:
//                          pStdData->SrcDataLen = 4;
//                          offLen = 1;
//                          offLenEnd = 0;
//                          break;
//                          
//                      case 0x09:          //字符串 
//                          pStdData->SrcDataLen = 8;
//                          offLen = 2;
//                          offLenEnd = 4;
//                          break;
//                          
//                      case 0x14:          //字符串 
//                          pStdData->SrcDataLen = 8;
//                          offLen = 1;
//                          offLenEnd = 0;
//                          break;                              
//                      default:
//                          break;
//                  }
//                  //printf(" pBuf[fraOffs] = %02x pBuf[fraOffs+offLen+pStdData->SrcDataLen] = %02x\n", pBuf[fraOffs], pBuf[fraOffs+offLen+pStdData->SrcDataLen]);
//                  fraOffs = fraOffs + offLen;
//
//                  pStdData->SrcDecLen = 1;    //需量的小数点是1位
//                  InverseBuf(&Comm_Info_dlms->pRcvBuf[fraOffs], pStdData->SrcDataLen);
//                  pStdData->pSrcBuf = &Comm_Info_dlms->pRcvBuf[fraOffs];                                          
//                  //FormatToDLMS(pipe, BIN, eDataId, pStdData, 0);                                
//                    //cbtasknew_DataToXBBF(AUTKTAG_SUCCESS, pipe, index, j, &StdData, Comm_Info_dlms->TaskType); /*清除相应缓冲区数据,加上四费率的偏移量*/
//                }
//              fraOffs += (pStdData->SrcDataLen + offLenEnd);                  
//          }/*-考虑四费率的情况-*/
//      }/*-数据项正反差别（根据符号来判断的）-*/              
//  }       
//  
//  if(dataExite >= 1)
//    { 
//      return 1;
//  }
//  else
//  {
//      return 0;
//  }
//}



/*-
***************************************************************************
*模块编号:
*名称:ABB规约数据解析
*功能:ABB规约数据解析
*输入参数:通道号（pipe）、接收数据存放的缓冲区的地址（*pBuf）、帧长存放地址（rcvLen）、抄表任务结构体（*pTaskAllot）
*输出参数:将接收到的数据放入到XBBF[Pipe]的相应位置中
*处理:首先根据数据类型，确定在xram中的位置，再依次按照DataId，截取数据；
*   然后根据jl_rt_data_len[*pTasks]和偏移地址DataOffset[Pipe]，将转换后的数据存储到XBBF中，pTasks++。
*返回:  正确解析，返回1；否则，返回0。
***************************************************************************
-*/
void DLMS_AUTK_Recv(uint8 pipe, uint8* pbuf,uint16 revlen,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,cbMtrPara *MtrPara,DateTimeBcd_t *StoreTime,OOP_OAD_U oad,uint8 dlsnum)
{
//  uint8       idNum = 0;              /*-表示一帧报文能够读取的数据ID数量-*/       
//  uint8       fraOffs = 0;            /*-数据项在返回数据帧中的偏移地址-*/
//  uint8       oldFraOffs;
//  uint8       i;
//  uint8       j;
    //uint16        eDataId;
//  uint8       eDataNum;
//  uint8       k;  
//  uint16      index;
//  uint8       ret;
//  struct MET_PRTL_DLMS  *pSTDMapTmp;;
//  dataFormat  StdData;    
//  uint8       num = 0;
//  uint8   idOffMap[24];/*-回码中的数据在STD_FRADI_MAP的偏移量-*/
//    uint8   tmpOff;
//  uint16      offset;
//  dataFormat *pStdData;
//  int8     dataRate = 1;
    //uint8     offLen = 0;
    //uint8     dataStore = 0;
//  S_CbIDInfo pCbTaskID; //= //Comm_Info_dlms->pCurcbTask->pQueueTaskID;
    
//  uint16 offSet=0;
    time_t longTm;
    time_t colStartTm;
    uint16 tmpoffset=0;
    uint16 pn;
    uint16 MsIndex;
    OadStoreInfo StoreInfo;
    uint16 CsdIndex;
    uint8 MtrAddr[6];
    uint8 desbuffer[2000];
    uint16 MeterIndex;
    uint16 deslength=0;
    OadInfoMap pOadInfo;
    DateTimeBcd_t taskStoreTime;
    DateTimeBcd_t taskStartTime;
    uint8 pwrType;
    uint8 Csdtype=0;
//   
//
    CsdIndex=RunTaskInfo->CsdIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    MsIndex=taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    pwrType=taskmng_mtr_conntype_get(pn,RunTaskInfo->logicid);
    //根据OAD查找OAD属性描述
    if(FALSE==taskmng_oad_info_get(oad, &pOadInfo))
    {
        return ;
    }
    if((pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID !=0x0000)&&(pCbTask->AcqInfo.Acqtype !=ACQTYP_NULL))
    {
        Csdtype=1;
    }
    TASK_BUF_TRACE( pipe, REC_LOG, pbuf,revlen);
    Convert_Dlms_Data_To_Struct(pbuf,&tmpoffset,revlen,desbuffer,&pOadInfo,&deslength,pwrType,Csdtype,dlsnum);
    
    memcpy_r((uint8*)&taskStartTime,(uint8*)RunTaskInfo->ThisTurnBeginTime,7);//采集启动时间
       DT_DateTimeBCD2Time(&taskStartTime,&colStartTm);

       if(StoreTime==NULL)
       {
           time_t storeTm;
           OOP_TI_T delayTi;
           DT_DateTimeBCD2Time(&taskStartTime,&storeTm);
           delayTi.unit = pCbTask->DelayTimeUnit;
           delayTi.value = pCbTask->DelayTime;
           DT_TimeSubInv(&storeTm, delayTi);
           taskStoreTime = taskmng_acq_store_time_get(pCbTask->StoreTimeType,storeTm);
           DT_DateTimeBCD2Time(&taskStoreTime,&longTm);
           if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
               ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据
    
               {
                   //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);//不需要抄读
                   taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                   taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
               }
    
       }
       else
       {
           if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5004 || pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5006
               ||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5002||pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID == 0x5005)// 日月曲线数据
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
                       return;
                   }
               //set_bit_value(RunTaskInfo->MtrResult,128,MsIndex);
               taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
               taskmng_task_record_file_set(pipe,MsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
    
               }
               DT_DateTimeBCD2Time(StoreTime,&longTm);
    
           }
          else
           {
               DT_DateTimeBCD2Time(StoreTime,&longTm);
           }
       } 
       taskmng_mtr_addr_get(pn,RunTaskInfo->logicid,MtrAddr);
       memcpy(StoreInfo.MAC.nValue,MtrAddr,6);
       StoreInfo.recordOAD.infoNum=pn;
       StoreInfo.recordOAD.logicId=RunTaskInfo->logicid;
       if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x0000)
       {
    
           StoreInfo.recordOAD.road.oadMain.value= 0x00;
           StoreInfo.recordOAD.road.oadCols.nNum=1;
           StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
           StoreInfo.recordOAD.road.oadCols.rsv=0;
       }
       else
       {
           StoreInfo.recordOAD.road.oadMain=pCbTask->CsdTask.RoadTask[CsdIndex].MainOad;
           StoreInfo.recordOAD.road.oadCols.nNum=1;
           StoreInfo.recordOAD.road.oadCols.oad[0]=oad;
           StoreInfo.recordOAD.road.oadCols.rsv=0;
       }
       StoreInfo.recordOAD.classtag = 5;
       StoreInfo.colStartTm=colStartTm;
       StoreInfo.colStoreTm=longTm;
       StoreInfo.colEndTm=time(NULL);
       TASK_FMT_DEBUG( pipe, REC_LOG, "longTm is %d\n",longTm);
       
       taskmng_meter_data_save(desbuffer,deslength,StoreInfo,pipe);
    
       
  
    return;
}


/****************************************************************************
*模块编号：
*名称：
*功能：按照MAP中的顺序进行多帧合并
*输入参数：
*输出参数：
*处理：
*返回：无
****************************************************************************/
BOOL STDDlms_FrameCombin(S_COMM_INFO_MET *Comm_Info_dlms,CTS_RUN_TASK_T *RunTaskInfo,SCHEME_TASK_RUN_INFO *pCbTask)
{
    uint16               offSet=0;
    //uint8 tmpbuf[100];
    uint8 tmplen=0;
    uint8 CsdIndex, SubOadIndex;
    uint32 rsv=0;
    CSD_TASK_INFO  *csd;
    OadInfoMap pOadInfo;
    uint8 index;
    uint16  k_mark = 0;
    uint16 shortname=0;
    OOP_OAD_U subOAD;
    OOP_OAD_U tmpoad;

    CsdIndex=RunTaskInfo->CsdIndex;
    csd = &pCbTask->CsdTask.RoadTask[CsdIndex];
    SubOadIndex=RunTaskInfo->SubOadIndex;
    rsv = Comm_Info_dlms->rsv;
    subOAD = csd->OadListInfo[SubOadIndex];
    shortname =Comm_Info_dlms->shortname[Comm_Info_dlms->DlmsIndex];

    if(FALSE==taskmng_oad_info_get(subOAD, &pOadInfo))
    {
       TASK_FMT_TRACE( 2, COMM_LOG, "该次OAD不存在 csd->OadListInfo[subOadIndex].value is %x\n", csd->OadListInfo[SubOadIndex].value); 
        return 0;
    }

    if(TRUE==taskmng_need_merge_check(&pOadInfo))
    {
        while (SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value != 0xFFFFFFFF)
        {
            if(SD_MET_PRTL_DLMS_PRTL[k_mark].ShortName == shortname )
            {
                tmpoad.value = SD_MET_PRTL_DLMS_PRTL[k_mark].oad.value;
                index = tmpoad.nIndex;
                if(index>0)
                index--;
                set_bit_value((uint8*)&rsv, 32, index);
                break;
            }
            k_mark++;
        }
    }

    Comm_Info_dlms->rsv = rsv;
    Comm_Info_dlms->combinbuf[0]++;//个数增加
    memcpy(&Comm_Info_dlms->combinbuf[1],(uint8*)&rsv,4);//块抄用来表示那个分项有效
    if(Comm_Info_dlms->combinoffset==0)
    {
        Comm_Info_dlms->combinoffset=5;
    }
    offSet=Comm_Info_dlms->combinoffset;
    tmplen = Comm_Info_dlms->pRcvBuf[2];
    Comm_Info_dlms->combinbuf[offSet++] = tmplen-18;
    memcpy(&Comm_Info_dlms->combinbuf[offSet],&Comm_Info_dlms->pRcvBuf[17],tmplen-18);
    offSet =offSet+tmplen-18;
    Comm_Info_dlms->combinoffset=offSet;  
    
    TASK_BUF_TRACE( 2, REC_LOG, Comm_Info_dlms->combinbuf,Comm_Info_dlms->combinoffset);
    return TRUE;
}


/****************************************************************************
*函数名称：Oop_ToNextReadItem
*功能：  切换抄读的数据项
*输入参数：
*输出参数：
*返回：无
****************************************************************************/

void  Dlms_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,OOP_OAD_U Suboad)
{
    time_t    tmpTime;
    time_t    tmpendTime;
    uint8 CSDType;
    uint16 CsdIndex,SubOadIndex;
    uint16 MeterIndex,pn,tmpMsIndex;
    OOP_TI_T    delayTi;

     delayTi.value = pCbTask->DelayTime;
     delayTi.unit = pCbTask->DelayTimeUnit;

    CsdIndex=RunTaskInfo->CsdIndex;
    SubOadIndex=RunTaskInfo->SubOadIndex;

    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);

    if(pCbTask==NULL || (CsdIndex>=pCbTask->CsdTask.RoadNum))
    {
        return;
    }

    if(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID!=0x0000)
    {
        CSDType=1;
    }
    else
    {
        CSDType=0;
    }
 
    if(NextCSDFlg==FALSE)
    {
        if(CSDType>0)
        {
            if(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL)
            {
                if((SubOadIndex+1)<pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
                {
                    RunTaskInfo->SubOadIndex++;//转到下一个从OAD
                    pComm_Info->MetStat = CB_STATE_SEND;
                    return;
                }
            }
            else
            {
                if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
                        ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003)
                        #if(1 == OOP_DAY_ADDREAD_485)
                        ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004)
                        #else
                        ||((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC))
                        #endif
                        )
                {

                    //分钟曲线需要判断所有的时间点是否都已抄完
                    tmpTime = pComm_Info->cbLPPara.startTime;
                    DT_TimeAddInv(&tmpTime,pComm_Info->cbLPPara.tmInv);
                    if(tmpTime > pComm_Info->cbLPPara.endTime)
                    {
                        if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
                            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003))
                        {
                            OOP_TI_T tmInv;
                            tmInv = pComm_Info->cbLPPara.tmInv;
                            tmInv.value = tmInv.value/4;
                            DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,tmInv);
                        }
                        else
                            DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);
                    }
                    else
                        DT_TimeAddInv(&pComm_Info->cbLPPara.startTime,pComm_Info->cbLPPara.tmInv);

                    tmpendTime = pComm_Info->cbLPPara.endTime;

                    DT_TimeSubInv(&tmpendTime, delayTi);

                    if(pComm_Info->cbLPPara.startTime < tmpendTime)
                    {
                        RunTaskInfo->ProfilePara.startTime = pComm_Info->cbLPPara.startTime;
                        pComm_Info->MetStat = CB_STATE_SEND;
                        return;
                    }
                }

            }
        }
    }
    if(CSDType==0)
    {
        RunTaskInfo->SubOadIndex++;
        if(NextCSDFlg==FALSE)
        {
            pComm_Info->SucOADNum++;
        }
        if((RunTaskInfo->SubOadIndex)>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
        {
            
            if(pComm_Info->SucOADNum>=pCbTask->CsdTask.RoadTask[CsdIndex].SubOadNum)
            {                     
                taskmng_mtr_result_set(MeterIndex,RunTaskInfo->MtrResult);
                taskmng_task_record_file_set(pipe,tmpMsIndex,RunTaskInfo->TaskId,RunTaskInfo->logicid);
               
            }
            RunTaskInfo->CsdIndex++;
        }
    }
    else
    {
        RunTaskInfo->CsdIndex++;//有疑问的方法，是该切换CsdIndex，还是切换SubOadIndex;
        RunTaskInfo->SubOadIndex= 0;

    }
    CsdIndex=RunTaskInfo->CsdIndex;
    if((CsdIndex<pCbTask->CsdTask.RoadNum)&&(pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x3000))
    {
        if(CSDType==1)
        {
            RunTaskInfo->SubOadIndex= 0;
        }
        if((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5002)
            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5003)
            #if(1 == OOP_DAY_ADDREAD_485)
            ||(CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004)
            #else
            ||((CSDType==1 &&pCbTask->CsdTask.RoadTask[CsdIndex].MainOad.nObjID==0x5004) && (pCbTask->AcqInfo.Acqtype == ACQTYP_BC))
            #endif
            )
            Oop_LoadPrifle_Init(&pComm_Info->cbLPPara,pCbTask,RunTaskInfo);
    }
    pComm_Info->MetStat = CB_STATE_SEND;

}

/****************************************************************************
*函数名称：Deal_Receive_Result_Oop
*功能：  对面向对象规约485抄表返回的数据的结果进行处理
        抄读成功就切换下一个数据项进行抄读;
        抄读失败会进行重试
       
*输入参数：
*输出参数：
*返回：无

-           无
****************************************************************************/

CB_STATE Deal_Receive_Result_Dlms(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, BOOL actResult,uint8 pipe,OOP_OAD_U Suboad)
{
    uint16 pn,tmpMsIndex,MeterIndex;
    MeterIndex=RunTaskInfo->MeterIndex;
    pn=pCbTask->MsInfo[MeterIndex].Sn;
    tmpMsIndex = taskmng_ms_index_get(pipe,pn,RunTaskInfo->logicid);
    if(actResult)                          //该帧交换正确
    {
        Dlms_ToNextReadItem(FALSE,Comm_Info_Oop,pCbTask,RunTaskInfo,pipe,Suboad);
        Comm_Info_Oop->ComFrame++;          //总帧数增加
        Comm_Info_Oop->ErrFrame = 0;        //失败帧数和错误帧数清零，准备重新计数
        Comm_Info_Oop->ErrTimes = 0;
        taskmng_task_monitor_update(pipe,pCbTask->TaskId,pCbTask->logicid,UDTASK_RECVFRMNUM,NULL);//pCbTask->TaskMonitor.recvFrameNum ++;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt = 0;
        gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = TRUE;
    }
    else                                            //该帧交换失败
    {
        if(Comm_Info_Oop->ErrTimes >= ERR_TIMES_MAX)        //同一帧的失败次数
        {
            if(Comm_Info_Oop->ErrFrame >= ERR_FRAME_MAX)      //失败次数过多，不再继续，准备退出
            {
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
                gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
                Comm_Info_Oop->ErrFrame = ERR_FRAME_MAX;
                Comm_Info_Oop->MetStat = CB_STATE_FINISH;
                return Comm_Info_Oop->MetStat;
            }
            else
            {
                Comm_Info_Oop->ErrFrame++;                        //失败帧数增加
                Comm_Info_Oop->ComFrame++;                        //总帧数增加

                Comm_Info_Oop->ErrTimes = 0;                         //重试次数清零

                Dlms_ToNextReadItem(TRUE,Comm_Info_Oop,pCbTask,RunTaskInfo,pipe,Suboad);
            }
            
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastResult = FALSE;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].AllFailCnt++;
            gAmrThreadArg[pipe].pMtrAmrInfo[tmpMsIndex].LastFailTime=time(NULL);
        }
        else
        {
            Comm_Info_Oop->ErrTimes++;                              //重发当前数据项

        }
    }
    Comm_Info_Oop->MetStat = CB_STATE_SEND;                    //转入发送流程
    return Comm_Info_Oop->MetStat;
}

/**
*********************************************************************
* @brief:      面向对象规约电表抄读报文组帧和发送函数
* @param[in]：   pipe  线程号,485模块共8个线程,485的线程pipe从2开始到9 
* @param[in]：   SCHEME_TASK_RUN_INFO *pCbTask  当前执行的任务内容
* @param[in]：   CTS_RUN_TASK_T *RunTaskInfo  运行的任务信息
* @param[in]：   S_COMM_INFO_MET *Comm_Info_Oop   抄读的状态信息以及接收报文的缓存
* @param[in]：   cbMtrPara *MtrPara   抄读的电表的信息
* @return：
*********************************************************************
*/

void Meter_Dlms_Recv(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_dlms,cbMtrPara *MtrPara)
{      
    int ret;//recv_04_ret;
    BOOL actResult;
    uint8 CsdIndex,SubOadIndex;;
    OOP_OAD_U Suboad;
    
    CSD_TASK_INFO  *csd;
    CsdIndex=RunTaskInfo->CsdIndex;
    csd=&pCbTask->CsdTask.RoadTask[CsdIndex];
    SubOadIndex=RunTaskInfo->SubOadIndex;
    
    memset(Comm_Info_dlms->pRcvBuf, 0x00, sizeof(Comm_Info_dlms->pRcvBuf));                             
    Comm_Info_dlms->RcvLen= rs485_RS232_recv(pipe,Comm_Info_dlms->pRcvBuf, sizeof(Comm_Info_dlms->pRcvBuf));
    TASK_FMT_TRACE( pipe, REC_LOG, "接收报文\n");
    TASK_BUF_TRACE( pipe, REC_LOG, Comm_Info_dlms->pRcvBuf, Comm_Info_dlms->RcvLen);
    if (Comm_Info_dlms->RcvLen == 0)
    {
        actResult = FALSE;
    }
    else 
    {
        uint8 *ptr = (uint8*)memchr_dlms((char*)Comm_Info_dlms->pRcvBuf, SD_DLMSPara.Sync, Comm_Info_dlms->RcvLen);
        if (NULL == ptr)
        {
            actResult = FALSE;
        }
        else
        {
            Comm_Info_dlms->RcvLen -= (ptr - Comm_Info_dlms->pRcvBuf);
            memmove(Comm_Info_dlms->pRcvBuf, ptr, Comm_Info_dlms->RcvLen);
            
        }
        Suboad=pCbTask->CsdTask.RoadTask[CsdIndex].OadListInfo[SubOadIndex];
        ret = DLMS_CheckRspFrame(pipe, *MtrPara, Comm_Info_dlms);
        


        if ((ret == 3) ||(ret == 4)||(ret == 5)||(ret == 6)) /*-有效，但无数据返回-*/
        {                                                   
            actResult = TRUE;                   
        }
        else
        {
            if ((ret == 1)||(ret == 2))  /*-有效,且有数据返回-*/
            {    
                if((csd->MainOad.nObjID==0x0000)||(pCbTask->AcqInfo.Acqtype == ACQTYP_NULL))
                {
                    if(Comm_Info_dlms->DlmsNum ==1)
                    {
                        DLMS_AUTK_Recv(pipe, Comm_Info_dlms->pRcvBuf+17,Comm_Info_dlms->RcvLen-20,pCbTask,RunTaskInfo,MtrPara,NULL,csd->OadListInfo[RunTaskInfo->SubOadIndex],Comm_Info_dlms->DlmsNum);         
                    }
                    else
                    {
                        if(Comm_Info_dlms->DlmsIndex<Comm_Info_dlms->DlmsNum)
                        {
                            STDDlms_FrameCombin(Comm_Info_dlms,RunTaskInfo,pCbTask);
                        }
                        Comm_Info_dlms->DlmsIndex++;
                        if(Comm_Info_dlms->DlmsIndex>=Comm_Info_dlms->DlmsNum)
                        {
                            Comm_Info_dlms->DlmsIndex =0;
                            DLMS_AUTK_Recv(pipe, Comm_Info_dlms->combinbuf,Comm_Info_dlms->combinoffset,pCbTask,RunTaskInfo,MtrPara,NULL,csd->OadListInfo[RunTaskInfo->SubOadIndex],Comm_Info_dlms->DlmsNum);  
                            Comm_Info_dlms->combinoffset =0;
                            memset(Comm_Info_dlms->combinbuf,0x00,sizeof(Comm_Info_dlms->combinbuf));
                            Comm_Info_dlms->rsv=0;
                        }
                        else
                        {
                            Comm_Info_dlms->MetStat = CB_STATE_SEND;
                            return;
                        }
                    }
                }
                else
                {

                }
                actResult = TRUE;
            }
            else  /*-报文无效、错误-*/
            {
                actResult = FALSE;
            }
        }           

    }

    if (actResult)  /*-该帧交换正确-*/
    {                                       
        if (ret == 2)
        {
            Comm_Info_dlms->FraTimes++;
            Comm_Info_dlms->ErrFrame = 0;
            Comm_Info_dlms->ErrTimes = 0;       
            //帧正确切换到读后续帧,序号减1,在发送的时候再加1
            if (Comm_Info_dlms->FraTimes == 1)
            {
                Comm_Info_dlms->recvseq = (((Comm_Info_dlms->recvseq>>5)-1)<<5);
                RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
            }
//                        if(recv_04_ret == 1)
//                        {                                
//                            Comm_Info_dlms->ComFrame++; 
//                          RunTaskInfo->SubOadIndex++; 
//                        }
            Comm_Info_dlms->MetStat = CB_STATE_SEND;   
            return;

        }
        else
        {                        
            Comm_Info_dlms->FraTimes = 0;
            Comm_Info_dlms->ComFrame++;                       
            Comm_Info_dlms->ErrFrame = 0;
            Comm_Info_dlms->ErrTimes = 0;             

//                        if (ret != 6)
//                        {
//                          RunTaskInfo->SubOadIndex++;  
//                        }
        }
    }
    else  /*-该帧交换失败-*/
    {                       
//        Comm_Info_dlms->ErrTimes++;                     
//            
//        if (Comm_Info_dlms->ErrTimes >= ERR_TIMES_MAX)
//        {
//            Comm_Info_dlms->ErrFrame++;             
//            Comm_Info_dlms->ComFrame++;
//            Comm_Info_dlms->ErrTimes = 0;                               
//            //RunTaskInfo->SubOadIndex++;                                           
//        }
        //帧失败读后续帧的时候,序号减1,在重发的时候再加1
        if (Comm_Info_dlms->FraTimes != 0)
        {
            Comm_Info_dlms->recvseq = (((Comm_Info_dlms->recvseq>>5)-1)<<5);
            RECV_SEQ = (((RECV_SEQ>>5)-1)<<5);
        }
    }

    //Comm_Info_dlms->MetStat = CB_STATE_SEND;                                                           

    //电表拒绝连接重抄
    if(ret == 6)
    {
        Comm_Info_dlms->MetStat = CB_STATE_FINISH;
        Comm_Info_dlms->AutkStat = CB_RET_FAIL;
    }
    Deal_Receive_Result_Dlms(Comm_Info_dlms,pCbTask,RunTaskInfo,actResult,pipe,Suboad);
}
//
//
///****************************************************************************
//名称： Prepare_645_07_meter_read
//功能：
//-           为645_07规约的电表的抄读做准备，检查执行任务的测量的信息
//处理流程：
//-           
//输入参数：
//-           TASK_RUN_INFO *pCbTask, uint8* pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara
//-       
//输出参数：
//-           无
//返回：
//-           无
//****************************************************************************/
//
//void Prepare_Dlms_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara)
//{
//
//
//    uint16 sn;
//    uint16 MeterIndex;
//    OOP_METER_BASIC_T MeterBasicInfo;
//    //task_id=pCbTask->TaskId;
//    MeterIndex=RunTaskInfo->MeterIndex;
//    sn=pCbTask->MsInfo[MeterIndex].Sn;
//    //读取测量点信息
//    memset((void *)MtrPara, 0x0, sizeof(cbMtrPara));
//
//    //读取测量点信息
//    taskmng_mtr_basic_info_get(sn,RunTaskInfo->logicid,&MeterBasicInfo);
//    MtrPara->MtrNum=sn;
//    MtrPara->Speed=9600;
//    memcpy_r(MtrPara->MetAddr,MeterBasicInfo.tsa.add,6);
//    memcpy((uint8 *)&MtrPara->MtrCommAddr,(uint8*)&MeterBasicInfo.tsa,sizeof(OOP_TSA_T));
//    memset(Comm_Info_Oop,0,sizeof(S_COMM_INFO_MET));
// 
//    Comm_Info_Oop->pMtrPara = MtrPara;
//    Comm_Info_Oop->MetStat = CB_STATE_INIT;
//    //读取通信端口信息COM
//    //STD2007_LoadPrifle_Init(&Comm_Info_Oop->cbLPPara,pCbTask,RunTaskInfo);//有疑问的地方，算补抄的？？？？？？？？？
//}


void  DLMS_Frz_Cmd_Init(S_COMM_INFO_MET *pdlms_comm,SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo)
{
//    uint8 CsdIndex;
//    CSD_TASK_INFO  *csd;
//    
//    CsdIndex=RunTaskInfo->CsdIndex;
//    csd =&pCbTask->CsdTask.RoadTask[CsdIndex];

    
    uint8 index=0;
    switch (pdlms_comm->frzType)
    {
    case eCAPTURE_DAY:
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_PERIOD_DAY;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_OBJECT_DAY;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_DATA_DAY;
        pdlms_comm->FrzFrmTypeId    = 0;
        pdlms_comm->FrzFrmTypeTotal = index;
        break;

    case eCAPTURE_MON:
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_PERIOD_MON;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_OBJECT_MON;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_DATA_MON;
        pdlms_comm->FrzFrmTypeId    = 0;
        pdlms_comm->FrzFrmTypeTotal = index;
        break;

    case eCAPTURE_CURVE_1:
        //pdlms_comm->FrzFrmType[index++]   = DLMS_DLMS_CAPTURE_PERIOD_CURVE1;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_OBJECT_CURVE1;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_DATA_CURVE1;
        pdlms_comm->FrzFrmTypeId    = 0;
        pdlms_comm->FrzFrmTypeTotal = index;
        break;

    case eCAPTURE_CURVE_2:
        //pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_PERIOD_CURVE2;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_OBJECT_CURVE2;
        pdlms_comm->FrzFrmType[index++] = DLMS_DLMS_CAPTURE_DATA_CURVE2;
        pdlms_comm->FrzFrmTypeId    = 0;
        pdlms_comm->FrzFrmTypeTotal = index;
        break;

    default :
        break;
        
    }
    return;
}

/*-
***************************************************************************
*模块编号：7-9-1-1
*名称：组织抄表任务发送帧 
*功能：部颁规约电表抄表组帧
*输入参数：电表地址pMetAddr、数据标识fraDI、命令标识cmdDI、帧存放缓冲区地址pBuf、帧长存放地址pSndLen
*输出参数：组帧后的缓冲区、帧长
           电表类型 bStdType   0-普通国标 1-重庆简易表

*处理：根据电表规约组帧
*返回：无
***************************************************************************
-*/
void DLMS_MakeFrzFrame(uint8 *pMetAddr, uint8 Class, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pdlms_comm)
{
 //   int pos=0;
//    int  len_llc_frame;
//  uint8 ctrl_code;
//    uint8 axdr[1000];
//  uint8 hdlcaddr[10];   
    //uint8 ShortNameLen = 0;
//  int returnLen;
    uint16 phyaddr;
//    axdr[pos++] = cmdDI;  //[195] Get_request

    if (!pdlms_comm)
    {
        *pSndLen=0;
        return;
    }

    phyaddr = ((pMetAddr[0])>>4)*10+(pMetAddr[0]&0x0f)+\
    (pMetAddr[1]&0x0f)*100 + ((pMetAddr[1])>>4)*1000;
    if(pdlms_comm->FraTimes != 0)
    {
        *pSndLen= RR_MakeFrame(pBuf, phyaddr,pdlms_comm);
        return;
    }
    if(Class == CLASS_07)
    {

	    TASK_FMT_TRACE( 2, REC_LOG, "3333Comm_Info_dlms->FrzFrmTypeId %d ,Comm_Info_dlms->FrzFrmTypeTotal is %d,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId] is %d\n",pdlms_comm->FrzFrmTypeId,pdlms_comm->FrzFrmTypeTotal,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId]);
    
        switch(pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId])
        {
        case DLMS_DLMS_CAPTURE_PERIOD_DAY:
        case DLMS_DLMS_CAPTURE_PERIOD_MON:
        case DLMS_DLMS_CAPTURE_PERIOD_CURVE1:
        case DLMS_DLMS_CAPTURE_PERIOD_CURVE2:
            *pSndLen=Read_PERIOD_MakeFrame(pdlms_comm,pBuf,phyaddr);
            return;
        
        case DLMS_DLMS_CAPTURE_OBJECT_DAY:
        case DLMS_DLMS_CAPTURE_OBJECT_MON:
        case DLMS_DLMS_CAPTURE_OBJECT_CURVE1:
        case DLMS_DLMS_CAPTURE_OBJECT_CURVE2:
            *pSndLen=Read_CAPTURE_OBJECT_MakeFrame(pdlms_comm,pBuf,phyaddr);
            return;
        
        case DLMS_DLMS_CAPTURE_DATA_DAY:
        case DLMS_DLMS_CAPTURE_DATA_MON:
        case DLMS_DLMS_CAPTURE_DATA_CURVE1:
        case DLMS_DLMS_CAPTURE_DATA_CURVE2:
            *pSndLen=Read_CAPTURE_DATA_MakeFrame(pdlms_comm,pBuf,phyaddr);
            return;
        
        default :
            *pSndLen=0;
            
        }
    
    }
    
    return;
}

//保存冻结数据，type为冻结类型
int DLMS_saveFrzParaAndData(S_COMM_INFO_MET *pdlms_comm,uint8 *recvBuf, uint16 recvLen, uint8 *met_addr, uint8 * RRFlag,SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo)
{
    uint16 k,i = 0;
    uint16 len = 0;
    uint16 trialfcs;
    uint16 phyaddr = 0;
    uint8 hdlcaddr[4];
    uint8 pBuf[10];
    //int ret=0;
    uint16 shortname;
    MET_PRTL_DLMS *pDLMS_PRTL;

    if (!pdlms_comm || pdlms_comm->frzType>=4)
        return 0;
    
    //DLMS_Association_OBJ_LIST * pDlmsObjbjlist=&pdlms_comm->dlmsAssociation_OBJ_LIST;
    uint8 type=pdlms_comm->frzType;
    
    *RRFlag=0;
    phyaddr = ((met_addr[0])>>4)*10+(met_addr[0]&0x0f)+\
        (met_addr[1]&0x0f)*100 + ((met_addr[1])>>4)*1000;

    HdlcAddrTran(hdlcaddr, 0x01, phyaddr);

    while(i < (recvLen-8))
    {
        if(((recvBuf[i] == 0x7e)&&(recvBuf[i+4] == hdlcaddr[0])
            &&(recvBuf[i+5] == hdlcaddr[1])&&(recvBuf[i+6] == hdlcaddr[2])
            &&(recvBuf[i+7] == hdlcaddr[3]))
            )//||(recvLen == 0x1a))
        {
            pBuf[1] = recvBuf[i+1]&0x03; //后面三位是长度的高位，高4位是帧类型，高第5位是长短帧标志
            pBuf[0] = recvBuf[i+2];
            memcpy(&len, pBuf, 2);
            len &= 0x03ff; //获取长度

            if (len<10 || len>200)//发现过长度为0的情况
            {
                i++;
                continue;
            }
            
            trialfcs = pppfcs16(0xffff, &recvBuf[i+1], len-2);//数据帧校验
            trialfcs ^= 0xffff;                             /* complement */
            pBuf[0] = (trialfcs & 0x00ff);          /* least significant byte first */
            pBuf[1] = ((trialfcs >> 8) & 0x00ff);

            uint8 *pdataPos=NULL;//数据起始位置
            uint16 datalen=0;//数据区长度
            
            if((pBuf[0] == recvBuf[i+len-1])&&(pBuf[1] == recvBuf[i+len]))  //计算出得校验与发送来的校验一致
            {
                printf("DLMS_saveFrzData 回码:\n ");
                commfun_printstring(&recvBuf[i], len+2);
            
                if(recvBuf[i+8] == 0x97)//拒绝连接的帧
                {
                    printf("DLMS_saveFrzData 拒绝连接的帧.\n ");
                    return 6;
                }

                pdlms_comm->sendseq=(((recvBuf[i+8]&0xe0)>>5)-1)<<1;
                SEND_SEQ = pdlms_comm->sendseq;

                if ((recvBuf[i+8]&0x0f) == 0x01) //RR帧 ，       RR帧：7e a0 0a 21 00 02 2e 79 71 f9 f9 7e
                {
                    printf("DLMS_saveFrzData 回码为RR帧.\n ");
                    *RRFlag=1;
                    return 3;
                }

                if(recvBuf[i+1] == 0xa8)    //有后续数据
                {
                    pdlms_comm->recvseq=(((recvBuf[i+8]&0x0e)>>1))<<5;//-----------
                    RECV_SEQ = pdlms_comm->recvseq;
                }
                else
                {            
                    pdlms_comm->recvseq=(((recvBuf[i+8]&0x0e)>>1)+1)<<5;//-----------
                    RECV_SEQ = pdlms_comm->recvseq;
                }
                    
                    
                if((recvBuf[i+11] == 0xe6)&&(recvBuf[i+12] == 0xe7)
                    &&(recvBuf[i+13] == 0x00)&&(recvBuf[i+14] == 0x0C)
                    &&(recvBuf[i+15] == 0x01))//第一帧回码
                {
                    printf("DLMS_saveFrzData 第一帧回码.\n ");
                    if (recvBuf[i+16] != 0x00)//00 成功，01 0C表示选择的时间跨度内没有数据
                    {
                        //读取数据失败
                        return 4;//回失败标志
                    }

                //  pdlms_comm->RECV_SEQ=(((recvBuf[i+8]&0x0e)>>1)+1)<<5;//-----------
                    pdataPos=&recvBuf[i+17];
                    datalen=len-2-16;
                }
                else //RR帧召测回码,是I帧    
                {
                    pdataPos=&recvBuf[i+11];
                    datalen=len-2-10;
                }

                if (datalen)
                {
                    switch (pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId])
                    {
                    case DLMS_DLMS_CAPTURE_PERIOD_DAY:
                    case DLMS_DLMS_CAPTURE_PERIOD_MON:
                    case DLMS_DLMS_CAPTURE_PERIOD_CURVE1:
                    case DLMS_DLMS_CAPTURE_PERIOD_CURVE2:
                        {
                            //writelog_buf("/mnt/data/dlms_captureperiod.log",recvBuf,recvLen);
                    
                            k = i;
                            uint8 respComm[20]={0xe6,0xe7,0x00,0x0c,0x01,0x00};
                            while(k < (recvLen-3))
                            {
                                if(memcpy(&recvBuf[k],respComm,6)==0)
                                {
                                    if (recvBuf[k+6]==0x06)
                                        pdlms_comm->dlmsFrzCaptureParaAndData[type].period=*(uint32*)&recvBuf[k+7];
                                    break;
                                }
                                k++;
                            }
                        }
                        break;
                    
                    case DLMS_DLMS_CAPTURE_OBJECT_DAY:
                    case DLMS_DLMS_CAPTURE_OBJECT_MON:
                    case DLMS_DLMS_CAPTURE_OBJECT_CURVE1:
                    case DLMS_DLMS_CAPTURE_OBJECT_CURVE2:
                        {
                            k = i;
                            //writelog_buf("/mnt/data/dlms_captureobj.log",recvBuf,recvLen);
                    
                            uint8 itemNum=pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_itemNum;
                            //uint16 objI;
                            BOOL bFound;
                            
                            while(k < (recvLen-3))
                            {
                                if((recvBuf[k] == 0x02)&&(recvBuf[k+1] == 0x04)&&(recvBuf[k+2] == 0x12))
                                {
                                    bFound=FALSE;
                                    //帧格式   02 04 12 00 03 09 06 01 01 15 08 00 ff 0f 02 12 00 00
                                    pDLMS_PRTL = SD_MET_PRTL_DLMS_PRTL;
                            
                                    while(pDLMS_PRTL->oad.value != 0xffffffff)
                                    {
                                        if(memcmp(pDLMS_PRTL->oBis, &recvBuf[k+7], 6) == 0)
                                        {
                                            pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item[itemNum]= pDLMS_PRTL->ShortName;
                                            pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item[itemNum]+=(recvBuf[k+14]-1)*8;//指定属性号的地址（短名），也就是数据项VALUE的短名
                                            pdlms_comm->dlmsFrzCaptureParaAndData[type].frzoad[itemNum].value = pDLMS_PRTL->oad.value;
                                            if(pDLMS_PRTL->oad.value == 0x40000200)
                                            {
                                                pdlms_comm->dlmsFrzCaptureParaAndData[type].frzoad[itemNum].value = 0x20210200;
                                            }
                    
                                            bFound=TRUE;
                                            break;
                                        }
                                        pDLMS_PRTL++;
                                    }
                                    if (!bFound)
                                        pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item[itemNum]=0x0000;//为准确与数据匹配，在全局对象列表中没找到也要放一个短名
                                        TASK_FMT_TRACE( pipe, REC_LOG, "冻结对象 is %d,%d,%x,%x\n",type,itemNum,pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item[itemNum],pdlms_comm->dlmsFrzCaptureParaAndData[type].frzoad[itemNum].value);
                                    itemNum++;

                                    if (itemNum>=sizeof(pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item)/sizeof(pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_item[0]))
                                        break;
                                        
                                }
                                k++;
                            }
                            pdlms_comm->dlmsFrzCaptureParaAndData[type].SN_itemNum = itemNum;
                            
                        }
                        break;
                    
                    case DLMS_DLMS_CAPTURE_DATA_DAY:
                    case DLMS_DLMS_CAPTURE_DATA_MON:
                    case DLMS_DLMS_CAPTURE_DATA_CURVE1:
                    case DLMS_DLMS_CAPTURE_DATA_CURVE2:
                        {
                            if ((pdlms_comm->dlmsFrzCaptureParaAndData[type].dataLen+datalen)>sizeof(pdlms_comm->dlmsFrzCaptureParaAndData[type].dataBuf))
                                break;

                            memcpy(pdlms_comm->dlmsFrzCaptureParaAndData[type].dataBuf+pdlms_comm->dlmsFrzCaptureParaAndData[type].dataLen, pdataPos,datalen);
                            pdlms_comm->dlmsFrzCaptureParaAndData[type].dataLen+=datalen;
                            
                            TASK_FMT_TRACE( pipe, REC_LOG, "冻结数据\n");
                            TASK_BUF_TRACE( pipe, REC_LOG, pdataPos,datalen);
                            TASK_FMT_TRACE( pipe, REC_LOG, "总冻结数据\n");
                            TASK_BUF_TRACE( pipe, REC_LOG, pdlms_comm->dlmsFrzCaptureParaAndData[type].dataBuf,pdlms_comm->dlmsFrzCaptureParaAndData[type].dataLen);
                                

                        }
                        break;
                    
                    default:
                        break;
                    }
                }

                if(recvBuf[i+1] == 0xa8)    //有后续数据
                    return 2;
                else
                {
                	TASK_FMT_TRACE( pipe, REC_LOG, "1111Comm_Info_dlms->FrzFrmTypeId %d ,Comm_Info_dlms->FrzFrmTypeTotal is %d,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId] is %d\n",pdlms_comm->FrzFrmTypeId,pdlms_comm->FrzFrmTypeTotal,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId]);
                    if (pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId]==DLMS_DLMS_CAPTURE_DATA_CURVE1)
                    {
                        //曲线有两条，第一条召测结束后开始转到召测第二条
                        shortname=get_DLMS_Association_OBJ(pdlms_comm,g_frzObjObis[eCAPTURE_CURVE_2].obis);
                        if (shortname !=0xFFFF)
                        {
                            //有第二条曲线
                            pdlms_comm->frzType=eCAPTURE_CURVE_2;
                          
                            DLMS_Frz_Cmd_Init(pdlms_comm,pCbTask, pipe, RunTaskInfo);
                        }
                        else
                            pdlms_comm->FrzFrmTypeId++;//转到下一个召测命令类型
                    }
                    else
                        pdlms_comm->FrzFrmTypeId++;//转到下一个召测命令类型
                    TASK_FMT_TRACE( pipe, REC_LOG, "2222Comm_Info_dlms->FrzFrmTypeId %d ,Comm_Info_dlms->FrzFrmTypeTotal is %d,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId] is %d\n",pdlms_comm->FrzFrmTypeId,pdlms_comm->FrzFrmTypeTotal,pdlms_comm->FrzFrmType[pdlms_comm->FrzFrmTypeId]);
    
                    return 1;//返回1表示没有后续帧，本次交互完毕
                }
            }
        }
        else
        {
            i++;
        }
    }
    return 0;
}


