#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" )
#include <locale.h>
#include "Service.h"
#include "watchDogService.h"
#include "CUtils.h"

extern BOOL(*createProcess)(const wchar_t *);


int wmain(int argc, wchar_t *argv[]) {
	setlocale(LC_ALL, "chs");
	ERROR_LOG = _tfopen(_T(ERROR_LOG_PATH),_T("w+"));
	//没有权限,退出程序
	if (ERROR_LOG == NULL) {
		_ftprintf(ERROR_LOG, _T("日志文件打开失败,程序退出..."));
		exit(EXIT_FAILURE);
	}
	_ftprintf(ERROR_LOG, _T("日志文件打开失败,程序退出..."));
	TCHAR * currentDir = GetFullDir();
	TCHAR * logPath = _tcscat(currentDir, _T(LOG_PATH));
	if (_taccess(logPath, 0) != 0) {
		_ftprintf(ERROR_LOG, _T("日志目录不存在:%s,程序退出\n"),logPath);
		exit(EXIT_FAILURE);
	}
	TCHAR * logFilePath = _tcscat(currentDir, _T(LOG_FILE_PATH));
	DOG_LOG = _tfopen(logFilePath, _T("w+"), stdout);
	if (DOG_LOG == NULL) {
		_ftprintf(ERROR_LOG, _T("日志文件无权限访问:%s,程序退出..\n"),logFilePath);
		exit(EXIT_FAILURE);
	}


	BOOL falg = InitLog();
	if (!falg) {
		_ftprintf(ERROR_LOG,_T("日志初始化失败!程序退出..\n"));
		exit(EXIT_FAILURE);
	}

	//-s 代表服务已经安装只需要启动服务即可
	if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/')))) {
		if (_wcsicmp(L"s", argv[1] + 1) == 0) {
			createProcess = CreateProcessForService;
			//启动服务
			RunService();
		}
	}
	else {
		//如果没有参数或者不是-s,表示服务未安装,先安装服务.
		BOOL result = InstallService();
		if (result) {
			//服务安装成功
			log_i("服务安装成功,启动服务中..");
			createProcess = CreateProcessNoService;
			Run();
		}
		else {
			log_e("服务安装失败,程序退出..\n");
		}
	}
	
	free(logFilePath);
	return 0;

}