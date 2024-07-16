/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�AppCommon.h
* �汾�ţ�1.0
* ���ܣ����¼����������õ������ݽṹ�����Ⱥ�����������
* ԭʼ�����˼�ʱ�䣺
		���� 2006��12��25��
* �޸���Ա��ʱ�估�޸�����:
* 		�޸����ݣ����伸������֡�ṹ�Ķ���
* 		֣���� 2006��1��19��
* ������
******************************************************************-*/
#ifndef _APPCOMMON_H
#define _APPCOMMON_H


#define DPRINT_PRTL_OOP(fmt...) \
    ({ if(getenv("DEBUG_OOP_PRTL") != NULL)  printf("OOPPRTL: "fmt);})

/*******************************************************************************
* �����Ĳ��֣��ⲿ�ֶ���һ�㲻��Ҫ���ݵ�����Ӳ�����ı�
*******************************************************************************/
//--�����������ǿ��ת��������--
#define chartoint16(p) 			((*((p) + 1) << 8) + *(p))
#define chartoint32(p) 			((*((p) + 3) << 24) + (*((p) + 2) << 16) + (*((p) + 1) << 8) + *(p))
#define chartoint64(p) 			(((int64)(*((p) + 7)) << 56) +((int64)(*((p) + 6)) << 48) +((int64)(*((p) + 5)) << 40) +((int64)(*((p) + 4)) << 32) +(*((p) + 3) << 24) + (*((p) + 2) << 16) + (*((p) + 1) << 8) + *(p))


#define CBTIME_DATA_FORMAT 		1 				//�йس���ʱ������ݣ�������Чֵʱ����0xee����0x0��1��0xee��0��0x0
#define PasssWordCheck_EN 		1				//������ģ��ʹ�ܣ�1��ⱨ��������ȷ�ԣ�0����������ֱ��ͨ��
#define AB_TYPE_SWITCH  		1				//1a�ʹ��㣬0b�ʹ��㣻����a��b�ʹ����ң��״̬���л�����ص�idΪSWITCH_XCH_STATU

//��ʾ��ȡ��¼ʱ��¼����ʼָ�����״ζ�ȡ�ɹ��ļ�¼�����
//����ָ�����ڳɹ���ȡ��¼��������������ȡ����¼ʧ��ʱ�ļ�¼��ǰһ����¼
#define SUCCRECFLAG 			1

//�����ṩ������CheckDataValidʹ��
#define CHAR_FORMAT_0   		0x0
#define CHAR_FORMAT_EE  		0xee
#define CHAR_FORMAT_FF  		0xff

#define TEN_THOUSAND			10000			//������ص�ֵ�Ŵ�10000����洢�������ֵ���
#define POWER_STOP_TIME_LMT 	300 		    //�ϵ��¼��ж��ϵ������Ҫ�ĳ�ʱʱ��
/******************************************************************************
* �������ݵ����ݵĶ�ȡʱ�����ʼ��ѡ��
* ʾ��1:
* 1Сʱһ����ʱ����ʼʱ��13:30,
* ȡ0��ʾ13:00~14:00;
* ȡ1��ʾ13:30~14:30--
* ʾ��2:
* 1Сʱһ����ʱ����ʼʱ��13:15,
* ȡ0��ʾ13:00~14:00;
* ȡ1��ʾ13:15~14:15--
*******************************************************************************/
#define GET_96_DATA_TIMESTD  	0 				//�ж�96��洢����ȡ����ѡ��

#define FIRST_DATA_TYPE   		1  				//��Ҫ�¼�
#define SECOND_DATA_TYPE  		2  				//һ���¼�

#define NEW_RECORD_TYPE   		0x00 			//�¼�¼
#define OLD_RECORD_TYPE   		0x01 			//ԭ�еļ�¼
#define MUST_SEND_TYPE    		0x00 			//��Ҫ�ж��Ƿ��ϱ�
#define NONE_SEND_TYPE    		0x02 			//����Ҫ�ж��Ƿ��ϱ�

//��¼��
#define	ERC_NO_01	    		1 				//��ʼ�����汾�����¼
#define	ERC_NO_02	    		2 				//�ն˲�����ʧ��¼
#define	ERC_NO_03	    		3 				//����
#define	ERC_NO_04	    		4 				//ң�ű�λ��¼
#define	ERC_NO_05	    		5 				//ң����բ��¼
#define	ERC_NO_06	    		6 				//������բ��¼
#define	ERC_NO_07	    		7 				//�����բ��¼
#define	ERC_NO_08	    		8 				//���ܱ���������¼
#define	ERC_NO_09	    		9 				//������·�쳣��¼
#define	ERC_NO_10	    		10				//��ѹ��·�쳣��¼
#define	ERC_NO_11	    		11				//�����쳣��¼
#define	ERC_NO_12	    		12				//��ʱ���쳣��¼
#define	ERC_NO_13	    		13				//���ܱ���ϼ�¼
#define	ERC_NO_14	    		14				//�ն�ͣ/�ϵ��¼���¼
#define	ERC_NO_15	    		15				//г��Խ�޼�¼
#define	ERC_NO_16	    		16				//����
#define	ERC_NO_17	    		17				//��ѹ������ƽ��
#define	ERC_NO_18	    		18				//����
#define	ERC_NO_19	    		19				//��ѹ������ƽ��
#define	ERC_NO_20	    		20				//��������¼
#define	ERC_NO_21	    		21				//�ն˹��ϼ�¼(485����)
#define	ERC_NO_22	    		22				//�й��ܵ������Խ��
#define	ERC_NO_23	    		23				//��ظ澯�¼�
#define	ERC_NO_24	    		24				//��ѹԽ�޼�¼
#define	ERC_NO_25	    		25				//����Խ�޸澯
#define	ERC_NO_26	   	 		26				//���ڹ���Խ�޸澯
#define	ERC_NO_27	    		27				//���ʾ���½��澯
#define	ERC_NO_28	    		28				//������������澯
#define	ERC_NO_29	    		29				//�����߸澯
#define	ERC_NO_30	    		30				//���ͣ�߸澯
#define	ERC_NO_31				31				//485����ʧ���¼���¼

//698�����¼�
#define	ERC_NO_32				32				//-�ն�����վͨ�������������¼���¼
#define	ERC_NO_33				33				//-���ܱ�����״̬�ֱ�λ�¼���¼
#define	ERC_NO_34				34				//-CT�쳣�¼���¼
#define	ERC_NO_35				35				//-����δ֪����¼���¼

#define ERC_NO_36				36				//���������·���ؽ���״̬����λ��¼
#define ERC_NO_37				37				//���ܱ�����¼���¼
#define ERC_NO_38				38				//���ܱ���ť���¼���¼
#define ERC_NO_39				39				//����ʧ���¼���¼
#define ERC_NO_40				40				//�ų��쳣�¼�
#define ERC_NO_41				41				//�ն˶�ʱ�¼�
#define ERC_NO_42				42				//ģ�����¼�
#if LIAONING_FUN == 1 || ANHUI_FUN == 1
#define ERC_NO_51				51				//�ն˶Ե��Уʱ�¼�
#endif
#if TMNHALT_METHOD ==1
#define ERC_NO_45				45				//���ʧЧ�¼�
#endif

/*-��¼���ȣ���������¼�źͳ���ֵ����-*/
#define	ERC_NO_01_LEN			14
#define	ERC_NO_02_LEN			6
#define	ERC_NO_03_LEN			0
#define	ERC_NO_04_LEN			7
#define	ERC_NO_05_LEN			10
#define	ERC_NO_06_LEN			14
#define	ERC_NO_07_LEN			16
#define	ERC_NO_08_LEN			8
#define	ERC_NO_09_LEN			28
#define	ERC_NO_10_LEN			28
#define	ERC_NO_11_LEN			24
#define	ERC_NO_12_LEN			7
#define	ERC_NO_13_LEN			8
#if TMNHALT_METHOD ==1
#define	ERC_NO_14_LEN			11
#else
#define	ERC_NO_14_LEN			10
#endif

#define	ERC_NO_15_LEN			49
#if ANOLOGY_EN == 1
#define	ERC_NO_16_LEN			9
#else
#define	ERC_NO_16_LEN			0
#endif
#define	ERC_NO_17_LEN   		27
#define	ERC_NO_18_LEN   		16
#define	ERC_NO_19_LEN   		31
#define	ERC_NO_20_LEN   		22
#define	ERC_NO_21_LEN  		 	6
#define	ERC_NO_22_LEN  		 	21
#define	ERC_NO_23_LEN   		16
#define	ERC_NO_24_LEN   		14
#define	ERC_NO_25_LEN   		17
#define	ERC_NO_26_LEN   		14
#define	ERC_NO_27_LEN   		17
#define	ERC_NO_28_LEN   		18
#define	ERC_NO_29_LEN   		18
#define	ERC_NO_30_LEN   		13
#define	ERC_NO_31_LEN			21
#define	ERC_NO_32_LEN			13
#define	ERC_NO_33_LEN			35
#define	ERC_NO_34_LEN			8
#define	ERC_NO_35_LEN			0
#define ERC_NO_36_LEN			7
#define ERC_NO_37_LEN 		70
#define ERC_NO_38_LEN 		70
#define ERC_NO_39_LEN 		21
#define ERC_NO_40_LEN 		15
#define ERC_NO_41_LEN		14
#if TMNHALT_METHOD ==1
#define ERC_NO_42_LEN       0
#define ERC_NO_43_LEN       0
#define ERC_NO_44_LEN       0
#define ERC_NO_45_LEN       5				//���ʧЧ�¼�
#define ERC_NO_46_LEN       0
#define ERC_NO_47_LEN       0
#define ERC_NO_48_LEN       0
#define ERC_NO_49_LEN       0
#define ERC_NO_50_LEN       0
#define ERC_NO_51_LEN       0
#define ERC_NO_52_LEN       0
#define ERC_NO_53_LEN       0
#define ERC_NO_54_LEN       0
#define ERC_NO_55_LEN       0
#define ERC_NO_56_LEN       0
#define ERC_NO_57_LEN       0
#define ERC_NO_58_LEN       0
#define ERC_NO_59_LEN       0
#define ERC_NO_60_LEN       0
#define ERC_NO_61_LEN       0
#define ERC_NO_62_LEN       0
#define ERC_NO_63_LEN       0
#if JIANGSU_JZQ_FUN == 1
#define ERC_NO_64_LEN       93
#else
#define ERC_NO_64_LEN       0
#endif
#endif

#define MAX_EVENT_NUM  			15  			//�¼���¼�ж����¼���������Դ�ŵ���Ϣ����
#define MAX_DIFF_NUMBER 		4  				//�Խ���¼����֧�ֵĲ�����

