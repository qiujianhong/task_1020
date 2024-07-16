//#include "main.h"
#include "miniguidef.h"
#include "miniguidef2.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <unistd.h>

//获取系统版本号
#if GUI_SYSVERSION_EN == 1
BOOL commfun_GetSysVersion(char *pBuf)
{
	FILE *fp;
	char *p;
	char *q;
	int nNum,i = 0;
	char version[VERSION_LEN];
	
	memset(version, 0 ,VERSION_LEN);
	if ((fp = fopen("/proc/version","r"))== NULL)
	{
		perror("fail to fopen!");
		return FALSE;
	}
	while((nNum = fgetc(fp)) != EOF)
	{
		version[i++] = (char)nNum;
	}
	fclose(fp);
	if ((p = strstr(version,"+++")) == NULL)
	{
		perror("no the string!");
		return FALSE;
	}
	p = p + 6;
	
	if ((q = strstr(p,"+++")) == NULL)
	{
		perror("no the string!");
		return FALSE;
	}
	
	if((q-p) > 0 && (q-p) <= OS_VERSION_LEN)
	{
		memcpy(pBuf,p,(q-p));
		memset(pBuf+(q-p), 0 ,1);
	}
	else
	{
	memcpy(pBuf,p,OS_VERSION_LEN);
	memset(pBuf+OS_VERSION_LEN, 0 ,1);
	}
	return TRUE;
}
#endif

BOOL GetTime(char* pczTime, int* nHour, int* nMin)
{
	if(pczTime == NULL || strlen(pczTime) == 0)
    {
        return FALSE;
    }
	char* p1[2];
	char czBuf[20];
	strcpy(czBuf, pczTime);
	
	char* pTmp = p1[0] = czBuf;
	int nIndex = 1;
	while(*pTmp != '\0')
	{
		if(*pTmp == ':')
		{
			*pTmp = '\0';
			pTmp ++;
			if(*pTmp == '\0')
			{
				break;
			}
			else
			{
				if(*pTmp > '9' || *pTmp < '0')
				{
					return FALSE;
				}
				if(nIndex > 2)
				{
					return FALSE;
				}
				p1[nIndex] = pTmp;
				nIndex ++;
			}
		}
		else if(*pTmp > '9' || *pTmp < '0')
		{
			return FALSE;
		}
		pTmp++;
	}
	if(nIndex != 2)
	{
		return FALSE;
	}
	*nHour = atoi(p1[0]);
	*nMin = atoi(p1[1]);
	if(*nHour > 12 || *nHour < 0
		|| *nMin > 59 || *nMin < 0)
	{
		return FALSE;
	}
	return TRUE;
}
/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    验证字符串是否为有效的number型数据
	输入参数：    char* pNum：验证字符串
				  int nMax：数据的最大值，-1则不验证最大值
	输出参数：	  TRUE：验证通过
	函数返回值：  
	内部处理流程：
