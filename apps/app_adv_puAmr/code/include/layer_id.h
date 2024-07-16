#ifndef _LAYER_ID_H_
#define _LAYER_ID_H_

//===============================================================================================================
#if GUI_DISP_TYPE == 0  // 2�ͷ��(��״̬��)
//===============================================================================================================
/************************************************************************/
/* ʵʱ��Ϣ                                                           */
/************************************************************************/
#define REMOTE_CTRL_STATUS		0x00810000	// ң����բ״̬ 
#define REMOTE_CTRL_STATUS_2	0x00800000	// ң�غ�բ״̬ 

#define MON_CTRL_STATUS_1	0x00080100	// �µ��״̬
#define MON_CTRL_STATUS_2	0x00080200	// �µ��״̬
#define MON_CTRL_STATUS_3	0x00080300	// �µ��״̬
#define MON_CTRL_STATUS_4	0x00080400	// �µ��״̬
#define MON_CTRL_STATUS_5	0x00080500	// �µ��״̬
#define MON_CTRL_STATUS_6	0x00080600	// �µ��״̬
#define MON_CTRL_STATUS_7	0x00080700	// �µ��״̬
#define MON_CTRL_STATUS_8	0x00080800	// �µ��״̬

#define OVERFLOW_CTRL_STATUS_1	0x00820100	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_2	0x00820200	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_3	0x00820300	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_4	0x00820400	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_5	0x00820500	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_6	0x00820600	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_7	0x00820700	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_8	0x00820800	// �¸���״̬


#define STOP_CTRL_STATUS_1	0x00870100	// ��ͣ��״̬
#define STOP_CTRL_STATUS_2	0x00870200	// ��ͣ��״̬
#define STOP_CTRL_STATUS_3	0x00870300	// ��ͣ��״̬
#define STOP_CTRL_STATUS_4	0x00870400	// ��ͣ��״̬
#define STOP_CTRL_STATUS_5	0x00870500	// ��ͣ��״̬
#define STOP_CTRL_STATUS_6	0x00870600	// ��ͣ��״̬
#define STOP_CTRL_STATUS_7	0x00870700	// ��ͣ��״̬
#define STOP_CTRL_STATUS_8	0x00870800	// ��ͣ��״̬

#define RESET_CTRL_STATUS_1	0x00830100	// ���ݿ�״̬
#define RESET_CTRL_STATUS_2	0x00830200	// ���ݿ�״̬
#define RESET_CTRL_STATUS_3	0x00830300	// ���ݿ�״̬
#define RESET_CTRL_STATUS_4	0x00830400	// ���ݿ�״̬
#define RESET_CTRL_STATUS_5	0x00830500	// ���ݿ�״̬
#define RESET_CTRL_STATUS_6	0x00830600	// ���ݿ�״̬
#define RESET_CTRL_STATUS_7	0x00830700	// ���ݿ�״̬
#define RESET_CTRL_STATUS_8	0x00830800	// ���ݿ�״̬

#define REMOTE_SWITCH_STATUS	0x00030000	// ң�ſ���״̬

#define TIME_CTRL_STATUS_1	0x00860100	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_2	0x00860200	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_3	0x00860300	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_4	0x00860400	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_5	0x00860500	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_6	0x00860600	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_7	0x00860700	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_8	0x00860800	// ʱ�ο�״̬

#define BUY_CTRL_STATUS_1	0x00850100	// �����״̬
#define BUY_CTRL_STATUS_2	0x00850200	// �����״̬
#define BUY_CTRL_STATUS_3	0x00850300	// �����״̬
#define BUY_CTRL_STATUS_4	0x00850400	// �����״̬
#define BUY_CTRL_STATUS_5	0x00850500	// �����״̬
#define BUY_CTRL_STATUS_6	0x00850600	// �����״̬
#define BUY_CTRL_STATUS_7	0x00850700	// �����״̬
#define BUY_CTRL_STATUS_8	0x00850800	// �����״̬

/************************************************************************/
/* �ն�����                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // ϵͳ����
/************************************************************************/
/* �ն����ݽ���                                                         */
/************************************************************************/


/************************************************************************/
/* ���Ʋ���                                                             */
/************************************************************************/
// ʱ�οز���
#define TIME_CTRL_PARA_1_1 		0x00280100
#define TIME_CTRL_PARA_2_1 		0x00280200
#define TIME_CTRL_PARA_3_1 		0x00280300
#define TIME_CTRL_PARA_4_1 		0x00280400
#define TIME_CTRL_PARA_5_1 		0x00280500
#define TIME_CTRL_PARA_6_1 		0x00280600
#define TIME_CTRL_PARA_7_1 		0x00280700
#define TIME_CTRL_PARA_8_1 		0x00280800 