//�����澯����
#define YXHZ_PHONE				0x01			//�����բ
#define YKTZ_PHONE				0x02			//ң����բ
#define DLKYW_PHONE				0x03			//���������꣬���ٹ���
#define DLYWYTZ_PHONE   		0x04			//�������꣬����բ
#define YCFHQXD_PHONE			0x05			//�ѳ����ɣ����޵�
#define GKTZ_PHONE				0x06			//������բ
#define YDKTR_PHONE				0x07			//�µ����Ͷ��
#define YDKJC_PHONE				0x08			//�µ���ѽ��
#define GDKTR_PHONE				0x09			//�������Ͷ��
#define GDKJC_PHONE				0x0a			//������ѽ��
#define XFKTR_PHONE				0x0b			//��ǰ�����¸�����Ͷ��
#define XFKJC_PHONE				0x0c			//��ǰ�����¸����ѽ��
#define BTKTR_PHONE				0x0d			//��ͣ��Ͷ��
#define BTKJC_PHONE				0x0e			//��ͣ�ؽ��
#define CXKTR_PHONE				0x0f			//���ݿ���Ͷ��
#define CXKJC_PHONE				0x10			//���ݿ��ѽ��
#define SDKTR_PHONE				0x11			//ʱ�ο���Ͷ��
#define SDKJC_PHONE				0x12			//ʱ�ο��ѽ��
#define YXCSYGG_PHONE			0x13			//���в����Ѹ��ģ���ע��鿴
#define ZXXX_PHONE				0x14			//������Ϣ����鿴
#define DUDUDU_PHONE			0x15			//���
#define DEFAULT_PHONE			0x18			//������
#define STOPVOICE				0x0				//ֹͣ����
#define CFKYI_PHONE    		 	0x19			//���û���ʱ������

//-��־�ļ��궨��-
#define WRITE_LOG_EN 			1				//1��д��־��0����д��־
#define FILE_MAX_NUM      		(1024*400)  	//��־��Ϣ��󳤶�  20K
//-��־�ļ������·��-
#define EVENT_LOG_FILE			"/mnt/log/libEvent.log"
#define PRTL_04_LOG_FILE		"/mnt/log/libptrl.log"
#define CONTROL_LOG_FILE		"/mnt/log/libControl.log"
#define AUTOTASK_LOG_FILE    	"/mnt/log/libAutoTask.log"
#define EN_REMAIN_APPMAIN_LOG_FILE	"/mnt/log/enremainappmain.log"

#define SECURITY_LOG_FILE		"/mnt/log/security.log"         // ��ȫ���ļӽ�����־

#if DELIVER_LOG_EN == 1
#define EVENT_SETGET_LOG    	"/mnt/log/event_setget.log"
#define EVENT_DATA_LOG    		"/mnt/log/event_data.log"
#define CONTROL_DATA_LOG    	"/mnt/log/control_data.log"
#endif
#define JSBLACK_APP     		2   			//���ս���
#define AC_SAMPINGMET   		2   			//���ɵ��
#define PLC_PRTL				31				//�ز��ӿ�Э��
#define MTR_Invalid       		0   			//�ò�����û������
#define NULLMET					0x00			//��ͨ��δ�ӵ��
#define Non485Met       		255   			//�ɵ�·������·������ɵĻ���ͼ����㣬��ѹ����һ������ֱ����/��-

//��·�ڼ��ʹ�õĺ궨��
#define GPRS_TRY_CNT			2
#define GPRS_LOGIN_FLAG			0x11
#define GPRS_HEART_FLAG			0xaa

/*****************-ͨ��������ص��ӹ���ʹ��-**************/

#define Heart_Proj_EN			0			//��������ʹ��
#define Heart_ZJ_GB				0			//�㽭,��������ѡ��,0Ϊ�㽭��Լ,1Ϊ�����Լ

#define LOGIN_INTERVAL 			30			//���Ե�½�ļ��ʱ�䣬10~20s
#if SHANGHAI_FUN == 1
#define HEART_INTERVAL 			300			//�������ʱ�䣬s
#elif SHANXI_FUN == 1
#define HEART_INTERVAL 			600			//�������ʱ�䣬s
#else
#define HEART_INTERVAL 			180			//�������ʱ�䣬s
#endif
#define RESET_DELAY_TIME 		20			//��λ����ӽ��������ִ�е���ʱ

/******************************************************************************
*	ͨ�������շ����ݵ���󳤶�
******************************************************************************/
#if CH_AC485_CACHE_EN == 1
#define SFP_485_3761_LEN		2048
#else
#define SFP_485_3761_LEN		1024
#endif
#if HUNAN_FUN == 1
#define SFP_MAX_LEN				2048	 //�����������⴦��(�в��ն�֤�鱨�ĳ�)
#else
#define SFP_MAX_LEN				300			//�������ݲ�����255
#endif
#define SFP_SMS_MAX_LEN       	200			//���Ŷ�֡���֡����
#define SFP_GPRS_GSM_MAX_LEN	1400		//GPRS_GSM���ݲ�����800
#if TD_SCDMA == 1
//�������������Ǹ�CDMAʹ�õĳ��ȱ�־����û���õ�
//���ڸ�TD_SCDMAʹ��
#define SFP_CDMA_MAX_LEN		1400		//TD_CDMA���ݲ�����4100
#else
#define SFP_CDMA_MAX_LEN		560			//CDMA���ݲ�����560
#endif
#define SFP_RADIO_MAX_LEN		255			//Radio���ݲ�����255
#define SFP_NET_MAX_LEN			1400		//���紫�䲻����16383,һ��ȡ13000��������ȡ800-

//�շ����ݵ���󳤶ȵ���غ궨��
#define CCH1_BUFF_SIZE_SND		SFP_GPRS_GSM_MAX_LEN
#define CCH1_BUFF_SIZE_RCV		2048

#if HUABEI_FUN
//Ӧ�÷�֡�������Զ���Ҫ��
#define APPLY_FRAME_CUSTOM      1
#define APPLY_FRAME_SIZE        1400 - SFP_FRA_DATA
#endif


//�½�Ӧ�÷�֡���Ⱥ궨�� ֮����Ժϲ�������Ӧ�÷�֡�����Զ���Ҫ��
#if XINJIANG_FUN
#define APPLY_FRAME_SIZE		1400 - SFP_FRA_DATA
#endif

// ���ɴ����APDU����
#define APDU_BUFF_SIZE_MAX		4096


#define RELAY_485_3761			12	//485ά���ڷ���3761֡

#define TMNRELAY 					1			//�ն��м�ת��������Ҫ���к�������
#define SUCCESS 					0			//����������������Ҫ���к�������
#define PROCISOVER 					0			//���������������Ҫ���к�������
#define ERROR 						-1			//�������г��ִ���
#define ERR_PW						-2			//-2  �������,
#define ERR_OVERTIME				-3			//-3  ʱ���ǩʱ����Ч��
#define ERR_ADDR 					-4			//-4 ��ַ�����

#define RELAY_ADDR_MAXNUM	 		16			//��ת���ն��м�ת����ַ������

#define DELAYTORESET 				1			//��λ������Ҫ����ʱ��ԭ�е���800ms����ȡ1s


//#if ESAM_ENCRYPT_EN_2012 == 1
// 2012��ESAM��صĺ궨��
#define	NO_ESAM		0x0fffffff		// �ն˲�����ESAM
//#endif

//��·״̬
#if GB101_104_EN  
#define LINK_WAIT       -1      //δ֪��ȴ�
#define LINK_SEND       0x00    //ͨѶ
#endif

#define LINK_LOGIN		0x01	//��¼
#define LINK_HEART		0x02	//����
#define LINK_QUIT		0x03	//�˳���¼
#define LINK_CHANGE		0x04	//״̬�л�

#if GB101_104_EN 
#define LINK_CLOSE      0x04    //�ر�ͨ��
#endif
#define INIT_SUCCESS	0		//��ʼ���ɹ�
#define INIT_DOING		1		//���ڳ�ʼ��
#define INIT_FAIL		2		//��ʼ��ʧ��
//��Ҫ������������¼֡���ŵ�����
#define CCH_NEED_LINK(CH_ID)	(((CH_ID) == SFP_CCH_GPRS) || ((CH_ID) == SFP_CCH_Ethernet) ||((CH_ID) == SFP_CCH_CDMA) || ((CH_ID) == TDSCDMA_ID))

/*-**************     �Զ�������صĳ�������       ******************-*/
#define TASK_TYPE_1      			1        	//�ϱ���������1
#define TASK_TYPE_2      			2        	//�ϱ���������1
#define AUTOTASK_MAX_NUM     		64       	//�ն�֧�ֵ�ÿ�������������������
#define MAX_ID_NUM       			256       //-ÿ��������֧�ֵ����ID����-
#define SEND_PARA_LENGTH 	((1+6+1+1)+4*MAX_ID_NUM + 2)       //-ÿ������Ĳ�������:9(1+6+1+1)+4*256=73,�ټ���2bytes��У���룬һ��75bytes-
#define SEND_PARA_Data_LENGTH (SEND_PARA_LENGTH-2)	//--ÿ���������Ч���ݳ���(��������У�����2bytes)--
/*-**************    �Զ�������صĳ�������    end    ******************-*/

/*--֡����ƫ�Ƶ�ַ--*/
#define COMMAND_SITE   				6    		//������-
#define A1_SITE        				7			//��������A1-
#define A2_SITE        				8			//��������A2-
#define B1_SITE        				9   		//�ն˵�ַB1-
#define B2_SITE        				10			//�ն˵�ַB2-

#define MSA_SITE       				11   		//��վ��ַ-
#define DATASTART      				12   		//���ݿ�ʼλ��-
#define AFN_SITE       				12   		//Ӧ�ò㹦����-
#define SEQ_SITE       				13   		//֡������-
#define APP_DATA_SITE  				14   		//Ӧ�ò����ݿ�ʼλ�ã�6(����ͷ)+1(������C)+5(��ַ��A)+1(Ӧ�ò㹦����AFN)+1(������SEQ)--
#define SFP_FRA_DATA				17			//(����ͷ1������2��������1����ַ��8��ͷУ��2��βУ��2��������1

#define SFP_FRA_FIXHEADLEN			6			//֡ͷ�̶�����-
#define SFP_FRA_FIXUSERLEN			2			//֡β�̶�����-

#define VF_BEGIN       				0x68 		//֡��ʼ�ַ�-
#define VF_END         				0x16		//֡�����ַ�-

