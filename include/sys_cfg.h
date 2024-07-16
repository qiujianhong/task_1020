/*******************************************************************************
  Copyright (c) 2006-2007,�Ͼ��������������������ι�˾
             All rights reserved.
    ��������: sys_cfg.h
    ��    �ܣ�ϵͳ�����ļ�����
    �� �� �ˣ��ȱ�
    ����ʱ�䣺2007-10-24
    �汾��ʷ: 2007-10-24 ,v1.0 : New
*******************************************************************************/
#ifndef _SYS_CFG_H
#define _SYS_CFG_H

//begin of GB101\101 define 

/*�����ں��ն�����GB101_104Э��  ��Ϊ���������ֲ��
   �����������޸Ĵ����������GB101_104_EN��ѯ���Ӿ��� 2019��6��26��*/
   
#define GB101_104_EN            0  /*���й�Լͬʱ֧��DLT/T634.5 104�� DL/T634.5 101��Լ����ʹ��*/

#define PROTOCAL_101               1 //-101��Լ����-
#define PROTOCAL_104               2 //-104��Լ����-
#define PROTOCAL_376               3 //-GB376.1��Լ����-
#define PROTOCAL_698_45            4 //-GB698.45.1��Լ����-

#define NOMMAL_PROT_TYPE        PROTOCAL_698_45 //-δ���ò���ʱ��Ĭ�ϵ�Э������-

#define SOE_MAX_NUM             40  //soe֧���������
//----------------------����ѡ�񿪹أ�����4�����ܿ�����Ҫ�����õ�Ҫ���Ƿ�ſ�--------------
#define YC_CHANGE_SEND_EN           1        //-ң�����ݱ仯���͹���-
#define EN_CYC_SEND_EN              0        //-ͻ���͵������͹���-
#define YC_CYC_SEND_EN              0        //-ѭ���������ݹ���-
#define BACK_SCAN_EN                0        //-����ɨ�蹦��-
//-------------------end of ����ѡ�񿪹�------------------------------------------------------
//end of GB101\101 define 

#define CHONG_QING_FUN              0           //���칦��
#define JIANG_SU_FUN                0           //���չ���
#define FUJIAN_FUN                  0           //��������
#define HEBEI_FUN                   0           //�ӱ�����
#define SHANDONG_FUN                0           //ɽ������
#define HENAN_FUN                   0           //���Ϲ���
#define HUABEI_FUN                  0           //��������
#define HUBEI_FUN                   0           //��������
#define BEIJING_FUN                 0           //��������
#define SHANGHAI_FUN                0           //�Ϻ�����
#define GANSU_FUN                   0           //���๦��
#define SICHUAN_FUN                 0           //�Ĵ�����
#define LIAONING_FUN                0           //��������
#define NINGXIA_FUN                 0           //���Ĺ���
#define JIANGXI_FUN                 0           //��������
#define XINJIANG_FUN                0           //�½�����
#define ZHEJIANG_FUN                0           //�㽭����
#define HEILONGJIANG_FUN            0           //����������
#define HUNAN_FUN                   0           //���Ϲ���
#define SHANXI_FUN                  0           //ɽ������
#define SHANXI1_FUN                 0           //��������
#define JILIN_FUN                   0           //���ֹ���
#define MENGDONG_FUN                0           //�ɶ�����
#define TIANJIN_FUN                 0           //�����
#define ANHUI_FUN                   0           //���չ���

#define STD_698PRO_BETA	            1           //����698��Լ
#define STD_698PRO_PATCH			1			//����698��Լ����
#define DELGW2013                   0           //�������ô���
#define SECURITY_DEFEND				0			//������ȫ��������


#define HARDWARE_TYPE               4       //1:һ�ʹ��ն�;2:���ͽ����ն�;3:����208�ն�;4:�������ն�;9:II�ͼ�����

#define MODULARIZATION_TMN_EN       1           // ģ�黯�ն˹���

#if MODULARIZATION_TMN_EN == 1
#define MDL_RECOGNITION_EN          1           // ģ��ʶ����
#endif

#if HARDWARE_TYPE == 1
#define HARDWARE_IV_EN              1       //���ս��ɱ�I���ն�
#else
#define HARDWARE_IV_EN              0       //���ս��ɱ�I���ն�
#endif
#define TRAFFIC8_EN                 0           //8����ʹ��


#define GPRS_USB_VIRTUAL_NET_EN     0           //����USB�������ڵ�GPRSͨ�ŷ�ʽ
#define LINK_BREAKPOINT_EN          1           //Ӧ�÷�֡ʹ��

#define MET07_RT_DUMP_CURVE_MIN_EN  1           //07���ʵʱת������ʹ��
#define PROTOCOL_CHANGE             0           //376.1Э���698Э���л�
#define CH_AC485_CACHE_EN           0
#define DELIVER_DETECT_EN  			1  			//�ͼ칦��ʹ�ܣ���ĳЩ����ȥ�����ֳ�Ӧ�õĿ���
												//��Ա������Ժ��̨��������ش���
#define DELIVER_LOG_EN				0			//�ͼ�ʱ����־����

#define APN_ADAPT_CHANGE_EN         0           //APN����Ӧ����
#define JIAOCAIYOUHUA 				1           //���������Ż���Ŀ�궨�壬���ڴ������

#define CBTASK_NEW					1
#define CBTASK_OOP					1
#define ROUTER_UPDATE_FILE			"/mnt/router_update_file"
#define ROUTER_UPDATE_FILE_BACK	    "/mnt/router_update_file_back"

#if MODULARIZATION_TMN_EN == 1
#define MDL_UPDATE_FILE             "/data/app/mapManager/mdl_update_file"
#define MDL_UPDATE_FILE_BACK        "/data/app/mapManager/mdl_update_file_back"

#define JC_UPDATE_FILE              "/data/app/acMeter/jc_update_file"
#define JC_UPDATE_FILE_BACK         "/data/app/acMeter/jc_update_file_back"