// �¸��ز���
#define LOW_CTRL_PATA_1		0x00210100
#define LOW_CTRL_PATA_2		0x00210200
#define LOW_CTRL_PATA_3		0x00210300
#define LOW_CTRL_PATA_4		0x00210400
#define LOW_CTRL_PATA_5		0x00210500
#define LOW_CTRL_PATA_6		0x00210600
#define LOW_CTRL_PATA_7		0x00210700
#define LOW_CTRL_PATA_8		0x00210800

// ���ݿز���
#define RESET_CTRL_PATA_1	0x00220100
#define RESET_CTRL_PATA_2	0x00220200
#define RESET_CTRL_PATA_3	0x00220300
#define RESET_CTRL_PATA_4	0x00220400
#define RESET_CTRL_PATA_5	0x00220500
#define RESET_CTRL_PATA_6	0x00220600
#define RESET_CTRL_PATA_7	0x00220700
#define RESET_CTRL_PATA_8	0x00220800

// �µ�ز���
#define MON_CTRL_PATA_1		0x00240100
#define MON_CTRL_PATA_2   	0x00240200
#define MON_CTRL_PATA_3   	0x00240300
#define MON_CTRL_PATA_4   	0x00240400
#define MON_CTRL_PATA_5		0x00240500
#define MON_CTRL_PATA_6   	0x00240600
#define MON_CTRL_PATA_7   	0x00240700
#define MON_CTRL_PATA_8   	0x00240800

// ��ͣ�ز���
#define STOP_CTRL_PATA_1  	0x00290100
#define STOP_CTRL_PATA_2  	0x00290200
#define STOP_CTRL_PATA_3  	0x00290300
#define STOP_CTRL_PATA_4  	0x00290400
#define STOP_CTRL_PATA_5  	0x00290500
#define STOP_CTRL_PATA_6  	0x00290600
#define STOP_CTRL_PATA_7  	0x00290700
#define STOP_CTRL_PATA_8  	0x00290800

// ����ز���
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400
#define BUY_CTRL_PATA_5	 0x00010500
#define BUY_CTRL_PATA_6  0x00010600
#define BUY_CTRL_PATA_7  0x00010700
#define BUY_CTRL_PATA_8  0x00010800

#define PowerDown_PARA_1 0x99999999
#define PowerDown_PARA_2 0x99999999
#define PowerDown_PARA_3 0x99999999
#define PowerDown_PARA_4 0x99999999

#define CALLING_EXES_PARA  0x00260000 // �߷ѿز���
/************************************************************************/
/* ���Ʋ�������                                                         */
/************************************************************************/


