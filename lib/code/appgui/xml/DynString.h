/************************************
  REVISION LOG ENTRY
  Revision By: ������
  Revised on 2002-4-25 10:24:48
  Comments: ��ʼ��д
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

///��CString���Ƶ��ִ��࣬���ܽϵ�һ�������ڳ������ŵĳ���Ч�ʱ�CString�ߵö�
class CDynString 
{
public:
	///����ִ�
	void Empty();
	
	///�ж��Ƿ�Ϊ��
	BOOL IsEmpty();

	///ȡ����
	DWORD GetLength();
	
	///�趨���Ȳ�ȡ������ָ��
	char* GetBufferSetLength(int nLen);

	///�ڵ���GetBufferSetLength��д���ִ���ˢ���ַ�������
	void RecalcStrLen();

	///��ʽ���ִ�
	void Format(const char* strFormat,...);

	///ȱʡ���캯��
	CDynString();
	
	///�������캯��
	CDynString(const char* str);
	
	///�������캯��
	CDynString(const CDynString& str);

	///��������
	virtual ~CDynString();
	
	///��ֵ���������
	const CDynString& operator =(const char *str);

	///��ֵ�����2
	const CDynString& operator =(const CDynString& str);
	
	///�ۼӲ��������
	const CDynString& operator +=(const char *str);
	
	///�ӷ�(����)���������
//	const CDynString operator +(const char* str);

	///����ת�������
	operator const char* () const;

public:
	///�ӷ�(����)���������
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
