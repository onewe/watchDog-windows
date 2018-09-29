#include "common.h"

#define TIME_BUFLEN 64

#ifndef  LOG_TAG 
#define LOG_TAG L"main"
#endif // ! LOG_TAG 

#ifdef LOGGER_H
#define LOGGER_H


#include <time.h>
//时间字符串 最大长度

enum Level{DEBUG,INFO,ERROR};
char * leveInfo[] = {"DEBUG","INFO","ERROR"};

#define log_d(...) PrintLog(DEBUG,LOG_TAG,__VA_ARGS__) 
#define log_i(...) PrintLog(INFO,LOG_TAG,__VA_ARGS__)
#define log_e(...) PrintLog(ERROR,LOG_TAG,__VA_ARGS__)
#endif // LOGGER_H


//打印日志
void PrintLog(int level,TCHAR * tag,TCHAR * format,...);