//��ַ���ͱ�־
#define RTU_ADDRESS_UNBC       		-2			//��֧���Զ���㲥-
#define RTU_ADDRESS_ERROR       	-1			//��ַ����-
#define RTU_ADDRESS_SINGLE       	0			//����ַ-
#define RTU_ADDRESS_WILD         	1 			//ͨ���ַ-
#define RTU_ADDRESS_TEAM         	2 			//���ַ-
#define RTU_ADDRESS_BROADCAST    	3 			//�㲥��ַ-
#define RTU_ADDRESS_RELAY        	4      		//�м̵�ַ-
#if proAdvanceEN == 1
#define RTU_ADDRESS_SELFBROADCAST   5      //-�Զ���Ĺ㲥��ַ(��������9999��ַ0000,�����뷢��һ��)
#endif

#define RTU_ADDRESS_SPEC_BROADCAST  7      // ����㲥�㲥��ַ����������FFFF��ַFFFF������ʱ��ʵ�ʵ�ַ����

//-ȷ��/����Ӧ��-
#define ANS_ASK						0x00		//ȫȷ��-
#define ANS_NAK						0xFF		//ȫ����-
#define ANS_ANK						0x55		//����ȷ��/����-//

#define SFP_FEERATE_NUM				4       	//ϵͳ֧�ֵķ�����-
#define SFP_PnFn_NUM				8			//֧��һ��DI��Pn*Fn��������-

//-��·������վ������-
#if GB101_104_EN  
#define SFP_CMD_RsetFartrans_HS0    0           //��λԶ����·
#endif
#define SFP_CMD_SendConfirm_HS1		1			//����/ȷ��	��λ����-
#if GB101_104_EN
#define SFP_CMD_SendConfirm_HS2     2			//����֡
#define SFP_CMD_SendNoReply_HS3     3           //����ȷ���û�����
#endif
#define SFP_CMD_SendNoReply_HS4		4			//����/�޻ش�	�û�����-
#define SFP_CMD_RequestRespond_HS9	9			//����/��Ӧ֡	��·����-
#define SFP_CMD_RequestRespond_HS10	10			//����/��Ӧ֡	����1������-
#define SFP_CMD_RequestRespond_HS11	11			//����/��Ӧ֡	����2������-

//-��·��Ӷ�վ������-
#define SFP_CMD_Confirm_SS0			0			//ȷ��		�Ͽ�-
#if GB101_104_EN
#define SFP_CMD_Confirm_SS1			1			//ȷ��		����-
#endif
#define SFP_CMD_Respond_SS8			8			//��Ӧ֡	�û�����-
#define SFP_CMD_Respond_SS9			9			//��Ӧ֡	���ϣ������ٻ�������-
#define SFP_CMD_Respond_SS11		11			//��Ӧ֡	��·״̬-

//-Ӧ�ò㹦����AFN-
#define SFP_AFN_Respond				0x00		//ȷ��/����
#define SFP_AFN_Reset				0x01		//��λ
#define SFP_AFN_CheckLink			0x02		//��·�ӿڼ��
#define SFP_AFN_RelayStation		0x03		//�м�վ����
#define SFP_AFN_SetPara				0x04		//���ò���
#define SFP_AFN_SetCtrl				0x05		//��������
#define SFP_AFN_SetPW				0x06		//�����֤����ԿЭ��
#define SFP_AFN_GetLevelJoin		0x08		//���󱻼����ն������ϱ�
#define SFP_AFN_GetTmnCfg			0x09		//�����ն�����
#define SFP_AFN_GetPara				0x0A		//��ѯ����
#define SFP_AFN_GetTask				0x0B		//�������ݲ�ѯ
#define SFP_AFN_GetData1			0x0C		//����1�����ݣ�ʵʱ���ݣ�
#define SFP_AFN_GetData2			0x0D		//����2�����ݣ���ʷ���ݣ�
#define SFP_AFN_GetData3			0x0E		//����3�����ݣ��¼����ݣ�
#define SFP_AFN_TransmitFile		0x0F		//�ļ�����
#define SFP_AFN_RelayData			0x10		//����ת��
#define SFP_AFN_ChannelTest			0x11		//�ŵ��������Ӧ��

#define SFP_AFN_SetPara_Ext			0x22		//�Զ����������ò���
#define SFP_AFN_GetPara_Ext			0x23		//�Զ��������ѯ����


#define NOSECURITY                  0           // �ް�ȫ����
#define NORMALSECURITY              1           // ���ļ���
#define ENCRYPTIONSECURITY          2           // ���ļ���


#define Max_PhyCh_Num   13

#if GB101_104_EN
#define PRO_DATA_TRANSMIT_APP     0     /*�ڲ�Э�� - ������ת����*/
#define PRO_LOGIN_FRAME_APP       5     /*�ڲ�Э�� - ��½֡*/
#define PRO_EXIT_LOGIN_FRAME_APP  6     /*�ڲ�Э�� - �˳���½*/
#define PRO_HEART_FRAME_APP       7     /*�ڲ�Э�� - ����֡*/
#define PROT_INIT_CHANNLE_APP     8     /*�ر�ͨ��*/
#endif

/*-******************************************************************-
 *-                             ͨ�ýṹ��                           -
 *-******************************************************************-*/

#define pt_char(vari)	(char *)&(vari)						/*-ȡ����ָ��char��-*/
#define pt_word(vari)	(uint16 *)&(vari)					/*-ȡ����ָ��word��-*/
#define pt_lword(vari)	(long *)&(vari)						/*-ȡ����ָ��lword��-*/
#define abs_long(vari)	((vari>=(long)0) ? (vari):-(vari))	/*-long�����ݵľ���ֵ-*/

typedef struct									// ��,��,��,ʱ,��,��,����
{
	char	ss;									// 0~59 BCD
	char	nn;									// 0~59 BCD
	char	hh;									// 0~23 BCD
	char	dd;									// 1~31 BCD
	char	mm;									// 1~12 BCD
	char	yy;									// 00~99 BCD
	char	ww;									// 0~6 opposite Sun~Sat
}rtc;											//ϵͳʱ�ӽṹ��

/*-�ṹ������-*/
struct rtc_hex
{
	uint8 ss;
  	uint8 nn;
  	uint8 hh;
  	uint8 dd;
  	uint8 mm;
  	uint8 yy;
};

typedef struct
{
    int	addr;
    int  len;
}T_OUT_XRAM;

typedef struct									//-��¼������ݽṹ-
{
	uint8 bERC;	     							//-��¼��-
	uint8 bLen;		 							//-��¼����-
	uint8 bData[128];							//-��¼��������-
}RECODE_SAVE_T;

typedef struct
{
  int8 min;
  int8 hour;
  int8 day;
  int8 month;
}Date_T;

typedef struct
{
	Date_T pt;									//-���һ�α��ʱ��-
	Date_T ct;              					//-���һ�������������ʱ��-
	int8 prg_num[2];        					//-��̴���-
	int8 clr_num[2];        					//-��������������-
}PROG_INFO_AND_EV_T;

typedef union SFP_SEQ
{
	struct
	{
		uint8	seq:4;
		uint8	con:1;
		uint8	fin:1;
		uint8	fir:1;
		uint8	tpv:1;
	}__attribute__ ((packed)) bit;
	uint8	all;
}__attribute__ ((packed)) SFP_SEQ_U;	//-֡����ֽ�-

/****************************--֡�ṹ--**********************************
-֡�ṹ��ʽ-
��ʼ�ַ���68H��	   ��			1B
����L			�̶�����		2B
����L			�ı���ͷ		2B
��ʼ�ַ���68H��	   ��			1B
������C			������			1B
��ַ��A			��ַ��			5B
��·�û�����	Ӧ�ò�			nB
У���CS		֡У���		1B
�����ַ���16H��					1B

-��·�û�����-
Ӧ�ò㹦����AFN					1B
֡������SEQ						1B
���ݵ�Ԫ��ʶ1					4B
���ݵ�Ԫ1						nB
|								|
���ݵ�Ԫ��ʶn					4B
���ݵ�Ԫn						nB
������Ϣ��AUX					2B+2B+6B

-������Ϣ��AUX-
����://������Ҫ�����б�����
PW0								1B
PW1								1B
+
�¼�������EC://���ھ�����Ҫ�¼��澯״̬�����б�����
��Ҫ�¼�������EC1				1B
һ���¼�������EC2				1B
+
ʱ���ǩTp:	//��������ͬʱ�������ͨ�ŷ������·������ŵ���ʱ���Խϲ�Ĵ�����
����֡֡��ż�����PFC	BIN		1B
����֡����ʱ��			���ʱ��4B
�����ʹ�����ʱʱ��	BIN	min	1B
********************************************************************************/

/*************-����L-*******************************
��Լ��ʶ�������£�
D1	D0	��Լ��ʶ����
0	0	����
0	1	����Լʹ��(04��Լ)
1	0	����
1	1	����

�û����ݳ���L1����D2~D15��ɣ�����BIN���룬�ǿ����򡢵�ַ����·�û����ݣ�Ӧ�ò㣩���ֽ�������
��������ר�����������ŵ�������L1������255��
�����������紫�䣬����L1������16383��
******************************************************/

typedef union SFP_LEN
{
	struct
	{
		uint16	sign:2;	//-��Լ��ʶ-
		uint16	len:14;
	}__attribute__ ((packed)) bit;
	uint16 all;
}__attribute__ ((packed)) SFP_LEN_U;	//-�������ֽڣ��㶫��Լ��û�й�Լ��ʶ��������Ϊ�˺͹�������-

/******************************-������C-************************************************
	        D7	        D6  	    D5	        	D4				D3~D0
���з���->	���䷽��λ	������־λ	֡����λFCB		֡������ЧλFCV	������
���з���->	DIR			PRM			Ҫ�����λACD	����			CMD
***************************************************************************************/
typedef union SFP_CMD
{
	struct
	{
		uint8	cmd:4;							//-����������-
		uint8	fcv:1;							//-֡������Чλ(����)-
		uint8	fcb_acd:1;						//-Ҫ�����λ(���з���)-
		uint8	prm:1;							//-������־λ-
		uint8	dir:1;							//-���䷽��λ-
	}__attribute__ ((packed)) bit;
	uint8 all;
}__attribute__ ((packed)) SFP_CMD_U;	//-�������ֽ�-

/********************************-��ַ��A-******************************
B1
B2
-A2-//FFFFHΪ�㲥��ַ��0000HΪ��Ч��ַ
B1
B2
-A3-
D8	D7	D6	D5	D4	D3	D2	D1	D0
        ��վ��ַMSA            |���ַ��־0/1
��վ�����ķ���֡��MSA����Ϊ0�����ն���Ӧ֡��MSAӦ����վ����֡��MSA��ͬ��
�ն���������֡��MSAӦΪ0������վ��Ӧ֡��MSAҲӦΪ0��
*************************************************************************/
typedef union SFP_ADR
{
	struct
	{
		uint16	region:16;
		uint16	address:16;
		union
		{
			struct
			{
				uint8	gflg:1;
				uint8	msa:7;
			}__attribute__ ((packed)) bit;
			uint8 all;
		}__attribute__ ((packed)) msa_gflg;
	}__attribute__ ((packed)) bit;
	uint8 all[5];
}__attribute__ ((packed)) SFP_ADR_U;	//-��ַ���ֽ�-

