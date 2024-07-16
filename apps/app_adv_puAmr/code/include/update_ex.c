/****************************************************************************
Copyright (c) 2006-2009,南京新联电子股份有限责任公司
         All rights reserved.
程序名称: update_ex.c
功    能：升级程序和其他程序交互的时候使用的数据字典
		  因为升级程序不能直接使用数据字典，所以需要自己来计算偏移量
开 发 人：顾君
开发时间：2009-05-26
版本历史:
        2009-05-26 ,v1.0 : New              
****************************************************************************/

#include    <stdio.h>       /*标准输入输出定义*/
#include    <stdlib.h>      /*标准函数库定义*/
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<unistd.h>
#include    <sys/file.h>    /*文件系统定义*/
#include    <string.h>      /*字符处理函数定义*/

#include "../../../../include/dataType.h"
#include 	"sys_cfg.h"
#include 	"update_ex.h"

extern int errno;

const char *updateex_file = "/ramdisk/updateex.dat";		//交互文件的位置
#define UPDATEEX_NUM (UPDATEEX_MAX_ID+1) // 数据项总数

#define UPDATEEX_ERR_PTR           2   //输入错误的指针值
#define UPDATEEX_ERR_O_RANGE       3   //数据超出有效范围
#define UPDATEEX_ERR_LOCK          4   //文件上存在不兼容的锁
#define UPDATEEX_ERR_ACCESS        6   //文件访问错误，一般指文件不存在
#define UPDATEEX_ERR_IO            13  //读写文件或设备错误
#define UPDATEEX_ERR_FILEEXCEED    19  //读写文件越界

/* 记录锁  */
inline int UpdateEx_RcdLock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

#define updateex_read_lock(fd, offset, len) \
            UpdateEx_RcdLock(fd, F_SETLK, F_RDLCK, offset, SEEK_SET, len)

#define updateex_readw_lock(fd, offset, len) \
            UpdateEx_RcdLock(fd, F_SETLKW, F_RDLCK, offset, SEEK_SET, len)

#define updateex_write_lock(fd, offset, len) \
            UpdateEx_RcdLock(fd, F_SETLK, F_WRLCK, offset, SEEK_SET, len)

#define updateex_writew_lock(fd, offset, len) \
            UpdateEx_RcdLock(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, len)

#define updateex_un_lock(fd, offset, len) \
            UpdateEx_RcdLock(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, len)

#define   PATH_MAX_LEN      256       //定义路径最大长度

updateex_info_t  updateex_info[]=
{
	{NEED_IUART, 	1},		// 1字节接口板升级请求标志
	{IUART_OK, 		1},		// 1字节接口板允许升级标志
	{NEED_AUART, 	1},		// 1字节交采版升级请求标志
	{AUART_OK, 		1},		// 1字节交采版允许升级标志
	{Update_State,	1},		// 1字节升级状态
	{Frame_Count,	2},		// 2字节无符号
	{Cur_Frame,		2},		// 2字节无符号
	{0xffff, 		0},
};

//取得升级交互ID在私有文件中的偏移
int updateex_id_offset(uint16 index)
{
	uint16 i = 0;
	int tmpLen = 0;
	if(index >= UPDATEEX_MAX_ID)
		return -1;		//如果查询的ID错误，不进行任何处理
	for(i = 0; i < index; i++)
		tmpLen += updateex_id_len(i);
	
	return tmpLen;
}

/*
  名  称: FGetFileSize
  功  能: 获取文件尺寸
  输  入: 
  	文件描述符fd
  输  出: 
  返回值:
			-1  : 错误
			>=0 : 文件/子目录占用字节数
 */
inline ssize_t FGetUpdateExFileSize(int fd)
{
	struct stat sta;
	
	if(fstat(fd, &sta) < 0)	
		return -1;
	
	return sta.st_size;	
}

/*
  名  称: FileExists
  功  能: 检测文件是否存在
  输  入:
 			filepath - 查找文件是否存在
  输  出:
 
  返回值:
 			TRUE 	- 存在
 			FALSE - 不存在
 */
inline BOOL UpdateExFileExists(const char *filepath)
{
	struct stat sta;
	
	if( stat(filepath, &sta) < 0)
		return FALSE;
		
	if( S_ISREG(sta.st_mode))
		return TRUE;

	return FALSE;
}

/*
********************************************************************
 名  称: RcdLock 
 功  能: 用fcntl方式实现记录锁
 输  入: fd	 - 锁定文件的文件描述符
		 cmd  - 锁定命令(F_SETLK, F_SETLKW, F_GETLK)
		 type - 锁类型(F_RDLCK, F_WRLCK, F_UNLCK)  
 输  出:	
 返回值:	< 0 -- 失败  = 0 -- 成功
********************************************************************
*/
inline int UpdateEx_RcdLock(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock locker;
	
	locker.l_type   = type;
	locker.l_start  = offset;
	locker.l_whence = SEEK_SET;
	locker.l_len	  = len;

	return fcntl(fd, cmd, &locker);
}

