/*
***************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
    ��������:   mettype.h
    ��    �ܣ������Ͷ���
    �� �� �ˣ��ȱ�  
    ����ʱ�䣺2007-03-17
    
    �汾��ʷ:
            2007-03-17 ,v1.0 : New              
    
***************************************************************************
*/

#ifndef _METTYPE_H
#define _METTYPE_H

#include "dataType.h"


#if CH_AC485_CACHE_EN == 1
#define RELAY_MAX_LEN	2048
#else
#define RELAY_MAX_LEN	1200
#endif
// ����ģ����Ӧ�ò�֮���ͨѶ�ṹ

#define PLC_EVENT_DETECT		10	//�ز��������¼����

typedef struct _RELAY_IPC_PULS
{
	uint8 RelayIPCType;					//-�м��������ͣ�͸��ת����0x1X�����м����0x2X�����������0x3X����-
	uint8 RelayIPCTaskStat;				//-�м̽����0-ʧ�� 1-�ɹ� 0xff-��ʱ��-
	uint8 RelayIPCPort;					//-�˿ںţ��ֽڣ�-
	uint8 RelayIPCMode;					//-�˿ڲ�������Ϣ���ֽڣ�-
	uint32 RelayIPCOverTime;			//-�м̳�ʱʱ�䣨�ֽڣ���698��λms-
	uint8 RelayIPCPrtl;					//-��Լ����(0-97��Լ��-07��Լ)-
    uint8 RelayIPCFrameSeq;				//-֡��ţ���Ҫ�Ǹ���������֡ʱʹ��-
	uint16 RelayIPCLen;					//-���ȣ��ֽ�698��Ϊ�����ֽڣ�
	uint8 RelayIPCData[RELAY_MAX_LEN];	//-�м�����-
}RELAY_IPC_PULS;

/*
����Ŀǰ���м�����ʹ�ñȽϻ��ң�Ϊ�˽��ʹ�����ҳ��򣬹ʶ��м�����������
Ŀǰ˼·�ǽ��м������Ϊ��͸��ת���������м������������������֣���Ӧ����ÿ���������������С��
1��͸��ת����0x10��
-	���ڡ�͸��ת�����������֡��Ӧ�ò㸺�𣬳���ģ�飨cbtask��PLC��AC��ֻ����ת������
-	����ģ��ԭ�����س���Ӧ���ģ�����Ӧ���Ľ��н���
2���м����0x20��
-	���������ɳ���ģ�鸺����֡�����룬�������������;����ظ�Ӧ�ò�ı�������
3���������0x30��
-	���ݾ�������������ܹ���
*/
#define RELAY_TRAN					0x10			//͸��ת��
#define RELAY_REPEAT				0x20			//�м�����
#define RELAY_OTHER					0x30			//��������
#define RELAY_EVENT                 0x40            //�¼������������

#define RELAY_TRAN_F1				0x11			//����ת��F1
#define RELAY_TRAN_BATCH_AUTH		0x12			//������֤����
#define RELAY_TRAN_BATCH_COMP		0x13			//������ʱ����
//#define RELAY_TRAN_BROAD_TIME		0x14			//�㲥��ʱ����

#define RELAY_REPEAT_F9				0x21			//����ת��F9
#define RELAY_REPEAT_DATA1_POINT	0x22			//һ�����ݵ㳭
#define RELAY_REPEAT_BROAD_TIME		0x23			//�㲥��ʱ����

#define RELAY_OTHER_CB_START		0x31			//������������ģ�飨cbtask��PLC���յ����ĺ�ظ�Ӧ���ģ���������������
#define RELAY_OTHER_PLC_DELAY		0x32			//����ͨѶ��ʱ�����������PLCͨ���£�������ʱ����ʱʹ�ã�������������ͨ����ʱ����ͨ�����ݡ�����
#define RELAY_OTHER_AC_3761			0x33			//�������������ģ����appmainͨѶʹ�ã�����͸��ת��ͨ�����ɱ������շ���376.1����
#define RELAY_OTHER_GUI				0x34			//�����������ʾģ����appmainͨѶʹ�ã����ھɳ�������ʾ��appmain��������û������IPC�ṹ��Ĺ���
#if TMNHALT_METHOD ==1
#define RELAY_HALT              	0x35			//�����¼�м�
#endif

#if EVENT_2014_EN == 1
#define RELAY_REPEAT_F10            0x24            //����������ݸ�ʽ��F9�ĸ�ʽ��һ�µģ������ñ�ʶ��Ŀ����Ϊ�˷���appmain���룬appmainͨ���ñ�ʶ�Ϳ���֪���Ǵ�͸F9����F10��

#define RELAY_AUTOEVENT             0x41            //�¼������ϱ�������ģ����յ�·��ģ��������ϱ�֡��ͨ���ñ�ʶ֪ͨappmain��
#define RELAY_FOLLOW_SEQ            0x42            //��������֡������ģ����յ�����֡�����ͨ���ñ�ʶ֪ͨappmain��
#define RELAY_READ_EVENTCOUNT       0x43            //appmain֪ͨ����ģ�鳭��ָ�����¼���������������ģ�鷢�𳭶���������Ļ�����н��������������ֵ䡣
#define RELAY_READ_EVENTRECORD      0x44            //appmain֪ͨ����ģ�鳭��ָ���¼���1~N�ļ�¼������������ݸ�ʽ��F9��F10�ĸ�ʽ��һ�µ�
#define RELAY_READ_AUTOEVENT        0x45            //appmain�����������ܱ�״̬��0x04001501������ģ�鲻������ֱ�ӷ���Ӧ���ģ����ݸ�ʽ��F9��F10�ĸ�ʽ��һ�µģ�
#define RELAY_REST_METERSTATUS      0x46            //��λ���ܱ������ϱ�״̬�֣���Ҫ�����ڵȴ�ȷ��֡���ٳ���һ��״̬��
#define RELAY_READ_FOLLOW_FRAME     0x47            //����������֡������ģ����յ�����֡�����ͨ���ñ�ʶ֪ͨappmain��
#if JIANG_SU_FUN == 1
#define RELAY_READ_DI_SET           0x48            //appmain֪ͨ����ģ�鳭���ɼ������õ�״̬�ֶ�Ӧ�¼����ݱ�ʶ������ģ�鷢�𳭶���������Ļ��뱨��appmain��
#endif
#endif

#if LIAONING_TICK_EN == 1
#define RELAY_RDMETER_CLOCK          0x51           //�������ʱ�ӣ��������ʽ������һ�����ݵ�
#define RELAY_SETMETER_CLOCK         0x52           //���õ��ʱ��
#endif

#if ZHEJIANG_FUN == 1
#define RELAY_TMN_RESET              0x61           //�ն�Ӳ����λ
#endif

#if STD_698PRO_BETA == 1
#define RELAY_GET_NORMALLIST		 0x71			//��ȡ���ɸ������������ɸ���������
#define RELAY_GET_RECORD		 	 0x72			//��ȡһ����������һ����¼�Ͷ�������
#define RELAY_SET_NORMALLIST		 0x73			//�������ɸ������������ɸ���������
#define RELAY_SET_GETLIST		 	 0x74			//���ú��ȡ���ɸ������������ɸ���������
#define RELAY_ACTION_NORMALLIST		 0x75			//�������ɸ������������ɸ����󷽷�
#define RELAY_ACTION_GETLIST	 	 0x76			//�������ȡ���ɸ������������ɸ����󷽷�������
#define RELAY_TRANS_CMD		 	     0x77			//͸��ת��
#define RELAY_TRANS_F209		 	 0x78			//F209͸��ת��
#define RELAY_TRANS_F208		 	 0x79			//����F208͸��ת��
#endif

//�ⲿ���������
/*-��ͬ��ͨѶ��Լ�޸ĸú궨�弴�ɣ�����Ϊ220�Ժ�ı�ʾ�˵���û�����ֱ�-*/
#if STD_698PRO_BETA == 1
#define OOP_PRTL     	235      //2��������Լ���
#endif
#if JIANG_SU_FUN == 1
#define NO_MET_APP      0       //-δ�ӵ��
#define DLT645_APP      1    	//-������-
#define AC_SAMPINGMET   2    	//-���ɵ��-
#define JSBLACK_APP     2    	//-���ս���-
#define FJ_APP          3    	//-������Լ-
#define LGR_APP_D3      4    	//-ZFB������d�ͱ���������-       
#define LGR_APP         5    	//-ZFB������d�ͱ���������-
#define LGR_D_MET_B3    6    	//-������B�ͱ���������-
#define LGR_D_MET       7    	//-������B�ͱ���������-    
#define ABB_APP         8    	//-ABB��(����)-       
#define ABB_APP_S       9    	//-ABB(Բ��)-
#define EDMI_APP		10	 	//-����-
#define DLT645_FFEE_APP 11 //�ն�����Ӧ�Ƿ�֧�����ݿ��ȡ���ķ��ʣ�˳��Ϊ�ܡ��塢ƽ���ȡ��⣻
#define DLT645_TFEE_APP 12 //���֧�����ݿ��ȡ���������ʣ�
#define WS20_APP        13   	//-��ʤ���-    
#define DSSD25_APP      14   	//-�㽭��Լ-
#define HETONG_OLD      15   	//-��ͨI��(�ɹ�Լ) -
#define HETONG_NEW      16   	//-��ͨII��(�¹�Լ)-
#define HND_APP         17   	//-������(�������Լ���Լ)-
#define JIANGBEI        18   	//-�������-
#define WXABB_APP      	19   	//-�¹�ABB(ELSTER)��ԭ�궨��ΪELSTER_ABB-
#define LG9600_APP      20   	//-�����°�LDS_D(9600����)-
#define JSSIMULATE_APP	21	 	//-����ģ����-
#define SIMPLE_DLT_APP	22	 	//-����������-*/ 
#define DLMS_APP        23      //-DLMS��Լ-
#define CQINTELL_APP	24	 	//-�������ܱ�-
#define LGR_B_MET		25   	//-������������-      
#define ZMC_APP         27      //-lds c��-
#define ELSTER_APP      28		//-ELSTER��-
#define DL2007_PLC_EVT  29      //13�淶���ܱ�֧���¼��ϱ�
#define DL2007_PRTL     30      //2007��Լ��� 
#define DL2007_PLC      31      //2007��Լ���(�ز�)

