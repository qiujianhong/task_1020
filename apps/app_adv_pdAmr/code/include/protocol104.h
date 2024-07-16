/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief��   104��Ϣģ��ͷ�ļ�
* @date��    2020-3-4
* @history��
*********************************************************************
*/
#ifndef _PROTOCOL104_H
#define _PROTOCOL104_H

/* ��������ʵʩϸ����Ĭ�����䶨��
		״̬����Ϣ 		1-4000
		ģ������Ϣ 		4001-6000
		��������Ϣ 		6001-6200
		��������Ϣ 		6401-6500
		������Ϣ        8001-9000
*/

/*��������ʵʩϸ���Ӧ�ô�������ȼ�������
		1 ��ʼ������
		2 ���ٻ���Ӧ�����ݣ���ʼ����
		3 ң�������Ӧ����
		4 �¼�˳���¼��TCOS��
		5 ���ٻ���Ӧ�����ݣ��ǳ�ʼ����
		6 �����¼�
		7 ʱ��ͬ����Ӧ����
		8 �仯ң��
		9 ��λ����
		10 �ļ��ٻ�
		11 �ļ�����
		12 �������ٻ�
*/


//����������ʵʩϸ���޸�����ʱ�估����201907
#define MAX_FRAMCOM_DELAY_T1   20  //����֡δ�յ�ȷ�������ʱʱ��
#define IFRAMCOM_TIMES_MAX_K   12  //I֡δȷ�Ϸ���֡�������
#define IFRAMCOM_TIMES_MAX_W   8   //I֡δȷ�Ͻ���֡�������
#define MAX_FRAMCOM_DELAY_T2   10  //����I֡�����ȷ�ϵļ��ʱ��
#define MAX_FRAMCOM_DELAY_T3   30  //������ʱ����



#define YX_START_NUM       	0x0001 	//ң�š���Ϣ�����ַ��ΧΪ1H~1000H��
#define MAX_YX_NUM               4  //�ն�ң��������·������Ҫ���������ֵ������
#define JDPROT1_START_NUM  	0x1001 	/*�̵籣��1����Ϣ�����ַ��ΧΪ1001H~2000H����Ӧ��CP56Time2aʱ��ļ̵籣���豸�¼���
									  ��CP56Time2aʱ��ļ̵籣���豸���������¼�����CP56Time2aʱ��ļ̵籣���豸�������
									  ��·��Ϣ*/
#define JDPROT2_START_NUM  	0x2001 	//�̵籣��2����Ϣ�����ַ��ΧΪ2001H~3000H��Ϊ�̵籣�������Ĺ��ϵ�������
#define JDPROT3_START_NUM  	0x3001 	//�̵籣��3����Ϣ�����ַ��ΧΪ3001H~4000H��Ϊ�̵籣����ֵ�趨����
#define YC_START_NUM       	0X4001 	//ң�����Ϣ�����ַ��ΧΪ4001H~5000H��
#define MAX_YC_NUM             59   //ң������������·������Ҫ���������ֵ������
#define PARA_START_NUM  	0x5001	//������ַ��ΧΪ5001H~6000H��
#define PARA_MAX_NUM        20
#define YK_START_NUM 		0x6001	//ң�ء�������ַ��ΧΪ6001H~6200H��
#define SET_START_NUM 		0x6201	//�趨��ַ��ΧΪ6201H~6400H��
#define ENERGY_START_NUM 	0x6401	//�����ۼ�����ַ��ΧΪ6401H~6600H��
#define MAX_ENDATA_NUM      8  		//�������ݸ���
#define STEP_START_NUM 		0x6601	//��λ����Ϣ��ַ��ΧΪ6601H~6700H��
#define TRSBIT_START_NUM 	0x6701	//�򱻿�վ���Ͷ�������Ϣ��ַ��ΧΪ6701H~6800H��
#define BCD_START_NUM 		0x6801	//BCD���ַ��ΧΪ6801H~6900H��
#define RTUREMOT_START_NUM 	0x6901	//Զ���ն�״̬��ַ6901H��
#define FILE_START_NUM 		0x6902	//�ļ����͵�ַ6902H~7000H
#define MAX_YK_NUM			2		//ң��·����·������Ҫ���������ֵ������
#define FAULT_MAX_NUM       40      //������Ϣ�ϱ�֧����Ϣ��������
#define FALT_START_NO       0x0005  //������Ϣ�ϱ���Ϣ���ַ��ʼ���
#define FALT_START_ADDR     0x0101  //������Ϣ�ϱ���Ϣ���ַ��ʼ���

/****************************************************************/
//IEC104��ԼӦ�÷���Ӧ��������̽��̶���
/***************************************************************/
#define STEP_ALL_DATA_CALL_ALL      0//���ٲ�
#define STEP_SOE_CHANGE_SEND       	1//soe��λ�ϱ�
#define STEP_YC_CHANGE_SEND         2//ң��仯�ϱ�
#define STEP_ENN_DATA_CALL_ALL      3//�������ٲ�
#define STEP_PRESS_RESET            4//���̸�λ�ϱ���ʼ������
#define STEP_DATA_READ              5//������
#define STEP_TIMER_SYN              6//ʱ��ͬ��
#define STEP_REMOTE_CTRL_ACK        7//ң��Ӧ��
#define STEP_PARA_RES_ACK0          8//�����ϱ���������
#define STEP_PARA_RES_ACK1          9//�����ϱ���������
#define STEP_NEW_PARA_CALL_ACK      10//�����¹淶�ٲ������������
#define STEP_IDEL_ACK_SEND          11//ͨѶ���з�����·����
#define STEP_FAULT_CHANGE_SEND      12//������Ϣ�ϱ�
#define STEP_CONT_DEFAULT           0xaa//����
/****************************************************************/
/****************************************************************/
//IEC101��104��Լ��������Ӧ����̽��̶���
/***************************************************************/
#define ENDATA_STEP_SEND_START       0
#define ENDATA_STEP_SEND_DATA        1
#define ENDATA_STEP_SEND_DELAY       2
#define ENDATA_STEP_SEND_STOP        3
#define ALLDATA_STEP_SEND_DEFAULT   0xaa
/****************************************************************/

/****************************************************************/
//IEC104��ԼӦ�÷���Ӧ�����ٹ��̽��̶���
/***************************************************************/
#define ALLDATA_STEP_SEND_YX        0
#define ALLDATA_STEP_SEND_YC        1
#define ALLDATA_STEP_SEND_STOP      2
#define ALLDATA_STEP_SEND_DEFAULT   0xaa
/****************************************************************/


/* **************************************************************/
//ҵ�����Ӧ�����Ͷ���
#define APP_TYPE_NOMMAL           		0x00 //ͨ��
#define APP_TYPE_FILE_ABOUT             0X01 //�ļ����������
#define APP_TYPE_CTRL_SELECT_ACK       	0x02 //ң��ѡ��ȷ�ϵȣ�����ն������
#define APP_TYPE_PARA_ACK               0X03 //����ȷ�ϵ�
#define APP_TYPE_CTRL_SELECT			0X05 //ң��ѡ��ң�س���
#define APP_TYPE_CTRL_RUN				0x07 //ң��ִ��
#define APP_TYPE_UPDATA				    0x08 //�������


//�������ն�˫����֤Ӧ�����Ͷ���
#define APP_TYPE_NET_RAND               0x20//���ط��������
#define APP_TYPE_MTU_AUTH               0x21//�ն���������֤����
#define APP_TYPE_NET_AUTH               0x22//���ط����ն˵���֤����������ն�������֤
#define APP_TYPE_AUTH_ACK               0x23//��֤�������
#define APP_TYPE_NET_SERI_RES           0x24//���������ն˼�оƬ���к�
#define APP_TYPE_NET_SERI_ACK           0x25//�ն˷����ն����кż�����оƬ���к�

//֤�������Ӧ������ ����
#define TOOL_TYPE_CHECK_RES_START       0x30//֤������߷�����֤����
#define TOOL_TYPE_CHECK_RES_START_ACK   0x31//�ն���Ӧ֤������ߵ���֤����
#define TOOL_TYPE_CHECK_RES        		0x32//֤������߶��ն���֤�������Ӧ
#define TOOL_TYPE_CHECK_RES_ACK         0x33//�ն���֤������߷�����֤�����Ӧ������0x33
#define TOOL_TYPE_KEY_VER_RES           0x34//��ȡ�ն���Կ�汾
#define TOOL_TYPE_KEY_VER_RES_ACK       0x35//�ն˷�����Կ�汾
#define TOOL_TYPE_MTU_ID_RES            0x36//��ȡ�ն����к�
#define TOOL_TYPE_MTU_ID_RES_ACK        0x37//�ն˷����ն����к�
#define TOOL_TYPE_MTU_SERI_RES          0x38//��ȡ�ն����ð�ȫоƬ���к�
#define TOOL_TYPE_MTU_SERI_RES_ACK      0x39//�ն˷������ð�ȫоƬ�����к�
#define TOOL_TYPE_MTU_KEY_RES           0x3A//��ȡ�ն˰�ȫоƬ��Կ
#define TOOL_TYPE_MTU_KEY_RES_ACK       0x3B//�ն˷��ع�Կ
#define TOOL_TYPE_CERT_FILE_RES         0x3C//֤������߽����ɵ�֤�������ļ������ն�
#define TOOL_TYPE_CERT_FILE_RES_ACK     0x3D//��֤������߷���ǩ�����
#define TOOL_TYPE_CERT_FILE_DOWN        0x3E//֤������߽����Ӧ��CA֤�顢��վ֤�顢����֤�顢�ն�֤�鵼���ն�
#define TOOL_TYPE_CERT_FILE_DOWN_ACK    0x3F//�ն˷���֤�鵼����
#define TOOL_TYPE_CERT_REBACK_RES       0x40//�ն˳�ʼ֤���д
#define TOOL_TYPE_CERT_REBACK_RES_ACK   0x41//�ն˷��س�ʼ֤���д���
#define TOOL_TYPE_KEY_CERT_RES          0x42//֤������ߵ����ն˹�Կ֤��
#define TOOL_TYPE_KEY_CERT_SEND    		0x43//�ն���֤������߷����ն�֤��
#define TOOL_TYPE_KEY_CERT_RES_ACK      0x44//֤����������ն˷���֤�鵼�����
#define TOOL_TYPE_KEY_CERT_SEND_ACK     0x45//��֡���ݴ���������
#define TOOL_TYPE_MTU_KEY_REBACK        0x46//֤������߻ָ��ն���Կ
#define TOOL_TYPE_MTU_KEY_REBACK_ACK    0x47//�ն���֤������߷�����Կ�ָ����
#define TOOL_TYPE_APP_DATA_RES          0x48//֤����������ն˷����������
#define TOOL_TYPE_APP_DATA_RES_ACK      0x49//�ն���֤������߷����������



//��վ���ն�˫����֤Ӧ�����Ͷ���
#define APP_TYPE_HOST_RAND              0x50//��վ���������
#define APP_TYPE_CLIENT_AUTH            0x51//�ն�����վ��֤����
#define APP_TYPE_HOST_AUTH              0x52//��վ�����ն˵���֤����������ն�������֤
#define APP_TYPE_HOST_ACK               0x53//��֤���������վ
#define APP_TYPE_HOST_SERI_RES          0x54//��վ�����ն˼�оƬ���к�
#define APP_TYPE_HOST_SERI_ACK          0x55//�ն˷��ؼ���оƬ���к�
#define APP_TYPE_HOST_CODE_RES          0x56//��վ�����ն�������
#define APP_TYPE_HOST_CODE_ACK          0x57//�ն˷����ն�ID��оƬ���к�
#define APP_TYPE_MANA_CODE_RES          0x58//��վͨ������ͨ�������ն�������
#define APP_TYPE_MANA_CODE_ACK          0x59//�ն˷����ն�ID��оƬ���к�

//��վ���ն���Կ����Ӧ�����Ͷ���
#define APP_TYPE_KEY_VER_RES            0x60//��վ���ն�������Կ�汾
#define APP_TYPE_KEY_VER_ACK            0x61//�ն˷�����Կ�汾
#define APP_TYPE_KEY_CHANGE_RES         0x62//��վ������Կ����
#define APP_TYPE_KEY_CHANGE_ACK         0x63//�ն˷�����Կ���½��
#define APP_TYPE_KEY_REBACK_RES         0x64//��վ������Կ�ָ�
#define APP_TYPE_KEY_REBACK_ACK         0x65//�ն˷�����Կ�ָ����

