#ifndef CUTILS_H
#define CUTILS_H
#include "common.h"

//获取本程序完整路径
TCHAR *  GetFullPath();
//获取本程序工作目录
TCHAR *  GetFullDir();
//初始化日志
BOOL InitLog();
//窄字符转宽字符
wchar_t * CharToWchar(char *);
//宽字符转窄字符
char * WCharToChar(wchar_t *);

#endif // !CUTILS_h