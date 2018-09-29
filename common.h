#ifndef Common_H
#define Common_H


#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

//共享内存名称
#define Memory_Name L"ZZWTEC_SHARED_MEMORY"
//服务名称
#define ServiceName L"watchDogService"
//看门狗配置文件
#define CONFIG_FILE_PATH  "/conf/config.json"
//日志路径
#define LOG_PATH  "/logs/"
//日志文件名称
#define LOG_FILE_PATH "log.log"
//错误日志路径
#define ERROR_LOG_PATH "C:\\watch_dog_error.log"
//错误日志文件对象
FILE * ERROR_LOG;
//日志文件对象
FILE * DOG_LOG;
#endif // !Common_H