//��վ���ն�֤�����Ӧ�����Ͷ���
#define APP_TYPE_CERT_CHANGE_RES        0x70//��վ����֤�����
#define APP_TYPE_CERT_CHANGE_ACK        0x71//�ն˷���֤����½��
#define APP_TYPE_CERT_DOWN_RES          0x72//��վ����֤������
#define APP_TYPE_CERT_DOWN_ACK          0x73//�ն˷���֤�����ؽ��
#define APP_TYPE_CERT_GET_RES        	0x74//��վ��ȡ�ն�֤��
#define APP_TYPE_CERT_GET_ACK        	0x75//�ն˷���֤��
#define APP_TYPE_CERT_GET_OVER        	0x76//��վ����֤����ȡ���


//ҵ�������� ����˵��
#define BUSS_ERR_APP_TYPE   			0x9101//ҵ��Ӧ�����ʹ���
#define BUSS_ERR_MESS_CHACK 			0x9102//������ǩʧ��
#define BUSS_ERR_MESS_DECI  			0x9103//���Ľ���ʧ��
#define BUSS_ERR_RAND_CHECK 			0x9104//�������֤ʧ��
#define BUSS_ERR_TIME_CHECK 			0x9105//ʱ��У��ʧ��
#define BUSS_ERR_APP_SAFE   			0x9106//ҵ��ȫҪ�󲻺Ϲ棬�����ϱ��İ�ȫ�淶Ҫ������Ҫ���ܵı���δ���м���
#define BUSS_ERR_APP_FLOW   			0x9107//ҵ��ȫ���̷Ƿ� ��û�а��ա� ����֤����ҵ�񡱴���
#define BUSS_ERR_POWER_LOW  			0x9108//Ȩ�޲���
#define BUSS_ERR_NONE_KNOW  			0x9109//δ֪����
#define BUSS_ERR_MESS_UNPACK   			0x9110//���ĳ�����������ʧ��
#define BUSS_ERR_MESS_CRC  				0x9111//CRC ����
#define BUSS_ERR_MESS_MAC   			0x9112//MAC ����

//��ȫ��չ������
#define DASAFE_SUCCSESS 				0x9000//�ɹ�
#define DASAFE_ERR_CHACK 				0x9090//��֤ʧ��
#define DASAFE_ERR_KEY_CHANGE 			0x9091//��Կ����ʧ��
#define DASAFE_ERR_KEY_REBACK  			0x9092//��Կ�ָ�ʧ��
#define DASAFE_ERR_CERT_IN				0x9093//֤�鵼��ʧ��
#define DASAFE_ERR_CERT_OUT 			0x9094//֤�鵼��ʧ��
#define DASAFE_ERR_CERT_GET 			0x9095//֤����ȡʧ��
#define DASAFE_ERR_MESS_RECV 			0x9096//��֡���ݽ���ʧ��
#define DASAFE_ERR_CERT_DOWN 			0x9097//֤��Զ�̸���/����ʧ��

/**************************************************************/