#define JC_UPDATE_FILE_IC           "/data/app/acMeter/jc_update_file_ic"
#define JC_UPDATE_FILE_TOPO         "/data/app/acMeter/jc_update_file_topu"

#define MDL_CTL_FILE_NAME           "XLDZ_MODULE_YK"
#define MDL_YXYM_FILE_NAME          "XLDZ_MODULE_YXYM"
#define MDL_RS485_FILE_NAME         "XLDZ_MODULE_RS485"
#define MDL_PLC_FILE_NAME            "XLDZ_MODULE_PLC"
#define JC_FILE_NAME                "XLDZ_MODULE_JC"
#define MDL_CTL_UPDATE_FILE         "/mnt/ctl_update_file"
#define MDL_CTL_UPDATE_FILE_BACK    "/mnt/ctl_update_file_back"
#define MDL_YXYM_UPDATE_FILE        "/mnt/yxym_update_file"
#define MDL_YXYM_UPDATE_FILE_BACK   "/mnt/yxym_update_file_back"
#define MDL_RS485_UPDATE_FILE       "/mnt/rs485_update_file"
#define MDL_RS485_UPDATE_FILE_BACK  "/mnt/rs485_update_file_back"

#define MDL_PLC_UPDATE_FILE           "/mnt/plc_update_file"
#define MDL_PLC_UPDATE_FILE_BACK    "/mnt/plc_update_file_back"
#define MDL_TMP_UPDATE_FIEL            "/mnt/tmp_update_file"
#define MDL_TMP_UPDATE_FILE_BACK    "/mnt/tmp_update_file_back"

#define MDL_JC_UPDATE_FILE          "/mnt/jc_update_file"
#define MDL_JC_UPDATE_FILE_BACK     "/mnt/jc_update_file_back"

#endif

#define GET_DATA1_RELAY_EN            1            //�ٲ�һ������F129�Ժ�����ݣ��Ƿ���ô�͸�ķ�ʽ��Ŀǰֻ��F129�������̨���Կ��������⣩
#define REMOTE_VPN_EN               1           //�ն�Զ��VPN����
#define REMOTE_VPN_FILE             "^RemoteVpnInfo"        //�����ļ�
#define REMOTE_VPN_SCRIPT           "/etc/ppp/peers/vpn"    //VPN���Žű����ļ�

/*******************************************************************************
* �����ַ���ASCII������ձ�
* 0(0x30)~9(0x39)    !(0x21)    #(0x23)        %(0x25)    .(0x2E)        :(0x3A)    ?(0x3F)
* A(0x41)~G(0x47)    H(0x48)~N(0x4E)        O(0x4F)~T(0x54)        U(0x55)~Z(0x5A)
* a(0x61)~g(0x67)    h(0x68)~n(0x6E)        o(0x6F)~t(0x74)        u(0x75)~z(0x7A)
*******************************************************************************/
/*******************************************************************************
* ���¶�����Ҫ���ݵ�����Ӳ��ѡ����صĺ궨��
*******************************************************************************/
#define MINICURVE                   1           //��mini  curve ����

#define EXTEND_PARA                 1           //��������չ��������
#define EXTEND_DATA                 1           //��������չ���ݹ���
#define EXTEND_RAMDATA              1           //��������չRamDisk���ݹ���
#define HARDWARE_2012_EN            1           //2012���¹淶�ն˹���
#define EVENT_2012_EN               1           //2012���¹淶�¼�

#define EVENT_2014_EN               1           //2014���¹淶�¼�
#define EVENT_698_EN                0           //698���¼��ϱ�����
#define EVENT_2014_ENABLE           1           //�¼��ּ������Ƿ���Ч������ú�رգ����¼��ּ�������������Ч���¼��ּ��Ĵ��������Ȼ��Ч�����ڸù���Ŀǰ��û����ʽ�淶������Ĭ�ϲ����ã�
#define EVENT_2014_EVENTPARA_EN     0           //�¼��ּ��ж�ʱ�򣬳��������״̬�ֺ��Ƿ���Ҫ����F105��F106�Ĳ�������������Ƿ񳭱���ʱ�䣨���ڹ����ͼ�̨�壬����򿪣���������������򿪣�

#define SAL_PRTL                    0           //SSAL��ȫЭ��

#define BROADBAND_698_EXT_FUNC      0           //�����չ���ܼ�698���¼��ϱ�

//Э��ջ���ͣ�0-ͨѶģ���Э��ջ��1-�ն�Э��ջ
#define PRTLSTACK_TYPE_ID           (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2)

#define COMMPARA_BAK_EN             0           //ͨѶ�������ݹ��ܣ������ܹ�ͨ��F3�ָ�����ͨѶ������

#define TMNHALT_METHOD              1           //�ն�ͣ�ϵ�������

#define GPRS_ETH_ALL_OLINE          1           //GPRS����̫��ͬʱ����

#define	LONG_NEW_APN                0           //�������ֵ����ڴ洢���ȴ���16��APN�����ա����ࡢɽ�����㽭�����մ�ʱ��Ҫ�����޸�

//start Common Problem
#if HARDWARE_TYPE == 4
#define PLC_COMM_TYPE		        0           //�ز���·�ɰ廹��ͨ���忪��ѡ��,1Ϊ������64Mͨ���壬0Ϊ·�ɰ�
#endif
//end Common Problem
#define PULSECONSTS_6400			1			//���峣��3200/6400����	(�ر�Ҫ��ʱ�ſ�)

#define AUTO_AC_TYPE				1			//�ο���������,ʵ����������100V/��������220V����Ӧ�Ĺ��ܣ�
#if AUTO_AC_TYPE==1 && (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)
#undef CONFIG_AC_33
#undef Volt57p7Enable
#endif