#if BEIJING_JZQ_EN == 1
typedef struct
{
	uint8	a1_en:1, a2_en:1, rtu_id_en:1, comm_en:1,
			rs232_en:1, rs_en:1, area_en:1, pulseconst_en:1,
			version_en:1,dummy_en:4;
	uint16 a1;				//����������A1
	uint16 a2;				//�ն˵�ַA2
	char rtu_id[8];			//�豸���
	char comm_type;			//ͨѶ����	FF��Ч��01�����ţ�02��GPRS/CDMA��03��DTMF��04Ethernet�� 05�����⣻06��RS232��07��CSD�� 08��Radio��09�����Ż���(GPRS/CDMA);
	uint8 DataBit:2, CheckMode1:1, CheckMode2:1, StopBit:1, BPS:3;
	char rs_type;			//RSͨ������ =0  ��RS ;   =1  RS11/15;  =2  RS9/15;
	char area_info;			//������ʶ	0 ����ϵͳ  1 �Ͼ�ϵͳ  2 ����ϵͳ
	char pulseconst_info;	//���峣����ʶ 0 3200 1 6400
	char AreaVersion[4];    //����������ͨ�ð汾��
	char reserve_buf[4];	//Ԥ����8���ֽ�
	uint8 chksum;			//У���
}__attribute__ ((packed)) pro_para;
#else
typedef struct
{
	uint8	a1_en:1, a2_en:1, rtu_id_en:1, comm_en:1,
			rs232_en:1, rs_en:1, area_en:1, pulseconst_en:1,
			dummy_en:4;
	uint16 a1;				//����������A1
	uint16 a2;				//�ն˵�ַA2
	char rtu_id[8];			//�豸���
	char comm_type;			//ͨѶ����	FF��Ч��01�����ţ�02��GPRS/CDMA��03��DTMF��04Ethernet�� 05�����⣻06��RS232��07��CSD�� 08��Radio��09�����Ż���(GPRS/CDMA);
	uint8 DataBit:2, CheckMode1:1, CheckMode2:1, StopBit:1, BPS:3;
	char rs_type;			//RSͨ������ =0  ��RS ;   =1  RS11/15;  =2  RS9/15;
	char area_info;			//������ʶ	0 ����ϵͳ  1 �Ͼ�ϵͳ  2 ����ϵͳ
	char pulseconst_info;	//���峣����ʶ 0 3200 1 6400
	char reserve_buf[8];	//Ԥ����8���ֽ�
	uint8 chksum;			//У���
}__attribute__ ((packed)) pro_para;
#endif


//���ܱ�װ�õ����ò����ṹ��
typedef struct
{
	uint16  metsn;			//���ܱ�/��������װ�����
	uint16 	msn;			//�����������
	uint8 	port;			//ͨ�����ʼ��˿ں�
	uint8 	protcl;			//ͨ��Э������
	uint8	addr[6];		//ͨ�ŵ�ַ
	uint8	password[6];	//ͨ������
	uint8	bFeeNum;		//���ܷ��ʸ���
	uint8	bDecNum;		//�й�����ʾֵ����λ��С��λ����
	uint8	chlAddr[6];		//�����ɼ���ͨ�ŵ�ַ
	uint8	classID;		//�û�����ż��û�С���
}__attribute__((packed))Mtr_Set_Para;

//--����ģ����Ҫ�����˽���ļ��ĺ궨��,add by zjf--

//--��Լ���ͺ��Զ������õ�һ��˽���ļ�--
#define PrtlAndTaskFile SPDATA_PRIFIX"prtl_task_privatefile"

//--��������״̬˽���ļ�--
#define ParaStateFile SPDATA_PRIFIX"ParastateFile"

/*******************************************************************************
* ϵͳ�����ͼ�������������
* ϵͳ�������ܼ��������������
*******************************************************************************/
#define DIFF_RULE_NUM		4					//���������

//�ն�֧�ֵĲ����������,ǰ64��485����������������ã���16��Ϊ�ܼ���
//#define Mtr_Num_Max 		(NO_VIR_MTR_MAX_NUM + CtrlGrp_ROUTE + 1)

#define ANOLOGY_MAX_NUM    	8					//ģ�����������

#define MTR_ID_485			1					//485���������ʼ
//�ܼ������ʼ

#if AC_CALC_EN == 1   							//ϵͳ�����ý���ʱ��485���������ʼ�Լ�485��������Ŀ
#define MTR485_BEGIN	(MTR_ID_485+0)
#else/*--ϵͳ�����ý���ʱ��485���������ʼ�Լ�485��������Ŀ--*/
#define MTR485_BEGIN	MTR_ID_485
#endif

#define SIZE_DADT			(sizeof(DADT))
#define AFN_OK_DEF			0x01					//ȷ��֡
#define AFN_ERROR_DEF		0x02					//����֡
#define AFN_ERROR_ESAM		0x08					//Ӳ����ȫ��֤����Ӧ��
#define PRTL3761			2
#define MAX_RELAY_NODE		20						//�м�����������������
#define REST_PROTECT		1						//��ֹ��λ�����·�����

#define PP_PRINT(x...)		printf(x)

//һ��Ϊ�м�F9���㳭����������Ȳ���ʱ������ŵ���ʱʱ��
//Ϊ����Ӧ��ͬ��̨�����������ֵ��Ҫ����ʵ���������
#define RELAY_OVERTIME_485	55	//485��ʱʱ�䣬��Ҫ�ڸ�ֵ�����ϳ���1000����1000����1s
#define RELAY_OVERTIME_PLC	90	//PLC
#define RELAY_OVERTIME_AC	15	//AC
#define REALY_APP_WAIT		5		//Ϊ�˱��ڳ���ģ�����㹻��ʱʱ�䣬�����ڳ�ʱ�������ʵ����ӵ���ʱ

#if LINK_BREAKPOINT_EN == 1//��·���ݷ�֡�ϵ��ʶ
#define MAX_TIME_SCALE_NUM      10
#endif
#define MAX_QUEUE_CNT 10

typedef struct DADT_
{
	uint8 DA1;		//��Ϣ��DA1
	uint8 DA2;		//��Ϣ��DA2
	uint8 DT1;		//��Ϣ��DT1
	uint8 DT2;		//��Ϣ��DT2
}DADT; //-���ݵ�Ԫ��ʶ-

typedef struct DaDtPnFn_
{
	uint8 DA1;		//��Ϣ��DA1
	uint8 DA2;		//��Ϣ��DA2
	uint8 DT1;		//��Ϣ��DT1
	uint8 DT2;		//��Ϣ��DT2
	mtrnum_t Pn;	//��Ϣ��Pn
	uint16 Fn;		//��Ϣ��Fn
}DaDtPnFn; //-���ݵ�Ԫ��ʶ �� �ֽ���Pn��Fn-


typedef struct
{
	uint8			H1;			//68����ͷ
	SFP_LEN_U		Len1;		//��һ������
	SFP_LEN_U		Len2;		//�ڶ�������
	uint8			H2;			//68����ͷ
	SFP_CMD_U		Cmd;		//������
	SFP_ADR_U		Addr;		//��ַ��
	uint8			AFN;		//Ӧ�ò㹦����
	SFP_SEQ_U		Seq;		//֡������
}__attribute__ ((packed)) FRAME_HEAD;	//����ͷ��Ϣ����68��ʼ��֡���SEQ		Wait_Proc

typedef struct
{
	uint8 RecvPipe;			//���б��ĵ�ͨ����
	char TpInfo[6];			//ʱ���ǩ��Ϣ��������б����к��и���Ϣ��������ʱ����Ҫԭ������
	int8 AddrType;			//��վ�����ַ����
}FRAME_INFO;		//-��֡���ĵĻ�����Ϣ-

typedef struct DaDtPnFn_PRTL_
{
	uint8 DA1;			//��Ϣ��DA1
	uint8 DA2;			//��Ϣ��DA2
	uint8 DT1;
	uint8 DT2;

	mtrnum_t Pn;		//��Ϣ��Pn
	uint16 Fn;			//��Ϣ��Fn
	/***********************************************************/
	//Ӧ���ĵ�ʱ��ʹ��
	int8 UnpackResult;	//����Ľ�� -5����һ�����ݵ㳭 -4��������֡��F4 Ӳ����ȫ��֤����Ӧ�� -3���������Ե�ǰDADT���� -2��������Ӧ��	-1��������		0����ȷ��		>0 ��Ӧ����
	uint8 AnswerAFN;	//Ӧ���AFN
	uint16 OutLen;		//���ĵĳ���
	uint8 *OutBuf;		//��õ�Ӧ����
	/***********************************************************/
	struct DaDtPnFn_PRTL_* pNext;	//��һ������
}DaDtPnFn_PRTL; //-���ݵ�Ԫ��ʶ �� �ֽ���Pn��Fn-


typedef struct DaDtPack_
{
	uint16			DaDtPnFnCount;		//DaDtPnFn������
	DADT			DaDtInfo;			//DADT��Ϣ
	DaDtPnFn_PRTL	*pDaDtPnFn;			//��ָ�룬ָ��ÿ��DaDtPnFn��֯�ı��ģ�ע������һ������������ָͨ��
	struct DaDtPack_ *pNext;			//ָ����һ��DaDt�ı���
}DaDtPack;	//-��¼һ�����������ݵ�ԪDaDt����֡����-

//һ֡Ӧ���ģ�����68ͷ��16���е���Ϣ
typedef struct AnswerPack_
{
	uint8 RecvPipe;			//�����ŵ�
	uint16 OutLen;			//Ӧ���ĵĳ���
	uint8 *OutBuf;			//��õ�Ӧ����
	struct AnswerPack_* pNext;	//��һ������
}AnswerPack; //-���ݵ�Ԫ��ʶ �� �ֽ���Pn��Fn-

#define RELAY_NEW		1	//�µ㳭����δ����
#define RELAY_WAIT		2	//�㳭�����ѷ���,�ȴ�����ȷ��
#define	RELAY_OVERTIME	3	//�м̳�ʱ
#define RELAY_SUCC 		4   //�м̻���ȷ��
typedef struct RelayPack_
{
	uint8	RelayType;			//�м����� RELAY_DT RELAY_TK �ȵ�
	uint8	RelayState;			//��ǰ״̬  δִ��WAIT �ȴ�����
	time_t	RelayTimeStart;		//��ʼ����
	uint32	RelayOverTime;		//��ʱ����
	uint16	RelayDataLen;		//��͸���ĳ���
	uint8	RelayDataBuf[sizeof(RELAY_IPC_PULS)];	//��͸����
}RelayPack;