/*
  名  称: fillBlankChars
  功  能: 往fd指向的文件中填写填充字符
  参  数:
      fd     - 待写文件的文件描述符
      offset - 相对于文件首字节的字节偏移量
      len    - 填充数量
  返回值:
    (无)
 */
inline static void UpdateExfillBlankChars(int fd, int offset, int len)
{
  int   iCnt, iFrag, i;
  uint8 buff[32];

  memset(buff, 0xff, 32);
  iCnt  = (int)(len / 32);
  iFrag = len % 32;
  
  updateex_writew_lock(fd, offset, len); //Lock
  
  lseek(fd, offset, SEEK_SET); /* 偏移量定位 */  
  if( iFrag) write(fd, buff, iFrag);
  for( i = 0; i < iCnt; i++)
    write(fd, buff, 32);

  updateex_un_lock(fd, offset, len); //Unlock
}

/*
  名  称: WriteUpdateEXData
  功  能: 将升级交互数据写入到ramdisk文件系统中的指定文件.
  引  用:
  约  束:
  参  数:
    id[in]  - ramxdata数据项ID
    buf[in] - 数据项内容缓冲区
  返回值:
      <= 0 : 失败
      > 0  : 写入数据项的字节长度
*/
int WriteUpdateEXData(uint16 id, const char *buf)
{
  char          filepath[PATH_MAX_LEN + 1];
  int           fd;
  int           delta, filesize, phyOff, len;

  if( !buf) return -UPDATEEX_ERR_PTR;
  if(id >= UPDATEEX_NUM) /* id值范围判断 */
    return -UPDATEEX_ERR_O_RANGE;

  sprintf(filepath, "%s", updateex_file);

  if( !UpdateExFileExists(filepath)) /* 自动生成ramxdata文件 */
	{
  	fd = open(filepath, O_CREAT|O_WRONLY|O_TRUNC|O_EXCL, 0660);
	close(fd);
	}
  if( (fd = open(filepath, O_WRONLY)) < 0)
    return -UPDATEEX_ERR_ACCESS;

  phyOff    = updateex_id_offset(id); /* Ram数据的逻辑偏移量(查表) */
  len   	= updateex_id_len(id);


  /*--- 填充缺省字符流 ---*/
  filesize = FGetUpdateExFileSize(fd);
  delta    = phyOff - filesize;
  if(delta > 0)
  {
      filesize = FGetUpdateExFileSize(fd);
      delta    = phyOff - filesize;
      if(delta > 0)
    	UpdateExfillBlankChars(fd, filesize, delta); /* 填写缺省字符 */
  }
  
  /* 数据项加锁 */
  if( updateex_writew_lock(fd, phyOff, len) < 0)
  {
    close(fd);
    return -UPDATEEX_ERR_LOCK;
  }

  /*--- 数据写入 ---*/
  lseek(fd, phyOff, SEEK_SET);
  if( write(fd, buf, len) < 0)
  {
    close(fd);
    return -UPDATEEX_ERR_IO;
  }
  fsync(fd);
  close(fd);

  return len;
}

/*
  名  称: ReadUpdateEXData
  功  能: 从ramdisk文件系统中读取指定的升级交互数据项内容
  引  用:
  约  束:
  参  数:
    id  - UpdateEX数据项ID
    buf - 数据项内容返回值存放缓冲区
  返回值:
    <= 0 - 失败
    > 0  - 成功读取的数据项字节长度
*/
int ReadUpdateEXData(uint16 id, char *buf)
{
  char   pdataPath[PATH_MAX_LEN + 1];
  int    fd, phyOff, itemLen;
  int    filesize;

  if(id >= UPDATEEX_NUM) /* id值范围判断 */
    return -UPDATEEX_ERR_O_RANGE;

  sprintf(pdataPath, "%s", updateex_file);

  phyOff    = updateex_id_offset(id); /* Ram数据的逻辑偏移量(查表) */
  itemLen   = updateex_id_len(id);

  if( (fd = open(pdataPath, O_RDONLY)) < 0)
    return -UPDATEEX_ERR_ACCESS;

  filesize  = FGetUpdateExFileSize(fd);

  if( (filesize < (phyOff + itemLen)) )
  {
    close(fd);
    return -UPDATEEX_ERR_FILEEXCEED;
  }
  
  /* 数据项加锁 */
  if( updateex_readw_lock(fd, phyOff, itemLen) < 0)
  {
    close(fd);
    return -UPDATEEX_ERR_LOCK;
  }

  lseek(fd, phyOff, SEEK_SET);
  if( read(fd, buf, itemLen) < 0)
  {
    close(fd);
    return -UPDATEEX_ERR_IO;
  }
  close(fd);
  
  return itemLen;
}