/************************************************************************/
/* �澯ҳ���                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // ʱ�ο�8���ִεĸ澯״̬
#define ALARM_STATUS_2    	0x00900001    // ���ݿ�8���ִεĸ澯״̬
#define ALARM_STATUS_3    	0x00900002    // ʵʱ��8���ִεĸ澯״̬
#define ALARM_STATUS_4      0x00900003    // Ӫҵ��ͣ��8���ִεĸ澯״̬
#define ALARM_STATUS_5    	0x00900004    // �յ��8���ִεĸ澯״̬
#define ALARM_STATUS_6    	0x00900005    // �µ��8���ִεĸ澯״̬
#define ALARM_STATUS_7    	0x00900006    // �����8���ִεĸ澯״̬
#define ALARM_STATUS_8      0x00900007    // ң��8���ִεĸ澯״̬
#define ALARM_STATUS_9      0x00900008    // �ܿ���8���ִεĸ澯״̬

#define GATE_STATUS_1       0x00900009     // ʱ�ο�8���ִε���բ״̬
#define GATE_STATUS_2       0x0090000A     // ���ݿ�8���ִε���բ״̬
#define GATE_STATUS_3       0x0090000B     // ʵʱ��8���ִε���բ״̬
#define GATE_STATUS_4       0x0090000C     // Ӫҵ��ͣ��8���ִε���բ״̬
#define GATE_STATUS_5       0x0090000D     // �յ��8���ִε���բ״̬
#define GATE_STATUS_6       0x0090000E     // �µ��8���ִε���բ״̬
#define GATE_STATUS_7       0x0090000F     // �����8���ִε���բ״̬
#define GATE_STATUS_8       0x00900010     // ң��8���ִε���բ״̬
#define GATE_STATUS_9       0x00800000     // �ܿ���8���ִε���բ״̬
/************************************************************************/
/* �澯ҳ������                                                       */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                       */
/************************************************************************/
#define MSG_INFO_1      0x00700000       // ������Ϣ1 
#define MSG_INFO_2		0x00700001       // ������Ϣ2 
#define MSG_INFO_3		0x00700002       // ������Ϣ3 
#define MSG_INFO_4		0x00700003       // ������Ϣ4	
#define MSG_INFO_5		0x00700004       // ������Ϣ5	
#define MSG_INFO_6		0x00700005       // ������Ϣ6	
#define MSG_INFO_7		0x00700006       // ������Ϣ7	
#define MSG_INFO_8		0x00700007       // ������Ϣ8	
#define MSG_INFO_9		0x00700008       // ������Ϣ9	
#define MSG_INFO_10		0x00700009       // ������Ϣ10
/************************************************************************/
/* ������Ϣҳ������                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-�ն˵�ǰ�Ŀ���״̬-
#define DNB_PARA		0x00430101		//-���ܱ����-
#define PULSE_PARA		0x00400000		//-Kv Ki Kp-

//===============================================================================================================
#elif GUI_DISP_TYPE == 1  // 231���(��״̬��)
//===============================================================================================================
/************************************************************************/
/* ʵʱ��Ϣ                                                             */
/************************************************************************/
#define REMOTE_CTRL_STATUS		0x00020000	// ң����բ״̬ 
#define REMOTE_CTRL_STATUS_2	0x00020000	// ң�غ�բ״̬,��Ҫ��Ϊ�ۺϿ���״̬ 

#define MON_CTRL_STATUS_1		0x00080100	// �µ��״̬
#define MON_CTRL_STATUS_2		0x00080200	// �µ��״̬
#define MON_CTRL_STATUS_3		0x00080300	// �µ��״̬
#define MON_CTRL_STATUS_4		0x00080400	// �µ��״̬

#define OVERFLOW_CTRL_STATUS_1	0x00040100	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_2	0x00040200	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_3	0x00040300	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_4	0x00040400	// �¸���״̬

#define STOP_CTRL_STATUS_1		0x00050100	// ��ͣ��״̬
#define STOP_CTRL_STATUS_2		0x00050200	// ��ͣ��״̬
#define STOP_CTRL_STATUS_3		0x00050300	// ��ͣ��״̬
#define STOP_CTRL_STATUS_4		0x00050400	// ��ͣ��״̬

#define RESET_CTRL_STATUS_1		0x00060100	// ���ݿ�״̬
#define RESET_CTRL_STATUS_2		0x00060200	// ���ݿ�״̬
#define RESET_CTRL_STATUS_3		0x00060300	// ���ݿ�״̬
#define RESET_CTRL_STATUS_4		0x00060400	// ���ݿ�״̬

#define REMOTE_SWITCH_STATUS	0x000b0000	// ң�ſ���״̬

#define TIME_CTRL_STATUS_1		0x00030100	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_2		0x00030200	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_3		0x00030300	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_4		0x00030400	// ʱ�ο�״̬

#define BUY_CTRL_STATUS_1		0x00090100	// �����״̬
#define BUY_CTRL_STATUS_2		0x00090200	// �����״̬
#define BUY_CTRL_STATUS_3		0x00090300	// �����״̬
#define BUY_CTRL_STATUS_4		0x00090400	// �����״̬

/************************************************************************/
/* �ն�����                                                             */
/************************************************************************/
#define TERM_SYS_RESET			0x00610001 // ϵͳ����
/************************************************************************/
/* �ն����ݽ���                                                         */
/************************************************************************/


/************************************************************************/
/* ���Ʋ���                                                             */
/************************************************************************/
// ʱ�οز���
// ������1
#define TIME_CTRL_PARA_1_1 		0x00200100
#define TIME_CTRL_PARA_2_1 		0x00200200
#define TIME_CTRL_PARA_3_1 		0x00200300
#define TIME_CTRL_PARA_4_1 		0x00200400                            	
                            	
// �¸��ز���               	
#define LOW_CTRL_PATA_1			0x00210100
#define LOW_CTRL_PATA_2			0x00210200
#define LOW_CTRL_PATA_3			0x00210300
#define LOW_CTRL_PATA_4			0x00210400

