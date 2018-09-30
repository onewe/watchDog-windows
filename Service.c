#include "Service.h"
#define LOG_TAG L"serviceInstall"
#include "Logger.h"

//运行服务
void RunService() {
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = ServiceName;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcher(ServiceTable);
}


//判断是否已经安装过服务
BOOL IsInstalled() {
	BOOL bResult = FALSE;
	SC_HANDLE hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm != NULL) {
		SC_HANDLE hService = OpenService(hScm, ServiceName, SERVICE_QUERY_CONFIG);
		if (hService != NULL) {
			bResult = TRUE;
			CloseServiceHandle(hService);
		}
		CloseServiceHandle(hScm);
	}
	return bResult;
}

//安装服务
BOOL InstallService() {
	log_i(_T("安装服务中...\n"));
	if (IsInstalled()) {
		log_i(_T("服务已安装...\n"));
		return FALSE;
	}

	//打开服务控制器
	SC_HANDLE hScm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (hScm == NULL) {
		log_e(_T("打开服务控制器失败...\n"));
		return FALSE;
	}
	TCHAR * path = GetFullPath();
	TCHAR * arg = _T(" -s");
	TCHAR * fullPath = _tcscat(path,arg);
	SC_HANDLE hService = CreateService(hScm, 
		ServiceName, 
		ServiceName,
		SERVICE_QUERY_STATUS,
		SERVICE_WIN32_OWN_PROCESS, 
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL, 
		fullPath,
		NULL, 
		NULL, 
		NULL, 
		NULL,
		NULL);
	if (hService == NULL){
		log_e(_T("服务创建失败...\n"));
		return FALSE;
	}
	//释放句柄
	CloseServiceHandle(hScm);
	CloseServiceHandle(hService);
	return TRUE;
}

void Lerror(const char * format) {
	int rst = GetLastError();
	if (rst) {
		log_e(format,rst);
	}
}