#define RELAY_HEAD_NEW		1	//�µ㳭����δ����
#define RELAY_HEAD_WAIT		2	//�㳭�����ѷ���,�ȴ�����ȷ��
#define	RELAY_HEAD_OVERTIME	3	//�м̳�ʱ
#define RELAY_HEAD_SUCCESS 	4   //�м̻���ȷ��
#define RELAY_HEAD_RELACE 	5   //�ͷ�����

typedef struct RelayPackHead_
{
	uint8		RelayHeadPipe;				//�·���ǰ��͸���������ڵ�ͨ����
	uint8		RelayHeadState;				//��ǰ��͸��״̬
	uint8		RelayHeadDataBuf[CCH1_BUFF_SIZE_RCV];	//ԭʼ��͸����
	uint16		RelayHeadDataLen;			//ԭʼ��͸���ĳ���
	RelayPack	*pACRelayPack;				//�·������ɵĴ�͸����
	RelayPack	*p485RelayPack;				//�·���485�Ĵ�͸���ģ�������ʱ��֧�ֶ�·485��ͬʱ�м̣�
	RelayPack	*pPLCRelayPack;				//�·���PLC�Ĵ�͸����
	struct RelayPackHead_ *pNext;			//ָ����һ֡���ĵ��м���Ϣ
}RelayPackHead;		//-һ֡���ĵ��м�������Ϣ���ñ��Ŀ�������վ�·���F129һ�����ݴ�͸���Ҳ�������м�����F1��F9-



typedef enum
{
    eGetRequestNormal = 1,
    eGetRequestNormalList,
    eGetRequestRecord,
    eGetRequestRecordList,
    eGetRequestNext,
    #if GET_OPTIMIZATION_EN == 1
    eGetRequestConsistentRecord = 13,
    #endif
    #if GET_SIMPLIFYRECORD_EN == 1
    eGetRequestSimplifyRecord = 23,
    #endif
} GetRequestType_E;                 //GET���������

typedef enum
{
    eGetResponseNormal = 1,
    eGetResponseNormalList,
    eGetResponseRecord,
    eGetResponseRecordList,
    eGetResponseNext,
    #if GET_OPTIMIZATION_EN == 1
    eGetResponseConsistentRecord = 13,
    #endif
    #if GET_SIMPLIFYRECORD_EN == 1
    eGetResponseSimplifyRecord = 23,
    #endif
} GetResponseType_E;                //GET������Ӧ������

typedef enum
{
    eSetRequestNormal = 1,
    eSetRequestNormalList,
    eSetThenGetRequestNormalList,
} SetRequestType_E;                 //SET���������

typedef enum
{
    eSetResponseNormal = 1,
    eSetResponseNormalList,
    eSetThenGetResponseNormalList,
} SetResponseType_E;                //SET������Ӧ������

typedef enum
{
    eActionRequestNormal = 1,
    eActionRequestNormalList,
    eActionThenGetRequestNormalList,
} ActionRequestType_E;             //Action���������

typedef enum
{
    eActionResponseNormal = 1,
    eActionResponseNormalList,
    eActionThenGetResponseNormalList,
} ActionResponseType_E;            //Action������Ӧ������

typedef enum
{
    eProxyGetRequestList = 1,
    eProxyGetRequestRecord,
    eProxySetRequestList,
    eProxySetThenGetRequestList,
    eProxyActionRequestList,
    eProxyActionThenGetRequestList,
    eProxyTransCommandRequest,
} ProxyGetRequestType_E;           //Proxy���������

typedef enum
{
    eProxyGetResponseList = 1,
    eProxyGetResponseRecord,
    eProxySetResponseList,
    eProxySetThenGetResponseList,
    eProxyActionResponseList,
    eProxyActionThenGetResponseList,
    eProxyTransCommandResponse,
} ProxyResponseType_E;             //Proxy������Ӧ������


typedef struct{
	uint8 id;//����ID
	uint8 piid;
	uint8 type;
	uint8 beatPeriod[2];
	Date_Time_t dateTime;
}__attribute__ ((packed)) LinkRequest_t;

typedef struct{
	uint8 id;//����ID
	uint8 piid;
	uint8 result;
	Date_Time_t reqDateTime;//����ʱ��
	Date_Time_t recvDateTime;//����ʱ��
	Date_Time_t respDateTime;//Ӧ��ʱ��
}__attribute__ ((packed)) LinkResponse_t;



typedef struct{
	uint8 id;//����ID
	uint8 piid;
	DateTimeHex_t establishTime;
	DateTimeHex_t curTime;
}__attribute__ ((packed)) ConnectNotification_t;

#if LINK_BREAKPOINT_EN == 1//��·���ݷ�֡�ϵ��ʶ
typedef struct{
    uint8         appSlicingFlag;    //Ӧ�÷�֡��ʶ
    OOP_OAD_U          serviceOAD;        //��¼�����ݵ���oad(��60120300)
    uint8         seviceapdu[3];     //��ȡ�ͼ�¼�ķ�������
    uint16        seq;               //Ӧ�÷�֡���
    uint16        metsn;             //60120300��¼�Ĳ�������Ϣ
    DateTimeBcd_t dateTimeTag;       //60120300��¼�Ķ����ݵ�ʱ��
    uint8         theLastNTimes;     //60120300��¼��ʣ��δ������n��
    uint8         ms_cnt;            //60120300������¼��ʱ�����Ϣ
}__attribute__ ((packed)) LinkBreakPointInfo_t;
#endif

typedef struct ResultData_t_
{
	uint16			        resultDataLen;		// ���ݽ������
	SecurityMode_t          securityMode;       // ����ģʽ
	uint8                   linkSlicingFlag;    // 0xAA��·���֡��־�������˵�Ԫ����·���֡��Ƭ��
	uint16                  slicingSeq;         // ��·���֡��Ӧ�����
	#if LINK_BREAKPOINT_EN == 1
	LinkBreakPointInfo_t    linkbpinfo;
	#endif
	uint8 			        *pResultData;		// ���ݽ��������ָ��
	struct ResultData_t_    *pNext;			    // ָ����һ�����ݽ��
	struct ResultData_t_    *pFront;			// ָ��ǰһ�����ݽ��
	#if GET_OPTIMIZATION_EN == 1 || GET_SIMPLIFYRECORD_EN == 1
	uint8                   startTime[7];
	#endif
}ResultData_t;

typedef struct FrameData_t_
{
	uint16			        frameDataLen;		// ����֡����
	uint8                   pipe;               // ͨ����
	uint8                   frameType;          // ֡���ͣ���ʼ֡���м�֡������֡
	uint16                  slicingSeq;         // ��·���֡��Ӧ�����
	uint8 			        *pFrameData;		// ����֡������ָ��
	struct FrameData_t_     *pNext;			    // ָ����һ�����ݽ��
	struct FrameData_t_     *pFront;			// ָ��ǰһ�����ݽ��
}FrameData_t;

typedef enum
{
	PROXYST_NEW,//�µĴ�������
	PROXYST_RELAY_NEW,//����һ���м�
	PROXYST_RELAY_DEAL,//�м̴�����
	PROXYST_RELAY_SUCCESS,//�м̴���������м̳�ʱ
	PROXYST_RELAY_OVERTIMR,
	PROXYST_OVERTIME,//����ʱ
	PROXYST_SUCCESS,//�������
	PROXYST_RELEASE,//�ͷŴ���
}PROXY_STATE; 		/*-��͸������״̬-*/

//typedef struct FrameHeadInfo_t_
//{
//    uint8 pipe;//����ͨ����
//	LinkLayerCtrl_t ctrl;//������
//	OOP_TSA_T tsa;
//}FrameHeadInfo_t;
//
//typedef struct 
//{
//	uint8 PIID;
//	uint8 ServerID;
//	uint8 nChioce;
//	SecurityMode_t SecMod;
//	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
//	uint16 RequestApduLen;					//���������APDU��
//	uint8  pRequestApdu[1400];					//������������
//
//}Proxyresquest;
//
//typedef struct 
//{
//	uint8 PIID;
//	SecurityMode_t SecMod;
//	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
//	uint16 ResponseApduLen;					//���������APDU��
//	uint8  ResponseApdu[1400];					//������������
//	uint8 ServerID;
//
//}ProxyResponse;
//
//
//typedef struct
//{
//    uint16  Queue_Len;                          //����β
//    uint8*  Queue_Data;               //ͨѶ���ݶ���
//}Proxy_Queue; 
//
//typedef struct
//{
//	pthread_mutex_t QueueLock;                  //���ջ�����������
//    int16  QueueHead;                          //����ͷ
//    int16  QueueTail;                          //����β
//    Proxy_Queue Queue[MAX_QUEUE_CNT];               //ͨѶ���ݶ���
//}ProxyLockQueue; 

typedef struct{
	uint16 prtlVersion;//Э��汾��
	uint16 recvFrameLen;//����˽���֡����
	uint16 sendFrameLen;//����˷���֡����
	uint16 recvApduLen;//�����Apdu����
	uint8 prtlConformance[8];//Э��һ����
	uint8 funcConformance[16];//����һ����
	uint8 recvWindowSize;		//���մ��ڴ�С
	uint32 timeOut;//��̬��ʱʱ��
}AppContextInfo_t;//Ӧ�����ӻ�����Ϣ

typedef struct
{
	uint8 facCode[4];
	uint8 swVer[4];
	uint8 swDate[6];
	uint8 hwVer[4];
	uint8 hwDate[6];
	uint8 exInfo[8];
}FacVer_t;


typedef struct ApduFrameNeedInfo_t_
{
	uint8                   inServerID;         // ����ID  GET/SET/ACTION��
	uint8                   outServerID;        // ����ID  GET/SET/ACTION��
	uint16			        choiceVal;		    // �����е�����
	uint8                   ppidVal;            // PPID��
	uint16                  blockNo_l;          // ��·���֡�Ŀ����
	uint16                  blockNo_a;          // Ӧ�ò��֡�Ŀ����
}ApduFrameNeedInfo_t;	                    // APDU��֡��Ҫ����Ϣ

#if 0
typedef struct SelectTimeTagInfo_t_
{
    DateTimeBcd_t   startTime;                    // ��ʼʱ��
    DateTimeBcd_t   endTime;                      // ����ʱ��
    OOP_TI_T            ti;                           // ʱ����
    BOOL            firstStartTimeFlag;           // ��ʼ��ĵ�һ����ı�־
    uint8           theLastNTimes;                // �ϵ�n��
}SelectTimeTagInfo_t;	                      // ʱ����Ϣ