/****************************************************************/
//IEC101��104��ԼӦ�÷������ݵ�Ԫ֮���ͱ�ʶ
/***************************************************************/
//          ���ͱ�ʶ   ���		 ����
#define		M_SP_NA_1 	1		//������Ϣ
#define		M_SP_TA_1 	2		//��ʱ��ĵ�����Ϣ
#define		M_DP_NA_1  	3		//˫����Ϣ
#define		M_DP_TA_1 	4		//��ʱ���˫����Ϣ
#define		M_ST_NA_1  	5		//��λ����Ϣ
#define		M_ST_TA_1	6		//��ʱ��Ĳ�λ����Ϣ
#define		M_BO_NA_1	7		//32���ش�
#define		M_BO_TA_1	8		//��ʱ���32���ش�
#define		M_ME_NA_1	9		//����ֵ, ��һ��ֵ
#define		M_ME_TA_1  	10		//����ֵ����ʱ��Ĺ�һ��ֵ
#define		M_ME_NB_1	11		//����ֵ, ��Ȼ�ֵ
#define		M_ME_TB_1  	12		//����ֵ, ��ʱ��ı�Ȼ�ֵ
#define		M_ME_NC_1  	13		//����ֵ, �̸�����
#define		M_ME_TC_1  	14		//����ֵ, ��ʱ��Ķ̸�����
#define		M_IT_NA_1	15		//�ۼ���
#define		M_IT_TA_1  	16		//��ʱ����ۼ���
#define		M_EP_TA_1  	17		//��ʱ��ļ̵籣���豸�¼�
#define		M_EP_TB_1  	18		//��ʱ��ļ̵籣���豸���������¼�
#define		M_EP_TC_1  	19		//��ʱ��ļ̵籣���豸���������·��Ϣ
#define		M_PS_NA_1  	20		//����λ����ĳ��鵥����Ϣ
#define		M_ME_ND_1  	21		//����ֵ, ����Ʒ�������ʵĹ�һ��ֵ
#define		M_SP_TB_1  	30		//��CP56Time2aʱ��ĵ�����Ϣ
#define		M_DP_TB_1  	31		//��CP56Time2aʱ���˫����Ϣ
#define		M_ST_TB_1  	32		//��CP56Time2aʱ��Ĳ�λ����Ϣ
#define		M_BO_TB_1	33		//��CP56Time2aʱ���32���ش�
#define		M_ME_TD_1  	34		//��CP56Time2aʱ��Ĳ���ֵ, ��һ��ֵ
#define		M_ME_TE_1 	35		//��CP56Time2aʱ��Ĳ���ֵ, ��Ȼ�ֵ
#define		M_ME_TF_1	36		//��CP56Time2aʱ��Ĳ���ֵ, �̸�����
#define		M_IT_TB_1	37		//��CP56Time2aʱ����ۼ���
#define		M_EP_TD_1	38		//��CP56Time2aʱ��ļ̵籣���豸�¼�
#define		M_EP_TE_1	39		//��CP56Time2aʱ��ļ̵籣���豸���������¼�
#define		M_EP_TF_1  	40		//��CP56Time2aʱ��ļ̵籣���豸���������·��Ϣ
#define		M_FT_NA_1   42      //������ʵʩϸ����Ĺ�����Ϣ�ϱ����ͣ�pjhtest
#define		C_SC_NA_1  	45		//��������
#define		C_DC_NA_1  	46		//˫������
#define		C_RC_NA_1  	47		//����������
#define		C_SE_NA_1	48		//�趨ֵ����, ��һ��ֵ
#define		C_SE_NB_1	49		//�趨ֵ����, ��Ȼ�ֵ
#define		C_SE_NC_1	50		//�趨ֵ����, �̸�����
#define		C_BO_NA_1	51		//32���ش�
#define     C_SC_TA_1   58      //��CP56Time2aʱ��ĵ�������,104����
#define     C_DC_TA_1   59      //��CP56Time2aʱ���˫������,104����
#define     C_RC_TA_1   60      //��CP56Time2aʱ��ĵ��ڲ�����,104����
#define     C_SE_TA_1   61      //��CP56Time2aʱ����趨ֵ����, ��һ��ֵ,104����
#define     C_SE_TB_1   62      //��CP56Time2aʱ����趨ֵ����, ��Ȼ�ֵ,104����
#define     C_SE_TC_1   63      //��CP56Time2aʱ����趨ֵ����, �̸�����,104����
#define     C_BO_TA_1   64      //��CP56Time2aʱ���32���ش�,104����
#define		M_EI_NA_1	70		//��ʼ������
#define		C_IC_NA_1  	100		//���ٻ�����
#define		C_CI_NA_1  	101		//�������ٻ�����
#define		C_RD_NA_1   102		//������
#define		C_CS_NA_1  	103		//ʱ��ͬ������
#define		C_TS_NA_1  	104		//��������
#define		C_RP_NA_1  	105		//��λ��������
#define		C_CD_NA_1  	106		//��ʱ�������
#define		C_TS_TA_1   107     //��CP56Time2aʱ��Ĳ�������,104����
#define		P_ME_NA_1  	110		//����ֵ����, ��һ��ֵ
#define		P_ME_NB_1  	111		//����ֵ����, ��Ȼ�ֵ
#define		P_ME_NC_1   112		//����ֵ����, �̸�����
#define		P_AC_NA_1  	113		//��������
#define		F_FR_NA_1  	120		//�ļ���������
#define		F_SR_NA_1  	121		//�ڻ�������
#define		F_SC_NA_1  	122		//�ٻ�Ŀ¼, ѡ���ļ�, �ٻ��ļ����ٻ���
#define		F_LS_NA_1  	123		//���Ľ�,���Ķ�
#define		F_AF_NA_1  	124		//�Ͽ��ļ�,�Ͽɽ�
#define		F_SG_NA_1	125		//��
#define		F_DR_TA_1	126		//Ŀ¼
#define		C_SR_NA_1   200     //��������ʵʩϸ������л����ͱ�ʶ��pjhtest
#define		C_RR_NA_1   201     //��������ʵʩϸ���������ֵ�������ͱ�ʶ��pjhtest
#define		C_RS_NA_1   202     //��������ʵʩϸ�������������ֵ/�������ͱ�ʶ��pjhtest
#define		C_WS_NA_1   203     //��������ʵʩϸ��д����������ֵ/�������ͱ�ʶ��pjhtest
#define		M_IT_NB_1   206     //��������ʵʩϸ���ۼ������̸�������pjhtest
#define		M_IT_TC_1   207     //��������ʵʩϸ���CP56Time2aʱ����ۼ������̸�������pjhtest
#define		F_SC_NA_2   210     //��������ʵʩϸ���ļ��������ͱ�ʶ��pjhtest
#define		F_SR_NA_1_N 211     //��������ʵʩϸ�������������/�������ͱ�ʶ��pjhtest



