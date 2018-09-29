#include "watchDogService.h"
//初始化服务
void RunService();
//安装服务
BOOL InstallService();
//判断是否已经安装过服务
BOOL IsInstalled();
//打印错误日志
void Lerror(const char * format);