#if OS_SMARTBREAKER_EN == 1
#define SMARTBREAKER_PRTL     33      //���ܶ�·����Լ
#endif

#define EMAIL_APP   	64	 	//-email��-       
#define FQD_APP         69      //-���Ե��- 
#define DLT645_TLLD     153  	//-���ܷ��Ե��-   
#define ZZDB_APP        154     //-֣�ݸ���,������(��ʱ�Զ���)-
#define EDMI_APP_Y		221	 	//-����MK3-   
#define LGR_U_MET       222   	//-LDS U�ͱ�-
#define KELU_APP        223		//-��½��Լ- 
#define ANHUI_MET       224    	//-���չ����-
#define DLT645_TJ       225    	//-�����Լ-
#define WEISHENG_645    226    	//-��ʤ645��Լ-
#define DONGFANG_645    227    	//-����645��Լ-
#define HUALI_645       228    	//-����645��Լ- 
#define WXABB_APP_4     229   	//-ELSTER,��������- 
#define ELSTER_APP_D3   230     //-ELSTER1700����������-
#define JLB_APP         231   	//-���ֱ�-
#define DLT645CQJY_APP  234     //-������׹����-  
#elif HUBEI_FUN == 1

// �����涨
// 0//��Ч��1//DL/T645-1997��Լ��2//�����ɼ���3//Ԥ����4//������D��
// 5//������B��6//���棨�๦�ܣ���7//�����8//ABB����9//ABBԲ��
// 10//Ԥ����11//Ԥ����12//��ʤ4.0��13//����MK6��14//����MK3��
// 15//���棨���׶๦�ܣ���16//������U��17//���棨��ͨ����ʽ���ܱ���
// 18//���棨���񸴷��ʵ��ܱ���30// DL/T645-2007��

#define NO_MET_APP      0           //-δ�ӵ��
#define DLT645_APP      1    		//-������-
#define AC_SAMPINGMET   2    		//-���ɵ��-
#define LGR_APP         4    		//-ZFB������d�ͱ���������-
#define LGR_D_MET       5    		//-������B�ͱ���������-    
#define HB_STD_APP		6			//���棨�๦�ܣ�
#define HND_APP         7   		//-������(�������Լ���Լ)-
#define ABB_APP         8    		//-ABB��(����)-
#define ABB_APP_S       9    		//-ABB(Բ��)-
#define WS20_APP        12   		//-��ʤ���-   
#define EDMI_APP		13	 		//-����-
#define EDMI_APP_Y		14	 		//-����MK3-
#define HB_JY_APP		15			//���棨���׶๦�ܣ�
#define LGR_D_MET_B3    16    		//-������B�ͱ���������-    
#define HB_PT_APP		17			//���棨��ͨ����ʽ���ܱ�
#define HB_FFL_APP		18			//���棨���񸴷��ʵ��ܱ�
//#define DL2007_PRTL19	19   		//-DL/T645-2007-
#define DLT645_PRTL20	20			//����ʵ�ַ��Ե繦�ܵ�����ת��ģ�顣����Ҫ�����Լ��20
#define HB_MUL_STD_APP	21   		//����ʡ�๦�ܱ��Լ
#define HB_JY_WP_APP	22   		//����ʡ���ױ��Լ(�з������)
#define HB_JY_NP_APP	23   		//����ʡ���ױ��Լ(�޷������)
#define HB_JY_STD_APP	24   		//������ױ��Լ���ܵ�����
#define DL2007_PRTL		30    		//-DL/T645-2007-
#define DL2007_PLC		31			//	
#define FJ_APP          12+20    	//-������Լ-
#define DSSD25_APP      14+20    	//-�㽭��Լ-
#define HETONG_OLD      15+20    	//-��ͨI��(�ɹ�Լ) -
#define HETONG_NEW      16+20    	//-��ͨII��(�¹�Լ)-
#define LGR_APP_D3      17+20     	//-ZFB������d�ͱ���������-    
#define JIANGBEI        18+20    	//-�������-
#define WXABB_APP      	19+20    	//-�¹�ABB(ELSTER)��ԭ�궨��ΪELSTER_ABB-
#define JLB_APP         20+20    	//-���ֱ�-
#define JSSIMULATE_APP	21+20 	 	//-����ģ����-
#define SIMPLE_DLT_APP	22+20 	 	//-����������-*/ 
#define DLMS_APP        23+20       //-DLMS��Լ-
#define CQINTELL_APP	24+20 	 	//-�������ܱ�-
#define LGR_B_MET		25+20    	//-������������-      
#define ELSTER_APP      28+20 		//-ELSTER��-
#define ZMC_APP         29+20       //-lds c��-
#define EMAIL_APP   	64+20 	 	//-email��-       
#define FQD_APP         69+20       //-���Ե��- 
#define DLT645_TLLD     153  	//-���ܷ��Ե��-   
#define ZZDB_APP        154     //-֣�ݸ���,������(��ʱ�Զ���)-
#define LGR_U_MET       222   	//-LDS U�ͱ�-
#define KELU_APP        223		//-��½��Լ- 
#define ANHUI_MET       224    	//-���չ����-
#define DLT645_TJ       225    	//-�����Լ-
#define WEISHENG_645    226    	//-��ʤ645��Լ-
#define DONGFANG_645    227    	//-����645��Լ-
#define HUALI_645       228    	//-����645��Լ- 
#define WXABB_APP_4     229   	//-ELSTER,��������- 
#define ELSTER_APP_D3   230     //-ELSTER1700����������-
#define LG9600_APP      231   	//-�����°�LDS_D(9600����)-
#define DLT645CQJY_APP  234     //-������׹����-
#else
#define NO_MET_APP      0       //-δ�ӵ��
#define DLT645_APP      1    	//-������-
#define AC_SAMPINGMET   2    	//-���ɵ��-
#define JSBLACK_APP     2    	//-���ս���-
#define FJ_APP          3    	//-������Լ-
#define LGR_APP_D3      4    	//-ZFB������d�ͱ���������-       
#define LGR_APP         5    	//-ZFB������d�ͱ���������-
#define LGR_D_MET_B3    6    	//-������B�ͱ���������-
#define LGR_D_MET       7    	//-������B�ͱ���������-    
#define ABB_APP         8    	//-ABB��(����)-       
#define ABB_APP_S       9    	//-ABB(Բ��)-
#define EDMI_APP		10	 	//-����-
#define WS20_APP        12   	//-��ʤ���-    
#define DSSD25_APP      14   	//-�㽭��Լ-
#define HETONG_OLD      15   	//-��ͨI��(�ɹ�Լ) -
#define HETONG_NEW      16   	//-��ͨII��(�¹�Լ)-
#define HND_APP         17   	//-������(�������Լ���Լ)-
#define JIANGBEI        18   	//-�������-
#define WXABB_APP      	19   	//-�¹�ABB(ELSTER)��ԭ�궨��ΪELSTER_ABB-
#define LG9600_APP      20   	//-�����°�LDS_D(9600����)-
#define JSSIMULATE_APP	21	 	//-����ģ����-
#define SIMPLE_DLT_APP	22	 	//-����������-*/ 
#define DLMS_APP        23      //-DLMS��Լ-
#define CQINTELL_APP	24	 	//-�������ܱ�-
#define LGR_B_MET		25   	//-������������-      
#define ELSTER_APP      28		//-ELSTER��-
#define ZMC_APP         29      //-lds c��-
#define DL2007_PRTL     30      //2007��Լ��� 
#define DL2007_PLC      31      //2007��Լ���(�ز�)
#define EMAIL_APP   	64	 	//-email��-       
#define FQD_APP         69      //-���Ե��- 
#define DLT645_TLLD     153  	//-���ܷ��Ե��-   
#define ZZDB_APP        154     //-֣�ݸ���,������(��ʱ�Զ���)-
#define EDMI_APP_Y		221	 	//-����MK3-   
#define LGR_U_MET       222   	//-LDS U�ͱ�-
#define KELU_APP        223		//-��½��Լ- 
#define ANHUI_MET       224    	//-���չ����-
#define DLT645_TJ       225    	//-�����Լ-
#define WEISHENG_645    226    	//-��ʤ645��Լ-
#define DONGFANG_645    227    	//-����645��Լ-
#define HUALI_645       228    	//-����645��Լ- 
#define WXABB_APP_4     229   	//-ELSTER,��������- 
#define ELSTER_APP_D3   230     //-ELSTER1700����������-
#define JLB_APP         231   	//-���ֱ�-
#define DLT645CQJY_APP  234     //-������׹����-    
#endif
////////////////////////////////////

