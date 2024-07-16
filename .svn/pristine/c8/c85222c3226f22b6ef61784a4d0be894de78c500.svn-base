#ifndef _TESAM_H_
#define _TESAM_H_

#define DA_ESAM_MAX_LEN 	    8192//配网加密芯片通讯最大长度

#define ESAM_GET_DATA		_IOW('E', 0x01, struct encryption_reg_req)
#define ESAM_SET_DATA		_IOW('E', 0x02, struct encryption_reg_req)


#define DA_ESAM_SERI_READ 			0x20//读芯片ESAM序列号(8B) ID
#define DA_ESAM_KEY_VER         	0x21//读取芯片密钥版本1B
#define DA_ESAM_RAND_READ   		0x22//读配网加密芯片随机数8B
#define DA_ESAM_RAND_SIGN       	0x23//对随机数进行签名
#define DA_ESAM_RAND_SIGN_CHECK 	0x24//对随机数签名进行验证
#define DA_ESAM_INFO_SIGN			0x25//对信息签名
#define DA_ESAM_INFO_SIGN_CHECK 	0x26//对信息验证签名验证
#define DA_ESAM_KYE_UPDATA      	0x27//密钥更新
#define DA_ESAM_KYE_REBACK      	0x28//密钥恢复
#define DA_ESAM_MTU_CERT_GET		0x29//终端证书读取
#define DA_ESAM_MTU_CERT_UPDATA   	0x2A//终端证书更新
#define DA_ESAM_HOST_CERT_UPDATA    0x2B//主站/网关证书更新
#define DA_ESAM_READ_MTU_CERTLEN    0x2C//读取终端测试证书长度
#define DA_ESAM_READ_MTU_CERT       0x2D//读取终端测试证书

#define DA_ESAM_ENCRYPTE			0xA8//加密
#define DA_ESAM_DECIPHER        	0xA9//解密

#define DA_TOO_CERT_CHECK			0x30//管理工具证书验证
#define DA_TOOL_RAND_SIGN_CHECK     0x31//对管理工具随机数签名验签
#define DA_ESAM_READ_PUBLIC_KEY     0x32//读取公钥
#define DA_TOOL_KEY_REBACK			0x33//管理证书工具下发的密钥恢复处理

#define DA_TOOL_ENCRYPTE			0xB8//与证书管理工具通讯加密处理
#define DA_TOOL_DECIPHER        	0xB9//与证书管理工具通讯解密处理



#define RTN_SUCCESS 			0x9000 //成功
#define RTN_ERR_INS 			0x6D00 //INS不支持
#define RTN_ERR_CLA 			0x6E00 //CLA不支持
#define RTN_ERR_P1P2 			0x6A86 //P1P2不正确
#define RTN_ERR_LC_LEN 			0x6700 //LC长度错误
#define RTN_ERR_SAVE 			0x6581 //存储器故障
#define RTN_ERR_COUN 			0x6901 //计数器不正确
#define RTN_ERR_RADN 			0x6903 //随机数无效
#define RTN_ERR_OUT 			0x6904 //外部认证错误
#define RTN_ERR_BAG 			0x6905 //数据包序号错
#define RTN_ERR_TIMER 			0x6907 //TIMER超时
#define RTN_ERR_STRU 			0x6981 //命令与文件结构不兼容
#define RTN_ERR_DAL 			0x6982 //会话未建立
#define RTN_ERR_COND 			0x6985 //使用条件不满足
#define RTN_ERR_CACL 			0x6988 //计算错误
#define RTN_ERR_MAC 			0x6989 //MAC校验错
#define RTN_ERR_DATA 			0x6A80 //不正确的数据域
#define RTN_ERR_ENN 			0x6A81 //功能不支持
#define RTN_ERR_FILE 			0x6A82 //文件未找到
#define RTN_ERR_NONE_SA 		0x6A84 //无足够的文件存储空间
#define RTN_ERR_KEY 			0x6A88 //密钥未找到
#define RTN_ERR_LRC 			0x6A90 //LRC校验错误

int DAEsamCommand(uint8 bType,uint8 *rcvbuf,uint8 *sndbuf,uint8 *databuf,uint8 *pRandNum);

#endif

