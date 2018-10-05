#include "watchDogService.h"
#define LOG_TAG L"watchDogService"
#include "Logger.h"



SERVICE_STATUS                      ServiceStatus;                              //服务状态
SERVICE_STATUS_HANDLE               hStatus;                                    //服务状态句柄

//----------------


PROCESS_INFORMATION					pi;											//子进程句柄
DWORD								returnCode;									//子进程返回码
STARTUPINFO							si = { sizeof(STARTUPINFO) };

//---------------
HANDLE								hToken;										//用户token
HANDLE								hTokenDup;									//用户token
LPVOID								pEnv;										//环境信息


//看门狗函数主体
void WINAPI ServiceMain(DWORD argc, PWSTR* argv) {

	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
	ServiceStatus.dwWin32ExitCode = NO_ERROR;
	ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	hStatus = RegisterServiceCtrlHandler(ServiceName, ServiceCtrlHandler);
	if (!hStatus)
	{
		DWORD dwError = GetLastError();
		log_e(_T("启动服务失败!%d\n"), dwError);
		return;
	}

	//设置服务状态
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);
	
	Run();

	//停止服务
	ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
	SetServiceStatus(hStatus, &ServiceStatus);
}


//服务回调
void WINAPI ServiceCtrlHandler(DWORD fdwControl)
{
	switch (fdwControl) {
	case SERVICE_CONTROL_STOP:
		log_i(_T("WatchDog 服务停止...\n"));
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode = 0;
		SetServiceStatus(hStatus, &ServiceStatus);
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		log_i(_T("WatchDog 服务终止...\n"));
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode = 0;
		SetServiceStatus(hStatus, &ServiceStatus);
		break;
	default:
		break;
	}
}

//运行指定程序
void Run() {
	log_i(_T("服务调用成功!\n"));
	DogFood * dogFood = CreateDogFood();
	log_i(_T("狗粮状态码:%d\n"),GetLastError());
	if (dogFood == NULL) {
		log_e(_T("狗粮生产失败!\n"));
	}
	else {
		wchar_t * commandLine = ParseConfForCmd();
		BOOL flag = createProcess(commandLine);
		if (flag) {
			watching(dogFood, commandLine);
		}
		else {
			log_e(_T("程序启动失败!\n"));
		}
		free(commandLine);
	}
	log_i(_T("停止服务中....\n"));
}

//解析配置文件
wchar_t * ParseConfForCmd() {
	TCHAR * path = GetFullDir();
	TCHAR * configFilePath = _tcscat(path, _T(CONFIG_FILE_PATH));
	//文件结构体
	FILE * config_file;
	//打开文件
	if ((config_file = _tfopen(configFilePath, _T("r"))) == NULL) {
		log_e(_T("配置文件打开失败!\n"));
		exit(EXIT_FAILURE);
	}

	//读取文件长度
	//定位文件偏移到末尾
	fseek(config_file, 0L, SEEK_END);
	//获取文件长度
	long total_size = ftell(config_file);
	if (total_size < 0) {
		log_e(_T("读取配置文件失败!配置文件长度为0\n"));
		exit(EXIT_FAILURE);
	}
	//分配内存
	char * json_data = malloc(sizeof(char) * total_size + 1);

	if (json_data == NULL) {
		log_e(_T("读取配置文件失败!分配内存失败\n"));
		exit(EXIT_FAILURE);
	}

	//设置文件指针到开头

	fseek(config_file, 0L, SEEK_SET);

	fread(json_data, sizeof(char), total_size, config_file);

	//关闭配置文件
	fclose(config_file);

	//解析json
	cJSON *json = cJSON_Parse(json_data);

	cJSON *cmd = cJSON_GetObjectItem(json, "cmd");

	if (cmd == NULL) {
		log_e(_T("无法启动程序,命令行无效!配置中无cmd参数\n"));
		exit(EXIT_FAILURE);
	}

	//转宽字符
	wchar_t * commandLine = CharToWchar(cmd->valuestring);
	if (commandLine == NULL) {
		log_e(_T("无法启动程序,命令行无效!转宽字节无效\n"));
		exit(EXIT_FAILURE);
	}

	//释放JSON字符串内存
	free(json_data);
	free(configFilePath);
	return commandLine;
}

