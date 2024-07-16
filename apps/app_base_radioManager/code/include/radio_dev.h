
#ifndef __RADIO_DEV_H__
#define __RADIO_DEV_H__
#ifdef __cplusplus
extern "C" {
#endif
#define M_OLDVER		0/*获得当前modem版本*/
#define M_START			1/*切换model状态进入升级*/
#define M_CODE			2/*首先获得0x43，表示可以开始传输文件*/
#define TRAN_File_FIR	3/*升级文件传输第一帧*/
#define TRAN_File_FIN	4/*升级文件传输第二帧*/
#define M_END			5/*升级结束*/
#define M_NEXT			6/*升级next*/
#define M_NEWVER		7/*获得更新后的modem版本*/
#define M_FAIL			8/*升级失败*/



#define MODEM_SOH	0x01		/*数据块起始字符以128字节开始*/
#define MODEM_STX	0x02		/*1024字节报文*/
#define MODEM_EOT	0x04		/*文件传输结束*/
#define MODEM_ACK	0x06		/*确认应答*/
#define MODEM_NAK	0x15		/*出现错误*/
#define MODEM_CAN	0x18		/*取消传输*/
#define MODEM_C		0x43		/*大写字母Ｃ*/

#define UpModem		0x55		/*0x55表示找到modem升级文件*/
#define NoModem		0xAA		/*0xAA表示没有找到modem升级文件*/

//包大小
#define PACK128		128		/*128字节*/
#define PACK1024	1024	/*1024字节*/

typedef struct _YMODEL_TYPE
{
    uint8 Kind; /*1表示所发长度为128,2表示为1024*/
    uint8 SEQ[2];
    uint8 Sbuf[1024];
    uint8 CRC[2];
}__attribute__ ((packed))YMODEL_TYPE;

int radio_channel_get(int fd);
uint32 SetRadioChannel_command(int fd_232, uint8 ch, uint8 ptx, uint8 sq);
int DeviceInit_radio();
int8 AtCheck(const int fd, char *send_buf, char *check_buf, int u_time);
void update_modem(int fd);
int UpdateModemInit();
void update_modem_start(int fd);

    void radio_speak_manager(int fd);

#ifdef __cplusplus
}
#endif

#endif

