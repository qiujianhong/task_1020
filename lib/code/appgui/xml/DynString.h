/************************************
  REVISION LOG ENTRY
  Revision By: 赵晓明
  Revised on 2002-4-25 10:24:48
  Comments: 初始编写
 ************************************/

// DynString.h: interface for the CDynString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DYNSTRING_H__E773636E_B04C_4918_B00A_7E0DD7F8EAAD__INCLUDED_)
#define AFX_DYNSTRING_H__E773636E_B04C_4918_B00A_7E0DD7F8EAAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "miniguidef.h"
#include "miniguidef2.h"

#include <stdarg.h>

///与CString类似的字串类，功能较单一，但用于持续扩张的场合效率比CString高得多
class CDynString 
{
public:
	///清空字串
	void Empty();
	
	///判断是否为空
	BOOL IsEmpty();

	///取长度
	DWORD GetLength();
	
	///设定长度并取缓冲区指针
	char* GetBufferSetLength(int nLen);

	///在调用GetBufferSetLength并写入字串后，刷新字符串长度
	void RecalcStrLen();

	///格式化字串
	void Format(const char* strFormat,...);

	///缺省构造函数
	CDynString();
	
	///拷贝构造函数
	CDynString(const char* str);
	
	///拷贝构造函数
	CDynString(const CDynString& str);

	///析构函数
	virtual ~CDynString();
	
	///赋值操作运算符
	const CDynString& operator =(const char *str);

	///赋值运算符2
	const CDynString& operator =(const CDynString& str);
	
	///累加操作运算符
	const CDynString& operator +=(const char *str);
	
	///加法(连接)操作运算符
//	const CDynString operator +(const char* str);

	///类型转换运算符
	operator const char* () const;

public:
	///加法(连接)操作运算符
	friend CDynString& operator +(CDynString &dynStr, const char* str)
	{
		dynStr+=str;
		return dynStr;
	}

private:
	int m_nStrLen;
	BOOL malloc();
	BOOL AllocBuffer(int minLen);
	int m_nBufferLen;
	char* m_pBuffer;
};

/// Compare helpers
BOOLEAN  operator==(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator==(const CDynString& s1, const char* s2);
BOOLEAN  operator==(const char* s1, const CDynString& s2);
BOOLEAN  operator!=(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator!=(const CDynString& s1, const char* s2);
BOOLEAN  operator!=(const char* s1, const CDynString& s2);
BOOLEAN  operator<(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator<(const CDynString& s1, const char* s2);
BOOLEAN  operator<(const char* s1, const CDynString& s2);
BOOLEAN  operator>(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator>(const CDynString& s1, const char* s2);
BOOLEAN  operator>(const char* s1, const CDynString& s2);
BOOLEAN  operator<=(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator<=(const CDynString& s1, const char* s2);
BOOLEAN  operator<=(const char* s1, const CDynString& s2);
BOOLEAN  operator>=(const CDynString& s1, const CDynString& s2);
BOOLEAN  operator>=(const CDynString& s1, const char* s2);
BOOLEAN  operator>=(const char* s1, const CDynString& s2);

#endif // !defined(AFX_DYNSTRING_H__E773636E_B04C_4918_B00A_7E0DD7F8EAAD__INCLUDED_)