// ���ݿز���
#define RESET_CTRL_PATA_1		0x00220100
#define RESET_CTRL_PATA_2		0x00220200
#define RESET_CTRL_PATA_3		0x00220300
#define RESET_CTRL_PATA_4		0x00220400

// �µ�ز���
#define MON_CTRL_PATA_1			0x00240100
#define MON_CTRL_PATA_2   		0x00240200
#define MON_CTRL_PATA_3   		0x00240300
#define MON_CTRL_PATA_4   		0x00240400

// ��ͣ�ز���
#define STOP_CTRL_PATA_1		0x00290100
#define STOP_CTRL_PATA_2  		0x00290200
#define STOP_CTRL_PATA_3  		0x00290300
#define STOP_CTRL_PATA_4  		0x00290400

// ����ز���
#define BUY_CTRL_PATA_1	 		0x00250100
#define BUY_CTRL_PATA_2  		0x00250200
#define BUY_CTRL_PATA_3  		0x00250300
#define BUY_CTRL_PATA_4  		0x00250400

#define PowerDown_PARA_1        0x00210100
#define PowerDown_PARA_2        0x00210200
#define PowerDown_PARA_3        0x00210300
#define PowerDown_PARA_4        0x00210400

#define CALLING_EXES_PARA  		0x00260000 // �߷ѿز���
/************************************************************************/
/* ���Ʋ�������                                                         */
/************************************************************************/

/************************************************************************/
/* �澯ҳ���                                                           */
/************************************************************************/
#define ALARM_STATUS_1    		0x00900000    // ʱ�ο�8���ִεĸ澯״̬
#define ALARM_STATUS_2    		0x00900001    // ���ݿ�8���ִεĸ澯״̬
#define ALARM_STATUS_3    		0x00900002    // ʵʱ��8���ִεĸ澯״̬
#define ALARM_STATUS_4      	0x00900003    // Ӫҵ��ͣ��8���ִεĸ澯״̬
#define ALARM_STATUS_5    		0x00900004    // �յ��8���ִεĸ澯״̬
#define ALARM_STATUS_6    		0x00900005    // �µ��8���ִεĸ澯״̬
#define ALARM_STATUS_7    		0x00900006    // �����8���ִεĸ澯״̬
#define ALARM_STATUS_8      	0x00900007    // ң��8���ִεĸ澯״̬
#define ALARM_STATUS_9      	0x00900008    // �ܿ���8���ִεĸ澯״̬