typedef struct _TimeFileStruct {
	unsigned int time;
	char chFileName[10];
}TimeFileStruct;
#endif

#if GET_DATA1_RELAY_EN == 1

#define DATA1_RELAY_NEW			1	//�µ㳭����δ����
#define DATA1_RELAY_WAIT		2	//�㳭�����ѷ���,�ȴ�����ȷ��
#define	DATA1_RELAY_OVERTIME	3	//�㳭��ʱ
#define DATA1_RELAY_PACK 		4   //��ʼ��֡����

typedef struct Data1RelayHead_
{
	//���±������״ν���ʱ��ֵ�����м�ִ�кͻ���ʱ��ʹ��
	FRAME_HEAD		Data1FrameHead;			//֡ͷ��Ϣ
	FRAME_INFO		Data1FrameInfo;			//������������Ϣ
	uint8			Data1TaskFlag;			//��������
	DaDtPack		*pData1DaDtPack;		//������м䱨��
	//���±�����Ҫ���м̹�����ʹ��
	uint8			Data1RelayState;		//��ǰ��͸��״̬
	time_t			Data1RelayTimeStart;	//��ʼ����
	uint32			Data1RelayOverTime;		//��ʱ����
	struct Data1RelayHead_ *pNext;

}Data1RelayHead;		//-һ֡���ĵ��м�������Ϣ���ñ��Ŀ�������վ�·���F129һ�����ݴ�͸���Ҳ�������м�����F1��F9-
#endif

#if EVENT_2014_EN == 1
#define BReadTask_NEW                   1           //��������������δ����
#define BReadTask_WAIT                  2           //�����������������ѷ���,�ȴ�����ȷ��
#define BReadTask_OVERTIME              3           //������������ʱ
#define BReadTask_READEVENT             4           //��ȡ�ն˵��¼���¼
#define BReadTask_PACK                  5           //��ʼ��֡����
#define BReadTask_RELACE                8           //�ͷ�����
#define BReadTask_READMETERSTATUS_AGAIN     9       //������ɺ��ٶ�һ�µ��״̬��

typedef struct BReadDI0DI1Pack_     //�����м���Ϣ
{
    char        RelayResult;                //�м̴�����  0xAA:�ɹ���0x00:δ���ͣ�0x01-��ʱ��0x03-ȷ�ϣ�0x04-���ϣ�0x05-ת���������ݣ�����:ʧ�ܻ��߳�ʱ
    uint8       RelayRetryCount;            //���Դ���
    char        DI0DI1Info[4];              //645���ݱ�ʶ��Ϣ
    uint8       DIDealFlag;                 //���ݱ�ʶ������Ϣ��0-������Ĭ�ϣ� 1-�������¼���¼��ʶ 2-�¼��޹����Ѿ�������ϵ����ݱ�ʶ 10-�ظ���ʶ 11-�����¼���¼��ʶ 12-δ���������ڣ�BIN 1
    uint8       followFramFlag;             //����֡��ʶ    0x55Ϊ����֡,�����Ǻ���֡
    uint8       followFramSeq;              //����֡���
    uint32      EventCountDI;               //���������ݼ�¼��������¼������¼������Ҫ��¼��ǰ��¼��Ӧ���¼����������ı�ʶ��
    uint16      ReadDI0DI1RelayLen;         //�м̻ظ����ĳ���
    char        ReadDI0DI1RelayBuf[300];    //�м̻��������
    uint32      LastEventCount;             //ǰһ���¼������Ĵ��������ڳ���ʱ��ʹ�ã�
    uint32      curEventCount;              //���δ��¼������Ĵ��������ڳ���ʱ��ʹ�ã�
}BReadDI0DI1Pack;   //-��¼һ���������ݱ�ʶ���м���Ϣ-

typedef struct BReadTask_
{
    FRAME_HEAD          BatchReadTaskFrameHead;         //֡ͷ��Ϣ
    FRAME_INFO          BatchReadTaskFrameInfo;         //������������Ϣ

    uint8               BatchPackDataBuf[CCH1_BUFF_SIZE_RCV];	//��������վ�ı���
    uint16              BatchPackDataLen;			    //��������վ�ı��ĳ���

    uint8               BatchReadDataBuf[1 + 1 + 6 * 32 + 6 + 1];	//ԭʼ��͸���ģ��ӡ�ת���м̼���n���������ݱ�ʶ���͡�����F9��������ͬ��
    uint16              BatchReadDataLen;               //ԭʼ��͸���ĳ���

    uint8               BatchReadTaskMetCom;            //�ն�ͨ�Ŷ˿ں�
    uint8               BReadTaskMtrSeries[6];          //ת��Ŀ���ַ
    uint8               EventStateLenAndBuf[255];      // �����ϱ�״̬�ּ����ȣ�����+���ݱ�ʶ+����,�ݴ�,������ɺ����
                                                        // ��ȫ0�����,�������
    uint8               BatchReadTaskMetPrtl;           //ת��ֱ�ӳ��������ݱ�ʶ���ͣ�0��ʾ����DL/T 645��1997��1��ʾ����DL/T 645��2007
    uint16              BatchReadTaskNum;               //���ݱ�ʶ����m

    uint8               BatchReadTaskFlag;              //��������
    uint8               BatchReadTaskState;             //��ǰ��͸��״̬
    uint16              BatchReadTaskDealIndex;         //���δ�������ݱ�ʶ���
    time_t              BatchReadTaskTimeStart;         //��ʼ����
    uint32              BatchReadTaskOverTime;          //��ʱ����
    mtrnum_t            BatchReadTaskMtrNum;            //�����·��Ĳ������
    BReadDI0DI1Pack     *pBatchReadTaskDI0DI1Pack;      //ÿ�����ݱ�ʶ���м���Ϣ
    #if JIANG_SU_FUN == 1
    uint32              BatchReadDISet07[8];            //07��״̬�ֶ�Ӧ���ݱ�ʶ
    uint16              BatchReadDISet97[8];            //97��״̬�ֶ�Ӧ���ݱ�ʶ
    uint8               BatchReadStatOnOff[2];          //��ȡ���¼�����״̬:�ֽ�1Ϊ07���ֽ�2Ϊ97
    uint8               BatchReadAcqAddr[6];            //�ϱ��¼��Ĳɼ�����ַ
    #endif
    struct BReadTask_   *pNext;                         //ָ����һ֡����������м���Ϣ
}BReadTask;     //-��ת����վֱ�ӶԵ��ܱ������������������F10��������ͷ-


#define BREADREPORT_NEW         0       //������
#define BREADREPORT_REPORT      1       //�ϱ�����վ
#define BREADREPORT_CONFIRM     2       //�ȴ���վȷ��
#define BREADREPORT_RELACE      8       //�ͷ�
typedef struct BReadReport_
{
    uint8               BReportState;               //��ǰ�ϱ���״̬
    FRAME_HEAD          BReportFrameHead;           //֡ͷ��Ϣ
    FRAME_INFO          BReportFrameInfo;           //������������Ϣ
    uint8               BPackDataBuf[CCH1_BUFF_SIZE_RCV];	//��������վ�ı���
    uint16              BPackDataLen;			            //��������վ�ı��ĳ���
    time_t              BReportTimeStart;           //��ʼ����
    uint32              BReportOverTime;            //��ʱ����
    uint8               BReportRetryCount;          //���Դ���
    struct BReadReport_   *pNext;                           //ָ����һ֡����������м���Ϣ
}BReadReport;     //-��ת����վֱ�ӶԵ��ܱ������������������F10���ϱ����е�����ͷ-

int BatchRead_Confirm(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);              //����ģ��Ӧ��appmain���������Ĵ���
int16 Event2014_MtrEventReport(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);    //���ܱ������ϱ�״̬�ֵĴ���
int16 Event2014_MtrFollowSeq(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);
int16 Event2014_RouterReport(uint8 ipc_type, uint8 relay_type, uint8 *PIpcBuf, int IpcBufLen);          //·��ģ�������ϱ�֡�Ĵ���
int16 Event2014_DealRestConfirm(uint8 relayPort, uint8 *pIpcBuf, int ipcBufLen);      // ��λ�������ϱ�״̬�ֺ���յ�ȷ��֡�Ĵ���
#endif

#define BATCHRELAY_EN		1			//������ʱ����
#if BATCHRELAY_EN == 1

#define MAX_BATCHRELAY_NODE		2048		//��������������������
#define MAX_BATCHDATA			128			//�������ݵ���󳤶�
#define BATCHTASK_AUTH			0x00		//�����֤����
#define BATCHTASK_COMP			0x01		//����ʱ����
#define BATCHTASK_RETRY_TOTAL			3			//ִ��ʧ�ܵ�����£����ԵĴ���

typedef enum
{
	//����Ϊ�����֤�������״̬
	BATCH_AUTH_NEW = 0x30,			//������
	BATCH_AUTH_WAIT,				//���������֤���ĺ󣬵ȴ����ܱ����
	BATCH_AUTH_RESEND,				//���·���
	BATCH_AUTH_FAIL,				//�����֤ʧ��
	BATCH_AUTH_SUCCESS,				//�����֤�ɹ�

	//����Ϊ��ʱ�������״̬
	BATCH_COMP_NEW = 0x50,			//�µĶ�ʱ����
	BATCH_COMP_DELAY_REQ,			//�����ز��ŵ���ʱ
	BATCH_COMP_DELAY_WAIT,			//�����ز��ŵ���ʱ���ȴ�PLC���룩
	BATCH_COMP_DELAY_RESEND,		//�����ز��ŵ���ʱ�����·��ͣ�
	BATCH_COMP_DEAL,				//�Ե����ж�ʱ����
	BATCH_COMP_DEAL_WAIT,			//���Ͷ�ʱ�������ĺ󣬵ȴ����ܱ����
	BATCH_COMP_DEAL_RESEND,			//���·���
	BATCH_COMP_FAIL,				//��ʱ����ʧ��
	BATCH_COMP_SUCCESS,				//��ʱ����ɹ�
	BATCH_COMP_FINISH,				//��ʱ�������
}enumTaskState;

