#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <elog.h>

#ifndef Common_H
#define Common_H
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
#endif // !Common_H