#define GATE_STATUS_1       	0x00900009     // ʱ�ο�8���ִε���բ״̬
#define GATE_STATUS_2       	0x0090000A     // ���ݿ�8���ִε���բ״̬
#define GATE_STATUS_3       	0x0090000B     // ʵʱ��8���ִε���բ״̬
#define GATE_STATUS_4       	0x0090000C     // Ӫҵ��ͣ��8���ִε���բ״̬
#define GATE_STATUS_5       	0x0090000D     // �յ��8���ִε���բ״̬
#define GATE_STATUS_6       	0x0090000E     // �µ��8���ִε���բ״̬
#define GATE_STATUS_7       	0x0090000F     // �����8���ִε���բ״̬
#define GATE_STATUS_8       	0x00900010     // ң��8���ִε���բ״̬
#define GATE_STATUS_9       	0x00900011     // �ܿ���8���ִε���բ״̬
/************************************************************************/
/* �澯ҳ������                                                       */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                       */
/************************************************************************/
#define MSG_INFO_1    	0x00700000       // ������Ϣ1 
#define MSG_INFO_2		0x00700001       // ������Ϣ2 
#define MSG_INFO_3		0x00700002       // ������Ϣ3 
#define MSG_INFO_4		0x00700003       // ������Ϣ4	
#define MSG_INFO_5		0x00700004       // ������Ϣ5	
#define MSG_INFO_6		0x00700005       // ������Ϣ6	
#define MSG_INFO_7		0x00700006       // ������Ϣ7	
#define MSG_INFO_8		0x00700007       // ������Ϣ8	
#define MSG_INFO_9		0x00700008       // ������Ϣ9	
#define MSG_INFO_10		0x00700009       // ������Ϣ10
/************************************************************************/
/* ������Ϣҳ������                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x000a0100		//-�ն˵�ǰ�Ŀ���״̬-
#define DNB_PARA		0x00400100		//-���ܱ����-
#define PULSE_PARA		0x00400100		//-Kv Ki Kp-


//===============================================================================================================
#elif GUI_DISP_TYPE == 2  // 01g���(240x128)
//===============================================================================================================
#define REMOTE_CTRL_STATUS	0x00020000	// ң��״̬ 
#define REMOTE_CTRL_STATUS_2	0x20800000	// ң�غ�բ״̬ 

#define MON_CTRL_STATUS_1	0x00080100	// �µ��״̬
#define MON_CTRL_STATUS_2	0x00080200	// �µ��״̬
#define MON_CTRL_STATUS_3	0x00080300	// �µ��״̬
#define MON_CTRL_STATUS_4	0x00080400	// �µ��״̬

#define OVERFLOW_CTRL_STATUS_1	0x00040100	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_2	0x00040200	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_3	0x00040300	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_4	0x00040400	// �¸���״̬

#define STOP_CTRL_STATUS_1	0x00050100	// ��ͣ��״̬
#define STOP_CTRL_STATUS_2	0x00050200	// ��ͣ��״̬
#define STOP_CTRL_STATUS_3	0x00050300	// ��ͣ��״̬
#define STOP_CTRL_STATUS_4	0x00050400	// ��ͣ��״̬

#define RESET_CTRL_STATUS_1	0x00060100	// ���ݿ�״̬
#define RESET_CTRL_STATUS_2	0x00060200	// ���ݿ�״̬
#define RESET_CTRL_STATUS_3	0x00060300	// ���ݿ�״̬
#define RESET_CTRL_STATUS_4	0x00060400	// ���ݿ�״̬

#define REMOTE_SWITCH_STATUS	0x20030000	// ң�ſ���״̬

#define TIME_CTRL_STATUS_1	0x00030100	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_2	0x00030200	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_3	0x00030300	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_4	0x00030400	// ʱ�ο�״̬

#define BUY_CTRL_STATUS_1	0x00090100	// �����״̬
#define BUY_CTRL_STATUS_2	0x00090200	// �����״̬
#define BUY_CTRL_STATUS_3	0x00090300	// �����״̬
#define BUY_CTRL_STATUS_4	0x00090400	// �����״̬

/************************************************************************/
/* �ն�����                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // ϵͳ����
/************************************************************************/
/* �ն����ݽ���                                                         */
/************************************************************************/


/************************************************************************/
/* ���Ʋ���                                                             */
/************************************************************************/
// ʱ�οز���
// ��һ��
#define TIME_CTRL_PARA_1_1 		0x20280100
#define TIME_CTRL_PARA_2_1 		0x20280200
#define TIME_CTRL_PARA_3_1 		0x20280300
#define TIME_CTRL_PARA_4_1 		0x20280400

#define LOW_CTRL_PATA_1	  0x20120100
#define LOW_CTRL_PATA_2   0x20120200
#define LOW_CTRL_PATA_3   0x20120300
#define LOW_CTRL_PATA_4   0x20120400

// ���ݿز���
#define RESET_CTRL_PATA_1	0x20110100
#define RESET_CTRL_PATA_2   0x20110200
#define RESET_CTRL_PATA_3   0x20110300
#define RESET_CTRL_PATA_4   0x20110400

// �µ�ز���
#define MON_CTRL_PATA_1	  0x20240100
#define MON_CTRL_PATA_2   0x20240200
#define MON_CTRL_PATA_3   0x20240300
#define MON_CTRL_PATA_4   0x20240400

// ��ͣ�ز���
#define STOP_CTRL_PATA_1  0x20290100
#define STOP_CTRL_PATA_2  0x20290200
#define STOP_CTRL_PATA_3  0x20290300
#define STOP_CTRL_PATA_4  0x20290400

// ����ز���
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400

#define PowerDown_PARA_1 0x20120100
#define PowerDown_PARA_2 0x20120200
#define PowerDown_PARA_3 0x20120300
#define PowerDown_PARA_4 0x20120400

#define CALLING_EXES_PARA  0x00260000 // �߷ѿز���
/************************************************************************/
/* ���Ʋ�������                                                         */
/************************************************************************/