typedef struct
{
	mtrnum_t		TaskMtrNum;			//�����Ӧ�Ĳ�����
	uint8			TaskMtrAddr[6];		//�����Ӧ�Ĳ������ַ
	time_t			TaskTimeStart;		//����ʼ��ʱ��(�����ֱ�ʾ)
	uint8			TaskMtrPort;		//�������Ӧ�Ķ˿ں�
	uint8			TaskMtrMode;		//�������Ӧ�Ĳ����ʲ���
	uint16			TaskTimeout;		//���ʹ�͸���ĺ�ȴ��ĳ�ʱʱ��

	//�����֤ʱ�����ص�����
	char			TaskRand2[4];		//�����2�������֤�Ļ��룩
	char			TaskEsamID[8];		//ESAM���кţ������֤�Ļ��룩

	//��ʱʱ��ESAM���ص���������
	char			TaskMtrDI0DI1[4];	//���ݱ�ʶ4�ֽ�
	char			TaskMtrCipherAndMac[16 + 4];	//��ʱ��������� + 4�ֽ�MAC

	//�����ز������㣬��ʱǰ��Ҫ�����ŵ���ʱ
	uint16			TaskTimeDelay;		//�ŵ���ʱ����λ��
}BatchTaskProc;		//ִ��һ������ʱ����Ҫʹ�õ����м����

typedef struct BatchTask_
{
	uint64			BatchTaskIndex;						//�����������ţ�����Ҫһֱ�ۼӣ�ֻ����ת������Ȩ��ʱ��Ź�0��
	uint16			BatchTaskFormat;					//�����ʽ
	uint8			BatchTaskType;						//������������ BATCHTASK_AUTH BATCHTASK_COMP �ȵ�
	uint8			BatchTaskFailCount;					//��ǰ����ִ��ʧ�ܵĴ��������ڵ��ܱ�ط��ϻ��߲����룬��Ҫ�������ԣ�
	uint16			BatchTaskInfoDataLen;				//�����������ݳ���
	uint8			BatchTaskInfoData[MAX_BATCHDATA];	//������������
	uint8			BatchTaskMtrSeries[8];				//���
	uint8			BatchTaskInfoMtrPw[32];				//�����Կ����
	enumTaskState	BatchTaskState;						//��ǰ���������״̬
	uint8			BatchTaskSendBuf[255];				//���͸����ı��ģ��������������ط���ʱ��ʹ�ã�
	uint8			BatchTaskSendLen;					//���ĳ���
	rtc_t			BatchTaskCreateTime;				//�·������ʱ��
	struct BatchTask_ *pNext;							//ָ����һ֡����������м���Ϣ
}BatchTask;		//-����������������ͷ-

#define BATCHTASK_AUTH_FILE		"batchtask_auth_file"		//�����վ�·��������֤����
#define BATCHTASK_COMP_FILE		"batchtask_comp_file"		//�����վ�·���������ʱ����
#define BATCHTASK_TOTAL_LEN		sizeof(uint64)				//�������������������ļ���ƫ�ƴ�0��ʼ
#define BATCHTASK_ADDR(taskindex)	(8 + (taskindex) * (sizeof(BatchTask)))			//��ѯĳ����������������ļ��е�ƫ��λ��

int BatchTaskAuth_Confirm(uint8 *PIpcBuf, int IpcBufLen);
int BatchTaskComp_Confirm(uint8 *PIpcBuf, int IpcBufLen);
#endif

typedef struct
{
    uint8 listNum;                  // ����
    uint8 *dataBuf;                 // ����
    uint16 dataLen;                 // ���ݳ���
}F209RelatData_t;	                // F209��͸�����ݽṹ��

typedef enum
{
    ACQTYP_NULL=0,				//�ɼ���ǰ����
    ACQTYP_LASTN,				//�ɼ��ϵ�N
    ACQTYP_FRZTM,				//����ʱ��ɼ�
    ACQTYP_TI,					//��ʱ�����ɼ�
    ACQTYP_BC,					//����
}AcqType_t;

typedef struct
{
    uint8     planId;
    uint16    storDepth;
    AcqType_t acqType;
    uint8     acqValue;
    OOP_TI_T      ti;
#if HENAN_FUN == 1 || GANSU_FUN == 1||ANHUI_FUN == 1 || SICHUAN_FUN ==1 || HUBEI_FUN == 1
    uint8     MSType;
#endif
    uint8     userType[255];
    uint8     userNum;
    OOP_OAD_U      mainOad;
    uint16    subOadNum;
    const OOP_OAD_U *oadList;
    uint8     storType;
}__attribute__ ((packed))NORPLAN_PARA_INFO_T;       //Ĭ�ϲ���

typedef struct
{
    uint8 oadNum;
    const OOP_OAD_U *defOad;
}PLAN_DEFOAD_INFO_T;

typedef struct
{
    uint16 EvtOI;                   //�¼���ʶ
    uint8  ReportEn;                //�ϱ�ʹ��
} EVENT_DEFAULT_TYPE;

typedef struct
{
    OOP_TI_T runCycle;
    uint8 plantype;
    uint8 plan_id;
    DateTimeHex_t taskStartTime;
    DateTimeHex_t taskEndTime;
    OOP_TI_T delayTime;
    uint8 priority;
    uint8 state;
    uint16 Pre_RunScript;
    uint16 Over_RunScript;
    uint8 TimePeriodType;
    uint8 TimePeriordNum;
    uint8 TimePeriod[4];

}__attribute__ ((packed))DefTask_Para_INFO_T;

typedef struct
{
    uint8      planId;
    OOP_TI_T       Delay;//�ϱ���Ӧ��ʱʱ��
    uint8      MaxReportCount;// 1�ֽ�,����ϱ�����
    OOP_OAD_U       mainOad;
    OOP_TI_T       Ti;  //���ݼ��
    uint8      userType[255];
    uint8      userNum;
}__attribute__ ((packed))REPPLAN_PARA_INFO_T;

//#if LIAONING_TICK_EN == 1
#define printt(x...) ({if (getenv("DEBUG_TICK") != NULL)  printf(x);})

typedef enum
{
    MASTER_TO_TMN_IDLE = 0,
    MASTER_TO_TMN_NEW,                  //�ն�����ʱ��������(׼����������)
    MASTER_TO_TMN_WAIT,                 //�ն�����ʱ�ӵȴ�����״̬(�ȴ���������)
    MASTER_TO_TMN_OVERTIME,             //�ն�����ʱ�ӵȴ���ʱ״̬
    MASTER_TO_TMN_PRECESE_SUCC,         //�ն�����ʱ�ӽ������

    MASTER_TO_TMN_NEW_SEC,              //�ն��ٴ�����ʱ��������
    MASTER_TO_TMN_WAIT_SEC,             //�ն��ٴ�����ʱ�ӵȴ�����״̬
    MASTER_TO_TMN_OVERTIME_SEC,         //�ն��ٴ�����ʱ�ӵȴ���ʱ״̬
    MASTER_TO_TMN_PRECESE_SUCC_SEC,     //�ն��ٴ�����ʱ�ӽ������

    MASTER_TO_TMN_PRECESE_NEXTWAIT,     //�ն��´�����ʱ��
} MASTER_TO_TMN_STAT; //��վ���ն˵Ķ�ʱ����״̬

typedef enum
{
    TMN_TO_METER_IDLE = 0,
    TMN_TO_METER_NEW,
    TMN_TO_METER_RDINIT_FIRST,         //�ն��״ζԵ���ѯʱ�ӳ�ʼ��
    TMN_TO_METER_RDWAIT_FIRST,         //�ն��״ζԵ���ѯʱ�ӵĵȴ�
    TMN_TO_METER_FIRST_OVERTIME,       //�ն��״ζԵ���ѯʱ�ӳ�ʱ�ȴ�
    TMN_TO_METER_FIRST_SUC,            //�ն��״ζԵ���ѯʱ�ӳɹ���־

    TMN_TO_METER_SETTIME_INIT,         //�ն˶Ե������ʱ�ӵĳ�ʼ��
    TMN_TO_METER_SETTIME_WAIT,         //�ն˶Ե������ʱ�ӵĵȴ�
    TMN_TO_METER_SETTIME_OVERTIME,     //�ն˶Ե������ʱ�ӵĳ�ʱ
    TMN_TO_METER_SETTIME_SUC,          //�ն˶Ե������ʱ�ӵĳɹ�

    TMN_TO_METER_RDINIT_SEC,           //�ն��ٴζԵ���ѯʱ�ӳ�ʼ��
    TMN_TO_METER_RDWAIT_SEC,           //�ն��ٴζԵ���ѯʱ�ӵĵȴ�
    TMN_TO_METER_SEC_OVERTIME,         //�ն��ٴζԵ���ѯʱ�ӳ�ʱ�ȴ�
    TMN_TO_METER_SEC_SUC,              //�ն��ٴζԵ���ѯʱ�ӳɹ���־

} TMN_TO_METER_STAT; //�ն˶Ե���ʱ����״̬

typedef enum
{
    NULL_TASK = 0,                   //�޵�ǰ����
    MASTER_TO_TMN_PRECESE_TASK,      //��վ���ն˵ľ�ȷ��ʱ����
    TMN_TO_METER_TASK,               //�ն˶Ե���ʱ����
} TIMETASKTYPE; //��ʱ��������

typedef struct
{
    uint8     MasterToTmn_Mode;                // 1�ֽ�: ��ʱģʽ
    uint8     MasterToTmn_HeartRate;           // 1�ֽڣ��������ʱ���ܸ���
    char      MasterToTmn_MaxEliminatedNum;    // 1�ֽڣ����ֵ�޳�����
    char      MasterToTmn_MinEliminatedNum;    // 1�ֽڣ���Сֵ�޳�����
    uint8     MasterToTmn_HaltdelayTime;       // 1�ֽڣ�ͨѶ��ʱ��ֵ����λ���룩
    uint8     MasterToTmn_LeastEffectiveNum;   // 1�ֽڣ�������Ч����
} __attribute__ ((packed)) MASTER_TO_TMN_PARA_STRUCT; //�ն˾�ȷ��ʱ��ز���

typedef struct
{
    uint8     TmnToMeter_TimeFlag;  // 1�ֽ�: �ն��Ƿ��������Уʱģʽ
    uint8     TmnToMeter_Mode;      // 1�ֽ�: ��ʱģʽ 2:�ն˹㲥Уʱ 3:�ն˵���ַ�㲥Уʱ����
    uint8     TmnToMeter_Start[3];  // 3�ֽڣ��ն˹㲥Уʱ����ʱ��
    char      TmnToMeter_Limit;     // 1�ֽڣ�ʱ�������ֵ(��λ����)
} __attribute__ ((packed)) TMN_TO_METER_PARA_STRUCT; //����ʱ����

typedef struct
{
    TIMETASKTYPE    TimeTaskType;   //��ʱ��������(�ϵ�򵥶�ʱ���� ��ȷ��ʱ����  �ն˸�����ʱ����)
    uint8           TaskSucBit;     //����������ɱ�־(�ϵ�򵥶�ʱ���� ��ȷ��ʱ����  �ն˸�����ʱ����)
} TIMETASK_STRUCT;
//#endif