#define ESAM_ENCRYPT_EN             (HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)    //ESAM����ʹ�ܿ���
#define ESAM_ENCRYPT_EN_2012        (ESAM_ENCRYPT_EN == 1)    //2012��ESAM����ʹ�ܿ���
#define BOOTSCREEN                  1               //��ʾ����ʱ����ʾ��̬����
#define SpeBOOTSCREEN               BOOTSCREEN      //����ʱ������������ʾ����
#define GUI_SYSVERSION_EN            1                //0��ʾ����ʾϵͳ�汾�����Ի����Լ�������Ϣ����ҳ�еı�IP����ʾ�ճ���ʾ
                                                    //1��ʾȥ�����Ի����Լ�������Ϣ����ҳ�еı�IP����ʾ������ʾϵͳ�汾����

#define DEL_DAYDATA_EN              1               //С��5M ��ʱ��ɾ��һЩ���ݣ���ɾ���������ڵ�����
#if DEL_DAYDATA_EN ==1
#define DEL_DATA_CAP                (5*1024*1024)   //С��5M ��ʱ��ɾ��һЩ���ݣ���ɾ���������ڵ�����
#define DEL_DAY_CAP                 1               //����С��DEL_DATA_CAP��ʱ��һ��ɾ��������
#endif

#define TH208_RTU                    (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)            //TH208�ն˵ĺ�
#define ZJ_PRTL                        0            //�㽭��Լ
#define GB_PRTL                        1            //04�����Լ

#define MANUFACTURER                "xxxx"       //ע: 376�ն���������Ľ�������ԼҪ��֧��F161�·����̴��ţ����δ�����ܻ᲻������ʹ�ú�MANUFACTURER���峧�̴���

//�ն�Ӳ���汾��ASCII    4
#if HARDWARE_TYPE == 1            //һ���ն�
#define HW_VIRSION            "XA13"
#elif HARDWARE_TYPE == 2        //�����ն�
#define HW_VIRSION            "XB21"
#elif HARDWARE_TYPE == 3        //�����ն�
#define HW_VIRSION            "T208"
#elif HARDWARE_TYPE == 4        //�������ն�
#define HW_VIRSION            "JZQ1"
#elif HARDWARE_TYPE == 9        //�������ն�
#define HW_VIRSION            "JZQ2"
#else
#define HW_VIRSION            "T208"
#endif

#define GetMewMeter_EN              1           //�ѱ���

#define TD_SCDMA                    1            //TD-SCDMA����
#if TD_SCDMA == 1
#define TDSCDMA_ID                    9            /*����վͨѶͨ������ TD_SCDMA*/
#define TDSCDMA_GSM_ID                10            /*����վͨѶͨ������ TD_SCDMA_GSM*/
#endif
#define FASTER_RADIO_EN             1           //����ֱͨ��̨����
#if FASTER_RADIO_EN == 1
#define FASTER_RADIO_ID             12
#endif

#define PRTL_STACK_EXCHANGE_EN          1            //GPRS����Э��ջ������Э��ջ�л����ܣ�ͨ����ʾ����̨��Լ֡���л�Э��ջ(�������ն�)

#define GPRS_RETRY_EN                0            //GRPSģ���ڽ������ݱ����У����SIM������������յ�URC_CLOSE�������ϱ����߽��յ���վ���ص��������ݣ����պ�������-1
                                                //ͨ����������´�ģ�鴮�ڣ���������ģ��
                                                //�Ĵ����ܼ����������л������ö˿ں�ͨ�ų����쳣��һֱ�������´򿪴��ڣ�δ������ģ��
                                                //�ú�ʹ�ܺ�������γ������´򿪴��ڣ�ͨ���Բ��ɹ����������л�IP�Ͷ˿�

#if CBTASK_NEW == 1
#define MET07_CBDAY_TO_DAYFIX        1            //07��֧�ִӱ���ֱ�ӻ�ȡ��һ�ն���������Ϊ�ն�������
#define MET07_JIESR_TO_MONFIX        0            //07��֧�ִӱ���ֱ�ӻ�ȡ��һ������������Ϊ�¶�������
#define MET97_SHANGRI_TO_MONFIX        0            //97��֧�ִӱ���ֱ�ӻ�ȡ������Ϊ�¶�������
#define MET07_CBDAY_TO_MONFIX        1            //07��֧�ִ�1��ǰһ������ת��Ϊ���¶�������
#define MET07_RTDATA_TO_MONFIX        0            //07��֧��ʵʱ����ת��Ϊ��������
                                                //���Ժ������¶���ĺ�ͬʱ����,������ʵʱ���ݴ�,�������ݳ������ٲ���
#endif

#define DAY_FREEZE_MINITE           0           //�ա��¶���ʵʱת�浽��һ��/�µ�ʱ��:0ʱ0����


#define FAC_MODE                    1            //����ģʽ
#define DISTRITE_SCALE              50          //��С̨����ģ�ֽ���
#define STYLE_NUM_ENABLE            1            //�Ƿ�֧�ִ���š�С���
#define MEM_CATA_NEW                0//(STYLE_NUM_ENABLE == 1 && HARDWARE_TYPE == 4)    //�µĴ洢����
//#if MEM_CATA_NEW == 1
#define	WIDE_COUNT			        16		    //���������
#define NARROW_COUNT		        16		    //С�������
//#endif

#define PARACLEAR_WITH_AC			1			//������ʼ��ʱ���ɵĲ���Ҳ��ʼ��
#define DefualtSetPLCInter		    0	        /*��������ֳ��ķ�����Ϊ���԰汾�ͳ����汾��
											        �˴�Ϊ�ز����������ݴ˺궨���������*/
#define COMM_I_AUTO_SWITCH_EN		0			//485-1��ά���ͳ����Զ��л�����,Ĭ�Ϲر�

#define AUTOTASK_NEW				1

#define DCD_FUN						TH208_RTU	//����DCD ģʽ
#define COMM_DEBUG_PRO				1			//ͨѶ������Ϣ��ʾ
#define WORK_MODE					1			//����������ģʽ

#define NAND_SPECIAL				1
#define WR_READ_EN					1

//��վ�������ý��ɲ�����ʱ��ͬ�������ɱ�645�ĵ�ַ����
#define ACADDR_FROM_PRTL            (HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4|| HARDWARE_TYPE == 9)