/************************************************************************/
/* �澯ҳ���                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // ʱ�ο�8���ִεĸ澯״̬
#define ALARM_STATUS_2    	0x00900001    // ���ݿ�8���ִεĸ澯״̬
#define ALARM_STATUS_3    	0x00900002    // ʵʱ��8���ִεĸ澯״̬
#define ALARM_STATUS_4      0x00900003    // Ӫҵ��ͣ��8���ִεĸ澯״̬
#define ALARM_STATUS_5    	0x00900004    // �յ��8���ִεĸ澯״̬
#define ALARM_STATUS_6    	0x00900005    // �µ��8���ִεĸ澯״̬
#define ALARM_STATUS_7    	0x00900006    // �����8���ִεĸ澯״̬
#define ALARM_STATUS_8      0x00900007    // ң��8���ִεĸ澯״̬
#define ALARM_STATUS_9      0x00900008    // �ܿ���8���ִεĸ澯״̬

#define GATE_STATUS_1       0x00900009     // ʱ�ο�8���ִε���բ״̬
#define GATE_STATUS_2       0x0090000A     // ���ݿ�8���ִε���բ״̬
#define GATE_STATUS_3       0x0090000B     // ʵʱ��8���ִε���բ״̬
#define GATE_STATUS_4       0x0090000C     // Ӫҵ��ͣ��8���ִε���բ״̬
#define GATE_STATUS_5       0x0090000D     // �յ��8���ִε���բ״̬
#define GATE_STATUS_6       0x0090000E     // �µ��8���ִε���բ״̬
#define GATE_STATUS_7       0x0090000F     // �����8���ִε���բ״̬
#define GATE_STATUS_8       0x00900010     // ң��8���ִε���բ״̬
#define GATE_STATUS_9       0x00900011     // �ܿ���8���ִε���բ״̬
/************************************************************************/
/* �澯ҳ������                                                       */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                       */
/************************************************************************/
#define MSG_INFO_1      0x20400000       // ������Ϣ1 
#define MSG_INFO_2		0x20400001       // ������Ϣ2 
#define MSG_INFO_3		0x20400002       // ������Ϣ3 
#define MSG_INFO_4		0x20400003       // ������Ϣ4	
#define MSG_INFO_5		0x20400004       // ������Ϣ5	
#define MSG_INFO_6		0x20400005       // ������Ϣ6	
#define MSG_INFO_7		0x20400006       // ������Ϣ7	
#define MSG_INFO_8		0x20400007       // ������Ϣ8	
#define MSG_INFO_9		0x20400008       // ������Ϣ9	
#define MSG_INFO_10		0x20400009       // ������Ϣ10
/************************************************************************/
/* ������Ϣҳ������                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-�ն˵�ǰ�Ŀ���״̬-
#define DNB_PARA		0x20140000		//-���ܱ����-
#define PULSE_PARA		0x20130000		//-Kv Ki Kp-

#elif GUI_DISP_TYPE == 3  // 01g���(128x64)
//===============================================================================================================
#define REMOTE_CTRL_STATUS		0x00810000	// ң����բ״̬ 
#define REMOTE_CTRL_STATUS_2	0x00800000	// ң�غ�բ״̬ 

#define MON_CTRL_STATUS_1	0x00080100	// �µ��״̬
#define MON_CTRL_STATUS_2	0x00080200	// �µ��״̬
#define MON_CTRL_STATUS_3	0x00080300	// �µ��״̬
#define MON_CTRL_STATUS_4	0x00080400	// �µ��״̬
#define MON_CTRL_STATUS_5	0x00080500	// �µ��״̬
#define MON_CTRL_STATUS_6	0x00080600	// �µ��״̬
#define MON_CTRL_STATUS_7	0x00080700	// �µ��״̬
#define MON_CTRL_STATUS_8	0x00080800	// �µ��״̬

#define OVERFLOW_CTRL_STATUS_1	0x00820100	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_2	0x00820200	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_3	0x00820300	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_4	0x00820400	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_5	0x00820500	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_6	0x00820600	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_7	0x00820700	// �¸���״̬
#define OVERFLOW_CTRL_STATUS_8	0x00820800	// �¸���״̬


#define STOP_CTRL_STATUS_1	0x00870100	// ��ͣ��״̬
#define STOP_CTRL_STATUS_2	0x00870200	// ��ͣ��״̬
#define STOP_CTRL_STATUS_3	0x00870300	// ��ͣ��״̬
#define STOP_CTRL_STATUS_4	0x00870400	// ��ͣ��״̬
#define STOP_CTRL_STATUS_5	0x00870500	// ��ͣ��״̬
#define STOP_CTRL_STATUS_6	0x00870600	// ��ͣ��״̬
#define STOP_CTRL_STATUS_7	0x00870700	// ��ͣ��״̬
#define STOP_CTRL_STATUS_8	0x00870800	// ��ͣ��״̬

#define RESET_CTRL_STATUS_1	0x00830100	// ���ݿ�״̬
#define RESET_CTRL_STATUS_2	0x00830200	// ���ݿ�״̬
#define RESET_CTRL_STATUS_3	0x00830300	// ���ݿ�״̬
#define RESET_CTRL_STATUS_4	0x00830400	// ���ݿ�״̬
#define RESET_CTRL_STATUS_5	0x00830500	// ���ݿ�״̬
#define RESET_CTRL_STATUS_6	0x00830600	// ���ݿ�״̬
#define RESET_CTRL_STATUS_7	0x00830700	// ���ݿ�״̬
#define RESET_CTRL_STATUS_8	0x00830800	// ���ݿ�״̬

#define REMOTE_SWITCH_STATUS	0x00030000	// ң�ſ���״̬

#define TIME_CTRL_STATUS_1	0x00860100	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_2	0x00860200	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_3	0x00860300	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_4	0x00860400	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_5	0x00860500	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_6	0x00860600	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_7	0x00860700	// ʱ�ο�״̬
#define TIME_CTRL_STATUS_8	0x00860800	// ʱ�ο�״̬

#define BUY_CTRL_STATUS_1	0x00850100	// �����״̬
#define BUY_CTRL_STATUS_2	0x00850200	// �����״̬
#define BUY_CTRL_STATUS_3	0x00850300	// �����״̬
#define BUY_CTRL_STATUS_4	0x00850400	// �����״̬
#define BUY_CTRL_STATUS_5	0x00850500	// �����״̬
#define BUY_CTRL_STATUS_6	0x00850600	// �����״̬
#define BUY_CTRL_STATUS_7	0x00850700	// �����״̬
#define BUY_CTRL_STATUS_8	0x00850800	// �����״̬

/************************************************************************/
/* �ն�����                                                             */
/************************************************************************/
#define TERM_SYS_RESET		0x00610001 // ϵͳ����
/************************************************************************/
/* �ն����ݽ���                                                         */
/************************************************************************/


