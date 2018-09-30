#include "Logger.h"

//日志的最大长度
#define LOG_MAX_BUF_SIZE 1024
/*时间字符串最大64*/
#define TIME_BUFLEN 64
#define TIME_FORMAT L"%Y-%m-%d %H:%M:%S"
/*日志前面部分的格式 [时间]  日志级别  标签*/
#define LOG_FORMAT L"[%20s %s] %s/%s %s:%ld:   "

static TCHAR * leveInfo[] = { L"D",L"I",L"E" };
static TCHAR logBuf[LOG_MAX_BUF_SIZE];


//获取当前系统时间
TCHAR* GetTime() {
	static TCHAR timeStr[TIME_BUFLEN];
	time_t t = time(NULL);
	struct tm * local = localtime(&t);
	_tcsftime(timeStr, TIME_BUFLEN, TIME_FORMAT, local);
	return timeStr;
}
//获取进程ID
TCHAR *GetPID() {
	static TCHAR pidBuf[10];
	long pid = GetCurrentProcessId();
	_stprintf(pidBuf, L"PID:%04ld", GetCurrentProcessId());

	return pidBuf;
}



//打印日志
void PrintLog(int level, TCHAR * tag, TCHAR * func, long line, TCHAR * format, ...) {
	//时间
	TCHAR* timeStr = GetTime();
	//日志等级
	TCHAR* levelStr = leveInfo[level];
	TCHAR* pid = GetPID();
	int index = _stprintf(logBuf, LOG_FORMAT,timeStr, pid, levelStr,tag, func,line);
	va_list args;
	va_start(args,format);
	_vsntprintf(logBuf + index, LOG_MAX_BUF_SIZE - index, format,args);
	va_end(args);
	//打印日志
	_ftprintf(DOG_LOG,logBuf);

}