//                             ����ԭ��                             ���		����
#define  	CAUSE_PERCYC		1		//���ڡ�ѭ��
#define  	CAUSE_BACK        	2		//����ɨ��
#define  	CAUSE_SPONT 		3		//ͻ��(�Է�)  <ɽ������Ϊ��������>
#define  	CAUSE_INIT 			4		//��ʼ��
#define  	CAUSE_REQ 			5		//������߱�����
#define  	CAUSE_ACT			6		//����
#define  	CAUSE_ACTCON		7		//����ȷ��
#define  	CAUSE_DEACT			8		//��ֹ����
#define  	CAUSE_DEACTCON		9		//��ֹ����ȷ��
#define  	CAUSE_ACTTERM		10		//������ֹ
#define  	CAUSE_RETREM		11		//Զ����������ķ�����Ϣ
#define  	CAUSE_RETLOC		12		//������������ķ�����Ϣ
#define  	CAUSE_FILE			13		//�ļ�����
#define  	CAUSE_INTROGEN 		20		//��Ӧվ���ٻ�
#define     CAUSE_REQCOGEN      37      //��Ӧ���������ٻ�
#define  	ERROR_TYPE			44		//δ֪�����ͱ�ʶ
#define  	ERROR_CAUSE			45		//δ֪�Ĵ���ԭ��
#define  	ERROR_ADDR			46		//δ֪��Ӧ�÷������ݵ�Ԫ������ַ
#define  	ERROR_INFO			47		//δ֪����Ϣ�����ַ

#define MAX_OAD_COLS_NUM (2)

// �������ֵ ת��ϵ����ʽ
typedef enum
{
    E_OOP_TO_REAL=0,  //�������ֵ ת��Ϊ ��ʵֵ
	E_REAL_TO_OOP,  //��ʵֵ ת��Ϊ �������ֵ
	E_DISPATCH_TO_REAL,  //����ֵ ת��Ϊ ��ʵֵ
	E_REAL_TO_DISPATCH,  //��ʵֵ ת��Ϊ ����ֵ
}CONVERT_TYPE_E;

// ң�����ݽṹ
//(�����ֵ��д�ŵ�λ�ü�data_bitλ������ң������λ��
//state_bitλ�����������Ƿ�Ҫ����)
typedef struct
{
	uint16  objAddr;	 //ң�ŵ��
	uint32	data_code;   //�����ֵ��д�ŵ�λ��
	uint8  	data_bit;	 //��ǰ���ݴ�ŵ�bitλ
	uint8   state_bit;	 //ң���Ƿ��λ���bitλ
}YX_DATA_CONFIG;

//ң���ϱ����ݽṹ
typedef struct
{
	uint16	objAddr;    //ң�ŵ��
	uint8	state_sign; //��ǰ״̬�Ƿ����仯��־1:�仯��0:�ޱ仯
	uint8	state;		//״̬
	uint8   btime[7];   //-���ݷ����仯��ʱ��-
}YX_DATA_SEND;

// ң�����ݽṹ
//ң��������Ϣ�ṹ��
typedef struct
{
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	char   *szObjName;    //��Ϣ��������
	uint32 nSignalType;   //���ͱ�ʶ
	int8 nReportRatio;    //��ǰ����������վ�ı���
	uint16 nDeadZoneAddr; //�仯����ֵ�Ķ����ַ
	float32 fDeadZoneValue; //�仯����ֵ
	uint8 nLogicId; //�߼���ַ�����ɣ�1�����0
    uint16 nInfoNum; //��Ϣ���  �����ɣ�0�������1��ʼ������Ϣ���Ϊ0xFFFFʱ����ʾȫ����Ϣ��
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ����OAD��
	uint32 nOadColsNum;  //������OAD����
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //������OAD
	uint8  nItemIndex;    //������ţ���0��ʼ��
	float32 fFullValue;  //����ֵ�����ڹ�һ��ң��ֵ��
}YC_DATA_CONFIG;