/************************************************************************/
/* ���Ʋ���                                                             */
/************************************************************************/
// ʱ�οز���
#define TIME_CTRL_PARA_1_1 		0x00280100
#define TIME_CTRL_PARA_2_1 		0x00280200
#define TIME_CTRL_PARA_3_1 		0x00280300
#define TIME_CTRL_PARA_4_1 		0x00280400
#define TIME_CTRL_PARA_5_1 		0x00280500
#define TIME_CTRL_PARA_6_1 		0x00280600
#define TIME_CTRL_PARA_7_1 		0x00280700
#define TIME_CTRL_PARA_8_1 		0x00280800 

// �¸��ز���
#define LOW_CTRL_PATA_1		0x00210100
#define LOW_CTRL_PATA_2		0x00210200
#define LOW_CTRL_PATA_3		0x00210300
#define LOW_CTRL_PATA_4		0x00210400
#define LOW_CTRL_PATA_5		0x00210500
#define LOW_CTRL_PATA_6		0x00210600
#define LOW_CTRL_PATA_7		0x00210700
#define LOW_CTRL_PATA_8		0x00210800

// ���ݿز���
#define RESET_CTRL_PATA_1	0x00220100
#define RESET_CTRL_PATA_2	0x00220200
#define RESET_CTRL_PATA_3	0x00220300
#define RESET_CTRL_PATA_4	0x00220400
#define RESET_CTRL_PATA_5	0x00220500
#define RESET_CTRL_PATA_6	0x00220600
#define RESET_CTRL_PATA_7	0x00220700
#define RESET_CTRL_PATA_8	0x00220800

// �µ�ز���
#define MON_CTRL_PATA_1		0x00240100
#define MON_CTRL_PATA_2   	0x00240200
#define MON_CTRL_PATA_3   	0x00240300
#define MON_CTRL_PATA_4   	0x00240400
#define MON_CTRL_PATA_5		0x00240500
#define MON_CTRL_PATA_6   	0x00240600
#define MON_CTRL_PATA_7   	0x00240700
#define MON_CTRL_PATA_8   	0x00240800