#define USB_CLEAN_ACDATA			1			/*ͨ������u�̼�����е�clearjcdata.tar.gz�ļ��������ý�������*/


#define PLC_AMR_EN		    		(HARDWARE_TYPE == 4)   		//��ѹ��������ʹ��
#define PHOTO_PATH					"/mnt/data/photo"
#define ETH_MODEL					1//(HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4)		//��̫��ģ��ʹ��
#define ETH_IP_THREAD				ETH_MODEL					//������̫���ŵ�IP���߳�
#define CLEAR_MTR_HISDATA			1			//�������ò������ʱ���Ƿ���Ҫ����ò��������ʷ���ݣ�Ĭ���ǹرյ�
#define proAdvanceEN				1			//���������Ľ����ӵĹ�Լ(֧���˹㲥(��������0x9999 �ն˵�ַ0)���ú��в�)
#define proAdvanceEN2				proAdvanceEN//���������Ľ�(�ڶ�������:��������������0x9999֧��,�������ն�ͨѶ������ʼ������)
#define BAT_RMT_EN					TH208_RTU ||(HARDWARE_TYPE == 2)//ͨ��ң��λ���ж��ն˵���Ƿ����
#if BAT_RMT_EN == 1
    #if (HARDWARE_TYPE == 2)
	#define	 BAT_HARDWARE_POS		0x00000010	//��ر�ʶλ
	#else
	#define	 BAT_HARDWARE_POS		0x00000100	//��ر�ʶλ
	#endif
	#define  BAT_POS			    0x08		//Tmn_Kg_Status_ID�У���ط���D3λ
#endif

#if HARDWARE_IV_EN ==1
#define	 BAT_HARDWARE_POS		0x80000000	//��ر�ʶλ
#define  BAT_POS			    0x08					//Tmn_Kg_Status_ID�У���ط���D3λ
#endif

#define PROTECT_HW				    (HARDWARE_TYPE == 2)		//Ӳ��������繦��
#if PROTECT_HW == 1
#define	PROTECT_KEY				    0x01	//������
#define	CHAOBIAO_KEY			    0x02	//����
#endif

#define DOUBLE_CHANNEL_EN           0           // ˫ͨ��ʹ��

#define GUI_CLEAR_MTRDATA			1			//��ʾ�������ò�����ʱ����Ҫͨ��IPC�������������
#define GUI_CLEAR_MTR				0xA0		//�ڽ����ֶ�����485�������ʱ�����485����
#define GUI_CLEAR_DATA				0xA1		//���ݳ�ʼ��
#define GUI_CLEAR_AC_DATA			0x91		//�彻������

#define COMM_PROTECT				1			//��ͨѶ��������
#define PASSWORD_16					1			//16�ֽ����빦��

#define COMM_RETRY					1			//
#if COMM_PROTECT == 1
    //ÿ����㽫������Ч֡��������0,ÿ��6:00,�ж���6Сʱ��û���յ���Ч֡,���������նˣ�reboot��
    #define COMM_6_HOUR_IDLESEC        21600
    //ÿ��8�㽫������Ч֡��������0,ÿ��18:00,�ж���10Сʱ��û���յ���Ч֡,���������նˣ�reboot��
    #define COMM_10_HOUR_IDLESEC    36000

    #define PRT_PRINT(x...) ({ if (getenv("DEBUG_PROTECT") != NULL)  printf(x);})
    #define COMM_PROTECT_LOG        "/mnt/log/protect_comm.log"
#endif

#define DATA1F25_PULSE_VLUE         1           //1������F25�������Ϊ����ʱ�Ƿ��ֵ

/********************************************************************************
����ģ�������ļ�:
9260�ն��У�32·��������ȫ��Ϊң�š����壬�Žڵ㡢���ܼ���У�����β�ǵȿ�������8·�����Ŀ�����
DEFAULT_JLM_POS��JLM_HARDWARE_POS֮��Ĺ�ϵ:
JLM_HARDWARE_POS����Ӳ��������б仯����ʾ�Žڵ���8·�ϵ���һ·
���Ӳ�������ڵ�һ·��Ϊ0x01���ڶ�·Ϊ0x02���Դ����ƣ�һ����������Ӳ���ĵ�6·����0x20

DEFAULT_JLM_POS���Žӵ���ң���е�λ��
(��Ϊ04��Լ��ʵ����û���Žӵ�ĸ����Ϊ�˺������ն�Ӳ����ƥ�䣬���������Žӵ�ĸ��ʵ���϶�04��Լ����������������һ·ң��
���㽭��Լ�У���û�и����⣬�����4·ң�Ż�����������1·�Žӵ㣬��ô�ն��й���4·ң��+1·�Žӵ�)
��04��Լ�У���������ң��·���������ۼӵģ�����Ѿ�������4·ң�ţ��ڼ����Žӵ㣬��ô����5·ң��
�������DEFAULT_JLM_POS = 3����ô���ǽ��Žӵ�ı�λ��Ϣ��ʾΪ��3·ң�ŵı�λ��Ϣ
ԭ��3��4·ң�ŵı�λ��Ϣ����ʾΪ4��5
�̽��Žӵ㣬ʹ�ù�Լ�ٲ�Ӧ���ǵ�3·ң�ŷ����˱�λ;�̽ӵ�3·ң�ţ���Լ�ٲ��4·ң�ŷ����˱�λ;1��2·ң�ŵ�״̬���Ǻ�ԭ��һ��
һ���������Žӵ��λ���ڵ�8·��Ҳ���Ƕ̽��Žӵ�󣬵�8·ң�ŷ����˱�λ;�������Ͳ���Ӱ��ǰ���ң�š�
����ն�Ӳ��û���Žӵ㣬���Žӵ��λ����Ϊ����8С��17��ֵ���ɣ���Լ����ʾ�Ͷ�������

����Ӳ�������ȷ������JLM_HARDWARE_POSֵ��DEFAULT_JLM_POS��Ӧλ��ң��״̬���ܱ��ɼ���
*******************************************************************************/
#if HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
    #define  JLM_HARDWARE_POS        0x00000010    //�����ŵ�Ӳ��λ��
    #define  FHJ_HARDWARE_POS        0x00000000    //��������Ӳ��λ��
    #define  DEFAULT_JLM_POS        8            //Ĭ�ϼ�������ң���еĵڼ�·�����ݲ�ͬ�����û���Ҫ����� ��1��ʼ
