/************************************
  REVISION LOG ENTRY
  Revision By: 赵晓明
  Revised on 2002-4-25 10:24:48
  Comments: 初始编写
 ************************************/

// DynString.cpp: implementation of the CDynString class.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "DynString.h"

CDynString::CDynString()
{
	m_pBuffer=NULL;
	m_nStrLen=0;
	m_nBufferLen=0;
}

///拷贝构造函数
CDynString::CDynString(const char* str)
{
	m_pBuffer=NULL;
	m_nStrLen=0;
	m_nBufferLen=0;
	operator =(str);
}

///拷贝构造函数
CDynString::CDynString(const CDynString& str)
{
	m_pBuffer=NULL;
	m_nStrLen=0;
	m_nBufferLen=0;
	operator =(str);
}

CDynString::~CDynString()
{
	Empty();
}

void CDynString::Empty()
{
	if (m_pBuffer)
		delete[] m_pBuffer;
	m_pBuffer=NULL;
	m_nStrLen=0;
	m_nBufferLen=0;
}

///判断是否为空
BOOL CDynString::IsEmpty()
{
	return !m_pBuffer || !m_pBuffer[0];
}

///取长度
DWORD CDynString::GetLength()
{
	return m_nStrLen;
}

///设定长度并取缓冲区指针
char* CDynString::GetBufferSetLength(int nLen)
{
	Empty();
	AllocBuffer(nLen+1);
	return m_pBuffer;
}

///在调用GetBufferSetLength并写入字串后，刷新字符串长度
void CDynString::RecalcStrLen()
{
	m_nStrLen=m_pBuffer?strlen(m_pBuffer):0;
}

///格式化字串
void CDynString::Format(const char* strFormat,...)
{
	char tmpBuffer[10240];
    va_list arglist;
    va_start(arglist, strFormat);
	vsnprintf(tmpBuffer,sizeof(tmpBuffer),strFormat,arglist);
	operator =(tmpBuffer);
}

const CDynString& CDynString::operator =(const char *str)
{
	if (!str || strlen(str)==0 )
		Empty();
	else
	{
		int len=strlen(str);
		char *tmp=new char[len+1];
		if( tmp!=NULL )
		{
			strcpy(tmp,str);
			Empty();
			AllocBuffer(len+1);
			strcpy(m_pBuffer,tmp);
			m_nStrLen=len;
			delete tmp;
		}
	}
	return *this;
}

///赋值运算符2
const CDynString& CDynString::operator =(const CDynString& str)
{
	return operator =(str.m_pBuffer?str.m_pBuffer:"");
}

const CDynString& CDynString::operator+=(const char *str)
{
	if (!m_pBuffer)
		return operator =(str);
	int addLen=strlen(str);
	int nLen=m_nBufferLen;
	while (m_nStrLen+addLen+1 >m_nBufferLen)
	{
		m_nBufferLen*=2;
	}

	if( malloc()==FALSE )
	{
		m_nBufferLen=nLen;
		return *this;
	}
	
	strcat(m_pBuffer,str);
	m_nStrLen+=addLen;
	return *this;
}

///加法(连接)操作运算符
//const CDynString CDynString::operator +(const char* str)
//{
//	CDynString s=*this;
//	s+=str;
//	return s;

//	*this+=str;
//	return *this;
//}

CDynString::operator const char* () const 
{
	static const char* emptyString="";
	if (!m_pBuffer)
	{
		return emptyString;
	}
	return (const char*) m_pBuffer;
}

BOOL CDynString::AllocBuffer(int minLen)
{
	int i=0;
	while(minLen>>=1)
		i++;
	int len=1;
	len<<=(i+1);
	if (len<64)
		len=64;

	m_nBufferLen=len;
	return malloc();
}
	
BOOL CDynString::malloc()
{
	char* newBuffer=new char[m_nBufferLen];
	if (!newBuffer)
	{
		m_nBufferLen=0;
		m_nStrLen=0;
		return FALSE;
	}
	if ( m_pBuffer )
	{
		strcpy(newBuffer,m_pBuffer);
		delete[] m_pBuffer;
	}

	m_pBuffer=newBuffer;
	return TRUE;
}

/// Compare helpers
BOOLEAN operator==( const CDynString& s1, const CDynString& s2)
{
	return !strcmp((const char*)s1,(const char*)s2);
}

BOOLEAN operator==(const CDynString& s1, const char* s2)
{
	return !strcmp((const char*)s1,s2);
}

BOOLEAN operator==(const char* s1, const CDynString& s2)
{
	return !strcmp(s1,(const char*)s2);
}

BOOLEAN operator!=(const CDynString& s1, const CDynString& s2)
{
	return strcmp((const char*)s1,(const char*)s2)!=0;
}

BOOLEAN operator!=(const CDynString& s1, const char* s2)
{
	return strcmp((const char*)s1,s2)!=0;
}

BOOLEAN operator!=(const char* s1, const CDynString& s2)
{
	return strcmp(s1,(const char*)s2)!=0;
}

BOOLEAN operator<(const CDynString& s1, const CDynString& s2)
{
	return strcmp((const char*)s1,(const char*)s2)<0;
}

BOOLEAN operator<(const CDynString& s1, const char* s2)
{
	return strcmp((const char*)s1,s2)<0;
}

BOOLEAN operator<(const char* s1, const CDynString& s2)
{
	return strcmp(s1,(const char*)s2)<0;
}

BOOLEAN operator>(const CDynString& s1, const CDynString& s2)
{
	return strcmp((const char*)s1,(const char*)s2)>0;
}

BOOLEAN operator>(const CDynString& s1, const char* s2)
{
	return strcmp((const char*)s1,s2)>0;
}

BOOLEAN operator>(const char* s1, const CDynString& s2)
{
	return strcmp(s1,(const char*)s2)>0;
}

BOOLEAN operator<=(const CDynString& s1, const CDynString& s2)
{
	return strcmp((const char*)s1,(const char*)s2)<=0;
}

BOOLEAN operator<=(const CDynString& s1, const char* s2)
{
	return strcmp((const char*)s1,s2)<=0;
}

BOOLEAN operator<=(const char* s1, const CDynString& s2)
{
	return strcmp(s1,(const char*)s2)<=0;
}

BOOLEAN operator>=(const CDynString& s1, const CDynString& s2)
{
	return strcmp((const char*)s1,(const char*)s2)>=0;
}

BOOLEAN operator>=(const CDynString& s1, const char* s2)
{
	return strcmp((const char*)s1,s2)>=0;
}

BOOLEAN operator>=(const char* s1, const CDynString& s2)
{
	return strcmp(s1,(const char*)s2)>=0;
}