// ��ͣ�ز���
#define STOP_CTRL_PATA_1  	0x00290100
#define STOP_CTRL_PATA_2  	0x00290200
#define STOP_CTRL_PATA_3  	0x00290300
#define STOP_CTRL_PATA_4  	0x00290400
#define STOP_CTRL_PATA_5  	0x00290500
#define STOP_CTRL_PATA_6  	0x00290600
#define STOP_CTRL_PATA_7  	0x00290700
#define STOP_CTRL_PATA_8  	0x00290800

// ����ز���
#define BUY_CTRL_PATA_1	 0x00010100
#define BUY_CTRL_PATA_2  0x00010200
#define BUY_CTRL_PATA_3  0x00010300
#define BUY_CTRL_PATA_4  0x00010400
#define BUY_CTRL_PATA_5	 0x00010500
#define BUY_CTRL_PATA_6  0x00010600
#define BUY_CTRL_PATA_7  0x00010700
#define BUY_CTRL_PATA_8  0x00010800

#define PowerDown_PARA_1 0x00210100
#define PowerDown_PARA_2 0x00210200
#define PowerDown_PARA_3 0x00210300
#define PowerDown_PARA_4 0x00210400

#define CALLING_EXES_PARA  0x00260000 // �߷ѿز���
/************************************************************************/
/* ���Ʋ�������                                                         */
/************************************************************************/


/************************************************************************/
/* �澯ҳ���                                                           */
/************************************************************************/
#define ALARM_STATUS_1    	0x00900000    // ʱ�ο�8���ִεĸ澯״̬
#define ALARM_STATUS_2    	0x00900001    // ���ݿ�8���ִεĸ澯״̬
#define ALARM_STATUS_3    	0x00900002    // ʵʱ��8���ִεĸ澯״̬
#define ALARM_STATUS_4      0x00900003    // Ӫҵ��ͣ��8���ִεĸ澯״̬
#define ALARM_STATUS_5    	0x00900004    // �յ��8���ִεĸ澯״̬
#define ALARM_STATUS_6    	0x00900005    // �µ��8���ִεĸ澯״̬
#define ALARM_STATUS_7    	0x00900006    // �����8���ִεĸ澯״̬
#define ALARM_STATUS_8      0x00900007    // ң��8���ִεĸ澯״̬
#define ALARM_STATUS_9      0x00900008    // �ܿ���8���ִεĸ澯״̬

#define GATE_STATUS_1       0x00900009     // ʱ�ο�8���ִε���բ״̬
#define GATE_STATUS_2       0x0090000A     // ���ݿ�8���ִε���բ״̬
#define GATE_STATUS_3       0x0090000B     // ʵʱ��8���ִε���բ״̬
#define GATE_STATUS_4       0x0090000C     // Ӫҵ��ͣ��8���ִε���բ״̬
#define GATE_STATUS_5       0x0090000D     // �յ��8���ִε���բ״̬
#define GATE_STATUS_6       0x0090000E     // �µ��8���ִε���բ״̬
#define GATE_STATUS_7       0x0090000F     // �����8���ִε���բ״̬
#define GATE_STATUS_8       0x00900010     // ң��8���ִε���բ״̬
#define GATE_STATUS_9       0x00800000     // �ܿ���8���ִε���բ״̬
/************************************************************************/
/* �澯ҳ������                                                       */
/************************************************************************/


/************************************************************************/
/* ������Ϣҳ���                                                       */
/************************************************************************/
#define MSG_INFO_1      0x00700000       // ������Ϣ1 
#define MSG_INFO_2		0x00700001       // ������Ϣ2 
#define MSG_INFO_3		0x00700002       // ������Ϣ3 
#define MSG_INFO_4		0x00700003       // ������Ϣ4	
#define MSG_INFO_5		0x00700004       // ������Ϣ5	
#define MSG_INFO_6		0x00700005       // ������Ϣ6	
#define MSG_INFO_7		0x00700006       // ������Ϣ7	
#define MSG_INFO_8		0x00700007       // ������Ϣ8	
#define MSG_INFO_9		0x00700008       // ������Ϣ9	
#define MSG_INFO_10		0x00700009       // ������Ϣ10
/************************************************************************/
/* ������Ϣҳ������                                                   */
/************************************************************************/

#define TMN_CTRL_STAT 	0x00010101		//-�ն˵�ǰ�Ŀ���״̬-
#define DNB_PARA		0x00430101		//-���ܱ����-
#define PULSE_PARA		0x00400000		//-Kv Ki Kp-

#endif // GUI_DISP_TYPE
#endif
