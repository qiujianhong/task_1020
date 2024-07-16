#ifndef _TESAM_H_
#define _TESAM_H_

#define DA_ESAM_MAX_LEN 	    8192//��������оƬͨѶ��󳤶�

#define ESAM_GET_DATA		_IOW('E', 0x01, struct encryption_reg_req)
#define ESAM_SET_DATA		_IOW('E', 0x02, struct encryption_reg_req)


#define DA_ESAM_SERI_READ 			0x20//��оƬESAM���к�(8B) ID
#define DA_ESAM_KEY_VER         	0x21//��ȡоƬ��Կ�汾1B
#define DA_ESAM_RAND_READ   		0x22//����������оƬ�����8B
#define DA_ESAM_RAND_SIGN       	0x23//�����������ǩ��
#define DA_ESAM_RAND_SIGN_CHECK 	0x24//�������ǩ��������֤
#define DA_ESAM_INFO_SIGN			0x25//����Ϣǩ��
#define DA_ESAM_INFO_SIGN_CHECK 	0x26//����Ϣ��֤ǩ����֤
#define DA_ESAM_KYE_UPDATA      	0x27//��Կ����
#define DA_ESAM_KYE_REBACK      	0x28//��Կ�ָ�
#define DA_ESAM_MTU_CERT_GET		0x29//�ն�֤���ȡ
#define DA_ESAM_MTU_CERT_UPDATA   	0x2A//�ն�֤�����
#define DA_ESAM_HOST_CERT_UPDATA    0x2B//��վ/����֤�����
#define DA_ESAM_READ_MTU_CERTLEN    0x2C//��ȡ�ն˲���֤�鳤��
#define DA_ESAM_READ_MTU_CERT       0x2D//��ȡ�ն˲���֤��

#define DA_ESAM_ENCRYPTE			0xA8//����
#define DA_ESAM_DECIPHER        	0xA9//����

#define DA_TOO_CERT_CHECK			0x30//������֤����֤
#define DA_TOOL_RAND_SIGN_CHECK     0x31//�Թ����������ǩ����ǩ
#define DA_ESAM_READ_PUBLIC_KEY     0x32//��ȡ��Կ
#define DA_TOOL_KEY_REBACK			0x33//����֤�鹤���·�����Կ�ָ�����

#define DA_TOOL_ENCRYPTE			0xB8//��֤�������ͨѶ���ܴ���
#define DA_TOOL_DECIPHER        	0xB9//��֤�������ͨѶ���ܴ���



#define RTN_SUCCESS 			0x9000 //�ɹ�
#define RTN_ERR_INS 			0x6D00 //INS��֧��
#define RTN_ERR_CLA 			0x6E00 //CLA��֧��
#define RTN_ERR_P1P2 			0x6A86 //P1P2����ȷ
#define RTN_ERR_LC_LEN 			0x6700 //LC���ȴ���
#define RTN_ERR_SAVE 			0x6581 //�洢������
#define RTN_ERR_COUN 			0x6901 //����������ȷ
#define RTN_ERR_RADN 			0x6903 //�������Ч
#define RTN_ERR_OUT 			0x6904 //�ⲿ��֤����
#define RTN_ERR_BAG 			0x6905 //���ݰ���Ŵ�
#define RTN_ERR_TIMER 			0x6907 //TIMER��ʱ
#define RTN_ERR_STRU 			0x6981 //�������ļ��ṹ������
#define RTN_ERR_DAL 			0x6982 //�Ựδ����
#define RTN_ERR_COND 			0x6985 //ʹ������������
#define RTN_ERR_CACL 			0x6988 //�������
#define RTN_ERR_MAC 			0x6989 //MACУ���
#define RTN_ERR_DATA 			0x6A80 //����ȷ��������
#define RTN_ERR_ENN 			0x6A81 //���ܲ�֧��
#define RTN_ERR_FILE 			0x6A82 //�ļ�δ�ҵ�
#define RTN_ERR_NONE_SA 		0x6A84 //���㹻���ļ��洢�ռ�
#define RTN_ERR_KEY 			0x6A88 //��Կδ�ҵ�
#define RTN_ERR_LRC 			0x6A90 //LRCУ�����

int DAEsamCommand(uint8 bType,uint8 *rcvbuf,uint8 *sndbuf,uint8 *databuf,uint8 *pRandNum);

#endif