BOOL CreateProcessNoService(const wchar_t * commandLine) {
	log_i(_T("创建进程中.....\n"));
	return  CreateProcess(NULL, commandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
}

//服务环境下创建进程
BOOL CreateProcessForService(const wchar_t * commandLine) {
	log_i(_T("创建进程中.....\n"));
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();

	//获取当前处于活动状态用户的Token
	if (!WTSQueryUserToken(dwSessionID, &hToken)) {
		int nCode = GetLastError();
		log_e(_T("获取用户token失败,错误码:%d\n"), nCode);
		CloseHandle(hToken);
		return FALSE;
	}

	//复制新的Token
	if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hTokenDup)) {
		int nCode = GetLastError();
		log_e(_T("复制用户token失败,错误码:%d\n"), nCode);

		CloseHandle(hToken);
		return FALSE;
	}

	//创建环境信息
	if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE)) {
		DWORD nCode = GetLastError();
		log_e(_T("创建环境信息失败,错误码:%d\n"), nCode);
		CloseHandle(hTokenDup);
		CloseHandle(hToken);
		return FALSE;
	}

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = _T("winsta0\\default");

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	//开始创建进程
	DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;


	if (!CreateProcessAsUser(hTokenDup, NULL, commandLine, NULL, NULL, FALSE, dwCreateFlag, pEnv, GetFullDir(), &si, &pi))
	{
		DWORD nCode = GetLastError();
		log_e(_T("创建进程失败,错误码:%d\n"), nCode);
		DestroyEnvironmentBlock(pEnv);
		CloseHandle(hTokenDup);
		CloseHandle(hToken);
		return FALSE;
	}
	//创建一个进程
	return TRUE;
}

//狗粮快递
DogFood * CreateDogFood() {
	log_i(_T("生成狗粮中!\n"));

	PSECURITY_DESCRIPTOR pSec = (PSECURITY_DESCRIPTOR)LocalAlloc(LMEM_FIXED, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if (!pSec)
	{
		return GetLastError();
	}
	if (!InitializeSecurityDescriptor(pSec, SECURITY_DESCRIPTOR_REVISION))
	{
		LocalFree(pSec);
		return GetLastError();
	}
	if (!SetSecurityDescriptorDacl(pSec, TRUE, NULL, TRUE))
	{
		LocalFree(pSec);
		return GetLastError();
	}
	SECURITY_ATTRIBUTES attr;
	attr.bInheritHandle = FALSE;
	attr.lpSecurityDescriptor = pSec;
	attr.nLength = sizeof(SECURITY_ATTRIBUTES);


	HANDLE hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		&attr,
		PAGE_READWRITE,
		0,
		sizeof(DogFood),
		Memory_Name
	);
	LocalFree(pSec);

	int rst = GetLastError();
	if (rst) {
		log_e(_T("内存申请失败!%d\n"), rst);
		return NULL;
	}
	//获取狗粮
	DogFood * dogFood = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DogFood));
	//初始化
	dogFood->status = -1;
	dogFood->timestamp = 1000L;
	return dogFood;
}

//看门
void watching(DogFood * dogFood, wchar_t * commandLine) {
	//重试次数
	int re_count = 0;
	//之前状态
	char old_status = 0;
	long old_timestamp = 0L;
	//初始化狗粮
	dogFood->status = 0;
	dogFood->timestamp = 0L;
	log_i(_T("开门狗程序初始化完毕,等待1分钟进入喂狗流程!\n"));
	//等待1分钟,开始进入喂狗流程
	Sleep(1000 * 60);

	while (TRUE) {

		log_i(_T("狗饿了!\n"));
		//如果狗粮时间戳为0 并且 停止喂狗次数小于10,说明程序初始化失败了,尝试10次,每次等待时间延长一分钟
		if (dogFood->timestamp == 0L && re_count < 10) {
			re_count++;
			log_e(_T("程序初始化失败,第%d次重试!等待%d分钟...\n"), re_count, re_count + 1);
			TerminateProcess(pi.hProcess, 0);
			createProcess(commandLine);
			Sleep(1000 * 60 * (re_count + 1));
		}
		else if (dogFood->timestamp == 0L && re_count > 10) {
			//程序任然在初始化的过程中,但已经重试10次了,此时程序可以认为是无法启动
			log_e(_T("程序无法启动!\n"));
			TerminateProcess(pi.hProcess, 0);
			break;
		}
		else if (dogFood->timestamp == old_timestamp && dogFood->status == 1) {
			//程序在运行中,但是没有喂狗,重启程序
			log_e(_T("长时间没有喂狗,程序重启!\n"));
			re_count = 0;
			old_status = 0;
			old_timestamp = 0L;
			dogFood->status = 0;
			dogFood->timestamp = 0L;
			TerminateProcess(pi.hProcess, 0);
			int errorCode = GetLastError();
			if (errorCode) {
				log_e(_T("结束进程失败,%d\n"),errorCode);
			}
			Sleep(1000 * 10);
			createProcess(commandLine);
			Sleep(1000 * 60);

		}
		else {
			log_i(_T("喂狗成功!状态码:%d    时间戳:%ld\n"),dogFood->status,dogFood->timestamp);
			old_status = dogFood->status;
			old_timestamp = dogFood->timestamp;
			Sleep(1000 * 60);
		}

	}
}