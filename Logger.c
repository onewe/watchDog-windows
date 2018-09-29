#include "Logger.h"

//获取当前系统时间
TCHAR GetTime() {
	time_t t = time(0);
	//时间字符串
	static TCHAR timeStr[TIME_BUFLEN];
	_tcsftime(timeStr, TIME_BUFLEN, "%Y-%m-%d %H:%M:%S", localtime(&t));
	return timeStr;
}

//打印日志
void PrintLog(int level, TCHAR * tag, TCHAR * format, ...) {
	
	TCHAR timeStr = GetTime();
	_ftprintf(DOG_LOG,_T(""));
}