//ң��ʵʱ���ݽṹ�壨�����ϱ���
typedef struct
{
	float32 fNewValue;    //ʵʱ����
	float32 fHisValue;    //��ʷ����
	BOOL bSendFlag;       //�Ƿ��ϱ�
	BOOL bChangeFlag;     //�Ƿ�仯
	BOOL bHisValidFlag;   //��ʷ�����Ƿ���Ч
	uint8   btime[7];     //-���ݷ����仯��ʱ��-
}YC_DATA_SEND;

// ң�����ݽṹ
typedef struct
{
	uint16	objAddr;    //ң�ص��
	uint32	data_code;  //�����ֵ��д�ŵ�λ��
	uint8 	bSelect;	// ң��·��<ÿһ·��������·��--��--��-->
}YK_DATA_CONFIG;

//����������Ϣ�ṹ��
typedef struct
{
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	char   *szObjName;    //��Ϣ��������
	uint32 nSignalType;   //���ͱ�ʶ
	int8 nReportRatio;    //��ǰ����������վ�ı���
	uint16 nDeadZoneAddr; //�仯����ֵ�Ķ����ַ
	float32 fDeadZoneValue; //�仯����ֵ
	uint8 nLogicId; //�߼���ַ�����ɣ�1�����0
    uint16 nInfoNum; //��Ϣ���  �����ɣ�0�������1��ʼ������Ϣ���Ϊ0xFFFFʱ����ʾȫ����Ϣ��
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ����OAD��
	uint32 nOadColsNum;  //������OAD����
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //������OAD
	uint8  nItemIndex;    //������ţ���0��ʼ��
}EN_DATA_CONFIG;

typedef struct //ң���ϱ��������ݽṹ
{
	uint16 objAddr;         //-��Ϣ�����ַ������Ϊ��GB376.1�е����������+���ݱ�ʶ�İ�-
	uint32 data_code;		//-�����ֵ����-
}APP_PARA_CONFIG;

//������Ϣ�ϱ��������ݽṹ���ϱ�����ʱ��ĵ���ң���ϱ�
typedef struct
{
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	char   *szObjName;    //��Ϣ��������
	uint32 nSignalType;   //���ͱ�ʶ��������Ϣ��
	uint32 nFaultYxAddr;  //����ң�ŵ�ַ
	uint8  nReportNum;      //�ϱ�ң�����ݸ���
	uint16 aReportAddr[12]; //�ϱ�ң����Ϣ���ַ(ȡң������Ϣ���ַ�Ĵ��λ��)
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ��OAD��
	uint8  nItemIndex;    //������ţ���0��ʼ��
}FAULT_REPORT_CONFIG;

//������Ϣ�ϱ��������ݽṹ��ʵʱ�����ϱ�����ʱ��ĵ���ң���ϱ�
typedef struct //������Ϣ�ϱ��������ݽṹ���ϱ�����ʱ��ĵ���ң���ϱ�
{
	uint8       bfgchange;      //��λ��־
	uint16 		objAddr;		//��Ϣ���ַ
	uint8 		bstate;			//ң��״̬
	uint8       btime[7];       //��λ����ʱ��
	uint8       reportnum;      //�ϱ�ң�����ݸ���
	uint8       datatype;       //�ϱ�ң����������
	uint16      reportdata[240]; //�ϱ����ݻ�����
}FAULT_REPORT_SEND;

//104��Լ���յ�I֡��֡���
typedef struct
{
	uint16	fir:1;			//-I֡���ͱ�־-
	uint16	framenum:15;	//-֡���-
}__attribute__ ((packed)) RI_BIT;

typedef union SFP_R_NUM
{
	RI_BIT rbit;
	uint8 crtltype34[2];
}__attribute__ ((packed)) SFP_R_NUM_U;//-�������

typedef union SFP_S_NUM
{
	RI_BIT ibit;
	struct
	{
		uint16	fir:2;			//-S֡���ͱ�־-
		uint16	framenum:14;	//-��Ч-
	}__attribute__ ((packed)) sbit;
    struct
	{
		uint16	fir:2;			//-U֡���ͱ�־-
		uint16	staact:1;	    //-start act��־λ-
		uint16	stacon:1;	    //-start con��־λ-
		uint16	stoact:1;	    //-stop act��־λ-
        uint16	stocon:1;	    //-stop con��־λ-
        uint16	testact:1;	    //-test act��־λ-
		uint16	testcon:1;	    //-test con��־λ-
		uint16	framenum:8;	    //-��Ч-
	}__attribute__ ((packed)) ubit;
	uint8 crtltype12[2];
}__attribute__ ((packed)) SFP_S_NUM_U;//-�������

//104��Լ֡���ݽṹ
typedef struct
{
	uint8			head;		  //λ��0��68����ͷ
	uint8           len;          //λ��1�����ĳ���
    SFP_S_NUM_U   	sCrtlType12;  //λ��2��������1��2
	SFP_R_NUM_U     sCrtlType34;  //λ��4��������3��4
	uint8           bdatatype;    //λ��6�����ͱ�ʶ,1���ֽ�
	uint8           Vali;		  //λ��7���ɱ�ṹ�޶���,1���ֽ�
	uint8           CommCauseL;   //λ��8������ԭ��2�ֽڣ�����ǵ��ֽ�
	uint8           CommCauseH;   //λ��9������ԭ��2�ֽڣ�����Ǹ��ֽ�
	uint8           PubAddrL;	  //λ��10��������ַ��1�ֽڣ�����ǵ��ֽ�
	uint8           PubAddrH;	  //λ��11��������ַ��1�ֽڣ�����Ǹ��ֽ�
    uint8           *pdata;       //λ��12����Ϣ��
}__attribute__ ((packed)) S104FRAME_HEAD;


typedef union DA_SFP_S_NUM
{
    struct
	{
		uint16	base:8;			//-����-   �����Լ���ֽ���ǰ
		uint16	keyport:3;	    //-��Կ��ʶ��־λ-
		uint16	encry:1;	    //-���ܱ�־λ-
		uint16	none:2;	        //-����-
		uint16	from:2;	    	//-����ն����Ӷ���-
	}__attribute__ ((packed)) ubit;
	uint8 commtype[2];
}__attribute__ ((packed)) DA_SFP_S_NUM_U;//-��������-

//�������ܹ�Լ֡���ݽṹ
typedef struct
{
	uint8			headH;		  //0xEB����ͷ
	uint8           lenH;         //���ĳ��ȸ��ֽ�
	uint8           lenL;         //���ĳ��ȵ��ֽ�
	uint8			headL;		  //0xEB����ͷ
	DA_SFP_S_NUM_U  commtype;     //��������
	uint8           apptype;      //Ӧ������
}__attribute__ ((packed)) DASAFE_FRAME_HEAD;


//����2019��2�·������¹淶
typedef struct   //�����洢���ݽṹ
{
	uint16			bInfor;		  //��Ϣ���ַ
	uint16          dParaID;      //�����洢ID
    uint8   	    bRunType;     //�����������ͣ�0xaa�ɶ�д������0xbb��ʾֻ������
    uint8           bLen;         //��������
}__attribute__ ((packed)) PARA_INFO_CONFIG;

//end of ����2019��2�·������¹淶

//��������״̬
typedef enum
{
    TASK_CFG_UNINIT,              //δ��ʼ��
    TASK_CFG_MODIFY,              //�޸�
    TASK_CFG_MODIFY_DELAY,        //�޸���ʱ
    TASK_CFG_INITED,              //�ѳ�ʼ��
    TASK_CFG_REBUILD,             //�ؽ������ļ�
    TASK_CFG_PATCH,               //���������ļ�
} TASK_CFG_STATE;

#define MTR_MAX_NUM        	    2040				//�����������

//�����
typedef struct
{
    pthread_rwlock_t              rwLock;                     //��д�� ���̷߳���
    TASK_CFG_STATE                FileState;                 //����״̬
    int                           ModifyDelaySec;            //���������޸���ʱ
    time_t                        FunLastRunTime;            //�����ϴ��޸�ʱ��
    uint16                        MeterCount;                //����ܸ���
    OOP_METER_T                   MeterFile[MTR_MAX_NUM];    //ÿ�����ĵ���
}METER_FILE_T;

#endif