#elif HARDWARE_TYPE == 2
    #define  JLM_HARDWARE_POS        0x00        //�����ϼ����ŵ�λ�� 0x10 ��ʾ����չ��ĵ�5·
    #define  FHJ_HARDWARE_POS        0x00000040    //��������Ӳ��λ��
    #define  DEFAULT_JLM_POS        9            //Ĭ�ϼ�������ң���еĵڼ�·�����ݲ�ͬ�����û���Ҫ����� ��1��ʼ
    #if PROTECT_HW == 1
        #define  PRJ_HARDWARE_POS    0x00000080        //�ӽ�������Ӳ��λ��
        //#define  CB_HARDWARE_POS    0x00001000        //�������Ӳ��λ��
    #endif
#else
    #define  JLM_HARDWARE_POS        0x00100000    //�����ϼ����ŵ�λ�� 0x10 ��ʾ����չ��ĵ�5·
    #define  FHJ_HARDWARE_POS        0x00000000    //��������Ӳ��λ��
    #define  DEFAULT_JLM_POS        8            //Ĭ�ϼ�������ң���еĵڼ�·�����ݲ�ͬ�����û���Ҫ����� ��1��ʼ
#endif

/*******************************************************************************
    ң���������ã����ĳλ��ң�Ż������壬����Ӧλ��1��
    ���������1��2·Ϊ���壬��PULSE_BASEΪ0x03�������չ��7��8·Ϊ���壬��PULSE_EXTENDΪ0xC0��ң����������
    ע����ǲ�Ҫ��ĳһλͬʱ��Ϊ1(����ң��Ҳ������)��������������
    PULSE_BASE        �����������������
    PULSE_EXTEND    ��չ�������������
    SIG_BASE        ������ң���������
    SIG_EXTEND        ��չ��ң���������
    PULSE_MAX_NUM   ϵͳ��������(�����������,ע�ⲻ�����������)����,���ӱ��1~8
    YAOXIN_MAX_NUM  ϵͳ���ң�ŵ�(�����������)����,���ӱ��1~8
*******************************************************************************/
#if HARDWARE_TYPE == 1            //һ���ն�
    #if HARDWARE_IV_EN == 1
    #define PULSE_BASE             0xFF
    #define PULSE_EXTEND        0x00
    #define SIG_BASE             0xFF
    #define SIG_EXTEND            0xFF
    #define PULSE_MAX_NUM         8
    #define YAOXIN_MAX_NUM         16
    #else
    #define PULSE_BASE         0xFF
    #define PULSE_EXTEND    0x00
    #define SIG_BASE         0x00
    #define SIG_EXTEND        0x7F
    #define PULSE_MAX_NUM     8
    #define YAOXIN_MAX_NUM     8
    #endif
#elif HARDWARE_TYPE == 2        //�����ն� 8~16λ ǰ2mc��4yx
    #define PULSE_BASE             0x00
    #define PULSE_EXTEND        0x03
    #define SIG_BASE             0x0F
    #define SIG_EXTEND            0x00
    #define PULSE_MAX_NUM         2
    #define YAOXIN_MAX_NUM         4
#elif HARDWARE_TYPE == 3        //�����ն�
    #define PULSE_BASE             0x0C        //Ĭ�ϻ�����3��4·Ϊ����
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x03        //Ĭ�ϻ�����1��2·Ϊң��
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         2
    #define YAOXIN_MAX_NUM         2
#elif HARDWARE_TYPE == 4        //�������ն�
    #define PULSE_BASE             0x00        //
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x0F        //Ĭ�ϻ�����1��2��3��4·Ϊң��
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         0
    #define YAOXIN_MAX_NUM         4
#elif HARDWARE_TYPE == 9        //II�ͼ�����
    #define PULSE_BASE             0x00        //
    #define PULSE_EXTEND        0x00        //
    #define SIG_BASE             0x01        //Ĭ�ϻ�����1·Ϊң��
    #define SIG_EXTEND            0x00        //
    #define PULSE_MAX_NUM         0
    #define YAOXIN_MAX_NUM         1
#else
    #define PULSE_BASE             0x3C        //Ĭ�ϻ�����3��4·Ϊ����
    #define PULSE_EXTEND        0x00        //Ĭ����չ��3��4·Ϊ����
    #define SIG_BASE             0xC3        //Ĭ�ϻ�����1��2·Ϊң��
    #define SIG_EXTEND            0x00        //Ĭ����չ��1��2·Ϊң��
    #define PULSE_MAX_NUM         4
    #define YAOXIN_MAX_NUM         4
#endif

#if HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4        //261
    #define AC261_CALC_EN           1           //261���ý���ʹ��,�ñ�־�򿪣�ͬʱ���AC_CALC_EN
    #define AC_CALC_EN              1           //���ý���ʹ��
    #define HR_CALC_EN              1           //����г��ʹ��
#elif HARDWARE_TYPE == 9
    #define AC261_CALC_EN           1           //261���ý���ʹ��
    #define AC_CALC_EN              1           //���ý���ʹ��
    #define HR_CALC_EN              0           //����г��ʹ��
#elif HARDWARE_TYPE == 1
    #define AC261_CALC_EN           0           //261���ý���ʹ��
    #define AC_CALC_EN              1           //���ý���ʹ��
    #define HR_CALC_EN              1           //����г��ʹ��
#elif HARDWARE_TYPE == 2
    #define AC261_CALC_EN           0           //261���ý���ʹ��
    #define AC_CALC_EN              1           //���ý���ʹ��
    #define HR_CALC_EN              0           //����г��ʹ��
#else                                             //a11
    #define AC261_CALC_EN           0           //261���ý���ʹ��
    #define AC_CALC_EN              0           //���ý���ʹ��
    #define HR_CALC_EN              0           //����г��ʹ��
#endif

#define RELAYAC_EN                    AC261_CALC_EN    //�Ƿ���������ý��ɱ��·��м�����,III���뼯����

#if 1
    #define RRLAYAC_COMM            3            //��͸���ý��ɵ�ʱ����̨��Ҫ�·��Ķ˿ں�
    #define RELAYAC_PORT_ID            4009        //������ACSampling���̼�ͨ�ŵĶ˿ں�
    #define CMDID_WRITE                0x04        //д����
#endif

//�������޹�Ͷ�п���ʹ�ܣ�����Խ��ɣ���ǰ��AC_CALC_EN���
//�޹�Ͷ�й��ܺͿ��ƹ��ܳ�ͻ�����Կ��ƹ��ܱ���ر�
#define CAPACITOR_GRP_NUM           4           //����������
#define CAPACITOR_NUM       16             //�������ò�ѯ�ն�֧�����õĵ���������
#if AC_CALC_EN == 1 && CONTROL_EN == 0
    #define CAPACITOR_CTRL_EN       1             //��ǰ�κ�ʱ��ر��޹�Ͷ��
#else
    #define CAPACITOR_CTRL_EN       0             //��ǰ�κ�ʱ��ر��޹�Ͷ��
    #define CAPACITOR_CTRLPARASET_EN       1             //��������Ͷ��ʱ�����ն���������ݲ���
#endif

#if HARDWARE_TYPE == 1        //һ���ն�
    #if HARDWARE_IV_EN == 1
    #define TURN_NUM                    8            //�ִ�
    #else
    #define TURN_NUM                    4            //�ִ�
    #endif
#elif HARDWARE_TYPE == 2    //�����ն�
    #define TURN_NUM                    4            //�ִ�
#else
    #define TURN_NUM                    2            //�ִ�
#endif

#if AC261_CALC_EN == 0 && AC_CALC_EN == 1
#define OS_JCTASK_EN             1
#else
#define OS_JCTASK_EN             0
#endif

#define TTYS0 0
#define TTYS1 1
#define TTYS2 2
#define TTYS3 3
#define TTYS4 4
#define TTYS5 5
#define TTYS6 6

#if (HARDWARE_TYPE == 1)
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3
    #define PIPE2 254
    #define PIPE3 TTYS6
    #define PIPE4 254
    #define PIPE5 254
#elif (HARDWARE_TYPE == 2)
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
#elif HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3//�¹�����ά��485���óɿ��Գ������У��ڣ������໥�л�
    //#define PIPE1 254
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
#elif HARDWARE_TYPE == 9
    #define PIPE0 TTYS2
    #define PIPE1 TTYS3//
    #define PIPE2 254
    #define PIPE3 254
    #define PIPE4 254
    #define PIPE5 254
    #define PIPE6 TTYS6
#endif

/*******************************************************************************
*ϵͳ֧�ֵ�485����ͨ������
*���Ϊ1����ʾϵͳ֧��һ��485����ڣ�
*���Ϊ2����ʾϵͳ֧������485�����
*******************************************************************************/
#define CB_PIPE_NUM                    6            //ƽ̨���3������ͨ����Ҫ�޸ģ��ĸ�ͨ��û�У��ر�����ĺ궨��
#define PIPE0_ONOFF                 1           //ͨ��0  ������
#define PIPE1_ONOFF                 1            //ͨ��1,  ����Ҳȫ������
#if HARDWARE_IV_EN == 1
#define PIPE2_ONOFF                 1            //ͨ��2   can0
#else
#define PIPE2_ONOFF                 0            //ͨ��2   ������
#endif

//#if OS_JCTASK_EN == 1                           //ͨ��3   �ӽ��ɱ�
#define PIPE3_ONOFF                 1
//#else
//    #define PIPE3_ONOFF             0
//#endif
#define PIPE4_ONOFF                 1  //ͨ��4�������
#if HARDWARE_IV_EN == 1
#define PIPE5_ONOFF                 1  //ͨ��2   can1
#else
#define PIPE5_ONOFF                 0
#endif

#if HARDWARE_TYPE == 1
#define WH_485_MAX_NUM              1           // ר��I��ֻ��һ·�ڲ�����ͨѶ��
#else
#define WH_485_MAX_NUM              2           // ά��485���������
#endif

#define PORT_PRTL_AC                1           //��վ�·����ɲ�����ʱ��ʹ�õĶ˿ں�
#define PORT_PRTL_485_1             2           //��վ�·���1·485ʱ��ʹ�õĶ˿ں�
#define PORT_PRTL_485_2             3           //��վ�·���2·485ʱ��ʹ�õĶ˿ں�
#define PORT_PRTL_PULSE             11          //��վ�·������ʹ�õĶ˿ں�
#define PORT_PRTL_BROAD_PLC         28          //��������ز�ʱʹ�õĶ˿ں�
#define PORT_PRTL_PLC               31          //��վ�·��ز�������ʱ��ʹ�õĶ˿ں�

#define PIPE_METER_485_1            1           //����ģ�鳭����1·485ʱ��ʹ�õ�ͨ����
#define PIPE_METER_485_2            2           //����ģ�鳭����2·485ʱ��ʹ�õ�ͨ����
#if OS_JCTASK_EN == 0
#define PIPE_METER_AC               3           //����ģ�鳭�����ý���ʱ��ʹ�õ�ͨ����
#else
#define PIPE_METER_AC               4           //����ģ�鳭�����ý���ʱ��ʹ�õ�ͨ����
#endif
#define PIPE_METER_PULSE            5           //����ģ�鳭�������ʹ�õ�ͨ����
#define PIPE_METER_AC_INNER         3           //���ý��ɶ˿ں�(ר��I�ͽ��ɱ���III�͡�������)
#define PIPE_METER_BROAD_PLC        28          //��������ز�ʱʹ�õĶ˿ں�
#define PIPE_METER_PLC              31          //����ģ�鳭���ز�������ʱ��ʹ�õ�ͨ����

#define PIPE1_485_DEFAULT_CB           1         //485 I  ��Ĭ��Ϊ�����
#define PIPE2_485_DEFAULT_CB           0         //485 II ��Ĭ��Ϊά����

/********************************************************************************/


//�����������ݲ����޸�
#if HARDWARE_TYPE == 9
#define NO_VIR_MTR_MAX_NUM            192                            //�������ܼ���������������(���ز�������)
#define MTR485_MAX_NUM                192                            //485������ĸ���(ʼ��Ϊ64)
#elif HARDWARE_TYPE == 4
#define MTR485_MAX_NUM                64                            //485������ĸ���(ʼ��Ϊ64)
#define NO_VIR_MTR_MAX_NUM            3500                        //�������ܼ���������������
#else
#define MTR485_MAX_NUM                64                            //485������ĸ���(ʼ��Ϊ64)
#define NO_VIR_MTR_MAX_NUM            64                        //�������ܼ���������������

#endif


//#define MTR_MAX_NUM                    (NO_VIR_MTR_MAX_NUM    + 16)    //�����ܼ���������������

//ϵͳ485����������ý��ɼ���������
#define MTRPLS_MAX_NUM              8                                        //������������
#define AC_MTR_NO                      (NO_VIR_MTR_MAX_NUM + 1)                // ���ɲ������
#define PULSE_MTR_NO_BEGIN              (AC_MTR_NO+1)                              // �����������ʼ��

#define VIR_BEGIN                   (PULSE_MTR_NO_BEGIN + 8)    //�ܼ�����ʼ���, ֻ��04��Լ��Ҫ��65��ʼ����ͨ������1~64
#define    REPEATERS_MAX_NUM              64
#define    JOIN_MAX_NUM                3                            //�������������������������
#define    MTRVIP_MAX_NUM                20                            //�ص㻧����
#define DataDir_ROUTE               16                            //ϵͳ�ֵ䶨��֧�ֵ�����ܼ�����
#define CtrlGrp_ROUTE                  8                              //�ܼ�����


#define MTR_MAX_NUM                    (VIR_BEGIN + CtrlGrp_ROUTE) //���в�������������

#define MEMFILE_MTR_NUM                (MTRPLS_MAX_NUM + CtrlGrp_ROUTE + 1) // �ա��¡����߶�����ļ���ͷ�в����������
                                                                         // �������ɡ����塢�ܼ���


#define DEFAULT_FEE_NUM             4                            //Ĭ�Ϸ�����


#if HARDWARE_TYPE == 1 || HARDWARE_TYPE == 2 || HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4 || HARDWARE_TYPE == 9
    #define LMT_CDMA_ONLINE         0            //����CDMA���������߹���
    #define TIME_CTRL_ON_LINE       1            //ʱ�����߹���
    #define NET_FLUX_CTRL           1            //��������������(��������698��Լֻͳ�Ʋ�����)
    #define NET_FLUX_CTRL_MON          1            //��������������
    #define APP_CTRL_ON_LINE        0            //Ӧ�ò��������
    #define GPRS_FLUX_CTRL_OFF      0           //GPRS�������ƹر�-�ͼ����ʱ̨���������������ó�1�����
#else
    #define LMT_CDMA_ONLINE         0            //����CDMA���������߹���
    #define TIME_CTRL_ON_LINE       0            //ʱ�����߹���
    #define NET_FLUX_CTRL           0            //��������������(��������698��Լֻͳ�Ʋ�����)
    #define NET_FLUX_CTRL_MON          0            //��������������
    #define APP_CTRL_ON_LINE        0            //Ӧ�ò��������
    #define GPRS_FLUX_CTRL_OFF      0           //GPRS�������ƹر�-�ͼ����ʱ̨���������������ó�1�����
#endif

#if NET_FLUX_CTRL_MON == 1
//�ն˳�ʼ���󣬻Ὣ����Ĭ��Ϊ0xffffffff,������վ�ٲ�ʱ����Իط���
//Ĭ��һ��50M����������ͨѶ��ʹ�ã����Է�ֹ��������
//500M������һ�����㹻�ˣ�������ֲ����õ���� 500 * 1024 * 1024 = 52,428,8000
#define MAX_FLUX_MON                524288000L
#define MAX_FLUX_PLUS_MON            10485760L               //10M���������������ܣ�������������10M�����ߣ����������ţ�
#define TMN_COMM_LIMIT_EXT          1
#define TMN_COMM_LIMIT_FILE         "^OverFluxLmtTest"
#endif
/*******************************************************************************/

#if (DELIVER_DETECT_EN == 1)
    #if JIANG_SU_FUN == 1
    #define POWER_CUT_SAFE             1            //�ϵ籣�繦��ѡ�����ͼ�汾�н��鶨��Ϊ0���Ӿ���
    #define DEFAULT_PW_PARA            99900000000LL
    #else
    #define POWER_CUT_SAFE             1            //�ϵ籣�繦��ѡ�����ͼ�汾�н��鶨��Ϊ0���Ӿ���
    #define DEFAULT_PW_PARA            100L
    #endif
#else
    #define POWER_CUT_SAFE             1            //�ϵ籣�繦��ѡ�����ͼ�汾�н��鶨��Ϊ0���Ӿ���
    #define DEFAULT_PW_PARA            99900000000LL
#endif

#define CB_INTERVAL                 2            //Ĭ��485��������-�ȶ���2���ӣ����������������������δȷ���ݶ�2����

#define ANOLOGY_EN                    0            //ֱ��ģ����ʹ��

//����ģ��궨��
#define OS_RELAYTASK_EN              1
#define OS_METLGR_EN                 1
#define OS_METLGR2FRAME_EN           1
#define OS_METDLT645_EN              1
#define OS_METABB_EN                 1
#define OS_METEMAIL_EN                1
#define OS_METWS20_EN                1
#define OS_METHND_EN                 1
#define OS_METHT_EN                  1
#define OS_METEDMI_EN                1
#define OS_METJSBLACK_EN             0              //�����ֵ�г�����ֶ�Ӧ������
#define OS_METDSSD25_EN              1
#define OS_JSSIMULATE_EN             1
#define OS_CQINTELL_EN               0              //�����ֵ�ȱ�ֻ�������
#define OS_ZJSIM_EN                  0              //�����ֵ�ȱ�ֻ����㽭ģ���
#define OS_METHR_EN                  1
#define OS_METZMC_EN                 1
#define OS_METZZ_EN              ZHENGZHOU_FUN      //֣�ݱ�,��ZHENGZHOU_FUN��һ���
#define OS_METFJ_EN                  1
#define OS_METCBH_EN                 1
#define OS_METSTD2007_EN            1           //2007���Լʹ�ܿ���
#if HARDWARE_TYPE == 1
#define OS_JC2007TASK_EN            1           // 2007��Լ����ʹ��
#else
#define OS_JC2007TASK_EN            0           // 2007��Լ����ʹ��
#endif
#define OS_METDLMS_EN               1           //DLMS���ܱ�
#define OS_SMARTBREAKER_EN          0          //����I�ͼ�����֧�����ܶ�·��ͨ��Э�鹦��

/*******************************************************************************
* ��ʾģ������
* �޸���ʾģ��ֵ��Ҫ��Ӧ�޸�build.make�ļ�
*******************************************************************************/
#define GUI_FUN_EN                    (HARDWARE_TYPE != 9)           //GUI����ʹ��
#define GUI_LOGON_LAYER              0x000f0000
#define MAX_TRUNDISP_CNT              180            //����������
#define NEW_GW_GUI_APPEND           1           //�¹�����������

#if NEW_GW_GUI_APPEND == 1
#define GUI_RELAY_PN_FILE           "^GuiRelayPnInfo"  /*����㳭�Ĳ������
                                                        *��1���ֽ�Ϊ�㳭������־0x55��Ч,0xBBΪ�㳭ȡ��̬
                                                        *����Ϊ0xAA
                                                        *��2��3���ֽ�ΪҪ�㳭����Ŀ
                                                        *��4��5���ֽ���ΪҪ�㳭������,��6Ϊ�㳭�ɹ���־
                                                        *�����ظ���4��5��6���ֽڸ�ʽ
                                                       */
#endif


//����os����ϵͳ·��
#if HARDWARE_TYPE == 1            //һ���ն�
    #define DRIVER_BAOMI            0             //�Ƿ��ȡ�������� 0:û�б��ܼ� 1���б��ܼ�(231����) 2���б��ܼ�(2���ն�����)
    #define DRIVER_CAOBIAO          1             //�Ƿ��ȡ�������� 0:û�г����� 1:�г�����(2�ͣ�01g�г�����)
    #define MID_LCD_STYLE              1           //0:160x160 1:240x128
    #define GUI_DISP_TYPE             2           //0:2�ͷ��(��״̬��) 1:231���(��״̬��)  2:01g���(240x128)
    #define GUI_TYPE_DIR            "240-128"
#elif HARDWARE_TYPE == 2        //�����ն�
    #define DRIVER_BAOMI            1             //�Ƿ��ȡ�������� 0:û�б��ܼ� 1���б��ܼ�(231����) 2���б��ܼ�(2���ն�����)
    #define DRIVER_CAOBIAO          1             //�Ƿ��ȡ�������� 0:û�г����� 1:�г�����(2�ͣ�01g�г�����)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128
    #define GUI_DISP_TYPE             1           //0:2�ͷ��(��״̬��) 1:231���(��״̬��)  2:01g���(240x128)
    #define GUI_TYPE_DIR            "style231"
#elif HARDWARE_TYPE == 3 || HARDWARE_TYPE == 4 || HARDWARE_TYPE == 9        //�����ն�
    #if HARDWARE_TYPE == 3
    #define DRIVER_BAOMI            1             //�Ƿ��ȡ�������� 0:û�б��ܼ� 1���б��ܼ�(231����) 2���б��ܼ�(2���ն�����)
    #else    //������û�б��ܼ�
    #define DRIVER_BAOMI            0             //�Ƿ��ȡ�������� 0:û�б��ܼ� 1���б��ܼ�(231����) 2���б��ܼ�(2���ն�����)
    #endif
    #define DRIVER_CAOBIAO          0             //�Ƿ��ȡ�������� 0:û�г����� 1:�г�����(2�ͣ�01g�г�����)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128
    #define GUI_DISP_TYPE             1           //0:2�ͷ��(��״̬��) 1:231���(��״̬��)  2:01g���(240x128)
    #define GUI_TYPE_DIR            "style231"
#else
    #define DRIVER_BAOMI            1             //�Ƿ��ȡ�������� 0:û�б��ܼ� 1���б��ܼ�(231����) 2���б��ܼ�(2���ն�����)
    #define DRIVER_CAOBIAO          0             //�Ƿ��ȡ�������� 0:û�г����� 1:�г�����(2�ͣ�01g�г�����)
    #define MID_LCD_STYLE              0             //0:160x160 1:240x128

    #define GUI_DISP_TYPE             1           //0:2�ͷ��(��״̬��) 1:231���(��״̬��)  2:01g���(240x128)
    #define GUI_TYPE_DIR            "style231"
#endif

#if SAL_PRTL == 1
#define SecMod_SAL                  0x10        //SAL ����
#endif
// ˫ͨ��ʱ�ı䴮�ںţ���ŵ�������������
#if DOUBLE_CHANNEL_EN == 1
#define COMMU_TTYS_EXT                  5            //��չ��ͨѶͨ��ʹ�ô���

#undef INFRARED_TTYS
#define INFRARED_TTYS                      99            //��ͨѶͨ��ʹ�ô���
#endif

#endif