********************************************************************/
BOOL CheckNumber(char* pNum, int nMax)
{
    if(pNum == NULL || strlen(pNum) == 0)
    {
        return FALSE;
    }
    char* pTmp = pNum;
    while(*pTmp != '\0')
    {
        if(*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp ++;
    }
    if(nMax != -1)
    {
        if(nMax<atoi(pNum))
        {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CheckAPN(char* pNum)
{
    if(pNum == NULL || strlen(pNum) == 0)
    {
        return FALSE;
    }
    return TRUE;
}

/*********************************************************************
	所属功能单元：显示框架模块
	功能描述：    验证字符串是否为有效的float型数据
	输入参数：    char* pNum：验证字符串
				  int nMax：数据的最大值，-1则不验证最大值
	输出参数：	  TRUE：验证通过
	函数返回值：  
	内部处理流程：
********************************************************************/
BOOL CheckFloat(char* pNum, int nMax)
{
    if(pNum == NULL || strlen(pNum) == 0)
    {
        return FALSE;
    }
    int nPoint = 0;
    char* pTmp = pNum;
    while(*pTmp != '\0')
    {
        if(*pTmp == '.')
        {
            nPoint ++;
            if(nPoint > 1)
            {
                return FALSE;
            }
        }
        else if(*pTmp > '9' || *pTmp < '0')
        {
            return FALSE;
        }
        pTmp ++;
    }
    if(nMax != -1)
    {
        if((double)nMax<atof(pNum))
        {
            return FALSE;
        }
    }
    return TRUE;
}


// bcd码转换为int8类型 2位bcd转换为1字节bin
uint8 bcd2int8(char val)
{
    return((((val & 0xf0) >> 4) * 10) + (val & 0xf));
}
// int8类型转换为bcd码 1字节bin转换为2位bcd
char int82bcd(uint8 val)
{
    return(((val / 10) << 4) + (val % 10));
}

// bcd码转换为short类型 4位bcd转换为2字节
uint16 bcd2short(char* val)
{
	return ((val[0]&0xf0)>>4)*1000 + (val[0]&0x0f) * 100
		+ ((val[1]&0xf0)>>4) *10 + (val[1]&0x0f);
}
// short类型转换为bcd码 2字节bin转换为4位bcd
void short2bcd(uint16 val, char* pbcd)
{
	short tmp = val/100;
	val = val%100;

	pbcd[0] = ((tmp/10)<<4) + (tmp%10);
	pbcd[1] = ((val/10)<<4) + (val%10);
	pbcd[2] = '\0';
}

// 16进制ascii码转换为short
short hexasc2short(char* val)
{
    ASSERT(val != NULL);
    int nLen = strlen(val);
    ASSERT(nLen <= 4);
    short shortval = 0;
    int nIndex = 0;
    char c;
    for(int i = nLen-1; i >= 0; i --)
    {
        c = val[i]; 
        if(c >= '0' && c <= '9')
        {
            c -= '0';
        }
        else if(c >= 'a' && c <= 'f')
        {
            c -= 'a';
            c += 10;
        }
        else if(c>= 'A' && c <= 'F')
        {
            c -= 'A';
            c += 10;
        }
        else
        {
            ASSERT(FALSE);
        }
        
        shortval |= (c<<nIndex);
        nIndex += 4;
    }
    return shortval;    
}
// short转换为16进制ascii
void short2hexasc(char* val, char* pasc)
{
    sprintf(pasc, "%04X", *(unsigned short*)val);
}

// 16进制ascii码转换为int
int hexasc2int(char* val)
{
    ASSERT(val != NULL);
    int nLen = strlen(val);
    ASSERT(nLen <= 8);
    int intval = 0;
    int nIndex = 0;
    char c;
    for(int i = nLen-1; i >= 0; i --)
    {
        c = val[i]; 
        if(c >= '0' && c <= '9')
        {
            c -= '0';
        }
        else if(c >= 'a' && c <= 'f')
        {
            c -= 'a';
            c += 10;
        }
        else if(c>= 'A' && c <= 'F')
        {
            c -= 'A';
            c += 10;
        }
        else
        {
            ASSERT(FALSE);
        }
        
        intval |= (c<<nIndex);
        nIndex += 4;
    }
    
    return intval;  
}

int asc2int(char* val)
{
    ASSERT(val != NULL);
    int nLen = strlen(val);
    ASSERT(nLen <= 8);
    int intval = 0;
    //int nIndex = 0;
    char c;
    for(int i = 0; i<=nLen-1; i++)
    {
        c = val[i]; 
        if(c >= '0' && c <= '9')
        {
            c -= '0';
        }
        else if(c >= 'a' && c <= 'f')
        {
            c -= 'a';
            c += 10;
        }
        else if(c>= 'A' && c <= 'F')
        {
            c -= 'A';
            c += 10;
        }
        else
        {
            ASSERT(FALSE);
        }
        
        intval = intval*10 + c;
       
    }
    
    return intval;  
}

// int转换为16进制ascii
void int2hexasc(char* val, char* pasc)
{
    sprintf(pasc, "%08X", *(unsigned int*)val);
}

short chartoint16(char* pBuf)
{
	short sVal;
	memcpy(&sVal, pBuf, 2);
	return sVal;
}
int chartoint32(char* pBuf)
{
	int nVal;
	memcpy(&nVal, pBuf, 4);
	return nVal;
}
int64 chartoint64(char* pBuf)
{
	int64 nVal;
	memcpy(&nVal, pBuf, 8);
	return nVal;
}
void int16tochar(short nVal, char* pchar)
{
	memcpy(pchar, (char*)&nVal, 2);
}
void int32tochar(int nVal, char* pchar)
{
	memcpy(pchar, (char*)&nVal, 4);
}
void int64tochar(int64 nVal, char* pchar)
{
	memcpy(pchar, (char*)&nVal, 8);
}

char* GetMettypeStr(uint8 nMettypeID)
{
    uint32 i;
	static char czBuf[20];

	sprintf(czBuf, "未知表");

    for(i = 0; i < g_nMettypeCnt; i ++)
    {
        if(g_MettypeContrast[i].nMettypeID == nMettypeID)
        {
            return g_MettypeContrast[i].czMettype;
        }
    }
    return czBuf;
}
int GetMettypeIndex(uint8 nMettypeID)
{
    uint32 i;
    for(i = 0; i < g_nMettypeCnt; i ++)
    {
        if(g_MettypeContrast[i].nMettypeID == nMettypeID)
        {
            return i;
        }
    }
    return -1;
}

double GetKeepBaseConstant(void) // 通用函数GetKeepBaseConstant():从数据字典读取保安定值
{
	int nRet;
	double dKeepBase;
	char czData[50];

	//nRet = ReadXPara(TMN_FUNC_PARA, Keep_Base_Constant, czData, 0);
	nRet = -1;
	if(nRet < 0
	    || (uint64)chartoint64(czData) == NONE_LLWORD)
	{
		dKeepBase = (double)DEFAULT_PW_PARA*0.0001;
	}
	else
	{
		dKeepBase = (double)chartoint64(czData)*0.0001;
	}
	return (dKeepBase);
}



/*******************************************************************************
* 函数名称: IsBuffAllPointValue
* 函数功能: 是否整个buff都是某个value值
* 输入参数: uint8 *Buff
uint8 Value
int n     长度
* 输出参数: 无
* 返 回 值: 1 是
0 否
*******************************************************************************/
int IsBuffAllPointValue(uint8 *Buff, uint8 Value, int n)
{
    int i = 0;
    int sum = 0;
    for (i = 0; i < n; i++)
    if (Buff[i] == Value)
        sum++;

    if (sum == n)
        return 1;

    return 0;
}
/*******************************************************************************
* 函数名称: IsBuffAll00_EE_FF
* 函数功能: 是否整个buff都是00或者ee或者ff
* 输入参数: uint8 *Buff
int len     长度
* 输出参数: 无
* 返 回 值: 1 是
0 否
*******************************************************************************/
int IsBuffAll00_EE_FF(uint8 *Buff, int Len)
{
    if (IsBuffAllPointValue(Buff, 0x00, Len) == 1)
        return 1;

    if (IsBuffAllPointValue(Buff, 0xee, Len) == 1)
        return 1;

    if (IsBuffAllPointValue(Buff, 0xff, Len) == 1)
        return 1;

    return 0;
}

// 是否处于硬件防护,防护按键按下返回TRUE，否则返回FALSE
extern BOOL g_bHardWareSafety;
BOOL HardwareSafetyKeyIsPressed(void)
{
	return TRUE;
}

void SpeakCtrl(BOOL bOpen)
{
    char czBuf[50];
    if(bOpen == TRUE)
    {
        czBuf[0]=1;	//-通话标志-
        czBuf[1]=0x58;
        czBuf[2]=0x02;	//-默认600second,bin,单位:s- 	   
        //WriteTmnXData(RT_DATA,TONE_COM_STATU,czBuf);//-终端状态字-
        //ReadTmnXData(RT_DATA,CHANNEL_STATUS,czBuf);
        czBuf[0] &=0xf3;
        czBuf[0]|=0x04;
        //WriteTmnXData(RT_DATA,CHANNEL_STATUS,czBuf);

        czBuf[0] = 0;
        //WriteRamXData(Radio_Speak_Set, czBuf);
    }
    else
    {
        czBuf[0]=0;//-允许通话标志，0禁止，1允许-
        czBuf[1]=0;
        czBuf[2]=0x0;	//-默认0second- 	   
        //WriteTmnXData(RT_DATA,TONE_COM_STATU,czBuf);//-终端状态字-
        //ReadTmnXData(RT_DATA,CHANNEL_STATUS,czBuf);//--
        czBuf[0] &= 0xf3;
        czBuf[0] |= 0x08;
        //WriteTmnXData(RT_DATA,CHANNEL_STATUS,czBuf);

        czBuf[0] = 1;
        //WriteRamXData(Radio_Speak_Set, czBuf);
    }
}

BOOLEAN BufIsInitialized(void *buf, unsigned int len)//(本函数的用法简介请查看头文件miniguidef.h中对应的函数声明)
{
	for (unsigned int i=0; i<len; i++)
	{
		if (((char*)buf)[i] != 0xFF)
			return (true);
	}
	return (false);
}

