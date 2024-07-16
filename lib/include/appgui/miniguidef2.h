#ifndef MINIGUIDEF2_H
#define MINIGUIDEF2_H


/*********************************************************************
    所属功能单元：显示框架模块
    功能描述：    弹出一个输入字符串页面
    输入参数：    char* pDefault：缺省字符串，输入当前的字符串
                  fnRetureInput pfnReturn：返回字符串信息的回调函数
                  void* pPara：回调函数的参数
                  int nType：类型掩码，用以指定可选的字符串类型
    输出参数：
    函数返回值：  
    内部处理流程：
********************************************************************/
typedef void (*fnRetureInput)(char* pNewStr, void* pPara);
void InputBox(char* pDefault = NULL,
              fnRetureInput pfnReturn = NULL, void* pPara = NULL,
              int nType = 0x01|0x02|0x04|0x08|0x10,
              int nCurType = 0x01);

void KeepNextTrunDisp(BOOL bPre = FALSE);
BOOL CheckNumber(char* pNum, int nMax=-1);
BOOL CheckFloat(char* pNum, int nMax=-1);
BOOL MessageBox(const char* pczMsg, int nType = MB_OK);

BOOL HardwareSafetyKeyIsPressed(void);
inline BOOL IsHardWareSafety(void)  {return (HardwareSafetyKeyIsPressed());}


#endif