#define MAX_MET_NUM       	40
#define DLT645              1 //�����
#define LGR                 2 //LGR��
#define WS20                3 //��ʤ��
#define ABB                 4 //ABB��
#define EDMI                5 //�����
#define JSBLACK             6 //���ս��ɱ�
#define HND                 7 //�������
#define HT                  8 //��ͨ��
#define NZJC		        9 //���ý��ɱ�
#define WXABB              10 //����ABB��
#define DSSD25             11 //�㽭��Լ��
#define DLT645TJ           12 //����Լ
#define FQD                13 //���Ե��
#define DLT645JY           14 //���׹���
#define JLB                15 //���ֹ�Լ��
#define JSSIMULATE         16 //����ģ���
#define DLT645_2007		   17	//-DL/T645-2007-
#define DLT645_0           18 //���ܷ��Ե��
#define FJ                 19 //�������
#define CBH                20 //�����
#define DLMS               21 //DLMS
#define ZMC                22 //������C��
#define	DIRECTSIM		   23 //ֱ��ģ����
#define HR				   24 //г��
#define LG9600			   25 //9600bps������D��
#define ELSTER             26 //����ELSTER��
#define EMAIL              27 //email��
#define DLT645CQJY         28 //������׹���
#define ZZDB               29 //֣�ݹ�Լ��
#define CQINTELL           30 //����� 
#define ABB_S              31 //ABBԲ��
#define FJ_KN              32 //�������ݿ��ܱ�
#define LGR_B              33 //������B����������/��������
#define	HT_NEW		   	   34 //��ͨ��(�¹�Լ)
#define DL2007_PLC_MET     35 //2007��Լ��� 
#define ACTARIS            36 //����ACTARIS
#if HUBEI_FUN == 1
#define HB_STD			   37	//���棨�๦�ܣ�
#define HB_JY			   38	//���棨���׶๦�ܣ�
#define HB_PT			   39	//���棨��ͨ����ʽ���ܱ�
#define HB_FFL			   40	//���棨���񸴷��ʵ��ܱ�
#define FQDMK			   41	//���Ե�ģ��:����ʵ�ַ��Ե繦�ܵ�����ת��ģ�顣ע:����Ҫ���Ӧ DLT645_PRTL20
#define HB_MUL_STD         43   //����ʡ�๦�ܱ��Լ
#define HB_JY_WP           44   //����ʡ���ױ��Լ(�з������)
#define HB_JY_NP           45   //����ʡ���ױ��Լ(�޷������)
#endif

#define DLT645DC           42 //���������

#if JIANG_SU_FUN == 1
#define DLT645_FFEE        43 //�ն�����Ӧ�Ƿ�֧�����ݿ��ȡ���ķ��ʣ�˳��Ϊ�ܡ��塢ƽ���ȡ��⣻
#define DLT645_TFEE        44 //���֧�����ݿ��ȡ���������ʣ�
#define DLT645_2007_EVT    45   

#if  OS_SMARTBREAKER_EN == 1
#define SMARTBREAKER        46      //���ܶ�·����Լ
#endif

#endif

#if STD_698PRO_BETA == 1
#define DLTOOP				47 //��������Լ���ܱ�
#define DLT188				48 //��������Լ�����188��Լ��
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _METTYPE_CONTRAST
{
    uint8   nMettypeID;     // �ⲿ���������
    char*   czMettype;      // �������ַ�������Ҫ̫����������5��������
}METTYPE_CONTRAST;

extern METTYPE_CONTRAST g_MettypeContrast[];
extern uint32 g_nMettypeCnt;
uint8 MtTyp_To_IdMtTyp(uint8 MtTyp);
char* get_met_type_str(uint32 type);

#ifdef __cplusplus
}
#endif
#endif