#if SAL_PRTL == 1
typedef struct
{
    uint8   func        :4;	//������
    uint8   flag        :1;	//��վ����Э�̱�־
    uint8   subf        :1;	//��֡��־
    uint8   PRM         :1;	//������־λ
    uint8   DIR         :1;	//���䷽��λ
} __attribute__ ((packed)) SAL_FC_WORD_B;

typedef union
{
    SAL_FC_WORD_B      BitSect;
    uint8  			   byte;
} __attribute__ ((packed)) SAL_FC_BYTE;

typedef struct
{
    uint8   ESAMmode        :4;	//�����㷨 0:���� 1:CBC����
    uint8   reserve         :4;	//����
    uint8   var             :8;	//SALЭ��汾
} __attribute__ ((packed)) SAL_SV_B;

typedef union
{
    SAL_SV_B           BitSect;
    uint16              Word;
} __attribute__ ((packed)) SAL_SV_WORD;

typedef struct
{
	uint8   PrtlVer        :5; //Ӧ�ò�Э��汾
	uint8   reserve        :3; //����
	uint8   AddrClass      :3; //��ַ����
	uint8   DevClass       :5; //�豸����
} __attribute__ ((packed)) SAL_DAT_B;
typedef union
{
	SAL_DAT_B      bitsect;
	uint16           word;
} __attribute__ ((packed)) SAL_DAT_WORD;

typedef struct _DA_t
{
	union
	{
		struct
		{
			uint8 termlen    :4;      //������ַ+�߼���ַ����
			uint8 mslen      :3;      //��վ��ַ����
			uint8 reserve    :1;      //����
		}DAL_B;
		uint8 byte;
	}DAL;                               //�豸��ַ����
	uint8          DAC[8];                //�豸��ַ
}__attribute__ ((packed)) DA_t;

typedef struct _SA_t
{
	union
	{
		struct
		{
			uint8 len:5;                 //Դ��ַ����
			uint8 portLen:3;                //�˿ڳ���
		}SAL_B;
		uint8 byte;
	}SAL;
	uint8          SAC[16];               //�豸��ַ
}__attribute__ ((packed)) SA_t;

typedef struct _TA_t
{
	union
	{
		struct
		{
			uint8 len:5;                 //Դ��ַ����
			uint8 portLen:3;                //�˿ڳ���
		}TAL_B;
		uint8 byte;
	}TAL;
	uint8          TAC[16];              //�豸��ַ
}__attribute__ ((packed)) TA_t;

typedef struct _CI_t
{
	union 
	{
		struct
		{
			uint8 len:4;                    //�ŵ���Ϣ����
			uint8 type:4;                   //ͨ���ŵ�����
		}CIL_B;
		uint8 byte;
	}CIL;
	uint8          CIC[15];                 // ͨ����Ϣ
}__attribute__ ((packed)) CI_t;

typedef struct _TP_t
{
	uint8 yyH;
	uint8 yyL;
	uint8 mon;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}__attribute__ ((packed)) TP_t;

typedef struct _GA_t
{
	uint8          GAL;                    // ���س���
	uint8          GAC[6];                 // ������Ϣ
}__attribute__ ((packed)) GA_t;

typedef struct _CD_t
{
	SAL_FC_BYTE    FC;                      //������
	SAL_SV_WORD    SV;                      //SSALЭ��汾�ͼ����㷨
	SAL_DAT_WORD   DAT;                     //�豸��ַ����
	DA_t           DA;                      //�豸��ַ
	SA_t           SA;                      //Դ��ַ
	TA_t           TA;                      //Ŀ���ַ
	CI_t           CI;                      //�ŵ���Ϣ
	TP_t           TP;                      //ʱ���ǩ
	GA_t           GA;                      //������Ϣ	
}__attribute__ ((packed)) CD_t;

typedef struct _SALHeadInfo_t
{
	uint8          bStartword;               // ��ʼ�ַ�
	uint16         L;                        // SAL���� 
	uint16         SEQ;                      // ֡���
	uint16          C;                       // ������
	CD_t           CD;                       // ��������
	uint16         HCS;                      // ֡ͷУ��
	uint16         RevInfo;                  // ������Ϣ Ӧ��ʱ��
	uint16         len698;                   // �������������ȣ�����ʱ����Ϊ��������������
}__attribute__ ((packed)) SALHeadInfo_t;    // SAL֡ͷ���ݽṹ��  

typedef struct _SALSubFrame_t
{
	uint8          bStartword;               // ��ʼ�ַ�
	uint16         L;                        // SAL���� 
	uint16         SEQ;                      // ֡���
	uint16          C;                       // ������
	CD_t           CD;                       // ��������
	uint16         HCS;                      // ֡ͷУ��
	uint16         RevInfo;                  // ������Ϣ Ӧ��ʱ��
	uint16         len698;                   // �������������ȣ�����ʱ����Ϊ��������������
	uint8 *        salbuf;                   //�������ݻ�����
	struct _SALSubFrame_t *next;
}__attribute__ ((packed)) SALSubFrame_t;   //��֡��Ϣ�ṹ 

//�ն˷��ش�����
//��ȫ��֤����Base��0x1000��
#define SAL_RECVINFO_ERROR_TERMINAL_DECIPHERING 0x1001       //�ն˽��ܴ���
#define SAL_RECVINFO_ERROR_TERMINAL_SIGNCHECK   0x1002       //�ն���ǩʧ��
#define SAL_RECVINFO_ERROR_TERMINAL_MAC         0x1003       //�ն�MACУ��ʧ��
#define SAL_RECVINFO_ERROR_TERMINAL_SN          0x1004       //�Ự����������
#define SAL_RECVINFO_ERROR_GW_DECIPHERING       0x1005       //���ؽ��ܴ���
#define SAL_RECVINFO_ERROR_GW_SIGNCHECK         0x1006       //������ǩʧ��
#define SAL_RECVINFO_ERROR_GW_MAC               0x1007       //����MACУ��ʧ��
#define SAL_RECVINFO_ERROR_GW_PASSWD            0x1008       //�������뵥Ԫ����
#define SAL_RECVINFO_ERROR_LINK_PASSWD          0x1009       //��·�豸���뵥Ԫ����
//֡У�����(Base��0x2000)
#define SAL_RECVINFO_ERROR_DATA					0x2000       //֡��������(�ն�ά��)
#define SAL_RECVINFO_ERROR_SAL_SV               0x2001       //Э��汾����
#define SAL_RECVINFO_ERROR_DES_MISMATCHING      0x2002       //�ӽ����㷨��־��ƥ��
#define SAL_RECVINFO_ERROR_DEVICE_TYPE          0x2003       //�豸�����޷�ʶ��
#define SAL_RECVINFO_ERROR_CONTROL              0x2004       //�������޷�ʶ��
#define SAL_RECVINFO_ERROR_DIR                  0x2005       //���䷽��λ����
#define SAL_RECVINFO_ERROR_APDU_LEN_ABMORMAL    0x2006       //�����򳤶��쳣��С��4�ֽڣ�
#define SAL_RECVINFO_ERROR_APDU_LEN             0x2007       //�����򳤶Ȳ�ƥ��
//��·����(Base��0x3000)
#define SAL_RECVINFO_ERROR_NO_TARGET_NDOE       0x3001       //Ŀ��ڵ㲻����
#define SAL_RECVINFO_ERROR_NO_SESSION_LINK      0x3002       //��ǰ�Ự��·δ����
#define SAL_RECVINFO_ERROR_SEND_INFO            0x3003       //���ķ���ʧ��
#define SAL_RECVINFO_ERROR_CI                   0x3004       //�ŵ�����
#define SAL_RECVINFO_ERROR_SESSION_NEGOTIATION  0x3005       //��ǰ��·�ỰЭ��ʧ��

// ���ɴ����APDU����
#define APDU_BUFF_SIZE_MAX		4096

#define SAL_SUBFRAME                    0x7FFFFFF0  //��֡
#define SAL_SUBFRAME_START              0x7FFFFFF1  //��֡��ʼ֡
#define SAL_SUBFRAME_MID                0x7FFFFFF3  //��֡�м�֡
#define SAL_SUBFRAME_END                0x7FFFFFF2  //��֡����֡
#define SAL_SUBFRAME_GW_ANSWER          0x7FFFFFF4  //���ض��ն����з�֡��Ӧ��
#define SAL_FRAM_CHECK_FAIL 			-3			//SALЭ���֡ʧ��
#define SAL_PROCESS_FAIL 				-2			//SAL���봦����̳����ط���
#define SAL_LINK_FRAME 					-1			//SAL��·����֡,�������?
#define SAL_NONE_698_FRAME 				0			//��SAL֡������698���ݣ�ֱ�ӻ���
#define SAL_PROCESS_SUCCESS             1           //��֡�ɹ�

//�������Ӧλ
#define SAL_C_FC               (1<<15)     //������
#define SAL_C_SV               (1<<14)     //Э��汾
#define SAL_C_DAT              (1<<13)     //�豸����
#define SAL_C_DA               (1<<12)     //�豸��ַ
#define SAL_C_SA               (1<<11)     //ԭ��ַ
#define SAL_C_TA               (1<<10)     //Ŀ�ĵ�ַ
#define SAL_C_CI               (1<<9)      //ͨ����Ϣ
#define SAL_C_TP               (1<<8)      //ʱ���ǩ
#define SAL_C_GA               (1<<7)      //���ص�ַ
#endif

#if TMNHALT_METHOD ==1
//int Pro_RelayHalt(uint8 *PIpcBuf, int IpcBufLen);
#endif

extern int ToIPCUpdateFTP( uint8 *pBuf, int nBufLen );
int PrtlPro_CreatFrameHead(FRAME_HEAD *pFrameHead);			//��ȡ�����ϱ��ı���ͷ��Ϣ
int PrtlPro_Makeframe(FRAME_HEAD FrameHead, FRAME_INFO FrameInfo, char *pInBuf, uint16 InbufLen, char *pOutBuf, uint16 *OutbufLen);
int GetNextTimeTag(SelectTimeTagInfo_t STInfo, PlanTypeInfo_t planTypeInfo, OadQueryInfo_t oadQueryInfo, DateTimeBcd_t *dateTimeTag);
#if GPRS_ETH_ALL_OLINE == 1
extern int PrtlPro_Link(uint8 pipe, int bProcFlag, char *pOutBuf, uint16 *OutbufLen);
#else
extern int PrtlPro_Link(int bProcFlag, char *pOutBuf, uint16 *OutbufLen);
#endif

extern int16 Event2014_MtrEventCountRead(void);
extern int16 BatchRead_TaskProc(uint8 *pipe, uint8 *pOutBuf, uint16 *pOutBufLen);
extern int16 BatchReport_TaskProc(uint8 *pipe, uint8 *pOutBuf, uint16 *pOutBufLen);

#endif
