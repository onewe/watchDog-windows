#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" )
#include <stdio.h>
#include "Service.h"
#include "watchDogService.h"
#include "CUtils.h"

int wmain(int argc, wchar_t *argv[]) {
	extern BOOL(*createProcess)(const wchar_t *);

	FILE * error_log_file = _tfreopen(_T("C:\\watch_dog_error.log"), _T("w+"), stderr);
	BOOL falg = InitLog();
	if (!falg) {
		fprintf(stderr,"日志初始化失败!程序退出..\n");
		fclose(error_log_file);
		return -2;
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
			fclose(error_log_file);
		}
	}
	fclose(error_log_file);
	return 0;

}